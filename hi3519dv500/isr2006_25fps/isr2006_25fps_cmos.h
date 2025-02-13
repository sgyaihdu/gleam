/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#ifndef ISR2006_CMOS_H
#define ISR2006_CMOS_H

#include "ot_common.h"
#include "ot_common_isp.h"
#include "ot_common_video.h"
#include "ot_sns_ctrl.h"
#include "ot_type.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define ISR2006_I2C_ADDR    ((0xCA)>>1)
#define ISR2006_ADDR_BYTE   2
#define ISR2006_DATA_BYTE   1
#define isr2006_sensor_get_ctx(pipe, ctx)   ((ctx) = isr2006_get_ctx(pipe))

#define ISR2006_FULL_LINES_MAX 0xFFFFF

#define ISR2006_INCREASE_LINES  1  /* make real fps less than stand fps because NVR require */
#define ISR2006_VMAX_4K2K_LINEAR    ( 2381 + ISR2006_INCREASE_LINES )  /*高度*/

typedef enum {
    ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE = 0,
    ISR2006_MODE_BUTT
} isr2006_25fps_res_mode;

typedef struct {
    td_u32      ver_lines;
    td_u32      max_ver_lines;
    td_float    max_fps;
    td_float    min_fps;
    td_u32      width;
    td_u32      height;
    td_u8       sns_mode;
    ot_wdr_mode wdr_mode;
    const char *mode_name;
} isr2006_video_mode_tbl;

ot_isp_sns_state *isr2006_get_ctx(ot_vi_pipe vi_pipe);
ot_isp_sns_commbus *isr2006_get_bus_info(ot_vi_pipe vi_pipe);

td_void isr2006_init(ot_vi_pipe vi_pipe);
td_void isr2006_exit(ot_vi_pipe vi_pipe);
td_void isr2006_standby(ot_vi_pipe vi_pipe);
td_void isr2006_restart(ot_vi_pipe vi_pipe);
td_s32  isr2006_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data);
td_s32  isr2006_read_register(ot_vi_pipe vi_pipe, td_u32 addr);
td_void isr2006_blc_clamp(ot_vi_pipe vi_pipe, ot_isp_sns_blc_clamp blc_clamp);
td_void isr2006_set_blc_clamp_value(ot_vi_pipe vi_pipe, td_bool clamp_en);


#define NON_CONTINUOUS_CLOCK 0


#define ISR2006_WIDTH (1936)
#define ISR2006_HEIGHT (1280)

enum ramp_vftctrl_mode {
    RAMP_HCG_14_AND_12BIT = 0,
    RAMP_LCG_14BIT,
    RAMP_LCG_12BIT,
    RAMP_TEST,
    RAMP_CG_BUTT
};

#define RAMP_VFTCTRL_NUM 12



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */
#endif /* ISR2006_CMOS_H */

