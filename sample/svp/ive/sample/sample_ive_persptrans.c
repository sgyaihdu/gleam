/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_ive.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <semaphore.h>
#include <pthread.h>
#include <limits.h>

#define HI_SAMPLE_IVE_PSP_ROI_NUM               64
#define HI_SAMPLE_IVE_PSP_QUERY_SLEEP           100
#define HI_SAMPLE_IVE_PSP_POINT_PAIR_NUM        5
#define HI_PSP_NUM_TWO                          2
#define HI_PSP_NUM_THREE                        3
#define HI_PSP_NUM_FOUR                         4
#define HI_SAMPLE_IVE_PSP_LEFT_SHIT             2
#define HI_SAMPLE_IVE_PSP_SRC_WIDTH             250
#define HI_SAMPLE_IVE_PSP_SRC_HEIGHT            250
#define HI_SAMPLE_IVE_PSP_DST_WIDTH             96
#define HI_SAMPLE_IVE_PSP_DST_HEIGHT            112
#define HI_SAMPLE_IVE_PSP_MAX_POINT_PAIR_NUM    68
#define HI_SAMPLE_IVE_PSP_ROI_WIDTH             250
#define HI_SAMPLE_IVE_PSP_ROI_HEIGHT            250

typedef struct {
    hi_svp_src_img src;
    hi_svp_rect_u32 roi[HI_SAMPLE_IVE_PSP_ROI_NUM];
    hi_u16 roi_num;
    hi_svp_dst_img dst[HI_SAMPLE_IVE_PSP_ROI_NUM];
    hi_svp_src_mem_info point_pair[HI_SAMPLE_IVE_PSP_ROI_NUM];
    hi_ive_persp_trans_ctrl persp_trans_ctrl;

    FILE *fp_src;
    FILE *fp_dst;
} hi_sample_ive_persp_trans_info;

typedef struct {
    hi_svp_rect_u32 roi[HI_SAMPLE_IVE_PSP_ROI_NUM];
    hi_u16 roi_num;
    hi_u16 max_point_pair_num;
} hi_sample_ive_psp_roi_info;

static hi_sample_ive_persp_trans_info g_persp_trans;
static hi_bool g_stop_signal = HI_FALSE;

static hi_void sample_ive_persp_trans_uninit(hi_sample_ive_persp_trans_info *psp_info)
{
    hi_u16 i;

    sample_svp_check_exps_return_void(psp_info == HI_NULL, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");

    sample_svp_mmz_free(psp_info->src.phys_addr[0], psp_info->src.virt_addr[0]);

    for (i = 0; i < psp_info->roi_num; i++) {
        sample_svp_mmz_free(psp_info->dst[i].phys_addr[0], psp_info->dst[i].virt_addr[0]);
        sample_svp_mmz_free(psp_info->point_pair[i].phys_addr, psp_info->point_pair[i].virt_addr);
    }

    sample_svp_close_file(psp_info->fp_src);
    sample_svp_close_file(psp_info->fp_dst);
}

static hi_s32 sample_ive_persp_trans_proc(hi_sample_ive_persp_trans_info *persp_trans)
{
    hi_s32 ret = HI_ERR_IVE_NULL_PTR;
    hi_ive_handle handle;
    hi_bool is_finish = HI_FALSE;
    hi_bool is_block = HI_TRUE;
    hi_bool is_instant = HI_TRUE;
    hi_u32 i;

    sample_svp_check_exps_return(persp_trans == HI_NULL, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "test_mem can't be null\n");
    for (i = 0; (i < 1) && (g_stop_signal == HI_FALSE); i++) {
        ret = sample_common_ive_read_file(&(persp_trans->src), persp_trans->fp_src);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Read src file failed!\n", ret);

        ret = hi_mpi_ive_persp_trans(&handle, &persp_trans->src, persp_trans->roi, persp_trans->point_pair,
            persp_trans->dst, &persp_trans->persp_trans_ctrl, is_instant);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_persp_trans failed!\n", ret);

        ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        while (ret == HI_ERR_IVE_QUERY_TIMEOUT) {
            usleep(HI_SAMPLE_IVE_PSP_QUERY_SLEEP);
            ret = hi_mpi_ive_query(handle, &is_finish, is_block);
        }
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),hi_mpi_ive_query failed!\n", ret);

        ret = sample_common_ive_write_file(&persp_trans->dst[0], persp_trans->fp_dst);
        sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x),Read src file failed!\n", ret);
    }
    return HI_SUCCESS;
}

static hi_void sample_ive_persp_trans_ctrl_init(hi_sample_ive_persp_trans_info *psp_info,
    hi_u16 roi_num)
{
    psp_info->persp_trans_ctrl.alg_mode = HI_IVE_PERSP_TRANS_ALG_MODE_AFFINE;
    psp_info->persp_trans_ctrl.csc_mode = HI_IVE_PERSP_TRANS_CSC_MODE_NONE;
    psp_info->persp_trans_ctrl.roi_num = roi_num;
    psp_info->persp_trans_ctrl.point_pair_num = HI_SAMPLE_IVE_PSP_POINT_PAIR_NUM;
    psp_info->roi_num = roi_num;
}

static hi_s32 sample_ive_persp_trans_init(hi_sample_ive_persp_trans_info *psp,
    hi_sample_src_dst_size data, hi_sample_ive_psp_roi_info *psp_roi,
    const hi_char *src_file, const hi_char *dst_file)
{
    hi_s32 ret = HI_ERR_IVE_ILLEGAL_PARAM;
    hi_u32 size, i, j;
    hi_char path[PATH_MAX + 1] = {0};
    hi_u16 mark[] = { 107, 109, 30, 52, 149, 117, 66, 52, 123, 135, 48, 72, 99, 157, 34, 92, 144, 157, 63, 92 };
    hi_ive_persp_trans_point_pair *tmp = HI_NULL;

    sample_svp_check_exps_return(psp == HI_NULL, HI_ERR_IVE_NULL_PTR, SAMPLE_SVP_ERR_LEVEL_ERROR, "psp is null\n");
    sample_svp_check_exps_return((strlen(src_file) > PATH_MAX) || (realpath(src_file, path) == HI_NULL),
        ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");

    size = (hi_u32)sizeof(hi_svp_rect_u32) * psp_roi->roi_num;
    ret = memcpy_s(psp->roi, sizeof(hi_svp_rect_u32) * HI_SAMPLE_IVE_PSP_ROI_NUM, psp_roi->roi, size);
    sample_svp_check_exps_return(ret != EOK, HI_ERR_IVE_ILLEGAL_PARAM, SAMPLE_SVP_ERR_LEVEL_ERROR, "copy failed!\n");

    ret = sample_common_ive_create_image(&(psp->src), HI_SVP_IMG_TYPE_YUV420SP, data.src.width, data.src.height);
    sample_svp_check_failed_err_level_goto(ret, fail, "Error(%#x),Create src image failed!\n", ret);

    for (i = 0; i < psp_roi->roi_num; i++) {
        ret = sample_common_ive_create_image(&(psp->dst[i]), HI_SVP_IMG_TYPE_YUV420SP, data.dst.width, data.dst.height);
        sample_svp_check_failed_err_level_goto(ret, fail, "Error(%#x),Create src image failed!\n", ret);
    }

    size = (hi_u32)sizeof(hi_ive_persp_trans_point_pair) * psp_roi->max_point_pair_num;
    for (i = 0; i < psp_roi->roi_num; i++) {
        ret = sample_common_ive_create_mem_info(&(psp->point_pair[i]), size);
        sample_svp_check_failed_err_level_goto(ret, fail, "Error(%#x),Create src image failed!\n", ret);
    }

    sample_ive_persp_trans_ctrl_init(psp, psp_roi->roi_num);
    for (i = 0; i < psp_roi->roi_num; i++) {
        tmp = (hi_ive_persp_trans_point_pair *)(hi_uintptr_t)psp->point_pair[i].virt_addr;
        for (j = 0; (j < psp->persp_trans_ctrl.point_pair_num) && (j < HI_SAMPLE_IVE_PSP_POINT_PAIR_NUM); j++) {
            tmp->src_point.x = mark[j * HI_PSP_NUM_FOUR] << HI_SAMPLE_IVE_PSP_LEFT_SHIT;
            tmp->src_point.y = mark[j * HI_PSP_NUM_FOUR + 1] << HI_SAMPLE_IVE_PSP_LEFT_SHIT;
            tmp->dst_point.x = mark[j * HI_PSP_NUM_FOUR + HI_PSP_NUM_TWO] << HI_SAMPLE_IVE_PSP_LEFT_SHIT;
            tmp->dst_point.y = mark[j * HI_PSP_NUM_FOUR + HI_PSP_NUM_THREE] << HI_SAMPLE_IVE_PSP_LEFT_SHIT;
            tmp++;
        }
    }

    /* open src file */
    ret = HI_FAILURE;
    psp->fp_src = fopen(path, "rb");
    sample_svp_check_exps_goto(psp->fp_src == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    /* open dst file */
    sample_svp_check_exps_goto(realpath(dst_file, path) == NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "invalid file!\n");
    ret = strcat_s(path, PATH_MAX, "/Amelia_Vega_Affine_96x112_420sp.yuv");
    sample_svp_check_exps_goto(ret != EOK, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "strcat_s failed!\n");
    ret = HI_FAILURE;
    psp->fp_dst = fopen(path, "wb");
    sample_svp_check_exps_goto(psp->fp_dst == HI_NULL, fail, SAMPLE_SVP_ERR_LEVEL_ERROR, "Open file failed!\n");

    return HI_SUCCESS;
fail:
    sample_ive_persp_trans_uninit(psp);
    return ret;
}

static hi_void sample_ive_persp_trans_stop(hi_void)
{
    sample_ive_persp_trans_uninit(&g_persp_trans);
    (hi_void)memset_s(&g_persp_trans, sizeof(g_persp_trans), 0, sizeof(g_persp_trans));
    sample_common_ive_mpi_exit();
    printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
}
hi_void sample_ive_persp_trans(hi_void)
{
    hi_s32 ret;
    const hi_char *src_file = "./data/input/psp/src/Amelia_Vega_250x250_420sp.yuv";
    const hi_char *dst_file = "./data/output/psp";
    hi_sample_src_dst_size data;
    data.src.width = HI_SAMPLE_IVE_PSP_SRC_WIDTH;
    data.src.height = HI_SAMPLE_IVE_PSP_SRC_HEIGHT;
    data.dst.width = HI_SAMPLE_IVE_PSP_DST_WIDTH;
    data.dst.height = HI_SAMPLE_IVE_PSP_DST_HEIGHT;

    hi_sample_ive_psp_roi_info psp_roi;
    psp_roi.roi_num = 1;
    psp_roi.max_point_pair_num = HI_SAMPLE_IVE_PSP_MAX_POINT_PAIR_NUM;
    psp_roi.roi[0].x = 0;
    psp_roi.roi[0].y = 0;
    psp_roi.roi[0].width = HI_SAMPLE_IVE_PSP_ROI_WIDTH;
    psp_roi.roi[0].height = HI_SAMPLE_IVE_PSP_ROI_HEIGHT;

    (hi_void)memset_s(&g_persp_trans, sizeof(g_persp_trans), 0, sizeof(g_persp_trans));
    ret = sample_common_ive_check_mpi_init();
    sample_svp_check_exps_return_void(ret != HI_TRUE, SAMPLE_SVP_ERR_LEVEL_ERROR, "ive_check_mpi_init failed!\n");

    ret = sample_ive_persp_trans_init(&g_persp_trans, data, &psp_roi, src_file, dst_file);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, persp_trans_fail, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),sample_ive_persp_trans_init failed!\n", ret);

    ret = sample_ive_persp_trans_proc(&g_persp_trans);
    if (g_stop_signal == HI_TRUE) {
        sample_ive_persp_trans_stop();
        return;
    }
    if (ret == HI_SUCCESS) {
        sample_svp_trace_info("Process success!\n");
    }

    g_stop_signal = HI_TRUE;
    sample_ive_persp_trans_uninit(&g_persp_trans);
    (hi_void)memset_s(&g_persp_trans, sizeof(g_persp_trans), 0, sizeof(g_persp_trans));

persp_trans_fail:
    g_stop_signal = HI_TRUE;
    sample_common_ive_mpi_exit();
}

/*
 * function : PerspTrans sample signal handle
 */
hi_void sample_ive_persp_trans_handle_sig(hi_void)
{
    g_stop_signal = HI_TRUE;
}
