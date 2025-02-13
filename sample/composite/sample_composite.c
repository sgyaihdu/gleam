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

#include "sample_comm.h"

#define PIC_SIZE   PIC_1080P
#define BIG_STREAM_SIZE     PIC_1080P
#define SMALL_STREAM_SIZE   PIC_D1_NTSC
#define DEFAULT_WAIT_TIME   20000
#define VB_MAX_NUM     10

#define ENTER_ASCII 10

#define SAMPLE_VDEC_MAX_NUM   1

#define VI_DEV_ID       0
#define VI_PIPE_ID      0
#define VENC_CHN_ID     0
#define VDEC_CHN_ID     0
#define VO_CHN_ID       0
#define VPSS_CHN_DEC_ID 0
#define VPSS_CHN_0      0
#define VPSS_CHN_1      1
#define VPSS_CHN_2      2
#define VO_LAYER        0
#define VPSS_GRP        0
#define VPSS_GRP_DEC    1
#define VPSS_CHN_NUM    2
#define CHN_NUM_MAX    2

#define VI_VB_YUV_CNT    6
#define VPSS_VB_YUV_CNT  11

#define DEFAULT_VDEC_WAIT_TIME (-1)

typedef struct {
    hi_size            max_size;
    hi_pixel_format    pixel_format;
    hi_size            output_size[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_compress_mode   compress_mode[HI_VPSS_MAX_PHYS_CHN_NUM];
    hi_bool            enable[HI_VPSS_MAX_PHYS_CHN_NUM];
} sample_composite_vpss_chn_attr;

typedef struct {
    hi_bool            chn_num;
    hi_payload_type    type[SAMPLE_VDEC_MAX_NUM];
    hi_size            size[SAMPLE_VDEC_MAX_NUM];
    hi_u32             ref_frame_num[SAMPLE_VDEC_MAX_NUM];
    hi_u32             display_frame_num[SAMPLE_VDEC_MAX_NUM];
    hi_bool            frame_buf_cnt[SAMPLE_VDEC_MAX_NUM];
    hi_bool            composite_dec_en[SAMPLE_VDEC_MAX_NUM];
} sample_composite_vdec_chn_attr;

typedef struct {
    hi_u32            valid_num;
    hi_u64            blk_size[HI_VB_MAX_COMMON_POOLS];
    hi_u32            blk_cnt[HI_VB_MAX_COMMON_POOLS];
    hi_u32            supplement_config;
} sample_composite_vb_attr;

typedef struct {
    hi_size          enc_size[VPSS_CHN_NUM];
    hi_pic_size      enc_pic_size[VPSS_CHN_NUM];
    hi_payload_type  payload[CHN_NUM_MAX];
    sample_vi_cfg    vi_cfg;
    sample_vo_cfg    vo_cfg;
    hi_size          vi_size;
    hi_s32           vdec_chn_num;
    hi_venc_chn      venc_chn[CHN_NUM_MAX];
    sample_composite_vpss_chn_attr vpss_param[VPSS_CHN_NUM];
    sample_composite_vdec_chn_attr vdec_param;
    sample_composite_vb_attr       vb_attr;
} sample_composite_param;

static hi_bool g_send_multi_frame_signal = HI_FALSE;
static hi_bool g_stop_get_stream_signal = HI_FALSE;
static pthread_t g_send_multi_frame_thread = 0;
static pthread_t g_get_stream_thread = 0;
FILE *g_dbg_file = HI_NULL;

/******************************************************************************
* function : show usage
******************************************************************************/
void sample_composite_usage(const char *prg_nm)
{
    printf("Usage : %s [index] \n", prg_nm);
    printf("index:\n");
    printf("\t  0) Composite venc + vdec.\n");
    printf("\t  1) Composite venc + save stream.\n");

    return;
}

static hi_bool g_sample_venc_exit = HI_FALSE;

/******************************************************************************
* function : to process abnormal case
******************************************************************************/
hi_void sample_composite_handle_sig(hi_s32 signo)
{
    if (g_sample_venc_exit == HI_TRUE) {
        return;
    }

    if (signo == SIGINT || signo == SIGTERM) {
        g_sample_venc_exit = HI_TRUE;
    }
}

static hi_s32 sample_venc_getchar()
{
    hi_s32 c;
    if (g_sample_venc_exit == HI_TRUE) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return 'e';
    }

    c = getchar();

    if (g_sample_venc_exit == HI_TRUE) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        return 'e';
    }

    return c;
}

static hi_s32 sample_clear_invalid_ch()
{
    hi_s32 c;

    while ((c = sample_venc_getchar()) != ENTER_ASCII) {
    }
    return HI_SUCCESS;
}

static hi_s32 sample_venc_set_gop_mode(hi_s32 c, hi_venc_gop_mode *gop_mode)
{
    switch (c) {
        case '0':
            *gop_mode = HI_VENC_GOP_MODE_NORMAL_P;
            break;

        case '1':
            *gop_mode = HI_VENC_GOP_MODE_SMART_P;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static td_void print_gop_mode()
{
    printf("please input choose gop mode!\n");
    printf("\t 0) normal p.\n");
    printf("\t 1) smart p.\n");
}

static hi_s32 get_gop_mode(hi_venc_gop_mode *gop_mode)
{
    hi_s32 c;

    while (HI_TRUE) {
        print_gop_mode();
        c = sample_venc_getchar();
        if (c == 'e' && sample_venc_getchar() == ENTER_ASCII) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (sample_venc_set_gop_mode(c, gop_mode) == HI_SUCCESS &&
            sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }

        sample_clear_invalid_ch();
        sample_print("invalid input! please try again.\n");
    }

    return HI_SUCCESS;
}

static hi_void print_rc_mode(hi_payload_type type)
{
    printf("please input choose rc mode!\n");
    printf("\t c) cbr.\n");
    printf("\t v) vbr.\n");
    if (type != HI_PT_MJPEG) {
        printf("\t a) avbr.\n");
        printf("\t x) cvbr.\n");
        printf("\t q) qvbr.\n");
    }
    printf("\t f) fix_qp\n");
}

static hi_s32 sample_set_rc_mode(hi_s32 c, sample_rc *rc_mode)
{
    switch (c) {
        case 'c':
            *rc_mode = SAMPLE_RC_CBR;
            break;

        case 'v':
            *rc_mode = SAMPLE_RC_VBR;
            break;

        case 'a':
            *rc_mode = SAMPLE_RC_AVBR;
            break;

        case 'x':
            *rc_mode = SAMPLE_RC_CVBR;
            break;

        case 'q':
            *rc_mode = SAMPLE_RC_QVBR;
            break;

        case 'f':
            *rc_mode = SAMPLE_RC_FIXQP;
            break;

        default:
            return HI_FAILURE;
    }
    return HI_SUCCESS;
}

hi_s32 get_rc_mode(hi_payload_type type, sample_rc *rc_mode)
{
    hi_s32 c;

    if (type == HI_PT_JPEG) {
        return HI_SUCCESS;
    }

    while (HI_TRUE) {
        print_rc_mode(type);
        c = sample_venc_getchar();
        if (c == 'e' && sample_venc_getchar() == ENTER_ASCII) {
            return HI_FAILURE;
        }

        if (c == ENTER_ASCII) {
            sample_print("invalid input! please try again.\n");
            continue;
        } else if (sample_set_rc_mode(c, rc_mode) == HI_SUCCESS &&
            sample_venc_getchar() == ENTER_ASCII) {
            return HI_SUCCESS;
        }
        sample_print("invalid input! please try again.\n");
        sample_clear_invalid_ch();
    }

    return HI_SUCCESS;
}

static hi_void get_vb_attr(const hi_size *vi_size, const sample_composite_vpss_chn_attr *vpss_chn_attr,
    sample_composite_vb_attr *vb_attr)
{
    hi_s32 i;
    hi_pic_buf_attr pic_buf_attr = {0};

    vb_attr->valid_num = 0;

    // vb for vi-vpss
    pic_buf_attr.width  = vi_size->width;
    pic_buf_attr.height = vi_size->height;
    pic_buf_attr.align = HI_DEFAULT_ALIGN;
    pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    pic_buf_attr.pixel_format = HI_PIXEL_FORMAT_YUV_SEMIPLANAR_422;
    pic_buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    vb_attr->blk_size[vb_attr->valid_num] = hi_common_get_pic_buf_size(&pic_buf_attr);
    vb_attr->blk_cnt[vb_attr->valid_num] = VI_VB_YUV_CNT; // 10: vb num
    vb_attr->valid_num++;

    // vb for vpss-venc(big stream)
    if (vb_attr->valid_num >= HI_VB_MAX_COMMON_POOLS) {
        return;
    }

    for (i = 0; i < HI_VPSS_MAX_PHYS_CHN_NUM && vb_attr->valid_num < HI_VB_MAX_COMMON_POOLS; i++) {
        if (vpss_chn_attr->enable[i] == HI_TRUE) {
            pic_buf_attr.width = vpss_chn_attr->output_size[i].width;
            pic_buf_attr.height = vpss_chn_attr->output_size[i].height;
            pic_buf_attr.align = HI_DEFAULT_ALIGN;
            pic_buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
            pic_buf_attr.pixel_format = vpss_chn_attr->pixel_format;
            pic_buf_attr.compress_mode = vpss_chn_attr->compress_mode[i];
            vb_attr->blk_size[vb_attr->valid_num] = hi_common_get_pic_buf_size(&pic_buf_attr);
            vb_attr->blk_cnt[vb_attr->valid_num] = VPSS_VB_YUV_CNT;
            vb_attr->valid_num++;
        }
    }
}

static hi_void get_default_vo_cfg(sample_vo_cfg *vo_cfg, hi_size enc_size)
{
    sample_comm_vo_get_def_config(vo_cfg);

    vo_cfg->intf_sync  = HI_VO_OUT_1080P50;
    vo_cfg->disp_rect.x  = 0;
    vo_cfg->disp_rect.y  = 0;
    vo_cfg->disp_rect.width = enc_size.width;
    vo_cfg->disp_rect.height = enc_size.height;
    vo_cfg->image_size.width = enc_size.width;
    vo_cfg->image_size.height = enc_size.height;
    vo_cfg->vo_mode   = VO_MODE_1MUX;
    vo_cfg->bg_color  = COLOR_RGB_BLACK;
}

static hi_void get_default_vpss_chn_attr(hi_size vi_size, hi_size enc_size[], hi_s32 len,
    sample_composite_vpss_chn_attr *vpss_chan_attr)
{
    hi_s32 i;
    hi_u32 max_width;
    hi_u32 max_height;

    (hi_void)memset_s(vpss_chan_attr, sizeof(sample_composite_vpss_chn_attr), 0,
        sizeof(sample_composite_vpss_chn_attr));

    max_width = vi_size.width;
    max_height = vi_size.height;

    for (i = 0; (i < len) && (i < HI_VPSS_MAX_PHYS_CHN_NUM); i++) {
        vpss_chan_attr->output_size[i].width  = enc_size[i].width;
        vpss_chan_attr->output_size[i].height = enc_size[i].height;
        vpss_chan_attr->compress_mode[i]      = HI_COMPRESS_MODE_NONE;
        vpss_chan_attr->enable[i]             = HI_TRUE;

        max_width = MAX2(max_width, enc_size[i].width);
        max_height = MAX2(max_height, enc_size[i].height);
    }

    vpss_chan_attr->max_size.width  = max_width;
    vpss_chan_attr->max_size.height = max_height;
    vpss_chan_attr->pixel_format    = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    return;
}

static hi_void get_default_vdec_chn_attr(hi_size enc_size[], hi_s32 len, hi_payload_type payload[],
    hi_s32 pt_len, sample_composite_vdec_chn_attr *vdec_chan_attr)
{
    hi_s32 i;

    for (i = 0; (i < len) && (i < SAMPLE_VDEC_MAX_NUM) && (i < pt_len); i++) {
        vdec_chan_attr->type[i] = payload[i];
        vdec_chan_attr->size[i].width = enc_size[i].width;
        vdec_chan_attr->size[i].height = enc_size[i].height;
        vdec_chan_attr->ref_frame_num[i]       = 6;   /* 6 : shvc ref num */
        vdec_chan_attr->display_frame_num[i]   = 1;   /* 1 : display_frame_num */
        vdec_chan_attr->frame_buf_cnt[i]       = 6;   /* 6 : frame_buf_cnt */
    }

    vdec_chan_attr->chn_num = len > SAMPLE_VDEC_MAX_NUM ? SAMPLE_VDEC_MAX_NUM : len;
}

hi_s32 sample_composite_sys_init(sample_composite_vb_attr *vb_attr)
{
    hi_u32 i;
    hi_s32 ret;
    hi_vb_cfg vb_cfg = {0};

    if (vb_attr->valid_num > HI_VB_MAX_COMMON_POOLS) {
        sample_print("sample_composite_sys_init vb valid num(%u) too large than HI_VB_MAX_COMMON_POOLS(%d)!\n",
            vb_attr->valid_num, HI_VB_MAX_COMMON_POOLS);
        return HI_FAILURE;
    }

    for (i = 0; i < vb_attr->valid_num; i++) {
        vb_cfg.common_pool[i].blk_size = vb_attr->blk_size[i];
        vb_cfg.common_pool[i].blk_cnt = vb_attr->blk_cnt[i];
    }

    vb_cfg.max_pool_cnt = vb_attr->valid_num;

    if (vb_attr->supplement_config == 0) {
        ret = sample_comm_sys_vb_init(&vb_cfg);
    } else {
        ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, vb_attr->supplement_config);
    }

    if (ret != HI_SUCCESS) {
        sample_print("sample_composite_sys_init failed!\n");
    }

    return ret;
}

hi_s32 sample_composite_vi_init(sample_vi_cfg *vi_cfg)
{
    hi_s32 ret;

    ret = sample_comm_vi_start_vi(vi_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vi_start_vi failed: 0x%x\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

hi_void sample_composite_vi_deinit(sample_vi_cfg *vi_cfg)
{
    sample_comm_vi_stop_vi(vi_cfg);
}

hi_s32 sample_composite_vpss_init(hi_vpss_grp vpss_grp, sample_composite_vpss_chn_attr *vpss_chan_cfg)
{
    hi_s32 ret;
    hi_vpss_chn vpss_chn;
    hi_vpss_grp_attr grp_attr = {0};
    sample_vpss_chn_attr vpss_chn_attr = {0};

    grp_attr.max_width  = vpss_chan_cfg->max_size.width;
    grp_attr.max_height = vpss_chan_cfg->max_size.height;
    grp_attr.dei_mode = HI_VPSS_DEI_MODE_OFF;
    grp_attr.pixel_format = vpss_chan_cfg->pixel_format;
    grp_attr.frame_rate.src_frame_rate = -1;
    grp_attr.frame_rate.dst_frame_rate = -1;

    for (vpss_chn = 0; vpss_chn < HI_VPSS_MAX_PHYS_CHN_NUM; vpss_chn++) {
        if (vpss_chan_cfg->enable[vpss_chn] == 1) {
            vpss_chn_attr.chn_attr[vpss_chn].width                   = vpss_chan_cfg->output_size[vpss_chn].width;
            vpss_chn_attr.chn_attr[vpss_chn].height                  = vpss_chan_cfg->output_size[vpss_chn].height;
            vpss_chn_attr.chn_attr[vpss_chn].chn_mode                = HI_VPSS_CHN_MODE_USER;
            vpss_chn_attr.chn_attr[vpss_chn].compress_mode           = vpss_chan_cfg->compress_mode[vpss_chn];
            vpss_chn_attr.chn_attr[vpss_chn].pixel_format            = vpss_chan_cfg->pixel_format;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.src_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].frame_rate.dst_frame_rate = -1;
            vpss_chn_attr.chn_attr[vpss_chn].depth                     = 2;    /* 2 : user mode */
            vpss_chn_attr.chn_attr[vpss_chn].mirror_en                 = 0;
            vpss_chn_attr.chn_attr[vpss_chn].flip_en                   = 0;
            vpss_chn_attr.chn_attr[vpss_chn].aspect_ratio.mode         = HI_ASPECT_RATIO_NONE;
        }
    }

    memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
        vpss_chan_cfg->enable, sizeof(vpss_chn_attr.chn_enable));
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
    ret = sample_common_vpss_start(vpss_grp, &grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("start vpss failed with %#x!\n", ret);
    }

    return ret;
}

hi_void sample_composite_vpss_deinit(hi_vpss_grp vpss_grp, sample_composite_vpss_chn_attr *vpss_chan_cfg)
{
    hi_s32 ret;

    ret = sample_common_vpss_stop(vpss_grp, vpss_chan_cfg->enable, HI_VPSS_MAX_PHYS_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
    }
}

hi_s32 sample_composite_vdec_init(sample_composite_vdec_chn_attr *vdec_chan_cfg)
{
    hi_s32 ret = HI_SUCCESS;
    hi_u32 i;
    sample_vdec_attr vdec_attr[SAMPLE_VDEC_MAX_NUM];
    hi_vdec_chn_param vdec_chn_param = {0};

    for (i = 0; (i < vdec_chan_cfg->chn_num) && (i < SAMPLE_VDEC_MAX_NUM); i++) {
        (hi_void)memset_s(&vdec_attr[i], sizeof(vdec_attr), 0, sizeof(sample_vdec_attr));
        vdec_attr[i].type                           = vdec_chan_cfg->type[i];
        vdec_attr[i].width                          = vdec_chan_cfg->size[i].width;
        vdec_attr[i].height                         = vdec_chan_cfg->size[i].height;
        vdec_attr[i].mode                            = HI_VDEC_SEND_MODE_FRAME;
        vdec_attr[i].sample_vdec_video.dec_mode      = HI_VIDEO_DEC_MODE_IP;
        vdec_attr[i].sample_vdec_video.bit_width     = HI_DATA_BIT_WIDTH_8;
        vdec_attr[i].sample_vdec_video.ref_frame_num = vdec_chan_cfg->ref_frame_num[i];
        vdec_attr[i].display_frame_num               = vdec_chan_cfg->display_frame_num[i];
        vdec_attr[i].frame_buf_cnt                   = vdec_chan_cfg->frame_buf_cnt[i];

        ret = sample_comm_vdec_init_vb_pool(vdec_chan_cfg->chn_num, &vdec_attr[i], SAMPLE_VDEC_MAX_NUM);
        if (ret != HI_SUCCESS) {
            sample_print("init mod common vb fail for %#x!\n", ret);
            return ret;
        }

        ret = sample_comm_vdec_start(vdec_chan_cfg->chn_num, &vdec_attr[i], SAMPLE_VDEC_MAX_NUM);
        if (ret != HI_SUCCESS) {
            sample_print("start Vdec fail for %#x!\n", ret);
            sample_comm_vdec_exit_vb_pool();
            return ret;
        }

        if (hi_mpi_vdec_get_chn_param(VDEC_CHN_ID, &vdec_chn_param) == HI_SUCCESS) {
            vdec_chn_param.video_param.quick_mark_mode = HI_QUICK_MARK_NONE;
            vdec_chn_param.video_param.composite_dec_en = vdec_chan_cfg->composite_dec_en[i];
            ret = hi_mpi_vdec_set_chn_param(VDEC_CHN_ID, &vdec_chn_param);
            if (ret != HI_SUCCESS) {
                sample_print("vdec chn %d set chn param (composite dec = %d)failed! ret = 0x%x\n", VDEC_CHN_ID,
                    vdec_chan_cfg->composite_dec_en[i], ret);
            }
        } else {
            sample_print("vdec chn %d get chn param failed!\n", VDEC_CHN_ID);
        }
    }

    return ret;
}

hi_void sample_composite_vdec_deinit(sample_composite_vdec_chn_attr *vdec_chan_cfg)
{
    sample_comm_vdec_stop(vdec_chan_cfg->chn_num);
    sample_comm_vdec_exit_vb_pool();
}

hi_s32 sample_composite_vo_init(sample_vo_cfg *vo_cfg)
{
    hi_s32 ret;

    ret = sample_comm_vo_start_vo(vo_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_start_vo failed! ret = 0x%x\n", ret);
    }

    return ret;
}

hi_void sample_composite_vo_deinit(sample_vo_cfg *vo_cfg)
{
    sample_comm_vo_stop_vo(vo_cfg);
}

static hi_void set_venc_composite_enable(hi_void)
{
    hi_s32 ret;
    hi_venc_chn_config chn_cfg = {0};

    ret = hi_mpi_venc_get_chn_config(VENC_CHN_ID, &chn_cfg);
    if (ret == HI_SUCCESS) {
        chn_cfg.composite_enc_en = HI_TRUE;
        chn_cfg.mosaic_en = HI_TRUE;
        chn_cfg.quality_level = 1;
        ret = hi_mpi_venc_set_chn_config(VENC_CHN_ID, &chn_cfg);
        if (ret != HI_SUCCESS) {
            sample_print("Venc set chn config (composite encode = %d) failed! ret = 0x%x\n",
                chn_cfg.composite_enc_en, ret);
        }
    } else {
        sample_print("Venc get chn config failed! ret = 0x%x\n", ret);
    }
}

static hi_s32 sample_composite_venc_init(hi_pic_size enc_pic_size)
{
    hi_s32 ret;
    hi_venc_chn      venc_chn[1] = {VENC_CHN_ID};   // just use venc chn 0
    hi_u32           profile[1]  = {0};
    hi_payload_type  payload[1]  = {HI_PT_H265};
    hi_venc_gop_mode gop_mode;
    hi_venc_gop_attr gop_attr;
    sample_rc        rc_mode;
    hi_bool          is_rcn_ref_share = HI_TRUE;
    sample_comm_venc_chn_param venc_create_param = {0};

    if (get_rc_mode(payload[0], &rc_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    if (get_gop_mode(&gop_mode) != HI_SUCCESS) {
        return HI_FAILURE;
    }
    ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Get GopAttr for %#x!\n", ret);
        return HI_FAILURE;
    }

    set_venc_composite_enable();

    venc_create_param.frame_rate = 30; /* 30: is a number */
    venc_create_param.gop = 60; /* 60: is a number */
    venc_create_param.stats_time = 2; /* 2: is a number */
    venc_create_param.type                  = payload[0];
    venc_create_param.size                  = enc_pic_size;
    venc_create_param.rc_mode               = rc_mode;
    venc_create_param.profile               = profile[0];
    venc_create_param.is_rcn_ref_share_buf  = is_rcn_ref_share;
    venc_create_param.gop_attr              = gop_attr;

    ret = sample_comm_venc_start(venc_chn[0], &venc_create_param);
    if (ret != HI_SUCCESS) {
        sample_print("Venc Start failed for %#x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_composite_venc_deinit(hi_void)
{
    sample_comm_venc_stop(VENC_CHN_ID);
}

static hi_void config_mosaic_info(hi_venc_mosaic_info *mosaic_info, hi_phys_addr_t phys_addr)
{
    mosaic_info->mode = HI_VENC_MOSAIC_MODE_MAP;
    mosaic_info->blk_size   = HI_MOSAIC_BLK_SIZE_64;
    mosaic_info->map_param.valid = HI_TRUE;
    mosaic_info->map_param.phys_addr = phys_addr;
    mosaic_info->map_param.specified_yuv_en = HI_FALSE;
    mosaic_info->map_param.pixel_yuv.data_y = 255; // 255: yuv data
    mosaic_info->map_param.pixel_yuv.data_u = 255; // 255: yuv data
    mosaic_info->map_param.pixel_yuv.data_v = 255; // 255: yuv data
}

static hi_u32 venc_trans_blk_size(hi_mosaic_blk_size blk_size)
{
    hi_u32 size;

    switch (blk_size) {
        case HI_MOSAIC_BLK_SIZE_4:
            size = 4; // 4: blk size
            break;

        case HI_MOSAIC_BLK_SIZE_8:
            size = 8; // 8: blk size
            break;

        case HI_MOSAIC_BLK_SIZE_16:
            size = 16; // 16: blk size
            break;

        case HI_MOSAIC_BLK_SIZE_32:
            size = 32; // 32: blk size
            break;

        case HI_MOSAIC_BLK_SIZE_64:
            size = 64; // 64: blk size
            break;

        case HI_MOSAIC_BLK_SIZE_128:
            size = 128; // 128: blk size
            break;

        default:
            size = 0;
            break;
    }

    return size;
}

static hi_void composite_send_multi_frame(hi_phys_addr_t phys_addr)
{
    hi_s32 ret, ret0_get, ret0_rls, ret1_get, ret1_rls;
    hi_video_frame_info mosaic_frm, ori_frm;
    hi_venc_multi_frame_info multi_frm;
    hi_u32 max_time_ref;

    ret0_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm, DEFAULT_WAIT_TIME);
    ret1_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm, DEFAULT_WAIT_TIME);
    if (ret0_get == TD_FAILURE || ret1_get == TD_FAILURE) {
        goto GET_FRAME_ERR;
    }

    max_time_ref = MAX2(mosaic_frm.video_frame.time_ref, ori_frm.video_frame.time_ref);
    if (mosaic_frm.video_frame.time_ref < max_time_ref) {
        ret0_rls = hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm);
        ret0_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm, DEFAULT_WAIT_TIME);
        sample_print("VPSS_CHN0 is lags behind others. rls 0x%x, get 0x%x\n", ret0_rls, ret0_get);
        if (ret0_rls == TD_FAILURE || ret0_get == TD_FAILURE) {
            goto GET_FRAME_ERR;
        }
    }

    if (ori_frm.video_frame.time_ref < max_time_ref) {
        ret1_rls = hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm);
        ret1_get = hi_mpi_vpss_get_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm, DEFAULT_WAIT_TIME);
        sample_print("VPSS_CHN0 is lags behind others. rls 0x%x, get 0x%x\n", ret1_rls, ret1_get);
        if (ret1_rls == TD_FAILURE || ret1_get == TD_FAILURE) {
            goto GET_FRAME_ERR;
        }
    }

    multi_frm.frame[0] = mosaic_frm;
    multi_frm.frame[1] = ori_frm;
    multi_frm.frame_num = 2; /* 2: frame num */
    config_mosaic_info(&multi_frm.mosaic_info, phys_addr);
    ret = hi_mpi_venc_send_multi_frame(VENC_CHN_ID, &multi_frm, DEFAULT_WAIT_TIME);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_send_multi_frame Failed! Error(%#x)\n", ret);
    }

GET_FRAME_ERR:
    if (ret0_get == HI_SUCCESS) {
        hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_0, &mosaic_frm);
    }

    if (ret1_get == HI_SUCCESS) {
        hi_mpi_vpss_release_chn_frame(VPSS_GRP, VPSS_CHN_1, &ori_frm);
    }
}

static hi_void *sample_composite_send_multi_frame_proc(hi_void *p)
{
    hi_s32 ret;
    hi_u32 map_size;
    hi_u32 stride;
    hi_u32 blk_size;
    hi_phys_addr_t phys_addr;
    hi_void *virt_addr = HI_NULL;
    hi_venc_chn_attr attr = { 0 };

    hi_mpi_venc_get_chn_attr(VENC_CHN_ID, &attr);

    blk_size = venc_trans_blk_size(HI_MOSAIC_BLK_SIZE_64);
    stride = hi_venc_get_mosaic_map_stride(attr.venc_attr.pic_width, blk_size);
    map_size = hi_venc_get_mosaic_map_size(attr.venc_attr.pic_width, attr.venc_attr.pic_height, blk_size);

    ret = hi_mpi_sys_mmz_alloc(&phys_addr, &virt_addr, "mosaic_map", HI_NULL, map_size);
    if (ret != HI_SUCCESS) {
        sample_print("alloc mosaic map failed.\n");
        return HI_NULL;
    }

    (hi_void)memset_s(virt_addr, map_size, 0, map_size);
    (hi_void)memset_s(virt_addr, stride, 0xff, stride);
    (hi_void)memset_s(virt_addr + stride * 2, stride, 0xff, stride); /* 2: stride num */

    while (g_send_multi_frame_signal == HI_FALSE) {
        composite_send_multi_frame(phys_addr);
    }

    ret = hi_mpi_sys_mmz_free(phys_addr, virt_addr);
    if (ret != HI_SUCCESS) {
        sample_print("free mosaic map failed.\n");
    }

    return HI_NULL;
}

hi_u32 sample_composite_copy_stream(const hi_venc_stream *stream, hi_u8 *buffer, hi_u32 max_len)
{
    hi_u32 i;
    hi_u32 len = 0;

    for (i = 0; i < stream->pack_cnt; i++) {
        hi_u32 pack_len = stream->pack[i].len - stream->pack[i].offset;
        if (len + pack_len > max_len) {
            sample_print("sample_composite_copy_stream: buffer overflow!\n");
            break;
        }

        if (memcpy_s(buffer + len, max_len - len, stream->pack[i].addr + stream->pack[i].offset, pack_len) != EOK) {
            sample_print("sample_composite_copy_stream: memcpy_s return failed! des len = %u, copylen = %u\n",
                max_len - len, pack_len);
        }
        len += pack_len;
    }

    return len;
}

static hi_void *sample_composite_get_stream_proc(hi_void *p)
{
    hi_u32 len;
    hi_venc_stream venc_stream;
    hi_vdec_stream vdec_stream;
    hi_u8 *buffer = HI_NULL;
    const hi_u32 max_buffer_len = 4000 * 1024; /* 4000,1024: buffer len */

    buffer = (hi_u8 *)malloc(max_buffer_len);
    if (buffer == HI_NULL) {
        sample_print("mallic failed!\n");
        return NULL;
    }

    /*******************************************************
     step 0 : malloc corresponding number of pack nodes.
    *******************************************************/
    (hi_void)memset_s(&venc_stream, sizeof(venc_stream), 0, sizeof(hi_venc_stream));
    venc_stream.pack = (hi_venc_pack *)malloc(sizeof(hi_venc_pack) * 32); /* 32: size */
    if (venc_stream.pack == HI_NULL) {
        sample_print("malloc stream pack failed!\n");
        free(buffer);
        return NULL;
    }

    printf("join SAMPLE_COMPOSITE_GetStreamProc!\n");
    /******************************************
     step 1:  Start to get streams of each channel.
    ******************************************/
    while (g_stop_get_stream_signal == HI_FALSE) {
        /*******************************************************
         step 2.0 : call mpi to get one-frame stream
        *******************************************************/
        venc_stream.pack_cnt = 30; /* 30: size */
        if (hi_mpi_venc_get_stream(VENC_CHN_ID, &venc_stream, DEFAULT_WAIT_TIME) != HI_SUCCESS) {
            break;
        }

        /*******************************************************
         step 2.1 : save frame to file
        *******************************************************/
        len = sample_composite_copy_stream(&venc_stream, buffer, max_buffer_len);

        vdec_stream.addr = buffer;
        vdec_stream.len  = len;
        vdec_stream.need_display = 1;
        vdec_stream.end_of_frame  = 1;
        vdec_stream.end_of_stream = 0;
        vdec_stream.pts = venc_stream.pack[0].pts;
        hi_mpi_vdec_send_stream(VDEC_CHN_ID, &vdec_stream, DEFAULT_VDEC_WAIT_TIME);
        /*******************************************************
         step 2.2 : release stream
         *******************************************************/
        if (hi_mpi_venc_release_stream(VENC_CHN_ID, &venc_stream) != HI_SUCCESS) {
            sample_print("hi_mpi_venc_release_stream failed!\n");
            break;
        }
    }

    /*******************************************************
     step 3 : free pack nodes
    *******************************************************/
    free(venc_stream.pack);
    venc_stream.pack = NULL;

    if (buffer != NULL) {
        free(buffer);
        buffer = NULL;
    }

    sample_print("SAMPLE_COMPOSITE_GetStreamProc End!\n");
    return NULL;
}

static hi_void sample_composite_stop_thread_sendframe(hi_void)
{
    g_send_multi_frame_signal = HI_TRUE;
    if (g_send_multi_frame_thread != 0) {
        pthread_join(g_send_multi_frame_thread, HI_NULL);
        g_send_multi_frame_thread = 0;
    }
}

static hi_void sample_composite_stop_thread_stream(hi_void)
{
    g_stop_get_stream_signal = HI_TRUE;
    if (g_get_stream_thread != 0) {
        pthread_join(g_get_stream_thread, HI_NULL);
        g_get_stream_thread = 0;
    }
}

static hi_void sample_composite_stop_thread(hi_void)
{
    sample_composite_stop_thread_sendframe();
    sample_composite_stop_thread_stream();
}

static hi_void sample_composite_start_thread(hi_void)
{
    g_send_multi_frame_signal = HI_FALSE;
    g_stop_get_stream_signal = HI_FALSE;
    pthread_create(&g_send_multi_frame_thread, 0, sample_composite_send_multi_frame_proc, HI_NULL);
    pthread_create(&g_get_stream_thread, 0, sample_composite_get_stream_proc, HI_NULL);
}

static hi_void sample_composite_init_composite_param(sample_composite_param *param)
{
    param->enc_pic_size[0] = PIC_1080P; /* 0, idx */
    param->enc_pic_size[1] = PIC_1080P; /* 1, idx */

    param->payload[0] = HI_PT_H265;
    param->vdec_chn_num = 1;
}

static hi_s32 sample_composite_vi_vpss_cfg(sample_composite_param *param)
{
    hi_s32 i, ret;
    sample_sns_type sns_type = SENSOR0_TYPE;

    for (i = 0; i < VPSS_CHN_NUM; i++) {
        ret = sample_comm_sys_get_pic_size(param->enc_pic_size[i], &param->enc_size[i]);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_sys_get_pic_size failed!\n");
            return ret;
        }
    }

    // get vi param
    sample_comm_vi_get_default_vi_cfg(sns_type, &(param->vi_cfg));

    // get vpss param
    get_default_vpss_chn_attr(param->vi_cfg.dev_info.dev_attr.in_size,
        param->enc_size, VPSS_CHN_NUM, &param->vpss_param[0]);
    get_default_vpss_chn_attr(param->vi_cfg.dev_info.dev_attr.in_size,
        param->enc_size, param->vdec_chn_num, &param->vpss_param[1]);

    return HI_SUCCESS;
}

static hi_s32 sample_composite_init_vi_vpss(sample_composite_param *param)
{
    hi_s32 ret;

    ret = sample_composite_vi_vpss_cfg(param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    // get vdec param
    get_default_vdec_chn_attr(param->enc_size, param->vdec_chn_num, param->payload, 1, &param->vdec_param);
    param->vdec_param.composite_dec_en[0] = HI_TRUE;   /* open composite decode */

    // get vo param
    get_default_vo_cfg(&param->vo_cfg, param->enc_size[0]);

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    get_vb_attr(&(param->vi_cfg.dev_info.dev_attr.in_size), &param->vpss_param[0], &param->vb_attr);

    if ((ret = sample_composite_sys_init(&param->vb_attr)) != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void sample_composite_start_and_stop(hi_void)
{
    /******************************************
     create work thread
    ******************************************/
    sample_composite_start_thread();

    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }

    /******************************************
     exit process
    ******************************************/
    sample_composite_stop_thread();

    sample_composite_venc_deinit();
}


static hi_s32 sample_composite_vi_vpss_init(sample_composite_param *param)
{
    hi_s32 ret;

    if (sample_composite_init_vi_vpss(param) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if ((ret = sample_composite_vi_init(&param->vi_cfg)) != HI_SUCCESS) {
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_composite_vpss_init(VPSS_GRP, &param->vpss_param[0])) != HI_SUCCESS) {
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_composite_vpss_init(VPSS_GRP_DEC, &param->vpss_param[1])) != HI_SUCCESS) {
        goto EXIT_VPSS_STOP_0;
    }

    return HI_SUCCESS;

EXIT_VPSS_STOP_0:
    sample_composite_vpss_deinit(VPSS_GRP, &param->vpss_param[0]);
EXIT_VI_STOP:
    sample_composite_vi_deinit(&param->vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}
/******************************************************************************
* function :  venc_vdec_vo
******************************************************************************/
hi_s32 sample_composite_venc_vdec_vo(hi_void)
{
    hi_s32 ret;

    sample_composite_param param = {0};
    sample_composite_init_composite_param(&param);

    if ((ret = sample_composite_vi_vpss_init(&param)) != HI_SUCCESS) {
        return ret;
    }

    if ((ret = sample_composite_vdec_init(&param.vdec_param)) != HI_SUCCESS) {
        goto EXIT_VPSS_STOP_1;
    }

    if ((ret = sample_composite_vo_init(&param.vo_cfg)) != HI_SUCCESS) {
        goto EXIT_VDEC_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(VI_PIPE_ID, VI_DEV_ID, VPSS_GRP, 0)) != HI_SUCCESS) {
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VO_STOP;
    }

    if ((ret = sample_comm_vdec_bind_vpss(VDEC_CHN_ID, VPSS_GRP_DEC)) != HI_SUCCESS) {
        sample_print("VDEC Bind VPSS err for %#x!\n", ret);
        goto EXIT_VI_VPSS_UNBIND;
    }

    if ((ret = sample_comm_vpss_bind_vo(VPSS_GRP_DEC, VPSS_CHN_DEC_ID, VO_LAYER, VO_CHN_ID)) != HI_SUCCESS) {
        sample_print("VPSS Bind VO err for %#x!\n", ret);
        goto EXIT_VDEC_VPSS_UNBIND;
    }

    /******************************************
        start stream venc
    ******************************************/
    if ((ret = sample_composite_venc_init(param.enc_pic_size[0])) != HI_SUCCESS) {
        goto EXIT_VPSS_VO_UNBIND;
    }

    sample_composite_start_and_stop();

EXIT_VPSS_VO_UNBIND:
    sample_comm_vpss_un_bind_vo(VPSS_GRP_DEC, VPSS_CHN_0, VO_LAYER, VO_CHN_ID);
EXIT_VDEC_VPSS_UNBIND:
    sample_comm_vdec_un_bind_vpss(VDEC_CHN_ID, VPSS_GRP_DEC);
EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(VI_PIPE_ID, VI_DEV_ID, VPSS_GRP, VPSS_CHN_0);
EXIT_VO_STOP:
    sample_composite_vo_deinit(&param.vo_cfg);
EXIT_VDEC_STOP:
    sample_composite_vdec_deinit(&param.vdec_param);
EXIT_VPSS_STOP_1:
    sample_composite_vpss_deinit(VPSS_GRP_DEC, &param.vpss_param[1]);
    sample_composite_vpss_deinit(VPSS_GRP, &param.vpss_param[0]);
    sample_composite_vi_deinit(&param.vi_cfg);
    sample_comm_sys_exit();

    return ret;
}

static hi_void sample_composite_waiting_to_exit(hi_void)
{
    g_send_multi_frame_signal = HI_FALSE;
    pthread_create(&g_send_multi_frame_thread, 0, sample_composite_send_multi_frame_proc, HI_NULL);

    printf("please press twice ENTER to exit this sample\n");
    (hi_void)getchar();

    if (g_sample_venc_exit != HI_TRUE) {
        (hi_void)getchar();
    }

    /******************************************
     exit process
    ******************************************/
    sample_composite_stop_thread_sendframe();
    sample_comm_venc_stop_get_stream(1); // 1: chn num
}

static hi_void sample_composite_file_init(sample_composite_param *param)
{
    param->enc_pic_size[0] = PIC_1080P; /* 0: idx */
    param->enc_pic_size[1] = PIC_1080P; /* 1: idx */
}

static hi_s32 sample_composite_file_init_vi_vpss(sample_composite_param *param)
{
    hi_s32 ret;

    ret = sample_composite_vi_vpss_cfg(param);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    /******************************************
      step 1: init sys alloc common vb
    ******************************************/
    get_vb_attr(&(param->vi_cfg.dev_info.dev_attr.in_size), &param->vpss_param[0], &param->vb_attr);

    if ((ret = sample_composite_sys_init(&param->vb_attr)) != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}
/******************************************************************************
* function :  vi-vpss-venc-file
******************************************************************************/
hi_s32 sample_composite_venc_file(hi_void)
{
    hi_s32           ret;

    sample_composite_param param =  {0};
    sample_composite_file_init(&param);

    if (sample_composite_file_init_vi_vpss(&param) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if ((ret = sample_composite_vi_init(&param.vi_cfg)) != HI_SUCCESS) {
        goto EXIT_SYS_STOP;
    }

    if ((ret = sample_composite_vpss_init(VPSS_GRP, &param.vpss_param[0])) != HI_SUCCESS) {
        goto EXIT_VI_STOP;
    }

    if ((ret = sample_comm_vi_bind_vpss(VI_PIPE_ID, VI_DEV_ID, VPSS_GRP, 0)) != HI_SUCCESS) {
        sample_print("VI Bind VPSS err for %#x!\n", ret);
        goto EXIT_VPSS_STOP_0;
    }

   /******************************************
    start stream venc
    ******************************************/
    if ((ret = sample_composite_venc_init(param.enc_pic_size[0])) != HI_SUCCESS) {
        goto EXIT_VI_VPSS_UNBIND;
    }

    /******************************************
     create work thread
    ******************************************/
    if ((ret = sample_comm_venc_start_get_stream(param.venc_chn, 1)) != HI_SUCCESS) {
        sample_print("Start Venc failed!\n");
        goto EXIT_VENC_STOP;
    }

    sample_composite_waiting_to_exit();

EXIT_VENC_STOP:
    sample_composite_venc_deinit();
EXIT_VI_VPSS_UNBIND:
    sample_comm_vi_un_bind_vpss(VI_PIPE_ID, VI_DEV_ID, VPSS_GRP, VPSS_CHN_0);
EXIT_VPSS_STOP_0:
    sample_composite_vpss_deinit(VPSS_GRP, &param.vpss_param[0]);
EXIT_VI_STOP:
    sample_composite_vi_deinit(&param.vi_cfg);
EXIT_SYS_STOP:
    sample_comm_sys_exit();

    return ret;
}

/******************************************************************************
* function    : main()
* description : video venc sample
******************************************************************************/
#ifdef __LITEOS__
hi_s32 app_main(hi_s32 argc, hi_char *argv[])
#else
hi_s32 main(hi_s32 argc, hi_char *argv[])
#endif
{
    hi_s32 ret;
    hi_u32 index;
    char *end_ptr = HI_NULL;

    if (argc != 2) { /* 2:argc num */
        sample_composite_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_composite_usage(argv[0]);
        return HI_SUCCESS;
    }

    if (strlen(argv[1]) != 1) {
        sample_composite_usage(argv[0]);
        return HI_SUCCESS;
    }

    if (argv[1][0] < '0' || argv[1][0] > '9') {
        sample_composite_usage(argv[0]);
        return HI_SUCCESS;
    }

    index = strtoul(argv[1], &end_ptr, 10); /* 10： numberbase */
    if (end_ptr == argv[1] || *end_ptr !='\0') {
        sample_composite_usage(argv[0]);
        return HI_FAILURE;
    }

#ifndef __LITEOS__
    sample_sys_signal(sample_composite_handle_sig);
#endif

    switch (index) {
        case 0: /* 0:case num */
            ret = sample_composite_venc_vdec_vo();
            break;

        case 1: /* 1:case num */
            ret = sample_composite_venc_file();
            break;

        default:
            sample_composite_usage(argv[0]);
            return HI_FAILURE;
    }

    if (ret == HI_SUCCESS) {
        printf("program exit normally!\n");
    } else {
        printf("program exit abnormally!\n");
    }

#ifdef __LITEOS__
    return ret;
#else
    exit(ret);
#endif
}
