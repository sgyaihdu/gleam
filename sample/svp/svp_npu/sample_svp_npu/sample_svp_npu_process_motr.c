/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <linux/limits.h>
#include "sample_svp_npu_process.h"
#include "svp_acl_rt.h"
#include "svp_acl.h"
#include "svp_acl_ext.h"
#include "sample_common_svp.h"
#include "sample_common_svp_npu.h"
#include "sample_common_svp_npu_model.h"
#include "sample_svp_npu_resources.h"
#include "sample_svp_npu_process_motr.h"

#define SAMPLE_SVP_NPU_MOTR_BOX 50 /* 50: yolox to motr max box value */
#define SAMPLE_SVP_NPU_MOTR_CHANNEL 256 /* 256: query and hidden c */
#define SAMPLE_SVP_NPU_MOTR_WIDTH 120 /* 120: (yolox) 50 + (pre) 50 + (last) 20 */
#define SAMPLE_SVP_NPU_MOTR_CONF_SIZE 4  /* 4: motr conf size: x,y,w,h */
#define SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP 2
#define SAMPLE_SVP_NPU_TRACKER_EXPIRE_MAX 20
#define SAMPLE_SVP_NPU_TRACKER_CONTAINER 200
#define SAMPLE_SVP_NPU_YOLOX_INPUT_W 640
#define SAMPLE_SVP_NPU_YOLOX_INPUT_H 384
#define SAMPLE_SVP_NPU_MOTR_INPUT_W 576
#define SAMPLE_SVP_NPU_MOTR_INPUT_H 320
#define SAMPLE_SVP_NPU_YOLOX_IMAGE_IN 0 /* [640, 384] YUV420P Image */
#define SAMPLE_SVP_NPU_MOTR_IMAGE_IN 0 /* [576, 320] YUV420P Image */
#define SAMPLE_SVP_NPU_MOTR_QUERY_IN 1 /* [256, 50] query in each channel (querypre ... | train_data) */
#define SAMPLE_SVP_NPU_MOTR_CONF_IN 2 /* [50] Conf from YoloX */
#define SAMPLE_SVP_NPU_MOTR_BOX_IN 3 /* [120, 4] (0-50) RefPre, (50-100) YoloX xywh, (100-120) refTrain */
#define SAMPLE_SVP_NPU_MOTR_CONF_OUT 0 /* [120] Conf of Motr */
#define SAMPLE_SVP_NPU_MOTR_BOX_OUT 2 /* [120, 4] xyhw Output of Motr */
#define SAMPLE_SVP_NPU_MOTR_HID_OUT 1 /* [120, 256] hidden representation of Motr */
#define SAMPLE_SVP_NPU_QIM_QUERY_IN 0 /* [256, 50] query of Qim */
#define SAMPLE_SVP_NPU_QIM_HID_IN 1 /* [120, 256] hidden of Qim */
#define SAMPLE_SVP_NPU_QIM_BOX_IN 2 /* [120, 4] xyhw of Qim */
#define SAMPLE_SVP_NPU_QIM_CONF_IN 3 /* [50] conf of Qim */
#define SAMPLE_SVP_NPU_QIM_QUERY_OUT 0 /* [120, 256] */
#define SAMPLE_SVP_NPU_CROP_BOX_SIZE 5 /* [X, Y, W, H, Conf] */
#define SAMPLE_SVP_NPU_CROP_X 0
#define SAMPLE_SVP_NPU_CROP_Y 1
#define SAMPLE_SVP_NPU_CROP_W 2
#define SAMPLE_SVP_NPU_CROP_H 3
#define SAMPLE_SVP_NPU_CROP_CONF_BIT 4
#define SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE 7 /* [X1, Y1, X2, Y2, Conf, CLS_IDX, Area] */
#define SAMPLE_SVP_NPU_NMS_THR 0.45
#define SAMPLE_SVP_NPU_NMS_X1 0
#define SAMPLE_SVP_NPU_NMS_Y1 1
#define SAMPLE_SVP_NPU_NMS_X2 2
#define SAMPLE_SVP_NPU_NMS_Y2 3
#define SAMPLE_SVP_NPU_NMS_CONF 4
#define SAMPLE_SVP_NPU_NMS_CLS 5
#define SAMPLE_SVP_NPU_NMS_AREA_BIT 6
#define SAMPLE_SVP_NPU_HALF_DIV 2
#define SAMPLE_SVP_NPU_SCORE_THR 0.5
#define SAMPLE_SVP_NPU_VALID_SIZE 6 /* [Conf, X, Y, W, H, CLS_IDX] */
#define SAMPLE_SVP_NPU_VGS_RGN_CNT 30
#define SAMPLE_SVP_NPU_VALID_CONF 0
#define SAMPLE_SVP_NPU_VALID_X 1
#define SAMPLE_SVP_NPU_VALID_Y 2
#define SAMPLE_SVP_NPU_VALID_W 3
#define SAMPLE_SVP_NPU_VALID_H 4
#define SAMPLE_SVP_NPU_VALID_CLS 5
#define SAMPLE_SVP_NPU_YOLOX_SCALE 3
#define SAMPLE_SVP_NPU_YOLOX_SCALE_0 8
#define SAMPLE_SVP_NPU_YOLOX_SCALE_1 16
#define SAMPLE_SVP_NPU_YOLOX_SCALE_2 32
#define SAMPLE_SVP_NPU_YOLOX_SCALE_0_IDX 0
#define SAMPLE_SVP_NPU_YOLOX_SCALE_1_IDX 1
#define SAMPLE_SVP_NPU_RES_REF_LAST_SEG_NUM 20
#define SAMPLE_SVP_NPU_YOLOX_CELL_AXIS 1
#define SAMPLE_SVP_NPU_YOLOX_CELL_SIZE 8400
#define SAMPLE_SVP_NPU_DEFAULT_MOTR_TRAIN_DATA (-0.07625690102577209473)
#define SAMPLE_SVP_NPU_TRACKER_NOT_FIND (-1)
#define SAMPLE_SVP_NPU_MOTR_VPSS_CHN 3
#define SAMPLE_SVP_NPU_YOLOX_OUTPUT_H_AXIS 2
#define SAMPLE_SVP_NPU_YOLOX_OUTPUT_W_AXIS 3
#define SAMPLE_SVP_NPU_DUMMY_QUERY "./data/vector/dummy_query.txt" /* motr hyper param */
#define SAMPLE_SVP_NPU_DUMMY_XYWH "./data/vector/dummy_xywh.txt" /* motr hyper param */
#define SAMPLE_SVP_NPU_DUMMY_HID "./data/vector/dummy_hidden.txt" /* motr hyper param */
#define SAMPLE_SVP_NPU_DUMMY_CONF_QIM "./data/vector/dummy_conf_qim.txt" /* motr hyper param */
#define SAMPLE_SVP_NPU_DUMMY_QUERY_QIM "./data/vector/dummy_query_qim.txt" /* motr hyper param */
#define SAMPLE_SVP_NPU_VIDEO_PATH "dance_video.h264" /* name of input video for motr Vdec input */

#define SAMPLE_SVP_NPU_MOTR_VO_WIDTH    1920
#define SAMPLE_SVP_NPU_MOTR_VO_HEIGHT   1080
#define SAMPLE_SVP_NPU_SHAERD_WORK_BUF_NUM 1
#define SAMPLE_SVP_NPU_PATH_LEN 0x100
#define SAMPLE_SVP_NPU_MOTR_DEFAULT_FPS 30 /* Default fps. Warning! too high fps may cause frame skip! */
#define SAMPLE_SVP_NPU_MOTR_PREALLOC_MMZ 430080 /* if not enough: change it within mmz malloc limit */
#define SAMPLE_SVP_NPU_G_C_FILE_LEN 128
#define SAMPLE_SVP_NPU_RELEASE_FROM_YOLOX (-1)
#define SAMPLE_SVP_NPU_RELEASE_FROM_MOTR (-2)
#define SAMPLE_SVP_NPU_RELEASE_FROM_QIM (-3)

static hi_void *g_svp_npu_vb_virt_addr_yolox = HI_NULL; /* For YoloX Input Frame */
static hi_void *g_svp_npu_vb_virt_addr_motr = HI_NULL; /* For Motr Input Frame */
static hi_vb_pool_info g_svp_npu_vb_pool_info_yolox; /* For YoloX Input Frame */
static hi_vb_pool_info g_svp_npu_vb_pool_info_motr; /* For Motr Input Frame */

static hi_void *g_svp_npu_mmz_alloc_base_attr = HI_NULL;
static hi_u32 g_svp_npu_mmz_alloc_base_offset = 0;
static hi_float *g_query_traindata = HI_NULL;
static hi_float *g_ref_traindata = HI_NULL;
static hi_float *g_query_pre = HI_NULL;
static hi_float *g_ref_pre = HI_NULL;
static hi_float *g_res_conf = HI_NULL;
static hi_u32 g_res_confnum = 0;
static hi_float *g_hidden_traindata = HI_NULL;
static hi_float *g_qim_conf_traindata = HI_NULL;
static hi_float *g_qim_query_traindata = HI_NULL;
static hi_float *g_ref_res = HI_NULL;

static hi_float *g_tmp_box = HI_NULL;
static hi_s32 *g_swap_list = HI_NULL;
static hi_float *g_crop = HI_NULL;
static hi_float *g_grids_x = HI_NULL;
static hi_float *g_grids_y = HI_NULL;
static hi_float *g_valid_box = HI_NULL;
static hi_float *g_tmp_insert_sort_buf = HI_NULL;
static hi_vgs_osd *g_osd_buf = HI_NULL;

typedef struct {
    hi_s32 expire_max;
    hi_s32 max_id;
    hi_s32 container_max;
    hi_s32 *id_position;
    hi_s32 *lost_frames;
} sample_svp_npu_motr_tracker;

static hi_void sample_svp_npu_motr_tracker_add_pos(sample_svp_npu_motr_tracker *tracker, hi_s32 pos)
{
    hi_s32 new_id = tracker->max_id;
    tracker->max_id++;
    tracker->lost_frames[new_id] = 0;
    tracker->id_position[new_id] = pos;
}

static hi_bool sample_svp_npu_motr_tracker_is_positionid(const sample_svp_npu_motr_tracker *tracker, hi_s32 pos)
{
    for (hi_s32 i = 0; i < tracker->max_id; i++) {
        if (tracker->id_position[i] == pos) {
            return HI_FALSE;
        }
    }
    return HI_TRUE;
}

static hi_s32 sample_svp_npu_motr_tracker_get_positionid(const sample_svp_npu_motr_tracker *tracker, hi_s32 pos)
{
    for (hi_s32 i = tracker->max_id - 1; i >= 0; i--) {
        if (tracker->id_position[i] == pos) {
            return i;
        }
    }
    return SAMPLE_SVP_NPU_TRACKER_NOT_FIND;
}

static hi_bool sample_svp_npu_motr_tracker_is_expired(const sample_svp_npu_motr_tracker *tracker, hi_s32 pos)
{
    for (hi_s32 i = 0; i < tracker->max_id; i++) {
        if (tracker->id_position[i] == pos) {
            if (tracker->lost_frames[i] > tracker->expire_max) {
                return HI_TRUE;
            } else {
                return HI_FALSE;
            }
        }
    }
    return HI_TRUE;
}

static hi_void sample_svp_npu_motr_tracker_expired_count(const sample_svp_npu_motr_tracker *tracker, hi_s32 pos)
{
    for (hi_s32 i = 0; i < tracker->max_id; i++) {
        if (tracker->id_position[i] == pos) {
            tracker->lost_frames[i]++;
        }
    }
}

static hi_void sample_svp_npu_motr_tracker_reset_expire_count(const sample_svp_npu_motr_tracker *tracker, hi_s32 pos)
{
    hi_s32 id = sample_svp_npu_motr_tracker_get_positionid(tracker, pos);
    if (id != SAMPLE_SVP_NPU_TRACKER_NOT_FIND) {
        tracker->lost_frames[id] = 0;
    }
}

static hi_void sample_svp_npu_motr_tracker_clean(const sample_svp_npu_motr_tracker *tracker)
{
    for (hi_s32 i = 0; i < tracker->max_id; i++) {
        if (tracker->lost_frames[i] > tracker->expire_max) {
            tracker->id_position[i] = SAMPLE_SVP_NPU_TRACKER_NOT_FIND;
        }
    }
}

static hi_void sample_svp_npu_motr_tracker_update_pos(const sample_svp_npu_motr_tracker *tracker, hi_s32 *pos_list,
    hi_s32 pos_list_num)
{
    for (hi_s32 i = 0; i < pos_list_num; i++) {
        if (pos_list[i * SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP] == pos_list[i * SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP + 1]) {
            continue;
        }
        hi_s32 old_pos = pos_list[i * SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP];
        hi_s32 idd = sample_svp_npu_motr_tracker_get_positionid(tracker, old_pos);
        if (idd < 0) {
            break;
        }
        hi_s32 new_pos = pos_list[i * SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP + 1];
        tracker->id_position[idd] = new_pos;
    }
}

static hi_s32 sample_svp_npu_motr_acl_vb_map_dual_frames(hi_u32 vb_pool_idx)
{
    hi_s32 ret;
    if (g_svp_npu_vb_virt_addr_yolox == HI_NULL && vb_pool_idx == SAMPLE_SVP_NPU_YOLOX_FRAME) {
        ret = hi_mpi_vb_get_pool_info(sample_svp_npu_get_media_cfg()->vb_pool[vb_pool_idx],
            &g_svp_npu_vb_pool_info_yolox);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get pool info failed!\n");

        g_svp_npu_vb_virt_addr_yolox = hi_mpi_sys_mmap(g_svp_npu_vb_pool_info_yolox.pool_phy_addr,
            g_svp_npu_vb_pool_info_yolox.pool_size);
        sample_svp_check_exps_return(g_svp_npu_vb_virt_addr_yolox == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "map vb pool failed!\n");
    } else if (g_svp_npu_vb_virt_addr_motr == HI_NULL && vb_pool_idx == SAMPLE_SVP_NPU_MOTR_FRAME) {
        ret = hi_mpi_vb_get_pool_info(sample_svp_npu_get_media_cfg()->vb_pool[vb_pool_idx],
            &g_svp_npu_vb_pool_info_motr);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get pool info failed!\n");

        g_svp_npu_vb_virt_addr_motr = hi_mpi_sys_mmap(g_svp_npu_vb_pool_info_motr.pool_phy_addr,
            g_svp_npu_vb_pool_info_motr.pool_size);
        sample_svp_check_exps_return(g_svp_npu_vb_virt_addr_motr == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "map vb pool failed!\n");
    }
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_global_get_mmz_addr(hi_u8 **ptr, hi_u32 size)
{
    sample_svp_check_exps_return(size + g_svp_npu_mmz_alloc_base_offset > SAMPLE_SVP_NPU_MOTR_PREALLOC_MMZ, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "sample svp npu malloc memory failed!\n");
    *ptr = g_svp_npu_mmz_alloc_base_attr + g_svp_npu_mmz_alloc_base_offset;
    g_svp_npu_mmz_alloc_base_offset += size;
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_init_between_frame_globals(sample_svp_npu_motr_tracker *tracker)
{
    sample_svp_check_exps_return(tracker == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "tracker is nullptr");
    g_res_confnum = 0;

    /* Malloc Global Params to Store Information Shared between Frames */
    hi_s32 ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_query_traindata, SAMPLE_SVP_NPU_MOTR_CHANNEL *
        sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_query_traindata == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "query train data alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_ref_traindata, SAMPLE_SVP_NPU_MOTR_CONF_SIZE *
        SAMPLE_SVP_NPU_RES_REF_LAST_SEG_NUM * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_ref_traindata == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "ref train data alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_query_pre, SAMPLE_SVP_NPU_MOTR_CHANNEL *
        SAMPLE_SVP_NPU_MOTR_WIDTH * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_query_pre == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "query pre alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_ref_pre, SAMPLE_SVP_NPU_MOTR_CONF_SIZE *
        SAMPLE_SVP_NPU_MOTR_WIDTH * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_ref_pre == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "ref pre alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_hidden_traindata, SAMPLE_SVP_NPU_MOTR_CHANNEL *
        sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_hidden_traindata == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "hidden train data alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_qim_conf_traindata, sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_qim_conf_traindata == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "conf train data alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_qim_query_traindata, SAMPLE_SVP_NPU_MOTR_CHANNEL *
        sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_qim_query_traindata == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "qim query train data alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_res_conf, SAMPLE_SVP_NPU_MOTR_WIDTH * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_res_conf == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "res conf alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_ref_res, SAMPLE_SVP_NPU_MOTR_CONF_SIZE *
        SAMPLE_SVP_NPU_MOTR_WIDTH * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_ref_res == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "ref res alloc failed!!\n");

    tracker->expire_max = SAMPLE_SVP_NPU_TRACKER_EXPIRE_MAX;
    tracker->max_id = 0;
    tracker->container_max = SAMPLE_SVP_NPU_TRACKER_CONTAINER;
    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &(tracker->id_position), tracker->container_max *
        sizeof(hi_s32));
    sample_svp_check_exps_return(ret != HI_SUCCESS || tracker->id_position == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "tracker id position alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &(tracker->lost_frames), tracker->container_max *
        sizeof(hi_s32));
    sample_svp_check_exps_return(ret != HI_SUCCESS || tracker->lost_frames == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "tracker lost frames alloc failed!!\n");
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_init_inside_frame_globals()
{
    /* Malloc Tmp Params Used inside Single Frame */
    hi_s32 ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_tmp_box, SAMPLE_SVP_NPU_MOTR_BOX *
        SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_tmp_box == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "tmp box alloc failed!!");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_swap_list, SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP *
        SAMPLE_SVP_NPU_MOTR_WIDTH * sizeof(hi_s32));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_swap_list == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "swap list alloc failed!!");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_crop, SAMPLE_SVP_NPU_MOTR_BOX * SAMPLE_SVP_NPU_CROP_BOX_SIZE
        * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_crop == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "crop alloc failed!!");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_grids_x, SAMPLE_SVP_NPU_YOLOX_CELL_SIZE * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_grids_x == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "grids x alloc failed!!");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_grids_y, SAMPLE_SVP_NPU_YOLOX_CELL_SIZE * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_grids_y == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "grids y alloc failed!!");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_valid_box, SAMPLE_SVP_NPU_YOLOX_CELL_SIZE *
        SAMPLE_SVP_NPU_VALID_SIZE * sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_valid_box == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "valid box alloc failed!!");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_tmp_insert_sort_buf, SAMPLE_SVP_NPU_VALID_SIZE *
        sizeof(hi_float));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_tmp_insert_sort_buf == HI_NULL, HI_FAILURE,
        SAMPLE_SVP_ERR_LEVEL_ERROR, "tracker insert sort buf alloc failed!!\n");

    ret = sample_svp_npu_global_get_mmz_addr((hi_u8 **) &g_osd_buf, SAMPLE_SVP_NPU_VGS_RGN_CNT * sizeof(hi_vgs_osd));
    sample_svp_check_exps_return(ret != HI_SUCCESS || g_osd_buf == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "vgs osd buf alloc failed!!");
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_init_global(sample_svp_npu_motr_tracker *tracker)
{
    /* Init Global Data */
    hi_s32 ret = svp_acl_rt_malloc_cached(&g_svp_npu_mmz_alloc_base_attr, SAMPLE_SVP_NPU_MOTR_PREALLOC_MMZ,
        SVP_ACL_MEM_MALLOC_NORMAL_ONLY); /* Malloc Base Size */
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "svp_acl_rt_malloc_cached failed!!\n");

    /* Malloc Global Params to Store Information Shared between Frames */
    ret = sample_svp_npu_motr_init_between_frame_globals(tracker);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_svp_npu_motr_init_between_frame_globals failed!!\n");

    /* Malloc Tmp Params Used inside Single Frame */
    ret = sample_svp_npu_motr_init_inside_frame_globals();
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_svp_npu_motr_init_inside_frame_globals failed!!\n");
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_preprocess_query(hi_float* query_train_data)
{
    hi_s32 ret;
    hi_char sample_svp_npu_dummy_query_realpath[PATH_MAX] = {0};
    sample_svp_check_exps_return(realpath(SAMPLE_SVP_NPU_DUMMY_QUERY, sample_svp_npu_dummy_query_realpath) == HI_NULL,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "file (%s) dose not exists\n", sample_svp_npu_dummy_query_realpath);
    FILE *fp = fopen(sample_svp_npu_dummy_query_realpath, "r");
    sample_svp_check_exps_return(fp == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "open file failed!\n");
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_CHANNEL; i++) {
        ret = fscanf_s(fp, "%f", &query_train_data[i]);
        if (ret <= 0) { /* -1: optee error, 0: no fields were assigned */
            (hi_void)fclose(fp);
            return HI_FAILURE;
        }
    }
    (hi_void)fclose(fp);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_preprocess_refdata(hi_float* res_ref)
{
    hi_s32 ret;
    hi_char sample_svp_npu_dummy_xywh_realpath[PATH_MAX] = {0};
    sample_svp_check_exps_return(realpath(SAMPLE_SVP_NPU_DUMMY_XYWH, sample_svp_npu_dummy_xywh_realpath) == HI_NULL,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "file (%s) dose not exists\n", sample_svp_npu_dummy_xywh_realpath);
    FILE *fp = fopen(sample_svp_npu_dummy_xywh_realpath, "r");
    sample_svp_check_exps_return(fp == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "open file failed!\n");
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_RES_REF_LAST_SEG_NUM * SAMPLE_SVP_NPU_MOTR_CONF_SIZE; i++) {
        ret = fscanf_s(fp, "%f", &res_ref[i]);
        if (ret <= 0) { /* -1: optee error, 0: no fields were assigned */
            (hi_void)fclose(fp);
            return HI_FAILURE;
        }
    }
    (hi_void)fclose(fp);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_preprocess_hiddendata(hi_float* hidden_data)
{
    hi_s32 ret;
    hi_char sample_svp_npu_dummy_hid_realpath[PATH_MAX] = {0};
    sample_svp_check_exps_return(realpath(SAMPLE_SVP_NPU_DUMMY_HID, sample_svp_npu_dummy_hid_realpath) == HI_NULL,
        HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "file (%s) dose not exists\n", sample_svp_npu_dummy_hid_realpath);
    FILE *fp = fopen(sample_svp_npu_dummy_hid_realpath, "r");
    sample_svp_check_exps_return(fp == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "open file failed!\n");
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_CHANNEL; i++) {
        ret = fscanf_s(fp, "%f", &hidden_data[i]);
        if (ret <= 0) { /* -1: optee error, 0: no fields were assigned */
            (hi_void)fclose(fp);
            return HI_FAILURE;
        }
    }
    (hi_void)fclose(fp);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_preprocess_qimconfdata(hi_float* qim_confdata)
{
    hi_s32 ret;
    hi_char sample_svp_npu_dummy_qimconf_realpath[PATH_MAX] = {0};
    sample_svp_check_exps_return(realpath(SAMPLE_SVP_NPU_DUMMY_CONF_QIM, sample_svp_npu_dummy_qimconf_realpath) ==
        HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "file (%s) dose not exist\n",
        sample_svp_npu_dummy_qimconf_realpath);
    FILE *fp = fopen(sample_svp_npu_dummy_qimconf_realpath, "r");
    sample_svp_check_exps_return(fp == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "open file failed!\n");
    for (hi_u32 i = 0; i < 1; i++) {
        ret = fscanf_s(fp, "%f", &qim_confdata[i]);
        if (ret <= 0) { /* -1: optee error, 0: no fields were assigned */
            (hi_void)fclose(fp);
            return HI_FAILURE;
        }
    }
    (hi_void)fclose(fp);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_preprocess_qimquerydata(hi_float* qim_querydata)
{
    hi_s32 ret;
    hi_char sample_svp_npu_dummy_qimquery_realpath[PATH_MAX] = {0};
    sample_svp_check_exps_return(realpath(SAMPLE_SVP_NPU_DUMMY_QUERY_QIM, sample_svp_npu_dummy_qimquery_realpath) ==
        HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "file (%s) dose not exists\n",
        sample_svp_npu_dummy_qimquery_realpath);
    FILE *fp = fopen(sample_svp_npu_dummy_qimquery_realpath, "r");
    sample_svp_check_exps_return(fp == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "open file failed!\n");
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_CHANNEL; i++) {
        ret = fscanf_s(fp, "%f", &qim_querydata[i]);
        if (ret <= 0) { /* -1: optee error, 0: no fields were assigned */
            (hi_void)fclose(fp);
            return HI_FAILURE;
        }
    }
    (hi_void)fclose(fp);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_preprocess_motr_global(sample_svp_npu_motr_tracker *tracker)
{
    hi_s32 ret;
    ret = sample_svp_npu_motr_init_global(tracker);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), global malloc error!\n", ret);

    ret = sample_svp_npu_motr_preprocess_query(g_query_traindata);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), preprocessQuery failed!\n", ret);

    ret = sample_svp_npu_motr_preprocess_refdata(g_ref_traindata);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), preprocessRefQuery failed!\n", ret);

    ret = sample_svp_npu_motr_preprocess_hiddendata(g_hidden_traindata);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), preprocessHiddenData failed!\n", ret);

    ret = sample_svp_npu_motr_preprocess_qimconfdata(g_qim_conf_traindata);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), PreProcessQimConfData failed!\n", ret);

    ret = sample_svp_npu_motr_preprocess_qimquerydata(g_qim_query_traindata);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x), preprocessQimQuerryData failed!\n", ret);
    return HI_SUCCESS;
release:
    if (g_svp_npu_mmz_alloc_base_attr != HI_NULL) {
        (hi_void)svp_acl_rt_free(g_svp_npu_mmz_alloc_base_attr);
    }
    g_svp_npu_mmz_alloc_base_attr = HI_NULL;
    return ret;
}

static hi_s32 sample_svp_npu_destroy_global(sample_svp_npu_motr_tracker *tracker)
{
    /* Allocate complete mmz on g_svp_npu_mmz_alloc_base_attr, only to release once */
    if (g_svp_npu_mmz_alloc_base_attr != HI_NULL) {
        svp_acl_error ret = svp_acl_rt_free(g_svp_npu_mmz_alloc_base_attr);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "Error(%#x), svp_acl_rt_free failed!\n", ret);
    }
    g_svp_npu_mmz_alloc_base_attr = HI_NULL;
    g_svp_npu_mmz_alloc_base_offset = 0;
    g_query_traindata = HI_NULL;
    g_ref_traindata = HI_NULL;
    g_query_pre = HI_NULL;
    g_ref_pre = HI_NULL;
    g_hidden_traindata = HI_NULL;
    g_qim_conf_traindata = HI_NULL;
    g_qim_query_traindata = HI_NULL;
    g_res_conf = HI_NULL;
    g_ref_res = HI_NULL;
    g_tmp_box = HI_NULL;
    g_swap_list = HI_NULL;
    g_valid_box = HI_NULL;
    g_crop = HI_NULL;
    g_grids_x = HI_NULL;
    g_grids_y = HI_NULL;
    g_tmp_insert_sort_buf = HI_NULL;
    g_osd_buf = HI_NULL;
    if (tracker != HI_NULL) {
        tracker->id_position = HI_NULL;
        tracker->lost_frames = HI_NULL;
        tracker->max_id = 0;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_get_tri_frames(hi_video_frame_info *yolox_frame, hi_video_frame_info *motr_frame,
    hi_video_frame_info *show_frame, hi_s32 *vpss_chn, hi_s32 vpss_grp)
{
    hi_s32 ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[SAMPLE_SVP_NPU_YOLOX_FRAME], yolox_frame,
        SAMPLE_SVP_NPU_MILLIC_SEC);
    sample_svp_check_exps_return(ret != HI_SUCCESS, SAMPLE_SVP_NPU_RELEASE_FROM_YOLOX, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_vpss_get_chn_frame failed, vpss_chn(%d)!\n", ret, vpss_chn[SAMPLE_SVP_NPU_YOLOX_FRAME]);

    ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[SAMPLE_SVP_NPU_MOTR_FRAME], motr_frame,
        SAMPLE_SVP_NPU_MILLIC_SEC);
    sample_svp_check_exps_return(ret != HI_SUCCESS, SAMPLE_SVP_NPU_RELEASE_FROM_MOTR, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_vpss_get_chn_frame failed,  vpss_chn(%d)!\n", ret, vpss_chn[SAMPLE_SVP_NPU_MOTR_FRAME]);

    ret = hi_mpi_vpss_get_chn_frame(vpss_grp, vpss_chn[SAMPLE_SVP_NPU_SHOW_FRAME], show_frame,
        SAMPLE_SVP_NPU_MILLIC_SEC);
    sample_svp_check_exps_return(ret != HI_SUCCESS, SAMPLE_SVP_NPU_RELEASE_FROM_QIM, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),hi_mpi_vpss_get_chn_frame failed, vpss_chn(%d)!\n", ret, vpss_chn[SAMPLE_SVP_NPU_SHOW_FRAME]);

    return ret;
}

static hi_s32 sample_svp_npu_release_tri_frames(hi_video_frame_info *yolox_frame, hi_video_frame_info *motr_frame,
    hi_video_frame_info *show_frame, hi_s32 *vpss_chn, hi_s32 release_seq) /* finish */
{
    hi_s32 ret;
    sample_svp_check_exps_goto(release_seq == SAMPLE_SVP_NPU_RELEASE_FROM_YOLOX, release_from_yolox,
        SAMPLE_SVP_ERR_LEVEL_INFO, "release from yolox!\n");
    sample_svp_check_exps_goto(release_seq == SAMPLE_SVP_NPU_RELEASE_FROM_MOTR, release_from_motr,
        SAMPLE_SVP_ERR_LEVEL_INFO, "release from motr!\n");
    sample_svp_check_exps_goto(release_seq == SAMPLE_SVP_NPU_RELEASE_FROM_QIM, release_from_qim,
        SAMPLE_SVP_ERR_LEVEL_INFO, "release from qim!\n");
    ret = hi_mpi_vpss_release_chn_frame(0, vpss_chn[SAMPLE_QIM_TASK], show_frame);
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, 0, vpss_chn[SAMPLE_SVP_NPU_SHOW_FRAME]);
release_from_qim:
    ret = hi_mpi_vpss_release_chn_frame(0, vpss_chn[SAMPLE_MOTR_TASK], motr_frame);
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, 0, vpss_chn[SAMPLE_SVP_NPU_MOTR_FRAME]);
release_from_motr:
    ret = hi_mpi_vpss_release_chn_frame(0, vpss_chn[SAMPLE_YOLOX_TASK], yolox_frame);
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "Error(%#x),release_frame failed,grp(%d) chn(%d)!\n", ret, 0, vpss_chn[SAMPLE_SVP_NPU_YOLOX_FRAME]);
release_from_yolox:
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_motr_flush_motr_mmz(svp_acl_data_buffer *data_buffer_motr_in_query, svp_acl_data_buffer
    *data_buffer_motr_in_conf, svp_acl_data_buffer *data_buffer_motr_in_ref)
{
    hi_s32 ret;
    hi_u32 size_motr_query = svp_acl_get_data_buffer_size(data_buffer_motr_in_query);
    hi_u32 size_motr_conf = svp_acl_get_data_buffer_size(data_buffer_motr_in_conf);
    hi_u32 size_motr_ref = svp_acl_get_data_buffer_size(data_buffer_motr_in_ref);
    hi_u8 *motr_in_query = svp_acl_get_data_buffer_addr(data_buffer_motr_in_query);
    hi_u8 *motr_in_conf = svp_acl_get_data_buffer_addr(data_buffer_motr_in_conf);
    hi_u8 *motr_in_ref = svp_acl_get_data_buffer_addr(data_buffer_motr_in_ref);
    ret = svp_acl_rt_mem_flush(motr_in_query, size_motr_query);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush %u-th output data failed, error code is %d!\n", SAMPLE_SVP_NPU_MOTR_QUERY_IN, ret);

    ret = svp_acl_rt_mem_flush(motr_in_conf, size_motr_conf);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush %u-th output data failed, error code is %d!\n", SAMPLE_SVP_NPU_MOTR_CONF_IN, ret);

    ret = svp_acl_rt_mem_flush(motr_in_ref, size_motr_ref);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush %u-th output data failed, error code is %d!\n", SAMPLE_SVP_NPU_MOTR_BOX_IN, ret);
    return ret;
}

static hi_s32 sample_svp_npu_flush_qim_mmz(hi_s32 task_idx)
{
    hi_u32 size_qim_in_query;
    hi_u32 size_qim_in_hidden;
    hi_u32 size_qim_in_ref;
    hi_u32 size_qim_in_conf;
    hi_u32 stride;
    hi_u8 *qim_in_query = HI_NULL;
    hi_u8 *qim_in_hidden = HI_NULL;
    hi_u8 *qim_in_ref = HI_NULL;
    hi_u8 *qim_in_conf = HI_NULL;
    hi_s32 ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(task_idx),
        SAMPLE_SVP_NPU_QIM_QUERY_IN, &qim_in_query, &size_qim_in_query, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(task_idx),
        SAMPLE_SVP_NPU_QIM_HID_IN, &qim_in_hidden, &size_qim_in_hidden, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(task_idx),
        SAMPLE_SVP_NPU_QIM_BOX_IN, &qim_in_ref, &size_qim_in_ref, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(task_idx),
        SAMPLE_SVP_NPU_QIM_CONF_IN, &qim_in_conf, &size_qim_in_conf, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = svp_acl_rt_mem_flush(qim_in_query, size_qim_in_query);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush %u-th output data failed, error code is %d!\n", SAMPLE_SVP_NPU_QIM_QUERY_IN, ret);

    ret = svp_acl_rt_mem_flush(qim_in_hidden, size_qim_in_hidden);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush %u-th output data failed, error code is %d!\n", SAMPLE_SVP_NPU_QIM_HID_IN, ret);

    ret = svp_acl_rt_mem_flush(qim_in_ref, size_qim_in_ref);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush %u-th output data failed, error code is %d!\n", SAMPLE_SVP_NPU_QIM_BOX_IN, ret);

    ret = svp_acl_rt_mem_flush(qim_in_conf, size_qim_in_conf);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush %u-th output data failed, error code is %d!\n", SAMPLE_SVP_NPU_QIM_CONF_IN, ret);
    return ret;
}

static hi_s32 sample_svp_npu_get_dataset_by_name(hi_s32 task_index, const hi_char *name, hi_bool is_input, hi_u8 **data,
    hi_u32 *stride)
{
    hi_s32 ret;
    size_t idx = 0;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    sample_svp_npu_model_info* model_info = sample_common_svp_npu_get_model_info(
        sample_svp_npu_get_task_info(task_index)->cfg.model_idx);
    svp_acl_mdl_desc *desc_ptr =  model_info->model_desc;
    if (is_input) {
        ret = svp_acl_mdl_get_input_index_by_name(desc_ptr, name, &idx);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get input by name %s failed!\n", name);
        data_buffer = svp_acl_mdl_get_dataset_buffer(sample_svp_npu_get_task_info(task_index)->input_dataset, idx);
    } else {
        ret = svp_acl_mdl_get_output_index_by_name(desc_ptr, name, &idx);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get output by name %s failed!\n", name);
        data_buffer = svp_acl_mdl_get_dataset_buffer(sample_svp_npu_get_task_info(task_index)->output_dataset, idx);
    }
    *data = svp_acl_get_data_buffer_addr(data_buffer);
    *stride = svp_acl_get_data_buffer_stride(data_buffer);
    return ret;
}

static hi_void sample_init_box_info_for_vgs(hi_sample_svp_rect_info *rect_info, sample_svp_npu_motr_tracker *tracker,
    hi_u32 width, hi_u32 height)
{
    hi_u32 visit_i = 0;
    for (hi_u32 i = 0; i < g_res_confnum; i++) {
        if (g_res_conf[i] > 0 && i < HI_SVP_RECT_NUM) { /* check rect num */
            hi_u32 id = sample_svp_npu_motr_tracker_get_positionid(tracker, (hi_s32)(i));
            hi_float xl = ceil((g_ref_pre[SAMPLE_SVP_NPU_CROP_X * SAMPLE_SVP_NPU_MOTR_WIDTH + i] -
                g_ref_pre[SAMPLE_SVP_NPU_CROP_W * SAMPLE_SVP_NPU_MOTR_WIDTH + i] / SAMPLE_SVP_NPU_HALF_DIV) * width);
            hi_float yl = ceil((g_ref_pre[SAMPLE_SVP_NPU_CROP_Y * SAMPLE_SVP_NPU_MOTR_WIDTH + i] -
                g_ref_pre[SAMPLE_SVP_NPU_CROP_H * SAMPLE_SVP_NPU_MOTR_WIDTH + i] / SAMPLE_SVP_NPU_HALF_DIV) * height);
            hi_float xh = floor((g_ref_pre[SAMPLE_SVP_NPU_CROP_X * SAMPLE_SVP_NPU_MOTR_WIDTH + i] +
                g_ref_pre[SAMPLE_SVP_NPU_CROP_W * SAMPLE_SVP_NPU_MOTR_WIDTH + i] / SAMPLE_SVP_NPU_HALF_DIV) * width);
            hi_float yh = floor((g_ref_pre[SAMPLE_SVP_NPU_CROP_Y * SAMPLE_SVP_NPU_MOTR_WIDTH + i] +
                g_ref_pre[SAMPLE_SVP_NPU_CROP_H * SAMPLE_SVP_NPU_MOTR_WIDTH + i] / SAMPLE_SVP_NPU_HALF_DIV) * height);
            if (xl <= xh && yl <= yh) { /* VGS support out-framed box */
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].x = (hi_u32)(xl) & (~1);
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_LEFT_TOP].y = (hi_u32)(yl) & (~1);
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_LEFT_BOTTOM].x = (hi_u32)(xl) & (~1);
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_LEFT_BOTTOM].y = (hi_u32)(yh) & (~1);
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_RIGHT_TOP].x = (hi_u32)(xh) & (~1);
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_RIGHT_TOP].y = (hi_u32)(yl) & (~1);
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_RIGHT_BOTTOM].x = (hi_u32)(xh) & (~1);
                rect_info->rect[visit_i].point[SAMPLE_SVP_NPU_RECT_RIGHT_BOTTOM].y = (hi_u32)(yh) & (~1);
                rect_info->ids[visit_i] = id;
                visit_i++;
            }
        }
    }
    rect_info->num = (hi_u16)visit_i;
}

static hi_void sample_gen_yolox_box_info(const svp_acl_mdl_io_dims *input_dims, hi_float* out_data_buffer_addr,
    size_t w_stride_offset, hi_s64 out_height)
{
    hi_float* out_data = out_data_buffer_addr;
    hi_u32 img_height = (hi_u32)(input_dims->dims[SAMPLE_SVP_NPU_YOLOX_OUTPUT_H_AXIS]);
    hi_u32 img_width = (hi_u32)(input_dims->dims[SAMPLE_SVP_NPU_YOLOX_OUTPUT_W_AXIS]);
    hi_u32 expanded_strides[SAMPLE_SVP_NPU_YOLOX_SCALE] = {SAMPLE_SVP_NPU_YOLOX_SCALE_0, SAMPLE_SVP_NPU_YOLOX_SCALE_1,
        SAMPLE_SVP_NPU_YOLOX_SCALE_2};
    hi_u32 h_sizes[SAMPLE_SVP_NPU_YOLOX_SCALE] = {0, 0, 0}, w_sizes[SAMPLE_SVP_NPU_YOLOX_SCALE] = {0, 0, 0};
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_YOLOX_SCALE; i++) {
        h_sizes[i] = img_height / expanded_strides[i];
        w_sizes[i] = img_width / expanded_strides[i];
    }
    hi_u32 grids_cnt = 0;
    for (hi_u32 k = 0; k < SAMPLE_SVP_NPU_YOLOX_SCALE; k++) {
        for (hi_u32 i = 0; i < h_sizes[k]; i++) {
            for (hi_u32 j = 0; j < w_sizes[k]; j++) {
                g_grids_x[grids_cnt] = j;
                g_grids_y[grids_cnt] = i;
                grids_cnt++;
            }
        }
    }
    hi_float *x_center = out_data;
    hi_float *y_center = x_center + 1;
    hi_float *box_w = y_center + 1;
    hi_float *box_h = box_w + 1;
    hi_u32 expanded_idx_phase0 = h_sizes[SAMPLE_SVP_NPU_YOLOX_SCALE_0_IDX] * w_sizes[SAMPLE_SVP_NPU_YOLOX_SCALE_0_IDX];
    hi_u32 expanded_idx_phase1 = h_sizes[SAMPLE_SVP_NPU_YOLOX_SCALE_1_IDX] * w_sizes[SAMPLE_SVP_NPU_YOLOX_SCALE_1_IDX] +
        expanded_idx_phase0;
    hi_u32 expanded_stride_idx = 0;
    for (hi_u32 i = 0; i < out_height; i++) {
        if (i == expanded_idx_phase0 || i == expanded_idx_phase1) {
            expanded_stride_idx++;
        }
        *x_center = (*x_center + g_grids_x[i]) * expanded_strides[expanded_stride_idx];
        *y_center = (*y_center + g_grids_y[i]) * expanded_strides[expanded_stride_idx];
        *box_w = exp(*box_w) * expanded_strides[expanded_stride_idx];
        *box_h = exp(*box_h) * expanded_strides[expanded_stride_idx];
        x_center += w_stride_offset;
        y_center += w_stride_offset;
        box_w += w_stride_offset;
        box_h += w_stride_offset;
    }
}

static hi_void sample_svp_npu_sort_by_box_score(hi_float *valid_box, hi_u32 *valid_box_num, hi_u32 offset,
    hi_u32 confbit)
{
    /* InsertSort */
    hi_s32 flag = 1;
    for (hi_u32 i = 0; i < *valid_box_num && flag; i++) {
        hi_float key = valid_box[i * offset + confbit];
        for (hi_u32 k = 0; k < offset; k++) {
            g_tmp_insert_sort_buf[k] = valid_box[i * offset + k];
        }
        hi_s32 j = (hi_s32)i - 1;
        while (j >= 0 && valid_box[(hi_u32)j * offset + confbit] < key) {
            for (hi_u32 k = 0; k < offset; k++) {
                valid_box[(hi_u32)j * offset + k + offset] =  valid_box[(hi_u32)j * offset + k];
            }
            j--;
        }
        for (hi_u32 k = 0; k < offset; k++) {
            valid_box[(hi_u32)j * offset + k + offset] =  g_tmp_insert_sort_buf[k];
        }
    }
}

static hi_float sample_svp_npu_calc_iou(hi_float *box1, hi_float *box2, hi_u32 j)
{
    hi_float area1 = box1[SAMPLE_SVP_NPU_NMS_AREA_BIT];
    hi_float area2 = box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_AREA_BIT];
    hi_float xx1 = (box1[SAMPLE_SVP_NPU_NMS_X1] > box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_X1]) ? box1[SAMPLE_SVP_NPU_NMS_X1] : box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_X1];
    hi_float yy1 = (box1[SAMPLE_SVP_NPU_NMS_Y1] > box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_Y1]) ? box1[SAMPLE_SVP_NPU_NMS_Y1] : box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_Y1];
    hi_float xx2 = (box1[SAMPLE_SVP_NPU_NMS_X2] < box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_X2]) ? box1[SAMPLE_SVP_NPU_NMS_X2] : box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_X2];
    hi_float yy2 = (box1[SAMPLE_SVP_NPU_NMS_Y2] < box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_Y2]) ? box1[SAMPLE_SVP_NPU_NMS_Y2] : box2[j * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE +
        SAMPLE_SVP_NPU_NMS_Y2];
    hi_float w = (xx2 - xx1 + 1 > 0.0) ? xx2 - xx1 + 1 : 0.0;
    hi_float h = (yy2 - yy1 + 1 > 0.0) ? yy2 - yy1 + 1 : 0.0;
    hi_float inter = w * h;
    hi_float ovr = inter / (area1 + area2 - inter);
    return ovr;
}

static hi_void sample_svp_npu_motr_multiclass_nms(hi_float *boxes, hi_u32 *box_num, hi_float *valid_box,
    hi_u32 valid_box_num, hi_float nms_thr)
{
    hi_u32 tmp_box_num = 0;
    for (hi_u32 i = 0; i < valid_box_num; i++) {
        hi_float box_x_center = valid_box[i * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_X];
        hi_float box_y_center = valid_box[i * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_Y];
        hi_float box_width = valid_box[i * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_W];
        hi_float box_height = valid_box[i * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_H];

        hi_float x1 = (box_x_center - box_width / SAMPLE_SVP_NPU_HALF_DIV);
        hi_float y1 = (box_y_center - box_height / SAMPLE_SVP_NPU_HALF_DIV);
        hi_float x2 = (box_x_center + box_width / SAMPLE_SVP_NPU_HALF_DIV);
        hi_float y2 = (box_y_center + box_height / SAMPLE_SVP_NPU_HALF_DIV);
        hi_float conf = valid_box[i * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_CONF];
        hi_float cls_idx = valid_box[i * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_CLS];
        hi_float area = (x2 - x1 + 1) * (y2 - y1 + 1);
        hi_bool keep = 1;

        hi_float bbox[SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE] = {x1, y1, x2, y2, conf, cls_idx, area};
        hi_float *bbox_ptr = bbox;
        for (hi_u32 j = 0; j < tmp_box_num; j++) {
            if (sample_svp_npu_calc_iou(bbox_ptr, g_tmp_box, j) > nms_thr) {
                keep = 0;
                break;
            }
        }
        if (keep) {
            g_tmp_box[tmp_box_num * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_X1] = x1;
            g_tmp_box[tmp_box_num * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_Y1] = y1;
            g_tmp_box[tmp_box_num * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_X2] = x2;
            g_tmp_box[tmp_box_num * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_Y2] = y2;
            g_tmp_box[tmp_box_num * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_CONF] = valid_box[i *
                SAMPLE_SVP_NPU_VALID_SIZE];
            g_tmp_box[tmp_box_num * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_CLS] = valid_box[i *
                SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_CLS];
            g_tmp_box[tmp_box_num * SAMPLE_SVP_NPU_NMS_TMP_BOX_SIZE + SAMPLE_SVP_NPU_NMS_AREA_BIT] = area;
            tmp_box_num++;

            boxes[(*box_num) * SAMPLE_SVP_NPU_CROP_BOX_SIZE + SAMPLE_SVP_NPU_CROP_X] = box_x_center /
                SAMPLE_SVP_NPU_YOLOX_INPUT_W;
            boxes[(*box_num) * SAMPLE_SVP_NPU_CROP_BOX_SIZE + SAMPLE_SVP_NPU_CROP_Y] = box_y_center /
                SAMPLE_SVP_NPU_YOLOX_INPUT_H;
            boxes[(*box_num) * SAMPLE_SVP_NPU_CROP_BOX_SIZE + SAMPLE_SVP_NPU_CROP_W] = box_width /
                SAMPLE_SVP_NPU_YOLOX_INPUT_W;
            boxes[(*box_num) * SAMPLE_SVP_NPU_CROP_BOX_SIZE + SAMPLE_SVP_NPU_CROP_H] = box_height /
                SAMPLE_SVP_NPU_YOLOX_INPUT_H;
            boxes[(*box_num) * SAMPLE_SVP_NPU_CROP_BOX_SIZE + SAMPLE_SVP_NPU_CROP_CONF_BIT] = valid_box[i *
                SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_CONF];
            (*box_num)++;
        }
    }
}

static hi_void sample_svp_npu_filter_box(hi_float* out_data_buffer_addr, size_t w_stride_offset, hi_s64 out_height,
    hi_float *crop, hi_u32 *crop_size)
{
    const hi_float *out_data = (const hi_float *)out_data_buffer_addr;
    const hi_float score_thr = SAMPLE_SVP_NPU_SCORE_THR; /* 0.5 */
    const hi_float *obj_score = out_data + SAMPLE_SVP_NPU_MOTR_CONF_SIZE; /* 4: x, y, w, h */
    hi_u32 valid_box_num = 0;
    hi_float tmp_score;
    for (hi_u32 i = 0; i < out_height; i++) {
        const hi_float *class_score = obj_score + 1;
        hi_float max_score = 0.0;
        hi_u32 max_cls_idx = 1;
        hi_bool valid_score = 0;
        for (hi_u32 j = 0; j < 1; j++) {
            tmp_score = (*obj_score) * (*class_score);
            if (tmp_score > score_thr && tmp_score > max_score) {
                max_cls_idx = j;
                max_score = tmp_score;
            }
            if (tmp_score > score_thr) {
                valid_score = 1;
            }
            class_score++;
        }
        if (valid_score) {
            const hi_float *box_info = out_data + i * w_stride_offset;
            hi_float x_center = *(box_info + SAMPLE_SVP_NPU_CROP_X);
            hi_float y_center = *(box_info + SAMPLE_SVP_NPU_CROP_Y);
            hi_float box_width = *(box_info + SAMPLE_SVP_NPU_CROP_W);
            hi_float box_height = *(box_info + SAMPLE_SVP_NPU_CROP_H);
            g_valid_box[valid_box_num * SAMPLE_SVP_NPU_VALID_SIZE] = max_score,
            g_valid_box[valid_box_num * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_X] = x_center;
            g_valid_box[valid_box_num * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_Y] = y_center,
            g_valid_box[valid_box_num * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_W] = box_width;
            g_valid_box[valid_box_num * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_H] = box_height;
            g_valid_box[valid_box_num * SAMPLE_SVP_NPU_VALID_SIZE + SAMPLE_SVP_NPU_VALID_CLS] =
                (hi_float)max_cls_idx;
            valid_box_num++;
        }
        obj_score += w_stride_offset;
    }
    sample_svp_npu_sort_by_box_score(g_valid_box, &valid_box_num, SAMPLE_SVP_NPU_VALID_SIZE, SAMPLE_SVP_NPU_VALID_CONF);
    sample_svp_npu_motr_multiclass_nms(crop, crop_size, g_valid_box, valid_box_num, SAMPLE_SVP_NPU_NMS_THR);
}

static hi_void sample_motr_track_fill_hidden_ref_traindata(hi_float *motr_out_hidden, hi_u32 stride_motr_out_hidden,
    hi_s32 row_idx)
{
    stride_motr_out_hidden /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CHANNEL; j++) {
        hi_float* hidden_ptr = motr_out_hidden + row_idx + j * stride_motr_out_hidden;
        *hidden_ptr = g_hidden_traindata[j];
    }
    g_ref_res[SAMPLE_SVP_NPU_CROP_X * SAMPLE_SVP_NPU_MOTR_WIDTH + row_idx] = -1;
    g_ref_res[SAMPLE_SVP_NPU_CROP_Y * SAMPLE_SVP_NPU_MOTR_WIDTH + row_idx] = -1;
    g_ref_res[SAMPLE_SVP_NPU_CROP_W * SAMPLE_SVP_NPU_MOTR_WIDTH + row_idx] = 0;
    g_ref_res[SAMPLE_SVP_NPU_CROP_H * SAMPLE_SVP_NPU_MOTR_WIDTH + row_idx] = 0;
}

static hi_void sample_motr_track_fill_query_conf_traindata(hi_float *motr_in_query, hi_u32 stride_motr_query,
    hi_float *motr_in_conf, hi_s32 row_idx)
{
    stride_motr_query /= (hi_u32)(sizeof(hi_float));
    if (row_idx < SAMPLE_SVP_NPU_MOTR_BOX) {
        for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CHANNEL; j++) {
            hi_float* query_ptr = motr_in_query + row_idx + j * stride_motr_query;
            *query_ptr = g_qim_query_traindata[j];
        }
    } else if (row_idx < SAMPLE_SVP_NPU_MOTR_BOX + SAMPLE_SVP_NPU_MOTR_BOX) {
        hi_float* conf_yolox_ptr = motr_in_conf + (row_idx - SAMPLE_SVP_NPU_MOTR_BOX);
        *conf_yolox_ptr = g_qim_conf_traindata[0];
    }
}

static hi_void sample_motr_track_fill_ref_res(hi_float *motr_in_ref, hi_u32 stride_motr_ref, hi_u32 row_idx)
{
    stride_motr_ref /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; j++) {
        hi_float* ref_yolox_ptr = motr_in_ref + row_idx + j * stride_motr_ref;
        g_ref_res[j * SAMPLE_SVP_NPU_MOTR_WIDTH + row_idx] = *ref_yolox_ptr;
    }
}

static hi_void sample_svp_npu_fill_motr_data(hi_float *ref_ptr, hi_u32 ref_stride, hi_u32 count, hi_float* crop,
    hi_u32 crop_size)
{
    ref_stride /= (hi_u32)(sizeof(hi_float));
    hi_float *x_center = ref_ptr;
    hi_float *y_center = x_center + ref_stride;
    hi_float *box_w = y_center + ref_stride;
    hi_float *box_h = box_w + ref_stride;
    for (uint32_t i = 0; i < SAMPLE_SVP_NPU_MOTR_WIDTH; i++) {
        if (i < count) {
            for (hi_u32 j = 0 ; j < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; j++) {
                hi_float* ptr = x_center + j * ref_stride;
                *ptr = g_ref_pre[j * SAMPLE_SVP_NPU_MOTR_WIDTH + i];
            }
        } else if (i < SAMPLE_SVP_NPU_MOTR_BOX) {
            *x_center = -1;
            *y_center = -1;
            *box_w = 0;
            *box_h = 0;
        } else if ((i < (uint32_t)crop_size + SAMPLE_SVP_NPU_MOTR_BOX) && (i < SAMPLE_SVP_NPU_MOTR_BOX +
            SAMPLE_SVP_NPU_MOTR_BOX)) {
            for (hi_u32 j = 0 ; j < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; j++) {
                hi_float* ptr = x_center + j * ref_stride;
                *ptr = crop[(i - SAMPLE_SVP_NPU_MOTR_BOX) * (SAMPLE_SVP_NPU_MOTR_CONF_SIZE + 1) + j];
            }
        } else if (i < SAMPLE_SVP_NPU_MOTR_BOX + SAMPLE_SVP_NPU_MOTR_BOX) {
            *x_center = -1;
            *y_center = -1;
            *box_w = 0;
            *box_h = 0;
        } else {
            for (hi_u32  j = 0; j < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; j++) {
                hi_float* ptr = x_center + j * ref_stride;
                *ptr = g_ref_traindata[i - (SAMPLE_SVP_NPU_MOTR_BOX + SAMPLE_SVP_NPU_MOTR_BOX) + j *
                    SAMPLE_SVP_NPU_RES_REF_LAST_SEG_NUM];
            }
        }
        x_center++;
        y_center++;
        box_w++;
        box_h++;
    }
}

static hi_void sample_svp_npu_qim_track_fill_query_pre_unexpired(hi_float *motr_in_query, hi_u32 stride_motr_query,
    hi_u32 row_idx, hi_u32 swap_count)
{
    stride_motr_query /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CHANNEL; j++) {
        hi_float* motr_query_ptr = motr_in_query + row_idx + j * stride_motr_query;
        g_query_pre[j * SAMPLE_SVP_NPU_MOTR_WIDTH + swap_count] = *motr_query_ptr;
    }
}

static hi_void sample_svp_npu_qim_track_fill_ref_pre_unexpired(hi_float *motr_in_ref, hi_u32 stride_motr_ref,
    hi_u32 row_idx, hi_u32 swap_count)
{
    stride_motr_ref /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; j++) {
        hi_float* yolox_ref_ptr = (hi_float*)motr_in_ref + row_idx + j * stride_motr_ref;
        g_ref_pre[j * SAMPLE_SVP_NPU_MOTR_WIDTH + swap_count] = *yolox_ref_ptr;
    }
}

static hi_void sample_svp_npu_qim_track_fill_query_pre_conf(hi_float *qim_out_query, hi_u32 stride_qim_out_query,
    hi_u32 row_idx, hi_u32 swap_count)
{
    stride_qim_out_query /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CHANNEL; j++) {
        hi_float* qim_query_ptr = (hi_float*)qim_out_query + row_idx + j * stride_qim_out_query;
        g_query_pre[j * SAMPLE_SVP_NPU_MOTR_WIDTH + swap_count] = *qim_query_ptr;
    }
}

static hi_void sample_svp_npu_qim_track_fill_ref_pre_conf(hi_float *motr_out_ref, hi_u32 stride_motr_out_ref,
    hi_u32 row_idx, hi_u32 swap_count)
{
    stride_motr_out_ref /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; j++) {
        hi_float* motr_ref_ptr = (hi_float*)motr_out_ref + row_idx + j * stride_motr_out_ref;
        g_ref_pre[j * SAMPLE_SVP_NPU_MOTR_WIDTH + swap_count] = *motr_ref_ptr;
    }
}

static hi_void sample_svp_npu_qim_track_add_swap(hi_s32 *swap_list, hi_u32 *swap_list_cnt, hi_u32 *swap_count,
    hi_u32 row)
{
    swap_list[(*swap_list_cnt) * SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP] = row;
    swap_list[(*swap_list_cnt) * SAMPLE_SVP_NPU_MOTR_TRACKER_SWAP + 1] = *swap_count;
    (*swap_list_cnt)++;
    (*swap_count)++;
}

static hi_s32 sample_svp_npu_post_process_yolox(const sample_svp_npu_task_info *task, hi_float *crop,
    hi_u32 *crop_size)
{
    svp_acl_error ret = HI_SUCCESS;
    svp_acl_data_buffer *data_buffer = HI_NULL;
    hi_void *data = HI_NULL;
    size_t w_stride_offset;
    if (task->cfg.is_cached == HI_TRUE) {
        data_buffer = svp_acl_mdl_get_dataset_buffer(task->output_dataset, 0);
        sample_svp_check_exps_return(data_buffer == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th output data_buffer is NULL!\n", 0);

        svp_acl_mdl_io_dims input_dims;
        sample_svp_npu_model_info* model_info = sample_common_svp_npu_get_model_info(task->cfg.model_idx);
        svp_acl_mdl_desc *desc_ptr =  model_info->model_desc;
        ret = svp_acl_mdl_get_input_dims(desc_ptr, 0, &input_dims);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get input dims failed!\n");
        svp_acl_mdl_io_dims output_dims;
        ret = svp_acl_mdl_get_output_dims(desc_ptr, 0, &output_dims);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get output dims failed!\n");
        data = svp_acl_get_data_buffer_addr(data_buffer);
        sample_svp_check_exps_return(data == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "get %u-th output data is NULL!\n", 0);

        w_stride_offset = svp_acl_mdl_get_output_default_stride(desc_ptr, 0) / sizeof(hi_float);
        int64_t out_height = output_dims.dims[SAMPLE_SVP_NPU_YOLOX_CELL_AXIS];

        sample_gen_yolox_box_info(&input_dims, (hi_float*)data, w_stride_offset, out_height);
        sample_svp_npu_filter_box((hi_float*)data, w_stride_offset, out_height, crop, crop_size);
        sample_svp_npu_sort_by_box_score(crop, crop_size, SAMPLE_SVP_NPU_CROP_BOX_SIZE, SAMPLE_SVP_NPU_CROP_CONF_BIT);
    }
    return ret;
}

static hi_s32 sample_svp_npu_pre_process_motr(hi_float *crop, hi_u32 crop_size, hi_u32 index)
{
    /* Load in buffer 1 for Motr */
    svp_acl_data_buffer *data_buffer_motr_in_query = svp_acl_mdl_get_dataset_buffer(
        sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK)->input_dataset, SAMPLE_SVP_NPU_MOTR_QUERY_IN);
    sample_svp_check_exps_return(data_buffer_motr_in_query == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th data_buffer failed!\n", 1);

    hi_float *motr_in_query = (hi_float*)svp_acl_get_data_buffer_addr(data_buffer_motr_in_query);
    hi_u32 stride_motr_query = svp_acl_get_data_buffer_stride(data_buffer_motr_in_query) / (hi_u32)(sizeof(hi_float));
    hi_u32 count = (g_res_confnum > SAMPLE_SVP_NPU_MOTR_BOX) ? SAMPLE_SVP_NPU_MOTR_BOX : g_res_confnum;
    for (uint32_t i = 0; i < SAMPLE_SVP_NPU_MOTR_CHANNEL; i++) {
        hi_float* query_ptr = motr_in_query + i * stride_motr_query;
        for (hi_u32 j = 0; j < count; j++) {
            *query_ptr = g_query_pre[i * SAMPLE_SVP_NPU_MOTR_WIDTH + j];
            query_ptr++;
        }
        for (hi_u32 j = count; j < SAMPLE_SVP_NPU_MOTR_BOX; j++) {
            *query_ptr = g_query_traindata[i];
            query_ptr++;
        }
    }

    /* Load in buffer 2 for Motr */
    svp_acl_data_buffer *data_buffer_motr_in_conf = svp_acl_mdl_get_dataset_buffer(
        sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK)->input_dataset, SAMPLE_SVP_NPU_MOTR_CONF_IN);
    sample_svp_check_exps_return(data_buffer_motr_in_conf == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th data_buffer failed!\n", SAMPLE_SVP_NPU_MOTR_CONF_IN);
    hi_float* motr_in_conf = svp_acl_get_data_buffer_addr(data_buffer_motr_in_conf);
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_BOX; i++) {
        hi_float* conf_ptr = motr_in_conf + i;
        if (i < (hi_u32)crop_size) {
            *conf_ptr = crop[i * (SAMPLE_SVP_NPU_MOTR_CONF_SIZE + 1) + SAMPLE_SVP_NPU_CROP_CONF_BIT];
        } else {
            *conf_ptr = SAMPLE_SVP_NPU_DEFAULT_MOTR_TRAIN_DATA;
        }
    }

    /* Load in buffer 3 for Motr */
    svp_acl_data_buffer *data_buffer_motr_in_ref = svp_acl_mdl_get_dataset_buffer(
        sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK)->input_dataset, SAMPLE_SVP_NPU_MOTR_BOX_IN);
    sample_svp_check_exps_return(data_buffer_motr_in_ref == HI_NULL, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get %u-th data_buffer failed!\n", SAMPLE_SVP_NPU_MOTR_BOX_IN);
    hi_float* motr_in_ref = (hi_float*)svp_acl_get_data_buffer_addr(data_buffer_motr_in_ref);
    hi_u32 stride_motr_ref = svp_acl_get_data_buffer_stride(data_buffer_motr_in_ref);
    sample_svp_npu_fill_motr_data(motr_in_ref, stride_motr_ref, count, crop, crop_size);
    sample_svp_npu_motr_flush_motr_mmz(data_buffer_motr_in_query, data_buffer_motr_in_conf, data_buffer_motr_in_ref);
    return HI_SUCCESS;
}

static hi_s32 sample_svp_npu_post_process_motr(sample_svp_npu_motr_tracker *tracker, hi_s32 index)
{
    hi_u8 *motr_out_conf = HI_NULL;
    hi_u8 *motr_out_ref = HI_NULL;
    hi_u8 *motr_out_hidden = HI_NULL;
    hi_u8 *motr_in_query = HI_NULL;
    hi_u8 *motr_in_conf = HI_NULL;
    hi_u8 *motr_in_ref = HI_NULL;
    hi_u32 stride_motr_out_ref;
    hi_u32 stride_motr_out_hidden;
    hi_u32 stride_motr_query;
    hi_u32 stride_motr_ref;
    hi_u32 stride;
    svp_acl_data_buffer *data_buffer = HI_NULL;

    data_buffer = svp_acl_mdl_get_dataset_buffer(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK)->input_dataset,
        SAMPLE_SVP_NPU_MOTR_QUERY_IN);
    motr_in_query = svp_acl_get_data_buffer_addr(data_buffer);
    stride_motr_query = svp_acl_get_data_buffer_stride(data_buffer);
    data_buffer = svp_acl_mdl_get_dataset_buffer(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK)->input_dataset,
        SAMPLE_SVP_NPU_MOTR_CONF_IN);
    motr_in_conf = svp_acl_get_data_buffer_addr(data_buffer);
    data_buffer = svp_acl_mdl_get_dataset_buffer(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK)->input_dataset,
        SAMPLE_SVP_NPU_MOTR_BOX_IN);
    motr_in_ref = svp_acl_get_data_buffer_addr(data_buffer);
    stride_motr_ref = svp_acl_get_data_buffer_stride(data_buffer);

    hi_s32 ret = sample_svp_npu_get_dataset_by_name(SAMPLE_MOTR_TASK, "output0", 0, &motr_out_conf, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");
    ret = sample_svp_npu_get_dataset_by_name(SAMPLE_MOTR_TASK, "output1", 0, &motr_out_ref, &stride_motr_out_ref);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");
    ret = sample_svp_npu_get_dataset_by_name(SAMPLE_MOTR_TASK, "output2", 0, &motr_out_hidden, &stride_motr_out_hidden);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    stride_motr_out_ref /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_WIDTH; i++) {
        hi_float* conf_motr = (hi_float*)motr_out_conf + i;
        if (*conf_motr < 0) {
            if (sample_svp_npu_motr_tracker_is_expired(tracker, i)) {
                sample_motr_track_fill_hidden_ref_traindata((hi_float*)motr_out_hidden, stride_motr_out_hidden, i);
                sample_motr_track_fill_query_conf_traindata((hi_float*)motr_in_query, stride_motr_query,
                    (hi_float*)motr_in_conf, i);
            } else {
                sample_motr_track_fill_ref_res((hi_float*)motr_in_ref, stride_motr_ref, i);
            }
        } else {
            for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; j++) {
                hi_float* motr_ref_ptr = (hi_float*)motr_out_ref + i + j * stride_motr_out_ref;
                g_ref_res[j * SAMPLE_SVP_NPU_MOTR_WIDTH + i] = *motr_ref_ptr; /* Fill the RefRes with Motr xywh */
            }
        }
    }
    return ret;
}

hi_s32 sample_svp_npu_pre_process_qim_qry_hid()
{
    hi_u8 *motr_in_query = HI_NULL;
    hi_u8 *qim_in_query = HI_NULL;
    hi_u8 *qim_in_hidden = HI_NULL;
    hi_u8 *motr_out_hidden = HI_NULL;
    hi_u32 stride_motr_query;
    hi_u32 stride_qim_query;
    hi_u32 stride_qim_hidden;
    hi_u32 size;
    hi_u32 stride;
    hi_s32 ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK),
        SAMPLE_SVP_NPU_MOTR_QUERY_IN, &motr_in_query, &size, &stride_motr_query);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_QIM_TASK),
        SAMPLE_SVP_NPU_QIM_QUERY_IN, &qim_in_query, &size, &stride_qim_query);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_QIM_TASK),
        SAMPLE_SVP_NPU_QIM_HID_IN, &qim_in_hidden, &size, &stride_qim_hidden);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_svp_npu_get_dataset_by_name(SAMPLE_MOTR_TASK, "output2", 0, &motr_out_hidden, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    stride_qim_query /= (hi_u32)(sizeof(hi_float));
    stride_motr_query /= (hi_u32)(sizeof(hi_float));
    stride_qim_hidden /= (hi_u32)(sizeof(hi_float));
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_CHANNEL; i++) {
        for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_BOX; j++) {
            *((hi_float*)qim_in_query + j + i * stride_qim_query) = *((hi_float*)motr_in_query + j + i *
                stride_motr_query);
        }
    }
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_CHANNEL; i++) {
        for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_WIDTH; j++) {
            *((hi_float*)qim_in_hidden + j + i * stride_qim_hidden) = *((hi_float*)motr_out_hidden + j + i *
                stride_qim_hidden);
        }
    }
    return HI_SUCCESS;
}

hi_s32 sample_svp_npu_pre_process_qim_ref_conf()
{
    hi_u8 *motr_in_conf = HI_NULL;
    hi_u8 *qim_in_ref = HI_NULL;
    hi_u8 *qim_in_conf = HI_NULL;
    hi_u32 stride_qim_ref;
    hi_u32 size;
    hi_u32 stride;
    hi_s32 ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK),
        SAMPLE_SVP_NPU_MOTR_CONF_IN, &motr_in_conf, &size, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_QIM_TASK),
        SAMPLE_SVP_NPU_QIM_BOX_IN, &qim_in_ref, &size, &stride_qim_ref);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_QIM_TASK),
        SAMPLE_SVP_NPU_QIM_CONF_IN, &qim_in_conf, &size, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");
    stride_qim_ref /= sizeof(float);
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_CONF_SIZE; i++) {
        for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_WIDTH; j++) {
            *((hi_float*)qim_in_ref + j + i * stride_qim_ref) = g_ref_res[i * SAMPLE_SVP_NPU_MOTR_WIDTH + j];
        }
    }
    for (hi_u32 j = 0; j < SAMPLE_SVP_NPU_MOTR_BOX; j++) {
        *((hi_float*)qim_in_conf + j) = *((hi_float*)motr_in_conf + j);
    }
    return HI_SUCCESS;
}

hi_s32 sample_svp_npu_pre_process_qim(hi_s32 index)
{
    hi_s32 ret = sample_svp_npu_pre_process_qim_qry_hid();
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "pre process qim query hidden failed\n");

    ret = sample_svp_npu_pre_process_qim_ref_conf();
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "pre process qim ref conf failed\n");

    ret = sample_svp_npu_flush_qim_mmz(SAMPLE_QIM_TASK);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "flush qim memory failed\n");
    return ret;
}

static hi_s32 sample_svp_npu_get_motr_in_ref_qry(hi_u8 **motr_in_ref, hi_u32 *stride_motr_ref, hi_u8 **motr_in_query,
    hi_u32 *stride_motr_query)
{
    hi_u32 size;
    hi_s32 ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK),
        SAMPLE_SVP_NPU_MOTR_BOX_IN, motr_in_ref, &size, stride_motr_ref);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK),
        SAMPLE_SVP_NPU_MOTR_QUERY_IN, motr_in_query, &size, stride_motr_query);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");
    return ret;
}

static hi_s32 sample_svp_npu_post_process_qim(sample_svp_npu_motr_tracker *tracker, hi_u32 index)
{
    hi_u8 *motr_out_ref = HI_NULL;
    hi_u8 *motr_in_ref = HI_NULL;
    hi_u8 *motr_out_conf = HI_NULL;
    hi_u8 *motr_in_query = HI_NULL;
    hi_u8 *qim_out_query = HI_NULL;
    hi_u32 stride_motr_out_ref;
    hi_u32 stride_motr_ref;
    hi_u32 stride_motr_query;
    hi_u32 stride_qim_out_query;
    hi_u32 size;
    hi_u32 stride;

    hi_s32 ret = sample_svp_npu_get_motr_in_ref_qry(&motr_in_ref, &stride_motr_ref, &motr_in_query, &stride_motr_query);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_common_svp_npu_get_output_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_QIM_TASK),
        SAMPLE_SVP_NPU_QIM_QUERY_OUT, &qim_out_query, &size, &stride_qim_out_query);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_svp_npu_get_dataset_by_name(SAMPLE_MOTR_TASK, "output1", 0, &motr_out_ref, &stride_motr_out_ref);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    ret = sample_svp_npu_get_dataset_by_name(SAMPLE_MOTR_TASK, "output0", 0, &motr_out_conf, &stride);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "get buffer failed!\n");

    /* Prepare Tracker */
    hi_u32 swap_list_cnt = 0;
    hi_u32 swap_count = 0;
    g_res_confnum = 0;
    for (hi_u32 i = 0; i < SAMPLE_SVP_NPU_MOTR_WIDTH; i++) {
        hi_float* motr_confidence_ptr = (hi_float*)motr_out_conf + i;
        if (*motr_confidence_ptr > 0) {
            sample_svp_npu_qim_track_fill_query_pre_conf((hi_float*)qim_out_query, stride_qim_out_query, i, swap_count);
            sample_svp_npu_qim_track_fill_ref_pre_conf((hi_float*)motr_out_ref, stride_motr_out_ref, i,
                swap_count);
            g_res_conf[g_res_confnum++] = *motr_confidence_ptr;
            if (sample_svp_npu_motr_tracker_is_positionid(tracker, i))
                sample_svp_npu_motr_tracker_add_pos(tracker, i);
            sample_svp_npu_qim_track_add_swap(g_swap_list, &swap_list_cnt, &swap_count, i);
            sample_svp_npu_motr_tracker_reset_expire_count(tracker, i);
        } else if (!sample_svp_npu_motr_tracker_is_expired(tracker, i)) {
            if (i < SAMPLE_SVP_NPU_MOTR_BOX) {
                sample_svp_npu_qim_track_fill_query_pre_unexpired((hi_float*)motr_in_query, stride_motr_query, i,
                    swap_count);
            }
            sample_svp_npu_qim_track_fill_ref_pre_unexpired((hi_float*)motr_in_ref, stride_motr_ref, i, swap_count);
            g_res_conf[g_res_confnum++] = *motr_confidence_ptr;
            sample_svp_npu_motr_tracker_expired_count(tracker, i);
            sample_svp_npu_qim_track_add_swap(g_swap_list, &swap_list_cnt, &swap_count, i);
        }
    }
    sample_svp_npu_motr_tracker_update_pos(tracker, g_swap_list, swap_list_cnt);
    sample_svp_npu_motr_tracker_clean(tracker);
    return ret;
}

static hi_s32 sample_svp_npu_process_yolox(const hi_video_frame_info *yolox_frame, hi_float *crop, hi_u32 *crop_size)
{
    hi_s32 ret;
    hi_void *virt_addr = HI_NULL;
    hi_u32 size = (hi_u32)(yolox_frame->video_frame.height * yolox_frame->video_frame.stride[0] *
        SAMPLE_SVP_NPU_IMG_THREE_CHN / SAMPLE_SVP_NPU_DOUBLE);
    virt_addr = g_svp_npu_vb_virt_addr_yolox + (yolox_frame->video_frame.phys_addr[0] -
        g_svp_npu_vb_pool_info_yolox.pool_phy_addr);
    ret = sample_common_svp_npu_update_input_data_buffer_info(virt_addr, size,
        yolox_frame->video_frame.stride[0], SAMPLE_SVP_NPU_YOLOX_IMAGE_IN,
        sample_svp_npu_get_task_info(SAMPLE_YOLOX_TASK));
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "update data buffer failed!\n");

    ret = sample_common_svp_npu_model_execute(sample_svp_npu_get_task_info(SAMPLE_YOLOX_TASK));
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "model execute failed!\n");

    ret = sample_svp_npu_post_process_yolox(sample_svp_npu_get_task_info(SAMPLE_YOLOX_TASK), crop, crop_size);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "Preprocess Yolo failed!\n");
    return ret;
}

static hi_s32 sample_svp_npu_process_motr(const hi_video_frame_info *motr_frame, hi_float *crop, hi_u32 crop_size,
    sample_svp_npu_motr_tracker *tracker, uint32_t index)
{
    hi_s32 ret;
    /* Load Image into the buffer for input buffer 0 for task 1 (Motr) */
    hi_u32 motr_size = (hi_u32)(motr_frame->video_frame.height * motr_frame->video_frame.stride[0] *
        SAMPLE_SVP_NPU_IMG_THREE_CHN / SAMPLE_SVP_NPU_DOUBLE);
    hi_void *virt_addr = HI_NULL;
    virt_addr = g_svp_npu_vb_virt_addr_motr +
        (motr_frame->video_frame.phys_addr[0] - g_svp_npu_vb_pool_info_motr.pool_phy_addr);
    ret = sample_common_svp_npu_update_input_data_buffer_info(virt_addr, motr_size, motr_frame->video_frame.stride[0],
        SAMPLE_SVP_NPU_MOTR_IMAGE_IN, sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK));
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "update data buffer failed!\n");

    /* Prepare Motr input */
    sample_svp_npu_pre_process_motr(crop, crop_size, index);

    /* Motr Infer */
    ret = sample_common_svp_npu_model_execute(sample_svp_npu_get_task_info(SAMPLE_MOTR_TASK));
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "model execute failed!\n");

    /* Post process Motr */
    sample_svp_npu_post_process_motr(tracker, index);
    return ret;
}

static hi_s32 sample_svp_npu_process_qim(sample_svp_npu_motr_tracker *tracker, hi_s32 index)
{
    /* Prepare Qim Input */
    hi_s32 ret = sample_svp_npu_pre_process_qim(index);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "model prepare failed!\n");

    /* Qim Infer */
    ret = sample_common_svp_npu_model_execute(sample_svp_npu_get_task_info(SAMPLE_QIM_TASK));
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "model execute failed!\n");

    ret = sample_svp_npu_post_process_qim(tracker, index);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "model post process failed!\n");
    return ret;
}

static hi_s32 sample_svp_npu_motr_acl_frame_proc(hi_video_frame_info *yolox_frame, hi_video_frame_info *motr_frame,
    sample_svp_npu_motr_tracker *tracker, hi_video_frame_info *show_frame, hi_u32 index)
{
    hi_u32 crop_size = 0;
    /* Process YoloX */
    hi_s32 ret = sample_svp_npu_process_yolox(yolox_frame, g_crop, &crop_size);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "process yolox failed!\n");

    /* Process Motr */
    ret = sample_svp_npu_process_motr(motr_frame, g_crop, crop_size, tracker, index);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "process motr failed!\n");

    /* Process Qim */
    ret = sample_svp_npu_process_qim(tracker, index);
    sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR, "process qim failed!\n");

    /* Writeout Motr result into rect info by VGS */
    sample_init_box_info_for_vgs(sample_svp_npu_get_svp_rect_info(), tracker, show_frame->video_frame.width,
        show_frame->video_frame.height);

    /* Report Inferred Box Num from Motr */
    printf("[INFER] Motr Predict %u Boxes.\n", sample_svp_npu_get_svp_rect_info()->num);

    /* VGS Draw Box */
    ret = sample_common_svp_vgs_fill_rect(show_frame, sample_svp_npu_get_svp_rect_info(), SAMPLE_SVP_NPU_RECT_COLOR);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "vgs fill rect failed!\n");

    /* VGS Write id */
    ret = sample_common_svp_vgs_fill_tracker_id(show_frame, sample_svp_npu_get_svp_rect_info(),
        show_frame->video_frame.width, show_frame->video_frame.height, g_osd_buf);
    sample_svp_check_exps_return(ret != HI_SUCCESS, ret, SAMPLE_SVP_ERR_LEVEL_ERROR, "notify box fill rect failed!\n");
    return ret;
}

hi_s32 sample_svp_npu_main_motr_loop(sample_svp_npu_motr_tracker *tracker)
{
    hi_s32 ret;
    hi_u32 frame_index = 0;
    hi_u32 vpss_grp = 0;
    hi_video_frame_info yolox_frame, motr_frame, show_frame;
    hi_s32 vpss_chn[] = {HI_VPSS_CHN0, HI_VPSS_CHN1, HI_VPSS_CHN2}; /* Add VPSS CHANNEL 2 */
    hi_s32 *vpss_chn_ptr = vpss_chn;

    hi_u64 start_curr_time = sample_common_svp_npu_get_timestamp();
    while (*sample_svp_npu_get_thread_stop() == HI_FALSE && ret == HI_SUCCESS) {
        printf("[TIME] %llu ms,Idx: %u, ", sample_common_svp_npu_get_timestamp() - start_curr_time, ++frame_index);
        hi_s32 f_ret = sample_svp_npu_motr_get_tri_frames(&yolox_frame, &motr_frame, &show_frame, vpss_chn_ptr,
            vpss_grp);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR, "get frame failed!\n");

        /* Main Infer of Motr */
        ret = sample_svp_npu_motr_acl_frame_proc(&yolox_frame, &motr_frame, tracker, &show_frame, frame_index);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR, "frame proc fail!\n");

        ret = sample_common_svp_venc_vo_send_stream(&sample_svp_npu_get_media_cfg()->svp_switch, 0, 0, 0, &show_frame);
        sample_svp_check_exps_goto(ret != HI_SUCCESS, release, SAMPLE_SVP_ERR_LEVEL_ERROR, "vo_send_stream failed!\n");
release:
        ret = sample_svp_npu_release_tri_frames(&yolox_frame, &motr_frame, &show_frame, vpss_chn_ptr, f_ret);
        sample_svp_check_exps_return(ret != HI_SUCCESS, HI_FAILURE, SAMPLE_SVP_ERR_LEVEL_ERROR,
            "release frame failed\n");
    }
    return HI_SUCCESS;
}

hi_s32 sample_svp_npu_motr_set_params(hi_u32 model_idx_yolox, hi_u32 model_idx_motr)
{
    hi_s32 ret;
    sample_svp_npu_get_vdec_cfg()->display_frame_num = SAMPLE_SVP_NPU_MOTR_VPSS_CHN;
    sample_svp_npu_get_vdec_cfg()->sample_vdec_video.ref_frame_num = SAMPLE_SVP_NPU_MOTR_VPSS_CHN;
    sample_svp_npu_get_vdec_cfg()->frame_buf_cnt = SAMPLE_SVP_NPU_MOTR_VPSS_CHN + SAMPLE_SVP_NPU_MOTR_VPSS_CHN + 1;
    sample_svp_npu_get_vdec_param()->fps = SAMPLE_SVP_NPU_MOTR_DEFAULT_FPS;
    strcpy_s(sample_svp_npu_get_vdec_param()->c_file_name, SAMPLE_SVP_NPU_G_C_FILE_LEN, SAMPLE_SVP_NPU_VIDEO_PATH);

    /* get input resolution */
    sample_svp_npu_get_media_cfg()->pic_size[SAMPLE_SVP_NPU_SHOW_FRAME].width = SAMPLE_SVP_NPU_MOTR_VO_WIDTH;
    sample_svp_npu_get_media_cfg()->pic_size[SAMPLE_SVP_NPU_SHOW_FRAME].height = SAMPLE_SVP_NPU_MOTR_VO_HEIGHT;
    sample_svp_npu_get_media_cfg()->pic_type[SAMPLE_SVP_NPU_SHOW_FRAME] = PIC_1080P;
    sample_svp_npu_get_media_cfg()->chn_num = SAMPLE_SVP_NPU_MOTR_VPSS_CHN;

    /* Set pic size */
    ret = sample_common_svp_npu_get_input_resolution(model_idx_yolox, 0,
        &(sample_svp_npu_get_media_cfg()->pic_size[SAMPLE_SVP_NPU_YOLOX_FRAME]));
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get pic size failed!\n");

    ret = sample_common_svp_npu_get_input_resolution(model_idx_motr, 0,
        &(sample_svp_npu_get_media_cfg()->pic_size[SAMPLE_SVP_NPU_MOTR_FRAME]));
    sample_svp_check_exps_goto(ret != HI_SUCCESS, process_end, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get pic size failed!\n");
process_end:
    return ret;
}

hi_void *sample_svp_npu_motr_acl_vdec_to_vo()
{
    (hi_void)prctl(PR_SET_NAME, "svp_npu_vdec_to_vo", 0, 0, 0);
    hi_s32 ret = svp_acl_rt_set_device(*sample_svp_npu_get_device_id());
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_0, SAMPLE_SVP_ERR_LEVEL_ERROR, "open device failed!\n");

    /* VB init for two VPSS channel */
    ret = sample_svp_npu_motr_acl_vb_map_dual_frames(SAMPLE_SVP_NPU_YOLOX_FRAME);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_1_1, SAMPLE_SVP_ERR_LEVEL_ERROR, "map yolox vb failed!\n");
    ret = sample_svp_npu_motr_acl_vb_map_dual_frames(SAMPLE_SVP_NPU_MOTR_FRAME);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_1_2, SAMPLE_SVP_ERR_LEVEL_ERROR, "map motr vb failed!\n");

    hi_u32 size_yolo = 0, stride_yolo = 0, size_motr = 0, stride_motr = 0;
    hi_u8 *data_yolo = HI_NULL, *data_motr = HI_NULL;

    /* get input data buffer for update */
    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_SVP_NPU_YOLOX_IDX),
        0, &data_yolo, &size_yolo, &stride_yolo);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_1_2, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get_data_buffer_info fail!\n");
    ret = sample_common_svp_npu_get_input_data_buffer_info(sample_svp_npu_get_task_info(SAMPLE_SVP_NPU_MOTR_IDX), 0,
        &data_motr, &size_motr, &stride_motr);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, fail_1_2, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "get_data_buffer_info fail!\n");

    /* Init Global Data */
    sample_svp_npu_motr_tracker tracker = {0};

    ret = sample_svp_npu_motr_preprocess_motr_global(&tracker);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, destroy_global, SAMPLE_SVP_ERR_LEVEL_ERROR, "preprocess fail!\n");

    /* Motr main process */
    ret = sample_svp_npu_main_motr_loop(&tracker);
    sample_svp_check_exps_goto(ret != HI_SUCCESS, destroy_global, SAMPLE_SVP_ERR_LEVEL_ERROR,
        "sample_svp_npu_motr_process failed");

    /* to unify the dataset mmz release; update data buffer */
    ret = sample_common_svp_npu_update_input_data_buffer_info(data_yolo, size_yolo, stride_yolo, 0,
        sample_svp_npu_get_task_info(SAMPLE_SVP_NPU_YOLOX_IDX));
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "update buffer failed!\n");

    ret = sample_common_svp_npu_update_input_data_buffer_info(data_motr, size_motr, stride_motr, 0,
        sample_svp_npu_get_task_info(SAMPLE_SVP_NPU_MOTR_IDX));
    sample_svp_check_exps_trace(ret != HI_SUCCESS, SAMPLE_SVP_ERR_LEVEL_ERROR, "update buffer failed!\n");

/* Destory Global Data */
destroy_global:
    (hi_void)sample_svp_npu_destroy_global(&tracker);

fail_1_2:
    (hi_void)hi_mpi_sys_munmap(g_svp_npu_vb_virt_addr_yolox, g_svp_npu_vb_pool_info_yolox.pool_size);
fail_1_1:
    (hi_void)hi_mpi_sys_munmap(g_svp_npu_vb_virt_addr_motr, g_svp_npu_vb_pool_info_motr.pool_size);
fail_0:
    (hi_void)svp_acl_rt_reset_device(*sample_svp_npu_get_device_id());
    return HI_NULL;
}
