/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef SAMPLE_SVP_NPU_PROCESS_MOTR_H
#define SAMPLE_SVP_NPU_PROCESS_MOTR_H

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include "hi_type.h"
#include "sample_common_svp_npu.h"

#define SAMPLE_SVP_NPU_MOTR_MODEL_PATH "./data/model/motr_yuv.om" // MOTR_PATH
#define SAMPLE_SVP_NPU_YOLOX_MODEL_PATH "./data/model/yolox_yuv.om" // YOLOX_PATH
#define SAMPLE_SVP_NPU_QIM_MODEL_PATH "./data/model/qim_yuv.om" // QIM_PATH
#define SAMPLE_SVP_NPU_YOLOX_IDX 0
#define SAMPLE_SVP_NPU_MOTR_IDX 1
#define SAMPLE_SVP_NPU_QIM_IDX 2
#define SAMPLE_SVP_NPU_YOLOX_FRAME 1
#define SAMPLE_SVP_NPU_MOTR_FRAME 2
#define SAMPLE_SVP_NPU_SHOW_FRAME 0
#define SAMPLE_MOTR_TASK_NUM 3
#define MOTR_VPSS_CHN_NUM 3
#define SAMPLE_YOLOX_TASK 0
#define SAMPLE_MOTR_TASK 1
#define SAMPLE_QIM_TASK 2

/* Motr includes three models and three frames, others one model and two frames */
hi_void *sample_svp_npu_motr_acl_vdec_to_vo();

/* Motr should change frames num / image attr of cfg */
hi_s32 sample_svp_npu_motr_set_params(hi_u32 model_idx_yolox, hi_u32 model_idx_motr);

#endif