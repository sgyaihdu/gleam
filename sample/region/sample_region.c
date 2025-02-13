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
#include <sys/prctl.h>
#include <termios.h>
#include<sys/select.h>



#include "sample_comm.h"
#include "common/sample_rtsp.h"

#define MM16_BMP "./res/mm16.bmp"
#define MM_BMP "./res/mm.bmp"
#define STREAM_PATH "res"
#define STREAM_H265_1080P "1080P.h265"
#define STREAM_H265_4K "3840x2160_8bit.h265"

#define VB_NUM_PRE_CHN 8
#define RGN_VI_CHN   0
#define RGN_VI_PIPE  0
#define RGN_VPSS_GRP 0
#define RGN_VPSS_CHN 0
#define RGN_VENC_CHN 0

#define RGN_HANDLE_NUM_1 1
#define RGN_HANDLE_NUM_4 4
#define RGN_HANDLE_NUM_8 8

#define RGN_VO_CHN 0
#define RGN_VDEC_CHN 0
#define RGN_VI_CHN_NUM 1
#define RGN_VDEC_CHN_NUM 1

#define REGION_STOP_GET_VENC_STREAM (0x01L << 0)
#define REGION_UNBIND_VPSS_VENC     (0x01L << 1)
#define REGION_STOP_VENC            (0x01L << 2)
#define REGION_UNBIND_VPSS_VO       (0x01L << 3)
#define REGION_STOP_VO              (0x01L << 4)
#define REGION_UNBIND_VDEC_VPSS     (0x01L << 5)
#define REGION_STOP_SEND_STREAM     (0x01L << 6)
#define REGION_STOP_VPSS            (0x01L << 7)
#define REGION_STOP_VDEC            (0x01L << 8)
#define REGION_UNBIND_VI_VPSS       (0x01L << 9)
#define REGION_STOP_VI              (0x01L << 10)
typedef hi_u32 region_stop_flag;

#define _1080P_WIDTH 1920
#define _1080P_HEIGHT 1080


#define HJ_ENABLE
//#define L2414_ENABLE

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


static hi_s32 start_vo_mipi_tx(const sample_vo_mipi_tx_cfg *vo_tx_cfg);
static hi_s32 sample_vo_oled_gpio(unsigned int gpio_chip_num, unsigned int gpio_offset_num,
                                  unsigned int gpio_out_val);


static void delay_ms(int ms)
{
    usleep(ms * 1000); /* 1ms: 1000us */
    return;
}

static void boe701_init_oled_rstn()
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


static void l2414_init_oled_rstn()
{
    sample_vo_oled_gpio(14, 3, 0);
    delay_ms(1000);
    sample_vo_oled_gpio(14, 3, 1);
    delay_ms(120);
}



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








static const sample_vo_mipi_tx_cfg g_vo_tx_cfg_1920x1080_user = {
    .vo_config = {
        /* for device */
        .vo_dev = SAMPLE_VO_DEV_UHD,
        .vo_layer = SAMPLE_VO_LAYER_VHD0,
        .vo_intf_type = HI_VO_INTF_MIPI,
        .intf_sync = HI_VO_OUT_USER, // 输出时序
        .bg_color = COLOR_RGB_BLACK, // 背景颜色
        /* for layer */
        //.pix_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_422,
        .pix_format =  HI_PIXEL_FORMAT_YUV_400,
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

        .user_sync = {
#if 0
#endif
            .clk_reverse_en = HI_FALSE,
            .op_mode = HI_OP_MODE_AUTO,
            .auto_user_sync_info.pixel_clk = 57200 * 1000,
        },
        .dev_frame_rate = 25, // 设备帧率 25
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



static hi_char *g_path_bmp, *g_path_bmp2;
static pthread_t g_vdec_thread;
static hi_vo_intf_sync g_rgn_intf_sync = HI_VO_OUT_1080P60;
static int g_rgn_sample_exit = 0;



static sample_vdec_attr g_rgn_vdec_cfg = {
    .type = HI_PT_H265,
    .mode = HI_VDEC_SEND_MODE_FRAME,
    .width = _4K_WIDTH,
    .height = _4K_HEIGHT,
    .sample_vdec_video.dec_mode = HI_VIDEO_DEC_MODE_IP,
    .sample_vdec_video.bit_width = HI_DATA_BIT_WIDTH_8,
    .sample_vdec_video.ref_frame_num = 2, /* 2:ref_frame_num */
    .display_frame_num = 2,               /* 2:display_frame_num */
    .frame_buf_cnt = 5,                   /* 5:2+2+1 */
};

static sampe_sys_cfg g_vio_sys_cfg = {
    .route_num = 1,
    .mode_type = HI_VI_OFFLINE_VPSS_OFFLINE,
    .nr_pos = HI_3DNR_POS_VI,
    .vi_fmu = {0},
    .vpss_fmu = {0},
};

static sample_vi_cfg g_vi_cfg = { 0 };

static vdec_thread_param g_rgn_vdec_thread_param = {
    .chn_id = 0,
    .type = HI_PT_H265,
    .stream_mode = HI_VDEC_SEND_MODE_FRAME,
    .interval_time = 1000, /* 1000000:interval_time */
    .pts_init = 0,
    .pts_increase = 0,
    .e_thread_ctrl = THREAD_CTRL_START,
    .circle_send = HI_TRUE,
    .milli_sec = 0,
    .min_buf_size = (_4K_WIDTH * _4K_HEIGHT * 3) >> 1, /* 3:buff_size */
    .c_file_path = "res",
    .c_file_name = STREAM_H265_4K,
    .fps = 60, /* 60:default fps */
};

static sample_vo_cfg g_rgn_vo_cfg = {
    .vo_dev = SAMPLE_VO_DEV_DHD0,
    .vo_layer = SAMPLE_VO_LAYER_VHD0,
    .vo_intf_type = HI_VO_INTF_BT1120,
    .intf_sync = HI_VO_OUT_1080P60,
    .bg_color = COLOR_RGB_BLUE,
    .vo_mode = VO_MODE_1MUX,
    .vo_part_mode = HI_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len = 3, /* 3:buf len */
    .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
    .compress_mode = HI_COMPRESS_MODE_NONE,
};

static sample_vb_param g_vb_param = {
    .vb_size = {3840, 2160},
    .pixel_format = {HI_PIXEL_FORMAT_RGB_BAYER_12BPP, HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420},
    .compress_mode = {HI_COMPRESS_MODE_LINE, HI_COMPRESS_MODE_SEG},
    .video_format = {HI_VIDEO_FORMAT_LINEAR, HI_VIDEO_FORMAT_LINEAR},
    .blk_num = {20, 20}
};

/*
 * function: to process abnormal case
 */
static hi_void sample_region_handle_sig(hi_s32 signo)
{
    if ((signo == SIGINT) || (signo == SIGTERM)) {
        g_rgn_sample_exit = 1;
    }
}

static hi_void rgn_sample_pause(hi_void)
{
    if (g_rgn_sample_exit == 1) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        sample_comm_sys_exit();
        exit(-1);
    }

    sample_pause();

    if (g_rgn_sample_exit == 1) {
        printf("\033[0;31mprogram termination abnormally!\033[0;39m\n");
        sample_comm_sys_exit();
        exit(-1);
    }
}

static hi_s32 sample_region_start_venc(hi_void)
{
    hi_s32 ret;
    hi_venc_gop_mode gop_mode;
    hi_venc_gop_attr gop_attr;
    sample_comm_venc_chn_param venc_create_param;
    hi_venc_start_param start_param;

    gop_mode = HI_VENC_GOP_MODE_NORMAL_P;

    (hi_void)memset_s(&venc_create_param, sizeof(sample_comm_venc_chn_param), 0, sizeof(sample_comm_venc_chn_param));
    ret = sample_comm_venc_get_gop_attr(gop_mode, &gop_attr);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_venc_get_gop_attr failed!\n");
        return ret;
    }
    venc_create_param.type = HI_PT_H265;
    venc_create_param.gop = 60; /* 60:default gop val */
    venc_create_param.frame_rate = 30; /* 30:is a number */
    venc_create_param.stats_time = 2; /* 2:is a number */
    venc_create_param.rc_mode = SAMPLE_RC_CBR;
    venc_create_param.venc_size.height = _1080P_HEIGHT;
    venc_create_param.venc_size.width = _1080P_WIDTH;
    venc_create_param.size = PIC_1080P;

    (hi_void)memcpy_s(&venc_create_param.gop_attr, sizeof(hi_venc_gop_attr), &gop_attr, sizeof(hi_venc_gop_attr));

    /* step 1:  creat encode channel */
    ret = sample_comm_venc_create(RGN_VENC_CHN, &venc_create_param);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_venc_create failed with%#x! \n", ret);
        return HI_FAILURE;
    }
    /* step 2:  start recv venc pictures */
    start_param.recv_pic_num = -1;
    ret = hi_mpi_venc_start_chn(RGN_VENC_CHN, &start_param);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_venc_start_recv_pic failed with%#x! \n", ret);
        ret = hi_mpi_venc_destroy_chn(RGN_VENC_CHN);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_venc_destroy_chn vechn[%d] failed with %#x!\n", RGN_VENC_CHN, ret);
        }
    }
    return ret;
}

static hi_s32 sample_region_stop_venc(hi_void)
{
    return sample_comm_venc_stop(RGN_VENC_CHN);
}

static hi_s32 sample_region_start_get_venc_stream(hi_void)
{
    hi_s32 venc_chn[2] = {0, 1}; /* 2:VENC chn max num */

    return sample_comm_venc_start_get_stream(venc_chn, 1);
}

static hi_void sample_region_stop_get_venc_stream(hi_void)
{
    sample_comm_venc_stop_get_stream(1);
}

static hi_s32 sample_region_start_vdec(hi_u32 chn_num, sample_vdec_attr *vdec_attr)
{
    hi_s32 ret;
    hi_pic_buf_attr buf_attr = { 0 };
    hi_vb_cfg vb_cfg;
    hi_size disp_size;
    hi_pic_size disp_pic_size = PIC_3840X2160;

    ret = sample_comm_sys_get_pic_size(disp_pic_size, &disp_size);
    if (ret != HI_SUCCESS) {
        sample_print("sys get pic size fail for %#x!\n", ret);
        sample_comm_sys_exit();
        return ret;
    }
    buf_attr.align = 0;
    buf_attr.bit_width = HI_DATA_BIT_WIDTH_8;
    buf_attr.compress_mode = HI_COMPRESS_MODE_NONE;
    buf_attr.height = disp_size.height;
    buf_attr.width = disp_size.width;
    buf_attr.pixel_format = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420;

    (hi_void)memset_s(&vb_cfg, sizeof(hi_vb_cfg), 0, sizeof(hi_vb_cfg));
    vb_cfg.max_pool_cnt = 1;                               /* 1:vb pool cnt */
    vb_cfg.common_pool[0].blk_cnt = 10 * RGN_VDEC_CHN_NUM; /* 10:vb blk cnt */
    vb_cfg.common_pool[0].blk_size = hi_common_get_pic_buf_size(&buf_attr);
    ret = sample_comm_sys_vb_init(&vb_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("init sys fail for %#x!\n", ret);
        sample_comm_sys_exit();
        return ret;
    }
    ret = sample_comm_vdec_init_vb_pool(chn_num, vdec_attr, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("vdec init vb_pool fail\n");
        sample_comm_sys_exit();
        return ret;
    }
    ret = sample_comm_vdec_start(chn_num, vdec_attr, HI_VDEC_MAX_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("vdec start fail\n");
        sample_comm_vdec_exit_vb_pool();
        sample_comm_sys_exit();
    }

    return ret;
}

static hi_s32 sample_region_start_vi(hi_u32 chn_num)
{
    hi_s32 i;
    hi_s32 ret = HI_SUCCESS;
    hi_vb_cfg vb_cfg;
    sample_sns_type sns_type = SENSOR0_TYPE;
    hi_u32 supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK;

    sample_comm_sys_get_default_vb_cfg(&g_vb_param, &vb_cfg);
    if (sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (sample_comm_vi_set_vi_vpss_mode(g_vio_sys_cfg.mode_type, HI_VI_AIISP_MODE_DEFAULT) != HI_SUCCESS) {
        sample_comm_sys_exit();
        return ret;
    }

    if (hi_mpi_sys_set_3dnr_pos(g_vio_sys_cfg.nr_pos) != HI_SUCCESS) {
        sample_comm_sys_exit();
        return ret;
    }

    sample_comm_vi_get_vi_cfg_by_fmu_mode(sns_type, g_vio_sys_cfg.vi_fmu[0], &g_vi_cfg);

    for (i = 0; i < g_vio_sys_cfg.route_num; i++) {
        ret = sample_comm_vi_start_vi(&g_vi_cfg);
        if (ret != HI_SUCCESS) {
            return ret;
        }
    }

    return ret;
}


static hi_s32 sample_region_stop_vdec(hi_u32 chn_num)
{
    hi_s32 ret;
    ret = sample_comm_vdec_stop(chn_num);
    if (ret != HI_SUCCESS) {
        printf("vdec stop fail\n");
        return HI_FAILURE;
    }
    sample_comm_vdec_exit_vb_pool();
    return HI_SUCCESS;
}

static hi_s32 sample_region_common_start_vpss(hi_s32 vpss_grp, hi_s32 vpss_chn, const hi_size *size)
{
    hi_s32 ret;
    hi_vpss_grp_attr grp_attr;
    sample_vpss_chn_attr vpss_chn_attr = {0};

    sample_comm_vpss_get_default_grp_attr(&grp_attr);
    grp_attr.max_width = size->width;
    grp_attr.max_height = size->height;
    sample_comm_vpss_get_default_chn_attr(&vpss_chn_attr.chn_attr[vpss_chn]);
    vpss_chn_attr.chn_attr[vpss_chn].width = size->width;
    vpss_chn_attr.chn_attr[vpss_chn].height = size->height;

    vpss_chn_attr.chn_attr[vpss_chn].width = 1920;
    vpss_chn_attr.chn_attr[vpss_chn].height = 1080;

    vpss_chn_attr.chn_attr[vpss_chn].compress_mode = HI_COMPRESS_MODE_NONE;
    vpss_chn_attr.chn_enable[vpss_chn] = HI_TRUE;
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
    ret = sample_common_vpss_start(vpss_grp, &grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_region_stop_vpss()
{
    hi_s32 ret;
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {0};

    ret = sample_common_vpss_stop(RGN_VPSS_GRP, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("failed with %#x!\n", ret);
        return HI_FAILURE;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_region_start_vpss(hi_void)
{
    hi_s32 ret;
    hi_size size;

    size.width = _1080P_WIDTH;
    size.height = _1080P_HEIGHT;
    ret = sample_region_common_start_vpss(RGN_VPSS_GRP, RGN_VPSS_CHN, &size);
    if (ret != HI_SUCCESS) {
        sample_print("start vpss failed with 0x%x!\n", ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_void sample_region_start_send_stream(hi_void)
{
    sample_comm_vdec_start_send_stream(RGN_VDEC_CHN_NUM, &g_rgn_vdec_thread_param, &g_vdec_thread, HI_VDEC_MAX_CHN_NUM,
        2 * HI_VDEC_MAX_CHN_NUM); /* 2:thread num */
}

static hi_void sample_region_stop_send_stream(hi_void)
{
    sample_comm_vdec_stop_send_stream(RGN_VDEC_CHN_NUM, &g_rgn_vdec_thread_param, &g_vdec_thread, HI_VDEC_MAX_CHN_NUM,
        2 * HI_VDEC_MAX_CHN_NUM); /* 2:thread num */
    sleep(1);
}

static hi_void sample_region_do_stop_vi(region_stop_flag flag)
{
    if (flag & REGION_STOP_VI) {
        sample_comm_vi_stop_vi(&g_vi_cfg);
    }

    if (flag & REGION_UNBIND_VI_VPSS) {
        sample_comm_vi_un_bind_vpss(RGN_VI_PIPE, RGN_VI_CHN, RGN_VPSS_GRP, RGN_VPSS_CHN);
    }
}

static hi_s32 sample_region_do_stop(region_stop_flag flag)
{
    hi_s32 ret = HI_SUCCESS;

    if (flag & REGION_STOP_GET_VENC_STREAM) {
        sample_region_stop_get_venc_stream();
    }

    if (flag & REGION_UNBIND_VPSS_VENC) {
        ret = sample_comm_vpss_un_bind_venc(RGN_VPSS_GRP, RGN_VPSS_CHN, RGN_VENC_CHN);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_vpss_un_bind_venc failed!\n");
        }
    }

    if (flag & REGION_STOP_VENC) {
        ret = sample_region_stop_venc();
        if (ret != HI_SUCCESS) {
            sample_print("stop venc failed!\n");
        }
    }

    if (flag & REGION_UNBIND_VPSS_VO) {
        ret = sample_comm_vpss_un_bind_vo(RGN_VPSS_GRP, RGN_VPSS_CHN, SAMPLE_VO_LAYER_VHD0, RGN_VO_CHN);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_vpss_un_bind_vo failed with 0x%x!\n", ret);
        }
    }

    if (flag & REGION_STOP_VO) {
        sample_comm_stop_mipi_tx(g_vo_tx_cfg_1920x1080_user.vo_config.vo_intf_type);
        sample_comm_vo_stop_vo(&(g_vo_tx_cfg_1920x1080_user.vo_config));
    }

    sample_region_do_stop_vi(flag);

    if (flag & REGION_UNBIND_VDEC_VPSS) {
        sample_comm_vdec_un_bind_vpss(RGN_VDEC_CHN, RGN_VPSS_GRP);
    }

    if (flag & REGION_STOP_SEND_STREAM) {
        sample_region_stop_send_stream();
    }

    if (flag & REGION_STOP_VPSS) {
        ret = sample_region_stop_vpss();
        if (ret != HI_SUCCESS) {
            sample_print("stop vpss failed!\n");
        }
    }

    if (flag & REGION_STOP_VDEC) {
        ret = sample_region_stop_vdec(RGN_VDEC_CHN_NUM);
        if (ret != HI_SUCCESS) {
            sample_print("stop vdec failed!\n");
        }
    }

    return ret;
}

static hi_s32 sample_region_mpp_vi_vpss_venc_start(hi_void)
{
    hi_s32 ret;

    InitRtspServer();

    ret = sample_region_start_vi(RGN_VDEC_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("start vdec failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_region_start_vpss();
    if (ret != HI_SUCCESS) {
        sample_print("start vpss failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VI);
    }
    ret = sample_comm_vi_bind_vpss(RGN_VI_PIPE, RGN_VI_CHN, RGN_VPSS_GRP, RGN_VPSS_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("vdec_bind_multi_vpss 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VI | REGION_STOP_VPSS);
    }

    ret = start_vo_mipi_tx(&g_vo_tx_cfg_1920x1080_user);
    if(ret != HI_SUCCESS) {
        sample_print("start mipi tx 1080p failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VDEC | REGION_STOP_VPSS | REGION_UNBIND_VI_VPSS);
    }
    
    ret = sample_region_start_venc();
    if (ret != HI_SUCCESS) {
        sample_print("start venc failed!\n");
        return sample_region_do_stop(REGION_STOP_VDEC | REGION_STOP_VPSS | REGION_UNBIND_VI_VPSS | \
        REGION_STOP_VO);
    }

    ret = sample_comm_vpss_bind_vo(RGN_VPSS_GRP, RGN_VPSS_CHN, SAMPLE_VO_LAYER_VHD0, RGN_VO_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("bind vpss vo failed!\n");
        return sample_region_do_stop(REGION_STOP_VI | REGION_STOP_VPSS | REGION_UNBIND_VI_VPSS |REGION_STOP_VO| \
        REGION_STOP_VENC);
    }

    ret = sample_comm_vpss_bind_venc(RGN_VPSS_GRP, RGN_VPSS_CHN, RGN_VENC_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("bind vpss venc failed!\n");
        return sample_region_do_stop(REGION_STOP_VI | REGION_STOP_VPSS | REGION_UNBIND_VI_VPSS |REGION_STOP_VO| \
        REGION_STOP_VENC | REGION_UNBIND_VPSS_VENC);
    }

    ret = sample_region_start_get_venc_stream();
    if (ret != HI_SUCCESS) {
        sample_print("start get venc stream failed!\n");
        g_rgn_sample_exit = 1;
        return sample_region_do_stop(REGION_STOP_VDEC | REGION_STOP_VPSS | REGION_UNBIND_VDEC_VPSS | REGION_STOP_VENC | \
        REGION_UNBIND_VPSS_VENC | REGION_STOP_VENC | REGION_UNBIND_VPSS_VENC);
    }
    return ret;
}

static hi_s32 sample_region_mpp_vi_vpss_venc_end(hi_void)
{
    hi_s32 ret;

    sample_region_stop_get_venc_stream();
    ret = sample_comm_vpss_un_bind_venc(RGN_VPSS_GRP, RGN_VPSS_CHN, RGN_VENC_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_un_bind_venc failed!\n");
        return ret;
    }
    ret = sample_region_stop_venc();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_stop_venc failed!\n");
        return ret;
    }
    sample_region_stop_send_stream();
    ret = sample_region_stop_vpss();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_stop_vpss failed!\n");
        return ret;
    }
    sample_comm_vi_stop_vi(&g_vi_cfg);
   // ret = sample_region_stop_vdec(RGN_VDEC_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_stop_vdec failed!\n");
        return ret;
    }
    sample_comm_sys_exit();

    return HI_SUCCESS;
}

static hi_s32 sample_region_do_destroy(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn, region_op_flag flag)
{
    hi_s32 ret = HI_SUCCESS;

    if (flag & REGION_OP_CHN) {
        ret = sample_comm_region_detach(handle_num, type, chn, flag);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_region_detach failed!\n");
        }
    } else if (flag & REGION_OP_DEV) {
        ret = sample_comm_region_detach(handle_num, type, chn, flag);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_region_detach failed!\n");
        }
    }

    if (flag & REGION_DESTROY) {
        ret = sample_comm_region_destroy(handle_num, type);
        if (ret != HI_SUCCESS) {
            sample_print("sample_comm_region_destroy failed!\n");
        }
    }

    return ret;
}

static hi_s32 sample_region_do_destroy_venc(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn,
    region_op_flag flag)
{
    hi_s32 ret;

    (hi_void)sample_region_do_destroy(handle_num, type, chn, flag);
    ret = sample_region_mpp_vi_vpss_venc_end();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_mpp_vi_vpss_venc_end failed!\n");
    }

    return ret;
}



static void boe701_init_oled_pwdn()
{
    // oled_a
    sample_vo_oled_gpio(8, 0, 1);
    delay_ms(10);
    // oled_b
    sample_vo_oled_gpio(7, 6, 0);

    delay_ms(10);
}



hi_s32 start_vo_mipi_tx(const sample_vo_mipi_tx_cfg *vo_tx_cfg)
{
    hi_s32 ret;
    const sample_vo_cfg *vo_config = &vo_tx_cfg->vo_config;
    const sample_mipi_tx_config *tx_config = &vo_tx_cfg->tx_config;

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
    return HI_SUCCESS;
}








#define FALSE  -1    
#define TRUE   0
#define UART_INPUT_TTL_LEN 8

hi_s32 sample_region_vi_vpss_venc(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn)
{
    hi_s32 i;
    hi_s32 ret;
    hi_s32 min_handle;
    pthread_t thread_id;

    rgn_check_handle_num_return(handle_num);

    ret = sample_region_mpp_vi_vpss_venc_start(); // 设置vi vpss venc并启动
    if (ret != HI_SUCCESS) {
        return ret;
    }
    ret = sample_comm_region_create(handle_num, type); // 根据叠图数量和叠图类型创建region
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_region_create failed!\n");
        return sample_region_do_destroy_venc(handle_num, type, chn, REGION_DESTROY);
    }
    ret = sample_comm_region_attach(handle_num, type, chn, REGION_OP_CHN); //将region和vpss对应通道绑定
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_region_attach failed!\n");
        return sample_region_do_destroy_venc(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
    }
    min_handle = sample_comm_region_get_min_handle(type); //获取最小任务句柄
    if (sample_comm_check_min(min_handle) != HI_SUCCESS) {
        sample_print("min_handle(%d) should be in [0, %d).\n", min_handle, HI_RGN_HANDLE_MAX);
        return sample_region_do_destroy_venc(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
    }

    if (type == HI_RGN_OVERLAY || type == HI_RGN_OVERLAYEX) {
        for (i = min_handle; i < min_handle + handle_num; i++) {
            ret = sample_comm_region_set_bit_map(i, g_path_bmp2);
            if (ret != HI_SUCCESS) {
                sample_print("sample_comm_region_set_bit_map failed!\n");
                sample_region_do_destroy_venc(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
            }
        }
    }




    // 创建线程
    ret = pthread_create(&thread_id, NULL, sample_comm_region_draw_number, (void*)&min_handle);
    if (ret != 0) {
        sample_print("pthread_create failed with %d!\n", ret);
        return sample_region_do_destroy_venc(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
    }

    // 等待线程执行完毕
    ret = pthread_join(thread_id, NULL);
    if (ret != 0) {
        sample_print("pthread_join failed with %d!\n", ret);
        return sample_region_do_destroy_venc(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
    }



    rgn_sample_pause();
    return sample_region_do_destroy_venc(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
}

static hi_s32 sample_region_vi_vpss_vo_start(hi_void)
{
    hi_s32 ret;

    ret = sample_region_start_vi(RGN_VDEC_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("start vi failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_region_start_vpss();
    if (ret != HI_SUCCESS) {
        sample_print("start vpss failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VI);
    }
    ret = sample_comm_vi_bind_vpss(RGN_VI_PIPE, RGN_VI_CHN, RGN_VPSS_GRP, RGN_VPSS_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("vi_bind_multi_vpss 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VPSS | REGION_STOP_VI);
    }
    sample_comm_vo_get_def_config(&g_rgn_vo_cfg);
    g_rgn_vo_cfg.intf_sync = g_rgn_intf_sync;
    ret = sample_comm_vo_start_vo(&g_rgn_vo_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("start vo failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_UNBIND_VI_VPSS | REGION_STOP_VPSS | REGION_STOP_VI);
    }
    ret = sample_comm_vpss_bind_vo(RGN_VPSS_GRP, RGN_VPSS_CHN, SAMPLE_VO_LAYER_VHD0, RGN_VO_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("vpss bind vo failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VO | REGION_UNBIND_VI_VPSS | REGION_STOP_VPSS | REGION_STOP_VI);
    }

    return ret;
}

static hi_s32 sample_region_vdec_vpss_vo_start(hi_void)
{
    hi_s32 ret;

  ret = sample_region_start_vi(RGN_VDEC_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("start vdec failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_region_start_vpss();
    if (ret != HI_SUCCESS) {
        sample_print("start vpss failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VDEC);
    }
        ret = sample_comm_vi_bind_vpss(RGN_VI_PIPE, RGN_VI_CHN, RGN_VPSS_GRP, RGN_VPSS_CHN);

    if (ret != HI_SUCCESS) {
        sample_print("vi_bind_multi_vpss 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VPSS | REGION_STOP_VDEC);
    }
    sample_comm_vo_get_def_config(&g_rgn_vo_cfg);
    g_rgn_vo_cfg.intf_sync = g_rgn_intf_sync;
    ret = sample_comm_vo_start_vo(&g_rgn_vo_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("start vo failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_UNBIND_VDEC_VPSS | REGION_STOP_VPSS | REGION_STOP_VDEC);
    }
    ret = sample_comm_vpss_bind_vo(RGN_VPSS_GRP, RGN_VPSS_CHN, SAMPLE_VO_LAYER_VHD0, RGN_VO_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("vpss bind vo failed with 0x%x!\n", ret);
        return sample_region_do_stop(REGION_STOP_VO | REGION_UNBIND_VDEC_VPSS | REGION_STOP_VPSS | REGION_STOP_VDEC);
    }
    sample_region_start_send_stream();

    return ret;
}

static hi_s32 sample_region_vi_vpss_vo_end(hi_void)
{
    hi_s32 ret;

    ret = sample_comm_vpss_un_bind_vo(RGN_VPSS_GRP, RGN_VPSS_CHN, SAMPLE_VO_LAYER_VHD0, RGN_VO_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_un_bind_vo failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_comm_vo_stop_vo(&g_rgn_vo_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_vo failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_comm_vi_un_bind_vpss(RGN_VI_PIPE, RGN_VI_CHN, RGN_VPSS_GRP, RGN_VPSS_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vi_un_bind_vpss failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_region_stop_vpss();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_stop_vpss failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_region_do_stop(REGION_STOP_VI);
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_do_stop vi failed with 0x%x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_region_vdec_vpss_vo_end(hi_void)
{
    hi_s32 ret;

    ret = sample_comm_vpss_un_bind_vo(RGN_VPSS_GRP, RGN_VPSS_CHN, SAMPLE_VO_LAYER_VHD0, RGN_VO_CHN);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vpss_un_bind_vo failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_comm_vo_stop_vo(&g_rgn_vo_cfg);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vo_stop_vo failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_comm_vdec_un_bind_vpss(RGN_VDEC_CHN, RGN_VPSS_GRP);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_vdec_un_bind_vpss failed with 0x%x!\n", ret);
        return ret;
    }
    sample_region_stop_send_stream();
    ret = sample_region_stop_vpss();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_stop_vpss failed with 0x%x!\n", ret);
        return ret;
    }
    ret = sample_region_stop_vdec(RGN_VDEC_CHN_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_stop_vdec failed with 0x%x!\n", ret);
        return ret;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_region_do_destroy_vo(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn,
    region_op_flag flag)
{
    hi_s32 ret;

    (hi_void)sample_region_do_destroy(handle_num, type, chn, flag);
    ret = sample_region_vdec_vpss_vo_end();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_mpp_vi_vpss_venc_end failed!\n");
    }

    return ret;
}

static hi_s32 sample_region_do_destroy_vi_vpss_vo(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn,
    region_op_flag flag)
{
    hi_s32 ret;

    (hi_void)sample_region_do_destroy(handle_num, type, chn, flag);
    ret = sample_region_vi_vpss_vo_end();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_mpp_vi_vpss_venc_end failed!\n");
    }

    return ret;
}

static hi_s32 sample_region_vi_vpss_vo(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn,
    region_op_flag op_flag)
{
    hi_s32 i, ret, min_handle;

    rgn_check_handle_num_return(handle_num);

    ret = sample_region_vi_vpss_vo_start();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_mpp_vi_vpss_vo_start failed!\n");
        return ret;
    }
    ret = sample_comm_region_create(handle_num, type);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_region_create failed!\n");
        return sample_region_do_destroy_vo(handle_num, type, chn, REGION_DESTROY);
    }

    ret = sample_comm_region_attach(handle_num, type, chn, op_flag);
    if (ret != HI_SUCCESS) {
        if (op_flag & REGION_OP_CHN) {
            sample_print("sample_comm_region_attach failed!\n");
            return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
        } else if (op_flag & REGION_OP_DEV) {
            sample_print("sample_comm_region_attach_to_dev failed!\n");
            return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_DEV | REGION_DESTROY);
        }
    }

    min_handle = sample_comm_region_get_min_handle(type);
    if (sample_comm_check_min(min_handle) != HI_SUCCESS) {
        sample_print("min_handle(%d) should be in [0, %d).\n", min_handle, HI_RGN_HANDLE_MAX);
        return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
    }
    if (type == HI_RGN_OVERLAY) {
        for (i = min_handle; i < min_handle + handle_num; i++) {
            ret = sample_comm_region_set_bit_map(i, g_path_bmp);
            if (ret != HI_SUCCESS) {
                sample_print("sample_comm_region_set_bit_map failed!\n");
                return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
            }
        }
    } else if (type == HI_RGN_OVERLAYEX) {
        for (i = min_handle; i < min_handle + handle_num; i++) {
            ret = sample_comm_region_get_up_canvas(i, g_path_bmp);
            if (ret != HI_SUCCESS) {
                sample_print("sample_comm_region_get_up_canvas failed!\n");
                return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
            }
        }
    }
    rgn_sample_pause();

    return sample_region_do_destroy_vi_vpss_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
}

static hi_s32 sample_region_vdec_vpss_vo(hi_s32 handle_num, hi_rgn_type type, hi_mpp_chn *chn,
    region_op_flag op_flag)
{
    hi_s32 i, ret, min_handle;

    rgn_check_handle_num_return(handle_num);

    ret = sample_region_vdec_vpss_vo_start();
    if (ret != HI_SUCCESS) {
        sample_print("sample_region_mpp_vi_vpss_vo_start failed!\n");
        return ret;
    }
    ret = sample_comm_region_create(handle_num, type);
    if (ret != HI_SUCCESS) {
        sample_print("sample_comm_region_create failed!\n");
        return sample_region_do_destroy_vo(handle_num, type, chn, REGION_DESTROY);
    }

    ret = sample_comm_region_attach(handle_num, type, chn, op_flag);
    if (ret != HI_SUCCESS) {
        if (op_flag & REGION_OP_CHN) {
            sample_print("sample_comm_region_attach failed!\n");
            return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
        } else if (op_flag & REGION_OP_DEV) {
            sample_print("sample_comm_region_attach_to_dev failed!\n");
            return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_DEV | REGION_DESTROY);
        }
    }

    min_handle = sample_comm_region_get_min_handle(type);
    if (sample_comm_check_min(min_handle) != HI_SUCCESS) {
        sample_print("min_handle(%d) should be in [0, %d).\n", min_handle, HI_RGN_HANDLE_MAX);
        return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
    }
    if (type == HI_RGN_OVERLAY) {
        for (i = min_handle; i < min_handle + handle_num; i++) {
            ret = sample_comm_region_set_bit_map(i, g_path_bmp);
            if (ret != HI_SUCCESS) {
                sample_print("sample_comm_region_set_bit_map failed!\n");
                return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
            }
        }
    } else if (type == HI_RGN_OVERLAYEX) {
        for (i = min_handle; i < min_handle + handle_num; i++) {
            ret = sample_comm_region_get_up_canvas(i, g_path_bmp);
            if (ret != HI_SUCCESS) {
                sample_print("sample_comm_region_get_up_canvas failed!\n");
                return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
            }
        }
    }
    rgn_sample_pause();

    return sample_region_do_destroy_vo(handle_num, type, chn, REGION_OP_CHN | REGION_DESTROY);
}

static hi_s32 sample_region_vpss_overlayex(hi_void)
{
    hi_s32 handle_num;
    hi_rgn_type type;
    hi_mpp_chn chn;

    handle_num = RGN_HANDLE_NUM_1; //设置叠图数量，这里为1，表示只有一个区域任务
    type = HI_RGN_OVERLAYEX; //设置叠图类型，这里为OVERLAYEX
    chn.mod_id = HI_ID_VPSS;
    chn.dev_id = 0;
    chn.chn_id = 0; 
    g_path_bmp = MM_BMP;  //设置文件路径
    g_path_bmp2 = "./res/mm2.bmp";  //设置文件路径
    return sample_region_vi_vpss_venc(handle_num, type, &chn);
    // return sample_region_vdec_vpss_vo(handle_num, type, &chn, REGION_OP_CHN);
}




hi_s32 sample_vo_oled_gpio(unsigned int gpio_chip_num, unsigned int gpio_offset_num,
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






/*
 * function : show usage
 */
int main() {
    // 注册信号处理函数
    signal(SIGINT, handle_exit_signal);
    signal(SIGTERM, handle_exit_signal);

    // 调用功能函数
    hi_s32 ret = sample_region_vpss_overlayex();

    return ret;
}




