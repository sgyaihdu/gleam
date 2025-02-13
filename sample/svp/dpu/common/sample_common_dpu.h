/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#ifndef SAMPLE_COMMON_DPU_H
#define SAMPLE_COMMON_DPU_H
#include <stdio.h>
#include "hi_mpi_sys.h"

#include "hi_common_vb.h"
#include "hi_mpi_dpu_rect.h"
#include "hi_mpi_dpu_match.h"
#include "hi_common_dpu_rect.h"
#include "hi_common_dpu_match.h"

#ifdef __cplusplus
#if __cplusplus
extern "C"{
#endif
#endif /* __cplusplus */

#define SAMPLE_SVP_DPU_ALIGN_16              16
#define SAMPLE_SVP_DPU_FRAME_NUM             1
#define SAMPLE_SVP_DPU_DISPARITY_NUM         112

#define SAMPLE_SVP_DPU_IN_WIDTH              640
#define SAMPLE_SVP_DPU_IN_HEIGHT             480
#define SAMPLE_SVP_DPU_DST_WIDTH             640
#define SAMPLE_SVP_DPU_DST_HEIGHT            480
#define SAMPLE_SVP_DPU_CHN_NUM               2
#define SAMPLE_SVP_DPU_TIME_OUT              2000
#define SAMPLE_SVP_DPU_BLOCK                 (-1)
#define SAMPLE_SVP_DPU_NO_RATE_CTRL          (-1)
#define SAMPLE_SVP_DPU_MAX_POOL_CNT          128
#define SAMPLE_SVP_DPU_BLK_CNT_10            10
#define SAMPLE_SVP_DPU_BLK_CNT_8             8
#define SAMPLE_SVP_DPU_VI_CHN_CNT            2
#define SAMPLE_SVP_DPU_VI_PIPE_CNT           2
#define SAMPLE_SVP_DPU_VPSS_GRP_NUM          2
#define SAMPLE_SVP_DPU_PER_CHN_BLK_CNT       4
#define SAMPLE_SVP_DPU_VPSS_MAX_WIDTH        3840
#define SAMPLE_SVP_DPU_VPSS_MAX_HEIGHT       2160
#define SAMPLE_SVP_DPU_BG_COLOR              0xff
#define SAMPLE_SVP_DPU_COMMON_POOL_2         2
#define SAMPLE_SVP_DPU_COMMON_POOL_3         3
#define SAMPLE_SVP_DPU_OUTPUT_NUM            3
#define SAMPLE_SVP_DPU_OUTPUT_DISP_IDX       0
#define SAMPLE_SVP_DPU_OUTPUT_CONF_IDX       1
#define SAMPLE_SVP_DPU_OUTPUT_DEPTH_IDX      2

#define SAMPLE_SVP_DPU_INVALID_DISP          (-1)
#define SAMPLE_SVP_DPU_MATCH_A0_UNIQ_RATIO   24
#define SAMPLE_SVP_DPU_MATCH_A1_UNIQ_RATIO   28
#define SAMPLE_SVP_DPU_MATCH_A2_UNIQ_RATIO   31
#define SAMPLE_SVP_DPU_MATCH_D0_AGGREGATE_COEFF 0
#define SAMPLE_SVP_DPU_MATCH_D1_AGGREGATE_COEFF 4
#define SAMPLE_SVP_DPU_MATCH_D2_AGGREGATE_COEFF 6
#define SAMPLE_SVP_DPU_MATCH_D3_AGGREGATE_COEFF 8
#define SAMPLE_SVP_DPU_MATCH_FIRST_PENALTY_COEF 4
#define SAMPLE_SVP_DPU_MATCH_SECOND_PENALTY_COEF 32
#define SAMPLE_SVP_DPU_MATCH_FG_INIT_COST_ZERO_THR 8
#define SAMPLE_SVP_DPU_MATCH_LOCAL_FAETURE         4
#define SAMPLE_SVP_DPU_MATCH_CONF_FILTER_DISP_THR  10
#define SAMPLE_SVP_DPU_MATCH_FB_NORM_BITS_8        8
#define SAMPLE_SVP_DPU_MATCH_FB_NORM_BITS_10       10
#define SAMPLE_SVP_DPU_MATCH_FB_NORM_BITS_12       12
#define SAMPLE_SVP_DPU_MATCH_FB                    48720
#define SAMPLE_SVP_DPU_MATCH_Z0                    400
#define SAMPLE_SVP_DPU_MATCH_MIN_DEPTH_THR         0
#define SAMPLE_SVP_DPU_MATCH_MAX_DEPTH_THR         30000
#define SAMPLE_SVP_DPU_MATCH_INVALID_DEPTH         0
#define SAMPLE_SVP_DPU_MATCH_DISP_COEF             (-1)


#define sample_svp_dpu_convert_addr_to_ptr(type, addr) ((type *)(hi_uintptr_t)(addr))

#define sample_svp_dpu_mmz_free(phys, virt) \
do { \
    if (((phys) != 0) && ((virt) != 0)) { \
        hi_mpi_sys_mmz_free((hi_phys_addr_t)(phys), (hi_void*)(hi_uintptr_t)(virt)); \
        (phys) = 0; \
        (virt) = 0; \
    } \
} while (0)

typedef enum {
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D0_A11 = 0x0,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D1_A10 = 0x1,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D2_A9 = 0x2,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D3_A8 = 0x3,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D4_A7 = 0x4,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D5_A6 = 0x5,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D6_A5 = 0x6,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D7_A4 = 0x7,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D8_A3 = 0x8,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D9_A2 = 0x9,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D10_A1 = 0xa,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D11_A0 = 0xb,
    SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_BUTT
} sample_svp_dpu_match_density_accuracy_mode;

typedef struct {
    hi_dpu_rect_grp dpu_rect_grp;
    hi_dpu_rect_grp_attr dpu_rect_grp_attr;
    hi_dpu_rect_lut_id lut_id[HI_DPU_RECT_MAX_PIPE_NUM];
    hi_dpu_rect_mem_info dpu_rect_mem_info[HI_DPU_RECT_MAX_PIPE_NUM];
    hi_vb_pool pipe_vb_pool[HI_DPU_RECT_MAX_PIPE_NUM];
    FILE *pipe_file[HI_DPU_RECT_MAX_PIPE_NUM];
    hi_vb_blk vb_blk[HI_DPU_RECT_MAX_PIPE_NUM];

    hi_dpu_match_grp dpu_match_grp;
    hi_dpu_match_grp_attr dpu_match_grp_attr;
    hi_dpu_match_src_frame_info dpu_match_src_frame;
    hi_dpu_match_dst_frame_info dpu_match_dst_frame;
    FILE *dst_file[SAMPLE_SVP_DPU_OUTPUT_NUM];
} hi_sample_svp_dpu_cfg;

#define sample_svp_dpu_pause()                                                      \
do {                                                                    \
    printf("---------------press Enter key to exit!---------------\n"); \
    (void)getchar();                                                    \
} while (0)

#define sample_svp_dpu_printf(level_str, msg, ...) \
do { \
    fprintf(stderr, "[level]:%s,[func]:%s [line]:%d [info]:"msg, \
        level_str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while (0)
#define sample_svp_dpu_printf_red(level_str, msg, ...) \
do { \
    fprintf(stderr, "\033[0;31m [level]:%s,[func]:%s [line]:%d [info]:"msg"\033[0;39m\n", \
        level_str, __FUNCTION__, __LINE__, ##__VA_ARGS__); \
} while (0)

/* error conditions */
#define sample_svp_dpu_trace_err(msg, ...)     sample_svp_dpu_printf_red("Error", msg, ##__VA_ARGS__)
#define sample_svp_dpu_trace_info(msg, ...)    sample_svp_dpu_printf("Info", msg, ##__VA_ARGS__)
/* exps is true, goto */
#define sample_svp_dpu_check_exps_goto(exps, label, msg, ...)                  \
do {                                                                              \
    if ((exps)) {                                                                 \
        sample_svp_dpu_trace_err(msg, ## __VA_ARGS__);                                \
        goto label;                                                               \
    }                                                                             \
} while (0)

/* exps is true, return ret */
#define sample_svp_dpu_check_exps_return(exps, ret, msg, ...)                 \
do {                                                                             \
    if ((exps)) {                                                                \
        sample_svp_dpu_trace_err(msg, ##__VA_ARGS__);                                \
        return (ret);                                                            \
    }                                                                            \
} while (0)                                                                      \

hi_u32 sample_common_svp_dpu_calc_stride(hi_u32 width, hi_u8 align);

/* function : get frame from file */
hi_s32 sample_common_svp_dpu_rect_get_frame_from_file(FILE *fp, const hi_size *pic_size,
    hi_u32 stride, hi_video_frame_info *frame_info, hi_vb_blk vb_blk);

hi_s32 sample_common_svp_dpu_rect_load_lut(const hi_char *file_name,
    hi_dpu_rect_mem_info *mem_info, hi_dpu_rect_lut_id *lut_id);

hi_void sample_common_svp_dpu_rect_unload_lut(hi_dpu_rect_mem_info *mem_info,
    hi_dpu_rect_lut_id lut_id);

/* function : start dpu rect grp. */
hi_s32 sample_common_svp_dpu_rect_start(hi_dpu_rect_grp dpu_rect_grp,
    hi_dpu_rect_grp_attr *grp_attr, hi_dpu_rect_chn_attr *chn_attr, hi_u32 chn_num);

/* function : stop dpu rect grp */
hi_s32 sample_common_svp_dpu_rect_stop(hi_dpu_rect_grp dpu_rect_grp, hi_dpu_rect_mode rect_mode);

/* function : write frame to file. */
hi_s32 sample_common_svp_dpu_write_frame_to_file(FILE *fp, hi_u32 ele_size,
    hi_video_frame_info *frame_info);

/* function : start dpu match grp. */
hi_s32 sample_common_svp_dpu_match_start(hi_dpu_match_grp dpu_match_grp,
    hi_dpu_match_grp_attr *grp_attr, hi_dpu_match_chn_attr *chn_attr);
/* function : stop dpu match grp */
hi_s32 sample_common_svp_dpu_match_stop(hi_dpu_match_grp dpu_match_grp);

hi_s32 sample_common_svp_dpu_match_create_mem_info(hi_dpu_match_mem_info *mem_info,
    hi_char *mmb, hi_char *zone, hi_u32 size);

hi_s32 sample_common_svp_dpu_rect_bind_match(hi_dpu_rect_grp dpu_rect_grp,
    hi_dpu_match_grp dpu_match_grp);

hi_s32 sample_common_svp_dpu_rect_unbind_match(hi_dpu_rect_grp dpu_rect_grp,
    hi_dpu_match_grp dpu_match_grp);

hi_s32 sample_common_svp_dpu_vpss_bind_rect(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn,
    hi_dpu_rect_grp dpu_rect_grp, hi_dpu_rect_pipe dpu_rect_pipe);

hi_s32 sample_common_svp_dpu_vpss_unbind_rect(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn,
    hi_dpu_rect_grp dpu_rect_grp, hi_dpu_rect_pipe dpu_rect_pipe);
hi_s32 sample_common_svp_dpu_get_fg_aggregate_coef_and_unique_ratio(
    sample_svp_dpu_match_density_accuracy_mode density_accuracy_mode, hi_u8 *aggregate_coef, hi_u8 *unique_ratio);
#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
#endif /* SAMPLE_COMMON_DPU_H */
