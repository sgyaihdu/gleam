/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef SAMPLE_SVP_NPU_RESOURCES_H
#define SAMPLE_SVP_NPU_RESOURCES_H

#include <pthread.h>
#include <sys/prctl.h>
#include <unistd.h>
#include "sample_svp_npu_process.h"
#include "svp_acl_rt.h"
#include "svp_acl.h"
#include "svp_acl_ext.h"
#include "sample_common_svp.h"
#include "sample_common_svp_npu.h"
#include "sample_common_svp_npu_model.h"

/* shared variates between files */
hi_s32 *sample_svp_npu_get_device_id(hi_void);

hi_sample_svp_rect_info *sample_svp_npu_get_svp_rect_info(hi_void);

hi_bool *sample_svp_npu_get_thread_stop(hi_void);

sample_svp_npu_task_info *sample_svp_npu_get_task_info(int task_idx);

hi_sample_svp_media_cfg *sample_svp_npu_get_media_cfg(hi_void);

sample_vdec_attr *sample_svp_npu_get_vdec_cfg(hi_void);

vdec_thread_param *sample_svp_npu_get_vdec_param(hi_void);

#endif