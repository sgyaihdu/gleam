/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include "media_vdec.h"
#include <unistd.h>

hi_vb_src g_vdec_vb_source = HI_VB_SRC_MOD;

#define REF_NUM 2
#define DISPLAY_NUM 2

static vdec_display_cfg g_vdec_display_cfg = {
    .pic_size = PIC_1080P,
    .intf_sync = HI_VO_OUT_1080P60,
    .intf_type = HI_VO_INTF_BT1120,
};

static hi_u32 g_input_width;
static hi_u32 g_input_height;
static hi_size g_disp_size;
sample_vo_cfg g_vo_config;
static hi_bool g_is_need_vdec = HI_TRUE;

static hi_s32 sample_uvc_init_module_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type,
    hi_u32 len)
{
    hi_u32 i;
    hi_s32 ret;
    for (i = 0; (i < vdec_chn_num) && (i < len); i++) {
        sample_vdec[i].type                           = type;
        sample_vdec[i].width                         = g_input_width;
        sample_vdec[i].height                        = g_input_height;
        sample_vdec[i].mode                           = sample_comm_vdec_get_lowdelay_en() ? HI_VDEC_SEND_MODE_COMPAT :
                                                        HI_VDEC_SEND_MODE_FRAME;
        sample_vdec[i].sample_vdec_video.dec_mode      = HI_VIDEO_DEC_MODE_IP;
        sample_vdec[i].sample_vdec_video.bit_width     = HI_DATA_BIT_WIDTH_8;
        if (type == HI_PT_JPEG) {
            sample_vdec[i].sample_vdec_video.ref_frame_num = 0;
        } else {
            sample_vdec[i].sample_vdec_video.ref_frame_num = REF_NUM;
        }
        sample_vdec[i].display_frame_num               = DISPLAY_NUM;
        sample_vdec[i].frame_buf_cnt = (type == HI_PT_JPEG) ? (sample_vdec[i].display_frame_num + 1) :
            (sample_vdec[i].sample_vdec_video.ref_frame_num + sample_vdec[i].display_frame_num + 1);
        if (type == HI_PT_JPEG) {
            sample_vdec[i].sample_vdec_picture.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
            sample_vdec[i].sample_vdec_picture.alpha      = 255; /* 255:pic alpha value */
        }
    }
    ret = sample_comm_vdec_init_vb_pool(vdec_chn_num, &sample_vdec[0], len);
    if (ret != HI_SUCCESS) {
        sample_print("init mod common vb fail for %#x!\n", ret);
        return ret;
    }
    return ret;
}

static hi_s32 sample_uvc_init_sys_and_vb(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_payload_type type,
    hi_u32 len)
{
    hi_vb_cfg vb_cfg;
    hi_pic_buf_attr buf_attr = {0};
    hi_s32 ret;

    ret = sample_comm_sys_get_pic_size(g_vdec_display_cfg.pic_size, &g_disp_size);
    if (ret != HI_SUCCESS) {
        sample_print("sys get pic size fail for %#x!\n", ret);
        return ret;
    }
    buf_attr.align = 0;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.compress_mode = HI_COMPRESS_MODE_SEG;
    buf_attr.height = 2160; /* 2160:height */
    buf_attr.width = 3840;  /* 3840:width */
    buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    (hi_void)memset_s(&vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg.max_pool_cnt             = 1;
    vb_cfg.common_pool[0].blk_cnt  = 10 * vdec_chn_num; /* 10:common vb cnt */
    vb_cfg.common_pool[0].blk_size = hi_common_get_pic_buf_size(&buf_attr);
    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, OT_VB_SUPPLEMENT_BNR_MOT_MASK);
    if (ret != HI_SUCCESS) {
        sample_print("init sys fail for %#x!\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    if (g_is_need_vdec == HI_TRUE) {
        ret = sample_uvc_init_module_vb(&sample_vdec[0], vdec_chn_num, type, len);
        if (ret != HI_SUCCESS) {
            sample_print("init mod vb fail for %#x!\n", ret);
            sample_comm_vdec_exit_vb_pool();
            sample_comm_sys_exit();
            return ret;
        }
    }

    return ret;
}

static hi_s32 sample_uvc_vdec_bind_vpss(hi_u32 vpss_grp_num)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vdec_bind_vpss(i, i);
        if (ret != HI_SUCCESS) {
            sample_print("vdec bind vpss fail for %#x!\n", ret);
            return ret;
        }
    }
    return ret;
}

static hi_void sample_uvc_stop_vpss(hi_vpss_grp vpss_grp, hi_bool *vpss_chn_enable, hi_u32 chn_array_size)
{
    hi_s32 i;
    for (i = vpss_grp; i >= 0; i--) {
        vpss_grp = i;
        sample_common_vpss_stop(vpss_grp, &vpss_chn_enable[0], chn_array_size);
    }
}

static hi_s32 sample_uvc_vdec_unbind_vpss(hi_u32 vpss_grp_num)
{
    hi_u32 i;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vdec_un_bind_vpss(i, i);
        if (ret != HI_SUCCESS) {
            sample_print("vdec unbind vpss fail for %#x!\n", ret);
        }
    }
    return ret;
}

static hi_void sample_uvc_config_vpss_grp_attr(hi_vpss_grp_attr *vpss_grp_attr)
{
    vpss_grp_attr->max_width = g_input_width;
    vpss_grp_attr->max_height = g_input_height;
    vpss_grp_attr->frame_rate.src_frame_rate = -1;
    vpss_grp_attr->frame_rate.dst_frame_rate = -1;
    vpss_grp_attr->pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vpss_grp_attr->ie_en   = HI_FALSE;
    vpss_grp_attr->dci_en   = HI_FALSE;
    vpss_grp_attr->dei_mode = HI_VPSS_DEI_MODE_OFF;
    vpss_grp_attr->buf_share_en   = HI_FALSE;
}

static hi_void sample_uvc_stop_vdec(hi_u32 vdec_chn_num)
{
    if (g_is_need_vdec == HI_FALSE) {
        return;
    }

    sample_comm_vdec_stop(vdec_chn_num);
    sample_comm_vdec_exit_vb_pool();

    return;
}

static hi_s32 sample_uvc_start_vdec(sample_vdec_attr *sample_vdec, hi_u32 vdec_chn_num, hi_u32 len)
{
    hi_s32 ret;

    if (g_is_need_vdec == HI_FALSE) {
        return HI_SUCCESS;
    }

    ret = sample_comm_vdec_start(vdec_chn_num, &sample_vdec[0], len);
    if (ret != HI_SUCCESS) {
        sample_print("start VDEC fail for %#x!\n", ret);
        sample_uvc_stop_vdec(vdec_chn_num);
    }

    return ret;
}

static hi_s32 sample_uvc_config_vpss_ldy_attr(hi_u32 vpss_grp_num)
{
    hi_u32 i;
    hi_s32 ret;
    hi_low_delay_info vpss_ldy_info;

    for (i = 0; i < vpss_grp_num; i++) {
        ret = hi_mpi_vpss_get_chn_low_delay(i, 0, &vpss_ldy_info);
        if (ret != HI_SUCCESS) {
            sample_print("vpss get low delay attr fail for %#x!\n", ret);
            return ret;
        }
        vpss_ldy_info.enable = HI_TRUE;
        vpss_ldy_info.line_cnt = g_disp_size.height / 4 * 1; /* 1/4:lowdelay line num */
        ret = hi_mpi_vpss_set_chn_low_delay(i, 0, &vpss_ldy_info);
        if (ret != HI_SUCCESS) {
            sample_print("vpss set low delay attr fail for %#x!\n", ret);
            return ret;
        }
    }
    return HI_SUCCESS;
}

static hi_s32 sample_uvc_start_vpss(hi_vpss_grp *vpss_grp, hi_u32 vpss_grp_num,
    hi_bool *vpss_chn_enable, hi_u32 arr_len)
{
    hi_u32 i;
    hi_s32 ret;
    sample_vpss_chn_attr vpss_chn_attr = {0};
    hi_vpss_grp_attr vpss_grp_attr = {0};
    sample_uvc_config_vpss_grp_attr(&vpss_grp_attr);
    (hi_void)memset_s(vpss_chn_enable, arr_len * sizeof(hi_bool), 0, arr_len * sizeof(hi_bool));
    if (arr_len > 1) {
        vpss_chn_enable[0] = HI_TRUE;
        vpss_chn_attr.chn_attr[0].width                     = g_disp_size.width;
        vpss_chn_attr.chn_attr[0].height                    = g_disp_size.height;
        vpss_chn_attr.chn_attr[0].chn_mode                  = HI_VPSS_CHN_MODE_USER;
        vpss_chn_attr.chn_attr[0].compress_mode             = HI_COMPRESS_MODE_NONE;
        vpss_chn_attr.chn_attr[0].pixel_format              = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
        vpss_chn_attr.chn_attr[0].frame_rate.src_frame_rate = -1;
        vpss_chn_attr.chn_attr[0].frame_rate.dst_frame_rate = -1;
        vpss_chn_attr.chn_attr[0].depth                     = 0;
        vpss_chn_attr.chn_attr[0].mirror_en                 = HI_FALSE;
        vpss_chn_attr.chn_attr[0].flip_en                   = HI_FALSE;
        vpss_chn_attr.chn_attr[0].border_en                 = HI_FALSE;
        vpss_chn_attr.chn_attr[0].aspect_ratio.mode         = HI_ASPECT_RATIO_NONE;
    }

    memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
        vpss_chn_enable, sizeof(vpss_chn_attr.chn_enable));
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_CHN_NUM;
    for (i = 0; i < vpss_grp_num; i++) {
        *vpss_grp = i;
        ret = sample_common_vpss_start(*vpss_grp, &vpss_grp_attr, &vpss_chn_attr);
        if (ret != HI_SUCCESS) {
            sample_print("start VPSS fail for %#x!\n", ret);
            sample_uvc_stop_vpss(*vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
            return ret;
        }
    }

    ret = sample_uvc_config_vpss_ldy_attr(vpss_grp_num);
    if (ret != HI_SUCCESS) {
        sample_uvc_stop_vpss(*vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
        return ret;
    }

    ret = sample_uvc_vdec_bind_vpss(vpss_grp_num);
    if (ret != HI_SUCCESS) {
        sample_uvc_vdec_unbind_vpss(vpss_grp_num);
        sample_uvc_stop_vpss(*vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    }
    return ret;
}

static hi_s32 sample_uvc_vpss_bind_vo(const sample_vo_cfg *vo_config, hi_u32 vpss_grp_num)
{
    hi_u32 i;
    hi_vo_layer vo_layer;
    hi_s32 ret = HI_SUCCESS;
    vo_layer = vo_config->vo_dev;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vpss_bind_vo(i, 0, vo_layer, i);
        if (ret != HI_SUCCESS) {
            sample_print("vpss bind vo fail for %#x!\n", ret);
            return ret;
        }
    }
    return ret;
}

static hi_s32 sample_uvc_vpss_unbind_vo(hi_u32 vpss_grp_num, const sample_vo_cfg *vo_config)
{
    hi_u32 i;
    hi_vo_layer vo_layer = vo_config->vo_dev;
    hi_s32 ret = HI_SUCCESS;
    for (i = 0; i < vpss_grp_num; i++) {
        ret = sample_comm_vpss_un_bind_vo(i, 0, vo_layer, i);
        if (ret != HI_SUCCESS) {
            sample_print("vpss unbind vo fail for %#x!\n", ret);
        }
    }
    return ret;
}

static hi_void sample_uvc_get_default_vo_cfg(sample_vo_cfg *vo_config)
{
    vo_config->vo_dev            = SAMPLE_VO_DEV_UHD;
    vo_config->vo_intf_type      = g_vdec_display_cfg.intf_type;
    vo_config->intf_sync         = g_vdec_display_cfg.intf_sync;
    vo_config->pic_size          = g_vdec_display_cfg.pic_size;
    vo_config->bg_color          = COLOR_RGB_BLUE;
    vo_config->dis_buf_len       = 3; /* 3:buf length */
    vo_config->dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    vo_config->vo_mode           = VO_MODE_1MUX;
    vo_config->pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vo_config->disp_rect.x       = 0;
    vo_config->disp_rect.y       = 0;
    vo_config->disp_rect.width   = g_disp_size.width;
    vo_config->disp_rect.height  = g_disp_size.height;
    vo_config->image_size.width  = g_disp_size.width;
    vo_config->image_size.height = g_disp_size.height;
    vo_config->vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE;
    vo_config->compress_mode     = HI_COMPRESS_MODE_NONE;
}

static hi_s32 sample_uvc_start_vo(sample_vo_cfg *vo_config, hi_u32 vpss_grp_num)
{
    hi_s32 ret;

    sample_uvc_get_default_vo_cfg(vo_config);

    ret = sample_comm_vo_start_vo(vo_config);
    if (ret != HI_SUCCESS) {
        sample_print("start VO fail for %#x!\n", ret);
        sample_comm_vo_stop_vo(vo_config);
        return ret;
    }

    ret = sample_uvc_vpss_bind_vo(vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        sample_uvc_vpss_unbind_vo(vpss_grp_num, vo_config);
        sample_comm_vo_stop_vo(vo_config);
    }

    return ret;
}

static hi_payload_type sample_uvc_get_payload_type(const hi_char *type_name)
{
    if (strcmp(type_name, "H264") == 0) {
        return HI_PT_H264;
    } else if (strcmp(type_name, "H265") == 0) {
        return HI_PT_H265;
    } else if (strcmp(type_name, "MJPEG") == 0) {
        return HI_PT_JPEG;
    } else {
        sample_print("type name error!\n");
        return HI_PT_BUTT;
    }
}

static hi_void sample_uvc_update_vdec_flag(const hi_char *type_name)
{
    if (type_name == HI_NULL) {
        g_is_need_vdec = HI_FALSE;
        return;
    }
    if ((strcmp(type_name, "H264") == 0) ||
        (strcmp(type_name, "H265") == 0) ||
        (strcmp(type_name, "MJPEG") == 0)) {
        g_is_need_vdec = HI_TRUE;
    } else {
        g_is_need_vdec = HI_FALSE;
    }
}

hi_s32 sample_uvc_media_init(const hi_char *type_name, hi_u32 width, hi_u32 height)
{
    hi_s32 ret;
    hi_u32 vdec_chn_num, vpss_grp_num;
    sample_vdec_attr sample_vdec[HI_VDEC_MAX_CHN_NUM];
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM];
    hi_vpss_grp vpss_grp;
    hi_payload_type payload_type = HI_PT_H264;

    vdec_chn_num = 1;
    vpss_grp_num = vdec_chn_num;

    g_input_width = width;
    g_input_height = height;

    sample_uvc_update_vdec_flag(type_name);

    if (g_is_need_vdec == HI_TRUE) {
        payload_type = sample_uvc_get_payload_type(type_name);
        if (payload_type == OT_PT_BUTT) {
            return HI_FAILURE;
        }
    }

    ret = sample_uvc_init_sys_and_vb(&sample_vdec[0], vdec_chn_num, payload_type, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = sample_uvc_start_vdec(&sample_vdec[0], vdec_chn_num, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_sys;
    }

    ret = sample_uvc_start_vpss(&vpss_grp, vpss_grp_num, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        goto stop_vdec;
    }

    ret = sample_uvc_start_vo(&g_vo_config, vpss_grp_num);
    if (ret != HI_SUCCESS) {
        goto stop_vpss;
    }

    return HI_SUCCESS;

stop_vpss:
    ret = sample_uvc_vdec_unbind_vpss(vpss_grp_num);
    sample_uvc_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
stop_vdec:
    sample_uvc_stop_vdec(vdec_chn_num);
stop_sys:
    sample_comm_sys_exit();

    return HI_FAILURE;
}

hi_s32 sample_uvc_media_exit(hi_void)
{
    hi_u32 vdec_chn_num = 1;
    hi_u32 vpss_grp_num = 1;
    hi_vpss_grp vpss_grp = 0;
    hi_bool vpss_chn_enable[HI_VPSS_MAX_CHN_NUM] = {0};
    vpss_chn_enable[0] = HI_TRUE;

    (hi_void)sample_uvc_vpss_unbind_vo(vpss_grp_num, &g_vo_config);
    sample_comm_vo_stop_vo(&g_vo_config);
    (hi_void)sample_uvc_vdec_unbind_vpss(vpss_grp_num);
    sample_uvc_stop_vpss(vpss_grp, &vpss_chn_enable[0], HI_VPSS_MAX_CHN_NUM);
    sample_uvc_stop_vdec(vdec_chn_num);
    sample_comm_sys_exit();

    return HI_SUCCESS;
}

static hi_void sample_uvc_cut_stream_for_mjpeg(hi_void *data, hi_u32 size, hi_s32 chn_id,
    hi_s32 *read_num, hi_u32 *start)
{
    hi_s32 i;
    hi_u32 len;
    hi_s32 read_len = size;
    hi_u8 *buf = data;
    hi_bool find_start = HI_FALSE;

    for (i = 0; i < read_len - 1; i++) {
        if (buf[i] == 0xFF && buf[i + 1] == 0xD8) {
            *start = i;
            find_start = HI_TRUE;
            i = i + 2; /* 2 mjpeg data structure index */
            break;
        }
    }

    for (; i < read_len - 3; i++) { /* 2 3 mjpeg data structure limit */
        if ((buf[i] == 0xFF) && (buf[i + 1] & 0xF0) == 0xE0) {
            len = (buf[i + 2] << 8) + buf[i + 3]; /* 2 3 mjpeg data structure index, 8 high byte in integer */
            i += 1 + len;
        } else {
            break;
        }
    }

    for (; i < read_len - 1; i++) {
        if (buf[i] == 0xFF && buf[i + 1] == 0xD9) {
            break;
        }
    }
    read_len = i + 2; /* 2 mjpeg data structure index */

    if (find_start == HI_FALSE) {
        sample_print("chn %d can not find JPEG start code! read_len %d!\n", chn_id, read_len);
    }

    *read_num = read_len;
}

static hi_void sample_uvc_cut_stream_for_h264(hi_void *data, hi_u32 size, hi_s32 chn_id, hi_s32 *read_num)
{
    hi_bool find_start = HI_FALSE;
    hi_bool find_end = HI_FALSE;
    hi_s32 i;
    hi_u8 *buf = data;
    hi_s32 read_len = size;
    const int min_block_len = 8;

    for (i = 0; i < read_len - min_block_len; i++) {
        int tmp = buf[i + 3] & 0x1F;  /* byte 3 contains NALU type */
        if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && /* index 0 1 2 for start code */
            (((tmp == 0x5 || tmp == 0x1) && ((buf[i + 4] & 0x80) == 0x80)) || /* 5 i-frame, index 4 for stream info */
            (tmp == 20 && (buf[i + 7] & 0x80) == 0x80))) { /* 20 nalue type, index 7 for type info */
            find_start = HI_TRUE;
            i += min_block_len;
            break;
        }
    }

    for (; i < read_len - min_block_len; i++) {
        int tmp = buf[i + 3] & 0x1F;  /* byte 3 contains NALU type */
        if (buf[i] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 &&  /* index 0 1 2 for start code */
            (tmp == 15 || tmp == 7 || tmp == 8 || tmp == 6 || /* 6 SEI, 7 SPS, 8 PPS, 15 reserve NALU type */
            ((tmp == 5 || tmp == 1) && ((buf[i + 4] & 0x80) == 0x80)) || /* 5 i-frame, index 4 for stream info */
            (tmp == 20 && (buf[i + 7] & 0x80) == 0x80))) { /* 20 nalue type, index 7 for type info */
            find_end = HI_TRUE;
            break;
        }
    }

    if (i > 0) {
        read_len = i;
    }

    if (find_start == HI_FALSE) {
        sample_print("chn %d can not find H264 start code! read_len %d!\n", chn_id, read_len);
    }

    if (find_end == HI_FALSE) {
        read_len = i + min_block_len;
    }

    *read_num = read_len;
}

static hi_void sample_uvc_cut_stream_for_h265(hi_void *data, hi_u32 size, hi_s32 chn_id, hi_s32 *read_num)
{
    hi_bool find_start = HI_FALSE;
    hi_bool find_end = HI_FALSE;
    hi_bool new_pic = HI_FALSE;
    hi_s32 i;
    hi_u8 *buf = data;
    hi_s32 read_len = size;

    for (i = 0; i < read_len - 6; i++) { /* 6:h265 frame start code length */
        hi_u32 tmp = (buf[i + 3] & 0x7E) >> 1; /* 0x7E:frame start marker 3:index */
        new_pic = (buf[i + 0] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && /* 1 2:index */
            (tmp <= 21) && ((buf[i + 5] & 0x80) == 0x80)); /* 5:index 21 0x80:frame start marker */

        if (new_pic) {
            find_start = HI_TRUE;
            i += 6; /* 6:h265 frame start code length */
            break;
        }
    }

    for (; i < read_len - 6; i++) { /* 6:h265 frame start code length */
        hi_u32 tmp = (buf[i + 3] & 0x7E) >> 1; /* 0x7E:frame start marker 3:index */
        new_pic = (buf[i + 0] == 0 && buf[i + 1] == 0 && buf[i + 2] == 1 && /* 1 2:index */
            (tmp == 32 || tmp == 33 || tmp == 34 || tmp == 39 || tmp == 40 || /* 32 33 34 39 40:frame start marker */
            ((tmp <= 21) && (buf[i + 5] & 0x80) == 0x80))); /* 5:index 21 0x80:frame start marker */

        if (new_pic) {
            find_end = HI_TRUE;
            break;
        }
    }

    if (i > 0) {
        read_len = i;
    }

    if (find_start == HI_FALSE) {
        sample_print("chn %d can not find H265 start code! read_len %d!\n", chn_id, read_len);
    }

    if (find_end == HI_FALSE) {
        read_len = i + 6; /* 6:h265 frame start code length */
    }

    *read_num = read_len;
}

static hi_s32 sample_uvc_prepare_frame_info(hi_vb_blk vb_blk, const hi_pic_buf_attr *buf_attr,
    const hi_vb_calc_cfg *calc_cfg, hi_video_frame_info *video_frame)
{
    video_frame->video_frame.header_phys_addr[0] = hi_mpi_vb_handle_to_phys_addr(vb_blk);
    if (video_frame->video_frame.header_phys_addr[0] == HI_NULL) {
        sample_print("hi_mpi_vb_handle_to_phys_addr fail\n");
        return HI_FAILURE;
    }

    video_frame->video_frame.header_virt_addr[0] =
        (hi_u8*)hi_mpi_sys_mmap(video_frame->video_frame.header_phys_addr[0], calc_cfg->vb_size);
    if (video_frame->video_frame.header_virt_addr[0] == HI_NULL) {
        sample_print("hi_mpi_sys_mmap fail\n");
        return HI_FAILURE;
    }

    video_frame->mod_id = HI_ID_VGS;
    video_frame->pool_id = hi_mpi_vb_handle_to_pool_id(vb_blk);

    video_frame->video_frame.header_phys_addr[1] =
        video_frame->video_frame.header_phys_addr[0] + calc_cfg->head_y_size;
    video_frame->video_frame.header_virt_addr[1] =
        video_frame->video_frame.header_virt_addr[0] + calc_cfg->head_y_size;
    video_frame->video_frame.phys_addr[0] =
        video_frame->video_frame.header_phys_addr[0] + calc_cfg->head_size;
    video_frame->video_frame.phys_addr[1] =
        video_frame->video_frame.phys_addr[0] + calc_cfg->main_y_size;
    video_frame->video_frame.virt_addr[0] =
        video_frame->video_frame.header_virt_addr[0] + calc_cfg->head_size;
    video_frame->video_frame.virt_addr[1] =
        video_frame->video_frame.virt_addr[0] + calc_cfg->main_y_size;
    video_frame->video_frame.header_stride[0] = calc_cfg->head_stride;
    video_frame->video_frame.header_stride[1] = calc_cfg->head_stride;
    video_frame->video_frame.stride[0] = calc_cfg->main_stride;
    video_frame->video_frame.stride[1] = calc_cfg->main_stride;

    video_frame->video_frame.width = buf_attr->width;
    video_frame->video_frame.height = buf_attr->height;
    video_frame->video_frame.dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    video_frame->video_frame.compress_mode = HI_COMPRESS_MODE_NONE;
    video_frame->video_frame.video_format = HI_VIDEO_FORMAT_LINEAR;
    video_frame->video_frame.field = HI_VIDEO_FIELD_FRAME;
    video_frame->video_frame.pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;

    return HI_SUCCESS;
}

static hi_void sample_uvc_buf_attr_init(const hi_size *pic_size, hi_pic_buf_attr *buf_attr)
{
    buf_attr->width = pic_size->width;
    buf_attr->height = pic_size->height;
    buf_attr->pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    buf_attr->compress_mode = HI_COMPRESS_MODE_NONE;
    buf_attr->align = 0;
    buf_attr->bit_width = HI_DATA_BIT_WIDTH_8;
}

static hi_void sample_uvc_yuyv_to_nv12(hi_char *image_in, hi_u32 width, hi_u32 height, hi_u32 size, hi_char *image_out)
{
    hi_u32 pixel_num = width * height;
    hi_u32 cycle_num = size / pixel_num / 2; /* every pixel have 2 bytes in YUYV */

    hi_char *y = image_out;
    hi_char *uv = image_out + pixel_num;

    hi_char *start = image_in;
    hi_u32 i = 0, j = 0, k = 0;

    /* Y */
    for (i = 0; i < cycle_num; i++) {
        int index = 0;
        for (j = 0; j < pixel_num * 2; j = j + 2) { /* 2 bytes have 1 Y. (Y U Y V ...) */
            *(y + index) = *(start + j);
            index++;
        }
        start = image_in + pixel_num * 2 * i;   /* every pixel have 2 bytes in YUYV */
        y = y + pixel_num * 3 / 2;              /* 3 / 2 : every pixel 1.5 bytes  for NV12 */
    }

    /* UV */
    start = image_in;
    for (i = 0; i < cycle_num; i++) {
        int uv_index = 0;
        for (j = 0; j < height; j = j + 2) {  /* 2 : even lines have UV for YUYV */
            for (k = j * width * 2 + 1; k < width * 2 * (j + 1); k = k + 4) { /* 4 bytes has a UV, 2 for step */
                *(uv + uv_index) = *(start + k);
                *(uv + uv_index + 1) = *(start + k + 2); /* 2 is step length from U to V */
                uv_index += 2;  /* 2 bytes UV for NV12(U V U V...) */
            }
        }
        start = image_in + pixel_num * 2 * i;  /* every pixel have 2 bytes in YUYV */
        uv = uv + pixel_num * 3 / 2;  /* 3 / 2 : every pixel 1.5 bytes  for NV12 */
    }
}

static hi_void sample_uvc_update_vb_cfg(hi_pixel_format pixel_format, hi_u32 stride, const hi_size *pic_size,
    hi_vb_calc_cfg *calc_cfg)
{
    if (pixel_format != HI_PIXEL_FORMAT_VY1UY0_PACKAGE_422) {
        return;
    }

    calc_cfg->main_stride = stride >> 1;
    calc_cfg->main_y_size = calc_cfg->main_stride * pic_size->height;
}

static hi_s32 sample_uvc_send_frame_to_vpss(hi_void *data, hi_u32 size, hi_u32 stride,
    const hi_size *pic_size, hi_pixel_format pixel_format)
{
    hi_s32 ret;
    hi_vpss_grp grp = 0;
    hi_video_frame_info frame_info = {0};
    hi_s32 milli_sec = -1;
    hi_vb_blk vb_blk;
    hi_pic_buf_attr buf_attr = {0};
    hi_vb_calc_cfg calc_cfg = {0};

    sample_uvc_buf_attr_init(pic_size, &buf_attr);
    hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);
    sample_uvc_update_vb_cfg(pixel_format, stride, pic_size, &calc_cfg);

    vb_blk = hi_mpi_vb_get_blk(HI_VB_INVALID_POOL_ID, calc_cfg.vb_size, HI_NULL);
    if (vb_blk == HI_VB_INVALID_HANDLE) {
        sample_print("get vb blk(size:%u) failed!\n", calc_cfg.vb_size);
        return HI_FAILURE;
    }

    ret = sample_uvc_prepare_frame_info(vb_blk, &buf_attr, &calc_cfg, &frame_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (pixel_format == HI_PIXEL_FORMAT_VY1UY0_PACKAGE_422) {
        sample_uvc_yuyv_to_nv12(data, pic_size->width, pic_size->height, size, frame_info.video_frame.virt_addr[0]);
    } else {
        frame_info.video_frame.pixel_format = pixel_format;
        ret = memcpy_s(frame_info.video_frame.virt_addr[0], size, data, size);
        if (ret != EOK) {
            sample_print("memcpy_s video frame data fail %x\n", ret);
        }
    }

    ret = hi_mpi_vpss_send_frame(grp, &frame_info, milli_sec);
    if (ret != HI_SUCCESS) {
        sample_print("send frame to vpss failed!\n");
    }

    hi_mpi_sys_munmap(frame_info.video_frame.virt_addr[0], calc_cfg.vb_size);
    ret = hi_mpi_vb_release_blk(vb_blk);
    if (ret != HI_SUCCESS) {
        sample_print("release vb failed!\n");
        return HI_FAILURE;
    }

    return ret;
}

static hi_s32 sample_uvc_get_pixel_format(const hi_char *type_name, hi_pixel_format *pixel_format)
{
    if (strcmp(type_name, "YUYV") == 0) {
        *pixel_format = HI_PIXEL_FORMAT_VY1UY0_PACKAGE_422;
    } else if (strcmp(type_name, "NV12") == 0) {
        *pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    } else if (strcmp(type_name, "NV21") == 0) {
        *pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    } else {
        sample_print("pixel format error!\n");
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

hi_s32 sample_uvc_media_send_data(hi_void *data, hi_u32 size, hi_u32 stride,
    const hi_size *pic_size, const hi_char *type_name)
{
    hi_bool end_of_stream = HI_FALSE;
    hi_s32 read_len = size;
    hi_u8 *buf = data;
    hi_vdec_stream vdec_stream;
    hi_u32 start = 0;
    hi_s32 ret;
    hi_s32 chn_id = 0;
    hi_pixel_format pixel_format;
    hi_bool send_again = HI_TRUE;

    if (data == NULL || pic_size == NULL || type_name == NULL) {
        sample_print("data, pic_size or type_name is NULL\n");
        return HI_FAILURE;
    }

    if (strcmp(type_name, "MJPEG") == 0) {
        sample_uvc_cut_stream_for_mjpeg(data, size, chn_id, &read_len, &start);
    } else if (strcmp(type_name, "H264") == 0) {
        sample_uvc_cut_stream_for_h264(data, size, chn_id, &read_len);
    } else if (strcmp(type_name, "H265") == 0) {
        sample_uvc_cut_stream_for_h265(data, size, chn_id, &read_len);
    } else {
        ret = sample_uvc_get_pixel_format(type_name, &pixel_format);
        if (ret != HI_SUCCESS) {
            return ret;
        }
        return sample_uvc_send_frame_to_vpss(data, size, stride, pic_size, pixel_format);
    }

    vdec_stream.addr      = buf + start;
    vdec_stream.len       = read_len;
    vdec_stream.end_of_frame  = HI_TRUE;
    vdec_stream.end_of_stream = end_of_stream;
    vdec_stream.need_display     = 1;

    while (send_again == HI_TRUE) {
        hi_mpi_sys_get_cur_pts(&vdec_stream.pts);
        ret = hi_mpi_vdec_send_stream(chn_id, &vdec_stream, 0);
        if (ret != HI_SUCCESS) {
            usleep(1000);   /* 1000:1000us */
            sample_print("send stream to vdec failed!\n");
            continue;
        } else {
            end_of_stream = HI_FALSE;
        }
        send_again = HI_FALSE;
    }
    usleep(1000);       /* 1000:1000us */

    return HI_SUCCESS;
}

hi_s32 sample_uvc_media_stop_receive_data(hi_void)
{
    hi_vdec_stream vdec_stream = {0};
    hi_s32 vdec_chn_num = 1;
    hi_s32 chn_id = 0;

    if (g_is_need_vdec == HI_FALSE) {
        return HI_SUCCESS;
    }

    vdec_stream.end_of_stream = HI_TRUE;
    hi_mpi_vdec_send_stream(chn_id, &vdec_stream, -1);
    hi_mpi_vdec_stop_recv_stream(vdec_chn_num);

    return HI_SUCCESS;
}
