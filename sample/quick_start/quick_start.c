/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "sample_ipc.h"
#include "securec.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_awb.h"
#include "ot_sns_ctrl.h"


#define MIPI_DEV_NAME "/dev/ot_mipi_rx"
#define ARGV_AE_EXP 1
#define ARGV_AE_AGAIN 2
#define ARGV_AE_DGAIN 3
#define ARGV_AE_ISP_DGAIN 4
#define ARGV_AE_EXPOSURE 5
#define ARGV_AE_LINE_PER500MS 6
#define ARGV_AWB_RGAIN 7
#define ARGV_AWB_GGAIN 8
#define ARGV_AWB_BGAIN 9

static volatile sig_atomic_t g_sig_flag = 0;
static hi_u32 g_sns_exp, g_sns_again, g_sns_dgain, g_isp_dgain, g_exposure, g_lines_per500ms;
static hi_u32 g_awb_rgain, g_awb_ggain, g_awb_bgain;


/* this configuration is used to adjust the size and number of buffer(VB).  */
static sample_vb_param g_vb_param = {
    .vb_size = {2688, 1520},
    .pixel_format = {HI_PIXEL_FORMAT_RGB_BAYER_12BPP, HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420},
    .compress_mode = {HI_COMPRESS_MODE_LINE, HI_COMPRESS_MODE_SEG},
    .video_format = {HI_VIDEO_FORMAT_LINEAR, HI_VIDEO_FORMAT_LINEAR},
    .blk_num = {4, 6}
};

static sampe_sys_cfg g_vio_sys_cfg = {
    .route_num = 1,
    .mode_type = HI_VI_OFFLINE_VPSS_OFFLINE,
    .nr_pos = HI_3DNR_POS_VI,
    .vi_fmu = {0},
    .vpss_fmu = {0},
};

static sample_vo_cfg g_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_layer          = SAMPLE_VO_LAYER_VHD0,
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

static sample_comm_venc_chn_param g_venc_chn_param = {
    .frame_rate           = 30, /* 30 is a number */
    .stats_time           = 2,  /* 2 is a number */
    .gop                  = 60, /* 60 is a number */
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

void sensor_os04a10_init(td_u32 sns_exp, td_u32 sns_again, td_u32 sns_dgain);
void sensor_os04a10_read_exp(td_u32 *sns_exp);
void sensor_os04a10_read_gain (td_u32 *sns_again, td_u32 *sns_dgain);

void sensor_os08a20_init(td_u32 sns_exp, td_u32 sns_again, td_u32 sns_dgain);
void sensor_os08a20_read_exp(td_u32 *sns_exp);
void sensor_os08a20_read_gain (td_u32 *sns_again, td_u32 *sns_dgain);

static hi_void sample_get_char(hi_void)
{
    if (g_sig_flag == 1) {
        return;
    }

    sample_pause();
}

static hi_s32 sample_comm_vi_mipi_ctrl_cmd(hi_u32 devno, hi_u32 cmd)
{
    hi_s32 ret;
    hi_s32 fd;

    fd = open(MIPI_DEV_NAME, O_RDWR);
    if (fd < 0) {
        sample_print("open %s failed!\n", MIPI_DEV_NAME);
        return HI_FAILURE;
    }

    ret = ioctl(fd, cmd, &devno);

    close(fd);

    return ret;
}

static hi_s32 sample_quick_start_vi_start_mipi_rx(const sample_sns_info *sns_info, const sample_mipi_info *mipi_info)
{
    hi_s32 ret = 0;

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_clk_src, HI_MIPI_ENABLE_SENSOR_CLOCK);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u enable sensor clock failed!\n", sns_info->sns_clk_src);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_rst_src, HI_MIPI_RESET_SENSOR);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u reset sensor failed!\n", sns_info->sns_rst_src);
        return HI_FAILURE;
    }

    ret = sample_comm_vi_mipi_ctrl_cmd(sns_info->sns_rst_src, HI_MIPI_UNRESET_SENSOR);
    if (ret != HI_SUCCESS) {
        sample_print("devno %u unreset sensor failed!\n", sns_info->sns_rst_src);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}


static hi_u32 sample_quick_start_get_fmu_wrap_num(hi_fmu_mode fmu_mode[], hi_u32 len)
{
    hi_u32 i;
    hi_u32 cnt = 0;

    for (i = 0; i < len; i++) {
        if (fmu_mode[i] == HI_FMU_MODE_WRAP) {
            cnt++;
        }
    }
    return cnt;
}

static hi_s32 sample_quick_start_fmu_wrap_init(sampe_sys_cfg *fmu_cfg, hi_size *in_size)
{
    hi_u32 cnt;
    hi_fmu_attr fmu_attr;

    cnt = sample_quick_start_get_fmu_wrap_num(fmu_cfg->vi_fmu, fmu_cfg->route_num);
    if (cnt > 0) {
        fmu_attr.wrap_en = HI_TRUE;
        fmu_attr.page_num = MIN2(hi_common_get_fmu_wrap_page_num(HI_FMU_ID_VI,
            in_size->width, in_size->height) + (cnt - 1) * 3, /* 3: for multi pipe */
            HI_FMU_MAX_Y_PAGE_NUM);
    } else {
        fmu_attr.wrap_en = HI_FALSE;
    }
    if (hi_mpi_sys_set_fmu_attr(HI_FMU_ID_VI, &fmu_attr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    cnt = sample_quick_start_get_fmu_wrap_num(fmu_cfg->vpss_fmu, fmu_cfg->route_num);
    if (cnt > 0) {
        fmu_attr.wrap_en = HI_TRUE;
        fmu_attr.page_num = MIN2(hi_common_get_fmu_wrap_page_num(HI_FMU_ID_VPSS,
            in_size->width, in_size->height) + (cnt - 1) * 3, /* 3: for multi pipe */
            HI_FMU_MAX_Y_PAGE_NUM + HI_FMU_MAX_C_PAGE_NUM);
    } else {
        fmu_attr.wrap_en = HI_FALSE;
    }
    if (hi_mpi_sys_set_fmu_attr(HI_FMU_ID_VPSS, &fmu_attr) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/* define SAMPLE_MEM_SHARE_ENABLE, when use tools to dump YUV/RAW. */
#ifdef SAMPLE_MEM_SHARE_ENABLE
hi_void sample_quick_start_init_mem_share(hi_void)
{
    hi_u32 i;
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) != HI_SUCCESS) {
        sample_print("get common pool_id failed!\n");
        return;
    }
    for (i = 0; i < pools_id.pool_cnt; ++i) {
        hi_mpi_vb_pool_share_all(pools_id.pool[i]);
    }
}
#endif

static hi_s32 sample_quick_start_sys_init(hi_void)
{
    hi_vb_cfg vb_cfg;
    hi_u32 supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK | HI_VB_SUPPLEMENT_MOTION_DATA_MASK;

    sample_comm_sys_get_default_vb_cfg(&g_vb_param, &vb_cfg);
    if (sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config) != HI_SUCCESS) {
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    sample_quick_start_init_mem_share();
#endif

    if (sample_comm_vi_set_vi_vpss_mode(g_vio_sys_cfg.mode_type, HI_VI_AIISP_MODE_DEFAULT) != HI_SUCCESS) {
        goto sys_exit;
    }

    if (hi_mpi_sys_set_3dnr_pos(g_vio_sys_cfg.nr_pos) != HI_SUCCESS) {
        goto sys_exit;
    }

    if (sample_quick_start_fmu_wrap_init(&g_vio_sys_cfg, &g_vb_param.vb_size) != TD_SUCCESS) {
        goto sys_exit;
    }

    return HI_SUCCESS;
sys_exit:
    sample_comm_sys_exit();
    return HI_FAILURE;
}

static hi_s32 sample_quick_start_start_vpss(hi_vpss_grp grp, sample_vpss_cfg *vpss_cfg)
{
    hi_s32 ret = TD_SUCCESS;
    sample_vpss_chn_attr vpss_chn_attr = {0};

    ret = memcpy_s(&vpss_chn_attr.chn_attr[0], sizeof(hi_vpss_chn_attr) * HI_VPSS_MAX_PHYS_CHN_NUM,
        vpss_cfg->chn_attr, sizeof(hi_vpss_chn_attr) * HI_VPSS_MAX_PHYS_CHN_NUM);
    if (g_vio_sys_cfg.vpss_fmu[grp] == HI_FMU_MODE_WRAP) {
        vpss_chn_attr.chn0_wrap = HI_TRUE;
    }
    ret += memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
        vpss_cfg->chn_en, sizeof(vpss_chn_attr.chn_enable));
    if (ret != HI_SUCCESS) {
        return ret;
    }
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
    ret = sample_common_vpss_start(grp, &vpss_cfg->grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (vpss_cfg->nr_attr.enable == HI_TRUE) {
        if (hi_mpi_vpss_set_grp_3dnr_attr(grp, &vpss_cfg->nr_attr) != HI_SUCCESS) {
            goto stop_vpss;
        }
    }
    /* OT_FMU_MODE_WRAP is set in sample_common_vpss_start() */
    if (g_vio_sys_cfg.vpss_fmu[grp] == HI_FMU_MODE_OFF) {
        const hi_low_delay_info low_delay_info = { HI_TRUE, 200, HI_FALSE }; /* 200: lowdelay line */
        if (hi_mpi_vpss_set_chn_low_delay(grp, 0, &low_delay_info) != HI_SUCCESS) {
            goto stop_vpss;
        }
    } else if (g_vio_sys_cfg.vpss_fmu[grp] == HI_FMU_MODE_DIRECT) {
        if (hi_mpi_vpss_set_chn_fmu_mode(grp, HI_VPSS_DIRECT_CHN, g_vio_sys_cfg.vpss_fmu[grp]) != HI_SUCCESS) {
            goto stop_vpss;
        }
        if (hi_mpi_vpss_enable_chn(grp, HI_VPSS_DIRECT_CHN) != HI_SUCCESS) {
            goto stop_vpss;
        }
    }

    if (g_vio_sys_cfg.mode_type != HI_VI_ONLINE_VPSS_ONLINE) {
        hi_gdc_param gdc_param = {0};
        gdc_param.in_size.width  = g_vb_param.vb_size.width;
        gdc_param.in_size.height = g_vb_param.vb_size.height;
        gdc_param.cell_size = HI_LUT_CELL_SIZE_16;
        if (hi_mpi_vpss_set_grp_gdc_param(grp, &gdc_param) != HI_SUCCESS) {
            goto stop_vpss;
        }
    }

    return HI_SUCCESS;
stop_vpss:
    sample_common_vpss_stop(grp, vpss_cfg->chn_en, HI_VPSS_MAX_PHYS_CHN_NUM);
    return HI_FAILURE;
}

static hi_void sample_quick_start_stop_vpss(hi_vpss_grp grp)
{
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_FALSE, HI_FALSE, HI_FALSE};

    sample_common_vpss_stop(grp, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static hi_s32 sample_quick_start_start_venc(hi_venc_chn venc_chn[], size_t size, hi_u32 chn_num)
{
    hi_s32 i;
    hi_s32 ret;

    if (chn_num > size) {
        return HI_FAILURE;
    }

    sample_comm_vi_get_size_by_sns_type(SENSOR0_TYPE, &g_venc_chn_param.venc_size);
    for (i = 0; i < (hi_s32)chn_num; i++) {
        ret = sample_comm_venc_start(venc_chn[i], &g_venc_chn_param);
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

static hi_void sample_quick_start_stop_venc(hi_venc_chn venc_chn[], size_t size, hi_u32 chn_num)
{
    hi_u32 i;

    if (chn_num > size) {
        return;
    }

    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static hi_s32 sample_quick_start_start_vo(sample_vo_mode vo_mode)
{
    g_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_vo_cfg);
}

static hi_void sample_quick_start_stop_vo(hi_void)
{
    sample_comm_vo_stop_vo(&g_vo_cfg);
}

static hi_s32 sample_quick_start_start_venc_and_vo(hi_vpss_grp vpss_grp[], size_t size, hi_u32 grp_num)
{
    hi_u32 i;
    hi_s32 ret;
    sample_vo_mode vo_mode = VO_MODE_1MUX;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    if (grp_num > size) {
        return HI_FAILURE;
    }

    if (grp_num > 1) {
        vo_mode = VO_MODE_4MUX;
    }

    ret = sample_quick_start_start_venc(venc_chn, size, grp_num);
    if (ret != HI_SUCCESS) {
        goto start_venc_failed;
    }

    for (i = 0; i < grp_num; i++) {
        if (g_vio_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_DIRECT) {
            sample_comm_vpss_bind_venc(vpss_grp[i], HI_VPSS_DIRECT_CHN, venc_chn[i]);
        } else {
            sample_comm_vpss_bind_venc(vpss_grp[i], HI_VPSS_CHN0, venc_chn[i]);
        }
    }

    ret = sample_quick_start_start_vo(vo_mode);
    if (ret != HI_SUCCESS) {
        goto start_vo_failed;
    }

    for (i = 0; i < grp_num; i++) {
        if (g_vio_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_WRAP) {
            sample_comm_vpss_bind_vo(vpss_grp[i], HI_VPSS_CHN1, vo_layer, vo_chn[i]);
        } else {
            sample_comm_vpss_bind_vo(vpss_grp[i], HI_VPSS_CHN0, vo_layer, vo_chn[i]);
        }
    }
    return HI_SUCCESS;

start_venc_failed:
    sample_quick_start_stop_vo();
start_vo_failed:
    return HI_FAILURE;
}

static hi_void sample_quick_start_stop_venc_and_vo(hi_vpss_grp vpss_grp[], size_t size, hi_u32 grp_num)
{
    hi_u32 i;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    if (grp_num > size) {
        return;
    }
    for (i = 0; i < grp_num; i++) {
        if (g_vio_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_WRAP) {
            sample_comm_vpss_un_bind_vo(vpss_grp[i], HI_VPSS_CHN1, vo_layer, vo_chn[i]);
        } else {
            sample_comm_vpss_un_bind_vo(vpss_grp[i], HI_VPSS_CHN0, vo_layer, vo_chn[i]);
        }
        if (g_vio_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_DIRECT) {
            sample_comm_vpss_un_bind_venc(vpss_grp[i], HI_VPSS_DIRECT_CHN, venc_chn[i]);
        } else {
            sample_comm_vpss_un_bind_venc(vpss_grp[i], HI_VPSS_CHN0, venc_chn[i]);
        }
    }

    sample_quick_start_stop_venc(venc_chn, size, grp_num);
    sample_quick_start_stop_vo();
}

static hi_s32 sample_quick_start_start_route(sample_vi_cfg *vi_cfg, sample_vpss_cfg *vpss_cfg, hi_s32 route_num)
{
    hi_s32 i, j, ret;
    hi_vpss_grp vpss_grp[SAMPLE_VIO_MAX_ROUTE_NUM] = {0, 1, 2, 3};

    sample_comm_vi_get_size_by_sns_type(SENSOR0_TYPE, &g_vb_param.vb_size);
    if (sample_quick_start_sys_init() != HI_SUCCESS) {
        return HI_FAILURE;
    }

    for (i = 0; i < route_num; i++) {
        ret = sample_comm_vi_start_vi(&vi_cfg[i]);
            if (ret != HI_SUCCESS) {
            goto start_vi_failed;
        }
    }

    for (i = 0; i < route_num; i++) {
        sample_comm_vi_bind_vpss(i, 0, vpss_grp[i], 0);
    }

    for (i = 0; i < route_num; i++) {
        ret = sample_quick_start_start_vpss(vpss_grp[i], vpss_cfg);
        if (ret != HI_SUCCESS) {
            goto start_vpss_failed;
        }
    }

    ret = sample_quick_start_start_venc_and_vo(vpss_grp, SAMPLE_VIO_MAX_ROUTE_NUM, route_num);
    if (ret != HI_SUCCESS) {
        goto start_venc_and_vo_failed;
    }

    return HI_SUCCESS;

start_venc_and_vo_failed:
start_vpss_failed:
    for (j = i - 1; j >= 0; j--) {
        sample_quick_start_stop_vpss(vpss_grp[j]);
    }
    for (i = 0; i < route_num; i++) {
        sample_comm_vi_un_bind_vpss(i, 0, vpss_grp[i], 0);
    }
start_vi_failed:
    for (j = i - 1; j >= 0; j--) {
        sample_comm_vi_stop_vi(&vi_cfg[j]);
    }
    sample_comm_sys_exit();
    return HI_FAILURE;
}

static hi_void sample_quick_start_stop_route(sample_vi_cfg *vi_cfg, hi_s32 route_num)
{
    hi_s32 i;
    hi_vpss_grp vpss_grp[SAMPLE_VIO_MAX_ROUTE_NUM] = {0, 1, 2, 3};

    sample_quick_start_stop_venc_and_vo(vpss_grp, SAMPLE_VIO_MAX_ROUTE_NUM, route_num);
    for (i = 0; i < route_num; i++) {
        sample_quick_start_stop_vpss(vpss_grp[i]);
        sample_comm_vi_un_bind_vpss(i, 0, vpss_grp[i], 0);
        sample_comm_vi_stop_vi(&vi_cfg[i]);
    }
    sample_comm_sys_exit();
}

static hi_void sample_quick_start_get_vi_vpss_mode()
{
    g_vio_sys_cfg.mode_type = HI_VI_ONLINE_VPSS_ONLINE;
    g_vio_sys_cfg.vi_fmu[0] = HI_FMU_MODE_OFF;
    g_vb_param.blk_num[0] = 0; /* raw_vb num 0 */
}

static hi_void sample_quick_start_read_exp(hi_u32 *exposure, hi_u32 *lines_per500ms, hi_u32 *isp_dgain)
{
    hi_s32 ret;
    ot_isp_exp_info exp_info;
    ret = hi_mpi_isp_query_exposure_info(0, &exp_info);
    if (ret == HI_SUCCESS) {
        *exposure = exp_info.exposure;
        *lines_per500ms = exp_info.lines_per500ms;
        *isp_dgain = exp_info.isp_d_gain;
    } else {
        *exposure = 0;
        *lines_per500ms = 0;
        *isp_dgain = 0;
    }
}

static hi_void sample_quick_start_read_awb(hi_u32 *awb_r, hi_u32 *awb_g, hi_u32 *awb_b)
{
    hi_s32 ret;
    ot_isp_wb_info wb_info;
    ret = hi_mpi_isp_query_wb_info(0, &wb_info);
    if (ret == HI_SUCCESS) {
        *awb_r = wb_info.r_gain;
        *awb_g = wb_info.gr_gain;
        *awb_b = wb_info.b_gain;
    } else {
        *awb_r = 0;
        *awb_g = 0;
        *awb_b = 0;
    }
}

static hi_void sample_quick_start_isp_init()
{
    sample_sns_type sns_type = SENSOR0_TYPE;
    ot_isp_sns_obj *sns_obj = HI_NULL;
    ot_isp_init_attr isp_init_attr = {0};
    if (sns_type == OV_OS04A10_MIPI_4M_30FPS_12BIT) {
        sns_obj = &g_sns_os04a10_obj;
    } else if (sns_type == OV_OS08A20_MIPI_8M_30FPS_12BIT) {
        sns_obj = &g_sns_os08a20_obj;
    }
    isp_init_attr.exp_time = g_sns_exp;
    isp_init_attr.a_gain = g_sns_again;
    isp_init_attr.d_gain = g_sns_dgain;
    isp_init_attr.ispd_gain = g_isp_dgain;
    isp_init_attr.wb_r_gain = g_awb_rgain;
    isp_init_attr.wb_g_gain = g_awb_ggain;
    isp_init_attr.wb_b_gain = g_awb_bgain;
    isp_init_attr.exposure = g_exposure;
    isp_init_attr.lines_per500ms = g_lines_per500ms;
    isp_init_attr.quick_start_en = HI_TRUE;
    if (sns_obj != HI_NULL) {
        sns_obj->pfn_set_init(0, &isp_init_attr);
    }
}

static hi_s32 sample_quick_start()
{
    sample_vi_cfg vi_cfg[1];
    sample_vpss_cfg vpss_cfg;
    sample_sns_type sns_type = SENSOR0_TYPE;

    if (sns_type != OV_OS04A10_MIPI_4M_30FPS_12BIT && sns_type != OV_OS08A20_MIPI_8M_30FPS_12BIT) {
        printf("quick start only support os04a10 and os08a20!\n");
        return HI_SUCCESS;
    }

    sample_quick_start_get_vi_vpss_mode();
    sample_comm_vi_get_vi_cfg_by_fmu_mode(sns_type, g_vio_sys_cfg.vi_fmu[0], &vi_cfg[0]);
    sample_comm_vpss_get_default_vpss_cfg(&vpss_cfg, g_vio_sys_cfg.vpss_fmu[0]);

    sample_quick_start_vi_start_mipi_rx(&vi_cfg[0].sns_info, &vi_cfg[0].mipi_info);
    vi_cfg[0].pipe_info[0].isp_quick_start = HI_TRUE;
    vi_cfg[0].sns_info.sns_clk_rst_en = HI_FALSE;
    if (sns_type == OV_OS04A10_MIPI_4M_30FPS_12BIT) {
        sensor_os04a10_init(g_sns_exp, g_sns_again, g_sns_dgain);
    } else if (sns_type == OV_OS08A20_MIPI_8M_30FPS_12BIT) {
        sensor_os08a20_init(g_sns_exp, g_sns_again, g_sns_dgain);
    }
    sample_quick_start_isp_init();

    if (sample_quick_start_start_route(vi_cfg, &vpss_cfg, g_vio_sys_cfg.route_num) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    sample_get_char();
    if (sns_type == OV_OS04A10_MIPI_4M_30FPS_12BIT) {
        sensor_os04a10_read_exp(&g_sns_exp);
        sensor_os04a10_read_gain(&g_sns_again, &g_sns_dgain);
    } else if (sns_type == OV_OS08A20_MIPI_8M_30FPS_12BIT) {
        sensor_os08a20_read_exp(&g_sns_exp);
        sensor_os08a20_read_gain(&g_sns_again, &g_sns_dgain);
    }
    sample_quick_start_read_exp(&g_exposure, &g_lines_per500ms, &g_isp_dgain);
    sample_quick_start_read_awb(&g_awb_rgain, &g_awb_ggain, &g_awb_bgain);
    printf("sns_exp = %u again = %u dgain = %u isp_dgain = %u exposure = %u lines_per500ms = %u "
        "awb_rgain = %u, awb_ggain = %u, awb_bgain = %u\n",
        g_sns_exp, g_sns_again, g_sns_dgain, g_isp_dgain, g_exposure, g_lines_per500ms,
        g_awb_rgain, g_awb_ggain, g_awb_bgain);

    sample_quick_start_stop_route(vi_cfg, g_vio_sys_cfg.route_num);
    return HI_SUCCESS;
}

static hi_void sample_quick_start_handle_sig(hi_s32 signo)
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

static hi_s32 sample_quick_start_msg_proc_vb_pool_share(hi_s32 pid)
{
    hi_s32 ret;
    hi_u32 i;
    hi_bool isp_states[HI_VI_MAX_PIPE_NUM];
#ifndef SAMPLE_MEM_SHARE_ENABLE
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) != HI_SUCCESS) {
        sample_print("get common pool_id failed!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pools_id.pool_cnt; ++i) {
        if (hi_mpi_vb_pool_share(pools_id.pool[i], pid) != HI_SUCCESS) {
            sample_print("vb pool share failed!\n");
            return HI_FAILURE;
        }
    }
#endif
    ret = sample_comm_vi_get_isp_run_state(isp_states, HI_VI_MAX_PIPE_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("get isp states fail\n");
        return HI_FAILURE;
    }

    for (i = 0; i < HI_VI_MAX_PIPE_NUM; i++) {
        if (!isp_states[i]) {
            continue;
        }
        ret = hi_mpi_isp_mem_share(i, pid);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_isp_mem_share vi_pipe %u, pid %d fail\n", i, pid);
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_quick_start_msg_proc_vb_pool_unshare(hi_s32 pid)
{
    hi_s32 ret;
    hi_u32 i;
    hi_bool isp_states[HI_VI_MAX_PIPE_NUM];
#ifndef SAMPLE_MEM_SHARE_ENABLE
    hi_vb_common_pools_id pools_id = {0};
    if (hi_mpi_vb_get_common_pool_id(&pools_id) == HI_SUCCESS) {
        for (i = 0; i < pools_id.pool_cnt; ++i) {
            ret = hi_mpi_vb_pool_unshare(pools_id.pool[i], pid);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vb_pool_unshare vi_pipe %u, pid %d fail\n", pools_id.pool[i], pid);
            }
        }
    }
#endif
    ret = sample_comm_vi_get_isp_run_state(isp_states, HI_VI_MAX_PIPE_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("get isp states fail\n");
        return;
    }

    for (i = 0; i < HI_VI_MAX_PIPE_NUM; i++) {
        if (!isp_states[i]) {
            continue;
        }
        ret = hi_mpi_isp_mem_unshare(i, pid);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_isp_mem_unshare vi_pipe %u, pid %d fail\n", i, pid);
        }
    }
}

static hi_s32 sample_quick_start_ipc_msg_proc(const sample_ipc_msg_req_buf *msg_req_buf,
    hi_bool *is_need_fb, sample_ipc_msg_res_buf *msg_res_buf)
{
    hi_s32 ret;

    if (msg_req_buf == HI_NULL || is_need_fb == HI_NULL) {
        return HI_FAILURE;
    }

    /* need feedback default */
    *is_need_fb = HI_TRUE;

    switch ((sample_msg_type)msg_req_buf->msg_type) {
        case SAMPLE_MSG_TYPE_VB_POOL_SHARE_REQ: {
            if (msg_res_buf == HI_NULL) {
                return HI_FAILURE;
            }
            ret = sample_quick_start_msg_proc_vb_pool_share(msg_req_buf->msg_data.pid);
            msg_res_buf->msg_type = SAMPLE_MSG_TYPE_VB_POOL_SHARE_RES;
            msg_res_buf->msg_data.is_req_success = (ret == HI_SUCCESS) ? HI_TRUE : HI_FALSE;
            break;
        }
        case SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_REQ: {
            if (msg_res_buf == HI_NULL) {
                return HI_FAILURE;
            }
            sample_quick_start_msg_proc_vb_pool_unshare(msg_req_buf->msg_data.pid);
            msg_res_buf->msg_type = SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_RES;
            msg_res_buf->msg_data.is_req_success = HI_TRUE;
            break;
        }
        default: {
            printf("unsupported msg type(%ld)!\n", msg_req_buf->msg_type);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

static hi_void sample_quick_start_usage(const char *prg_name)
{
    printf("usage : %s <sns_exp> <sns_again> <sns_dgain> <isp_dgain> <exposure> "
        "<line_per500ms> <r_gain> <g_gain> <b_gain>\n", prg_name);
}

#ifdef __LITEOS__
hi_s32 app_main(hi_s32 argc, hi_char *argv[])
#else
hi_s32 main(hi_s32 argc, hi_char *argv[])
#endif
{
    hi_s32 ret = HI_TRUE;
    hi_char *para_stop;
    if (argc != 10) { /* 10 parameter. */
        sample_quick_start_usage(argv[0]);
        return HI_FAILURE;
    }
    g_sns_exp = (hi_u32)strtol(argv[ARGV_AE_EXP], &para_stop, 10); /* 10 dec */
    g_sns_again = (hi_u32)strtol(argv[ARGV_AE_AGAIN], &para_stop, 10); /* 10 dec */
    g_sns_dgain = (hi_u32)strtol(argv[ARGV_AE_DGAIN], &para_stop, 10); /* 10 dec */
    g_isp_dgain = (hi_u32)strtol(argv[ARGV_AE_ISP_DGAIN], &para_stop, 10); /* 10 dec */
    g_exposure = (hi_u32)strtol(argv[ARGV_AE_EXPOSURE], &para_stop, 10); /* 10 dec */
    g_lines_per500ms = (hi_u32)strtol(argv[ARGV_AE_LINE_PER500MS], &para_stop, 10); /* 10 dec */

    g_awb_rgain = (hi_u32)strtol(argv[ARGV_AWB_RGAIN], &para_stop, 10); /* 10 dec */
    g_awb_ggain = (hi_u32)strtol(argv[ARGV_AWB_GGAIN], &para_stop, 10); /* 10 dec */
    g_awb_bgain = (hi_u32)strtol(argv[ARGV_AWB_BGAIN], &para_stop, 10); /* 10 dec */

#ifndef __LITEOS__
    sample_register_sig_handler(sample_quick_start_handle_sig);
#endif

    if (sample_ipc_server_init(sample_quick_start_ipc_msg_proc) != HI_SUCCESS) {
        printf("sample_ipc_server_init failed!!!\n");
    }

    ret = sample_quick_start();
    if ((ret == HI_SUCCESS) && (g_sig_flag == 0)) {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    sample_ipc_server_deinit();
    return ret;
}
