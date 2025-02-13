
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hi_common.h"
#include "hi_mpi_vi.h"
#include "hi_mpi_isp.h"
#include "hi_mpi_awb.h"
#include "hi_mpi_ae.h"

#include "cJSON.h"

//字符串赋值函数
#define SET_VALUE(type, target, input_str)  \
    {                                       \
        type temp_val = (type)atoi(input_str); \
        *(target) = temp_val;               \
    }

#define CHECK_VALUE(type, target, input_str, key)  \
    {                                       \
        type temp_val = (type)atoi(input_str); \
        if (temp_val != *(target)) { \
            printf("Value mismatch at %s\n", key); \
        } \
    }


//设置AE曝光属性
hi_s32 set_isp_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置wdr模式、智能模式、人脸快速收敛模式下的AE曝光属性
hi_s32 set_isp_wdr_smart_fastface_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_wdr_smart_fastface_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置AI光圈属性
hi_s32 set_isp_ai_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_ai_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置AWB白平衡属性
hi_s32 set_isp_wb_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_wb_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置AWB白平衡扩展属性
hi_s32 set_isp_awb_attr_ex_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_awb_attr_ex_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置色彩相关矩阵(CCM)属性
hi_s32 set_isp_ccm_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_ccm_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置黑电平属性
hi_s32 set_isp_black_level_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_black_level_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置图像锐化属性
hi_s32 set_isp_sharpen_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_sharpen_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置伽马变换属性
hi_s32 set_isp_gamma_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_gamma_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置wdr动态范围压缩属性
hi_s32 set_isp_drc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_drc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置Mesh Shading属性
hi_s32 set_isp_shading_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_shading_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置坏点矫正属性
hi_s32 set_isp_dp_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_dp_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置crosstalk和去雾属性
hi_s32 set_isp_cr_dehaze_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_cr_dehaze_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置去伪彩和去马赛克属性
hi_s32 set_isp_afc_demosaic_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_afc_demosaic_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置bayersharpen和ca属性
hi_s32 set_isp_bayershp_ca_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_bayershp_ca_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置去色差和wdr融合图像压缩属性
hi_s32 set_isp_cac_fswdr_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_cac_fswdr_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置局域自动对比度增强属性
hi_s32 set_isp_ldci_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_ldci_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置色调、局部裁剪、色彩空间转换、CLUT属性
hi_s32 set_isp_colortone_rc_csc_clut_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_colortone_rc_csc_clut_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);

//设置dng色彩属性、自动色彩校正、数据解压缩属性
hi_s32 set_isp_dngcolor_acs_expander_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_dngcolor_acs_expander_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);


//设置局部BLC属性
hi_s32 set_isp_lblc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);
hi_s32 check_isp_lblc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe);








