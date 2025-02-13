/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef HI_RATE_AUTO
#define HI_RATE_AUTO

#include <stdio.h>
#include "hi_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#ifndef rate_auto_prt
#define rate_auto_prt(fmt...)   \
    do { \
        printf("[%s]-%d: ", __FUNCTION__, __LINE__); \
        printf(fmt); \
    } while (0)
#endif

#define SVC_RECT_TYPE_NUM 5
#define FG_TYPE_0     0
#define FG_TYPE_1     1
#define FG_TYPE_2     2
#define FG_TYPE_3     3
#define FG_TYPE_4     4

typedef struct {
    hi_u8 avbr_rate_control;
    hi_u8 svc_fg_qpmap_val_p[SVC_RECT_TYPE_NUM];
    hi_u8 svc_fg_qpmap_val_i[SVC_RECT_TYPE_NUM];
    hi_u8 svc_fg_skipmap_val[SVC_RECT_TYPE_NUM];
    hi_u8 svc_bg_qpmap_val_p;
    hi_u8 svc_bg_qpmap_val_i;
    hi_u8 svc_bg_skipmap_val;
    hi_u8 svc_roi_qpmap_val_p;
    hi_u8 svc_roi_qpmap_val_i;
    hi_u8 svc_roi_skipmap_val;
    hi_u32 max_bg_qp;
    hi_u32 min_fg_qp;
    hi_u32 max_fg_qp;
    hi_u8 fg_protect_adjust;
} rate_auto_param;

hi_s32 hi_rate_auto_init(const rate_auto_param *init_param);
hi_s32 hi_rate_auto_deinit(hi_void);
hi_s32 hi_rate_auto_load_param(hi_char *module_name, rate_auto_param *rate_auto_para);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif /* End of #ifndef HI_RATE_AUTO */
