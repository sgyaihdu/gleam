/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "sample_comm.h"
#include "ot_i2c.h"

#define PRINTF_DEBUG                                                       \
    {                                                                      \
        printf("file:%s func:%s line:%d\n", __FILE__, __func__, __LINE__); \
    }

#define SAMPLE_VO_VB_NUM 8
#define SAMPLE_VO_DEV_FRAME_RATE 25
static volatile sig_atomic_t g_vo_sig_flag = 0;

// #define L2414_ENABLE
#define HJ_ENABLE

#define USLEEP_6000 6000
#define USLEEP_50 50
#define USLEEP_100000 100000
#define USLEEP_60000 60000
#define CMD_COUNT_1200X1920 304
// #define BOE071_PATERN_TEST

#ifndef BOE071_PATERN_TEST
#define HJ_I2C_CMD_COUNT_1080P (61)
#else
#define HJ_I2C_CMD_COUNT_1080P 40
#endif

#define HJ_I2C_CMD_COUNT_720P (59)

#define L2414_I2C_CMD_COUNT 8

#define I2C_OLEDA_BUS_NUM 5

#define I2C_OLEDB_BUS_NUM 7

#define HJ_I2C_DEV_ADDR ((0x98) >> 1)

#define L2414_I2C_BUS_NUM 6
#define L2414_I2C_DEV_ADDR ((0x18) >> 1)
#define L2414_I2C_CMD_COUNT_1080P 0

#define LT9211_I2C_BUS_NUM 7
#define LT9211_I2C_DEV_ADDR ((0x5A) >> 1)

#if 1

hi_u8 cmd_1[2] = {0x51, 0xff, 0x00};
hi_u8 cmd_2[5] = {0x80, 0x01, 0xe0, 0x0e, 0x11};
hi_u8 cmd_3[8] = {0x81, 0x02, 0x20, 0x00, 0x0c, 0x00, 0x08, 0x00};
hi_u8 cmd_4[8] = {0x82, 0x02, 0x20, 0x00, 0x08, 0x00, 0x10, 0x01};
hi_u8 cmd_5[3] = {0xff, 0x5a, 0x81};
hi_u8 cmd_6[0x12] = {0xf9, 0x31, 0x7f, 0x82, 0x85, 0x88, 0x8b, 0x8e, 0x91, 0x94, 0x97, 0x9a, 0x9d, 0xa0, 0xa3, 0xa6, 0xa9, 0xac};
hi_u8 cmd_7[3] = {0xf0, 0xaa, 0x11};
hi_u8 cmd_8[0x0a] = {0xc2, 0x00, 0x00, 0x02, 0x18, 0x02, 0x18, 0x00, 0x90, 0xc2};

#define MIPI_CMD_COUNT 18

static mipi_tx_cmd_info g_cmd_info_1920x1080[MIPI_CMD_COUNT] = {
    /* {devno work_mode lp_clk_en data_type cmd_size cmd}, usleep_value */
    {{0, 0, 0, 0x39, 0x0300, NULL}, USLEEP_6000},
    {{0, 0, 0, 0x39, 0x5320, NULL}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x0003, cmd_1}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x6b00, NULL}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x0005, cmd_2}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x0008, cmd_3}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x0008, cmd_4}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x3500, NULL}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x0003, cmd_5}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x6514, NULL}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x0012, cmd_6}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x2620, NULL}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x0003, cmd_7}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x000a, cmd_8}, USLEEP_50},
    {{0, 0, 0, 0x39, 0xd117, NULL}, USLEEP_50},
    {{0, 0, 0, 0x39, 0xd511, NULL}, USLEEP_50},
    {{0, 0, 0, 0x05, 0x1100, NULL}, USLEEP_50},
    {{0, 0, 0, 0x39, 0x2900, NULL}, USLEEP_50},
};
#endif

static mipi_tx_i2c_cmd_info g_i2c_cmd_info_boe701_720p60[HJ_I2C_CMD_COUNT_720P] =
    {
        {0x03, 0x00, 0x00, 0x00},
        {0x53, 0x00, 0x00, 0x20},
        {0x51, 0x00, 0x00, 0xff},
        {0x51, 0x01, 0x01, 0x00},
        {0x6b, 0x00, 0x00, 0x00},
        {0x80, 0x00, 0x00, 0x01},
        {0x80, 0x01, 0x00, 0x40},
        {0x80, 0x02, 0x00, 0x00},
        {0x80, 0x03, 0x00, 0x11},
        {0x81, 0x00, 0x00, 0x02},
        {0x81, 0x01, 0x00, 0x20},
        {0x81, 0x02, 0x00, 0x00},
        {0x81, 0x03, 0x00, 0x0c},
        {0x81, 0x04, 0x00, 0x00},
        {0x81, 0x05, 0x00, 0x08},
        {0x81, 0x06, 0x00, 0x00},
        {0x82, 0x00, 0x00, 0x02},
        {0x82, 0x01, 0x00, 0x20},
        {0x82, 0x02, 0x00, 0x00},
        {0x82, 0x03, 0x00, 0x08},
        {0x82, 0x04, 0x00, 0x00},
        {0x82, 0x05, 0x00, 0x10},
        {0x82, 0x06, 0x00, 0x01},
        {0x35, 0x00, 0x00, 0x00},
        {0x2a, 0x00, 0x00, 0x01},
        {0x2a, 0x01, 0x00, 0x40},
        {0x2a, 0x02, 0x00, 0x06},
        {0x2a, 0x03, 0x00, 0x40},
        {0x2b, 0x00, 0x00, 0x00},
        {0x2b, 0x01, 0x00, 0x1c},
        {0x2b, 0x02, 0x00, 0x04},
        {0x2b, 0x03, 0x00, 0x1c},
        {0xff, 0x00, 0x00, 0x5a},
        {0xff, 0x01, 0x00, 0x81},
        {0x65, 0x00, 0x00, 0x14},
        {0xf9, 0x00, 0x00, 0x31},
        {0xf9, 0x01, 0x00, 0x7f},
        {0xf9, 0x02, 0x00, 0x82},
        {0xf9, 0x03, 0x00, 0x85},
        {0xf9, 0x04, 0x00, 0x88},
        {0xf9, 0x05, 0x00, 0x8b},
        {0xf9, 0x06, 0x00, 0x8e},
        {0xf9, 0x07, 0x00, 0x91},
        {0xf9, 0x08, 0x00, 0x94},
        {0xf9, 0x09, 0x00, 0x97},
        {0xf9, 0x10, 0x00, 0x9a},
        {0xf9, 0x11, 0x00, 0x9d},
        {0xf9, 0x12, 0x00, 0xa0},
        {0xf9, 0x13, 0x00, 0xa3},
        {0xf9, 0x14, 0x00, 0xa6},
        {0xf9, 0x15, 0x00, 0xa9},
        {0xf9, 0x16, 0x00, 0xac},
        {0x26, 0x00, 0x00, 0x20},
        {0xf0, 0x00, 0x00, 0xaa},
        {0xf0, 0x00, 0x00, 0x13},
        {0xd1, 0x00, 0x00, 0x17},
        {0xd5, 0x00, 0x00, 0x11},
        {0x11, 0x00, HI_TRUE, 0x00},
        {0x29, 0x00, HI_TRUE, 0x00},
};

// #region

// static mipi_tx_i2c_cmd_info g_i2c_cmd_info_boe701_1080p60[HJ_I2C_CMD_COUNT_1080P] = {
//     // 高位地址 低位地址 延迟指示 寄存器值  如果2和3 同时是REG_NULL表示写0 如果只有2是REG_NULL表示写后延迟100ms
//     {0x53, 0x00, 0x00, 0x28},
//     {0x51, 0x00, 0x00, 0xFF},
//     {0x51, 0x01, 0x00, 0x01},
//     {0x80, 0x00, 0x00, 0x01},
//     {0x80, 0x01, 0x00, 0xF0},
//     {0x80, 0x02, 0x00, 0x87},
//     // 1920*1080@60 137.28MHz
//     // video_timing video ={56,   32,     72,     1920,   2080,   8,      4,      8,      1080,   1100,   137280};
//     ////hfp,  hs,     hbp,    hact,   htotal, vfp,    vs,     vbp,    vact,   vtotal, pclk

//     {0x81, 0x00, 0x00, 0x03},
//     {0x81, 0x01, 0x00, 0x28},
//     {0x81, 0x02, 0x00, 0x00},
//     {0x81, 0x03, 0x00, 0x0C},
//     {0x81, 0x04, 0x00, 0x00},
//     {0x81, 0x05, 0x00, 0x08},
//     {0x81, 0x06, 0x00, 0x00},
//     {0x82, 0x00, 0x00, 0x03},
//     {0x82, 0x01, 0x00, 0x28},
//     {0x82, 0x02, 0x00, 0x00},
//     {0x82, 0x03, 0x00, 0x0C},
//     {0x82, 0x04, 0x00, 0x00},
//     {0x82, 0x05, 0x00, 0x08},
//     {0x82, 0x06, 0x00, 0x00},
//     {0x35, 0x00, 0x00, 0x00},
//     {0xFF, 0x00, 0x00, 0x5A},
//     {0xFF, 0x01, 0x00, 0x80},
//     {0x65, 0x00, 0x00, 0x12},
//     {0xF9, 0x12, 0x00, 0x1E},
//     {0xFF, 0x00, 0x00, 0x5A},
//     {0xFF, 0x01, 0x00, 0x81},
//     {0xF4, 0x00, 0x00, 0x0C},

// {0x69,0x00,0x00,0x03},

//     {0x25, 0x00, HI_TRUE, 0x01},
//     {0x26, 0x00, HI_TRUE, 0x01},
// #ifndef BOE071_PATERN_TEST
//     {0x11, 0x00, HI_TRUE, 0x00}, // Sleep-out 0x11
//     {0x29, 0x00, HI_TRUE, 0x00}, // Display-On 0x29
// #else
//     {0xF0, 0x00, 0x00, 0xAA},
//     {0xF0, 0x01, 0x00, 0x11},
//     {0xC4, 0x00, 0x00, 0xAA},
//     {0xC4, 0x01, 0x00, 0x55},
//     {0xC4, 0x02, 0x00, 0x01},
//     {0xC4, 0x03, 0x00, 0x80},
//     {0xC5, 0x00, 0x00, 0x10},
//     {0xC5, 0x01, 0x00, 0xFF},
//     {0xC5, 0x02, 0x00, 0xFF},
// #endif
//     {0x0A, 0x00, 0x00, 0x00}, // return 0x9c if everything goes well

// };

// #endregion





static mipi_tx_i2c_cmd_info g_i2c_cmd_info_boe701_1080p60[HJ_I2C_CMD_COUNT_1080P] = {
    // 高位地址 低位地址 延迟指示 寄存器值  如果2和3 同时是REG_NULL表示写0 如果只有2是REG_NULL表示写后延迟100ms
    {0x03, 0x00, 0x00, 0x00},
    {0x53, 0x00, 0x00, 0x20},
    {0x51, 0x00, 0x00, 0xFF},
    {0x51, 0x01, 0x00, 0x00},
    {0x80, 0x00, 0x00, 0x01},
    {0x80, 0x01, 0x00, 0xe0},
    {0x80, 0x02, 0x00, 0x0e},
    {0x80, 0x03, 0x00, 0x11},
    // 1920*1080@60 137.28MHz
    // video_timing video ={56,   32,     72,     1920,   2080,   8,      4,      8,      1080,   1100,   137280};
    ////hfp,  hs,     hbp,    hact,   htotal, vfp,    vs,     vbp,    vact,   vtotal, pclk

    {0x81, 0x00, 0x00, 0x02},
    {0x81, 0x01, 0x00, 0x20},
    {0x81, 0x02, 0x00, 0x00},
    {0x81, 0x03, 0x00, 0x0C},
    {0x81, 0x04, 0x00, 0x00},
    {0x81, 0x05, 0x00, 0x08},
    {0x81, 0x06, 0x00, 0x00},
    {0x82, 0x00, 0x00, 0x02},
    {0x82, 0x01, 0x00, 0x20},
    {0x82, 0x02, 0x00, 0x00},
    {0x82, 0x03, 0x00, 0x08},
    {0x82, 0x04, 0x00, 0x00},
    {0x82, 0x05, 0x00, 0x10},
    {0x82, 0x06, 0x00, 0x01},
    {0x35, 0x00, 0x00, 0x00},
    {0xFF, 0x00, 0x00, 0x5A},
    {0xFF, 0x01, 0x00, 0x81},
    {0x65, 0x00, 0x00, 0x14},
    {0xF9, 0x00, 0x00, 0x31},
    {0xF9, 0x01, 0x00, 0x7f},
    {0xf9, 0x02, 0x00, 0x82},
    {0xf9, 0x03, 0x00, 0x85},
    {0xf9, 0x04, 0x00, 0x88},
    {0xf9, 0x05, 0x00, 0x8b},
    {0xf9, 0x06, 0x00, 0x8e},
    {0xf9, 0x07, 0x00, 0x91},
    {0xf9, 0x08, 0x00, 0x94},
    {0xf9, 0x09, 0x00, 0x97},
    {0xf9, 0x0a, 0x00, 0x9a},
    {0xf9, 0x0b, 0x00, 0x9d},
    {0xf9, 0x0c, 0x00, 0xa0},
    {0xf9, 0x0d, 0x00, 0xa3},
    {0xf9, 0x0e, 0x00, 0xa6},
    {0xf9, 0x0f, 0x00, 0xa9},
    {0xf9, 0x10, 0x00, 0xac},
    {0x26, 0x00, 0x00, 0x20},
    {0xf0, 0x00, 0x00, 0xaa},
    {0xf0, 0x01, 0x00, 0x11},
    {0x0a,0x00,0x00,0xc2},
    {0x0a,0x01,0x00,0x00},
    {0x0a,0x02,0x00,0x00},
    {0x0a,0x03,0x00,0x02},
    {0x0a,0x04,0x00,0x18},
    {0x0a,0x05,0x00,0x02},
    {0x0a,0x06,0x00,0x18},
    {0x0a,0x07,0x00,0x00},
    {0x0a,0x08,0x00,0x90},
    {0x0a,0x09,0x00,0xc2},
    {0xd1,0x00,0x00,0x17},
    {0xd5,0x00,0x00,0x11},


//     {0xFF, 0x00, 0x00, 0x5A},
//     {0xFF, 0x01, 0x00, 0x81},
//     {0xF4, 0x00, 0x00, 0x0C},

// {0x69,0x00,0x00,0x03},

//     {0x25, 0x00, HI_TRUE, 0x01},
//     {0x26, 0x00, HI_TRUE, 0x01},
#ifndef BOE071_PATERN_TEST
    {0x11, 0x00, HI_TRUE, 0x00}, // Sleep-out 0x11
    {0x29, 0x00, HI_TRUE, 0x00}, // Display-On 0x29
#else
    {0xF0, 0x00, 0x00, 0xAA},
    {0xF0, 0x01, 0x00, 0x11},
    {0xC4, 0x00, 0x00, 0xAA},
    {0xC4, 0x01, 0x00, 0x55},
    {0xC4, 0x02, 0x00, 0x01},
    {0xC4, 0x03, 0x00, 0x80},
    {0xC5, 0x00, 0x00, 0x10},
    {0xC5, 0x01, 0x00, 0xFF},
    {0xC5, 0x02, 0x00, 0xFF},
#endif
    {0x0A, 0x00, 0x00, 0x00}, // return 0x9c if everything goes well

};









static mipi_tx_i2c_cmd_info g_i2c_cmd_info_l2414_1024x768[L2414_I2C_CMD_COUNT] = {
    {0x00, 0x80, 0x00, 0x11},
    {0x00, 0x01, 0x00, 0x03},
    {0x00, 0x86, 0x00, 0x01},
    {0x00, 0x87, 0x00, 0x00},
    {0x00, 0x88, 0x00, 0x40},
    {0x00, 0x89, 0x00, 0x00},
    {0x00, 0xA0, 0x00, 0x20},
    {0x00, 0x30, 0x00, 0x00}};

// static mipi_tx_i2c_cmd_info g_i2c_cmd_info_l2414_1024x768[L2414_I2C_CMD_COUNT] = {
//     //addr , value
//     {0x80, 0x11},
//     {0x01, 0x01},
//     {0x86, 0x01},
//     {0x87, 0x00},
//     {0x88, 0x40},
//     {0x89, 0x00},
//     {0xA0, 0x20},
//     {0x30, 0x00}
// };

static const sample_vo_cfg g_vo_rgb_8bit_config = {
    .vo_dev = SAMPLE_VO_DEV_UHD,
    .vo_layer = SAMPLE_VO_LAYER_VHD0,
    .vo_intf_type = HI_VO_INTF_RGB_8BIT,
    .intf_sync = HI_VO_OUT_USER,
    .bg_color = COLOR_RGB_BLACK,
    .pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect = {0, 0, 320, 240},
    .image_size = {320, 240},
    .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len = 3, /* 3: def buf len for single */
    .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
    .vo_mode = VO_MODE_1MUX,
    .compress_mode = HI_COMPRESS_MODE_NONE,

    .sync_info = {0, 1, 1, 240, 21, 6, 320, 71, 110, 1, 240, 14, 9, 10, 2, 0, 1, 0},
    .user_sync = {
        .auto_user_sync_info.pixel_clk = 5400000,
        .clk_reverse_en = HI_TRUE,
        .op_mode = HI_OP_MODE_AUTO,
    },
    .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE,
    .vo_sharpen = {
        .enable = HI_TRUE,
        .peak_ratio = 255,
    },
    .hor_split_en = HI_FALSE,
};
#if 0
/* VO: USER 1200x1920_60, TX: USER 1200x1920 */
static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_1200x1920_user = {
    .vo_config = {
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_MIPI,
        .intf_sync = HI_VO_OUT_USER,
        .bg_color = COLOR_RGB_BLACK,
        .pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .disp_rect = {0, 0, 1200, 1920},
        .image_size = {1200, 1920},
        .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE,
        .dis_buf_len = 3, /* 3: def buf len for single */
        .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
        .vo_mode = VO_MODE_1MUX,
        .compress_mode = HI_COMPRESS_MODE_NONE,

        .sync_info = {0, 1, 1, 1920, 12, 15, 1200, 104, 60, 1, 540, 9, 7, 24, 2, 0, 0, 0},
        .user_sync = {
            .manual_user_sync_info.user_sync_attr = {
                .clk_src = HI_VO_CLK_SRC_PLL,
                .vo_pll = { /* if mipitx, set it by pixel clk and div mode */
                    .fb_div = 80, /* 80 fb div */
                    .frac = 0,
                    .ref_div = 1, /* 1 ref div */
                    .post_div1 = 6, /* 6 post div1 */
                    .post_div2 = 2, /* 2 post div2 */
                },
            },
            .manual_user_sync_info.pre_div = 1, /* if mipitx, set it by pixel clk */
            .manual_user_sync_info.dev_div = 1, /* if rgb, set it by serial mode */
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_MANUAL,
        },
        .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE,
    },
    .tx_config = {
        /* for combo dev config */
        .intf_sync = HI_MIPI_TX_OUT_USER,

        /* for screen cmd */
        .cmd_count = CMD_COUNT_1200X1920,
        .cmd_info = g_cmd_info_1200x1920,

        /* for user sync */
        .combo_dev_cfg = {
            .devno = 0,
            .lane_id = {0, 1, 2, 3},
            .out_mode = OUT_MODE_DSI_VIDEO,
            .out_format = OUT_FORMAT_RGB_24BIT,
            .video_mode =  BURST_MODE,
            .sync_info = {
                .hpw = 24, /* 24 pixel */
                .hbp = 80, /* 80 pixel */
                .hact = 1200, /* 1200 pixel */
                .hfp = 60, /* 60 pixel */
                .vpw = 2, /* 2 line */
                .vbp = 10, /* 10 line */
                .vact = 1920, /* 1920 line */
                .vfp = 15, /* 15 line */
            },
            .phy_data_rate = 999, /* 999 Mbps */
            .pixel_clk = 160000, /* 160000 KHz */
        },
    },
};
#endif
#if 0
static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_1920x1080_user = {
    .vo_config = {
/* for device */
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_MIPI ,
        .intf_sync = HI_VO_OUT_USER,//输出时序
        //.pic_size = PIC_1080P,
        .bg_color = COLOR_RGB_BLACK,//背景颜色
/* for layer */
        .pix_format = OT_PIXEL_FORMAT_YVU_PLANAR_420,
        .disp_rect = {0, 0, 1920, 1080},//显示的区域
        .image_size = {1920,1080},//
        .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE,//分区模式：不分区
        .compress_mode = HI_COMPRESS_MODE_NONE,//压缩模式：不压缩

        .dis_buf_len = 3, /* 3: def buf len for single */ //缓冲区数量
        .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,//动态范围
/* for channel */
        .vo_mode = VO_MODE_1MUX,
/* for user sync */
        // .sync_info = {
        //     .syncm = 0,
        //     .iop = 1,
        //     .intfb = 0,
        //     .vact = 1080,
        //     .vbb = (8+4),
        //     .vfb = 8,
        //     .hact = 1920,
        //     .hbb = (72+32),
        //     .hfb = 56,
        //     .hmid = 1,
        //     .bvact = 1,
        //     .bvbb = 1,
        //     .bvfb = 1,
        //     .hpw = 32,
        //     .vpw = 4,
        //     .idv = 0,
        //     .ihs = 0,
        //     .ivs = 0,
        // },
        .sync_info = {0, 0, 1, 1080, (8+4), 8, 1920, (72+32), 56, 1, 540, 9, 7, 32, 4, 0, 0, 0},
      //.sync_info = {0, 1, 1, 1920, 12, 15, 1200, 104, 60, 1, 540, 9, 7, 24, 2, 0, 0, 0},
        .user_sync = {
            // .manual_user_sync_info.user_sync_attr = {
                
            //     .clk_src = HI_VO_CLK_SRC_PLL,
            //     .vo_pll = { /* if mipitx, set it by pixel clk and div mode */
            //         .fb_div = 80, /* 80 fb div */
            //         .frac = 0,
            //         .ref_div = 1, /* 1 ref div */
            //         .post_div1 = 6, /* 6 post div1 */
            //         .post_div2 = 2, /* 2 post div2 */
            //     },
            // },
            // .manual_user_sync_info.pre_div = 1, /* if mipitx, set it by pixel clk */
            // .manual_user_sync_info.dev_div = 1, /* if rgb, set it by serial mode */
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_AUTO,
            // .auto_user_sync_info.pixel_clk = 137280000*2,
            .auto_user_sync_info.pixel_clk = 137280000,
        },
        .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE,//设备帧率 60
    },
    .tx_config = {
        /* for combo dev config */
        .intf_sync = HI_MIPI_TX_OUT_USER,

        /* for screen cmd */
        // .cmd_count = CMD_COUNT_1080P,
        // .cmd_info = g_cmd_info_1200x1920,
        .i2c_cmd_en = HI_TRUE,
        .i2c_bus = I2C_BUS_NUM,
        .i2c_addr = I2C_DEV_ADDR,//不含读写位
        .i2c_cmd_count = I2C_CMD_COUNT_1080P,
        .i2c_cmd_info = g_i2c_cmd_info_boe701_1080p60,
        .i2c_lt9211_bus = LT9211_I2C_BUS_NUM,
        .i2c_lt9211_addr = LT9211_I2C_DEV_ADDR,//不含读写位

        /* for user sync */
        .combo_dev_cfg = {
            .devno = 0,
            .lane_id = {0, 1, 2, 3},
            .out_mode = OUT_MODE_DSI_VIDEO,
            .out_format = OUT_FORMAT_RGB_24BIT,
            .video_mode =  BURST_MODE,
            .sync_info = {
                .hpw = 32, /* 32 pixel */
                .hbp = 72, /* 72 pixel */
                .hact = 1920, /* 1920 pixel */
                .hfp = 56, /* 56 pixel */
                .vpw = 4, /* 4 line */
                .vbp = 8, /* 8 line */
                .vact = 1080, /* 1080 line */
                .vfp = 8, /* 8 line */
            },
            .phy_data_rate = 344, /* 999 Mbps */
            .pixel_clk = 57200, /* 137280 KHz */
            .clklane_continue_mode = MIPI_TX_CLK_LANE_NON_CONTINUE,
        },
    },
};
#endif
#if 1
static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_1920x1080_user = {
    .vo_config = {
        /* for device */
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_MIPI,
        .intf_sync = HI_VO_OUT_USER, // 输出时序
        //.pic_size = PIC_1080P,
        .bg_color = COLOR_RGB_BLACK, // 背景颜色
        /* for layer */
        .pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
        .disp_rect = {0, 0, 1920, 1080},             // 显示的区域
        .image_size = {1920, 1080},                  /*图像分辨率，图像从输出通道进入后的合成画面尺寸，
                           具体可以看MPP 媒体处理软件 V6.0 开发参考P545~p546*/
        .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE, // 分区模式：不分区
        .compress_mode = HI_COMPRESS_MODE_NONE,      // 压缩模式：不压缩

        .dis_buf_len = 3,
        /* 3: def buf len for single */             // 缓冲区数量
        .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8, // 动态范围
        /* for channel */
        .vo_mode = VO_MODE_1MUX,
        /* for user sync */
        // .sync_info = {
        //     .syncm = 0,
        //     .iop = 1,
        //     .intfb = 0,
        //     .vact = 1080,
        //     .vbb = (8 + 4),
        //     .vfb = 8,
        //     .hact = 1920,
        //     .hbb = (72 + 32),
        //     .hfb = 56,
        //     .hmid = 1,
        //     .bvact = 1,
        //     .bvbb = 1,
        //     .bvfb = 1,
        //     .hpw = 32,
        //     .vpw = 4,
        //     .idv = 0,
        //     .ihs = 0,
        //     .ivs = 0,
        // },
        .sync_info = {
            .syncm = 0,
            .iop = 1,
            .intfb = 0,
            .vact = 1080,
            .vbb = (8 + 4),
            .vfb = 8,
            .hact = 1920,
            .hbb = (72 + 32),
            .hfb = 56,
            .hmid = 1,
            .bvact = 1,
            .bvbb = 1,
            .bvfb = 1,
            .hpw = 32,
            .vpw = 4,
            .idv = 0,
            .ihs = 0,
            .ivs = 0,
        },
        //.sync_info = {0, 0, 1, 1080, (8+4), 8, 1920, (72+32), 56, 1, 540, 9, 7, 32, 4, 0, 0, 0},
        //.sync_info = {0, 1, 1, 1920, 12, 15, 1200, 104, 60, 1, 540, 9, 7, 24, 2, 0, 0, 0},
        .user_sync = {
#if 0
            .manual_user_sync_info.user_sync_attr = {
                
                .clk_src = HI_VO_CLK_SRC_PLL,
                .vo_pll = { /* if mipitx, set it by pixel clk and div mode */
                    .fb_div = 80, /* 80 fb div */
                    .frac = 0,
                    .ref_div = 1, /* 1 ref div */
                    .post_div1 = 6, /* 6 post div1 */
                    .post_div2 = 2, /* 2 post div2 */
                },
            },
            .manual_user_sync_info.pre_div = 1, /* if mipitx, set it by pixel clk */
            .manual_user_sync_info.dev_div = 1, /* if rgb, set it by serial mode */
#endif
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_AUTO,
            .auto_user_sync_info.pixel_clk = 57200 * 1000,
        },
        .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE, // 设备帧率 60
    },
    .tx_config = {
        /* for combo dev config */
        .intf_sync = HI_MIPI_TX_OUT_USER,

        // .cmd_count = MIPI_CMD_COUNT,
        // .cmd_info = g_cmd_info_1920x1080,
        /* for screen cmd */
        .i2c_cmd_en = HI_TRUE,
        .oled_num = 2,
        .i2c_bus = {I2C_OLEDA_BUS_NUM, I2C_OLEDB_BUS_NUM},
        .i2c_addr = {HJ_I2C_DEV_ADDR, HJ_I2C_DEV_ADDR}, // 不含读写位
        .i2c_cmd_count = {HJ_I2C_CMD_COUNT_1080P, HJ_I2C_CMD_COUNT_1080P},
        .i2c_cmd_info = {g_i2c_cmd_info_boe701_1080p60, g_i2c_cmd_info_boe701_1080p60},
        .addr_len = 2,
        .data_len = 1,
        .i2c_lt9211_enable = HI_FALSE,
        .i2c_lt9211_bus = LT9211_I2C_BUS_NUM,
        .i2c_lt9211_addr = LT9211_I2C_DEV_ADDR, // 不含读写位

        /* for user sync */
        .combo_dev_cfg = {
            .devno = 0,
            .lane_id = {0, 1, 2, 3},
            .out_mode = OUT_MODE_DSI_VIDEO,
            .out_format = OUT_FORMAT_RGB_24BIT,
            .video_mode = BURST_MODE,
            .sync_info = {
                .hpw = 32,    /* 32 pixel */
                .hbp = 72,    /* 72 pixel */
                .hact = 1920, /* 1920 pixel */
                .hfp = 56,    /* 56 pixel */
                .vpw = 4,     /* 4 line */
                .vbp = 8,     /* 8 line */
                .vact = 1080, /* 1080 line */
                .vfp = 8,     /* 8 line */
            },
            .phy_data_rate = 344, /* 999 Mbps */
            .pixel_clk = 57200,   /* 137280 KHz */
            .clklane_continue_mode = MIPI_TX_CLK_LANE_NON_CONTINUE,
        },
    },
};

// #region7
#if 0
static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_1920x1080_user = {
    .vo_config = {
        /* for device */
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_BT1120,
        .intf_sync = HI_VO_OUT_USER, // 输出时序
        //.pic_size = PIC_1080P,
        .bg_color = COLOR_RGB_BLACK, // 背景颜色
        /* for layer */
        .pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
        .disp_rect = {0, 0, 3840, 1080},             // 显示的区域
        .image_size = {3840, 1080},                  /*图像分辨率，图像从输出通道进入后的合成画面尺寸，
                           具体可以看MPP 媒体处理软件 V6.0 开发参考P545~p546*/
        .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE, // 分区模式：不分区
        .compress_mode = HI_COMPRESS_MODE_NONE,      // 压缩模式：不压缩

        .dis_buf_len = 3,
        /* 3: def buf len for single */             // 缓冲区数量
        .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8, // 动态范围
        /* for channel */
        .vo_mode = VO_MODE_1MUX,
        /* for user sync */
        .sync_info = {
            .syncm = 0,
            .iop = 1,
            .intfb = 0,
            .vact = 1080,
            .vbb = (36 + 5),
            .vfb = 4,
            .hact = 3840,
            .hbb = (384),
            .hfb = 1056,
            .hmid = 1,
            .bvact = 1,
            .bvbb = 1,
            .bvfb = 1,
            .hpw = 88,
            .vpw = 5,
            .idv = 0,
            .ihs = 0,
            .ivs = 0,
        },
        //.sync_info = {0, 0, 1, 1080, (8+4), 8, 1920, (72+32), 56, 1, 540, 9, 7, 32, 4, 0, 0, 0},
        //.sync_info = {0, 1, 1, 1920, 12, 15, 1200, 104, 60, 1, 540, 9, 7, 24, 2, 0, 0, 0},
        .user_sync = {
#if 0
            .manual_user_sync_info.user_sync_attr = {
                
                .clk_src = HI_VO_CLK_SRC_PLL,
                .vo_pll = { /* if mipitx, set it by pixel clk and div mode */
                    .fb_div = 80, /* 80 fb div */
                    .frac = 0,
                    .ref_div = 1, /* 1 ref div */
                    .post_div1 = 6, /* 6 post div1 */
                    .post_div2 = 2, /* 2 post div2 */
                },
            },
            .manual_user_sync_info.pre_div = 1, /* if mipitx, set it by pixel clk */
            .manual_user_sync_info.dev_div = 1, /* if rgb, set it by serial mode */
#endif
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_AUTO,
            .auto_user_sync_info.pixel_clk = 148500000,
        },
        .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE, // 设备帧率 60
    },
    .tx_config = {
        /* for combo dev config */
        .intf_sync = HI_MIPI_TX_OUT_USER,

        /* for screen cmd */
        .i2c_cmd_en = HI_TRUE,
        .oled_num = 2,
        .i2c_bus = {I2C_OLEDA_BUS_NUM, I2C_OLEDB_BUS_NUM},
        .i2c_addr = {HJ_I2C_DEV_ADDR, HJ_I2C_DEV_ADDR}, // 不含读写位
        .i2c_cmd_count = {HJ_I2C_CMD_COUNT_1080P, HJ_I2C_CMD_COUNT_1080P},
        .i2c_cmd_info = {g_i2c_cmd_info_boe701_1080p60, g_i2c_cmd_info_boe701_1080p60},
        .addr_len = 2,
        .data_len = 1,
        .i2c_lt9211_enable = HI_FALSE,
        .i2c_lt9211_bus = LT9211_I2C_BUS_NUM,
        .i2c_lt9211_addr = LT9211_I2C_DEV_ADDR, // 不含读写位

        /* for user sync */
        .combo_dev_cfg = {
            .devno = 0,
            .lane_id = {0, 1, 2, 3},
            .out_mode = OUT_MODE_DSI_VIDEO,
            .out_format = OUT_FORMAT_RGB_24BIT,
            .video_mode = BURST_MODE,
            .sync_info = {
                .hpw = 32,    /* 32 pixel */
                .hbp = 72,    /* 72 pixel */
                .hact = 1920, /* 1920 pixel */
                .hfp = 56,    /* 56 pixel */
                .vpw = 4,     /* 4 line */
                .vbp = 8,     /* 8 line */
                .vact = 1080, /* 1080 line */
                .vfp = 8,     /* 8 line */
            },
            .phy_data_rate = 344, /* 999 Mbps */
            .pixel_clk = 57200,   /* 137280 KHz */
            .clklane_continue_mode = MIPI_TX_CLK_LANE_NON_CONTINUE,
        },
    },
};
#endif
// #endregion

#endif

static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_1280x720_user = {
    .vo_config = {
        /* for device */
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_MIPI,
        .intf_sync = HI_VO_OUT_USER, // 输出时序
        //.pic_size = PIC_1080P,
        .bg_color = COLOR_RGB_BLACK, // 背景颜色
        /* for layer */
        .pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
        .disp_rect = {0, 0, 1280, 720},              // 显示的区域
        .image_size = {1280, 720},                   /*图像分辨率，图像从输出通道进入后的合成画面尺寸，
                            具体可以看MPP 媒体处理软件 V6.0 开发参考P545~p546*/
        .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE, // 分区模式：不分区
        .compress_mode = HI_COMPRESS_MODE_NONE,      // 压缩模式：不压缩

        .dis_buf_len = 3,
        /* 3: def buf len for single */             // 缓冲区数量
        .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8, // 动态范围
        /* for channel */
        .vo_mode = VO_MODE_1MUX,
        /* for user sync */
        // .sync_info = {
        //     .syncm = 0,
        //     .iop = 1,
        //     .intfb = 0,
        //     .vact = 1080,
        //     .vbb = (8 + 4),
        //     .vfb = 8,
        //     .hact = 1920,
        //     .hbb = (72 + 32),
        //     .hfb = 56,
        //     .hmid = 1,
        //     .bvact = 1,
        //     .bvbb = 1,
        //     .bvfb = 1,
        //     .hpw = 32,
        //     .vpw = 4,
        //     .idv = 0,
        //     .ihs = 0,
        //     .ivs = 0,
        // },
        .sync_info = {
            .syncm = 0,
            .iop = 1,
            .intfb = 0,
            .vact = 720,
            .vbb = (8 + 4),
            .vfb = 8,
            .hact = 1280,
            .hbb = (72 + 32),
            .hfb = 56,
            .hmid = 1,
            .bvact = 1,
            .bvbb = 1,
            .bvfb = 1,
            .hpw = 32,
            .vpw = 4,
            .idv = 0,
            .ihs = 0,
            .ivs = 0,
        },
        //.sync_info = {0, 0, 1, 1080, (8+4), 8, 1920, (72+32), 56, 1, 540, 9, 7, 32, 4, 0, 0, 0},
        //.sync_info = {0, 1, 1, 1920, 12, 15, 1200, 104, 60, 1, 540, 9, 7, 24, 2, 0, 0, 0},
        .user_sync = {
#if 0
            .manual_user_sync_info.user_sync_attr = {
                
                .clk_src = HI_VO_CLK_SRC_PLL,
                .vo_pll = { /* if mipitx, set it by pixel clk and div mode */
                    .fb_div = 80, /* 80 fb div */
                    .frac = 0,
                    .ref_div = 1, /* 1 ref div */
                    .post_div1 = 6, /* 6 post div1 */
                    .post_div2 = 2, /* 2 post div2 */
                },
            },
            .manual_user_sync_info.pre_div = 1, /* if mipitx, set it by pixel clk */
            .manual_user_sync_info.dev_div = 1, /* if rgb, set it by serial mode */
#endif
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_AUTO,
            .auto_user_sync_info.pixel_clk = 63936 * 1000,
        },
        .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE, // 设备帧率 60
    },
    .tx_config = {
        /* for combo dev config */
        .intf_sync = HI_MIPI_TX_OUT_USER,

        /* for screen cmd */
        .i2c_cmd_en = HI_TRUE,
        .oled_num = 2,
        .i2c_bus = {I2C_OLEDA_BUS_NUM, I2C_OLEDB_BUS_NUM},
        .i2c_addr = {HJ_I2C_DEV_ADDR, HJ_I2C_DEV_ADDR}, // 不含读写位
        .i2c_cmd_count = {HJ_I2C_CMD_COUNT_1080P, HJ_I2C_CMD_COUNT_1080P},
        .i2c_cmd_info = {g_i2c_cmd_info_boe701_1080p60, g_i2c_cmd_info_boe701_1080p60},
        .addr_len = 2,
        .data_len = 1,
        .i2c_lt9211_enable = HI_FALSE,
        .i2c_lt9211_bus = LT9211_I2C_BUS_NUM,
        .i2c_lt9211_addr = LT9211_I2C_DEV_ADDR, // 不含读写位

        /* for user sync */
        .combo_dev_cfg = {
            .devno = 0,
            .lane_id = {0, 1, 2, 3},
            .out_mode = OUT_MODE_DSI_VIDEO,
            .out_format = OUT_FORMAT_RGB_24BIT,
            .video_mode = BURST_MODE,
            .sync_info = {
                .hpw = 32,    /* 32 pixel */
                .hbp = 72,    /* 72 pixel */
                .hact = 1280, /* 1920 pixel */
                .hfp = 56,    /* 56 pixel */
                .vpw = 4,     /* 4 line */
                .vbp = 8,     /* 8 line */
                .vact = 720,  /* 1080 line */
                .vfp = 8,     /* 8 line */
            },
            .phy_data_rate = 384, /* 999 Mbps */
            .pixel_clk = 63936,   /* 137280 KHz */
            .clklane_continue_mode = MIPI_TX_CLK_LANE_NON_CONTINUE,
        },
    },
};

#if 1
static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_1024x768_user = {
    .vo_config = {
        /* for device */
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_BT1120,
        .intf_sync = HI_VO_OUT_1024x768_60, // 输出时序
        //.pic_size = PIC_1080P,
        .bg_color = COLOR_RGB_BLACK, // 背景颜色
        /* for layer */
        .pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
        .disp_rect = {0, 0, 1024, 768},              // 显示的区域
        .image_size = {1024, 768},                   /*图像分辨率，图像从输出通道进入后的合成画面尺寸，
                            具体可以看MPP 媒体处理软件 V6.0 开发参考P545~p546*/
        .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE, // 分区模式：不分区
        .compress_mode = HI_COMPRESS_MODE_NONE,      // 压缩模式：不压缩

        .dis_buf_len = 3,
        /* 3: def buf len for single */             // 缓冲区数量
        .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8, // 动态范围
        /* for channel */
        .vo_mode = VO_MODE_1MUX,
        /* for user sync */
        .sync_info = {
            .syncm = 0,
            .iop = 1,
            .intfb = 0,
            .vact = 1080,
            .vbb = (36 + 5),
            .vfb = 4,
            .hact = 3840,
            .hbb = (384),
            .hfb = 1056,
            .hmid = 1,
            .bvact = 1,
            .bvbb = 1,
            .bvfb = 1,
            .hpw = 88,
            .vpw = 5,
            .idv = 0,
            .ihs = 0,
            .ivs = 0,
        },
        //.sync_info = {0, 0, 1, 1080, (8+4), 8, 1920, (72+32), 56, 1, 540, 9, 7, 32, 4, 0, 0, 0},
        //.sync_info = {0, 1, 1, 1920, 12, 15, 1200, 104, 60, 1, 540, 9, 7, 24, 2, 0, 0, 0},
        .user_sync = {
#if 0
            .manual_user_sync_info.user_sync_attr = {
                
                .clk_src = HI_VO_CLK_SRC_PLL,
                .vo_pll = { /* if mipitx, set it by pixel clk and div mode */
                    .fb_div = 80, /* 80 fb div */
                    .frac = 0,
                    .ref_div = 1, /* 1 ref div */
                    .post_div1 = 6, /* 6 post div1 */
                    .post_div2 = 2, /* 2 post div2 */
                },
            },
            .manual_user_sync_info.pre_div = 1, /* if mipitx, set it by pixel clk */
            .manual_user_sync_info.dev_div = 1, /* if rgb, set it by serial mode */
#endif
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_AUTO,
            .auto_user_sync_info.pixel_clk = 148500000,
        },
        .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE, // 设备帧率 60
    },
    .tx_config = {
        /* for combo dev config */
        .intf_sync = HI_MIPI_TX_OUT_1280X1024_60,

        /* for screen cmd */
        .i2c_cmd_en = HI_TRUE,
        .oled_num = 1,
        .i2c_bus = {L2414_I2C_BUS_NUM, L2414_I2C_BUS_NUM},
        .i2c_addr = {L2414_I2C_DEV_ADDR, L2414_I2C_DEV_ADDR}, // 不含读写位
        .i2c_cmd_count = {L2414_I2C_CMD_COUNT, L2414_I2C_CMD_COUNT},
        .i2c_cmd_info = {g_i2c_cmd_info_l2414_1024x768, g_i2c_cmd_info_l2414_1024x768},
        .addr_len = 1,
        .data_len = 1,
        /* for user sync */
        .combo_dev_cfg = {
            .devno = 0,
            .lane_id = {0, 1, 2, 3},
            .out_mode = OUT_MODE_DSI_VIDEO,
            .out_format = OUT_FORMAT_RGB_24BIT,
            .video_mode = BURST_MODE,
            .sync_info = {
                .hpw = 32,    /* 32 pixel */
                .hbp = 72,    /* 72 pixel */
                .hact = 1920, /* 1920 pixel */
                .hfp = 56,    /* 56 pixel */
                .vpw = 4,     /* 4 line */
                .vbp = 8,     /* 8 line */
                .vact = 1080, /* 1080 line */
                .vfp = 8,     /* 8 line */
            },
            .phy_data_rate = 344, /* 999 Mbps */
            .pixel_clk = 57200,   /* 137280 KHz */
            .clklane_continue_mode = MIPI_TX_CLK_LANE_NON_CONTINUE,
        },
    },
};

#endif

/* VO: USER 3840x720_60, TX: USER 3840x720 */
static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_3840x720_user = {
    .vo_config = {
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_MIPI,
        .intf_sync = HI_VO_OUT_USER,
        .bg_color = COLOR_RGB_BLACK,
        .pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
        .disp_rect = {0, 0, 3840, 720},
        .image_size = {3840, 720},
        .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE,
        .dis_buf_len = 3, /* 3: def buf len for single */
        .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
        .vo_mode = VO_MODE_1MUX,
        .compress_mode = HI_COMPRESS_MODE_NONE,

        .sync_info = {0, 1, 1, 720, 11, 5, 3840, 56, 52, 1, 1, 1, 1, 24, 8, 0, 0, 0},
        .user_sync = {
            .auto_user_sync_info.pixel_clk = 174344000,
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_AUTO,
        },
        .dev_frame_rate = SAMPLE_VO_DEV_FRAME_RATE,
        .vo_sharpen = {
            .enable = HI_TRUE,
            .peak_ratio = 255,
        },
        .hor_split_en = HI_TRUE,
    },
    .tx_config = {
        /* for combo dev config */
        .intf_sync = HI_MIPI_TX_OUT_USER,

        /* for screen cmd */
        .cmd_count = 0,
        .cmd_info = HI_NULL,

        /* for user sync */
        .combo_dev_cfg = {
            .devno = 0, .lane_id = {0, 1, 2, 3}, .out_mode = OUT_MODE_DSI_VIDEO, .out_format = OUT_FORMAT_RGB_24BIT, .video_mode = BURST_MODE, .sync_info = {
                                                                                                                                                   .vact = 720,  /* 720 line */
                                                                                                                                                   .vbp = 3,     /* 3 line */
                                                                                                                                                   .vfp = 5,     /* 5 line */
                                                                                                                                                   .hact = 3840, /* 3840 pixel */
                                                                                                                                                   .hbp = 32,    /* 32 pixel */
                                                                                                                                                   .hfp = 52,    /* 52 pixel */
                                                                                                                                                   .hpw = 24,    /* 24 pixel */
                                                                                                                                                   .vpw = 2,     /* 2 line */
                                                                                                                                               },
            .phy_data_rate = 1047, /* 1047 Mbps */
            .pixel_clk = 174344,   /* 174344 KHz */
        },
    },
};

static void sample_vo_handle_sig(hi_s32 signo)
{
    if ((signo == SIGINT) || (signo == SIGTERM))
    {
        g_vo_sig_flag = 1;
    }
}

static hi_void sample_vo_do_pause(hi_void)
{
    if (g_vo_sig_flag == 1)
    {
        return;
    }

    sample_pause();

    if (g_vo_sig_flag == 1)
    {
        return;
    }
}

static hi_void sample_vo_get_default_vb_config(hi_size *size, hi_vb_cfg *vb_cfg)
{
    hi_vb_calc_cfg calc_cfg;
    hi_pic_buf_attr buf_attr;

    (hi_void) memset_s(vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg->max_pool_cnt = 128; /* 128 blks */

    buf_attr.width = size->width;
    buf_attr.height = size->height;
    buf_attr.align = HI_DEFAULT_ALIGN;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422;
    buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    buf_attr.video_format = HI_VIDEO_FORMAT_LINEAR;
    hi_common_get_pic_buf_cfg(&buf_attr, &calc_cfg);

    vb_cfg->common_pool[0].blk_size = calc_cfg.vb_size;
    vb_cfg->common_pool[0].blk_cnt = 30; /* 30 blk */
}

#ifdef SAMPLE_MEM_SHARE_ENABLE
static hi_void sample_init_mem_share(hi_void)
{
    hi_u32 i;
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) != HI_SUCCESS)
    {
        sample_print("get common pool_id failed!\n");
        return;
    }
    for (i = 0; i < pools_id.pool_cnt; ++i)
    {
        hi_mpi_vb_pool_share_all(pools_id.pool[i]);
    }
}
#endif

static hi_s32 sample_vo_set_sys_cfg(hi_vi_vpss_mode_type mode_type, hi_vi_aiisp_mode aiisp_mode)
{
    hi_s32 ret;
    hi_size size;
    hi_vb_cfg vb_cfg;
    hi_u32 supplement_config;

    size.width = 3840;  /* 3840 x 2160 */
    size.height = 2160; /* 3840 x 2160 */
    sample_vo_get_default_vb_config(&size, &vb_cfg);

    supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;
    ret = sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config);
    if (ret != HI_SUCCESS)
    {
        return HI_FAILURE;
    }

    ret = sample_comm_vi_set_vi_vpss_mode(mode_type, aiisp_mode);
    if (ret != HI_SUCCESS)
    {
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    sample_init_mem_share();
#endif
    return HI_SUCCESS;
}

#ifdef SAMPLE_MEM_SHARE_ENABLE
static hi_void sample_init_vo_mem_share(hi_vo_layer vo_layer)
{
    hi_s32 ret;
    hi_video_frame_info frame_info;
    hi_s32 milli_sec = 5000;
    hi_sys_mem_info mem_info;

    ret = hi_mpi_vo_get_screen_frame(vo_layer, &frame_info, milli_sec);
    if (ret != HI_SUCCESS)
    {
        sample_print("layer %d hi_mpi_vo_get_screen_frame fail for %#x!\n", vo_layer, ret);
        return;
    }

    ret = hi_mpi_sys_get_mem_info_by_phys(frame_info.video_frame.phys_addr[0], &mem_info);
    if (ret != HI_SUCCESS)
    {
        hi_mpi_vo_release_screen_frame(vo_layer, &frame_info);
        sample_print("layer %d hi_mpi_sys_get_mem_info_by_phys fail for %#x!\n", vo_layer, ret);
        return;
    }

    ret = hi_mpi_sys_mem_share_all(mem_info.mem_handle);
    if (ret != HI_SUCCESS)
    {
        hi_mpi_vo_release_screen_frame(vo_layer, &frame_info);
        sample_print("layer %d hi_mpi_sys_mem_share_all fail for %#x!\n", vo_layer, ret);
        return;
    }

    hi_mpi_vo_release_screen_frame(vo_layer, &frame_info);
}
#endif

static hi_s32 sample_vo_oled_gpio(unsigned int gpio_chip_num, unsigned int gpio_offset_num,
                                  unsigned int gpio_out_val)
{
    // hi_s32 ret;
    FILE *fp = NULL;
    hi_s32 gpio_num = gpio_chip_num * 8 + gpio_offset_num;

    hi_char file_name[50] = {0};
    hi_char buf[10] = {0};
    sprintf(file_name, "/sys/class/gpio/export");
    fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        printf("Cannot open %s.\n", file_name);
        return HI_FAILURE;
    }
    fprintf(fp, "%d", gpio_num);
    fclose(fp);
    fp = NULL;
    sprintf(file_name, "/sys/class/gpio/gpio%d/direction", gpio_num);
    fp = fopen(file_name, "rb+");
    if (fp == NULL)
    {
        printf("Cannot open %s.\n", file_name);
        return HI_FAILURE;
    }
    fprintf(fp, "out");
    fclose(fp);
    sprintf(file_name, "/sys/class/gpio/gpio%d/value", gpio_num);
    fp = fopen(file_name, "rb+");
    if (fp == NULL)
    {
        printf("Cannot open %s.\n", file_name);
        return HI_FAILURE;
    }
    if (gpio_out_val)
        strcpy(buf, "1");
    else
        strcpy(buf, "0");

    fwrite(buf, sizeof(hi_char), sizeof(buf) - 1, fp);
    printf("%s: gpio%d_%d = %s\n", __func__,
           gpio_chip_num, gpio_offset_num, buf);
    fclose(fp);
    sprintf(file_name, "/sys/class/gpio/unexport");
    fp = fopen(file_name, "w");
    if (fp == NULL)
    {
        printf("Cannot open %s.\n", file_name);
        return HI_FAILURE;
    }
    fprintf(fp, "%d", gpio_num);
    fclose(fp);
    return HI_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000); /* 1ms: 1000us */
    return;
}

void boe701_init_oled_pwdn()
{
    // oled_a
    sample_vo_oled_gpio(8, 0, 1);
    delay_ms(10);
    // oled_b
    sample_vo_oled_gpio(7, 6, 0);

    delay_ms(10);
}

void boe701_init_oled_rstn()
{
    // oled_a
    sample_vo_oled_gpio(7, 3, 0);
    delay_ms(1000);
    sample_vo_oled_gpio(7, 3, 1);
    delay_ms(120);

    // oled_b
    sample_vo_oled_gpio(7, 7, 0);
    delay_ms(1000);
    sample_vo_oled_gpio(7, 7, 1);
    delay_ms(120);
}

void l2414_init_oled_rstn()
{
    sample_vo_oled_gpio(14, 3, 0);
    delay_ms(1000);
    sample_vo_oled_gpio(14, 3, 1);
    delay_ms(120);
}

static hi_s32 start_vo_mipi_tx(const sample_vo_mipi_tx_cfg *vo_tx_cfg)
{
    hi_s32 ret;
    const sample_vo_cfg *vo_config = &vo_tx_cfg->vo_config;
    const sample_mipi_tx_config *tx_config = &vo_tx_cfg->tx_config;

    PRINTF_DEBUG;

#ifdef HJ_ENABLE
    boe701_init_oled_pwdn();

    boe701_init_oled_rstn();
#endif
#ifdef L2414_ENABLE
    l2414_init_oled_rstn();
#endif

    ret = sample_comm_vo_start_vo(vo_config);
    if (ret != HI_SUCCESS)
    {
        sample_print("start vo failed with 0x%x!\n", ret);
        return ret;
    }
    printf("start vo dhd%d.\n", vo_config->vo_dev);

#ifdef SAMPLE_MEM_SHARE_ENABLE
    sample_init_vo_mem_share(vo_config->vo_layer);
#endif

    // printf("please hit any\n");
    // getchar();
    if ((vo_config->vo_intf_type & HI_VO_INTF_MIPI) ||
        (vo_config->vo_intf_type & HI_VO_INTF_MIPI_SLAVE)
        //|| (vo_config->vo_intf_type & HI_VO_INTF_BT1120)
    )
    {
        ret = sample_comm_start_mipi_tx(tx_config);
        if (ret != HI_SUCCESS)
        {
            sample_print("start mipi tx failed with 0x%x!\n", ret);
            return ret;
        }
    }

    if ((vo_config->vo_intf_type & HI_VO_INTF_BT1120) && (tx_config->i2c_cmd_info[0] != NULL))
    {
        if (tx_config->i2c_cmd_en == HI_TRUE)
        {
            sample_comm_vo_bt1120_i2c_init(tx_config);
        }
    }

    return HI_SUCCESS;
}

static hi_void stop_vo_mipi_tx(const sample_vo_mipi_tx_cfg *vo_tx_cfg)
{
    hi_s32 ret;
    const sample_vo_cfg *vo_config = &vo_tx_cfg->vo_config;

    sample_comm_stop_mipi_tx(vo_config->vo_intf_type);
    ret = sample_comm_vo_stop_vo(vo_config);
    if (ret != HI_SUCCESS)
    {
        sample_print("stop vo failed with 0x%x!\n", ret);
    }
}

static hi_void sample_vo_mipitx_do_stop(sample_vi_cfg *vi_cfg, const sample_vo_mipi_tx_cfg *vo_tx_cfg)
{
    stop_vo_mipi_tx(vo_tx_cfg);
    sample_comm_vi_stop_vi(vi_cfg);
    sample_comm_sys_exit();
}

static hi_void sample_vo_do_stop(sample_vi_cfg *vi_cfg, const sample_vo_cfg *vo_config)
{
    hi_s32 ret;

    ret = sample_comm_vo_stop_vo(vo_config);
    if (ret != HI_SUCCESS)
    {
        sample_print("stop vo failed with 0x%x!\n", ret);
    }
    sample_comm_vi_stop_vi(vi_cfg);
    sample_comm_sys_exit();
}

static hi_void sample_switch_user_pic(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    sample_vi_user_pic_type user_pic_type;
    sample_vi_user_frame_info user_frame_info = {0};
    printf("vi_pipe = %d\n", vi_pipe);

    for (user_pic_type = VI_USER_PIC_FRAME; user_pic_type <= VI_USER_PIC_FRAME; user_pic_type++)
    {
        ret = sample_common_vi_load_user_pic(vi_pipe, user_pic_type, &user_frame_info);
        if (ret != HI_SUCCESS)
        {
            sample_print("load user pic failed!\n");
            return;
        }

        ret = hi_mpi_vi_set_pipe_user_pic(vi_pipe, &user_frame_info.frame_info);
        if (ret != HI_SUCCESS)
        {
            sample_print("hi_mpi_vi_set_pipe_user_pic failed!\n");
            printf("errno = %#x\n", ret);
            printf("width = %d, height = %d\n", user_frame_info.frame_info.video_frame.width, user_frame_info.frame_info.video_frame.height);
        }

        // printf("Enter any key to enable user pic!\n");
        // sample_get_char();
        sleep(1);
        ret = hi_mpi_vi_enable_pipe_user_pic(vi_pipe);
        if (ret != HI_SUCCESS)
        {
            sample_print("hi_mpi_vi_enable_pipe_user_pic failed!\n");
            printf("errno = %#x\n", ret);
        }

        // printf("Enter any key to disable user pic!\n");
        // sample_get_char();
        // ret = hi_mpi_vi_disable_pipe_user_pic(vi_pipe);
        // if (ret != HI_SUCCESS) {
        //     sample_print("hi_mpi_vi_disable_pipe_user_pic failed!\n");
        // }

        // sleep(1);
        // sample_common_vi_unload_user_pic(&user_frame_info);
    }
}

static hi_s32 sample_vo_mipi_tx(const sample_vo_mipi_tx_cfg *vo_tx_cfg)
{
    /* vi */
    hi_s32 ret;
    sample_vi_cfg vi_cfg;

    /* vo */
    const sample_vo_cfg *vo_config = &vo_tx_cfg->vo_config;

    ret = sample_vo_set_sys_cfg(HI_VI_ONLINE_VPSS_OFFLINE, HI_VI_AIISP_MODE_DEFAULT);
    if (ret != HI_SUCCESS)
    {
        sample_print("start sys failed with 0x%x!\n", ret);
        return ret;
    }

    sample_comm_vi_get_default_vi_cfg(SENSOR_TYPE, &vi_cfg);

    ret = sample_comm_vi_start_vi(&vi_cfg);
    if (ret != HI_SUCCESS)
    {
        sample_print("start vi failed with 0x%x!\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    /* step2: start vo and mipi tx */
    ret = start_vo_mipi_tx(vo_tx_cfg);
    if (ret != HI_SUCCESS)
    {
        sample_print("start mipi tx 1080p failed with 0x%x!\n", ret);
        sample_comm_vi_stop_vi(&vi_cfg);
        sample_comm_sys_exit();
        return ret;
    }

    /* step3: bind vi and vo */
    ret = sample_comm_vi_bind_vo(0, 0, vo_config->vo_layer, 0);
    if (ret != HI_SUCCESS)
    {
        sample_print("bind vi and vo failed with 0x%x!\n", ret);
        sample_vo_mipitx_do_stop(&vi_cfg, vo_tx_cfg);
        return ret;
    }
#if USER_PIC_3840_1080
    sample_switch_user_pic(vi_cfg.bind_pipe.pipe_id[0]);
#endif
    /* step4: do pause. */
    sample_vo_do_pause();

    /* step5: exit */
    ret = sample_comm_vi_un_bind_vo(0, 0, vo_config->vo_layer, 0);
    if (ret != HI_SUCCESS)
    {
        sample_print("vi unbind vo failed with 0x%x!\n", ret);
    }

    sample_vo_mipitx_do_stop(&vi_cfg, vo_tx_cfg);
    return ret;
}

static hi_s32 sample_vo_rgb(const sample_vo_cfg *vo_config)
{
    /* vi */
    hi_s32 ret;
    sample_vi_cfg vi_cfg;

    ret = sample_vo_set_sys_cfg(HI_VI_ONLINE_VPSS_OFFLINE, HI_VI_AIISP_MODE_DEFAULT);
    if (ret != HI_SUCCESS)
    {
        sample_print("start sys failed with 0x%x!\n", ret);
        return ret;
    }

    sample_comm_vi_get_default_vi_cfg(SENSOR0_TYPE, &vi_cfg);

    ret = sample_comm_vi_start_vi(&vi_cfg);
    if (ret != HI_SUCCESS)
    {
        sample_print("start vi failed with 0x%x!\n", ret);
        sample_comm_sys_exit();
        return ret;
    }

    ret = sample_comm_vo_start_vo(vo_config);
    if (ret != HI_SUCCESS)
    {
        sample_print("start vo failed with 0x%x!\n", ret);
        sample_comm_vi_stop_vi(&vi_cfg);
        sample_comm_sys_exit();
        return ret;
    }
    /* step3: bind vi and vo */
    ret = sample_comm_vi_bind_vo(0, 0, vo_config->vo_layer, 0);
    if (ret != HI_SUCCESS)
    {
        sample_print("bind vi and vo failed with 0x%x!\n", ret);
        sample_vo_do_stop(&vi_cfg, vo_config);
        return ret;
    }

    /* step4: do pause. */
    sample_vo_do_pause();

    /* step5: exit */
    ret = sample_comm_vi_un_bind_vo(0, 0, vo_config->vo_layer, 0);
    if (ret != HI_SUCCESS)
    {
        sample_print("vi unbind vo failed with 0x%x!\n", ret);
    }

    sample_vo_do_stop(&vi_cfg, vo_config);
    return ret;
}

// static hi_s32 sample_vo_mipi_tx_1200x1920_user(hi_void)
// {
//     return sample_vo_mipi_tx(&g_vo_tx_cfg_1200x1920_user);
// }

static hi_s32 sample_vo_mipi_tx_1920_1080_user(hi_void)
{
    return sample_vo_mipi_tx(&g_vo_tx_cfg_1920x1080_user);
}

static hi_s32 sample_vo_mipi_tx_1280x720_user(hi_void)
{
    return sample_vo_mipi_tx(&g_vo_tx_cfg_1280x720_user);
}

static hi_s32 sample_vo_mipi_tx_3840x720_user(hi_void)
{
    return sample_vo_mipi_tx(&g_vo_tx_cfg_3840x720_user);
}

static hi_s32 sample_vo_rgb_8bit_240x320_user(hi_void)
{
    return sample_vo_rgb(&g_vo_rgb_8bit_config);
}

// static void sample_vo_usage(const char *name)
// {
//     printf("usage : %s <index>\n", name);
//     printf("index:\n");
//     printf("\t0: vo dhd0 rgb 8bit                   USER    240x320@60 output.\n");
//     printf("\t1: vo dhd0 mipi_tx                    USER    1920x1080@60 output.\n");
//     printf("\t2: vo dhd0 mipi_tx one to dual-screen USER    3840x720@60 output.\n");
// }

static hi_s32 sample_vo_execute_case(hi_char index)
{
    if (index == '0')
    {
        return sample_vo_rgb_8bit_240x320_user();
    }
    else if (index == '1')
    {
        return sample_vo_mipi_tx_1920_1080_user();
    }
    else if (index == '2')
    {
        return sample_vo_mipi_tx_3840x720_user();
    }
    else if (index == '3')
    {
        return sample_vo_mipi_tx_1280x720_user();
    }
    sample_print("the index %c is invalid!\n", index);
    return HI_FAILURE;
}

hi_s32 main(hi_s32 argc, hi_char *argv[])
{
    hi_s32 ret;
    hi_char index;

    // if (argc != 2) { /* 2: 2 arg num */
    //     sample_vo_usage(argv[0]);
    //     return HI_FAILURE;
    // }

    // if (!strncmp(argv[1], "-h", 2)) { /* 2: 2 chars */
    //     sample_vo_usage(argv[0]);
    //     return HI_SUCCESS;
    // }

#ifndef __LITEOS__
    sample_sys_signal(sample_vo_handle_sig);
#endif

    // if (strlen(argv[1]) > 1) {
    //     sample_vo_usage(argv[0]);
    //     return HI_FAILURE;
    // }

    // index = *argv[1];
    index = '1';
    ret = sample_vo_execute_case(index);
    if ((ret == HI_SUCCESS) && (g_vo_sig_flag == 0))
    {
        sample_print("sample_vo exit normally!\n");
    }
    else
    {
        sample_print("sample_vo exit abnormally!\n");
    }

    return ret;
}
