/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef __SAMPLE_MCF_CALIBRATION_H__
#define __SAMPLE_MCF_CALIBRATION_H__

#include "sample_comm_mcf.h"
#include "hi_mpi_mcf_calibration.h"

hi_s32 sample_mcf_calibration(hi_void);
hi_s32 sample_mcf_diff_img_size_calibration(hi_void);
hi_s32 sample_mcf_calibrate_online(hi_mcf_grp mcf_grp, hi_mcf_grp_attr *mcf_grp_attr, hi_mcf_crop_info *grp_crop,
    hi_fov_attr *fov_correction_attr);

#endif
