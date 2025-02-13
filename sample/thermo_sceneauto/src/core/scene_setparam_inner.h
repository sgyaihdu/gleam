/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef SCENE_SETPARAM_INNER_H
#define SCENE_SETPARAM_INNER_H

#include "ot_scene_setparam.h"


#ifdef __cplusplus
extern "C" {
#endif


#define scene_div_0to1(a) (((a) == 0) ? 1 : (a))

ot_scene_pipe_param *get_pipe_params(hi_void);
hi_bool *get_isp_pause(hi_void);

#define check_scene_ret(ret) do {                                                    \
        if ((ret) != HI_SUCCESS) {                                                   \
            printf("Failed at %s: LINE: %d with %#x!", __FUNCTION__, __LINE__, ret); \
        }                                                                            \
    } while (0)

#define check_scene_return_if_pause() do {   \
        if (*(get_isp_pause()) == HI_TRUE) { \
            return HI_SUCCESS;               \
        }                                    \
    } while (0)

hi_u32 get_level_ltoh(hi_u32 value, hi_u32 level, hi_u32 count, const hi_u32 *thresh);
hi_u32 scene_get_level_ltoh_u32(hi_u32 value, hi_u32 count, const hi_u32 *thresh);
hi_u32 scene_interpulate(hi_u64 middle, hi_u64 left, hi_u64 left_value, hi_u64 right, hi_u64 right_value);
hi_u32 scene_time_filter(hi_u32 param0, hi_u32 param1, hi_u32 time_cnt, hi_u32 index);
hi_s32 scene_set_3dnr(hi_vi_pipe vi_pipe, const ot_scene_3dnr *_3dnr, hi_u8 index, hi_3dnr_pos_type pos3Dnr);
hi_void scene_set_3dnr_nrx_nry(_3dnr_nrx_param param);
hi_void scene_set_3dnr_nrx_iey(_3dnr_nrx_param param);
hi_void scene_set_3dnr_nrx_sfy(_3dnr_nrx_param param);
hi_void scene_set_3dnr_nrx_tfy(_3dnr_nrx_param param, hi_3dnr_pos_type pos3Dnr);
hi_void scene_set_3dnr_nrx_mdy(_3dnr_nrx_param param);
hi_void scene_set_3dnr_nrx_nrc0(_3dnr_nrx_param param);
hi_void scene_set_3dnr_nrx_nrc1(_3dnr_nrx_param param);
hi_void scene_set_3dnr_nrx_adv_sfy(_3dnr_nrx_param param);
hi_void scene_set_3dnr_nrx_adv_iey(_3dnr_nrx_param param);

#ifdef __cplusplus
}
#endif

#endif
