/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <limits.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_awb.h"

static volatile sig_atomic_t g_sig_flag = 0;

static sample_sns_type g_sns_type = SENSOR0_TYPE;
static sample_vi_cfg g_vi_video_cfg;
static sample_vi_cfg g_vi_capture_cfg;

typedef struct {
    hi_vi_pipe video_pipe;
    hi_vi_pipe capture_pipe;
    pthread_t  thread_id;
    hi_bool    start;
} sample_capture_thread_info;

static sample_capture_thread_info g_capture_thread_info;

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

static hi_void sample_get_char(hi_void)
{
    if (g_sig_flag == 1) {
        return;
    }

    sample_pause();
}

static hi_s32 sample_traffic_capture_sys_init(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode,
    sample_sns_type sns_type)
{
    hi_s32 ret;
    hi_vb_cfg vb_cfg;
    sample_vb_param g_vb_param = {
        .vb_size = {2688, 1520},
        .pixel_format = {HI_PIXEL_FORMAT_RGB_BAYER_12BPP, HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420},
        .compress_mode = {HI_COMPRESS_MODE_NONE, HI_COMPRESS_MODE_SEG},
        .video_format = {HI_VIDEO_FORMAT_LINEAR, HI_VIDEO_FORMAT_LINEAR},
        .blk_num = {5, 5}
    };
    hi_u32 supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;

    sample_comm_vi_get_size_by_sns_type(sns_type, &g_vb_param.vb_size);
    sample_comm_sys_get_default_vb_cfg(&g_vb_param, &vb_cfg);
    if (sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, aiisp_mode);
    if (ret != HI_SUCCESS) {
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_traffic_capture_start_vpss(hi_vpss_grp grp, hi_size *in_size)
{
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
    return sample_common_vpss_start(grp, &grp_attr, &vpss_chn_attr);
}

static hi_void sample_traffic_capture_stop_vpss(hi_vpss_grp grp)
{
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_FALSE, HI_FALSE, HI_FALSE};

    sample_common_vpss_stop(grp, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static hi_s32 sample_traffic_capture_start_venc(hi_venc_chn venc_chn[], hi_u32 max_venc_chn_num, hi_u32 chn_num)
{
    hi_s32 i;
    hi_s32 ret;
    hi_size venc_size;

    if (chn_num > max_venc_chn_num) {
        return HI_FAILURE;
    }

    sample_comm_vi_get_size_by_sns_type(g_sns_type, &venc_size);
    g_venc_chn_param.venc_size.width  = venc_size.width;
    g_venc_chn_param.venc_size.height = venc_size.height;
    g_venc_chn_param.size = sample_comm_sys_get_pic_enum(&venc_size);

    for (i = 0; i < (hi_s32)chn_num; i++) {
        if (i == (hi_s32)chn_num - 1) {
            g_venc_chn_param.type = HI_PT_JPEG;
        }
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

static hi_void sample_traffic_capture_stop_venc(const hi_venc_chn venc_chn[], hi_u32 max_venc_chn_num, hi_u32 chn_num)
{
    hi_u32 i;

    if (chn_num > max_venc_chn_num) {
        return;
    }

    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static hi_s32 sample_traffic_capture_start_vo(sample_vo_mode vo_mode)
{
    g_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_vo_cfg);
}

static hi_void sample_traffic_capture_stop_vo(hi_void)
{
    sample_comm_vo_stop_vo(&g_vo_cfg);
}

static hi_s32 sample_traffic_capture_start_venc_and_vo(const hi_vpss_grp vpss_grp[], hi_u32 grp_num)
{
    hi_u32 i;
    hi_s32 ret;
    const sample_vo_mode vo_mode = VO_MODE_1MUX;
    const hi_vpss_chn vpss_chn = 0;
    const hi_vo_layer vo_layer = 0;
    const hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    ret = sample_traffic_capture_start_vo(vo_mode);
    if (ret != HI_SUCCESS) {
        goto start_vo_failed;
    }

    ret = sample_traffic_capture_start_venc(venc_chn, 4, grp_num); /* 4: max venc chn num */
    if (ret != HI_SUCCESS) {
        goto start_venc_failed;
    }

    for (i = 0; i < grp_num; i++) {
        if (i == 0) {
            sample_comm_vpss_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
        }
        sample_comm_vpss_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
    }
    return HI_SUCCESS;

start_venc_failed:
    sample_traffic_capture_stop_vo();
start_vo_failed:
    return HI_FAILURE;
}

static hi_void sample_traffic_capture_stop_venc_and_vo(const hi_vpss_grp vpss_grp[], hi_u32 grp_num)
{
    hi_u32 i;
    const hi_vpss_chn vpss_chn = 0;
    const hi_vo_layer vo_layer = 0;
    const hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    const hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */

    for (i = 0; i < grp_num; i++) {
        if (i == 0) {
            sample_comm_vpss_un_bind_vo(vpss_grp[i], vpss_chn, vo_layer, vo_chn[i]);
        }
        sample_comm_vpss_un_bind_venc(vpss_grp[i], vpss_chn, venc_chn[i]);
    }

    sample_traffic_capture_stop_venc(venc_chn, 4, grp_num); /* 4: max venc chn num */
    sample_traffic_capture_stop_vo();
}

static hi_void sample_vi_get_capture_vi_cfg(hi_vi_pipe capture_pipe, sample_vi_cfg *vi_cfg)
{
    hi_vi_bind_pipe *bind_pipe = HI_NULL;

    (hi_void)memset_s(vi_cfg, sizeof(sample_vi_cfg), 0, sizeof(sample_vi_cfg));

    /* bind info */
    bind_pipe = &vi_cfg->bind_pipe;
    bind_pipe->pipe_num = 1;
    bind_pipe->pipe_id[0] = capture_pipe;

    /* pipe info */
    sample_comm_vi_get_default_pipe_info(g_sns_type, &vi_cfg->bind_pipe, vi_cfg->pipe_info);
    vi_cfg->pipe_info[0].pipe_attr.pipe_bypass_mode = HI_VI_PIPE_BYPASS_FE;
    vi_cfg->pipe_info[0].pipe_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    vi_cfg->sns_info.bus_id = -1;
}

static hi_s32 sample_traffic_capture_start_video_route(hi_vi_pipe video_pipe)
{
    hi_s32 ret;

    sample_comm_vi_get_default_vi_cfg(g_sns_type, &g_vi_video_cfg);
    g_vi_video_cfg.pipe_info[0].pipe_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    ret = sample_comm_vi_start_vi(&g_vi_video_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hi_mpi_vi_set_pipe_frame_source(video_pipe, HI_VI_PIPE_FRAME_SOURCE_USER);
    if (ret != HI_SUCCESS) {
        sample_print("set pipe frame source failed, ret: 0x%x!\n", ret);
        sample_comm_vi_stop_vi(&g_vi_video_cfg);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void sample_traffic_capture_stop_video_route(hi_vi_pipe video_pipe)
{
    sample_comm_vi_stop_vi(&g_vi_video_cfg);
}

static hi_s32 sample_traffic_capture_start_capture_route(hi_vi_pipe capture_pipe)
{
    hi_s32 ret;

    sample_vi_get_capture_vi_cfg(capture_pipe, &g_vi_capture_cfg);
    ret = sample_comm_vi_start_virt_pipe(&g_vi_capture_cfg);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hi_mpi_vi_set_pipe_frame_source(capture_pipe, HI_VI_PIPE_FRAME_SOURCE_USER);
    if (ret != HI_SUCCESS) {
        sample_print("set pipe frame source failed, ret: 0x%x!\n", ret);
        sample_comm_vi_stop_vi(&g_vi_video_cfg);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void sample_traffic_capture_stop_capture_route(hi_vi_pipe video_pipe)
{
    sample_comm_vi_stop_virt_pipe(&g_vi_capture_cfg);
}

static hi_s32 sample_traffic_capture_start_vpss_venc_vo(hi_vi_pipe video_pipe, hi_vi_pipe capture_pipe)
{
    hi_s32 ret;
    const hi_vi_chn vi_chn = 0;
    const hi_vpss_grp vpss_grp[2] = {0, 1};
    const hi_u32 grp_num = 2;
    const hi_vpss_chn vpss_chn = 0;
    hi_size in_size;

    sample_comm_vi_bind_vpss(video_pipe, vi_chn, vpss_grp[0], vpss_chn);
    sample_comm_vi_get_size_by_sns_type(g_sns_type, &in_size);
    ret = sample_traffic_capture_start_vpss(vpss_grp[0], &in_size);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = sample_traffic_capture_start_vpss(vpss_grp[1], &in_size);
    if (ret != HI_SUCCESS) {
        goto start_vpss_failed;
    }

    ret = sample_traffic_capture_start_venc_and_vo(vpss_grp, grp_num);
    if (ret != HI_SUCCESS) {
        goto start_venc_vo_failed;
    }

    return HI_SUCCESS;

start_venc_vo_failed:
    sample_traffic_capture_stop_vpss(vpss_grp[1]);
start_vpss_failed:
    sample_traffic_capture_stop_vpss(vpss_grp[0]);
    return ret;
}

static hi_void sample_traffic_capture_stop_vpss_venc_vo(hi_vi_pipe video_pipe, hi_vi_pipe capture_pipe)
{
    const hi_vpss_grp vpss_grp[2] = {0, 1};
    const hi_u32 grp_num = 2;

    sample_traffic_capture_stop_venc_and_vo(vpss_grp, grp_num);
    sample_traffic_capture_stop_vpss(vpss_grp[1]);
    sample_traffic_capture_stop_vpss(vpss_grp[0]);
}

static hi_bool sample_is_capture_frame(hi_video_frame_info *capture_frame_info)
{
    hi_bool capture_frame;
    int rd = 0;
    int fd;
    fd = open("/dev/random", O_RDONLY);
    if (fd > 0) {
        read(fd, &rd, sizeof(int));
    }
    close(fd);

    capture_frame = ((rd % 64) == 0) ? HI_TRUE : HI_FALSE; /* 64: 1/64 probability */

    return capture_frame;
}

static hi_s32 sample_capture_set_isp_param(hi_vi_pipe video_pipe, hi_vi_pipe capture_pipe)
{
    hi_s32 ret;
    hi_isp_exp_info exp_info;
    hi_isp_exposure_attr exp_attr;
    hi_isp_wb_info wb_info;
    hi_isp_wb_attr wb_attr;

    ret = hi_mpi_isp_query_exposure_info(video_pipe, &exp_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hi_mpi_isp_get_exposure_attr(video_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    exp_attr.op_type                       = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.exp_time_op_type  = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.a_gain_op_type    = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.d_gain_op_type    = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.ispd_gain_op_type = HI_OP_MODE_MANUAL;
    exp_attr.manual_attr.exp_time          = exp_info.exp_time;
    exp_attr.manual_attr.a_gain            = exp_info.a_gain;
    exp_attr.manual_attr.d_gain            = exp_info.d_gain;
    exp_attr.manual_attr.isp_d_gain        = exp_info.isp_d_gain;
    ret = hi_mpi_isp_set_exposure_attr(capture_pipe, &exp_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hi_mpi_isp_query_wb_info(video_pipe, &wb_info);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    ret = hi_mpi_isp_get_wb_attr(video_pipe, &wb_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    wb_attr.op_type             = HI_OP_MODE_MANUAL;
    wb_attr.manual_attr.r_gain  = wb_info.r_gain;
    wb_attr.manual_attr.gr_gain = wb_info.gr_gain;
    wb_attr.manual_attr.gb_gain = wb_info.gb_gain;
    wb_attr.manual_attr.b_gain  = wb_info.b_gain;
    ret = hi_mpi_isp_set_wb_attr(capture_pipe, &wb_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    return HI_SUCCESS;
}

static hi_void sample_capture_send_frame_to_capture_pipe(hi_vi_pipe video_pipe, hi_vi_pipe capture_pipe,
                                                         hi_video_frame_info *frame_info)
{
    hi_s32 i;
    hi_s32 ret;
    const hi_s32 milli_sec = -1;
    hi_vi_chn_attr chn_attr;
    hi_video_frame_info yuv_frame;
    const hi_video_frame_info *send_frame_info[1];

    for (i = 0; i < 2; i++) { /* 2: repeat send raw */
        ret = sample_capture_set_isp_param(video_pipe, capture_pipe);
        if (ret != HI_SUCCESS) {
            return;
        }

        ret = hi_mpi_isp_run_once(capture_pipe);
        if (ret != HI_SUCCESS) {
            sample_print("isp run once failed!\n");
            return;
        }

        send_frame_info[0] = frame_info;
        ret = hi_mpi_vi_send_pipe_raw(capture_pipe, send_frame_info, 1, milli_sec);
        if (ret != HI_SUCCESS) {
            sample_print("send pipe frame failed!\n");
            return;
        }

        ret = hi_mpi_vi_get_chn_attr(capture_pipe, 0, &chn_attr);
        if (ret != HI_SUCCESS) {
            return;
        }
        chn_attr.depth = 1;
        ret = hi_mpi_vi_set_chn_attr(capture_pipe, 0, &chn_attr);
        if (ret != HI_SUCCESS) {
            return;
        }

        /* discare first frame */
        if (i == 0) {
            if (hi_mpi_vi_get_chn_frame(capture_pipe, 0, &yuv_frame, milli_sec) == HI_SUCCESS) {
                hi_mpi_vi_release_chn_frame(capture_pipe, 0, &yuv_frame);
            }
        } else {
            if (hi_mpi_vi_get_chn_frame(capture_pipe, 0, &yuv_frame, milli_sec) == HI_SUCCESS) {
                const hi_vpss_grp vpss_grp = 1;
                hi_mpi_vpss_send_frame(vpss_grp, &yuv_frame, milli_sec);
                hi_mpi_vi_release_chn_frame(capture_pipe, 0, &yuv_frame);
            }
        }
    }
}

static hi_void sample_capture_send_frame_to_video_pipe(hi_vi_pipe video_pipe, hi_video_frame_info *frame_info)
{
    hi_s32 ret;
    const hi_s32 milli_sec = -1;
    const hi_video_frame_info *send_frame_info[1];

    send_frame_info[0] = frame_info;
    ret = hi_mpi_vi_send_pipe_raw(video_pipe, send_frame_info, 1, milli_sec);
    if (ret != HI_SUCCESS) {
        sample_print("send pipe frame failed!\n");
    }
}

static hi_void *sample_capture_thread(hi_void *param)
{
    hi_s32 ret;
    const hi_s32 milli_sec = -1;
    hi_video_frame_info get_frame_info;
    hi_vi_frame_dump_attr dump_attr;
    sample_capture_thread_info *thread_info = (sample_capture_thread_info *)param;

    dump_attr.enable = HI_TRUE;
    dump_attr.depth = 2; /* 2: dump depth set 2 */
    ret = hi_mpi_vi_set_pipe_frame_dump_attr(thread_info->video_pipe, &dump_attr);
    if (ret != HI_SUCCESS) {
        sample_print("set pipe frame dump attr failed! ret:0x%x\n", ret);
        return HI_NULL;
    }

    while (thread_info->start == HI_TRUE) {
        ret = hi_mpi_vi_get_pipe_frame(thread_info->video_pipe, &get_frame_info, milli_sec);
        if (ret != HI_SUCCESS) {
            break;
        }

        if (sample_is_capture_frame(&get_frame_info)) {
            sample_capture_send_frame_to_capture_pipe(thread_info->video_pipe,
                                                      thread_info->capture_pipe, &get_frame_info);
        } else {
            sample_capture_send_frame_to_video_pipe(thread_info->video_pipe, &get_frame_info);
        }

        ret = hi_mpi_vi_release_pipe_frame(thread_info->video_pipe, &get_frame_info);
        if (ret != HI_SUCCESS) {
            sample_print("release pipe frame failed!\n");
            return HI_NULL;
        }
    }

    return HI_NULL;
}

static hi_s32 sample_traffic_capture_create_capture_thread(hi_vi_pipe video_pipe, hi_vi_pipe capture_pipe)
{
    hi_s32 ret;

    g_capture_thread_info.video_pipe   = video_pipe;
    g_capture_thread_info.capture_pipe = capture_pipe;
    ret = pthread_create(&g_capture_thread_info.thread_id, HI_NULL, sample_capture_thread, &g_capture_thread_info);
    if (ret != 0) {
        sample_print("create capture thread failed!\n");
        return HI_FAILURE;
    }
    g_capture_thread_info.start = HI_TRUE;

    return HI_SUCCESS;
}

static hi_void sample_traffic_capture_destroy_capture_thread(hi_void)
{
    if (g_capture_thread_info.start == HI_TRUE) {
        g_capture_thread_info.start = HI_FALSE;
        pthread_join(g_capture_thread_info.thread_id, NULL);
    }
}

static hi_s32 sample_traffic_capture_offline(hi_void)
{
    hi_s32 ret;
    const hi_vi_vpss_mode_type mode_type = HI_VI_OFFLINE_VPSS_OFFLINE;
    const hi_vi_aiisp_mode aiisp_mode = HI_VI_AIISP_MODE_DEFAULT;
    const hi_vi_pipe video_pipe = 0;
    const hi_vi_pipe capture_pipe = 7;

    ret = sample_traffic_capture_sys_init(mode_type, aiisp_mode, g_sns_type);
    if (ret != HI_SUCCESS) {
        goto sys_init_failed;
    }

    ret = sample_traffic_capture_start_video_route(video_pipe);
    if (ret != HI_SUCCESS) {
        goto start_video_route_failed;
    }

    ret = sample_traffic_capture_start_capture_route(capture_pipe);
    if (ret != HI_SUCCESS) {
        goto start_capture_route_failed;
    }

    ret = sample_traffic_capture_start_vpss_venc_vo(video_pipe, capture_pipe);
    if (ret != HI_SUCCESS) {
        goto start_vpss_venc_vo_failed;
    }

    ret = sample_traffic_capture_create_capture_thread(video_pipe, capture_pipe);
    if (ret != HI_SUCCESS) {
        goto create_capture_thread_failed;
    }

    sample_get_char();

    sample_traffic_capture_destroy_capture_thread();

create_capture_thread_failed:
    sample_traffic_capture_stop_vpss_venc_vo(video_pipe, capture_pipe);
start_vpss_venc_vo_failed:
    sample_traffic_capture_stop_capture_route(capture_pipe);
start_capture_route_failed:
    sample_traffic_capture_stop_video_route(video_pipe);
start_video_route_failed:
    sample_comm_sys_exit();
sys_init_failed:
    return ret;
}

hi_void sample_traffic_capture_usage(const char *prg_name)
{
    printf("usage : %s <index> \n", prg_name);
    printf("index:\n");
    printf("\t(0) traffic picture capture.\n");
}

static hi_void sample_traffic_capture_handle_sig(hi_s32 signo)
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

#ifdef __LITEOS__
hi_s32 app_main(hi_s32 argc, hi_char *argv[])
#else
hi_s32 main(hi_s32 argc, hi_char *argv[])
#endif
{
    hi_s32 ret;
    hi_u32 index;
    hi_char *end_ptr = HI_NULL;

    if (argc != 2) { /* 2:arg num */
        sample_traffic_capture_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_traffic_capture_usage(argv[0]);
        return HI_FAILURE;
    }

    if (strlen(argv[1]) != 1 || !check_digit(argv[1][0])) {
        sample_traffic_capture_usage(argv[0]);
        return HI_FAILURE;
    }

#ifndef __LITEOS__
    sample_register_sig_handler(sample_traffic_capture_handle_sig);
#endif

    index = (hi_u32)strtol(argv[1], &end_ptr, 10); /* base 10, argv[1] has been check between [0, 9] */
    if ((end_ptr == argv[1]) || (*end_ptr) != '\0') {
        sample_traffic_capture_usage(argv[0]);
        return HI_FAILURE;
    }
    switch (index) {
        case 0: /* 0 traffic capture */
            ret = sample_traffic_capture_offline();
            break;
        default:
            sample_traffic_capture_usage(argv[0]);
            ret = HI_FAILURE;
            break;
    }

    if ((ret == HI_SUCCESS) && (g_sig_flag == 0)) {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

#ifdef __LITEOS__
    return ret;
#else
    exit(ret);
#endif
}
