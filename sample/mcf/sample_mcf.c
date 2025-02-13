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
#include <limits.h>

#include "sample_comm.h"
#include "hi_sns_ctrl.h"
#include "sample_comm_mcf.h"
#include "hi_mpi_mcf_calibration.h"
#include "sample_mcf_calibration.h"
#include "hi_common_mcf.h"


#ifdef __LITEOS__
#define SAMPLE_MCF_SAVE_PATH   "/sharefs/data"
#else
#define SAMPLE_MCF_SAVE_PATH   "./data"
#endif

#define MM_BMP "./res/mm.bmp"
static hi_char *g_mcf_path_bmp = MM_BMP;
#define MCF_RGN_HANDLE_NUM_1 1
#define MCF_RGN_HANDLE_NUM_4 4
#define MCF_RGN_HANDLE_NUM_8 8

#define B_32BYTEALINE 0
#define VB_SYS_STRIDE 32

#define MCF_BIND_VPSS_GRP 2
#define MCF_VO_WIDHT_1920 1920
#define MCF_VO_HEIGHT_1080 1080

#define MCF_VENC_WIDHT_3840 3840
#define MCF_VENC_HEIGHT_2160 2160

#define VB_MCF_YUV_CNT  5
#define VB_MCF_EQUAL_YUV_CNT  5
#define MCF_SAMPLE_ARG_NUMBER_BASE 10

#define MCF_WIDTH_3072 3072
#define MCF_HEIGHT_1520 1520

static volatile sig_atomic_t g_sig_flag = 0;

hi_bool g_vpss_chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_TRUE, HI_FALSE};
hi_bool g_mcf_chn_enable[HI_MCF_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_FALSE, HI_FALSE};
hi_size g_mcf_chn_out_size[HI_MCF_MAX_PHYS_CHN_NUM] = { 0 };
static sample_vo_cfg g_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_layer          = SAMPLE_VO_LAYER_VHD0,
    .vo_intf_type      = HI_VO_INTF_BT1120,
    .intf_sync         = HI_VO_OUT_1080P60,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, MCF_VO_WIDHT_1920, MCF_VO_HEIGHT_1080},
    .image_size        = {MCF_VO_WIDHT_1920, MCF_VO_HEIGHT_1080},
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
    .venc_size            = {MCF_VENC_WIDHT_3840, MCF_VENC_HEIGHT_2160},
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

static hi_void sample_get_char(hi_char *s)
{
    if (g_sig_flag == 1) {
        return;
    }

    printf("   press any key to %s !   \n", s);
    (hi_void)getchar();
}

static hi_void sample_mcf_get_default_vb_config(const hi_size *size, hi_vb_cfg *vb_cfg, hi_bool fov_en)
{
    hi_vb_calc_cfg calc_cfg;
    hi_pic_buf_attr buf_attr;

    (hi_void)memset_s(vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 pool cnt */

    buf_attr.width         = size->width;
    buf_attr.height        = size->height;
    buf_attr.align         = HI_DEFAULT_ALIGN;
    buf_attr.bit_width     = HI_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format  = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    vb_cfg->common_pool[0].blk_size = hi_common_get_pic_buf_size(&buf_attr);
    if (fov_en) {
        vb_cfg->common_pool[0].blk_cnt  = 40; /* 40 or more vb block cnt if set fov */
    } else {
        vb_cfg->common_pool[0].blk_cnt  = 30; /* 30 vb block cnt */
    }
}

/* define SAMPLE_MEM_SHARE_ENABLE, when use tools to dump YUV/RAW. */
#ifdef SAMPLE_MEM_SHARE_ENABLE
hi_void sample_mcf_init_mem_share(hi_void)
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

static hi_s32 sample_mcf_sys_init(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode,
                                  const hi_size *color_size, hi_bool fov_en)
{
    hi_s32 ret;
    hi_vb_cfg vb_cfg;
    hi_u32 supplement_config;

    sample_mcf_get_default_vb_config(color_size, &vb_cfg, fov_en);

    supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;
    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
        sample_mcf_init_mem_share();
#endif

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, aiisp_mode);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_mcf_start_venc(hi_venc_chn venc_chn[], hi_u32 chn_num, hi_u32 chn_len)
{
    hi_s32 i;
    hi_s32 ret;
    if (chn_len != 3) {    /* 3: chn len  */
        return HI_FAILURE;
    }
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

static hi_void sample_mcf_stop_venc(hi_venc_chn venc_chn[], hi_u32 chn_num, hi_u32 chn_len)
{
    hi_u32 i;
    if (chn_len != 3) {           /* 3: chn len  */
        return;
    }
    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static hi_s32 sample_mcf_start_vo(sample_vo_mode vo_mode)
{
    g_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_vo_cfg);
}

static hi_void sample_mcf_stop_vo(hi_void)
{
    sample_comm_vo_stop_vo(&g_vo_cfg);
}

static hi_s32 sample_mcf_start_vpss(hi_vpss_grp grp, hi_size *in_size)
{
    hi_vpss_grp_attr grp_attr = {0};
    sample_vpss_chn_attr vpss_chn_attr = {0};
    hi_vpss_chn_attr *attr = HI_NULL;
    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width  = in_size->width;
    grp_attr.max_height = in_size->height;
    grp_attr.mcf_en     = HI_FALSE;
    attr = &vpss_chn_attr.chn_attr[0];
    sample_comm_vpss_get_default_chn_attr(attr);
    vpss_chn_attr.chn_attr[0].width  = in_size->width;
    vpss_chn_attr.chn_attr[0].height = in_size->height;
    attr = &vpss_chn_attr.chn_attr[1];
    sample_comm_vpss_get_default_chn_attr(attr);
    vpss_chn_attr.chn_attr[1].width  = MCF_VO_WIDHT_1920;
    vpss_chn_attr.chn_attr[1].height = MCF_VO_HEIGHT_1080;
    vpss_chn_attr.chn_attr[1].compress_mode = HI_COMPRESS_MODE_NONE;
    memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
        g_vpss_chn_enable, sizeof(vpss_chn_attr.chn_enable));
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
    return sample_common_vpss_start(grp, &grp_attr, &vpss_chn_attr);
}

static hi_void sample_mcf_stop_vpss(hi_vpss_grp grp)
{
    sample_common_vpss_stop(grp, g_vpss_chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static hi_s32 sample_mcf_start_vpss_venc_and_vo(hi_size *out_size)
{
    hi_s32 ret;
    const sample_vo_mode vo_mode = VO_MODE_1MUX;
    hi_venc_chn venc_chn[3] = {0, 1, 2}; /* 3: max chn num, 0/1/2 chn id */
    const hi_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    const hi_s32 grp_num = 1;

    ret = sample_mcf_start_vpss(vpss_grp, out_size);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = sample_mcf_start_vo(vo_mode);
    if (ret != HI_SUCCESS) {
        goto start_vo_failed;
    }

    g_venc_chn_param.venc_size.width        = out_size->width;
    g_venc_chn_param.venc_size.height       = out_size->height;
    g_venc_chn_param.size = sample_comm_sys_get_pic_enum(out_size);

    ret = sample_mcf_start_venc(venc_chn, grp_num, 3); /* 3: max chn num, 0/1/2 chn id */
    if (ret != HI_SUCCESS) {
        goto start_venc_failed;
    }
    return HI_SUCCESS;

start_vo_failed:
    sample_mcf_stop_vpss(vpss_grp);

start_venc_failed:
    sample_mcf_stop_vo();
    return HI_FAILURE;
}

static hi_void sample_mcf_stop_vpss_venc_and_vo(hi_void)
{
    const hi_s32 grp_num = 1;
    hi_venc_chn venc_chn[3] = {0, 1, 2}; /* 3: chn num, 0/1/2 chn id */
    const hi_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    sample_mcf_stop_venc(venc_chn, grp_num, 3); /* 3: chn num, 0/1/2 chn id */
    sample_mcf_stop_vo();
    sample_mcf_stop_vpss(vpss_grp);
}

static hi_void sample_mcf_get_size_by_sns_type(sample_sns_type sns_type, hi_size *size)
{
    sample_comm_vi_get_size_by_sns_type(sns_type, size);

    switch (sns_type) {
        case OV_OS08A20_MIPI_8M_30FPS_12BIT:
        case OV_OS08A20_MIPI_8M_30FPS_10BIT_WDR2TO1:
        case SC850SL_MIPI_8M_30FPS_12BIT:
        case SC850SL_MIPI_8M_30FPS_10BIT_WDR2TO1:
        case SONY_IMX515_MIPI_8M_30FPS_12BIT:
            size->width  = MCF_WIDTH_3072;
            size->height = MCF_HEIGHT_1520;
            break;

        default:
            break;
    }
}

hi_void sample_mcf_refresh_vi_cfg(sample_sns_type sns_type, sample_vi_cfg *vi_cfg)
{
    hi_size size;
    hi_u32 ob_height;
    hi_u32 i;

    sample_mcf_get_size_by_sns_type(sns_type, &size);
    ob_height = sample_comm_vi_get_obheight_by_sns_type(sns_type);
    vi_cfg->mipi_info.combo_dev_attr.img_rect.width = size.width;
    vi_cfg->mipi_info.combo_dev_attr.img_rect.height = size.height + ob_height;
    vi_cfg->dev_info.dev_attr.in_size.width  = size.width;
    vi_cfg->dev_info.dev_attr.in_size.height = size.height + ob_height;
    vi_cfg->grp_info.fusion_grp_attr[0].cache_line = size.height;

    for (i = 0; i < vi_cfg->bind_pipe.pipe_num; i++) {
        vi_cfg->pipe_info[i].pipe_attr.size.width  = size.width;
        vi_cfg->pipe_info[i].pipe_attr.size.height = size.height;
        vi_cfg->pipe_info[i].chn_info[0].chn_attr.size.width  = size.width;
        vi_cfg->pipe_info[i].chn_info[0].chn_attr.size.height = size.height;
        vi_cfg->pipe_info[i].isp_info.isp_pub_attr.wnd_rect.width  = size.width;
        vi_cfg->pipe_info[i].isp_info.isp_pub_attr.wnd_rect.height = size.height;
        vi_cfg->pipe_info[i].isp_info.isp_pub_attr.sns_size.width  = size.width;
        vi_cfg->pipe_info[i].isp_info.isp_pub_attr.sns_size.height = size.height;
    }
}

static hi_s32 sample_mcf_start_vi0(sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;
    sample_comm_vi_get_default_vi_cfg(SENSOR0_TYPE, vi_cfg);
    sample_mcf_refresh_vi_cfg(SENSOR0_TYPE, vi_cfg);
    vi_cfg->mipi_info.divide_mode = LANE_DIVIDE_MODE_1;
    vi_cfg->dev_info.vi_dev = 0;
    vi_cfg->bind_pipe.pipe_id[0] = VI_MONO_PIPE;
    vi_cfg->grp_info.fusion_grp_attr[0].pipe_id[0] = VI_MONO_PIPE;
    vi_cfg->pipe_info[0].chn_info[0].chn_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    ret = sample_comm_vi_start_vi(vi_cfg);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_mcf_start_vi1(sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;
    const hi_vi_dev vi_dev = 2; /* dev 2 for sensor1 */
    sample_comm_vi_get_default_vi_cfg(SENSOR1_TYPE, vi_cfg);
    sample_mcf_refresh_vi_cfg(SENSOR1_TYPE, vi_cfg);
    vi_cfg->sns_info.bus_id = 5; /* i2c5 */
    vi_cfg->sns_info.sns_clk_src = 1;
    vi_cfg->sns_info.sns_rst_src = 1;
    sample_comm_vi_get_mipi_info_by_dev_id(SENSOR1_TYPE, vi_dev, &vi_cfg->mipi_info);
    vi_cfg->mipi_info.divide_mode = LANE_DIVIDE_MODE_1;
    vi_cfg->dev_info.vi_dev = vi_dev;
    vi_cfg->bind_pipe.pipe_id[0] = VI_COLOR_PIPE;
    vi_cfg->grp_info.grp_num = 1;
    vi_cfg->grp_info.fusion_grp[0] = 1;
    vi_cfg->grp_info.fusion_grp_attr[0].pipe_id[0] = VI_COLOR_PIPE;
    vi_cfg->pipe_info[0].chn_info[0].chn_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    ret = sample_comm_vi_start_vi(vi_cfg);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_void sample_mcf_bind_vi(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr, hi_vi_chn vi_chn)
{
    hi_mcf_pipe mcf_color_pipe, mcf_mono_pipe;
    hi_vi_pipe vi_mono_pipe, vi_color_pipe;

    vi_mono_pipe = VI_MONO_PIPE;
    vi_color_pipe = VI_COLOR_PIPE;

    mcf_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    mcf_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    sample_common_vi_bind_mcf(vi_mono_pipe, vi_chn, mcf_grp, mcf_mono_pipe);
    sample_common_vi_bind_mcf(vi_color_pipe, vi_chn, mcf_grp, mcf_color_pipe);
}

static hi_void sample_mcf_un_bind_vi(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr, hi_vi_chn vi_chn)
{
    hi_mcf_pipe mcf_color_pipe, mcf_mono_pipe;
    hi_vi_pipe vi_mono_pipe, vi_color_pipe;

    vi_mono_pipe = VI_MONO_PIPE;
    vi_color_pipe = VI_COLOR_PIPE;

    mcf_mono_pipe = mcf_grp_attr->mono_pipe_attr.pipe_id;
    mcf_color_pipe = mcf_grp_attr->color_pipe_attr.pipe_id;

    sample_common_vi_unbind_mcf(vi_mono_pipe, vi_chn, mcf_grp, mcf_mono_pipe);
    sample_common_vi_unbind_mcf(vi_color_pipe, vi_chn, mcf_grp, mcf_color_pipe);
}

static hi_void sample_mcf_start_mcf_vi_cfg(hi_mcf_grp mcf_grp)
{
    hi_mcf_vi_attr mcf_vi_attr;
    hi_s32 ret;
    mcf_vi_attr.enable     = HI_TRUE;
    mcf_vi_attr.mono_pipe  = VI_MONO_PIPE;
    mcf_vi_attr.color_pipe = VI_COLOR_PIPE;
    ret = hi_mpi_mcf_set_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_mcf_set_vi_attr error\n");
    }
}

static hi_void sample_mcf_stop_mcf_vi_cfg(hi_mcf_grp mcf_grp)
{
    hi_mcf_vi_attr mcf_vi_attr;
    hi_s32 ret;
    ret = hi_mpi_mcf_get_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_mcf_get_vi_attr err\n");
        return;
    }
    mcf_vi_attr.enable     = HI_FALSE;
    ret = hi_mpi_mcf_set_vi_attr(mcf_grp, &mcf_vi_attr);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_mcf_set_vi_attr error\n");
    }
}

hi_void sample_mcf_set_default_vpss_attr(hi_vpss_grp_attr *vpss_grp_attr)
{
    (hi_void)memset_s(vpss_grp_attr, sizeof(hi_vpss_grp_attr), 0, sizeof(hi_vpss_grp_attr));
    vpss_grp_attr->ie_en                     = HI_FALSE;
    vpss_grp_attr->dci_en                    = HI_FALSE;
    vpss_grp_attr->buf_share_en              = HI_FALSE;
    vpss_grp_attr->mcf_en                    = HI_FALSE;
    vpss_grp_attr->max_width                 = 4096;  /* 4096 max width */
    vpss_grp_attr->max_height                = 4096; /* 4096 max height */
    vpss_grp_attr->max_dei_width             = 0;
    vpss_grp_attr->max_dei_height            = 0;
    vpss_grp_attr->dynamic_range             = HI_DYNAMIC_RANGE_SDR8;
    vpss_grp_attr->dei_mode                  = HI_VPSS_DEI_MODE_OFF;
    vpss_grp_attr->buf_share_chn             = HI_VPSS_CHN0;
    vpss_grp_attr->pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;
    vpss_grp_attr->frame_rate.src_frame_rate = -1;
    vpss_grp_attr->frame_rate.dst_frame_rate = -1;
}

static hi_void sample_mcf_bind(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr)
{
    const hi_vi_chn vi_chn = 0;
    const hi_mcf_chn mcf_chn = 0;
    const hi_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    hi_vpss_chn vpss_chn[2] = {0, 1};
    const hi_vo_layer vo_layer = 0;
    const hi_vo_chn vo_chn = 0;
    const hi_venc_chn venc_chn = 0;

    sample_mcf_bind_vi(mcf_grp, mcf_grp_attr, vi_chn);
    sample_common_mcf_bind_vpss(mcf_grp, mcf_chn, vpss_grp, vpss_chn[0]);
    sample_comm_vpss_bind_venc(vpss_grp, vpss_chn[0], venc_chn);
    sample_comm_vpss_bind_vo(vpss_grp, vpss_chn[1], vo_layer, vo_chn);
}

static hi_void sample_mcf_un_bind(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr)
{
    const hi_vi_chn vi_chn = 0;
    const hi_mcf_chn mcf_chn = 0;
    const hi_vpss_grp vpss_grp = MCF_BIND_VPSS_GRP;
    hi_vpss_chn vpss_chn[2] = {0, 1};
    const hi_vo_layer vo_layer = 0;
    const hi_vo_chn vo_chn = 0;
    const hi_venc_chn venc_chn = 0;

    sample_comm_vpss_un_bind_venc(vpss_grp, vpss_chn[0], venc_chn);
    sample_comm_vpss_un_bind_vo(vpss_grp, vpss_chn[1], vo_layer, vo_chn);
    sample_common_mcf_unbind_vpss(mcf_grp, mcf_chn, vpss_grp, vpss_chn[0]);
    sample_mcf_un_bind_vi(mcf_grp, mcf_grp_attr, vi_chn);
}

static hi_s32 sample_mcf_start_mcf_data_flow(hi_mcf_grp mcf_grp, hi_mcf_grp_attr *mcf_grp_attr,
    hi_size out_size, hi_mcf_crop_info *grp_crop_info, hi_fov_attr *fov_correction_attr)
{
    hi_s32 ret;
    sample_yuv_cnt yuv_cnt = { VB_MCF_YUV_CNT, VB_MCF_YUV_CNT };

    if (fov_correction_attr->enable == HI_TRUE) {
        yuv_cnt.color_cnt = VB_MCF_EQUAL_YUV_CNT;
        yuv_cnt.mono_cnt = VB_MCF_EQUAL_YUV_CNT;
    }

    if ((mcf_grp_attr->mono_pipe_attr.height * mcf_grp_attr->mono_pipe_attr.width >
        mcf_grp_attr->color_pipe_attr.height * mcf_grp_attr->color_pipe_attr.width) &&
        (mcf_grp_attr->color_pipe_attr.vpss_grp >= HI_VPSS_VGS_GRP_NO)) {
        mcf_grp_attr->color_pipe_attr.vpss_grp = 1;
    }

    ret = sample_common_mcf_start_vpss(mcf_grp_attr, yuv_cnt.color_cnt, yuv_cnt.mono_cnt, fov_correction_attr);
    if (ret != HI_SUCCESS) {
        goto stop_mcf_vi_cfg;
    }

    ret = sample_common_mcf_start(mcf_grp, mcf_grp_attr, grp_crop_info, &g_mcf_chn_enable[0], &g_mcf_chn_out_size[0]);
    if (ret != HI_SUCCESS) {
        goto start_mcf_failed;
    }

    ret = sample_mcf_start_vpss_venc_and_vo(&out_size);
    if (ret != HI_SUCCESS) {
        goto start_vpss_venc_and_vo_failed;
    }
    sample_mcf_bind(mcf_grp, mcf_grp_attr);

    sample_mcf_start_mcf_vi_cfg(mcf_grp);

    return ret;
start_vpss_venc_and_vo_failed:
    sample_common_mcf_stop(mcf_grp, g_mcf_chn_enable, HI_MCF_MAX_PHYS_CHN_NUM);
start_mcf_failed:
    sample_common_mcf_stop_vpss(mcf_grp_attr);
stop_mcf_vi_cfg:
    sample_mcf_stop_mcf_vi_cfg(mcf_grp);
    return ret;
}

static hi_void sample_mcf_stop_mcf_data_flow(hi_mcf_grp mcf_grp, hi_mcf_grp_attr *mcf_grp_attr)
{
    sample_mcf_stop_mcf_vi_cfg(mcf_grp);
    sample_mcf_un_bind(mcf_grp, mcf_grp_attr);
    sample_mcf_stop_vpss_venc_and_vo();
    sample_common_mcf_stop(mcf_grp, g_mcf_chn_enable, HI_MCF_MAX_PHYS_CHN_NUM);
    sample_common_mcf_stop_vpss(mcf_grp_attr);
}

static hi_void sample_mcf_set_chn_default_outsize(const hi_size *out_size)
{
    hi_s32 i;
    for (i = 0; i < HI_MCF_MAX_PHYS_CHN_NUM; i++) {
        (hi_void)memcpy_s(&g_mcf_chn_out_size[i], sizeof(hi_size), out_size, sizeof(hi_size));
    }
}

static hi_void sample_mcf_get_large_size(hi_size mono_size, hi_size color_size, hi_size *large_size)
{
    if (mono_size.width > color_size.width) {
        large_size->width = mono_size.width;
        large_size->height = mono_size.height;
    } else {
        large_size->width = color_size.width;
        large_size->height = color_size.height;
    }
}

static hi_void sample_mcf_sensor_cfg(const hi_mcf_grp_attr *mcf_grp_attr)
{
    hi_isp_sns_blc_clamp sns_blc_clamp;

    if (SENSOR1_TYPE != SONY_IMX347_SLAVE_MIPI_4M_30FPS_12BIT) {
        return;
    }

    g_sns_imx347_slave_obj.pfn_mirror_flip(mcf_grp_attr->color_pipe_attr.pipe_id, ISP_SNS_MIRROR);
    sns_blc_clamp.blc_clamp_en = HI_TRUE;
    g_sns_imx347_slave_obj.pfn_set_blc_clamp(mcf_grp_attr->color_pipe_attr.pipe_id, sns_blc_clamp);

    return;
}

static hi_s32 sample_mcf_set_path(hi_mcf_grp mcf_grp, hi_mcf_path mcf_path)
{
    hi_s32 ret = HI_SUCCESS;
    hi_mcf_grp_attr grp_attr;

    ret = hi_mpi_mcf_get_grp_attr(mcf_grp, &grp_attr);
    if (ret != HI_SUCCESS) {
        sample_print("get mcf grp attr failed.ret:0x%x !\n", ret);
        return ret;
    }
    grp_attr.mcf_path = mcf_path;
    ret = hi_mpi_mcf_set_grp_attr(mcf_grp, &grp_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set mcf grp attr failed.ret:0x%x !\n", ret);
        return ret;
    }

    return ret;
}

static hi_s32 sample_mcf_start_vpss_color_grp(hi_vpss_grp vpss_color_grp, const hi_mcf_grp_attr *mcf_grp_attr)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vpss_grp_attr vpss_grp_attr;

    sample_common_mcf_set_default_vpss_attr(&vpss_grp_attr, mcf_grp_attr, HI_FALSE);
    ret = hi_mpi_vpss_create_grp(vpss_color_grp, &vpss_grp_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_create_grp failed with %#x!\n", ret);
    }

    ret = hi_mpi_vpss_start_grp(vpss_color_grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_start_grp failed with %#x!\n", ret);
    }

    return ret;
}

static hi_s32 sample_mcf_fusion_to_color_path(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vpss_grp vpss_mono_grp;

    /* switch */
    sample_mcf_stop_mcf_vi_cfg(mcf_grp);
    sample_mcf_set_path(mcf_grp, HI_MCF_PATH_COLOR);

    /* stop mono */
    vpss_mono_grp = mcf_grp_attr->mono_pipe_attr.vpss_grp;
    ret = sample_common_mcf_stop_vpss_one_grp(vpss_mono_grp);

    sample_comm_vi_stop_vi(&vi_cfg[0]);
    return ret;
}

static hi_s32 sample_mcf_color_to_mono_path(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vpss_grp vpss_mono_grp;
    hi_vpss_grp vpss_color_grp;
    hi_vpss_grp_attr vpss_grp_attr;

    /* start mono */
    ret = sample_mcf_start_vi0(&vi_cfg[0]);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    vpss_mono_grp = mcf_grp_attr->mono_pipe_attr.vpss_grp;
    sample_common_mcf_set_default_vpss_attr(&vpss_grp_attr, mcf_grp_attr, HI_TRUE);
    ret = hi_mpi_vpss_create_grp(vpss_mono_grp, &vpss_grp_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_create_grp failed with %#x!\n", ret);
    }

    ret = hi_mpi_vpss_start_grp(vpss_mono_grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_start_grp failed with %#x!\n", ret);
    }
    /* switch */
    sample_mcf_set_path(mcf_grp, HI_MCF_PATH_MONO);

    /* stop color */
    vpss_color_grp = mcf_grp_attr->color_pipe_attr.vpss_grp;
    ret = sample_common_mcf_stop_vpss_one_grp(vpss_color_grp);

    sample_comm_vi_stop_vi(&vi_cfg[1]);

    return ret;
}

static hi_s32 sample_mcf_mono_to_fusion_path(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vpss_grp vpss_color_grp;

    /* start color */
    ret = sample_mcf_start_vi1(&vi_cfg[1]);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_mcf_sensor_cfg(mcf_grp_attr);

    vpss_color_grp = mcf_grp_attr->color_pipe_attr.vpss_grp;
    sample_mcf_start_vpss_color_grp(vpss_color_grp, mcf_grp_attr);

    /* switch */
    sample_mcf_set_path(mcf_grp, HI_MCF_PATH_FUSION);

    sample_mcf_start_mcf_vi_cfg(mcf_grp);

    return ret;
}

static hi_s32 sample_mcf_fusion_to_mono_path(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vpss_grp vpss_color_grp;

    /* switch */
    sample_mcf_stop_mcf_vi_cfg(mcf_grp);
    sample_mcf_set_path(mcf_grp, HI_MCF_PATH_MONO);

    /* stop color */
    vpss_color_grp = mcf_grp_attr->color_pipe_attr.vpss_grp;
    ret = sample_common_mcf_stop_vpss_one_grp(vpss_color_grp);

    sample_comm_vi_stop_vi(&vi_cfg[1]);
    return ret;
}

static hi_s32 sample_mcf_mono_to_color_path(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vpss_grp vpss_mono_grp;
    hi_vpss_grp vpss_color_grp;

    /* start color */
    ret = sample_mcf_start_vi1(&vi_cfg[1]);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_mcf_sensor_cfg(mcf_grp_attr);

    vpss_color_grp = mcf_grp_attr->color_pipe_attr.vpss_grp;
    sample_mcf_start_vpss_color_grp(vpss_color_grp, mcf_grp_attr);

    /* switch */
    sample_mcf_set_path(mcf_grp, HI_MCF_PATH_COLOR);

    /* stop mono */
    vpss_mono_grp = mcf_grp_attr->mono_pipe_attr.vpss_grp;
    ret = sample_common_mcf_stop_vpss_one_grp(vpss_mono_grp);

    sample_comm_vi_stop_vi(&vi_cfg[0]);

    return ret;
}

static hi_s32 sample_mcf_color_to_fusion_path(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_vpss_grp vpss_mono_grp;
    hi_vpss_grp_attr vpss_grp_attr;

    /* start mono */
    ret = sample_mcf_start_vi0(&vi_cfg[0]);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    vpss_mono_grp = mcf_grp_attr->mono_pipe_attr.vpss_grp;
    sample_common_mcf_set_default_vpss_attr(&vpss_grp_attr, mcf_grp_attr, HI_TRUE);
    ret = hi_mpi_vpss_create_grp(vpss_mono_grp, &vpss_grp_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_create_grp failed with %#x!\n", ret);
    }

    ret = hi_mpi_vpss_start_grp(vpss_mono_grp);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vpss_start_grp failed with %#x!\n", ret);
    }

    /* switch */
    sample_mcf_set_path(mcf_grp, HI_MCF_PATH_FUSION);

    sample_mcf_start_mcf_vi_cfg(mcf_grp);

    return ret;
}

static hi_s32 sample_mcf_path_switch_sync_true(hi_mcf_grp mcf_grp)
{
    hi_s32 ret = HI_SUCCESS;

    sample_get_char("switch to fusion to color path! sync: TRUE");
    ret = sample_mcf_set_path(mcf_grp, HI_MCF_PATH_COLOR);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to color to mono path! sync: TRUE");
    ret = sample_mcf_set_path(mcf_grp, HI_MCF_PATH_MONO);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to mono to fusion path! sync: TRUE");
    ret = sample_mcf_set_path(mcf_grp, HI_MCF_PATH_FUSION);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to fusion to mono path! sync: TRUE");
    ret = sample_mcf_set_path(mcf_grp, HI_MCF_PATH_MONO);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to mono to color path! sync: TRUE");
    ret = sample_mcf_set_path(mcf_grp, HI_MCF_PATH_COLOR);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to color to fusion path! sync: TRUE");
    ret = sample_mcf_set_path(mcf_grp, HI_MCF_PATH_FUSION);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    return ret;
}

static hi_s32 sample_mcf_path_switch_sync_false(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;

    sample_get_char("switch to fusion to color path! sync: FALSE");
    ret = sample_mcf_fusion_to_color_path(mcf_grp, mcf_grp_attr, vi_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to color to mono path! sync: FALSE");
    ret = sample_mcf_color_to_mono_path(mcf_grp, mcf_grp_attr, vi_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to mono to fusion path! sync: FALSE");
    ret = sample_mcf_mono_to_fusion_path(mcf_grp, mcf_grp_attr, vi_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to fusion to mono path! sync: FALSE");
    ret = sample_mcf_fusion_to_mono_path(mcf_grp, mcf_grp_attr, vi_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to mono to color path! sync: FALSE");
    ret = sample_mcf_mono_to_color_path(mcf_grp, mcf_grp_attr, vi_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_get_char("switch to color to fusion path! sync: FALSE");
    ret = sample_mcf_color_to_fusion_path(mcf_grp, mcf_grp_attr, vi_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }
    return ret;
}

static hi_s32 sample_mcf_path_switch(hi_mcf_grp mcf_grp, const hi_mcf_grp_attr *mcf_grp_attr,
    sample_vi_cfg *vi_cfg)
{
    hi_s32 ret = HI_SUCCESS;

    if (mcf_grp_attr->sync_pipe_en == HI_TRUE) {
        ret = sample_mcf_path_switch_sync_true(mcf_grp);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    } else {
        ret = sample_mcf_path_switch_sync_false(mcf_grp, mcf_grp_attr, vi_cfg);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }
    return ret;
}

static hi_s32 sample_mcf_two_vi_init_start(sample_vi_cfg *vi_cfg, const hi_mcf_grp mcf_grp,
    hi_mcf_grp_attr *mcf_grp_attr, hi_size *large_size, hi_bool fov_en)
{
    hi_s32 ret;
    hi_vi_vpss_mode_type mode_type = HI_VI_OFFLINE_VPSS_OFFLINE;
    hi_vi_aiisp_mode aiisp_mode = HI_VI_AIISP_MODE_DEFAULT;
    hi_size mono_size, color_size;

    sample_mcf_get_size_by_sns_type(SENSOR0_TYPE, &mono_size);
    sample_mcf_get_size_by_sns_type(SENSOR1_TYPE, &color_size);

    sample_mcf_get_large_size(mono_size, color_size, large_size);
    ret = sample_mcf_sys_init(mode_type, aiisp_mode, large_size, fov_en);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    sample_common_mcf_get_default_grp_attr(mcf_grp_attr, &mono_size, &color_size);
    sample_mcf_set_chn_default_outsize(large_size);

    ret = sample_mcf_start_vi0(&vi_cfg[0]);
    if (ret != HI_SUCCESS) {
        goto release_mem;
    }
    ret = sample_mcf_start_vi1(&vi_cfg[1]);
    if (ret != HI_SUCCESS) {
        goto stop_vi0;
    }

    sample_mcf_sensor_cfg(mcf_grp_attr);

    return HI_SUCCESS;

stop_vi0:
    sample_comm_vi_stop_vi(&vi_cfg[0]);
release_mem:
    sample_comm_sys_exit();
    return ret;
}

static hi_s32 sample_mcf_region_do_destroy(hi_s32 handle_num, hi_rgn_type type,
    hi_mpp_chn *chn, region_op_flag flag)
{
    hi_s32 ret = HI_SUCCESS;

    if (flag & REGION_OP_CHN) {
        ret = sample_comm_region_detach(handle_num, type, chn, flag);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_region_detach failed!\n");
        }
    } else if (flag & REGION_OP_DEV) {
        ret = sample_comm_region_detach(handle_num, type, chn, flag);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_region_detach failed!\n");
        }
    }

    if (flag & REGION_DESTROY) {
        ret = sample_comm_region_destroy(handle_num, type);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_region_destroy failed!\n");
        }
    }

    return ret;
}

static hi_s32 sample_mcf_start_region(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn,
    region_op_flag op_flag)
{
    hi_s32 i, ret, min_handle;
    rgn_check_handle_num_return(handle_num);
    ret = sample_comm_region_create(handle_num, type);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_region_create failed!\n");
        sample_mcf_region_do_destroy(handle_num, type, chn, REGION_DESTROY);
        return ret;
    }
    ret = sample_comm_region_attach(handle_num, type, chn, op_flag);
    if (ret != HI_SUCCESS) {
        if (op_flag & REGION_OP_CHN) {
            sample_print("sample_comm_region_attach failed!\n");
            sample_mcf_region_do_destroy(handle_num, type, chn, op_flag | REGION_DESTROY);
            return ret;
        } else if (op_flag & REGION_OP_DEV) {
            sample_print("sample_comm_region_attach_to_dev failed!\n");
            sample_mcf_region_do_destroy(handle_num, type, chn, op_flag | REGION_DESTROY);
            return ret;
        }
    }
    min_handle = sample_comm_region_get_min_handle(type);
    if (sample_comm_check_min(min_handle) != HI_SUCCESS) {
        sample_print("min_handle(%d) should be in [0, %d).\n", min_handle, HI_RGN_HANDLE_MAX);
        sample_mcf_region_do_destroy(handle_num, type, chn, op_flag | REGION_DESTROY);
        return HI_FAILURE;
    }
    if (type == HI_RGN_OVERLAY) {
        for (i = min_handle; i < min_handle + handle_num; i++) {
            ret = sample_comm_region_set_bit_map(i, g_mcf_path_bmp);
            if (ret != HI_SUCCESS) {
                sample_print("sample_comm_region_set_bit_map failed!\n");
                sample_mcf_region_do_destroy(handle_num, type, chn, op_flag | REGION_DESTROY);
                return ret;
            }
        }
    } else if (type == HI_RGN_OVERLAYEX) {
        for (i = min_handle; i < min_handle + handle_num; i++) {
            ret = sample_comm_region_get_up_canvas(i, g_mcf_path_bmp);
            if (ret != HI_SUCCESS) {
                sample_print("sample_comm_region_get_up_canvas failed!\n");
                sample_mcf_region_do_destroy(handle_num, type, chn, op_flag | REGION_DESTROY);
                return ret;
            }
        }
    }
    sample_get_char("exit");

    return sample_mcf_region_do_destroy(handle_num, type, chn, op_flag | REGION_DESTROY);
}

static hi_s32 sample_mcf_start_cover_region(hi_mcf_grp mcf_grp)
{
    hi_s32 handle_num;
    hi_rgn_type type;
    hi_mpp_chn chn;

    handle_num = MCF_RGN_HANDLE_NUM_8;
    type = HI_RGN_COVER;
    chn.mod_id = HI_ID_MCF;
    chn.dev_id = mcf_grp;
    chn.chn_id = g_mcf_chn_enable[0];

    return sample_mcf_start_region(handle_num, type, &chn, REGION_OP_DEV);
}

static hi_s32 sample_mcf_two_sensor_vo_venc(hi_void)
{
    hi_s32 ret;
    const hi_mcf_grp mcf_grp = 0;
    hi_mcf_grp_attr mcf_grp_attr;
    sample_vi_cfg vi_cfg[2];    /* vi config of two sensors */
    hi_mcf_crop_info grp_crop = { 0 }; /* do not use mcf grp crop function */
    hi_fov_attr fov_correction_attr = { 0 };
    hi_size large_size;

    ret = sample_mcf_two_vi_init_start(vi_cfg, mcf_grp, &mcf_grp_attr, &large_size, HI_FALSE);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = sample_mcf_start_mcf_data_flow(mcf_grp, &mcf_grp_attr, large_size, &grp_crop, &fov_correction_attr);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    sample_get_char("set region");
    ret = sample_mcf_start_cover_region(mcf_grp);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    sample_mcf_stop_mcf_data_flow(mcf_grp, &mcf_grp_attr);
exit:
    sample_comm_vi_stop_vi(&vi_cfg[0]);
    sample_comm_vi_stop_vi(&vi_cfg[1]);
    sample_comm_sys_exit();
    return ret;
}

static hi_s32 sample_mcf_switch(hi_bool sync_enable)
{
    hi_s32 ret;
    const hi_mcf_grp mcf_grp = 0;
    hi_mcf_grp_attr mcf_grp_attr;
    sample_vi_cfg vi_cfg[2];    /* vi config of two sensors */
    hi_mcf_crop_info grp_crop = { 0 }; /* do not use mcf grp crop function */
    hi_fov_attr fov_correction_attr = { 0 };
    hi_size large_size;

    ret = sample_mcf_two_vi_init_start(vi_cfg, mcf_grp, &mcf_grp_attr, &large_size, HI_FALSE);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    mcf_grp_attr.sync_pipe_en = sync_enable;
    ret = sample_mcf_start_mcf_data_flow(mcf_grp, &mcf_grp_attr, large_size, &grp_crop, &fov_correction_attr);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    ret = sample_mcf_path_switch(mcf_grp, &mcf_grp_attr, vi_cfg);
    if (ret == HI_SUCCESS) {
        sample_get_char("exit");
    }

    sample_mcf_stop_mcf_data_flow(mcf_grp, &mcf_grp_attr);
exit:
    sample_comm_vi_stop_vi(&vi_cfg[0]);
    sample_comm_vi_stop_vi(&vi_cfg[1]);
    sample_comm_sys_exit();
    return ret;
}

static hi_s32 sample_mcf_correction_vo_venc()
{
    hi_s32 ret;
    const hi_mcf_grp mcf_grp = 0;
    hi_mcf_grp_attr mcf_grp_attr;
    sample_vi_cfg vi_cfg[2];    /* vi config of two sensors */
    hi_mcf_crop_info grp_crop = { 0 }; /* do not use mcf grp crop function */
    hi_fov_attr fov_correction_attr = { 0 };
    hi_size large_size;

    ret = sample_mcf_two_vi_init_start(vi_cfg, mcf_grp, &mcf_grp_attr, &large_size, HI_TRUE);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (mcf_grp_attr.mono_pipe_attr.height < mcf_grp_attr.color_pipe_attr.height ||
        mcf_grp_attr.mono_pipe_attr.width < mcf_grp_attr.color_pipe_attr.width) {
        sample_print("mono pipe size need to be not less than color pipe size when calibrating online!\n");
        goto exit;
    }

    printf("---------------start calibration---------------\n");
    fov_correction_attr.enable = HI_TRUE;
    ret = sample_mcf_calibrate_online(mcf_grp, &mcf_grp_attr, &grp_crop, &fov_correction_attr);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    ret = sample_mcf_start_mcf_data_flow(mcf_grp, &mcf_grp_attr, large_size, &grp_crop, &fov_correction_attr);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    sample_get_char("exit");
    sample_mcf_stop_mcf_data_flow(mcf_grp, &mcf_grp_attr);
exit:
    sample_comm_vi_stop_vi(&vi_cfg[0]);
    sample_comm_vi_stop_vi(&vi_cfg[1]);
    sample_comm_sys_exit();
    return ret;
}


static hi_void sample_mcf_usage(const char *prg_name)
{
    printf("usage : %s <index> \n", prg_name);
    printf("index :\n");
    printf("    (0) mcf calib                 : mcf same img_size calib \n");
    printf("    (1) mcf calib                 : mcf diff img_size calib \n");
    printf("    (2) mcf base mode             : vi->vpss->mcf->vpss->venc && vo.\n");
    printf("    (3) mcf switch with sync      : mcf switch to daytime and night without destory\n");
    printf("    (4) mcf switch without sync   : mcf switch to daytime and night with destory\n");
    printf("    (5) mcf correction route      : mcf correct online, "
        "you'd better keep still while calibrating.\n");
}

static hi_void sample_mcf_handle_sig(hi_s32 signo)
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

static hi_s32 sample_mcf_execute_case(hi_u32 case_index)
{
    hi_s32 ret;

    switch (case_index) {
        case 0: /* 0 mcf calib */
            ret = sample_mcf_calibration();
            break;
        case 1: /* 1 mcf calib */
            ret = sample_mcf_diff_img_size_calibration();
            break;
        case 2: /* 2 two same sensor route */
            ret = sample_mcf_two_sensor_vo_venc();
            break;
        case 3: /* 3 mcf switch to daytime and night without destory. */
            ret = sample_mcf_switch(HI_TRUE);
            break;
        case 4: /* 4 mcf switch to daytime and night with destory. */
            ret = sample_mcf_switch(HI_FALSE);
            break;
        case 5: /* 5 mcf calib, and do correction. */
            ret = sample_mcf_correction_vo_venc();
            break;
        default:
            ret = HI_FAILURE;
            break;
    }
    return ret;
}

#ifdef __LITEOS__
hi_s32 app_main(hi_s32 argc, hi_char *argv[])
#else
hi_s32 main(hi_s32 argc, hi_char *argv[])
#endif
{
    hi_s32 ret;
    hi_u32 index;
    hi_slong val;
    hi_char *end_ptr = HI_NULL;

    if (argc != 2) { /* 2:arg num */
        sample_mcf_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_mcf_usage(argv[0]);
        return HI_FAILURE;
    }

    if ((strlen(argv[1]) != 1) ||
        (argv[1][0] < '0' || argv[1][0] > '5')) {
        sample_mcf_usage(argv[0]);
        return HI_FAILURE;
    }

#ifndef __LITEOS__
    sample_register_sig_handler(sample_mcf_handle_sig);
#endif

    errno = 0;
    val = strtol(argv[1], &end_ptr, MCF_SAMPLE_ARG_NUMBER_BASE);
    if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) {
        return HI_FAILURE;
    }
    if ((end_ptr == argv[1]) || (*end_ptr != '\0')) {
        return HI_FAILURE;
    }
    index = (hi_u32)val;

    ret = sample_mcf_execute_case(index);
    if ((ret == HI_SUCCESS) && (g_sig_flag == 0)) {
        sample_print("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        sample_print("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

#ifdef __LITEOS__
    return ret;
#else
    exit(ret);
#endif
}
