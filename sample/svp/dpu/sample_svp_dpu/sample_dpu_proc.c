/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include "sample_dpu_proc.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <limits.h>
#include "hi_buffer.h"
#include "hi_common.h"
#include "hi_common_sys.h"
#include "hi_common_dpu_rect.h"

#include "sample_comm.h"
#include "sample_common_dpu.h"

static hi_sample_svp_dpu_cfg g_dpu_cfg;
static hi_bool g_stop_signal = HI_FALSE;
static hi_void sample_svp_dpu_set_pic_buf_attr(hi_u32 width, hi_u32 height,
    hi_pixel_format pixel_format, hi_pic_buf_attr *pic_buf_attr)
{
    pic_buf_attr->width = width;
    pic_buf_attr->height = height;
    pic_buf_attr->bit_width = ((pixel_format == HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420) ?
        HI_DATA_BIT_WIDTH_8 : HI_DATA_BIT_WIDTH_16);
    pic_buf_attr->pixel_format = pixel_format;
    pic_buf_attr->compress_mode = HI_COMPRESS_MODE_NONE;
    pic_buf_attr->align = HI_DEFAULT_ALIGN;
    pic_buf_attr->video_format = HI_VIDEO_FORMAT_LINEAR;
    return;
}

static hi_void sample_svp_dpu_uninit_dpu_rect(hi_void)
{
    hi_u32 i;

    (hi_void)sample_common_svp_dpu_rect_stop(g_dpu_cfg.dpu_rect_grp,
        g_dpu_cfg.dpu_rect_grp_attr.rect_mode);
        for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
            (hi_void)sample_common_svp_dpu_rect_unload_lut(&g_dpu_cfg.dpu_rect_mem_info[i], g_dpu_cfg.lut_id[i]);
    }
    return;
}

static hi_s32 sample_svp_dpu_start_dpu_rect(const hi_size *src_size, const hi_size *dst_size,
    hi_char *lut_file_name[], hi_u32 file_num)
{
    hi_s32 ret;
    hi_u32 i;
    hi_u32 rect_pipe_num = 0;
    hi_dpu_rect_chn_attr rect_chn_attr[HI_DPU_RECT_MAX_CHN_NUM] = {0};
    hi_char path[PATH_MAX + 1] = {0};

    for (i = 0; i < file_num; i++) {
        sample_svp_dpu_check_exps_return(((strlen(lut_file_name[i]) > PATH_MAX) ||
            (realpath(lut_file_name[i], path) == HI_NULL)),
            HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Error, file_name is invalid!\n");
        ret = sample_common_svp_dpu_rect_load_lut(lut_file_name[i],
            &g_dpu_cfg.dpu_rect_mem_info[i], &g_dpu_cfg.lut_id[i]);
        sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_0, "load lut failed!\n");
        rect_pipe_num++;
    }

    g_dpu_cfg.dpu_rect_grp_attr.left_attr.lut_id = g_dpu_cfg.lut_id[0];
    g_dpu_cfg.dpu_rect_grp_attr.right_attr.lut_id = g_dpu_cfg.lut_id[1];
    g_dpu_cfg.dpu_rect_grp = 0;
    g_dpu_cfg.dpu_rect_grp_attr.left_attr.image_size.width = src_size->width;
    g_dpu_cfg.dpu_rect_grp_attr.left_attr.image_size.height = src_size->height;
    g_dpu_cfg.dpu_rect_grp_attr.right_attr.image_size.width = src_size->width;
    g_dpu_cfg.dpu_rect_grp_attr.right_attr.image_size.height = src_size->height;
    g_dpu_cfg.dpu_rect_grp_attr.rect_mode = HI_DPU_RECT_MODE_DOUBLE;
    g_dpu_cfg.dpu_rect_grp_attr.depth = 0;
    g_dpu_cfg.dpu_rect_grp_attr.is_need_src_frame = HI_TRUE;
    g_dpu_cfg.dpu_rect_grp_attr.frame_rate.src_frame_rate = SAMPLE_SVP_DPU_NO_RATE_CTRL;
    g_dpu_cfg.dpu_rect_grp_attr.frame_rate.dst_frame_rate = SAMPLE_SVP_DPU_NO_RATE_CTRL;

    for (i = 0; i < HI_DPU_RECT_MAX_CHN_NUM; i++) {
        rect_chn_attr[i].image_size.width = dst_size->width;
        rect_chn_attr[i].image_size.height = dst_size->height;
    }

    ret = sample_common_svp_dpu_rect_start(g_dpu_cfg.dpu_rect_grp,
        &g_dpu_cfg.dpu_rect_grp_attr, rect_chn_attr, HI_DPU_RECT_MAX_CHN_NUM);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_1, "start dpu rect failed!\n");

    return HI_SUCCESS;
fail_1:
    (hi_void)sample_common_svp_dpu_rect_stop(g_dpu_cfg.dpu_rect_grp, g_dpu_cfg.dpu_rect_grp_attr.rect_mode);
fail_0:
    for (i = 0; i < rect_pipe_num; i++) {
        (hi_void)sample_common_svp_dpu_rect_unload_lut(&g_dpu_cfg.dpu_rect_mem_info[i], g_dpu_cfg.lut_id[i]);
    }
    return ret;
}

static hi_void sample_svp_dpu_uninit_dpu_match(hi_void)
{
    (hi_void)sample_common_svp_dpu_match_stop(g_dpu_cfg.dpu_match_grp);
    sample_svp_dpu_mmz_free(g_dpu_cfg.dpu_match_grp_attr.assist_buf.phys_addr,
        g_dpu_cfg.dpu_match_grp_attr.assist_buf.virt_addr);
    return;
}

static hi_s32 sample_svp_dpu_set_match_grp_common_attr(hi_u32 width, hi_u32 height)
{
    hi_s32 ret;
    hi_u32 size;

    ret = hi_mpi_dpu_match_get_assist_buf_size(SAMPLE_SVP_DPU_DISPARITY_NUM, height, &size);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "get dpu match assist buffer size failed!\n");
    ret = sample_common_svp_dpu_match_create_mem_info(&g_dpu_cfg.dpu_match_grp_attr.assist_buf,
        "sampe_dpu_match_assist_buf", HI_NULL, size);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "create dpu match assist buffer failed!\n");
    g_dpu_cfg.dpu_match_grp = 0;

    g_dpu_cfg.dpu_match_grp_attr.left_image_size.width = width;
    g_dpu_cfg.dpu_match_grp_attr.left_image_size.height = height;
    g_dpu_cfg.dpu_match_grp_attr.right_image_size.width = width;
    g_dpu_cfg.dpu_match_grp_attr.right_image_size.height = height;
    g_dpu_cfg.dpu_match_grp_attr.invalid_disp = SAMPLE_SVP_DPU_INVALID_DISP;
    g_dpu_cfg.dpu_match_grp_attr.depth = 1;
    g_dpu_cfg.dpu_match_grp_attr.is_need_src_frame = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.frame_rate.src_frame_rate = SAMPLE_SVP_DPU_NO_RATE_CTRL;
    g_dpu_cfg.dpu_match_grp_attr.frame_rate.dst_frame_rate = SAMPLE_SVP_DPU_NO_RATE_CTRL;
    return HI_SUCCESS;
}

static hi_s32 sample_svp_dpu_set_match_grp_disp_attr(hi_void)
{
    hi_s32 ret;

    g_dpu_cfg.dpu_match_grp_attr.disp_param.version = HI_DPU_MATCH_DISP_VERSION_V1;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.is_proc_disp = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.is_output_disp = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.is_output_conf = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.is_post_proc = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.is_filter_disp_by_conf = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.is_time_domain_filter = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.is_enable_disp_subpixel = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.mask_mode = HI_DPU_MATCH_MASK_9X9_MODE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.speed_accuracy_mode = HI_DPU_MATCH_SPEED_ACCURACY_MODE_ACCURACY;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.bg_mode = HI_DPU_MATCH_BG_MODE_NONE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.opti_mode = HI_DPU_MATCH_OPTI_MODE_ADAPTIVE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.disp_num = SAMPLE_SVP_DPU_DISPARITY_NUM;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.disp_start_pos = 0;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.first_penalty_coef = SAMPLE_SVP_DPU_MATCH_FIRST_PENALTY_COEF;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.second_penalty_coef = SAMPLE_SVP_DPU_MATCH_SECOND_PENALTY_COEF;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.fg_init_cost_zero_thr = SAMPLE_SVP_DPU_MATCH_FG_INIT_COST_ZERO_THR;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.local_feature = SAMPLE_SVP_DPU_MATCH_LOCAL_FAETURE;
    g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.conf_filter_disp_thr = SAMPLE_SVP_DPU_MATCH_CONF_FILTER_DISP_THR;
    ret = sample_common_svp_dpu_get_fg_aggregate_coef_and_unique_ratio(
        SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D11_A0,
        &g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.aggregate_coef,
        &g_dpu_cfg.dpu_match_grp_attr.disp_param.v1.unique_ratio);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "get fg aggregate coef failed!\n");
    return HI_SUCCESS;
}

static hi_void sample_svp_dpu_set_match_grp_depth_attr(hi_void)
{
    g_dpu_cfg.dpu_match_grp_attr.depth_param.version = HI_DPU_MATCH_DEPTH_VERSION_V1;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.is_output_depth = HI_TRUE;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.disp_from = HI_DPU_MATCH_DISP_FROM_MATCH;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.fb_norm_bits = SAMPLE_SVP_DPU_MATCH_FB_NORM_BITS_8;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.fb = SAMPLE_SVP_DPU_MATCH_FB;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.z0 = SAMPLE_SVP_DPU_MATCH_Z0;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.min_depth_thr = SAMPLE_SVP_DPU_MATCH_MIN_DEPTH_THR;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.max_depth_thr = SAMPLE_SVP_DPU_MATCH_MAX_DEPTH_THR;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.invalid_depth = SAMPLE_SVP_DPU_MATCH_INVALID_DEPTH;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.disp_coef = SAMPLE_SVP_DPU_MATCH_DISP_COEF;
    g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.depth_prec = g_dpu_cfg.dpu_match_grp_attr.depth_param.v1.fb_norm_bits -
        SAMPLE_SVP_DPU_MATCH_FB_NORM_BITS_8;
}
static hi_s32 sample_svp_dpu_start_dpu_match(hi_u32 width, hi_u32 height)
{
    hi_s32 ret;
    hi_dpu_match_chn_attr match_chn_attr = {0};
    ret = sample_svp_dpu_set_match_grp_common_attr(width, height);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "set match grp common attr failed!\n");
    ret = sample_svp_dpu_set_match_grp_disp_attr();
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "set match grp disp attr failed!\n");
    sample_svp_dpu_set_match_grp_depth_attr();

    match_chn_attr.image_size.width = width;
    match_chn_attr.image_size.height = height;
    ret = sample_common_svp_dpu_match_start(g_dpu_cfg.dpu_match_grp, &g_dpu_cfg.dpu_match_grp_attr, &match_chn_attr);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "dpu match start failed!\n");
    return HI_SUCCESS;
fail:
    (hi_void)sample_common_svp_dpu_match_stop(g_dpu_cfg.dpu_match_grp);
    sample_svp_dpu_mmz_free(g_dpu_cfg.dpu_match_grp_attr.assist_buf.phys_addr,
        g_dpu_cfg.dpu_match_grp_attr.assist_buf.virt_addr);
    return ret;
}

static hi_s32 sample_svp_dpu_create_pool(const hi_size *pic_size)
{
    hi_s32 ret;
    hi_u32 i;
    hi_u32 stride;
    hi_s32 size;
    hi_vb_pool_cfg vb_pool_cfg;

    stride = sample_common_svp_dpu_calc_stride(pic_size->width, SAMPLE_SVP_DPU_ALIGN_16);
    size = stride * pic_size->height;
    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        ret = memset_s(&vb_pool_cfg, sizeof(hi_vb_pool_cfg), 0, sizeof(hi_vb_pool_cfg));
        sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_0, "memset_s for vb_pool_cfg failed!\n");

        vb_pool_cfg.blk_size  = size;
        vb_pool_cfg.blk_cnt   = 1;
        vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
        g_dpu_cfg.pipe_vb_pool[i] = hi_mpi_vb_create_pool(&vb_pool_cfg);
        sample_svp_dpu_check_exps_goto(g_dpu_cfg.pipe_vb_pool[i] == HI_VB_INVALID_POOL_ID,
            fail_0, "create vb pool failed!\n");

        g_dpu_cfg.vb_blk[i] = hi_mpi_vb_get_blk(g_dpu_cfg.pipe_vb_pool[i], size, HI_NULL);
        sample_svp_dpu_check_exps_goto(g_dpu_cfg.vb_blk[i] == HI_VB_INVALID_HANDLE,
            fail_1, "hi_mpi_vb_get_blk failed!\n");
    }
    return HI_SUCCESS;
fail_1:
    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        ret = hi_mpi_vb_release_blk(g_dpu_cfg.vb_blk[i]);
        g_dpu_cfg.vb_blk[i] = HI_VB_INVALID_HANDLE;
    }
fail_0:
    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        if (g_dpu_cfg.pipe_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
            (hi_void)hi_mpi_vb_destroy_pool(g_dpu_cfg.pipe_vb_pool[i]);
        }
    }
    return ret;
}

static hi_s32 sample_svp_dpu_sys_init_file_case(hi_size *pic_size, hi_u32 pic_size_num)
{
    hi_s32 ret;
    hi_pic_size pic_size_type = PIC_640X480;
    hi_vb_cfg vb_cfg;
    hi_pic_buf_attr pic_buf_attr;
    g_stop_signal = HI_FALSE;

    ret = memset_s(&g_dpu_cfg, sizeof(hi_sample_svp_dpu_cfg), 0, sizeof(hi_sample_svp_dpu_cfg));
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "memset_s for dpu_cfg failed!\n");

    sample_svp_dpu_check_exps_return(pic_size_num < SAMPLE_SVP_DPU_CHN_NUM, HI_FAILURE, "pic_size_num is illegal!\n");
    ret = sample_comm_sys_get_pic_size(pic_size_type, &pic_size[0]);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "sys get pic size failed!\n");

    ret = memset_s(&vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "memset_s for vb_cfg failed!\n");

    sample_svp_dpu_set_pic_buf_attr(pic_size[0].width, pic_size[0].height,
        HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420, &pic_buf_attr);

    vb_cfg.max_pool_cnt            = SAMPLE_SVP_DPU_MAX_POOL_CNT;
    vb_cfg.common_pool[0].blk_cnt  = SAMPLE_SVP_DPU_BLK_CNT_10;
    vb_cfg.common_pool[0].blk_size = hi_common_get_pic_buf_size(&pic_buf_attr);

    pic_size[1].width = SAMPLE_SVP_DPU_DST_WIDTH;
    pic_size[1].height = SAMPLE_SVP_DPU_DST_HEIGHT;
    sample_svp_dpu_set_pic_buf_attr(pic_size[1].width, pic_size[1].height,
        HI_PIXEL_FORMAT_S16C1, &pic_buf_attr);
    vb_cfg.common_pool[1].blk_cnt  = SAMPLE_SVP_DPU_BLK_CNT_8;
    vb_cfg.common_pool[1].blk_size = hi_common_get_pic_buf_size(&pic_buf_attr);

    ret = sample_comm_sys_vb_init(&vb_cfg);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "init sys failed failed!\n");

    ret = sample_svp_dpu_create_pool(&pic_size[0]);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "sample_svp_dpu_create_pool failed!\n");
    return ret;
fail:
    sample_comm_sys_exit();
    return ret;
}

static hi_void sample_svp_dpu_sys_uninit_file_case(hi_void)
{
    hi_u32 i;

    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        (hi_void)hi_mpi_vb_release_blk(g_dpu_cfg.vb_blk[i]);
        g_dpu_cfg.vb_blk[i] = HI_VB_INVALID_HANDLE;
    }
    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        if (g_dpu_cfg.pipe_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
            (hi_void)hi_mpi_vb_destroy_pool(g_dpu_cfg.pipe_vb_pool[i]);
        }
    }
    sample_comm_sys_exit();
    return;
}

static hi_s32 sample_svp_dpu_open_file(hi_char *src_file_name[HI_DPU_RECT_MAX_PIPE_NUM],
    hi_u32 src_file_num, const hi_char *dst_file_name[SAMPLE_SVP_DPU_OUTPUT_NUM], hi_u32 dst_file_num)
{
    hi_u32 i;
    hi_char path[PATH_MAX + 1] = {0};
    sample_svp_dpu_check_exps_return((src_file_num > HI_DPU_RECT_MAX_PIPE_NUM) || (src_file_num == 0),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Error, src_file_num(%u) must be (%u, %u]!\n", src_file_num, 0,
        HI_DPU_RECT_MAX_PIPE_NUM);
    sample_svp_dpu_check_exps_return((dst_file_num > SAMPLE_SVP_DPU_OUTPUT_NUM) || (dst_file_num == 0),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Error, dst_file_num(%u) must be (%u, %u]!\n", dst_file_num, 0,
        SAMPLE_SVP_DPU_OUTPUT_NUM);

    for (i = 0; i < src_file_num; i++) {
        sample_svp_dpu_check_exps_return((strlen(src_file_name[i]) > PATH_MAX) ||
            (realpath(src_file_name[i], path) == HI_NULL),
            HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Error, file_name is invalid!\n");
        g_dpu_cfg.pipe_file[i] = fopen(path, "rb");
        sample_svp_dpu_check_exps_goto(g_dpu_cfg.pipe_file[i] == HI_NULL,
            fail, "fopen file %s failed!\n", src_file_name[i]);
    }
    for (i = 0; i < dst_file_num; i++) {
        g_dpu_cfg.dst_file[i] = fopen(dst_file_name[i], "wb");
        sample_svp_dpu_check_exps_goto(g_dpu_cfg.dst_file[i] == HI_NULL,
            fail, "fopen file %s failed!\n", dst_file_name[i]);
    }

    return HI_SUCCESS;
fail:
    for (i = 0; i < dst_file_num; i++) {
        if (g_dpu_cfg.dst_file[i] != HI_NULL) {
            fclose(g_dpu_cfg.dst_file[i]);
            g_dpu_cfg.dst_file[i] = HI_NULL;
        }
    }

    for (i = 0; i < src_file_num; i++) {
        if (g_dpu_cfg.pipe_file[i] != HI_NULL) {
            fclose(g_dpu_cfg.pipe_file[i]);
            g_dpu_cfg.pipe_file[i] = HI_NULL;
        }
    }

    return HI_ERR_DPU_RECT_ILLEGAL_PARAM;
}

static void sample_svp_dpu_close_file(hi_void)
{
    hi_u32 i;
    for (i = 0; i < SAMPLE_SVP_DPU_OUTPUT_NUM; i++) {
        if (g_dpu_cfg.dst_file[i] != HI_NULL) {
            fclose(g_dpu_cfg.dst_file[i]);
            g_dpu_cfg.dst_file[i] = HI_NULL;
        }
    }

    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        if (g_dpu_cfg.pipe_file[i] != HI_NULL) {
            fclose(g_dpu_cfg.pipe_file[i]);
            g_dpu_cfg.pipe_file[i] = HI_NULL;
        }
    }
    return;
}

static hi_void sample_svp_dpu_file_rect_match_stop(hi_void)
{
    hi_u32 i;
    sample_svp_dpu_close_file();

    (hi_void)sample_common_svp_dpu_rect_unbind_match(g_dpu_cfg.dpu_rect_grp, g_dpu_cfg.dpu_match_grp);
    (hi_void)sample_common_svp_dpu_match_stop(g_dpu_cfg.dpu_match_grp);
    sample_svp_dpu_mmz_free(g_dpu_cfg.dpu_match_grp_attr.assist_buf.phys_addr,
        g_dpu_cfg.dpu_match_grp_attr.assist_buf.virt_addr);
    (hi_void)sample_common_svp_dpu_rect_stop(g_dpu_cfg.dpu_rect_grp, g_dpu_cfg.dpu_rect_grp_attr.rect_mode);

    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        (hi_void)sample_common_svp_dpu_rect_unload_lut(&g_dpu_cfg.dpu_rect_mem_info[i], g_dpu_cfg.lut_id[i]);
    }

    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        (hi_void)hi_mpi_vb_release_blk(g_dpu_cfg.vb_blk[i]);
        g_dpu_cfg.vb_blk[i] = HI_VB_INVALID_HANDLE;
    }

    for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
        if (g_dpu_cfg.pipe_vb_pool[i] != HI_VB_INVALID_POOL_ID) {
            (hi_void)hi_mpi_vb_destroy_pool(g_dpu_cfg.pipe_vb_pool[i]);
        }
    }
    g_stop_signal = HI_FALSE;
    sample_comm_sys_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
    return;
}

static hi_s32 sample_svp_dpu_proc_frame_from_file(const hi_size *pic_size)
{
    hi_s32 ret;
    hi_u32 stride;
    hi_u32 i;
    hi_u32 frame_num;
    hi_s32 milli_sec = SAMPLE_SVP_DPU_TIME_OUT;
    hi_video_frame_info dpu_rect_src_frame[HI_DPU_RECT_MAX_PIPE_NUM] = {0};

    stride = sample_common_svp_dpu_calc_stride(pic_size->width, SAMPLE_SVP_DPU_ALIGN_16);
    for (frame_num = 0; frame_num < SAMPLE_SVP_DPU_FRAME_NUM && g_stop_signal == HI_FALSE; frame_num++) {
        for (i = 0; i < HI_DPU_RECT_MAX_PIPE_NUM; i++) {
            ret = sample_common_svp_dpu_rect_get_frame_from_file(g_dpu_cfg.pipe_file[i], pic_size,
                stride, &dpu_rect_src_frame[i], g_dpu_cfg.vb_blk[i]);
            sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "get frame failed!\n");
        }

        ret = hi_mpi_dpu_rect_send_frame(g_dpu_cfg.dpu_rect_grp, &dpu_rect_src_frame[HI_DPU_RECT_LEFT_PIPE],
            &dpu_rect_src_frame[HI_DPU_RECT_RIGHT_PIPE], milli_sec);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "hi_mpi_dpu_rect_send_frame failed!\n");

       /* get match frame from DPU MATCH */
        milli_sec = SAMPLE_SVP_DPU_BLOCK;
        ret = hi_mpi_dpu_match_get_frame(g_dpu_cfg.dpu_match_grp, milli_sec,
            &g_dpu_cfg.dpu_match_src_frame, &g_dpu_cfg.dpu_match_dst_frame);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "get dpu match frame failed!\n");

        /* use dst frame to do something */
        ret = sample_common_svp_dpu_write_frame_to_file(g_dpu_cfg.dst_file[SAMPLE_SVP_DPU_OUTPUT_DISP_IDX],
            sizeof(hi_u16), &g_dpu_cfg.dpu_match_dst_frame.disp_frame);
        sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "sample_common_svp_dpu_write_frame_to_file failed!\n");
        ret = sample_common_svp_dpu_write_frame_to_file(g_dpu_cfg.dst_file[SAMPLE_SVP_DPU_OUTPUT_CONF_IDX],
            sizeof(hi_u8), &g_dpu_cfg.dpu_match_dst_frame.conf_frame);
        sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "sample_common_svp_dpu_write_frame_to_file failed!\n");
        ret = sample_common_svp_dpu_write_frame_to_file(g_dpu_cfg.dst_file[SAMPLE_SVP_DPU_OUTPUT_DEPTH_IDX],
            sizeof(hi_u16), &g_dpu_cfg.dpu_match_dst_frame.depth_frame);
        sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "sample_common_svp_dpu_write_frame_to_file failed!\n");

        ret = hi_mpi_dpu_match_release_frame(g_dpu_cfg.dpu_match_grp,
            &g_dpu_cfg.dpu_match_src_frame, &g_dpu_cfg.dpu_match_dst_frame);
        sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "release dpu match frame failed!\n");
    }
    sample_svp_dpu_trace_info("Process success!\n");
    return HI_SUCCESS;
fail:
    (hi_void)hi_mpi_dpu_match_release_frame(g_dpu_cfg.dpu_match_grp,
        &g_dpu_cfg.dpu_match_src_frame, &g_dpu_cfg.dpu_match_dst_frame);
    return ret;
}

/* This case only for function design reference */
hi_s32 sample_svp_dpu_file_rect_match(hi_void)
{
    hi_s32 ret;
    hi_size pic_size[SAMPLE_SVP_DPU_CHN_NUM];
    hi_char *lut_file_name[HI_DPU_RECT_MAX_PIPE_NUM] = {
        "./data/input/lut/map_int_left_test.bin", "./data/input/lut/map_int_right_test.bin"
    };
    hi_char *src_file_name[HI_DPU_RECT_MAX_PIPE_NUM] = {
        "./data/input/src/000_640_480_l.yuv", "./data/input/src/000_640_480_r.yuv"
    };
    const hi_char *dst_file_name[SAMPLE_SVP_DPU_OUTPUT_NUM] = { "./data/output/640_480_s16c1_disp.yuv",
        "./data/output/640_480_sp400_conf.yuv",
        "./data/output/640_480_u16c1_depth.yuv" };

    /*  init SYS, init common VB(for DPU RECT and DPU MATCH) */
    ret = sample_svp_dpu_sys_init_file_case(pic_size, SAMPLE_SVP_DPU_CHN_NUM);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret, "sys_init in file case failed!\n");

    if (g_stop_signal == HI_TRUE) {
        sample_svp_dpu_file_rect_match_stop();
        return HI_FAILURE;
    }
    /*  start DPU RECT */
    ret = sample_svp_dpu_start_dpu_rect(&pic_size[0], &pic_size[1], lut_file_name, HI_DPU_RECT_MAX_PIPE_NUM);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_0, "start dpu rect failed!\n");

    if (g_stop_signal == HI_TRUE) {
        sample_svp_dpu_file_rect_match_stop();
        return HI_FAILURE;
    }

    ret = sample_svp_dpu_start_dpu_match(pic_size[1].width, pic_size[1].height);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_1, "dpu match start failed!\n");

    if (g_stop_signal == HI_TRUE) {
        sample_svp_dpu_file_rect_match_stop();
        return HI_FAILURE;
    }
    /* bind DPU RECT to DPU MATCH */
    ret = sample_common_svp_dpu_rect_bind_match(g_dpu_cfg.dpu_rect_grp, g_dpu_cfg.dpu_match_grp);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_2, "dpu rect bind match failed!\n");

    /* step5:  send frame to DPU RECT */
    ret = sample_svp_dpu_open_file(src_file_name, HI_DPU_RECT_MAX_PIPE_NUM, dst_file_name, SAMPLE_SVP_DPU_OUTPUT_NUM);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_3, "fopen file failed!\n");

    ret = sample_svp_dpu_proc_frame_from_file(&pic_size[0]);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail_3, "release dpu match frame failed!\n");

fail_3:
    sample_svp_dpu_close_file();
    (hi_void)sample_common_svp_dpu_rect_unbind_match(g_dpu_cfg.dpu_rect_grp, g_dpu_cfg.dpu_match_grp);
fail_2:
    sample_svp_dpu_uninit_dpu_match();
fail_1:
    sample_svp_dpu_uninit_dpu_rect();
fail_0:
    sample_svp_dpu_sys_uninit_file_case();
    return ret;
}

hi_void sample_svp_dpu_file_rect_match_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}

/* This case only for function design reference */
hi_s32 sample_svp_dpu_vi_vpss_rect_match(hi_void)
{
    return HI_SUCCESS;
}

hi_void sample_svp_dpu_vi_vpss_rect_match_handle_sig(hi_void)
{
    return;
}
