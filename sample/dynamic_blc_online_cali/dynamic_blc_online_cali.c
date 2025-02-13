/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <unistd.h>
#ifndef __LITEOS__
#include <poll.h>
#endif
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sample_comm.h"
#include "hi_mpi_ae.h"
#include "hi_sns_ctrl.h"

#define CALI_ISO_NUM 3
#define HI_ISP_DYNAMIC_BLC_CALI_MAX_FRAME         30
#define HI_ISP_DYNAMIC_BLC_CALI_MAX_ISO_NUM       16

#define DUMP_RAW_AND_SAVE_DYNAMIC_BLC 0
#define MAX_FRM_CNT 42
#define MAX_FRM_WIDTH 8192
#define PIPE_4 4
#define DYNABLC_ISO_NUM_CALI 16
#define max_2(x, y)     ((x) > (y) ? (x) : (y))

static volatile sig_atomic_t g_sig_flag = 0;

#define VB_LINEAR_RAW_CNT   40
#define VB_YUV_ROUTE_CNT    8

static sample_vo_cfg g_dynablc_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_intf_type      = HI_VO_INTF_BT1120,
    .intf_sync         = HI_VO_OUT_1080P60,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, 1920, 1080},
    .image_size        = {1920, 1080},
    .vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len       = 3, /* 3: def buf len for single */
    .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
    .vo_mode           = VO_MODE_1MUX,
    .compress_mode     = HI_COMPRESS_MODE_NONE,
};

static sample_comm_venc_chn_param g_dynablc_venc_chn_param = {
    .frame_rate           = 30, /* 30 is a number */
    .stats_time           = 2,  /* 1 is a number */
    .gop                  = 60, /* 30 is a number */
    .venc_size            = {1920, 1080},
    .size                 = -1,
    .profile              = 0,
    .is_rcn_ref_share_buf = HI_FALSE,
    .gop_attr             = {
        .gop_mode = HI_VENC_GOP_MODE_NORMAL_P,
        .normal_p = {2},
    },
    .type                 = HI_PT_H265,
    .rc_mode              = SAMPLE_RC_CBR,
};

typedef enum {
    ISP_SENSOR_8BIT  = 8,
    ISP_SENSOR_10BIT = 10,
    ISP_SENSOR_12BIT = 12,
    ISP_SENSOR_14BIT = 14,
    ISP_SENSOR_16BIT = 16,
    ISP_SENSOR_32BIT = 32,
    ISP_SENSOR_BUTT
} isp_sensor_bit_width;

typedef struct {
    hi_u64 op_type;
    hi_u64 exp_time_op_type;
    hi_u64 a_gain_op_type;
    hi_u64 d_gain_op_type;
    hi_u64 ispd_gain_op_type;
    hi_u64 exp_time;
    hi_u64 a_gain;
    hi_u64 d_gain;
    hi_u64 isp_d_gain;
}ae_attr;

typedef struct {
    hi_u64 ob_blc;
    hi_u64 light_blc;
    hi_u64 offset;
    hi_u64 offset_final;
    hi_u64 iso;
    hi_u64 frame_cnt;
    hi_u64 bayer_format;
    hi_u32 iso_num;
    hi_u32 y;
    hi_u16 bit_width;
    hi_u16 pre_bit_width;
    hi_u64 light_blc_sum;
    hi_u64 light_pix_cnt;
    ae_attr ae_para;
    hi_rect light_area;
}blc_res;

static hi_u64 dynablc_iso_real_cali[DYNABLC_ISO_NUM_CALI] = {
    100, 200, 399, 796, 1588, 3169, 5701, 11403, 22752, 45398, 90579, 180736, 360620, 819200, 1638400, 3276800
};

static const hi_u64 dynablc_iso_samcali[DYNABLC_ISO_NUM_CALI] = {
    100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400, 204800, 409600, 819200, 1638400, 3276800
};

static hi_u8 get_iso_blc_index_cali(hi_u32 iso)
{
    hi_u8 index;
    for (index = 0; index < DYNABLC_ISO_NUM_CALI - 1; index++) {
        if (iso <= dynablc_iso_samcali[index]) {
            break;
        }
    }
    return index;
}

static hi_s32 linear_inter_cali(hi_s32 v, hi_s32 x0, hi_s32 y0, hi_s32 x1, hi_s32 y1)
{
    hi_s32 res;

    if (v <= x0) {
        return y0;
    }
    if (v >= x1) {
        return y1;
    }

    res = (hi_s64)(y1 - y0) * (hi_s64)(v - x0) / div_0_to_1((x1 - x0) + y0);
    return res;
}

static hi_void dynablc_offset_inter_cali(hi_u64 iso, blc_res *blc_stat, hi_u8 m)
{
    hi_u8 iso_index_up, iso_index_low;
    hi_u16 iso1, iso2;
    hi_s16 offset1, offset2;
    iso_index_up = get_iso_blc_index_cali(iso);
    iso_index_low = max_2((hi_s8)iso_index_up - 1, 0);
    iso1          = dynablc_iso_real_cali[iso_index_low];
    iso2          = dynablc_iso_real_cali[iso_index_up];
    offset1       = blc_stat[iso_index_low].offset;
    offset2       = blc_stat[iso_index_up].offset;

    blc_stat[m].offset_final = (hi_s16)linear_inter_cali(iso, iso1, offset1, iso2, offset2);
}

/* one frame */

static void usage(void)
{
    printf("\n"
        "*************************************************\n"
        "usage: ./sample_dynamic_blc_online_cali [vi_pipe] [frame_cnt] [iso_num]"
        "[light_stt_area.x] [light_stt_area.y] [light_stt_area.width] [light_stt_area.height]\n"
        "vi_pipe: \n"
        "   0:vi_pipe0 ~~ 3:vi_pipe3\n"
        "frame_cnt: \n"
        "   frame_cnt value to be used to calculate black level(range:(0,30])\n"
        "iso_num: \n"
        "   iso_num value to be used to set again(range:(0,16])\n"
        "light_stt_area.x \n"
        "X coordinates of the light statistical region(range:[0, image_width])\n"
        "light_stt_area.y \n"
        "Y coordinates of the light statistical region(range:[ob_height, image_height])\n"
        "light_stt_area.width \n"
        "width of the light statistical region(range:[0,image_width - light_stt_area.x])\n"
        "light_stt_area.height \n"
        "height of the light statistical region(range:[0, image_height - light_stt_area.y])\n"
        ": \n"
        "e.g : ./sample_dynamic_blc_online_cali 0 30 7 100 100 100 100\n"
        "*************************************************\n"
        "\n");
}

static hi_s32 pixel_format2_bit_width(hi_pixel_format *pixel_format)
{
    hi_pixel_format en_pixel_format;
    en_pixel_format = *pixel_format;
    hi_s32 bit_width;
    switch (en_pixel_format) {
        case HI_PIXEL_FORMAT_RGB_BAYER_8BPP:
            bit_width = ISP_SENSOR_8BIT;
            break;
        case HI_PIXEL_FORMAT_RGB_BAYER_10BPP:
            bit_width = ISP_SENSOR_10BIT;
            break;
        case HI_PIXEL_FORMAT_RGB_BAYER_12BPP:
            bit_width = ISP_SENSOR_12BIT;
            break;
        case HI_PIXEL_FORMAT_RGB_BAYER_14BPP:
            bit_width = ISP_SENSOR_14BIT;
            break;
        case HI_PIXEL_FORMAT_RGB_BAYER_16BPP:
            bit_width = ISP_SENSOR_16BIT;
            break;
        default:
            bit_width = HI_FAILURE;
            break;
    }

    return bit_width;
}

static hi_void convert_bit_pixel(hi_u8 *data, hi_u32 data_num, hi_u32 bit_width, hi_u16 *out_data)
{
    hi_s32 i, tmp_var, out_cnt;
    hi_u32 value;
    hi_u64 val;
    hi_u8 *tmp = data;

    out_cnt = 0;
    switch (bit_width) {
        case ISP_SENSOR_12BIT:
            tmp_var = data_num / 2; /* 2 pixels consist of 3 bytes */

            for (i = 0; i < tmp_var; i++) {
                /* byte2 byte1 byte0 */
                tmp = data + 3 * i; /* 2 pixels consist of 3 bytes */
                value = tmp[0] + (tmp[1] << 8) + (tmp[0x2] << 16); /* left shift 8, 16 */
                out_data[out_cnt++] = value & 0xfff;
                out_data[out_cnt++] = (value >> 12) & 0xfff; /* right shift 12 */
            }
            break;
        case ISP_SENSOR_14BIT:
            tmp_var = data_num / 4; /* 4 pixels consist of 7 bytes */

            for (i = 0; i < tmp_var; i++) {
                tmp = data + 7 * i; /* 4 pixels consist of 7 bytes */
                val = tmp[0] + ((hi_u32)tmp[1] << 0x8) + ((hi_u32)tmp[0x2] << 0x10) + ((hi_u32)tmp[0x3] << 0x18) +
                    ((hi_u64)tmp[0x4] << 32) + ((hi_u64)tmp[0x5] << 40) + ((hi_u64)tmp[0x6] << 48); /* lsh 32 40 48 */

                out_data[out_cnt++] = val & 0x3fff;
                out_data[out_cnt++] = (val >> 14) & 0x3fff; /* right shift 14 */
                out_data[out_cnt++] = (val >> 28) & 0x3fff; /* right shift 28 */
                out_data[out_cnt++] = (val >> 42) & 0x3fff; /* right shift 42 */
            }
            break;
        default: /* 16bit */
            /* 1 pixels consist of 2 bytes */
            tmp_var = data_num;

            for (i = 0; i < tmp_var; i++) { /* byte1 byte0 */
                tmp = data + 2 * i; /* 1 pixels consist of 2 bytes */
                val = tmp[0] + (tmp[1] << 8); /* left shift 8 */
                out_data[out_cnt++] = val & 0xffff;
            }
            break;
    }

    return;
}

static hi_s32 dump_raw_proc(hi_u8 *data, const hi_video_frame *v_buf,
    hi_u16 *data_16bit, hi_u32 nbit, FILE *pfd)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 h;

    for (h = 0; h < v_buf->height; h++) {
        if (nbit == ISP_SENSOR_8BIT) {
            fwrite(data, v_buf->width, 1, pfd);
        } else if (nbit == ISP_SENSOR_16BIT) {
            fwrite(data, v_buf->width, 2, pfd); /* 2 bytes */
            ret = fflush(pfd);
            if (ret != HI_SUCCESS) {
                printf("flush failed!!!\n");
            }
        } else {
            if (data_16bit == NULL) {
                printf("data_16bit is null!\n");
                return HI_FAILURE;
            }
            convert_bit_pixel(data, v_buf->width, nbit, data_16bit);
            fwrite(data_16bit, v_buf->width, 2, pfd); /* 2 bytes */
        }
        data += v_buf->stride[0];
    }
    return ret;
}

static hi_s32 dump_dynamic_blc_raw(hi_u8 *user_page_addr, hi_u32 nbit, const hi_video_frame *v_buf,
    hi_u64 size, FILE *pfd)
{
    hi_s32 ret;
    hi_u16 *data_16bit = NULL;

    printf("dump raw frame of vi  to file: \n");

    /* open file */
    if (pfd == NULL) {
        printf("open file failed!\n");
        return HI_FAILURE;
    }

    if (nbit != ISP_SENSOR_8BIT) {
        data_16bit = (hi_u16 *)malloc(v_buf->width * 2); /* 2 bytes */
        if (data_16bit == NULL) {
            printf("alloc memory failed\n");
            return HI_FAILURE;
        }
    }

    /* save Y ---------------------------------------------------------------- */
    printf("saving......dump data......stride[0]: %u, width: %u\n", v_buf->stride[0], v_buf->width);

    ret = dump_raw_proc(user_page_addr, v_buf, data_16bit, nbit, pfd);
    if (ret != HI_SUCCESS) {
        goto end;
    }

    printf("done time_ref: %u!\n", v_buf->time_ref);

end:
    if (data_16bit != NULL) {
        free(data_16bit);
    }
    return ret;
}

static hi_void dyna_blc_stat_light(const hi_video_frame *v_buf, hi_isp_black_level_attr *ob_rect,
    hi_u16 *data_16bit, blc_res *res)
{
    hi_u64 i, j;
    hi_u64 x_start, y_start;
    x_start = res->light_area.x;
    y_start = res->light_area.y;

    for (i = res->light_area.y; i < (y_start + res->light_area.height); i++) {
        for (j = res->light_area.x; j < (x_start + res->light_area.width); j++) {
            if ((ob_rect->dynamic_attr.low_threshold < data_16bit[i*v_buf->width + j]) &&
                (data_16bit[i*v_buf->width + j] < (ob_rect->dynamic_attr.high_threshold << 4))) { /* left shift 4bit */
                (res->light_blc_sum) += data_16bit[i*v_buf->width + j];
                (res->light_pix_cnt)++;
            }
        }
    }
}

static hi_void get_stat_res(blc_res *res)
{
    res->ob_blc = res->ob_blc / div_0_to_1(res->frame_cnt);
    res->light_blc = (res->light_blc_sum / div_0_to_1(res->light_pix_cnt)) >> 2; /* right shift 2 bit */
    res->offset = res->light_blc - res->ob_blc;
    res->light_blc_sum = 0;
    res->light_pix_cnt = 0;
}

static hi_s32 dyna_blc_stat(hi_u16 *user_page_addr, const hi_video_frame *v_buf,
    hi_isp_black_level_attr *ob_rect, blc_res *res, int cur_frame_cnt)
{
    dyna_blc_stat_light(v_buf, ob_rect, user_page_addr, res);

    if ((hi_u64)cur_frame_cnt == (res->frame_cnt - 1)) {
        get_stat_res(res);
    }

    return HI_SUCCESS;
}

static hi_s32 dynamic_blc_calibration_proc(hi_vi_pipe vi_pipe, hi_video_frame *v_buf,
    blc_res *res, hi_u8 cur_frame_cnt)
{
    hi_u64 phy_addr, size, info_blc;
    hi_u16 *user_page_addr = NULL; /* addr */
    hi_s32 nbit;
    hi_isp_black_level_attr black_level_attr;
    hi_s32 ret;
    hi_pixel_format pixel_format = v_buf->pixel_format;
    hi_isp_inner_state_info inner_state_info;
    ret = hi_mpi_isp_query_inner_state_info(vi_pipe, &inner_state_info);
    if (ret != HI_SUCCESS) {
        printf("get isp_info failed!!!\n");
    }

    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("get ob_attr failed!!!\n");
    }

    nbit = pixel_format2_bit_width(&pixel_format);
    if (nbit != ISP_SENSOR_10BIT && nbit != ISP_SENSOR_12BIT && nbit != ISP_SENSOR_14BIT && nbit != ISP_SENSOR_16BIT) {
        printf("can't not support %d bits raw, only support 10bits,12bits,14bits,16bits\n", nbit);
        return HI_FAILURE;
    }

    size = (v_buf->stride[0]) * (v_buf->height);
    phy_addr = v_buf->phys_addr[0];

    user_page_addr = (hi_u16 *)hi_mpi_sys_mmap(phy_addr, size);
    if (user_page_addr == NULL) {
        printf("user_page_addr == NULL");
        return HI_FAILURE;
    }
    info_blc = (inner_state_info.sns_black_level[0][HI_ISP_CHN_R] + inner_state_info.sns_black_level[0][HI_ISP_CHN_GR]
                + inner_state_info.sns_black_level[0][HI_ISP_CHN_GB]
                + inner_state_info.sns_black_level[0][HI_ISP_CHN_B]) >> 2; /* right shift 2 bits */
    res->ob_blc += info_blc;
    res->bit_width = nbit;
    ret = dyna_blc_stat(user_page_addr, v_buf, &black_level_attr, res, cur_frame_cnt);
    if (ret != HI_SUCCESS) {
        printf("calculate offset failed!\n");
        hi_mpi_sys_munmap(user_page_addr, size);
        return ret;
    }

    hi_mpi_sys_munmap(user_page_addr, size);
    return HI_SUCCESS;
}

static hi_s32 save_dynamic_blc_output(hi_isp_black_level_attr *blc_res)
{
    hi_s32 i;
    hi_s32 ret;

    FILE *file = fopen("dynamic_blc_offset.txt", "wb");
    if (file == HI_NULL) {
        printf("create file fails\n");
        return HI_FAILURE;
    }
    ret = fprintf(file, "dynamic blc offset = ");
    if (ret == HI_FAILURE) {
        printf("print failed!!!\n");
    }

    for (i = 0; i < DYNABLC_ISO_NUM_CALI; i++) {
        ret = fprintf(file, "%d,", blc_res->dynamic_attr.offset[i]);
        if (ret == HI_FAILURE) {
            printf("print failed!!!\n");
        }
    }
    ret = fprintf(file, "\n");
    if (ret == HI_FAILURE) {
        printf("print failed!!!\n");
    }

    ret = fprintf(file, "dynamic blc cali black level = ");
    if (ret == HI_FAILURE) {
        printf("print failed!!!\n");
    }

    for (i = 0; i < DYNABLC_ISO_NUM_CALI; i++) {
        ret = fprintf(file, "%d,", blc_res->dynamic_attr.calibration_black_level[i]);
        if (ret == HI_FAILURE) {
            printf("print failed!!!\n");
        }
    }

    ret = fclose(file);
    if (ret != HI_SUCCESS) {
        printf("close failed!!!\n");
    }

    return HI_SUCCESS;
}

static hi_void blc_stat_init(blc_res *blc_stat, hi_u32 iso_num, hi_u32 frame_cnt, hi_rect *light_area)
{
    hi_u32 i;
    for (i = 0; i < DYNABLC_ISO_NUM_CALI; i++) {
        blc_stat[i].iso = 100;      /* 100 is init iso */
        blc_stat[i].frame_cnt = frame_cnt;
        blc_stat[i].light_blc = 0;
        blc_stat[i].ob_blc = 0;
        blc_stat[i].offset = 0;
        blc_stat[i].offset_final = 0;
        blc_stat[i].bayer_format = 21; /* 21 bayer format */
        blc_stat[i].iso_num = iso_num;
        blc_stat[i].y = 0;
        blc_stat[i].bit_width = 0;
        blc_stat[i].pre_bit_width = 0;
        blc_stat[i].ae_para.op_type = 0;
        blc_stat[i].ae_para.exp_time_op_type = 0;
        blc_stat[i].ae_para.a_gain_op_type = 0;
        blc_stat[i].ae_para.d_gain_op_type = 0;
        blc_stat[i].ae_para.ispd_gain_op_type = 0;
        blc_stat[i].ae_para.exp_time = 0;
        blc_stat[i].ae_para.a_gain = 0;
        blc_stat[i].ae_para.d_gain = 0;
        blc_stat[i].ae_para.isp_d_gain = 0;
        blc_stat[i].light_blc_sum = 0;
        blc_stat[i].light_pix_cnt = 0;
        blc_stat[i].light_area.x = light_area->x;
        blc_stat[i].light_area.y = light_area->y;
        blc_stat[i].light_area.width  = light_area->width;
        blc_stat[i].light_area.height = light_area->height;
    }
}

static hi_void cali_change_pub_attr(hi_vi_pipe vi_pipe, blc_res *blc_stat, hi_u32 iso_num)
{
    hi_s32 ret;
    hi_isp_pub_attr pub_attr;

    ret = hi_mpi_isp_get_pub_attr(vi_pipe, &pub_attr);
    if (ret != HI_SUCCESS) {
        printf("get pub_attr failed!!!\n");
    }

    blc_stat[iso_num].y = pub_attr.wnd_rect.y;
    printf("%s %d blc_stat[%u].y = %u\n", __FUNCTION__, __LINE__, iso_num, blc_stat[iso_num].y);
    pub_attr.wnd_rect.y = 0;

    ret = hi_mpi_isp_set_pub_attr(vi_pipe, &pub_attr);
    if (ret != HI_SUCCESS) {
        printf("set pub_attr failed!!!\n");
    }
}

static hi_void save_ori_ae_attr(blc_res *blc_stat, hi_isp_exposure_attr *exp_attr)
{
    blc_stat[0].ae_para.op_type = exp_attr->op_type;
    blc_stat[0].ae_para.exp_time_op_type = exp_attr->manual_attr.exp_time_op_type;
    blc_stat[0].ae_para.a_gain_op_type = exp_attr->manual_attr.a_gain_op_type;
    blc_stat[0].ae_para.d_gain_op_type = exp_attr->manual_attr.d_gain_op_type;
    blc_stat[0].ae_para.ispd_gain_op_type = exp_attr->manual_attr.ispd_gain_op_type;
    blc_stat[0].ae_para.exp_time = exp_attr->manual_attr.exp_time;
    blc_stat[0].ae_para.a_gain = exp_attr->manual_attr.a_gain;
    blc_stat[0].ae_para.d_gain = exp_attr->manual_attr.d_gain;
    blc_stat[0].ae_para.isp_d_gain = exp_attr->manual_attr.isp_d_gain;
}

static hi_void set_gain_acor_sensor(hi_isp_exposure_attr *exp_attr,  hi_s32 k)
{
    sample_sns_type sns_type = SENSOR0_TYPE;
    isp_err_trace("sns_type = %d\n", sns_type);

    switch (sns_type) {
        case SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT:
            exp_attr->manual_attr.a_gain = 1024 * pow(2, k); /* 1024 means gain multiplied by one */
            exp_attr->manual_attr.d_gain = 1024; /* 1024 means gain multiplied by one */
            break;
        case OV_OS08A20_MIPI_8M_30FPS_12BIT:
            if (k < 4) { /* 4 is iso num */
                exp_attr->manual_attr.a_gain = 1024 * pow(2, k); /* 1024 means gain multiplied by one */
                exp_attr->manual_attr.d_gain = 1024; /* 1024 means gain multiplied by one */
            } else {
                exp_attr->manual_attr.d_gain = 1024 * pow(2, k); /* 1024 means gain multiplied by one */
            }
            break;
        case ISR2006_MIPI_1936_1280_25FPS_12BIT:
            if(k < 32) { /* 4 is iso num */
                exp_attr->manual_attr.a_gain = 1024 * pow(2, k); /* 1024 means gain multiplied by one */
                exp_attr->manual_attr.d_gain = 1024; /* 1024 means gain multiplied by one */    
            } else {
                exp_attr->manual_attr.d_gain = 1024 * pow(2, k); /* 1024 means gain multiplied by one */
            }
            break;
        default:
            break;
    }

    return;
}

static hi_void cali_change_exp_attr(hi_vi_pipe vi_pipe, blc_res *blc_stat, hi_s32 k)
{
    hi_s32 ret;
    hi_isp_exposure_attr   exp_attr;

    ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        printf("get exp_attr failed!!!\n");
    }
    if (k == 0) {
        save_ori_ae_attr(blc_stat, &exp_attr);
    }
    exp_attr.op_type        = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.exp_time_op_type = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.a_gain_op_type = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.d_gain_op_type = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.ispd_gain_op_type = HI_OP_MODE_MANUAL;
    set_gain_acor_sensor(&exp_attr, k);
    isp_err_trace("exp_attr.manual_attr.a_gain = %u\n", exp_attr.manual_attr.a_gain);
    exp_attr.manual_attr.isp_d_gain = 1024; /* 1024 means gain multiplied by one */
    ret = hi_mpi_isp_set_exposure_attr(vi_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        printf("set exp_attr failed!!!\n");
    }
}

static hi_void release_pipe_frame(hi_vi_pipe vi_pipe, hi_video_frame_info *ast_frame, hi_s32 num)
{
    hi_s32 ret;
    for (; num >= 0; num--) {
        ret = hi_mpi_vi_release_pipe_frame(vi_pipe, &ast_frame[num]);
        if (ret != HI_SUCCESS) {
            printf("release frame error\n");
            break;
        }
    }
}

static hi_s32 get_vi_frame(hi_vi_pipe vi_pipe, hi_video_frame_info *ast_frame, hi_u32 frame_cnt)
{
    hi_u32 i;
    const hi_s32 milli_sec = 4000; /* 4000 milli_sec */
    sleep(1);
    for (i = 0; i < frame_cnt; i++) {
        if (hi_mpi_vi_get_pipe_frame(vi_pipe, &ast_frame[i], milli_sec) != HI_SUCCESS) {
            printf("get vi pipe %d frame err\n", vi_pipe);
            printf("only get %u frame\n", i);
            printf("video buffer is not enought!!!\n");
            release_pipe_frame(vi_pipe, ast_frame, i);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

static hi_void get_exp_info(hi_vi_pipe vi_pipe, blc_res *blc_stat, hi_s32 k)
{
    hi_s32 ret;
    hi_isp_exp_info exp_info;

    ret = hi_mpi_isp_query_exposure_info(vi_pipe, &exp_info);
    if (ret != HI_SUCCESS) {
        printf("get exp_info failed!!!\n");
    }

    blc_stat[k].iso = exp_info.iso;
}

static hi_void clear_pfd_mem(FILE *pfd)
{
    hi_s32 ret;
    ret = fflush(pfd);
    if (ret != HI_SUCCESS) {
        printf("flush failed!!!\n");
    }

    if (pfd != NULL) {
        ret = fclose(pfd);
        if (ret != HI_SUCCESS) {
            printf("close failed!!!\n");
        }
    }
}

static hi_void dyna_close_pfd(FILE *pfd)
{
    hi_s32 ret;
    if (pfd != NULL) {
        ret = fclose(pfd);
        if (ret != HI_SUCCESS) {
            printf("close failed!!!\n");
        }
    }
}

static hi_s32 pass_res_to_mpi(hi_vi_pipe vi_pipe, hi_isp_black_level_attr *black_level_attr,
    blc_res *blc_stat)
{
    hi_u32 i;
    hi_s32 nbit;
    hi_s32 ret;

    hi_vi_pipe_attr pipe_attr;

    ret = hi_mpi_vi_get_pipe_attr(vi_pipe, &pipe_attr);
    if (ret != HI_SUCCESS) {
        printf("get pipe %d attr failed!\n", vi_pipe);
        return HI_FAILURE;
    }
    printf("pipe_attr.pixel_format = %d\n", pipe_attr.pixel_format);

    nbit = pixel_format2_bit_width(&pipe_attr.pixel_format);
    printf("nbit = %d\n", nbit);

    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("get ob_attr failed!!!\n");
    }

    for (i = 0; i < blc_stat[0].iso_num; i++) {
        dynablc_iso_real_cali[i] = blc_stat[i].iso;
    }

    for (i = 0; i < blc_stat[0].iso_num; i++) {
        dynablc_offset_inter_cali(dynablc_iso_samcali[i], blc_stat, i);

        black_level_attr->dynamic_attr.offset[i] = blc_stat[i].offset;
        printf("black_level_attr->dynamic_attr.offset[%u] = %d\n", i, black_level_attr->dynamic_attr.offset[i]);

        black_level_attr->dynamic_attr.calibration_black_level[i] = blc_stat[i].ob_blc;
        printf("black_level_attr->dynamic_attr.calibration_black_level[%u] = %u\n", i,
            black_level_attr->dynamic_attr.calibration_black_level[i]);
    }

    ret = hi_mpi_isp_set_black_level_attr(vi_pipe, black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("set ob_attr failed!!!\n");
    }

    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("set ob_attr failed!!!\n");
    }

    ret = save_dynamic_blc_output(black_level_attr);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return ret;
}

static hi_s32 save_raw_to_file(hi_video_frame_info *ast_frame, blc_res *blc_stat, hi_u32 k)
{
#if DUMP_RAW_AND_SAVE_DYNAMIC_BLC == 0
    return HI_SUCCESS;
#endif
    if (k != (blc_stat[0].iso_num - 1)) {
        return HI_SUCCESS;
    }
    /* dump  */
    hi_u64 phy_addr, size;
    hi_u8 *user_page_addr = NULL; /* addr */
    hi_u32 nbit;
    hi_s32 ret;
    hi_u32 j;
    FILE *pfd = NULL;
    pfd = fopen("dynamic_blc.raw", "wb");

    for (j = 0; j < blc_stat[0].frame_cnt; j++) {
        hi_pixel_format pixel_format = ast_frame[j].video_frame.pixel_format;

        nbit = pixel_format2_bit_width(&pixel_format);
        if (nbit != ISP_SENSOR_10BIT && nbit != ISP_SENSOR_12BIT && nbit != ISP_SENSOR_14BIT
            && nbit != ISP_SENSOR_16BIT) {
            printf("can't not support %d bits raw, only support 10bits,12bits,14bits,16bits\n", nbit);
            dyna_close_pfd(pfd);
            return HI_FAILURE;
        }

        size = (ast_frame[j].video_frame.stride[0]) * (ast_frame[j].video_frame.height);
        phy_addr = ast_frame[j].video_frame.phys_addr[0];

        user_page_addr = (hi_u8 *)hi_mpi_sys_mmap(phy_addr, size);
        if (user_page_addr == NULL) {
            dyna_close_pfd(pfd);
            return HI_FAILURE;
        }

        ret = dump_dynamic_blc_raw(user_page_addr, nbit, &ast_frame[j].video_frame, size, pfd);
        if (ret != HI_SUCCESS) {
            printf("dump dynamic blc.raw failed!\n");
            clear_pfd_mem(pfd);
            goto free_mem;
        }
        hi_mpi_sys_munmap(user_page_addr, size);
    }
    clear_pfd_mem(pfd);
    return HI_SUCCESS;
free_mem:
    hi_mpi_sys_munmap(user_page_addr, size);
    return ret;
}

static hi_void release_all_dump_fram(hi_vi_pipe vi_pipe, hi_video_frame_info *frame_buf, hi_s32 frame_cnt)
{
    hi_s32 i;
    for (i = 0; i < frame_cnt; i++) {
        hi_mpi_vi_release_pipe_frame(vi_pipe, &frame_buf[i]);
    }
}
static hi_s32 dynamic_blc_online_cali_proc(hi_vi_pipe vi_pipe, blc_res *blc_stat)
{
    hi_u32 j, k;
    hi_s32 ret;
    hi_video_frame_info *ast_frame = NULL;

    ast_frame = (hi_video_frame_info*)malloc(blc_stat[0].frame_cnt * sizeof(hi_video_frame_info));
    if (ast_frame == HI_NULL) {
        return HI_FAILURE;
    }
    for (k = 0; k < blc_stat[0].iso_num; k++) {
        cali_change_pub_attr(vi_pipe, blc_stat, k);
        cali_change_exp_attr(vi_pipe, blc_stat, k);

        (hi_void)memset_s(ast_frame, blc_stat[0].frame_cnt * sizeof(hi_video_frame_info), 0,
            blc_stat[0].frame_cnt * sizeof(hi_video_frame_info));
        /* get VI frame  */
        ret = get_vi_frame(vi_pipe, ast_frame, blc_stat[0].frame_cnt);
        if (ret != HI_SUCCESS) {
            goto free_mem;
        }
        ret = save_raw_to_file(ast_frame, blc_stat, k);
        if (ret != HI_SUCCESS) {
            release_all_dump_fram(vi_pipe, ast_frame, blc_stat[0].frame_cnt);
            goto free_mem;
        }

        /* calc  */
        for (j = 0; j < blc_stat[0].frame_cnt; j++) {
            /* save VI frame to file */
            ret = dynamic_blc_calibration_proc(vi_pipe, &ast_frame[j].video_frame, &blc_stat[k], j);
            if (ret != HI_SUCCESS) {
                release_all_dump_fram(vi_pipe, ast_frame, blc_stat[0].frame_cnt);
                goto free_mem;
            }

            if (j == (blc_stat[0].frame_cnt - 1)) {
                get_exp_info(vi_pipe, blc_stat, k);
            }
        }
        release_all_dump_fram(vi_pipe, ast_frame, blc_stat[0].frame_cnt);
    }
free_mem:
    free(ast_frame);
    return ret;
}

static hi_void dynamic_blc_cali_sensor_cfg(hi_vi_pipe vi_pipe, sample_sns_type sns_type)
{
    hi_isp_sns_blc_clamp sns_blc_clamp;

    sns_blc_clamp.blc_clamp_en = HI_FALSE;

    switch (sns_type) {
        case SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT:
            g_sns_imx347_slave_obj.pfn_mirror_flip(vi_pipe, ISP_SNS_MIRROR);
            g_sns_imx347_slave_obj.pfn_set_blc_clamp(vi_pipe, sns_blc_clamp);
            break;
        case OV_OS08A20_MIPI_8M_30FPS_12BIT:
            g_sns_os08a20_obj.pfn_set_blc_clamp(vi_pipe, sns_blc_clamp);
            break;
        default:
            break;
    }

    return;
}

static hi_s32 sample_dynamic_blc_prepare_vi(hi_vi_pipe vi_pipe,
    hi_vi_frame_dump_attr *raw_dump_attr, hi_vi_pipe_attr *ast_back_up_pipe_attr, blc_res *blc_stat)
{
    hi_s32 ret;
    hi_vi_frame_dump_attr dump_attr;
    hi_vi_pipe_attr pipe_attr;
    sample_sns_type sns_type = SENSOR0_TYPE;

    dynamic_blc_cali_sensor_cfg(vi_pipe, sns_type);

    ret = hi_mpi_vi_get_pipe_frame_dump_attr(vi_pipe, raw_dump_attr);
    if (ret != HI_SUCCESS) {
        printf("get pipe %d dump attr failed!\n", vi_pipe);
        return HI_FAILURE;
    }

    (hi_void)memcpy_s(&dump_attr, sizeof(hi_vi_frame_dump_attr), raw_dump_attr, sizeof(hi_vi_frame_dump_attr));
    dump_attr.enable = HI_TRUE;
    dump_attr.depth = 2; /* 2 raw depth */

    ret = hi_mpi_vi_set_pipe_frame_dump_attr(vi_pipe, &dump_attr);
    if (ret != HI_SUCCESS) {
        printf("set pipe %d dump attr failed!\n", vi_pipe);
        return HI_FAILURE;
    }

    ret = hi_mpi_vi_get_pipe_attr(vi_pipe, ast_back_up_pipe_attr);
    if (ret != HI_SUCCESS) {
        printf("get pipe %d attr failed!\n", vi_pipe);
        return HI_FAILURE;
    }

    (hi_void)memcpy_s(&pipe_attr, sizeof(hi_vi_pipe_attr), ast_back_up_pipe_attr, sizeof(hi_vi_pipe_attr));
    pipe_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    blc_stat[0].pre_bit_width = pipe_attr.pixel_format;
    pipe_attr.pixel_format = OT_PIXEL_FORMAT_RGB_BAYER_16BPP;
    ret = hi_mpi_vi_set_pipe_attr(vi_pipe, &pipe_attr);
    if (ret != HI_SUCCESS) {
        printf("set pipe %d attr failed!\n", vi_pipe);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void recovery_ae_and_pub_attr(hi_vi_pipe vi_pipe, blc_res *blc_stat)
{
    hi_s32 ret;
    hi_isp_pub_attr pub_attr;
    hi_isp_exposure_attr   exp_attr;

    ret = hi_mpi_isp_get_pub_attr(vi_pipe, &pub_attr);
    if (ret != HI_SUCCESS) {
        printf("get pub_attr failed!!!\n");
    }
    printf("blc_stat[0].y = %u\n", blc_stat[0].y);
    pub_attr.wnd_rect.y = blc_stat[0].y;

    ret = hi_mpi_isp_set_pub_attr(vi_pipe, &pub_attr);
    if (ret != HI_SUCCESS) {
        printf("set pub_attr failed!!!\n");
    }

    ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        printf("get exp_attr failed!!!\n");
    }
    exp_attr.op_type        = blc_stat[0].ae_para.op_type;
    exp_attr.manual_attr.exp_time_op_type = blc_stat[0].ae_para.exp_time_op_type;
    exp_attr.manual_attr.a_gain_op_type = blc_stat[0].ae_para.a_gain_op_type;
    exp_attr.manual_attr.d_gain_op_type = blc_stat[0].ae_para.d_gain_op_type;
    exp_attr.manual_attr.ispd_gain_op_type = blc_stat[0].ae_para.ispd_gain_op_type;
    exp_attr.manual_attr.exp_time = blc_stat[0].ae_para.exp_time;
    exp_attr.manual_attr.a_gain = blc_stat[0].ae_para.a_gain;
    exp_attr.manual_attr.d_gain = blc_stat[0].ae_para.d_gain;
    exp_attr.manual_attr.isp_d_gain = blc_stat[0].ae_para.isp_d_gain;
    ret = hi_mpi_isp_set_exposure_attr(vi_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        printf("set exp_attr failed!!!\n");
    }
}

static hi_s32 recovery_attr(hi_vi_pipe vi_pipe,
    hi_vi_frame_dump_attr *raw_dump_attr, hi_vi_pipe_attr *ast_back_up_pipe_attr, blc_res *blc_stat)
{
    hi_u32 ret;
    hi_vi_pipe_attr pipe_attr;

    ret = hi_mpi_vi_set_pipe_attr(vi_pipe, ast_back_up_pipe_attr);
    if (ret != HI_SUCCESS) {
        printf("set pipe %d attr failed!\n", vi_pipe);
        return ret;
    }

    ret = hi_mpi_vi_set_pipe_frame_dump_attr(vi_pipe, raw_dump_attr);
    if (ret != HI_SUCCESS) {
        printf("set pipe %d dump attr failed!\n", vi_pipe);
        return ret;
    }

    (hi_void)memcpy_s(&pipe_attr, sizeof(hi_vi_pipe_attr), ast_back_up_pipe_attr, sizeof(hi_vi_pipe_attr));
    pipe_attr.pixel_format = blc_stat[0].pre_bit_width;
    ret = hi_mpi_vi_set_pipe_attr(vi_pipe, &pipe_attr);
    if (ret != HI_SUCCESS) {
        printf("set pipe %d attr failed!\n", vi_pipe);
        return HI_FAILURE;
    }

    recovery_ae_and_pub_attr(vi_pipe, blc_stat);
    return HI_SUCCESS;
}

static hi_s32 sample_dynamic_blc_run(hi_vi_pipe vi_pipe, hi_u32 frame_cnt, hi_u32 iso_num, hi_rect *light_area)
{
    hi_s32 ret;
    hi_isp_black_level_attr black_level_attr;
    hi_vi_frame_dump_attr raw_dump_attr;
    hi_vi_pipe_attr ast_back_up_pipe_attr;
    blc_res *blc_stat = HI_NULL;

    printf("setting parameter ==> frame_cnt = %u\n", frame_cnt);
    printf("setting parameter ==> iso_num = %u\n", iso_num);
    blc_stat = (blc_res*)malloc(sizeof(blc_res) * DYNABLC_ISO_NUM_CALI);
    if (blc_stat == HI_NULL) {
        return HI_FAILURE;
    }

    blc_stat_init(&blc_stat[0], iso_num, frame_cnt, light_area);

    ret = sample_dynamic_blc_prepare_vi(vi_pipe, &raw_dump_attr, &ast_back_up_pipe_attr, blc_stat);
    if (ret != HI_SUCCESS) {
        goto end;
    }

    ret = dynamic_blc_online_cali_proc(vi_pipe, blc_stat);
    if (ret != HI_SUCCESS) {
        printf("%s %d blc cali err\n", __FUNCTION__, __LINE__);
        goto end;
    }

    ret = pass_res_to_mpi(vi_pipe, &black_level_attr, blc_stat);
    if (ret != HI_SUCCESS) {
        printf("%s %d set mpi para err\n", __FUNCTION__, __LINE__);
        goto end;
    }

    ret = recovery_attr(vi_pipe, &raw_dump_attr, &ast_back_up_pipe_attr, blc_stat);
    if (ret != HI_SUCCESS) {
        goto end;
    }

end:
    free(blc_stat);
    return ret;
}

static hi_void sample_dynablc_get_char(hi_void)
{
    if (g_sig_flag == 1) {
        return;
    }

    sample_pause();
}

static hi_void sample_dynablc_vi_get_default_vb_config(hi_size *size, hi_vb_cfg *vb_cfg,
    hi_u32 yuv_cnt, hi_u32 raw_cnt)
{
    hi_vb_calc_cfg calc_cfg;
    hi_pic_buf_attr buf_attr;

    (hi_void)memset_s(vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 blks */

    /* default YUV pool: SP420 + compress_seg */
    buf_attr.width         = size->width;
    buf_attr.height        = size->height;
    buf_attr.align         = HI_DEFAULT_ALIGN;
    buf_attr.bit_width     = HI_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    buf_attr.compress_mode = OT_COMPRESS_MODE_SEG;
    hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);
    vb_cfg->common_pool[0].blk_size = calc_cfg.vb_size;
    vb_cfg->common_pool[0].blk_cnt  = yuv_cnt;

    /* default raw pool: raw16bpp + compress_none */
    buf_attr.pixel_format  = HI_PIXEL_FORMAT_RGB_BAYER_16BPP;
    buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);
    vb_cfg->common_pool[1].blk_size = calc_cfg.vb_size;
    vb_cfg->common_pool[1].blk_cnt  = raw_cnt;

    isp_err_trace("raw_cnt = %u\n", raw_cnt);
    isp_err_trace("yuv_cnt = %u\n", yuv_cnt);
}

static hi_s32 sample_dynablc_vio_sys_init(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode,
    hi_u32 yuv_cnt, hi_u32 raw_cnt)
{
    hi_s32 ret;
    hi_size size;
    hi_vb_cfg vb_cfg;
    hi_u32 supplement_config;
    sample_sns_type sns_type = SENSOR0_TYPE;

    sample_comm_vi_get_size_by_sns_type(sns_type, &size);
    sample_dynablc_vi_get_default_vb_config(&size, &vb_cfg, yuv_cnt, raw_cnt);

    supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;
    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, aiisp_mode);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_dynablc_vio_start_vpss(hi_vpss_grp grp, hi_size *in_size)
{
    hi_s32 ret;
    hi_low_delay_info low_delay_info;
    hi_vpss_grp_attr grp_attr;
    sample_vpss_chn_attr vpss_chn_attr = {0};

    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width  = in_size->width;
    grp_attr.max_height = in_size->height;
    sample_comm_vpss_get_default_chn_attr(&vpss_chn_attr.chn_attr[0]);
    vpss_chn_attr.chn_attr[0].width  = in_size->width;
    vpss_chn_attr.chn_attr[0].height = in_size->height;
    vpss_chn_attr.chn_enable[0] = HI_TRUE;
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;

    ret = sample_common_vpss_start(grp, &grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    low_delay_info.enable     = HI_TRUE;
    low_delay_info.line_cnt   = 200; /* 200: lowdelay line */
    low_delay_info.one_buf_en = HI_FALSE;
    ret = hi_mpi_vpss_set_chn_low_delay(grp, 0, &low_delay_info);
    if (ret != HI_SUCCESS) {
        sample_common_vpss_stop(grp, vpss_chn_attr.chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
        return ret;
    }

    return HI_SUCCESS;
}


static hi_s32 sample_dynablc_vio_start_vo(sample_vo_mode vo_mode)
{
    g_dynablc_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_dynablc_vo_cfg);
}

static hi_s32 sample_dynablc_vio_start_venc(hi_venc_chn venc_chn[], size_t size, hi_u32 chn_num)
{
    hi_s32 i, ret;
    hi_size in_size;
    sample_sns_type sns_type = SENSOR0_TYPE;
    if (chn_num > size) {
        return HI_FAILURE;
    }

    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);

    g_dynablc_venc_chn_param.venc_size.width  = in_size.width;
    g_dynablc_venc_chn_param.venc_size.height = in_size.height;
    g_dynablc_venc_chn_param.size = sample_comm_sys_get_pic_enum(&in_size);

    for (i = 0; i < (hi_s32)chn_num; i++) {
        ret = sample_comm_venc_start(venc_chn[i], &g_dynablc_venc_chn_param);
        if (ret != HI_SUCCESS) {
            goto exit;
        }
    }

    ret = sample_comm_venc_start_get_stream(venc_chn, chn_num);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    return HI_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        sample_comm_venc_stop(venc_chn[i]);
    }
    return HI_FAILURE;
}

static hi_void sample_dynablc_vio_stop_vo(hi_void)
{
    sample_comm_vo_stop_vo(&g_dynablc_vo_cfg);
}

static hi_s32 sample_dynablc_vio_start_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num)
{
    hi_u32 i;
    hi_s32 ret;
    sample_vo_mode vo_mode = VO_MODE_1MUX;
    const hi_vpss_chn vpss_chn = 0;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    if (grp_num > 1) {
        vo_mode = VO_MODE_4MUX;
    }

    ret = sample_dynablc_vio_start_vo(vo_mode);
    if (ret != HI_SUCCESS) {
        goto start_vo_failed;
    }

    ret = sample_dynablc_vio_start_venc(venc_chn, sizeof(venc_chn) / sizeof(venc_chn[0]), grp_num);
    if (ret != HI_SUCCESS) {
        goto start_venc_failed;
    }

    for (i = 0; i < grp_num; i++) {
        sample_comm_vpss_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
        sample_comm_vpss_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
    }
    return HI_SUCCESS;

start_venc_failed:
    sample_dynablc_vio_stop_vo();
start_vo_failed:
    return HI_FAILURE;
}

static hi_void sample_dynablc_vio_stop_venc(hi_venc_chn venc_chn[], hi_u32 chn_num)
{
    hi_u32 i;

    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static hi_void sample_dynablc_vio_stop_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num)
{
    hi_u32 i;
    const hi_vpss_chn vpss_chn = 0;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    for (i = 0; i < grp_num; i++) {
        sample_comm_vpss_un_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
        sample_comm_vpss_un_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
    }

    sample_dynablc_vio_stop_venc(venc_chn, grp_num);
    sample_dynablc_vio_stop_vo();
}

static hi_void sample_dynablc_vio_stop_vpss(hi_vpss_grp grp)
{
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_FALSE, HI_FALSE, HI_FALSE};

    sample_common_vpss_stop(grp, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static hi_s32 check_light_stt_area(hi_vi_pipe vi_pipe, hi_rect *light_area)
{
    hi_isp_black_level_attr black_level_attr;
    hi_s32 ret;

    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("get ob_attr failed!!!\n");
    }

    sample_sns_type sns_type = SENSOR0_TYPE;
    hi_size in_size;
    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);
    if (light_area->x > (hi_s32)in_size.width) {
        printf("set x coordinates of the light statistical region failed !!!\n");
        return HI_FAILURE;
    }

    if ((light_area->y > (hi_s32)in_size.height) ||
        ((hi_u32)light_area->y < black_level_attr.dynamic_attr.ob_area.height)) {
        printf("set y of the light region failed, y:(ob_heigt, img_height), light_area->y:%d, ob_area.height:%u\n",
            light_area->y, black_level_attr.dynamic_attr.ob_area.height);
        return HI_FAILURE;
    }

    if ((light_area->x + light_area->width) > in_size.width) {
        printf("set width of the light statistical region failed !!!\n");
        return HI_FAILURE;
    }

    if ((light_area->y + light_area->height) > in_size.height) {
        printf("set height of the light statistical region failed !!!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_dynablc_cali(hi_vi_pipe vi_pipe, hi_u32 frame_cnt, hi_u32 iso_num, hi_rect *light_area)
{
    hi_s32 ret = HI_SUCCESS;

    if (g_sig_flag == 0) {
        printf("\033[0;32mprogram cannot be terminated using Ctrl + C!\033[0;39m\n");
    }

    ret = check_light_stt_area(vi_pipe, light_area);
    if (ret != HI_SUCCESS) {
        printf("set attr of the light statistical region failed !!!\n");
        return ret;
    }

    ret = sample_dynamic_blc_run(vi_pipe, frame_cnt, iso_num, light_area);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return ret;
}

static hi_s32 sample_dynablc_vio_all_mode(hi_vi_pipe vi_pipe, hi_u32 frame_cnt, hi_u32 iso_num, hi_rect *light_area)
{
    hi_s32 ret;
    hi_u32 yuv_cnt, raw_cnt;
    hi_vi_vpss_mode_type mode_type = HI_VI_OFFLINE_VPSS_OFFLINE;
    hi_vi_aiisp_mode aiisp_mode = HI_VI_AIISP_MODE_DEFAULT;
    const hi_vi_chn vi_chn = 0;
    hi_vpss_grp vpss_grp[1] = {0};
    const hi_u32 grp_num = 1;
    const hi_vpss_chn vpss_chn = 0;
    sample_vi_cfg vi_cfg;
    sample_sns_type sns_type;
    hi_size in_size;

    yuv_cnt = VB_YUV_ROUTE_CNT;
    raw_cnt = VB_LINEAR_RAW_CNT;

    ret = sample_dynablc_vio_sys_init(mode_type, aiisp_mode, yuv_cnt, raw_cnt);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sns_type = SENSOR0_TYPE;
    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);
    sample_comm_vi_get_default_vi_cfg(sns_type, &vi_cfg);
    ret = sample_comm_vi_start_vi(&vi_cfg);
    if (ret != HI_SUCCESS) {
        goto start_vi_failed;
    }

    sample_comm_vi_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], vpss_chn);

    ret = sample_dynablc_vio_start_vpss(vpss_grp[0], &in_size);
    if (ret != HI_SUCCESS) {
        goto start_vpss_failed;
    }

    ret = sample_dynablc_vio_start_venc_and_vo(vpss_grp, grp_num);
    if (ret != HI_SUCCESS) {
        goto start_venc_and_vo_failed;
    }

    ret = sample_dynablc_cali(vi_pipe, frame_cnt, iso_num, light_area);
    if (ret != HI_SUCCESS) {
        goto dyncblc_cali_failed;
    }

    sample_dynablc_get_char();

dyncblc_cali_failed:
    sample_dynablc_vio_stop_venc_and_vo(vpss_grp, grp_num);
start_venc_and_vo_failed:
    sample_dynablc_vio_stop_vpss(vpss_grp[0]);
start_vpss_failed:
    sample_comm_vi_un_bind_vpss(vi_pipe, vi_chn, vpss_grp[0], vpss_chn);
    sample_comm_vi_stop_vi(&vi_cfg);
start_vi_failed:
    sample_comm_sys_exit();
    return ret;
}

static hi_s32 get_arg_value(char *argv[], hi_vi_pipe *vi_pipe, hi_u32 *frame_cnt,
    hi_u32 *iso_num, hi_rect *light_stt_area)
{
    const hi_s32 base = 10; /* 10:Decimal */
    hi_char *end_ptr = HI_NULL;

    errno = 0;
    *vi_pipe = (hi_s32)strtol(argv[1], &end_ptr, base);
    if ((errno != 0) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }

    errno = 0;
    *frame_cnt = (hi_u32)strtol(argv[2], &end_ptr, base); /* 2: frame_cnt value of dynamic blc offset calibration */
    if ((errno != 0) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }

    errno = 0;
    *iso_num = (hi_u32)strtol(argv[3], &end_ptr, base); /* 3: iso_num value of dynamic blc offset calibration */
    if ((errno != 0) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }

    errno = 0;
    light_stt_area->x = (hi_s32)strtol(argv[4], &end_ptr, base); /* 4: X coordinates of the light statistical region */
    if ((errno != 0) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }

    errno = 0;
    light_stt_area->y = (hi_s32)strtol(argv[5], &end_ptr, base); /* 5: Y coordinates of the light statistical region */
    if ((errno != 0) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }

    errno = 0;
    light_stt_area->width = (hi_u32)strtol(argv[6], &end_ptr, base); /* 6: width of the light statistical region */
    if ((errno != 0) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }

    errno = 0;
    light_stt_area->height = (hi_u32)strtol(argv[7], &end_ptr, base); /* 7: height of the light statistical region */
    if ((errno != 0) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 check_input_value(hi_u32 frame_cnt,
    hi_u32 iso_num, hi_rect *light_stt_area)
{
    sample_sns_type sns_type = SENSOR0_TYPE;
    ot_size in_size;
    sample_comm_vi_get_size_by_sns_type(sns_type, &in_size);

    if ((frame_cnt > HI_ISP_DYNAMIC_BLC_CALI_MAX_FRAME) || (frame_cnt == 0)) {
        printf("can't not support frame_cnt %u, can choose only from 1~30!\n", frame_cnt);
        return HI_FAILURE;
    }

    if ((iso_num > HI_ISP_DYNAMIC_BLC_CALI_MAX_ISO_NUM) || (iso_num == 0)) {
        printf("can't not support iso_num %u, can choose only from 1~16!\n", iso_num);
        return HI_FAILURE;
    }

    if (light_stt_area->x < 0 || (hi_u32)light_stt_area->x > in_size.width) {
        printf("can't not support x %d, can choose only from 0~sensor_width!\n", light_stt_area->x);
        return HI_FAILURE;
    }

    if (light_stt_area->y < 0 || (hi_u32)light_stt_area->y > in_size.height) {
        printf("can't not support y %d, can choose only from 0~sensor_height!\n", light_stt_area->y);
        return HI_FAILURE;
    }

    if ((light_stt_area->width > (in_size.width - light_stt_area->x)) || (light_stt_area->width <= 0)) {
        printf("can't not support width %u, can choose only from 1~(image_width - x)!\n", light_stt_area->width);
        return HI_FAILURE;
    }

    if ((light_stt_area->height > (in_size.height - light_stt_area->y)) || (light_stt_area->height <= 0)) {
        printf("can't not support height %u, can choose only from 1~(image_height - y)!\n", light_stt_area->height);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_vio_handle_sig(hi_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_sig_flag = 1;
    }
}

static hi_void sample_register_sig_handler(hi_void (*sig_handle)(hi_s32))
{
    struct sigaction sa;

    (hi_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sig_handle;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, HI_NULL);
    sigaction(SIGTERM, &sa, HI_NULL);
}

hi_s32 main(int argc, char *argv[])
{
    hi_vi_pipe vi_pipe;
    hi_s32 ret;
    hi_u32 iso_num;
    hi_u32 frame_cnt;
    hi_rect light_stt_area;

    printf("\n_notice: this tool can only be used for TEST, to see more usage,"
           "enter: ./sample_dynamic_blc_online_cali -h\n");

    if (argc != 8) { /* 8 args */
        goto print_usage;
    }

    ret = get_arg_value(argv, &vi_pipe, &frame_cnt, &iso_num, &light_stt_area);
    if (ret != HI_SUCCESS) {
        goto print_usage;
    }

    if ((vi_pipe < 0) || (vi_pipe >= HI_ISP_MAX_PHY_PIPE_NUM)) {
        isp_err_trace("err vi_pipe %d!\n", vi_pipe);
        return HI_FAILURE;
    }

    ret = check_input_value(frame_cnt, iso_num, &light_stt_area);
    if (ret != HI_SUCCESS) {
        goto print_usage;
    }

    sample_register_sig_handler(sample_vio_handle_sig);

    ret = sample_dynablc_vio_all_mode(vi_pipe, frame_cnt, iso_num, &light_stt_area);
    if ((ret == HI_SUCCESS) && (g_sig_flag == 0)) {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
        return ret;
    }

    return ret;

print_usage:
    usage();
    return HI_FAILURE;
}

