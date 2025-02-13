/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include "ot_mpi_isp.h"
#include "ot_mpi_ae.h"
#include "ot_mpi_awb.h"
#include "securec.h"

#include "isr2006_cmos_ex.h"
#include "isr2006_cmos.h"

#include <hi_mpi_awb.h>
#include <hi_common_isp.h>

#define ISR2006_ID 2006

#define higher_4bits(x) (((x) & 0xf0000) >> 16)
#define high_8bits(x) (((x) & 0xff00) >> 8)
#define low_8bits(x) ((x) & 0x00ff)

#define higher_8bits(x) (((x) & 0xff0000) >> 16)

#define high_2bits(x) (((x) & 0x0300) >> 8)

#ifndef max
#define max(a, b) (((a) < (b)) ? (b) : (a))
#endif

#ifndef min
#define min(a, b) (((a) > (b)) ? (b) : (a))
#endif

#define ISR2006_TEST_PIC 0

/****************************************************************************
 * global variables                                                            *
 ****************************************************************************/
#define isr2006_sensor_set_ctx(pipe, ctx) ((g_isr2006_sns_state[pipe]) = (ctx))
#define isr2006_sensor_reset_ctx(pipe) (g_isr2006_sns_state[pipe] = TD_NULL)

static ot_isp_fswdr_mode g_fswdr_mode[OT_ISP_MAX_PIPE_NUM] = {
    [0 ... OT_ISP_MAX_PIPE_NUM - 1] = OT_ISP_FSWDR_NORMAL_MODE};
static td_u8 g_ae_stat_pos[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u32 g_max_time_get_cnt[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u32 g_init_exposure[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u32 g_lines_per500ms[OT_ISP_MAX_PIPE_NUM] = {0};

static td_u16 g_init_wb_gain[OT_ISP_MAX_PIPE_NUM][OT_ISP_RGB_CHN_NUM] = {{0}};
static td_u16 g_sample_r_gain[OT_ISP_MAX_PIPE_NUM] = {0};
static td_u16 g_sample_b_gain[OT_ISP_MAX_PIPE_NUM] = {0};
static td_bool g_quick_start_en[OT_ISP_MAX_PIPE_NUM] = {TD_FALSE};

static td_bool g_ae_route_ex_valid[OT_ISP_MAX_PIPE_NUM] = {0};
static ot_isp_ae_route g_init_ae_route[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route_ex g_init_ae_route_ex[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route g_init_ae_route_sf[OT_ISP_MAX_PIPE_NUM] = {{0}};
static ot_isp_ae_route_ex g_init_ae_route_sf_ex[OT_ISP_MAX_PIPE_NUM] = {{0}};

static hi_isp_wb_attr g_isp_wb_attr;


static td_u32 g_again = 0, g_dgain = 0;

typedef struct
{
    td_u8 hcg;
    td_u32 brl;
    td_u32 rhs1_max;
    td_u32 rhs2_max;
} isr2006_state;

isr2006_state g_isr2006_state[OT_ISP_MAX_PIPE_NUM] = {{0}};
ot_isp_sns_commbus g_isr2006_bus_info[OT_ISP_MAX_PIPE_NUM] = {
    [0] = {.i2c_dev = 0},
    [1 ... OT_ISP_MAX_PIPE_NUM - 1] = {.i2c_dev = -1}};

ot_isp_sns_state *g_isr2006_sns_state[OT_ISP_MAX_PIPE_NUM] = {TD_NULL};

static td_bool blc_clamp_info[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = TD_TRUE};
ot_isp_sns_commbus *isr2006_get_bus_info(ot_vi_pipe vi_pipe)
{
    return &g_isr2006_bus_info[vi_pipe];
}

ot_isp_sns_state *isr2006_get_ctx(ot_vi_pipe vi_pipe)
{
    return g_isr2006_sns_state[vi_pipe];
}

td_void isr2006_set_blc_clamp_value(ot_vi_pipe vi_pipe, td_bool clamp_en)
{
    blc_clamp_info[vi_pipe] = clamp_en;
}

const isr2006_video_mode_tbl g_isr2006_mode_tbl[ISR2006_MODE_BUTT] = {
    {ISR2006_VMAX_30FPS_LINEAR, ISR2006_FULL_LINES_MAX, 30, 5,
     ISR2006_WIDTH, ISR2006_HEIGHT, 0, OT_WDR_MODE_NONE, "ISR2006_SENSOR_LINEAR_MODE"},

    {ISR2006_VMAX_25FPS_LINEAR, ISR2006_FULL_LINES_MAX, 25, 5,
     ISR2006_WIDTH, ISR2006_HEIGHT, 0, OT_WDR_MODE_NONE, "ISR2006_SENSOR_LINEAR_MODE"},

    {ISR2006_VMAX_60FPS_LINEAR, ISR2006_FULL_LINES_MAX, 60, 5,
     ISR2006_WIDTH, ISR2006_HEIGHT, 0, OT_WDR_MODE_NONE, "ISR2006_SENSOR_LINEAR_MODE"},

    {ISR2006_VMAX_30FPS_LINEAR, ISR2006_FULL_LINES_MAX, 30, 5,
     ISR2006_1080P_WIDTH, ISR2006_1080P_HEIGHT, 0, OT_WDR_MODE_NONE, "ISR2006_SENSOR_LINEAR_MODE"},

    {ISR2006_VMAX_25FPS_LINEAR, ISR2006_FULL_LINES_MAX, 120, 5,
     ISR2006_WIDTH, ISR2006_HEIGHT, 0, OT_WDR_MODE_NONE, "ISR2006_SENSOR_LINEAR_MODE"},
};

td_u32 g_isr2006_vmax_linear = 0;
/****************************************************************************
 * local variables                                                            *
 ****************************************************************************/
/* isr2006 Register Address */
#define ISR2006_VMAX_ADDR_L 0x00ac
#define ISR2006_VMAX_ADDR_H 0x00ad

#define ISR2006_HMAX_ADDR_L 0x01eb
#define ISR2006_HMAX_ADDR_H 0x01ec
/*  VMAX 表示最大垂直高度
    HMAX表示最大水平宽度
*/

#define ISR2006_EXP_LINE_L 0x008f
#define ISR2006_EXP_LINE_M 0x0090
#define ISR2006_EXP_LINE_H 0x0091
/*曝光时间以行为单位 行周期与pll时钟相关 */

#define ISR2006_PLL_1 0x0083
#define ISR2006_PLL_2 0x0086
#define ISR2006_PLL_3 0x0089
/*与帧率相关的寄存器*/

#define ISR2006_CLK_DIG_L 0x0205
#define ISR2006_CLK_DIG_H 0x0206

#define ISR2006_HALF_GAIN 0x0044 // 写1是半增益，写0是全增益

#define ISR2006_dcg_1 0x009b
#define ISR2006_dcg_2 0x009c
#define ISR2006_dcg_3 0x009d
#define ISR2006_dcg_4 0x009e
/*dcg模式控制*/ // 全写0是Hcg 全写1是Lcg

#define ISR2006_REGA_L 0x0092
#define ISR2006_REGA_M 0x0093
#define ISR2006_REGA_H 0x0094
/*帧消隐时间 */

#define ISR2006_AGAIN_L 0x002b
#define ISR2006_AGAIN_H 0x002c
#define ISR2006_AGAIN_VFTCTRL 0x0030
/* 模拟增益*/

#define ISR2006_DGAIN_L 0x01d8
#define ISR2006_DGAIN_M 0x01d9
#define ISR2006_DGAIN_H 0x01dA
/*数字增益*/

// sensor gain
#define ISR2006_AGAIN_MIN 1024
#define ISR2006_AGAIN_MAX 32768

#define ISR2006_DGAIN_MIN 1024
#define ISR2006_DGAIN_MAX 1024
// #define ISR2006_DGAIN_MAX 1024

#define ISR2006_AD_GAIN_TBL_RANGE 241
#define ISR2006_A_GAIN_TBL_RANGE 193

#define ISR2006_AGAIN_TBL_RANGE 192
#define ISR2006_DGAIN_TBL_RANGE 120

#define ISR2006_EXP_TIME_LMT 1

#define isr2006_err_mode_print(sns_image_mode, sns_state)                       \
    do                                                                          \
    {                                                                           \
        isp_err_trace("Not support! Width:%u, Height:%u, Fps:%f, WDRMode:%d\n", \
                      (sns_image_mode)->width,                                  \
                      (sns_image_mode)->height,                                 \
                      (sns_image_mode)->fps,                                    \
                      (sns_state)->wdr_mode);                                   \
    } while (0)

static td_void cmos_get_ae_comm_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
                                        const ot_isp_sns_state *sns_state)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->flicker_freq = 50 * 256; /* light flicker freq: 50Hz, accuracy: 256 */
    ae_sns_dft->full_lines_max = ISR2006_FULL_LINES_MAX;
    ae_sns_dft->hmax_times = (1000000000) / (sns_state->fl_std * 25); /* 1000000000ns, 30fps */

    ae_sns_dft->int_time_accu.accu_type = OT_ISP_AE_ACCURACY_LINEAR; // 固定步进
    ae_sns_dft->int_time_accu.accuracy = 1;
    ae_sns_dft->int_time_accu.offset = 0;

    ae_sns_dft->again_accu.accu_type = OT_ISP_AE_ACCURACY_TABLE; // 查表
    ae_sns_dft->again_accu.accuracy = 1;

    ae_sns_dft->dgain_accu.accu_type = OT_ISP_AE_ACCURACY_LINEAR; //
    ae_sns_dft->dgain_accu.accuracy = (float)1 / 1024;
    ae_sns_dft->dgain_accu.offset = 0;

    ae_sns_dft->isp_dgain_shift = 8; /* accuracy: 8 */
    ae_sns_dft->min_isp_dgain_target = 1 << ae_sns_dft->isp_dgain_shift;
    ae_sns_dft->max_isp_dgain_target = 64 << ae_sns_dft->isp_dgain_shift; /* max 2 */
    if (g_lines_per500ms[vi_pipe] == 0)
    {
        ae_sns_dft->lines_per500ms = sns_state->fl_std * 25 / 2; /* 30fps, div 2 */
    }
    else
    {
        ae_sns_dft->lines_per500ms = g_lines_per500ms[vi_pipe];
    }

    ae_sns_dft->max_iris_fno = OT_ISP_IRIS_F_NO_1_0;
    ae_sns_dft->min_iris_fno = OT_ISP_IRIS_F_NO_32_0;

    ae_sns_dft->ae_route_ex_valid = TD_FALSE;
    ae_sns_dft->ae_route_attr.total_num = 0;
    ae_sns_dft->ae_route_attr_ex.total_num = 0;
    ae_sns_dft->quick_start.quick_start_enable = g_quick_start_en[vi_pipe];
    ae_sns_dft->quick_start.black_frame_num = 0;
    ae_sns_dft->ae_stat_pos = g_ae_stat_pos[vi_pipe]; /* 1 use be stat to AE */
    return;
}

static td_void cmos_get_ae_linear_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft,
                                          const ot_isp_sns_state *sns_state)
{
    ae_sns_dft->max_again = ISR2006_AGAIN_MAX;
    ae_sns_dft->min_again = ISR2006_AGAIN_MIN;
    ae_sns_dft->max_again_target = ae_sns_dft->max_again;
    ae_sns_dft->min_again_target = ae_sns_dft->min_again;

    ae_sns_dft->max_dgain = ISR2006_DGAIN_MAX;
    ae_sns_dft->min_dgain = ISR2006_DGAIN_MIN;
    ae_sns_dft->max_dgain_target = ae_sns_dft->max_dgain;
    ae_sns_dft->min_dgain_target = ae_sns_dft->min_dgain;

    ae_sns_dft->ae_compensation = 0x38;
    ae_sns_dft->ae_exp_mode = OT_ISP_AE_EXP_HIGHLIGHT_PRIOR;
    ae_sns_dft->init_exposure = g_init_exposure[vi_pipe] ? g_init_exposure[vi_pipe] : 76151; /* init 76151 */

    // ae_sns_dft->max_int_time = sns_state->fl_std - 2; /* sub 2 */
    ae_sns_dft->max_int_time = ISR2006_VMAX_25FPS_LINEAR; /* sub 2 */
    // printf("ae_sns_dft->max_int_time = %d\n", ae_sns_dft->max_int_time);
    ae_sns_dft->min_int_time = 1;            /* min 9 */
    ae_sns_dft->max_int_time_target = 65535; /* max 65535 */
    ae_sns_dft->min_int_time_target = 1;     /* min 9 */
    ae_sns_dft->ae_route_ex_valid = g_ae_route_ex_valid[vi_pipe];
    (td_void) memcpy_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route),
                       &g_init_ae_route[vi_pipe], sizeof(ot_isp_ae_route));
    (td_void) memcpy_s(&ae_sns_dft->ae_route_attr_ex, sizeof(ot_isp_ae_route_ex),
                       &g_init_ae_route_ex[vi_pipe], sizeof(ot_isp_ae_route_ex));
    return;
}

static td_s32 cmos_get_ae_default(ot_vi_pipe vi_pipe, ot_isp_ae_sensor_default *ae_sns_dft)
{
    // printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ot_isp_sns_state *sns_state = TD_NULL;

    sns_check_pointer_return(ae_sns_dft);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    (td_void) memset_s(&ae_sns_dft->ae_route_attr, sizeof(ot_isp_ae_route), 0, sizeof(ot_isp_ae_route));

    cmos_get_ae_comm_default(vi_pipe, ae_sns_dft, sns_state);

    switch (sns_state->wdr_mode)
    {
    case OT_WDR_MODE_NONE: /* linear mode */
        cmos_get_ae_linear_default(vi_pipe, ae_sns_dft, sns_state);
        hi_mpi_isp_get_wb_attr(vi_pipe, &g_isp_wb_attr);
        break;
    default:
        break;
    }

    return TD_SUCCESS;
}

static td_void cmos_config_vmax(ot_isp_sns_state *sns_state, td_u32 vmax)
{
    if (sns_state->wdr_mode == OT_WDR_MODE_NONE)
    {
        sns_state->regs_info[0].i2c_data[9].data = low_8bits(vmax);   /* array index 5 */
        sns_state->regs_info[0].i2c_data[10].data = high_2bits(vmax); /* array index 6 */
    }
    return;
}

/* the function of sensor set fps */
static td_void cmos_fps_set(ot_vi_pipe vi_pipe, td_float fps, ot_isp_ae_sensor_default *ae_sns_dft)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    td_u32 lines, lines_max, vmax;
    td_float max_fps, min_fps;
    ot_isp_sns_state *sns_state = TD_NULL;

    sns_check_pointer_void_return(ae_sns_dft);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    ot_isp_sns_state *pastisr2006 = TD_NULL;
    pastisr2006 = isr2006_get_ctx(vi_pipe);
    td_u8 img_mode = pastisr2006->img_mode;

    lines = g_isr2006_mode_tbl[sns_state->img_mode].ver_lines;
    if (img_mode == ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE)
    {
        lines = ISR2006_VMAX_25FPS_LINEAR;
    }
    lines_max = g_isr2006_mode_tbl[1].max_ver_lines;
    max_fps = g_isr2006_mode_tbl[sns_state->img_mode].max_fps;
    min_fps = g_isr2006_mode_tbl[sns_state->img_mode].min_fps;
    // lines_max = g_isr2006_mode_tbl[0].max_ver_lines;
    // max_fps = g_isr2006_mode_tbl[0].max_fps;
    // min_fps = g_isr2006_mode_tbl[0].min_fps;

    if ((fps > max_fps) || (fps < min_fps))
    {
        isp_err_trace("Not support Fps: %f\n", fps);
        return;
    }

    vmax = (td_u32)(lines * max_fps / div_0_to_1_float(fps));
    vmax = (vmax > lines_max) ? lines_max : vmax;
    cmos_config_vmax(sns_state, vmax);

    sns_state->fl_std = vmax;
    ae_sns_dft->lines_per500ms = (td_u32)(lines * max_fps / 2); /* div 2 */

    ae_sns_dft->fps = fps;
    ae_sns_dft->full_lines_std = sns_state->fl_std;
    ae_sns_dft->max_int_time = sns_state->fl_std - ISR2006_EXP_TIME_LMT;
    sns_state->fl[0] = sns_state->fl_std;
    ae_sns_dft->full_lines = sns_state->fl[0];
    ae_sns_dft->hmax_times =
        (td_u32)((1000000000) / (sns_state->fl_std * div_0_to_1_float(fps))); /* 1000000000ns */

    return;
}

static td_void cmos_slow_framerate_set(ot_vi_pipe vi_pipe, td_u32 full_lines, ot_isp_ae_sensor_default *ae_sns_dft)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    td_u32 lines_max;
    td_u32 vmax;
    ot_isp_sns_state *sns_state = TD_NULL;
    sns_check_pointer_void_return(ae_sns_dft);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    lines_max = g_isr2006_mode_tbl[sns_state->img_mode].max_ver_lines;

    if (sns_state->wdr_mode == OT_WDR_MODE_NONE)
    {
        vmax = full_lines;
        vmax = (vmax > lines_max) ? lines_max : vmax;
        sns_state->fl[0] = vmax;
    }

    switch (sns_state->wdr_mode)
    {
    case OT_WDR_MODE_NONE:
        sns_state->regs_info[0].i2c_data[9].data = low_8bits(vmax);   /* index 9 */
        sns_state->regs_info[0].i2c_data[10].data = high_2bits(vmax); /* index 10 */

        break;
    default:
        break;
    }

    ae_sns_dft->full_lines = sns_state->fl[0];
    ae_sns_dft->max_int_time = sns_state->fl[0] - ISR2006_EXP_TIME_LMT;
    return;
}

// static td_void cmos_slow_framerate_set(ot_vi_pipe vi_pipe, td_u32 full_lines, ot_isp_ae_sensor_default *ae_sns_dft)
// {
//     //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
//     td_u32 lines_max;
//     td_u32 vmax;
//     ot_isp_sns_state *sns_state = TD_NULL;
//     sns_check_pointer_void_return(ae_sns_dft);
//     isr2006_sensor_get_ctx(vi_pipe, sns_state);
//     sns_check_pointer_void_return(sns_state);

//    td_float fps = ae_sns_dft->fps;
//    if(fps*10 - 25 * 10 < 1 )
//    {
//         sns_state->regs_info[0].i2c_data[11].data = 0xc0;
//         sns_state->regs_info[0].i2c_data[12].data = 0x08;
//         ae_sns_dft->fps = 60;
//    }else if (fps*10 - 60 * 10 < 1)
//    {
//         sns_state->regs_info[0].i2c_data[11].data = 0xa0;
//         sns_state->regs_info[0].i2c_data[12].data = 0x03;
//         ae_sns_dft->fps = 25;
//    }

//     return;
// }

_Atomic td_u32 s_intttime = 0;

static td_u32 s_value = 0;
static hi_s32 isr2006_fps = 25;

static td_void cmos_fps_set_60_25(td_u32 value, ot_isp_sns_state *sns_state)
{
    if (((value <= 5)))
    {
        if (isr2006_fps == 25)
        {
            sns_state->regs_info[0].i2c_data[18].data = 0xc0;
            sns_state->regs_info[0].i2c_data[19].data = 0x08;
            sns_state->regs_info[0].i2c_data[20].data = 0x05;
            isr2006_fps = 60;
            printf("isr2006_fps = 60\n");
        }
    }
    else if (((value > 1200) && (g_again >=30*1024)))
    {
        if (isr2006_fps == 60)
        {
            sns_state->regs_info[0].i2c_data[18].data = 0xa0;
            sns_state->regs_info[0].i2c_data[19].data = 0x03;
            sns_state->regs_info[0].i2c_data[20].data = 0x05;
            isr2006_fps = 25;
            printf("isr2006_fps = 25\n");
        }
    }

    // printf("time0 = %x, time1 = %x\n", sns_state->regs_info[0].i2c_data[0].data, sns_state->regs_info[0].i2c_data[1].data);
    if (isr2006_fps == 60)
    {
        sns_state->regs_info[0].i2c_data[15].data = low_8bits(g_isr2006_vmax_linear - 40 - value - 1);
        sns_state->regs_info[0].i2c_data[16].data = high_8bits(g_isr2006_vmax_linear - 40 - value - 1);
        sns_state->regs_info[0].i2c_data[17].data = higher_8bits(g_isr2006_vmax_linear - 40 - value - 1);
        // printf("reg_a0 = %x, reg_a1 = %x, reg_a2 = %x\n", sns_state->regs_info[0].i2c_data[11].data, sns_state->regs_info[0].i2c_data[12].data, sns_state->regs_info[0].i2c_data[13].data);
    }
    else if (isr2006_fps == 25)
    {
        sns_state->regs_info[0].i2c_data[15].data = low_8bits(32);
        sns_state->regs_info[0].i2c_data[16].data = high_8bits(32);
        sns_state->regs_info[0].i2c_data[17].data = higher_8bits(32);
    }
}


static td_void cmos_dcg_set(td_u32 value, ot_isp_sns_state *sns_state)
{
    td_bool dcg_mode = sns_state->regs_info[0].i2c_data[11].data; // 0表示HCG 1表示LCG

    if ((value <= 5) && dcg_mode == 0)
    {
        sns_state->regs_info[0].i2c_data[11].data = 1;
        sns_state->regs_info[0].i2c_data[12].data = 1;
        sns_state->regs_info[0].i2c_data[13].data = 1;
        sns_state->regs_info[0].i2c_data[14].data = 1;

    }
    // else if (value <= 1000)
    // {
    //     sns_state->regs_info[0].i2c_data[11].data = 1;
    //     sns_state->regs_info[0].i2c_data[12].data = 1;
    //     sns_state->regs_info[0].i2c_data[13].data = 1;
    //     sns_state->regs_info[0].i2c_data[14].data = 1;
    // }
    else if ((value > 1200) && dcg_mode == 1)
    {
        sns_state->regs_info[0].i2c_data[11].data = 0;
        sns_state->regs_info[0].i2c_data[12].data = 0;
        sns_state->regs_info[0].i2c_data[13].data = 0;
        sns_state->regs_info[0].i2c_data[14].data = 0;

    }
}


static td_void cmos_half_gain_set(td_u32 value, ot_isp_sns_state *sns_state)
{
    if ((value <= (5)))
    {
        td_u16 clk_dig = 0x80f8 + 0x200;

        sns_state->regs_info[0].i2c_data[21].data = 0x01;

        sns_state->regs_info[0].i2c_data[22].data = low_8bits(clk_dig);
        sns_state->regs_info[0].i2c_data[23].data = high_8bits(clk_dig);
    }
    // else if(value > 20)
    // {
    //     td_u16 clk_dig = 0x80f8 + 0x200 - 0x20;

    //     sns_state->regs_info[0].i2c_data[21].data = 0x01;

    //     sns_state->regs_info[0].i2c_data[22].data = low_8bits(clk_dig);
    //     sns_state->regs_info[0].i2c_data[23].data = high_8bits(clk_dig);
    // }
    else if (value > 1200)
    {
        td_u16 clk_dig = 0x80f8;

        sns_state->regs_info[0].i2c_data[21].data = 0x01;

        sns_state->regs_info[0].i2c_data[22].data = low_8bits(clk_dig);
        sns_state->regs_info[0].i2c_data[23].data = high_8bits(clk_dig);
    }
    else if ((value > (1200) && (g_again >=30*1024)))
    {
        td_u16 clk_dig = 0x80f8;

        sns_state->regs_info[0].i2c_data[21].data = 0x00;

        sns_state->regs_info[0].i2c_data[22].data = low_8bits(clk_dig);
        sns_state->regs_info[0].i2c_data[23].data = high_8bits(clk_dig);
    }
}

static td_void cmos_inttime_update_linear(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    // int_time 表示曝光行数
    ot_isp_sns_state *sns_state = TD_NULL;
    td_u32 value;

    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    value = min(int_time, ISR2006_FULL_LINES_MAX);
    value = min(max(value, ISR2006_EXP_TIME_LMT), (ISR2006_VMAX_25FPS_LINEAR));

    value -= 1;
    s_intttime = value;
    if (s_value != value)
    {
        printf("int_time = %d, value = %d\n", int_time, value);
        s_value = value;
    }

    sns_state->regs_info[0].i2c_data[0].data = low_8bits(value);
    sns_state->regs_info[0].i2c_data[1].data = high_8bits(value);   /* index 1 */
    sns_state->regs_info[0].i2c_data[2].data = higher_8bits(value); /* index 2 */

    cmos_dcg_set(value, sns_state);

    cmos_fps_set_60_25(value, sns_state);

    //cmos_half_gain_set(value, sns_state);

    return;
}

/* while isp notify ae to update sensor regs, ae call these funcs. */
static td_void cmos_inttime_update(ot_vi_pipe vi_pipe, td_u32 int_time)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ot_isp_sns_state *sns_state = TD_NULL;
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    if (sns_state->wdr_mode == OT_WDR_MODE_NONE)
    {
        cmos_inttime_update_linear(vi_pipe, int_time);
    }
    return;
}

/* Again and Dgain use the same table, Step is 0.3db */
static td_u32 g_gain_table[ISR2006_AD_GAIN_TBL_RANGE] = {
    1024, 1059, 1097, 1135, 1175, 1217, 1259, 1304, 1349, 1397, 1446, 1497, 1549, 1604,
    1660, 1719, 1779, 1842, 1906, 1973, 2043, 2114, 2189, 2266, 2345, 2428, 2513, 2601,
    2693, 2788, 2886, 2987, 3092, 3201, 3313, 3430, 3550, 3675, 3804, 3938, 4076, 4219,
    4368, 4521, 4680, 4845, 5015, 5191, 5374, 5562, 5758, 5960, 6170, 6387, 6611, 6843,
    7084, 7333, 7591, 7857, 8133, 8419, 8715, 9021, 9338, 9667, 10006, 10358, 10722, 11099,
    11489, 11893, 12311, 12743, 13191, 13655, 14135, 14631, 15146, 15678, 16229, 16799, 17390, 18001,
    18633, 19288, 19966, 20668, 21394, 22146, 22924, 23730, 24564, 25427, 26320, 27245, 28203, 29194,
    30220, 31282, 32381, 41519, 34697, 35917, 37179, 38485, 39838, 41238, 42687, 44187, 45740, 47347,
    49011, 50734, 52517, 54362, 56273, 58250, 60297, 62416, 64610, 66880, 69230, 71663, 74182, 76789,
    79487, 82281, 85172, 88165, 91264, 94471, 97791, 101228, 104785, 108468, 112279, 116225, 120310, 124537,
    128914, 133444, 138134, 142988, 148013, 153215, 158599, 164172, 169942, 175914, 182096, 188495, 195119, 201976,
    209074, 216421, 224027, 231900, 240049, 248485, 257217, 266256, 275613, 285299, 295325, 305703, 316446, 327567,
    339078, 350994, 363329, 376097, 389314, 402995, 417157, 431817, 446992, 462700, 478961, 495793, 513216, 531251,
    549921, 569246, 589250, 609958, 631393, 653582, 676550, 700326, 724936, 750412, 776783, 804081, 832338, 861589,
    891867, 923209, 955652, 989236, 1024000, 1059985, 1097236, 1135795, 1175709, 1217026, 1259795, 1304067, 1349895,
    1397333, 1446438, 1497269, 1549887, 1604353, 1660734, 1719095, 1779508, 1842044, 1906777, 1973786, 2043149,
    2114949, 2189273, 2266209, 2345848, 2428287, 2513622, 2601956, 2693394, 2788046, 2886024, 2987445, 3092431,
    3201105, 3313599, 3430046, 3550585, 3675361, 3804521, 3938220, 4076617};

static td_u32 a_gain_table[ISR2006_A_GAIN_TBL_RANGE] = {
    1024, 1088, 1152, 1216, 1280, 1344, 1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856, 1920, 1984, 2048,
    2176, 2304, 2432, 2560, 2688, 2816, 2944, 3072, 3200, 3328, 3456, 3584, 3712, 3840, 3968, 4096,
    4224, 4352, 4480, 4608, 4736, 4864, 4992, 5120, 5248, 5376, 5504, 5632, 5760, 5888, 6016, 6144,
    6272, 6400, 6528, 6656, 6784, 6912, 7040, 7168, 7296, 7424, 7552, 7680, 7808, 7936, 8064, 8192,
    8320, 8448, 8576, 8704, 8832, 8960, 9088, 9216, 9344, 9472, 9600, 9728, 9856, 9984, 10112, 10240,
    10368, 10496, 10624, 10752, 10880, 11008, 11136, 11264, 11392, 11520, 11648, 11776, 11904, 12032,
    12160, 12288, 12416, 12544, 12672, 12800, 12928, 13056, 13184, 13312, 13440, 13568, 13696, 13824,
    13952, 14080, 14208, 14336, 14464, 14592, 14720, 14848, 14976, 15104, 15232, 15360, 15488, 15616,
    15744, 15872, 16000, 16128, 16256, 16384, 16640, 16896, 17152, 17408, 17664, 17920, 18176, 18432,
    18688, 18944, 19200, 19456, 19712, 19968, 20224, 20480, 20736, 20992, 21248, 21504, 21760, 22016,
    22272, 22528, 22784, 23040, 23296, 23552, 23808, 24064, 24320, 24576, 24832, 25088, 25344, 25600,
    25856, 26112, 26368, 26624, 26880, 27136, 27392, 27648, 27904, 28160, 28416, 28672, 28928, 29184,
    29440, 29696, 29952, 30208, 30464, 30720, 30976, 31232, 31488, 31744, 32000, 32256, 32512, 32768};

static td_void print_again(void)
{
    int i = 0;
    int j = 0;
    for (i = 0; i < ISR2006_AGAIN_TBL_RANGE + 1; i++, j++)
    {
        printf("again[%d]=%d,", i, a_gain_table[i]);
        if (a_gain_table[i] == 0)
        {
            printf("err num = %d\n", i);
            return;
        }
        if (j == 7)
        {
            printf("\n");
            j = 0;
        }
    }
    printf("\n");
}

static td_void cmos_again_calc_table(ot_vi_pipe vi_pipe, td_u32 *again_lin, td_u32 *again_db)
{
    int i;
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    sns_check_pointer_void_return(again_lin);
    sns_check_pointer_void_return(again_db);
    ot_unused(vi_pipe);

    if (*again_lin >= a_gain_table[ISR2006_AGAIN_TBL_RANGE])
    {
        *again_lin = a_gain_table[ISR2006_AGAIN_TBL_RANGE];
        *again_db = a_gain_table[ISR2006_AGAIN_TBL_RANGE];
    }
    else
    {
        for (i = 1; i < (ISR2006_AGAIN_TBL_RANGE + 1); i++)
        {
            if (*again_lin < a_gain_table[i])
            {
                *again_lin = a_gain_table[i - 1];
                *again_db = a_gain_table[i - 1];
                break;
            }
        }
    }

    // printf("againtable : lin = %d db = %d\n", *again_lin, *again_db);
    return;
}

static td_void cmos_dgain_calc_table(ot_vi_pipe vi_pipe, td_u32 *dgain_lin, td_u32 *dgain_db)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    int i;
    sns_check_pointer_void_return(dgain_lin);
    sns_check_pointer_void_return(dgain_db);
    ot_unused(vi_pipe);

    if (*dgain_lin >= g_gain_table[ISR2006_DGAIN_TBL_RANGE])
    {
        *dgain_lin = g_gain_table[ISR2006_DGAIN_TBL_RANGE];
        *dgain_db = g_gain_table[ISR2006_DGAIN_TBL_RANGE];
        return;
    }

    for (i = 1; i < (ISR2006_DGAIN_TBL_RANGE + 1); i++)
    {
        if (*dgain_lin < g_gain_table[i])
        {
            *dgain_lin = g_gain_table[i - 1];
            *dgain_db = g_gain_table[i - 1];
            break;
        }
    }
    return;
}


static td_u8 ramp_vftcrtl_reg_table[RAMP_CG_BUTT][RAMP_VFTCTRL_NUM] = {
    {0x1f, 0x2b, 0x2d, 0x30, 0x31, 0x32, 0x32, 0x32, 0x2e, 0x2e, 0x2e, 0x2e},
    {0x1f, 0x2b, 0x2d, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2f, 0x30, 0x31, 0x32},
    {0x1f, 0x2b, 0x2d, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2e, 0x2f, 0x30},
    {0x1f, 0x1f, 0x2b, 0x2b, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x31}};

static td_void cmos_gains_update(ot_vi_pipe vi_pipe, td_u32 again, td_u32 dgain)
{
    ot_isp_sns_state *sns_state = TD_NULL;
    // td_u32 tmp;

    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    td_u8 ramp_mode = RAMP_TEST;

// #region
// int ret = 0;
// if (again <= 4096)
// {
//     hi_mpi_isp_get_wb_attr(vi_pipe,&g_isp_wb_attr);
//     g_isp_wb_attr.auto_attr.curve_para[0] = 18;
//     g_isp_wb_attr.auto_attr.curve_para[1] = 188;
//     ret = hi_mpi_isp_set_wb_attr(vi_pipe, &g_isp_wb_attr);
// }else
// {
//     hi_mpi_isp_get_wb_attr(vi_pipe,&g_isp_wb_attr);
//     g_isp_wb_attr.auto_attr.curve_para[0] = 36;
//     g_isp_wb_attr.auto_attr.curve_para[1] = 338;
//     ret = hi_mpi_isp_set_wb_attr(vi_pipe, &g_isp_wb_attr);
// }
// #endregion
#if 1
    int again_tmp = again / 64;
    if (again_tmp <= 128) // 8
    {
        sns_state->regs_info[0].i2c_data[3].data = (again_tmp - 16) & (0x7f);
        sns_state->regs_info[0].i2c_data[4].data = 0x0;
        //[7:8] = 00b
        if (again_tmp == 16) // 1倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][0];
        }
        else if (again_tmp <= 32) // 小于等于2倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][1];
        }
        else if (again_tmp <= 48) // 小于等于3倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][2];
        }
        else if (again_tmp <= 64) // 小于等于4倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][3];
        }
        else if (again_tmp <= 96) // 小于等于6倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][4];
        }
        else if (again_tmp <= 128) // 小于等于8倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][5];
        }
    }
    else if (again_tmp <= 256) // 16   //[7:8] = 01b
    {
        sns_state->regs_info[0].i2c_data[3].data = (((again_tmp - 16) >> 1) & (0x7f)) | 0x80;
        sns_state->regs_info[0].i2c_data[4].data = 0x0;
        if (again_tmp <= 224) // 14倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][6];
        }
        else if (again_tmp <= 256) // 16倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][7];
        }
    }
    else if (again_tmp <= 512)
    {
        sns_state->regs_info[0].i2c_data[3].data = ((again_tmp - 16) >> 2) & (0x7f); //[7:8] = 10b
        sns_state->regs_info[0].i2c_data[4].data = 0x1;
        if (again_tmp <= 320) // 20倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][8];
        }
        else if (again_tmp <= 384) // 24倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][9];
        }
        else if (again_tmp <= 448) // 28倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][10];
        }
        else if (again_tmp <= 512) // 32倍增益
        {
            sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][11];
        }
    }

    sns_state->regs_info[0].i2c_data[6].data = (dgain / 1024) - 1;
    sns_state->regs_info[0].i2c_data[7].data = low_8bits((dgain) % 1024);
    sns_state->regs_info[0].i2c_data[8].data = high_2bits((dgain) % 1024);
#else
    sns_state->regs_info[0].i2c_data[3].data = (1024 / 64 - 16) & (0x7f);
    sns_state->regs_info[0].i2c_data[4].data = 0x0;
    sns_state->regs_info[0].i2c_data[5].data = ramp_vftcrtl_reg_table[ramp_mode][0];

    sns_state->regs_info[0].i2c_data[6].data = 1024 / 1024;
    sns_state->regs_info[0].i2c_data[7].data = low_8bits(1024 % 1024);
    sns_state->regs_info[0].i2c_data[8].data = high_2bits(1024 % 1024);
#endif
    if (g_again != again || g_dgain != dgain)
    {
        printf("again = %d又%d/16, dgain = %d又%d/1024\n", again_tmp / 16, again_tmp % 16, dgain / 1024, dgain % 1024);
        g_again = again;
        g_dgain = dgain;
    }

    return;
}

static td_void cmos_get_inttime_max(ot_vi_pipe vi_pipe, td_u16 man_ratio_enable, td_u32 *ratio,
                                    ot_isp_ae_int_time_range *int_time, td_u32 *lf_max_int_time)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ot_isp_sns_state *sns_state = TD_NULL;
    sns_check_pointer_void_return(ratio);
    sns_check_pointer_void_return(int_time);
    sns_check_pointer_void_return(lf_max_int_time);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    ot_unused(man_ratio_enable);

    return;
}

/* Only used in LINE_WDR mode */
static td_void cmos_ae_fswdr_attr_set(ot_vi_pipe vi_pipe, ot_isp_ae_fswdr_attr *ae_fswdr_attr)
{

    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    sns_check_pointer_void_return(ae_fswdr_attr);

    g_fswdr_mode[vi_pipe] = ae_fswdr_attr->fswdr_mode;
    g_max_time_get_cnt[vi_pipe] = 0;
    return;
}

static td_s32 cmos_init_ae_exp_function(ot_isp_ae_sensor_exp_func *exp_func)
{
    sns_check_pointer_return(exp_func);

    (td_void) memset_s(exp_func, sizeof(ot_isp_ae_sensor_exp_func), 0, sizeof(ot_isp_ae_sensor_exp_func));

    exp_func->pfn_cmos_get_ae_default = cmos_get_ae_default;
    exp_func->pfn_cmos_fps_set = cmos_fps_set;
    exp_func->pfn_cmos_slow_framerate_set = cmos_slow_framerate_set;
    exp_func->pfn_cmos_inttime_update = cmos_inttime_update;
    exp_func->pfn_cmos_gains_update = cmos_gains_update;
    exp_func->pfn_cmos_again_calc_table = cmos_again_calc_table;
    exp_func->pfn_cmos_dgain_calc_table = cmos_dgain_calc_table;
    exp_func->pfn_cmos_get_inttime_max = cmos_get_inttime_max;
    exp_func->pfn_cmos_ae_fswdr_attr_set = cmos_ae_fswdr_attr_set;

    return TD_SUCCESS;
}

/* awb static param for Fuji Lens New IR_Cut */
#define CALIBRATE_STATIC_TEMP 4950
#define CALIBRATE_STATIC_WB_R_GAIN 450
#define CALIBRATE_STATIC_WB_GR_GAIN 256
#define CALIBRATE_STATIC_WB_GB_GAIN 256
#define CALIBRATE_STATIC_WB_B_GAIN 447

/* Calibration results for Auto WB Planck */
#define CALIBRATE_AWB_P1 18
#define CALIBRATE_AWB_P2 238
#define CALIBRATE_AWB_Q1 0
#define CALIBRATE_AWB_A1 212714
#define CALIBRATE_AWB_B1 128
#define CALIBRATE_AWB_C1 (-163748)

/* Rgain and Bgain of the golden sample */
#define GOLDEN_RGAIN 0
#define GOLDEN_BGAIN 0
static td_s32 cmos_get_awb_default(ot_vi_pipe vi_pipe, ot_isp_awb_sensor_default *awb_sns_dft)
{
    ot_isp_sns_state *sns_state = TD_NULL;

    sns_check_pointer_return(awb_sns_dft);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    (td_void) memset_s(awb_sns_dft, sizeof(ot_isp_awb_sensor_default), 0, sizeof(ot_isp_awb_sensor_default));
    awb_sns_dft->wb_ref_temp = CALIBRATE_STATIC_TEMP; /* wb_ref_temp 4950 */

    awb_sns_dft->gain_offset[0] = CALIBRATE_STATIC_WB_R_GAIN;
    awb_sns_dft->gain_offset[1] = CALIBRATE_STATIC_WB_GR_GAIN;
    awb_sns_dft->gain_offset[2] = CALIBRATE_STATIC_WB_GB_GAIN; /* index 2 */
    awb_sns_dft->gain_offset[3] = CALIBRATE_STATIC_WB_B_GAIN;  /* index 3 */

    awb_sns_dft->wb_para[0] = CALIBRATE_AWB_P1;
    awb_sns_dft->wb_para[1] = CALIBRATE_AWB_P2;
    awb_sns_dft->wb_para[2] = CALIBRATE_AWB_Q1; /* index 2 */
    awb_sns_dft->wb_para[3] = CALIBRATE_AWB_A1; /* index 3 */
    awb_sns_dft->wb_para[4] = CALIBRATE_AWB_B1; /* index 4 */
    awb_sns_dft->wb_para[5] = CALIBRATE_AWB_C1; /* index 5 */

    awb_sns_dft->golden_rgain = GOLDEN_RGAIN;
    awb_sns_dft->golden_bgain = GOLDEN_BGAIN;

    switch (sns_state->wdr_mode)
    {
    case OT_WDR_MODE_NONE:
        (td_void) memcpy_s(&awb_sns_dft->ccm, sizeof(ot_isp_awb_ccm), &g_awb_ccm, sizeof(ot_isp_awb_ccm));
        (td_void) memcpy_s(&awb_sns_dft->agc_tbl, sizeof(ot_isp_awb_agc_table),
                           &g_awb_agc_table, sizeof(ot_isp_awb_agc_table));
        break;
    default:
        break;
    }

    awb_sns_dft->init_rgain = g_init_wb_gain[vi_pipe][0]; /* 0: Rgain */
    awb_sns_dft->init_ggain = g_init_wb_gain[vi_pipe][1]; /* 1: Ggain */
    awb_sns_dft->init_bgain = g_init_wb_gain[vi_pipe][2]; /* 2: Bgain */
    awb_sns_dft->sample_rgain = g_sample_r_gain[vi_pipe];
    awb_sns_dft->sample_bgain = g_sample_b_gain[vi_pipe];

    return TD_SUCCESS;
}

static td_s32 cmos_init_awb_exp_function(ot_isp_awb_sensor_exp_func *exp_func)
{
    sns_check_pointer_return(exp_func);

    (td_void) memset_s(exp_func, sizeof(ot_isp_awb_sensor_exp_func), 0, sizeof(ot_isp_awb_sensor_exp_func));

    exp_func->pfn_cmos_get_awb_default = cmos_get_awb_default;

    return TD_SUCCESS;
}

static ot_isp_cmos_dng_color_param g_dng_color_param = {{378, 256, 430}, {439, 256, 439}};

static td_void cmos_get_isp_dng_default(const ot_isp_sns_state *sns_state, ot_isp_cmos_default *isp_def)
{
    (td_void) memcpy_s(&isp_def->dng_color_param, sizeof(ot_isp_cmos_dng_color_param), &g_dng_color_param,
                       sizeof(ot_isp_cmos_dng_color_param));

    switch (sns_state->img_mode)
    {
    case ISR2006_SENSOR_1936_1280_30FPS_12BIT_LINEAR_MODE:
    case ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE:
        isp_def->sns_mode.dng_raw_format.bits_per_sample = 12; /* 12bit */
        isp_def->sns_mode.dng_raw_format.white_level = 4095;   /* max 4095  white_level = （2的Bit_Depth次方）- 1  Bit_Depth指比特深度 此处为12*/
        break;
    case ISR2006_SENSOR_1920_1080_30FPS_14BIT_LINEAR_MODE:
        isp_def->sns_mode.dng_raw_format.bits_per_sample = 14; /* 12bit */
        isp_def->sns_mode.dng_raw_format.white_level = 16383;  /* max 16383  white_level = （2的Bit_Depth次方）- 1  Bit_Depth指比特深度 此处为14*/
    default:
        break;
    }

    isp_def->sns_mode.dng_raw_format.default_scale.default_scale_hor.denominator = 1;
    isp_def->sns_mode.dng_raw_format.default_scale.default_scale_hor.numerator = 1;
    isp_def->sns_mode.dng_raw_format.default_scale.default_scale_ver.denominator = 1;
    isp_def->sns_mode.dng_raw_format.default_scale.default_scale_ver.numerator = 1;
    isp_def->sns_mode.dng_raw_format.cfa_repeat_pattern_dim.repeat_pattern_dim_row = 2; /* pattern 2 */
    isp_def->sns_mode.dng_raw_format.cfa_repeat_pattern_dim.repeat_pattern_dim_col = 2; /* pattern 2 */
    isp_def->sns_mode.dng_raw_format.black_level_repeat_dim.repeat_row = 2;             /* pattern 2 */
    isp_def->sns_mode.dng_raw_format.black_level_repeat_dim.repeat_col = 2;             /* pattern 2 */
    isp_def->sns_mode.dng_raw_format.cfa_layout = OT_ISP_CFALAYOUT_TYPE_RECTANGULAR;
    isp_def->sns_mode.dng_raw_format.cfa_plane_color[0] = 0;
    isp_def->sns_mode.dng_raw_format.cfa_plane_color[1] = 1;
    isp_def->sns_mode.dng_raw_format.cfa_plane_color[2] = 2; /* index 2, cfa_plane_color 2 */
    isp_def->sns_mode.dng_raw_format.cfa_pattern[0] = 0;
    isp_def->sns_mode.dng_raw_format.cfa_pattern[1] = 1;
    isp_def->sns_mode.dng_raw_format.cfa_pattern[2] = 1; /* index 2, cfa_pattern 1 */
    isp_def->sns_mode.dng_raw_format.cfa_pattern[3] = 2; /* index 3, cfa_pattern 2 */
    isp_def->sns_mode.valid_dng_raw_format = TD_TRUE;
    return;
}

static void cmos_get_isp_linear_default(ot_isp_cmos_default *isp_def)
{
    isp_def->key.bit1_demosaic = 1;
    isp_def->demosaic = &g_cmos_demosaic;
    isp_def->key.bit1_sharpen = 1;
    isp_def->sharpen = &g_cmos_yuv_sharpen;
    isp_def->key.bit1_drc = 1;
    isp_def->drc = &g_cmos_drc;
    isp_def->key.bit1_bayer_nr = 1;
    isp_def->bayer_nr = &g_cmos_bayer_nr;
    isp_def->key.bit1_anti_false_color = 1;
    isp_def->anti_false_color = &g_cmos_anti_false_color;
    isp_def->key.bit1_ldci = 1;
    isp_def->ldci = &g_cmos_ldci;
    isp_def->key.bit1_gamma = 1;
    isp_def->gamma = &g_cmos_gamma;
#ifdef CONFIG_OT_ISP_CR_SUPPORT
    isp_def->key.bit1_ge = 1;
    isp_def->ge = &g_cmos_ge;
#endif
    isp_def->key.bit1_dehaze = 1;
    isp_def->dehaze = &g_cmos_dehaze;
    (td_void) memcpy_s(&isp_def->noise_calibration, sizeof(ot_isp_noise_calibration),
                       &g_cmos_noise_calibration, sizeof(ot_isp_noise_calibration));
    return;
}

static td_s32 cmos_get_isp_default(ot_vi_pipe vi_pipe, ot_isp_cmos_default *isp_def)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ot_isp_sns_state *sns_state = TD_NULL;

    sns_check_pointer_return(isp_def);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    (td_void) memset_s(isp_def, sizeof(ot_isp_cmos_default), 0, sizeof(ot_isp_cmos_default));
#ifdef CONFIG_OT_ISP_CA_SUPPORT

    isp_def->key.bit1_ca = 1;
    isp_def->ca = &g_cmos_ca;
#endif
    isp_def->key.bit1_clut = 1;
    isp_def->clut = &g_cmos_clut;

    isp_def->key.bit1_dpc = 1;
    isp_def->dpc = &g_cmos_dpc;

    isp_def->key.bit1_wdr = 1;
    isp_def->wdr = &g_cmos_wdr;

    isp_def->key.bit1_lsc = 0;
    isp_def->lsc = &g_cmos_lsc;

    isp_def->key.bit1_acs = 0;
    isp_def->acs = &g_cmos_acs;

#ifdef CONFIG_OT_ISP_PREGAMMA_SUPPORT
    isp_def->key.bit1_pregamma = 0;
    isp_def->pregamma = &g_cmos_pregamma;
#endif
    switch (sns_state->wdr_mode)
    {
    case OT_WDR_MODE_NONE:
        cmos_get_isp_linear_default(isp_def);
        break;
    default:
        break;
    }

    isp_def->wdr_switch_attr.exp_ratio[0] = 0x40;
    isp_def->sns_mode.sns_id = ISR2006_ID;
    isp_def->sns_mode.sns_mode = sns_state->img_mode;
    cmos_get_isp_dng_default(sns_state, isp_def);

    return TD_SUCCESS;
}

static td_s32 cmos_get_isp_black_level(ot_vi_pipe vi_pipe, ot_isp_cmos_black_level *black_level)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    td_s32 i;
    ot_isp_sns_state *sns_state = TD_NULL;

    sns_check_pointer_return(black_level);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    (td_void) memcpy_s(black_level, sizeof(ot_isp_cmos_black_level), &g_cmos_blc, sizeof(ot_isp_cmos_black_level));

    /* Don't need to update black level when iso change */
    black_level->auto_attr.update = TD_FALSE;

    /* black level of linear mode */
    if (sns_state->wdr_mode == OT_WDR_MODE_NONE)
    {
        for (i = 0; i < OT_ISP_BAYER_CHN_NUM; i++)
        {
            black_level->auto_attr.black_level[0][i] = 0x00;
        }
    }

    return TD_SUCCESS;
}

static td_s32 cmos_get_isp_blc_clamp_info(ot_vi_pipe vi_pipe, td_bool *blc_clamp_en)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    sns_check_pointer_return(blc_clamp_en);

    *blc_clamp_en = blc_clamp_info[vi_pipe];

    return TD_SUCCESS;
}

static td_void cmos_set_pixel_detect(ot_vi_pipe vi_pipe, td_bool enable)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    td_u32 full_lines_5fps, max_int_time_5fps;
    ot_isp_sns_state *sns_state = TD_NULL;

    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    if (sns_state->wdr_mode == OT_WDR_MODE_2To1_LINE)
    {
        return;
    }
    else
    {
        if (sns_state->img_mode == ISR2006_SENSOR_1936_1280_30FPS_12BIT_LINEAR_MODE)
        {
            full_lines_5fps = (sns_state->fl[0] * 25) / 5; /* 30fps, 5fps */
        }
        else
        {
            return;
        }
    }

    max_int_time_5fps = ISR2006_EXP_TIME_LMT;

    if (enable)
    { /* setup for ISP pixel calibration mode */
#if !ISR2006_TEST_PIC
        isr2006_write_register(vi_pipe, ISR2006_AGAIN_L, 0x00);
        isr2006_write_register(vi_pipe, ISR2006_AGAIN_H, 0x00);
        isr2006_write_register(vi_pipe, ISR2006_AGAIN_VFTCTRL, 0x1f);

        isr2006_write_register(vi_pipe, ISR2006_VMAX_ADDR_L, low_8bits(full_lines_5fps));
        isr2006_write_register(vi_pipe, ISR2006_VMAX_ADDR_H, high_2bits(full_lines_5fps));

        isr2006_write_register(vi_pipe, ISR2006_EXP_LINE_L, low_8bits(max_int_time_5fps));
        isr2006_write_register(vi_pipe, ISR2006_EXP_LINE_M, high_8bits(max_int_time_5fps));
        isr2006_write_register(vi_pipe, ISR2006_EXP_LINE_H, higher_8bits(max_int_time_5fps));
#endif
    }
    else
    { /* setup for ISP 'normal mode' */
#if !ISR2006_TEST_PIC
        sns_state->fl_std = (sns_state->fl_std > ISR2006_FULL_LINES_MAX) ? ISR2006_FULL_LINES_MAX : sns_state->fl_std;
        isr2006_write_register(vi_pipe, ISR2006_VMAX_ADDR_L, low_8bits(sns_state->fl_std));
        isr2006_write_register(vi_pipe, ISR2006_VMAX_ADDR_H, high_2bits(sns_state->fl_std));
        sns_state->sync_init = TD_FALSE;
#endif
    }

    return;
}

static td_s32 cmos_set_wdr_mode(ot_vi_pipe vi_pipe, td_u8 mode)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ot_isp_sns_state *sns_state = TD_NULL;

    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    sns_state->sync_init = TD_FALSE;

    switch (mode & 0x3F)
    {
    case OT_WDR_MODE_NONE:
        sns_state->wdr_mode = OT_WDR_MODE_NONE;
        printf("linear mode\n");
        break;

    default:
        isp_err_trace("NOT support this mode!\n");
        return TD_FAILURE;
    }

    (td_void) memset_s(sns_state->wdr_int_time, sizeof(sns_state->wdr_int_time), 0, sizeof(sns_state->wdr_int_time));

    return TD_SUCCESS;
}

static td_void cmos_comm_sns_reg_info_init(ot_vi_pipe vi_pipe, ot_isp_sns_state *sns_state)
{
    printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    td_u32 i;
    sns_state->regs_info[0].sns_type = OT_ISP_SNS_TYPE_I2C;
    sns_state->regs_info[0].com_bus.i2c_dev = g_isr2006_bus_info[vi_pipe].i2c_dev;
    sns_state->regs_info[0].cfg2_valid_delay_max = 2; /* delay_max 2 */
    sns_state->regs_info[0].reg_num = 21;             /* reg_num 11 */
#if ISR2006_TEST_PIC
    sns_state->regs_info[0].reg_num = 0;
#endif
    for (i = 0; i < sns_state->regs_info[0].reg_num; i++)
    {
        sns_state->regs_info[0].i2c_data[i].update = TD_TRUE;
        sns_state->regs_info[0].i2c_data[i].dev_addr = ISR2006_I2C_ADDR;
        sns_state->regs_info[0].i2c_data[i].addr_byte_num = ISR2006_ADDR_BYTE;
        sns_state->regs_info[0].i2c_data[i].data_byte_num = ISR2006_DATA_BYTE;
    }

    /* shutter related */

    sns_state->regs_info[0].i2c_data[0].delay_frame_num = 0;           /* index 0 */
    sns_state->regs_info[0].i2c_data[0].reg_addr = ISR2006_EXP_LINE_L; /* index 0 */
    sns_state->regs_info[0].i2c_data[1].delay_frame_num = 0;           /* index 1 */
    sns_state->regs_info[0].i2c_data[1].reg_addr = ISR2006_EXP_LINE_M; /* index 1 */
    sns_state->regs_info[0].i2c_data[2].delay_frame_num = 0;           /* index 2 */
    sns_state->regs_info[0].i2c_data[2].reg_addr = ISR2006_EXP_LINE_H; /* index 2 */
    /*控制曝光时间*/

    sns_state->regs_info[0].i2c_data[3].delay_frame_num = 0;              /* index 3 */
    sns_state->regs_info[0].i2c_data[3].reg_addr = ISR2006_AGAIN_L;       /* index 3 */
    sns_state->regs_info[0].i2c_data[4].delay_frame_num = 0;              /* index 4 */
    sns_state->regs_info[0].i2c_data[4].reg_addr = ISR2006_AGAIN_H;       /* index 4 */
    sns_state->regs_info[0].i2c_data[5].delay_frame_num = 0;              /* index 5 */
    sns_state->regs_info[0].i2c_data[5].reg_addr = ISR2006_AGAIN_VFTCTRL; /* index 5 */
    /*曝光模拟增益*/

    sns_state->regs_info[0].i2c_data[6].delay_frame_num = 0;        /* index 6 */
    sns_state->regs_info[0].i2c_data[6].reg_addr = ISR2006_DGAIN_L; /* index 6 */
    sns_state->regs_info[0].i2c_data[7].delay_frame_num = 0;        /* index 7 */
    sns_state->regs_info[0].i2c_data[7].reg_addr = ISR2006_DGAIN_M; /* index 7 */
    sns_state->regs_info[0].i2c_data[8].delay_frame_num = 0;        /* index 8 */
    sns_state->regs_info[0].i2c_data[8].reg_addr = ISR2006_DGAIN_H; /* index 8 */
    /*曝光数字增益*/

    sns_state->regs_info[0].i2c_data[9].delay_frame_num = 1;             /* index 9 */
    sns_state->regs_info[0].i2c_data[9].reg_addr = ISR2006_VMAX_ADDR_L;  /* index 9 */
    sns_state->regs_info[0].i2c_data[10].delay_frame_num = 1;            /* index 10 */
    sns_state->regs_info[0].i2c_data[10].reg_addr = ISR2006_VMAX_ADDR_H; /* index 10 */
    /* Vmax */

    sns_state->regs_info[0].i2c_data[11].delay_frame_num = 2;      /* index 11 */
    sns_state->regs_info[0].i2c_data[11].reg_addr = ISR2006_dcg_1; /* index 11 */
    sns_state->regs_info[0].i2c_data[12].delay_frame_num = 2;      /* index 12 */
    sns_state->regs_info[0].i2c_data[12].reg_addr = ISR2006_dcg_2; /* index 12 */
    sns_state->regs_info[0].i2c_data[13].delay_frame_num = 2;      /* index 13 */
    sns_state->regs_info[0].i2c_data[13].reg_addr = ISR2006_dcg_3; /* index 13 */
    sns_state->regs_info[0].i2c_data[14].delay_frame_num = 2;      /* index 14 */
    sns_state->regs_info[0].i2c_data[14].reg_addr = ISR2006_dcg_4; /* index 14 */
    /*dcg 模式设置*/

    sns_state->regs_info[0].i2c_data[15].delay_frame_num = 0;       /* index 15 */
    sns_state->regs_info[0].i2c_data[15].reg_addr = ISR2006_REGA_L; /* index 15 */
    sns_state->regs_info[0].i2c_data[16].delay_frame_num = 0;       /* index 16 */
    sns_state->regs_info[0].i2c_data[16].reg_addr = ISR2006_REGA_M; /* index 16 */
    sns_state->regs_info[0].i2c_data[17].delay_frame_num = 0;       /* index 17 */
    sns_state->regs_info[0].i2c_data[17].reg_addr = ISR2006_REGA_H; /* index 17 */
    // 帧消隐

    sns_state->regs_info[0].i2c_data[18].delay_frame_num = 0;      /* index 18 */
    sns_state->regs_info[0].i2c_data[18].reg_addr = ISR2006_PLL_1; /* index 18 */
    sns_state->regs_info[0].i2c_data[19].delay_frame_num = 0;      /* index 19 */
    sns_state->regs_info[0].i2c_data[19].reg_addr = ISR2006_PLL_2; /* index 19 */
    sns_state->regs_info[0].i2c_data[20].delay_frame_num = 0;      /* index 20 */
    sns_state->regs_info[0].i2c_data[20].reg_addr = ISR2006_PLL_3; /* index 20 */

    sns_state->regs_info[0].i2c_data[21].delay_frame_num = 0;          /* index 21 */
    sns_state->regs_info[0].i2c_data[21].reg_addr = ISR2006_HALF_GAIN; /* index 21 */

    sns_state->regs_info[0].i2c_data[22].delay_frame_num = 0;          /* index 22 */
    sns_state->regs_info[0].i2c_data[22].reg_addr = ISR2006_CLK_DIG_L; /* index 23 */
    sns_state->regs_info[0].i2c_data[23].delay_frame_num = 0;          /* index 23 */
    sns_state->regs_info[0].i2c_data[23].reg_addr = ISR2006_CLK_DIG_H; /* index 23 */
    return;
}

static td_void cmos_sns_reg_info_update(ot_vi_pipe vi_pipe, ot_isp_sns_state *sns_state)
{
    td_u32 i;
    ot_unused(vi_pipe);
    // printf("cmos_sns_reg_info_update\n");

    for (i = 0; i < sns_state->regs_info[0].reg_num; i++)
    {
        if (sns_state->regs_info[0].i2c_data[i].data ==
            sns_state->regs_info[1].i2c_data[i].data)
        {
            sns_state->regs_info[0].i2c_data[i].update = TD_FALSE;
        }
        else
        {
            sns_state->regs_info[0].i2c_data[i].update = TD_TRUE;
            isr2006_write_register(vi_pipe, sns_state->regs_info[0].i2c_data[i].reg_addr,
                                   sns_state->regs_info[0].i2c_data[i].data);
        }
    }

    return;
}

static td_s32 cmos_get_sns_regs_info(ot_vi_pipe vi_pipe, ot_isp_sns_regs_info *sns_regs_info)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ot_isp_sns_state *sns_state = TD_NULL;

    sns_check_pointer_return(sns_regs_info);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    if ((sns_state->sync_init == TD_FALSE) || (sns_regs_info->config == TD_FALSE))
    {
        cmos_comm_sns_reg_info_init(vi_pipe, sns_state);
        sns_state->sync_init = TD_TRUE;
    }
    else
    {
        cmos_sns_reg_info_update(vi_pipe, sns_state);
    }

    sns_regs_info->config = TD_FALSE;
    (td_void) memcpy_s(sns_regs_info, sizeof(ot_isp_sns_regs_info),
                       &sns_state->regs_info[0], sizeof(ot_isp_sns_regs_info));
    (td_void) memcpy_s(&sns_state->regs_info[1], sizeof(ot_isp_sns_regs_info),
                       &sns_state->regs_info[0], sizeof(ot_isp_sns_regs_info));
    sns_state->fl[1] = sns_state->fl[0];

    return TD_SUCCESS;
}

static td_void cmos_config_image_mode_param(ot_vi_pipe vi_pipe, td_u8 sns_image_mode,
                                            ot_isp_sns_state *sns_state)
{
    switch (sns_image_mode)
    {
    case ISR2006_SENSOR_1936_1280_30FPS_12BIT_LINEAR_MODE:
    case ISR2006_SENSOR_1920_1080_30FPS_14BIT_LINEAR_MODE:
        sns_state->fl_std = ISR2006_VMAX_30FPS_LINEAR;
        break;
    case ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE:
        sns_state->fl_std = ISR2006_VMAX_25FPS_LINEAR;
        // printf("fl_std == ISR2006_VMAX_25FPS_LINEAR\n");
        break;
    default:
        sns_state->fl_std = ISR2006_VMAX_25FPS_LINEAR;
        break;
    }
}

static td_s32 cmos_set_image_mode(ot_vi_pipe vi_pipe, const ot_isp_cmos_sns_image_mode *sns_image_mode)
{
    // //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    // td_u32 i;
    td_u8 image_mode;
    ot_isp_sns_state *sns_state = TD_NULL;
    sns_check_pointer_return(sns_image_mode);
    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_return(sns_state);

    image_mode = sns_state->img_mode;

    // #region
    // for (i = 0; i < ISR2006_MODE_BUTT; i++)
    // {
    //     if (sns_image_mode->fps <= g_isr2006_mode_tbl[i].max_fps &&
    //         sns_image_mode->width <= g_isr2006_mode_tbl[i].width &&
    //         sns_image_mode->height <= g_isr2006_mode_tbl[i].height &&
    //         sns_state->wdr_mode == g_isr2006_mode_tbl[i].wdr_mode)
    //     {
    //         image_mode = (isr2006_res_mode)i; // = 0
    //         break;
    //     }
    // }
    // if (i >= ISR2006_MODE_BUTT)
    // {
    //     isr2006_err_mode_print(sns_image_mode, sns_state);
    //     return TD_FAILURE;
    // }
    // #endregion

    if (sns_image_mode->fps <= 25)
    {
        image_mode = ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE;
        printf("image_mode = 25fps\n");
    }
    else if (sns_image_mode->fps <= 30)
    {
        image_mode = ISR2006_SENSOR_1936_1280_30FPS_12BIT_LINEAR_MODE;
        if (sns_image_mode->width == 1920)
        {
            image_mode = ISR2006_SENSOR_1920_1080_30FPS_14BIT_LINEAR_MODE;
        }
        printf("image_mode = 30fps\n");
    }
    else if (sns_image_mode->fps <= 60)
    {
        image_mode = ISR2006_SENSOR_1936_1280_60FPS_12BIT_LINEAR_MODE;
        printf("image_mode = 30fps\n");
    }
    else if (sns_image_mode->fps <= 120)
    {
        image_mode = ISR2006_SENSOR_1920_1080_120FPS_10BIT_LINEAR_MODE;
        printf("image_mode = 120fps\n");
    }

    // image_mode = ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE;
    // printf("image_mode = 25fps\n");

    if (image_mode >= ISR2006_MODE_BUTT)
    {
        isr2006_err_mode_print(sns_image_mode, sns_state);
        return TD_FAILURE;
    }

    g_isr2006_vmax_linear = ISR2006_VMAX_60FPS_LINEAR;
    // printf("g_isr2006_vmax_linear = %d\n",g_isr2006_vmax_linear);

    cmos_config_image_mode_param(vi_pipe, image_mode, sns_state);

    if ((sns_state->init == TD_TRUE) && (image_mode == sns_state->img_mode))
    {                                               // sns_state->init 在第一次配置时应当为 TD_FALSE 如不是说明进行过配置
        return OT_ISP_DO_NOT_NEED_SWITCH_IMAGEMODE; /* Don't need to switch image_mode */
    }
    /*进行配置 */
    sns_state->sync_init = TD_FALSE;
    sns_state->img_mode = image_mode;
    sns_state->fl[0] = sns_state->fl_std;
    sns_state->fl[1] = sns_state->fl[0];
    return TD_SUCCESS;
}

static td_void sensor_global_init(ot_vi_pipe vi_pipe)
{
    // //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    ot_isp_sns_state *sns_state = TD_NULL;

    isr2006_sensor_get_ctx(vi_pipe, sns_state);
    sns_check_pointer_void_return(sns_state);

    sns_state->init = TD_FALSE;
    sns_state->sync_init = TD_FALSE;
    sns_state->img_mode = ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE;
    sns_state->wdr_mode = OT_WDR_MODE_NONE;
    sns_state->fl_std = ISR2006_VMAX_25FPS_LINEAR; //
    sns_state->fl[0] = ISR2006_VMAX_25FPS_LINEAR;
    sns_state->fl[1] = ISR2006_VMAX_25FPS_LINEAR;

    (td_void) memset_s(&sns_state->regs_info[0], sizeof(ot_isp_sns_regs_info), 0, sizeof(ot_isp_sns_regs_info));
    (td_void) memset_s(&sns_state->regs_info[1], sizeof(ot_isp_sns_regs_info), 0, sizeof(ot_isp_sns_regs_info));
    return;
}

static td_s32 cmos_init_sensor_exp_function(ot_isp_sns_exp_func *sensor_exp_func)
{
    sns_check_pointer_return(sensor_exp_func);

    (td_void) memset_s(sensor_exp_func, sizeof(ot_isp_sns_exp_func), 0, sizeof(ot_isp_sns_exp_func));

    sensor_exp_func->pfn_cmos_sns_init = isr2006_init;
    sensor_exp_func->pfn_cmos_sns_exit = isr2006_exit;
    sensor_exp_func->pfn_cmos_sns_global_init = sensor_global_init;
    sensor_exp_func->pfn_cmos_set_image_mode = cmos_set_image_mode;
    sensor_exp_func->pfn_cmos_set_wdr_mode = cmos_set_wdr_mode;
    sensor_exp_func->pfn_cmos_get_isp_default = cmos_get_isp_default;
    sensor_exp_func->pfn_cmos_get_isp_black_level = cmos_get_isp_black_level;
    sensor_exp_func->pfn_cmos_get_blc_clamp_info = cmos_get_isp_blc_clamp_info;
    sensor_exp_func->pfn_cmos_set_pixel_detect = cmos_set_pixel_detect;
    sensor_exp_func->pfn_cmos_get_sns_reg_info = cmos_get_sns_regs_info;

    return TD_SUCCESS;
}

static td_s32 isr2006_set_bus_info(ot_vi_pipe vi_pipe, ot_isp_sns_commbus sns_bus_info)
{
    g_isr2006_bus_info[vi_pipe].i2c_dev = sns_bus_info.i2c_dev;

    return TD_SUCCESS;
}

static td_s32 sensor_ctx_init(ot_vi_pipe vi_pipe)
{
    ot_isp_sns_state *sns_state_ctx = TD_NULL;

    isr2006_sensor_get_ctx(vi_pipe, sns_state_ctx);

    if (sns_state_ctx == TD_NULL)
    {
        sns_state_ctx = (ot_isp_sns_state *)malloc(sizeof(ot_isp_sns_state));
        if (sns_state_ctx == TD_NULL)
        {
            isp_err_trace("Isp[%d] SnsCtx malloc memory failed!\n", vi_pipe);
            return OT_ERR_ISP_NOMEM;
        }
    }

    (td_void) memset_s(sns_state_ctx, sizeof(ot_isp_sns_state), 0, sizeof(ot_isp_sns_state));

    isr2006_sensor_set_ctx(vi_pipe, sns_state_ctx);

    return TD_SUCCESS;
}

static td_void sensor_ctx_exit(ot_vi_pipe vi_pipe)
{
    ot_isp_sns_state *sns_state_ctx = TD_NULL;

    isr2006_sensor_get_ctx(vi_pipe, sns_state_ctx);
    sns_free(sns_state_ctx);
    isr2006_sensor_reset_ctx(vi_pipe);
    return;
}

static td_s32 sensor_register_callback(ot_vi_pipe vi_pipe, ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib)
{
    //  printf("[%s]-%d: \n", __FUNCTION__, __LINE__);

    td_s32 ret;
    ot_isp_sns_register isp_register;
    ot_isp_ae_sensor_register ae_register;
    ot_isp_awb_sensor_register awb_register;
    ot_isp_sns_attr_info sns_attr_info;

    sns_check_pointer_return(ae_lib);
    sns_check_pointer_return(awb_lib);

    ret = sensor_ctx_init(vi_pipe);
    if (ret != TD_SUCCESS)
    {
        return TD_FAILURE;
    }

    sns_attr_info.sns_id = ISR2006_ID;
    ret = cmos_init_sensor_exp_function(&isp_register.sns_exp);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("cmos init exp function failed!\n");
        return TD_FAILURE;
    }
    ret = ot_mpi_isp_sensor_reg_callback(vi_pipe, &sns_attr_info, &isp_register);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("sensor register callback function failed!\n");
        return ret;
    }

    ret = cmos_init_ae_exp_function(&ae_register.sns_exp);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("cmos init ae exp function failed!\n");
        return TD_FAILURE;
    }
    ret = ot_mpi_ae_sensor_reg_callback(vi_pipe, ae_lib, &sns_attr_info, &ae_register);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("sensor register callback function to ae lib failed!\n");
        return ret;
    }

    ret = cmos_init_awb_exp_function(&awb_register.sns_exp);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("cmos init awb exp function failed!\n");
        return TD_FAILURE;
    }
    ret = ot_mpi_awb_sensor_reg_callback(vi_pipe, awb_lib, &sns_attr_info, &awb_register);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("sensor register callback function to awb lib failed!\n");
        return ret;
    }

    return TD_SUCCESS;
}

static td_s32 sensor_unregister_callback(ot_vi_pipe vi_pipe, ot_isp_3a_alg_lib *ae_lib, ot_isp_3a_alg_lib *awb_lib)
{
    td_s32 ret;

    sns_check_pointer_return(ae_lib);
    sns_check_pointer_return(awb_lib);

    ret = ot_mpi_isp_sensor_unreg_callback(vi_pipe, ISR2006_ID);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("sensor unregister callback function failed!\n");
        return ret;
    }

    ret = ot_mpi_ae_sensor_unreg_callback(vi_pipe, ae_lib, ISR2006_ID);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("sensor unregister callback function to ae lib failed!\n");
        return ret;
    }

    ret = ot_mpi_awb_sensor_unreg_callback(vi_pipe, awb_lib, ISR2006_ID);
    if (ret != TD_SUCCESS)
    {
        isp_err_trace("sensor unregister callback function to awb lib failed!\n");
        return ret;
    }

    sensor_ctx_exit(vi_pipe);
    return TD_SUCCESS;
}

static td_s32 sensor_set_init(ot_vi_pipe vi_pipe, ot_isp_init_attr *init_attr)
{
    sns_check_pointer_return(init_attr);

    g_init_exposure[vi_pipe] = init_attr->exposure;
    g_lines_per500ms[vi_pipe] = init_attr->lines_per500ms;
    g_init_wb_gain[vi_pipe][0] = init_attr->wb_r_gain; /* 0: rgain */
    g_init_wb_gain[vi_pipe][1] = init_attr->wb_g_gain; /* 1: ggain */
    g_init_wb_gain[vi_pipe][2] = init_attr->wb_b_gain; /* 2: bgain */
    g_sample_r_gain[vi_pipe] = init_attr->sample_r_gain;
    g_sample_b_gain[vi_pipe] = init_attr->sample_b_gain;
    g_quick_start_en[vi_pipe] = init_attr->quick_start_en;
    g_ae_stat_pos[vi_pipe] = init_attr->ae_stat_pos;
    g_ae_route_ex_valid[vi_pipe] = init_attr->ae_route_ex_valid;
    (td_void) memcpy_s(&g_init_ae_route[vi_pipe], sizeof(ot_isp_ae_route),
                       &init_attr->ae_route, sizeof(ot_isp_ae_route));
    (td_void) memcpy_s(&g_init_ae_route_ex[vi_pipe], sizeof(ot_isp_ae_route_ex),
                       &init_attr->ae_route_ex, sizeof(ot_isp_ae_route_ex));
    (td_void) memcpy_s(&g_init_ae_route_sf[vi_pipe], sizeof(ot_isp_ae_route),
                       &init_attr->ae_route_sf, sizeof(ot_isp_ae_route));
    (td_void) memcpy_s(&g_init_ae_route_sf_ex[vi_pipe], sizeof(ot_isp_ae_route_ex),
                       &init_attr->ae_route_sf_ex, sizeof(ot_isp_ae_route_ex));

    return TD_SUCCESS;
}

ot_isp_sns_obj g_sns_isr2006_obj = {
    .pfn_register_callback = sensor_register_callback,
    .pfn_un_register_callback = sensor_unregister_callback,
    .pfn_standby = isr2006_standby,
    .pfn_restart = isr2006_restart,
    .pfn_mirror_flip = TD_NULL,
    .pfn_set_blc_clamp = isr2006_blc_clamp, // 黑电平开启
    .pfn_write_reg = isr2006_write_register,
    .pfn_read_reg = isr2006_read_register,
    .pfn_set_bus_info = isr2006_set_bus_info,
    .pfn_set_init = sensor_set_init};
