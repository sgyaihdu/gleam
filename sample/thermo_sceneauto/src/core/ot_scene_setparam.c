/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include "ot_scene_setparam.h"
#include "scene_setparam_inner.h"
#include <unistd.h>
#include <string.h>
#include "securec.h"
#include "hi_mpi_awb.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_isp.h"
#include "hi_mpi_venc.h"
#include "hi_common_rc.h"
#include "sample_comm.h"
#include "hi_mpi_mcf.h"
#include "hi_mpi_aidrc.h"
#include "hi_mpi_aibnr.h"

#include "ot_scenecomm.h"

#ifdef __cplusplus
extern "C" {
#endif

hi_char g_dir_name[DIR_NAME_BUFF];

hi_void set_dir_name(const hi_char *dir_name)
{
    snprintf_truncated_s(g_dir_name, DIR_NAME_BUFF, "%s", dir_name);
}

hi_s32 ot_scene_set_dynamic_3dnr(hi_vi_pipe vi_pipe, hi_u32 iso, hi_u8 index, hi_3dnr_pos_type pos3dnr)
{
    if (get_pipe_params()[index].module_state.dynamic_3dnr != HI_TRUE) {
        return HI_SUCCESS;
    }

    hi_u32 iso_level = 0;
    ot_scene_3dnr nrx_attr;
    const ot_scene_static_3dnr *nrx_param = &(get_pipe_params()[index].static_threednr);
    hi_u32 count = nrx_param->threed_nr_count;
    hi_u32 *p_thresh = (hi_u32 *)nrx_param->threed_nr_iso;
    iso_level = get_level_ltoh(iso, iso_level, count, p_thresh);
    if (iso_level == 0) {
        nrx_attr = nrx_param->threednr_value[0];
    } else {
        ot_scenecomm_expr_true_return(iso_level >= HI_SCENE_3DNR_MAX_COUNT, HI_FAILURE);
        hi_u32 mid = iso;
        hi_u32 left = p_thresh[iso_level - 1];
        hi_u32 right = p_thresh[iso_level];

        nrx_attr = nrx_param->threednr_value[iso_level];

        _3dnr_nrx_param param = { &nrx_attr, nrx_param, iso_level, mid, left, right };

        scene_set_3dnr_nrx_adv_iey(param);

        scene_set_3dnr_nrx_adv_sfy(param);

        scene_set_3dnr_nrx_nry(param);

        scene_set_3dnr_nrx_iey(param);

        scene_set_3dnr_nrx_sfy(param);

        scene_set_3dnr_nrx_tfy(param, pos3dnr);

        scene_set_3dnr_nrx_mdy(param);

        scene_set_3dnr_nrx_nrc0(param);

        scene_set_3dnr_nrx_nrc1(param);
    }

    hi_s32 ret = scene_set_3dnr(vi_pipe, &nrx_attr, index, pos3dnr);
    check_scene_ret(ret);
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_pause(hi_bool pause)
{
    *(get_isp_pause()) = pause;
    return HI_SUCCESS;
}

hi_s32 ot_scene_set_pipe_param(const ot_scene_pipe_param *pscene_pipe_param, hi_u32 num)
{
    if (pscene_pipe_param == HI_NULL) {
        printf("null pointer");
        return HI_FAILURE;
    }

    errno_t ret = memcpy_s(get_pipe_params(), sizeof(ot_scene_pipe_param) * HI_SCENE_PIPETYPE_NUM, pscene_pipe_param,
        sizeof(ot_scene_pipe_param) * num);
    if (ret != EOK) {
        printf("copy scene pipe params fail. num = %u\n", num);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

#ifdef __cplusplus
}
#endif
