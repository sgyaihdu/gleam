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
#include <sys/time.h>
#include <linux/fb.h>
#include "hi_mpi_thermo.h"
#ifdef OT_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ot_i2c.h"
#endif

#include "sample_comm.h"
#include "sample_ipc.h"
#include "securec.h"
#include "hi_mpi_ae.h"
#include "hi_mpi_awb.h"
#include "hi_common_aidestrip.h"
#include "hi_mpi_aidestrip.h"
#include "hi_mpi_nuc.h"

/* define */
#define SOURCE_PATH "./"
#define THERMO_WIDTH_400 400
#define THERMO_HEIGHT_308 308

#define THERMO_WIDTH 384
#define THERMO_HEIGHT 288

#define THERMO_TEMP_DIFF 100
#define THERMO_TIME_DIFF 120

#define div_0_to_1(a)   (((a) == 0) ? 1 : (a))

/* globals */
static volatile sig_atomic_t g_sig_flag = 0;
hi_bool g_thread_start = 1;

static hi_char *g_aidestrip_model_file = "./model/aidestrip_model_384x288.om";
static hi_char *g_nuc_model_file = "./model/nuc_model_384x288.om";

hi_bool g_thread_ooc_fpn_start = 1;
hi_bool g_fpn_calibrate_done = 0;
static int g_fd[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = -1};
static hi_u8 ooc_data[308][400] = {{0}};
static hi_u32 tmp_raw[308][400] = {{0}};

#define GST412C_I2C_ADDR    0x84
#define GST412C_ADDR_BYTE   1
#define GST412C_DATA_BYTE   1
#define GST412C_DEV_NUM     6
#define I2C_DEV_FILE_NUM   16
#define I2C_BUF_NUM         8
#define I2C_MSG_CNT         2
#define I2C_READ_BUF_LEN    4
#define I2C_RDWR       0x0707
#define I2C_READ_STATUS_OK  2
#define THERMO_FILE_NAME_LEN 50
#define LEN 10
#define USER_BLC 6000
#define NUC_BIAS 8350
#define THERMO_DATA_COUNT 2

pthread_t g_send_pid_nuc, g_get_pid_nuc;

hi_phys_addr_t g_fpn_phys_addr;
hi_void *g_fpn_virt_addr = TD_NULL;

hi_phys_addr_t g_k_phys_addr;
hi_void *g_k_virt_addr = TD_NULL;

hi_nuc_model g_nuc_model_info = {0};
td_s32 g_nuc_model_id;

struct i2c_rdwr_ioctl_data {
    struct i2c_msg  *msgs;  /* pointers to i2c_msgs */
    __u32 nmsgs;            /* number of i2c_msgs */
};

struct i2c_rdwr_args {
    unsigned int i2c_num;
    unsigned int dev_addr;
    unsigned int reg_addr;
    unsigned int reg_addr_end;
    unsigned int reg_width;
    unsigned int data_width;
    unsigned int reg_step;
};

/* this configuration is used to adjust the size and number of buffer(VB).  */
static sample_vb_param g_vb_param = {
    .vb_size = {1920, 1080},
    .pixel_format = {HI_PIXEL_FORMAT_RGB_BAYER_12BPP, HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420},
    .compress_mode = {HI_COMPRESS_MODE_LINE, HI_COMPRESS_MODE_SEG},
    .video_format = {HI_VIDEO_FORMAT_LINEAR, HI_VIDEO_FORMAT_LINEAR},
    .blk_num = {4, 16}
};

static sampe_sys_cfg g_thermo_sys_cfg = {
    .route_num = 1,
    .mode_type = HI_VI_OFFLINE_VPSS_OFFLINE,
    .nr_pos = HI_3DNR_POS_VI,
    .vi_fmu = {0},
    .vpss_fmu = {0},
};

static sample_vo_cfg g_vo_cfg = {
    .vo_dev            = SAMPLE_VO_DEV_UHD,
    .vo_layer          = SAMPLE_VO_LAYER_VHD0,
    .vo_intf_type      = HI_VO_INTF_BT1120,
    .intf_sync         = HI_VO_OUT_1080P60,
    .bg_color          = COLOR_RGB_BLACK,
    .pix_format        = HI_PIXEL_FORMAT_YVU_SEMIPLANAR_420,
    .disp_rect         = {0, 0, 1920, 1080},
    .image_size        = {1920, 1080},
    .vo_part_mode      = HI_VO_PARTITION_MODE_SINGLE,
    .dis_buf_len       = 3, /* 3: def buf len for single */
    .dst_dynamic_range = HI_DYNAMIC_RANGE_SDR8,
    .vo_mode           = VO_MODE_1MUX,
    .compress_mode     = HI_COMPRESS_MODE_NONE,
};

static sample_comm_venc_chn_param g_venc_chn_param = {
    .frame_rate           = 30, /* 30 is a number */
    .stats_time           = 2,  /* 2 is a number */
    .gop                  = 60, /* 60 is a number */
    .venc_size            = {1920, 1080},
    .size                 = -1,
    .profile              = 0,
    .is_rcn_ref_share_buf = HI_FALSE,
    .gop_attr             = {
        .gop_mode = HI_VENC_GOP_MODE_NORMAL_P,
        .normal_p = {2},
    },
    .type                 = HI_PT_H265,
    .rc_mode              = SAMPLE_RC_CBR,
};

static sample_vi_fpn_calibration_cfg g_calibration_cfg = {
    .threshold     = 4095,
    .frame_num     = 16,
    .fpn_type      = HI_ISP_FPN_TYPE_FRAME,
    .pixel_format  = HI_PIXEL_FORMAT_RGB_BAYER_16BPP,
    .compress_mode = HI_COMPRESS_MODE_NONE,
};

static sample_vi_fpn_correction_cfg g_correction_cfg = {
    .op_mode       = HI_OP_MODE_MANUAL,
    .fpn_type      = HI_ISP_FPN_TYPE_FRAME,
    .strength      = 256,
    .pixel_format  = HI_PIXEL_FORMAT_RGB_BAYER_16BPP,
    .compress_mode = HI_COMPRESS_MODE_NONE,
};

sample_vi_user_frame_info g_user_frame_info; // OOC frame

#define sample_thermo_check_fp_return(fp, file_name)                                   \
    do {                                                       \
        if (fp == NULL) {    \
            printf("Cannot open %s.\n", file_name); \
            return;                   \
        }                                                      \
    } while (0)

/* function */
hi_void sample_thermo_gpio_export(hi_u32 gpio_chip_num, hi_u32 gpio_offset)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char file_name[THERMO_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, THERMO_FILE_NAME_LEN, "/sys/class/gpio/export");
    if (ret < 0) {
        printf("export sprintf_s failed with %d.\n", ret);
        return;
    }
    fp = fopen(file_name, "w");
    sample_thermo_check_fp_return(fp, file_name);
    ret = fprintf(fp, "%u", gpio_num);
    if (ret < 0) {
        printf("print gpio_num failed!!!\n");
    }

    ret = fclose(fp);
    if (ret != 0) {
        printf("export close fp err ret:%d\n", ret);
    }
    return;
}

hi_void sample_thermo_gpio_dir(hi_u32 gpio_chip_num, hi_u32 gpio_offset)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char file_name[THERMO_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, THERMO_FILE_NAME_LEN, "/sys/class/gpio/gpio%u/direction", gpio_num);
    if (ret < 0) {
        printf("dir sprintf_s failed with %d.\n", ret);
        return;
    }
    fp = fopen(file_name, "rb+");
    sample_thermo_check_fp_return(fp, file_name);
    ret = fprintf(fp, "out");
    if (ret < 0) {
        printf("print out failed!!!\n");
    }
    ret = fclose(fp);
    if (ret != 0) {
        printf("dir close fp err, ret:%d\n", ret);
    }

    return;
}

hi_void sample_thermo_gpio_write(hi_u32 gpio_chip_num, hi_u32 gpio_offset, hi_u32 gpio_out_val)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char buf[LEN];
    char file_name[THERMO_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, THERMO_FILE_NAME_LEN, "/sys/class/gpio/gpio%u/value", gpio_num);
    if (ret < 0) {
        printf("write sprintf_s failed with %d.\n", ret);
        return;
    }

    fp = fopen(file_name, "rb+");
    sample_thermo_check_fp_return(fp, file_name);

    if (gpio_out_val) {
        ret = strcpy_s(buf, LEN, "1");
        if (ret != EOK) {
            printf("strcpy_s buf 1 failed.\n");
        }
    } else {
        ret = strcpy_s(buf, LEN, "0");
        if (ret != EOK) {
            printf("strcpy_s buf 0 failed.\n");
        }
    }
    ret = fwrite(buf, sizeof(char), sizeof(buf) - 1, fp);
    if (ret != (sizeof(buf) - 1)) {
        printf("%s: gpio%u_%u = %s, fwrite err ret:%d\n", __func__, gpio_chip_num, gpio_offset, buf, ret);
    }
    ret = fclose(fp);
    if (ret != 0) {
        printf("write close fp err, ret:%d\n", ret);
    }

    return;
}

hi_void sample_thermo_gpio_unexport(hi_u32 gpio_chip_num, hi_u32 gpio_offset)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;
    char file_name[THERMO_FILE_NAME_LEN];
    hi_u32 gpio_num = gpio_chip_num * 8 + gpio_offset; // 8 pins

    ret = sprintf_s(file_name, THERMO_FILE_NAME_LEN, "/sys/class/gpio/unexport");
    if (ret < 0) {
        printf("unexport sprintf_s failed with %d.\n", ret);
        return;
    }

    fp = fopen(file_name, "w");
    sample_thermo_check_fp_return(fp, file_name);
    ret = fprintf(fp, "%u", gpio_num);
    if (ret < 0) {
        printf("print unexport gpio_num failed!!!\n");
    }
    ret = fclose(fp);
    if (ret != 0) {
        printf("unexport close fp err, ret:%d\n", ret);
    }

    return;
}

hi_s32 thermo_gst412c_i2c_init(ot_vi_pipe vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        return TD_SUCCESS;
    }
#ifdef OT_GPIO_I2C
    g_fd[vi_pipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open gpioi2c_ex error!\n");
        return TD_FAILURE;
    }
#else
    {
        int ret;
        char dev_file[I2C_DEV_FILE_NUM] = {0};
        td_u8 dev_num;

        dev_num = GST412C_DEV_NUM;

        ret = snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);
        if (ret < 0) {
            isp_err_trace("snprintf_s dev_file failed\n");
            return TD_FAILURE;
        }

        g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
        if (g_fd[vi_pipe] < 0) {
            isp_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
            return TD_FAILURE;
        }

        ret = ioctl(g_fd[vi_pipe], OT_I2C_SLAVE_FORCE, (GST412C_I2C_ADDR >> 1));
        if (ret < 0) {
            isp_err_trace("I2C_SLAVE_FORCE error!\n");
            close(g_fd[vi_pipe]);
            g_fd[vi_pipe] = -1;
            return ret;
        }
    }
#endif

    return TD_SUCCESS;
}

hi_u32 i2c_ioc_init(struct i2c_rdwr_ioctl_data *rdwr, unsigned char *buf, size_t buf_size, struct i2c_rdwr_args args)
{
    if (memset_s(buf, buf_size, 0, I2C_READ_BUF_LEN) != EOK) {
        printf("memset_s fail!\n");
        return -1;
    }
    rdwr->msgs[0].addr = args.dev_addr;
    rdwr->msgs[0].flags = 0;
    rdwr->msgs[0].len = args.reg_width;
    rdwr->msgs[0].buf = buf;

    rdwr->msgs[1].addr = args.dev_addr;
    rdwr->msgs[1].flags = 0;
    rdwr->msgs[1].flags |= I2C_M_RD;
    rdwr->msgs[1].len = args.data_width;
    rdwr->msgs[1].buf = buf;

    return 0;
}

hi_u32 thermo_gst412c_read_register(hi_vi_pipe vi_pipe, hi_u32 addr)
{
    if (g_fd[vi_pipe] < 0) {
        sample_print("gst412c_read_register fd not opened!\n");
        return HI_FAILURE;
    }
    struct i2c_rdwr_args args;
    hi_u32 cur_addr;
    hi_u32 data;
    unsigned char buf[I2C_READ_BUF_LEN];
    static struct i2c_rdwr_ioctl_data rdwr;
    static struct i2c_msg msg[I2C_MSG_CNT];

    rdwr.msgs = &msg[0];
    rdwr.nmsgs = (__u32)I2C_MSG_CNT;
    args.i2c_num = GST412C_DEV_NUM;
    args.dev_addr = (GST412C_I2C_ADDR >> 1);
    args.reg_addr = addr;
    args.reg_addr_end = addr;
    args.reg_width = GST412C_ADDR_BYTE;
    args.data_width = GST412C_DATA_BYTE;
    args.reg_step = 1;
    if (i2c_ioc_init(&rdwr, buf, sizeof(buf), args) != 0) {
        return -1;
    }

    for (cur_addr = args.reg_addr; cur_addr <= args.reg_addr_end; cur_addr += args.reg_step) {
        if (args.reg_width == 2) {  /* 2 byte */
            buf[0] = (cur_addr >> 8) & 0xff;  /* shift 8 */
            buf[1] = cur_addr & 0xff;
        } else {
            buf[0] = cur_addr & 0xff;
        }

        if (ioctl(g_fd[vi_pipe], I2C_RDWR, &rdwr) != I2C_READ_STATUS_OK) {
            printf("CMD_I2C_READ error!\n");
            return -1;
        }

        if (args.data_width == 2) {  /* 2 byte */
            data = buf[1] | (buf[0] << 8);  /* shift 8 */
        } else {
            data = buf[0];
        }
    }

    return data;
}

td_s32 thermo_gst412c_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return TD_SUCCESS;
    }

#ifdef OT_GPIO_I2C
    i2c_data.dev_addr = GST412C_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = GST412C_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = GST412C_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        isp_err_trace("GPIO-I2C write failed!\n");
        return ret;
    }
#else
    td_u32 idx = 0;
    td_s32 ret;
    td_u8 buf[I2C_BUF_NUM];

    buf[idx] = addr & 0xff;
    idx++;

    buf[idx] = data & 0xff;

    ret = write(g_fd[vi_pipe], buf, GST412C_ADDR_BYTE + GST412C_DATA_BYTE);
    if (ret < 0) {
        isp_err_trace("I2C_WRITE error!\n");
        return TD_FAILURE;
    }

#endif
    return TD_SUCCESS;
}

static hi_s32 sample_thermo_get_ooc_frame_blk(sample_vi_get_frame_vb_cfg *get_frame_vb_cfg,
    sample_vi_user_frame_info user_frame_info[], hi_u32 frame_cnt)
{
    hi_u32 i = 0, y_stride, blk_size;
    hi_u64 phys_addr;
    hi_u8 *virt_addr;
    hi_vb_pool pool_id;
    hi_vb_pool_cfg vb_pool_cfg;

    /* calc OOC frame */
    y_stride  = sample_comm_vi_get_raw_stride(get_frame_vb_cfg->pixel_format,
        get_frame_vb_cfg->size.width, 1, HI_DEFAULT_ALIGN);
    blk_size = HI_ALIGN_UP((get_frame_vb_cfg->size.height * y_stride), HI_DEFAULT_ALIGN);

    memset_s(&vb_pool_cfg, sizeof(hi_vb_pool_cfg), 0, sizeof(hi_vb_pool_cfg));
    vb_pool_cfg.blk_size   = blk_size;
    vb_pool_cfg.blk_cnt    = frame_cnt;
    vb_pool_cfg.remap_mode = HI_VB_REMAP_MODE_NONE;
    pool_id = hi_mpi_vb_create_pool(&vb_pool_cfg);
    if (pool_id == HI_VB_INVALID_POOL_ID) {
        return HI_FAILURE;
    }

    for (i = 0; i < frame_cnt; i++) {
        hi_vb_blk vb_blk = hi_mpi_vb_get_blk(pool_id, blk_size, HI_NULL);
        if (vb_blk == HI_VB_INVALID_HANDLE) {
            return HI_FAILURE;
        }

        phys_addr = hi_mpi_vb_handle_to_phys_addr(vb_blk);
        virt_addr = (hi_u8 *)hi_mpi_sys_mmap(phys_addr, blk_size);
        if (virt_addr == HI_NULL) {
            return HI_FAILURE;
        }

        hi_video_frame_info *frame_info = &user_frame_info[i].frame_info;

        frame_info->pool_id = pool_id;
        frame_info->mod_id  = HI_ID_VI;

        frame_info->video_frame.phys_addr[0] = phys_addr;
        frame_info->video_frame.phys_addr[1] = frame_info->video_frame.phys_addr[0];
        frame_info->video_frame.virt_addr[0] = virt_addr;
        frame_info->video_frame.virt_addr[1] = frame_info->video_frame.virt_addr[0];
        frame_info->video_frame.stride[0]    = y_stride;
        frame_info->video_frame.stride[1]    = y_stride; /* same as y_stride */

        frame_info->video_frame.width         = get_frame_vb_cfg->size.width;
        frame_info->video_frame.height        = get_frame_vb_cfg->size.height;
        frame_info->video_frame.pixel_format  = get_frame_vb_cfg->pixel_format;
        frame_info->video_frame.video_format  = get_frame_vb_cfg->video_format;
        frame_info->video_frame.compress_mode = get_frame_vb_cfg->compress_mode;
        frame_info->video_frame.dynamic_range = get_frame_vb_cfg->dynamic_range;
        frame_info->video_frame.field         = HI_VIDEO_FIELD_FRAME;
        frame_info->video_frame.color_gamut   = HI_COLOR_GAMUT_BT601;

        user_frame_info[i].vb_blk   = vb_blk;
        user_frame_info[i].blk_size = blk_size;
    }

    return HI_SUCCESS;
}

static hi_s32 sample_thermo_release_ooc_frame_blk(sample_vi_user_frame_info user_frame_info[], hi_u32 frame_cnt)
{
    hi_s32 ret;
    hi_u32 i;
    hi_vb_pool pool_id;
    hi_vb_blk vb_blk;

    for (i = 0; i < frame_cnt; i++) {
        hi_u32 blk_size = user_frame_info[i].blk_size;
        hi_void *virt_addr = user_frame_info[i].frame_info.video_frame.virt_addr[0];
        ret = hi_mpi_sys_munmap(virt_addr, blk_size);
        if (ret != HI_SUCCESS) {
            printf("hi_mpi_sys_munmap failure!\n");
        }

        vb_blk = user_frame_info[i].vb_blk;
        ret = hi_mpi_vb_release_blk(vb_blk);
        if (ret != HI_SUCCESS) {
            printf("hi_mpi_vb_release_blk block 0x%x failure\n", vb_blk);
        }

        user_frame_info[i].vb_blk = HI_VB_INVALID_HANDLE;
    }
    sleep(2); // wait for 2 sec

    pool_id = user_frame_info[0].frame_info.pool_id;
    hi_mpi_vb_destroy_pool(pool_id);
    user_frame_info[0].frame_info.pool_id = HI_VB_INVALID_POOL_ID;

    return HI_SUCCESS;
}

static hi_u8 g_t3_sns_cfg[] = {
    0x40, 0x10, 0x03, 0xFF,
    0xC0, 0x01, 0x8C, 0xAB,
    0x0E, 0x10, 0x82, 0x0F,
    0x11, 0x60, 0x84, 0xB6,
    0xF4, 0xBC, 0x20, 0x00,
    0x00, 0x07, 0xFF, 0xBF,
    0xFF, 0xFF, 0xFF, 0xF4,
    0x78, 0x00, 0x10, 0xAA,
    0x1C, 0x00, 0x8A, 0xA0,
    0xA3, 0xD0, 0x80, 0x00,
    0x4F, 0x83, 0x34, 0xB0,
    0x32, 0x2C, 0x1E, 0xF2,
};

static hi_void sample_thermo_set_thermo_t3_cfg(hi_vi_dev vi_dev, sample_vi_user_frame_info *user_frame_info)
{
    hi_s32 ret;
    hi_vi_thermo_sns_attr sns_attr;
    hi_char *file_name = SOURCE_PATH"raw/ooc_gst412c_test_5_fanzhuan_demo_yes.raw";

    sns_attr.work_mode = HI_VI_THERMO_WORK_MODE_T3;
    sns_attr.cfg_num = (hi_u32)(sizeof(g_t3_sns_cfg)/sizeof(hi_u8));
    (hi_void)memcpy_s(&sns_attr.sns_cfg, sizeof(hi_u8) * HI_VI_MAX_SNS_CFG_NUM, g_t3_sns_cfg, sizeof(g_t3_sns_cfg));

    ret = sample_comm_vi_read_raw_frame(file_name, user_frame_info, 1);
    if (ret != HI_SUCCESS) {
        printf("read frame file failed\n");
        return;
    }

    memcpy_s(&sns_attr.ooc_frame_info, sizeof(hi_video_frame_info),
             &user_frame_info->frame_info, sizeof(hi_video_frame_info));
    sns_attr.frame_rate = 30; // 30 fps
    sns_attr.sd_mux[0] = HI_VI_SD_MUX_T2_SDA0; // pin_mux0:4
    sns_attr.sd_mux[1] = HI_VI_SD_MUX_T2_SDA1; // pin_mux1:5
    sns_attr.sd_mux[2] = HI_VI_SD_MUX_T2_SDA2; // pin_mux2:6
    sns_attr.sd_mux[3] = HI_VI_SD_MUX_T2_FS; // pin_mux3:7

    hi_mpi_vi_set_thermo_sns_attr(vi_dev, &sns_attr);
    sleep(5); // wait 5 sec for stable
}

static hi_void sample_get_char(hi_char *s)
{
    if (g_sig_flag == 1) {
        return;
    }

    printf("---------------press any key to %s!---------------\n", s);
    (hi_void)getchar();
}

/* define SAMPLE_MEM_SHARE_ENABLE, when use tools to dump YUV/RAW. */
#ifdef SAMPLE_MEM_SHARE_ENABLE
static hi_void sample_thermo_init_mem_share(hi_void)
{
    hi_u32 i;
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) != HI_SUCCESS) {
        sample_print("get common pool_id failed!\n");
        return;
    }
    for (i = 0; i < pools_id.pool_cnt; ++i) {
        hi_mpi_vb_pool_share_all(pools_id.pool[i]);
    }
}
#endif

static hi_s32 sample_thermo_sys_init(hi_void)
{
    hi_vb_cfg vb_cfg;
    hi_u32 supplement_config = HI_VB_SUPPLEMENT_BNR_MOT_MASK | HI_VB_SUPPLEMENT_MOTION_DATA_MASK;

    sample_comm_sys_get_default_vb_cfg(&g_vb_param, &vb_cfg);
    if (sample_comm_sys_init_with_vb_supplement(&vb_cfg, supplement_config) != HI_SUCCESS) {
        return HI_FAILURE;
    }

#ifdef SAMPLE_MEM_SHARE_ENABLE
    sample_thermo_init_mem_share();
#endif

    if (sample_comm_vi_set_vi_vpss_mode(g_thermo_sys_cfg.mode_type, HI_VI_AIISP_MODE_DEFAULT) != HI_SUCCESS) {
        goto sys_exit;
    }

    if (hi_mpi_sys_set_3dnr_pos(g_thermo_sys_cfg.nr_pos) != HI_SUCCESS) {
        goto sys_exit;
    }

    return HI_SUCCESS;
sys_exit:
    sample_comm_sys_exit();
    return HI_FAILURE;
}

static hi_s32 sample_thermo_start_vpss(hi_vpss_grp grp, sample_vpss_cfg *vpss_cfg)
{
    hi_s32 ret;
    sample_vpss_chn_attr vpss_chn_attr = {0};

    memcpy_s(&vpss_chn_attr.chn_attr[0], sizeof(hi_vpss_chn_attr) * HI_VPSS_MAX_PHYS_CHN_NUM,
        vpss_cfg->chn_attr, sizeof(hi_vpss_chn_attr) * HI_VPSS_MAX_PHYS_CHN_NUM);
    if (g_thermo_sys_cfg.vpss_fmu[grp] == HI_FMU_MODE_WRAP) {
        vpss_chn_attr.chn0_wrap = HI_TRUE;
    }
    memcpy_s(vpss_chn_attr.chn_enable, sizeof(vpss_chn_attr.chn_enable),
        vpss_cfg->chn_en, sizeof(vpss_chn_attr.chn_enable));
    vpss_chn_attr.chn_array_size = HI_VPSS_MAX_PHYS_CHN_NUM;
    ret = sample_common_vpss_start(grp, &vpss_cfg->grp_attr, &vpss_chn_attr);
    if (ret != HI_SUCCESS) {
        return ret;
    }

    if (vpss_cfg->nr_attr.enable == HI_TRUE) {
        if (hi_mpi_vpss_set_grp_3dnr_attr(grp, &vpss_cfg->nr_attr) != HI_SUCCESS) {
            goto stop_vpss;
        }
    }

    if (g_thermo_sys_cfg.mode_type != HI_VI_ONLINE_VPSS_ONLINE) {
        hi_gdc_param gdc_param = {0};
        gdc_param.in_size.width  = g_vb_param.vb_size.width;
        gdc_param.in_size.height = g_vb_param.vb_size.height;
        gdc_param.cell_size = HI_LUT_CELL_SIZE_16;
        if (hi_mpi_vpss_set_grp_gdc_param(grp, &gdc_param) != HI_SUCCESS) {
            goto stop_vpss;
        }
    }

    return HI_SUCCESS;
stop_vpss:
    sample_common_vpss_stop(grp, vpss_cfg->chn_en, HI_VPSS_MAX_PHYS_CHN_NUM);
    return HI_FAILURE;
}

static hi_void sample_thermo_stop_vpss(hi_vpss_grp grp)
{
    hi_bool chn_enable[HI_VPSS_MAX_PHYS_CHN_NUM] = {HI_TRUE, HI_FALSE, HI_FALSE, HI_FALSE};

    sample_common_vpss_stop(grp, chn_enable, HI_VPSS_MAX_PHYS_CHN_NUM);
}

static hi_s32 sample_thermo_start_venc(hi_venc_chn venc_chn[], hi_u32 chn_num, size_t venc_length)
{
    hi_s32 i;
    hi_s32 ret;

    sample_comm_vi_get_size_by_sns_type(SENSOR1_TYPE, &g_venc_chn_param.venc_size);
    for (i = 0; i < (hi_s32)chn_num && i < (hi_s32)venc_length; i++) {
        ret = sample_comm_venc_start(venc_chn[i], &g_venc_chn_param);
        if (ret != HI_SUCCESS) {
            goto exit;
        }
    }

    ret = sample_comm_venc_start_get_stream(venc_chn, chn_num);
    if (ret != HI_SUCCESS) {
        goto exit;
    }

    return HI_SUCCESS;

exit:
    for (i = i - 1; i >= 0; i--) {
        sample_comm_venc_stop(venc_chn[i]);
    }
    return HI_FAILURE;
}

static hi_void sample_thermo_stop_venc(hi_venc_chn venc_chn[], hi_u32 chn_num, size_t venc_length)
{
    hi_u32 i;

    sample_comm_venc_stop_get_stream(chn_num);

    for (i = 0; i < chn_num && i < venc_length; i++) {
        sample_comm_venc_stop(venc_chn[i]);
    }
}

static hi_s32 sample_thermo_start_vo(sample_vo_mode vo_mode)
{
    g_vo_cfg.vo_mode = vo_mode;

    return sample_comm_vo_start_vo(&g_vo_cfg);
}

static hi_void sample_thermo_stop_vo(hi_void)
{
    sample_comm_vo_stop_vo(&g_vo_cfg);
}

static hi_s32 sample_thermo_start_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num, size_t vpss_length)
{
    hi_u32 i;
    hi_s32 ret;
    sample_vo_mode vo_mode = VO_MODE_1MUX;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */
    size_t venc_length = sizeof(venc_chn) / sizeof(hi_venc_chn);

    if (grp_num > 1) {
        vo_mode = VO_MODE_4MUX;
    }

    ret = sample_thermo_start_venc(venc_chn, grp_num, venc_length);
    if (ret != HI_SUCCESS) {
        goto start_venc_failed;
    }
    for (i = 0; i < grp_num && i < vpss_length; i++) {
        if (g_thermo_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_DIRECT) {
            sample_comm_vpss_bind_venc(vpss_grp[i], HI_VPSS_DIRECT_CHN, venc_chn[i]);
        } else {
            sample_comm_vpss_bind_venc(vpss_grp[i], HI_VPSS_CHN0, venc_chn[i]);
        }
    }

    ret = sample_thermo_start_vo(vo_mode);
    if (ret != HI_SUCCESS) {
        goto start_vo_failed;
    }
    for (i = 0; i < grp_num && i < vpss_length; i++) {
        if (g_thermo_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_WRAP) {
            sample_comm_vpss_bind_vo(vpss_grp[i], HI_VPSS_CHN1, vo_layer, vo_chn[i]);
        } else {
            sample_comm_vpss_bind_vo(vpss_grp[i], HI_VPSS_CHN0, vo_layer, vo_chn[i]);
        }
    }

    return HI_SUCCESS;

start_vo_failed:
    for (i = 0; i < grp_num && i < vpss_length; i++) {
        if (g_thermo_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_DIRECT) {
            sample_comm_vpss_un_bind_venc(vpss_grp[i], HI_VPSS_DIRECT_CHN, venc_chn[i]);
        } else {
            sample_comm_vpss_un_bind_venc(vpss_grp[i], HI_VPSS_CHN0, venc_chn[i]);
        }
    }
    sample_thermo_stop_venc(venc_chn, grp_num, venc_length);
start_venc_failed:
    return HI_FAILURE;
}

static hi_void sample_thermo_stop_venc_and_vo(hi_vpss_grp vpss_grp[], hi_u32 grp_num, size_t vpss_length)
{
    hi_u32 i;
    const hi_vo_layer vo_layer = 0;
    hi_vo_chn vo_chn[4] = {0, 1, 2, 3};     /* 4: max chn num, 0/1/2/3 chn id */
    hi_venc_chn venc_chn[4] = {0, 1, 2, 3}; /* 4: max chn num, 0/1/2/3 chn id */
    size_t venc_length = sizeof(venc_chn) / sizeof(hi_venc_chn);

    for (i = 0; i < grp_num && i < vpss_length; i++) {
        if (g_thermo_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_WRAP) {
            sample_comm_vpss_un_bind_vo(vpss_grp[i], HI_VPSS_CHN1, vo_layer, vo_chn[i]);
        } else {
            sample_comm_vpss_un_bind_vo(vpss_grp[i], HI_VPSS_CHN0, vo_layer, vo_chn[i]);
        }
        if (g_thermo_sys_cfg.vpss_fmu[i] == HI_FMU_MODE_DIRECT) {
            sample_comm_vpss_un_bind_venc(vpss_grp[i], HI_VPSS_DIRECT_CHN, venc_chn[i]);
        } else {
            sample_comm_vpss_un_bind_venc(vpss_grp[i], HI_VPSS_CHN0, venc_chn[i]);
        }
    }

    sample_thermo_stop_venc(venc_chn, grp_num, venc_length);
    sample_thermo_stop_vo();
}

static hi_s32 sample_thermo_start_route(sample_vi_cfg *vi_cfg, sample_vpss_cfg *vpss_cfg, hi_s32 route_num,
    hi_bool aidestrip_en)
{
    hi_s32 i, j, ret;
    hi_vpss_grp vpss_grp[SAMPLE_VIO_MAX_ROUTE_NUM] = {0, 1, 2, 3};
    sample_vi_get_frame_vb_cfg get_frame_vb_cfg;
    size_t vpss_length = sizeof(vpss_grp) / sizeof(hi_vpss_grp);

    if (sample_thermo_sys_init() != HI_SUCCESS) {
        return HI_FAILURE;
    }

    for (i = 0; i < route_num; i++) {
        ret = sample_comm_vi_start_vi(&vi_cfg[i]);
            if (ret != HI_SUCCESS) {
            goto start_vi_failed;
        }
    }

    for (i = 0; (i < route_num) && (aidestrip_en == HI_FALSE); i++) {
        sample_comm_vi_bind_vpss(i, 0, vpss_grp[i], 0);
    }

    for (i = 0; i < route_num; i++) {
        ret = sample_thermo_start_vpss(vpss_grp[i], vpss_cfg);
        if (ret != HI_SUCCESS) {
            goto start_vpss_failed;
        }
    }

    ret = sample_thermo_start_venc_and_vo(vpss_grp, route_num, vpss_length);
    if (ret != HI_SUCCESS) {
        goto start_venc_and_vo_failed;
    }

    get_frame_vb_cfg.size.width = THERMO_WIDTH_400;
    get_frame_vb_cfg.size.height = THERMO_HEIGHT_308;
    get_frame_vb_cfg.pixel_format = HI_PIXEL_FORMAT_RGB_BAYER_8BPP;
    get_frame_vb_cfg.video_format = HI_VIDEO_FORMAT_LINEAR;
    get_frame_vb_cfg.compress_mode = HI_COMPRESS_MODE_NONE;
    get_frame_vb_cfg.dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    sample_thermo_get_ooc_frame_blk(&get_frame_vb_cfg, &g_user_frame_info, 1);

    return HI_SUCCESS;

start_venc_and_vo_failed:
start_vpss_failed:
    for (j = i - 1; j >= 0; j--) {
        sample_thermo_stop_vpss(vpss_grp[j]);
    }
    for (i = 0; (i < route_num) && (aidestrip_en == HI_FALSE); i++) {
        sample_comm_vi_un_bind_vpss(i, 0, vpss_grp[i], 0);
    }
start_vi_failed:
    for (j = i - 1; j >= 0; j--) {
        sample_comm_vi_stop_vi(&vi_cfg[j]);
    }
    sample_comm_sys_exit();
    return HI_FAILURE;
}

static hi_s32 sample_thermo_common_load_mem(ot_aiisp_mem_info *mem, hi_char *model_file)
{
    hi_s32 ret;
    FILE *fp = HI_NULL;

    /* Get model file size */
    fp = fopen(model_file, "rb");
    if (fp == HI_NULL) {
        sample_print("open file %s err!\n", model_file);
        return HI_FAILURE;
    }

    ret = fseek(fp, 0L, SEEK_END);
    if (ret != HI_SUCCESS) {
        sample_print("fseek end failed!\n");
        goto fail_0;
    }

    mem->size = ftell(fp);
    if (mem->size <= 0) {
        sample_print("ftell failed!\n");
        goto fail_0;
    }

    ret = fseek(fp, 0L, SEEK_SET);
    if (ret != HI_SUCCESS) {
        sample_print("fseek set failed!\n");
        goto fail_0;
    }

    /* malloc model file mem */
    ret = hi_mpi_sys_mmz_alloc(&(mem->phys_addr), &(mem->virt_addr), "model_file", HI_NULL, mem->size);
    if (ret != HI_SUCCESS) {
        sample_print("malloc mmz failed!\n");
        goto fail_0;
    }

    ret = fread(mem->virt_addr, mem->size, 1, fp);
    if (ret != 1) {
        sample_print("read model file failed!\n");
        goto fail_1;
    }

    ret = fclose(fp);
    if (ret != 0) {
        sample_print("close file error\n");
    }

    sample_print("load mode %s \n", model_file);
    return HI_SUCCESS;

fail_1:
    hi_mpi_sys_mmz_free(mem->phys_addr, mem->virt_addr);
    mem->phys_addr = 0;
    mem->virt_addr = HI_NULL;
fail_0:
    if (fp != HI_NULL) {
        fclose(fp);
    }
    return HI_FAILURE;
}

hi_void sample_aiisp_unload_mem(hi_aiisp_mem_info *param_mem)
{
    if ((param_mem->phys_addr != 0) && (param_mem->virt_addr != HI_NULL)) {
        (hi_void)hi_mpi_sys_mmz_free(param_mem->phys_addr, param_mem->virt_addr);
    }
}

td_s32 sample_aidestrip_start(hi_vi_pipe vi_pipe, hi_aidestrip_model *model_info, td_s32 *model_id)
{
    hi_s32 ret;
    hi_aidestrip_cfg cfg = {0};

    hi_mpi_aidestrip_init();
    hi_mpi_aidestrip_get_cfg(vi_pipe, &cfg);

    cfg.size.width = THERMO_WIDTH;
    cfg.size.height = THERMO_HEIGHT;
    hi_mpi_aidestrip_set_cfg(vi_pipe, &cfg);
    printf("sample set cfg %u %u \n\n ", cfg.size.width, cfg.size.height);

    ret = sample_thermo_common_load_mem(&model_info->model.mem_info,  g_aidestrip_model_file);
    if (ret != HI_SUCCESS) {
        printf("aidestrip_common_load_mem failed!\n");
        goto exit_load_mem;
    }
    model_info->model.image_size.width = THERMO_WIDTH;
    model_info->model.image_size.height = THERMO_HEIGHT;

    ret = hi_mpi_aidestrip_load_model(model_info, model_id);
    if (ret != HI_SUCCESS) {
        printf("aidestrip_common_load_mem failed!\n");
        goto exit_load;
    }

    ret = hi_mpi_aidestrip_enable(vi_pipe);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_aidestrip_enable failed!\n");
        goto exit_enable;
    }

    return HI_SUCCESS;

exit_enable:
    hi_mpi_aidestrip_unload_model(*model_id);
exit_load:
    sample_aiisp_unload_mem(&model_info->model.mem_info);
exit_load_mem:
    hi_mpi_aidestrip_exit();

    return HI_FAILURE;
}

td_void sample_aidestrip_stop(hi_vi_pipe vi_pipe, hi_aidestrip_model *model_info, td_s32 *model_id)
{
    hi_mpi_aidestrip_disable(vi_pipe);
    hi_mpi_aidestrip_unload_model(*model_id);

    sample_aiisp_unload_mem((hi_aiisp_mem_info *)&model_info->model.mem_info);
    hi_mpi_aidestrip_exit();
}

static hi_void sample_nuc_stop(hi_vi_pipe pipe)
{
    g_thread_start = 0;
    pthread_join(g_send_pid_nuc, HI_NULL);
    pthread_join(g_get_pid_nuc, HI_NULL);
    hi_mpi_nuc_disable(pipe);
    hi_mpi_nuc_unload_model(g_nuc_model_id);

    hi_mpi_sys_mmz_free(g_nuc_model_info.model.mem_info.phys_addr, g_nuc_model_info.model.mem_info.virt_addr);
    hi_mpi_sys_mmz_free(g_fpn_phys_addr, g_fpn_virt_addr);
    hi_mpi_sys_mmz_free(g_k_phys_addr, g_k_virt_addr);
    hi_mpi_nuc_exit();
}

static hi_void sample_thermo_stop_route(sample_vi_cfg *vi_cfg, hi_s32 route_num, hi_bool aidestrip_en, hi_vi_pipe pipe)
{
    hi_s32 i;
    hi_vpss_grp vpss_grp[SAMPLE_VIO_MAX_ROUTE_NUM] = {0, 1, 2, 3};
    size_t vpss_length = sizeof(vpss_grp) / sizeof(hi_vpss_grp);
    sample_thermo_stop_venc_and_vo(vpss_grp, route_num, vpss_length);
    for (i = 0; i < route_num; i++) {
        sample_thermo_stop_vpss(vpss_grp[i]);
        if (aidestrip_en == HI_FALSE) {
            sample_comm_vi_un_bind_vpss(i, 0, vpss_grp[i], 0);
        }
        sample_comm_vi_stop_vi(&vi_cfg[i]);
    }
    sample_thermo_release_ooc_frame_blk(&g_user_frame_info, 1);
    sample_comm_sys_exit();
}

hi_void *sample_aidestrip_send_frame_proc(hi_void *p)
{
    hi_s32 ret;
    hi_vi_pipe vi_pipe = 0;
    hi_vi_chn vi_chn = 0;
    hi_s32 milli_sec = 1000; // 1000 ms
    hi_video_frame_info frame_info = {0};

    hi_vi_chn_attr chn_attr;
    if (hi_mpi_vi_get_chn_attr(vi_pipe, vi_chn, &chn_attr) != HI_SUCCESS) {
        return HI_NULL;
    }

    chn_attr.depth = 4; // set vi depth 4

    if (hi_mpi_vi_set_chn_attr(vi_pipe, vi_chn, &chn_attr) != HI_SUCCESS) {
        return HI_NULL;
    }

    while (g_thread_start) {
        while (g_fpn_calibrate_done != HI_TRUE) {
            sleep(1);
        }
        ret = hi_mpi_vi_get_chn_frame(vi_pipe, vi_chn, &frame_info, milli_sec);
        if (ret != HI_SUCCESS) {
            continue;
        }

        ret = hi_mpi_aidestrip_send_frame(vi_pipe, &frame_info, 0);
        if (ret != HI_SUCCESS) {
            printf("aidestrip send frame failed!\n");
        }

        ret = hi_mpi_vi_release_chn_frame(vi_pipe, vi_chn, &frame_info);
        if (ret != TD_SUCCESS) {
            printf("hi_mpi_vi_release_chn_frame failed!\n");
        }
    }

    return HI_NULL;
}

hi_void *sample_aidestrip_get_frame_proc(hi_void *p)
{
    td_s32 ret;
    hi_s32 milli_sec = 1000; // 1000 ms
    hi_vpss_grp vpss_grp = 0;
    hi_vi_pipe vi_pipe = 0;
    hi_video_frame_info frame_info = {0};

    while (g_thread_start) {
        while (g_fpn_calibrate_done != HI_TRUE) {
            sleep(1);
        }
        ret = hi_mpi_aidestrip_get_frame(vi_pipe, &frame_info, milli_sec);
        if (ret != TD_SUCCESS) {
            continue;
        }

        ret = hi_mpi_vpss_send_frame(vpss_grp, &frame_info, milli_sec);
        if (ret != HI_SUCCESS) {
            printf("aidestrip send frame failed!\n");
        }

        ret = hi_mpi_aidestrip_release_frame(vi_pipe, &frame_info);
        if (ret != HI_SUCCESS) {
            printf("aidestrip release frame failed!\n");
        }
    }

    return HI_NULL;
}

static hi_void sample_aidestrip_set_attr(hi_void)
{
    hi_aidestrip_attr attr = {0};

    hi_mpi_aidestrip_get_attr(0, &attr);

    sample_get_char("set strength 0");

    attr.param.strength = 0;
    hi_mpi_aidestrip_set_attr(0, &attr);

    sample_get_char("set strength 31");

    attr.param.strength = 31; // max 31
    hi_mpi_aidestrip_set_attr(0, &attr);
}

static hi_void sample_aidestrip_loop(hi_vi_pipe pipe, hi_aidestrip_model *model_info, td_s32 *model_id)
{
    hi_s32 ret;
    pthread_t send_pid, get_pid;

    // wait for fpn calibrate
    while (g_fpn_calibrate_done != HI_TRUE) {
        sleep(1);
    }

    ret = pthread_create(&send_pid, HI_NULL, sample_aidestrip_send_frame_proc, HI_NULL);
    if (ret != HI_SUCCESS) {
        printf("create pthread failed!\n");
        return;
    }

    ret = pthread_create(&get_pid, HI_NULL, sample_aidestrip_get_frame_proc, HI_NULL);
    if (ret != HI_SUCCESS) {
        printf("create pthread failed!\n");
        return;
    }

    sample_aidestrip_set_attr();

    sample_get_char("exit");

    g_thread_start = 0;
    pthread_join(send_pid, HI_NULL);
    pthread_join(get_pid, HI_NULL);

    sample_aidestrip_stop(pipe, model_info, model_id);
}

static void delay_ms(int ms)
{
    usleep(ms * 1000); /* 1ms: 1000us */
    return;
}

hi_void sample_thermo_iris_control(hi_u8 open)
{
    hi_u32 gpio_chip_num2 = 2; // gpio2
    sample_thermo_gpio_export(gpio_chip_num2, 0x1);  // gpio2_1
    sample_thermo_gpio_export(gpio_chip_num2, 0x3);  // gpio2_3
    sample_thermo_gpio_dir(gpio_chip_num2, 0x1);  // gpio2_1 out
    sample_thermo_gpio_dir(gpio_chip_num2, 0x3);  // gpio2_3 out
    if (open == 1) {
        sample_thermo_gpio_write(gpio_chip_num2, 0x1, 0); // set GPIO2_1 0
        sample_thermo_gpio_write(gpio_chip_num2, 0x3, 1); // set GPIO2_3 1
        delay_ms(50); // delay 50ms
        sample_thermo_gpio_write(gpio_chip_num2, 0x1, 0); // set GPIO2_1 0
        sample_thermo_gpio_write(gpio_chip_num2, 0x3, 0); // set GPIO2_3 0
    } else if (open == 0) {
        sample_thermo_gpio_write(gpio_chip_num2, 0x1, 1); // set GPIO2_1 1
        sample_thermo_gpio_write(gpio_chip_num2, 0x3, 0); // set GPIO2_3 0
        delay_ms(50); // delay 50ms
        sample_thermo_gpio_write(gpio_chip_num2, 0x1, 0); // set GPIO2_1 0
        sample_thermo_gpio_write(gpio_chip_num2, 0x3, 0); // set GPIO2_3 0
    } else {}
    sample_thermo_gpio_unexport(gpio_chip_num2, 0x1);
    sample_thermo_gpio_unexport(gpio_chip_num2, 0x3);
}

static hi_s32 sample_thermo_convert_14bit_pixel(hi_u8 *data, hi_u32 data_num, hi_u32 bit_width, hi_u16 *out_data)
{
    hi_s32 i, tmp_data_num, out_cnt;
    hi_u64 u64_val;
    hi_u8 *tmp_data = data;

    out_cnt = 0;
    switch (bit_width) {
        case 14: {   /* 14 bit */
            /* 4 pixels consist of 7 bytes  */
            tmp_data_num = data_num / 4;

            for (i = 0; i < tmp_data_num; i++) {
                tmp_data = data + 7 * i;    /* 7 bytes */
                u64_val = tmp_data[0] + ((hi_u32)tmp_data[1] << 8) +   /* 8 shift */
                    ((hi_u32)tmp_data[0x2] << 16) + ((hi_u32)tmp_data[0x3] << 24) +    /* 16 shift 24 shift */
                    ((hi_u64)tmp_data[0x4] << 32) + ((hi_u64)tmp_data[0x5] << 40) +    /* 32 shift 40 shift */
                    ((hi_u64)tmp_data[0x6] << 48);    /* 48 shift */

                out_data[out_cnt++] = (u64_val >> 0)  & 0x3fff;
                out_data[out_cnt++] = (u64_val >> 14) & 0x3fff;   /* 14 shift */
                out_data[out_cnt++] = (u64_val >> 28) & 0x3fff;   /* 28 shift */
                out_data[out_cnt++] = (u64_val >> 42) & 0x3fff;   /* 42 shift */
            }
            break;
        }
        default:
            sample_print("unsuport bit_width: %d\n", bit_width);
            return HI_FAILURE;
    }

    return out_cnt;
}

hi_s32 get_raw_data(hi_video_frame *v_buf, hi_u32 nbit, hi_u32 tmp_raw[][400], hi_u32 height, hi_u32 width)
{
    hi_u32 i, j;
    hi_u32 h_bias, w_bias;
    hi_u64 phys_addr;
    hi_u64 size;
    hi_u16 *u16_data = HI_NULL;
    hi_u8 *virt_addr;
    hi_u8 *u8_data;

    size = (v_buf->stride[0]) * (v_buf->height);
    phys_addr = v_buf->phys_addr[0];
    virt_addr = (hi_u8 *)hi_mpi_sys_mmap(phys_addr, size);
    if (virt_addr == HI_NULL) {
        sample_print("hi_mpi_sys_mmap failed!\n");
        return HI_FAILURE;
    }

    u8_data = virt_addr;
    u16_data = (hi_u16 *)malloc(v_buf->width * 0x2);
    if (u16_data == HI_NULL) {
        sample_print("malloc memory failed\n");
        hi_mpi_sys_munmap(virt_addr, size);
        virt_addr = HI_NULL;
        return HI_FAILURE;
    }

    h_bias = (height - v_buf->height) / 0x2;
    w_bias = (width - v_buf->width) / 0x2;
    for (i = 0; i < v_buf->height; i++) {
        sample_thermo_convert_14bit_pixel(u8_data, v_buf->width, 14, u16_data);   /* 14 bit */
        for (j = 0; j < v_buf->width; j++) {
            tmp_raw[h_bias + i][w_bias + j] = u16_data[j];
        }
        u8_data += v_buf->stride[0];
    }

    free(u16_data);

    return HI_SUCCESS;
}

hi_void set_ooc_data(sample_vi_user_frame_info *user_frame_info, hi_u8 ooc_data[][400])
{
    hi_u32 i, j;
    hi_vi_dev vi_dev = 1;
    hi_vi_thermo_sns_attr sns_attr;

    hi_video_frame *v_buf1 = &(user_frame_info->frame_info.video_frame);
    hi_u8 *data;
    data = (hi_u8 *)(hi_uintptr_t)v_buf1->virt_addr[0];

    for (i = 0; i < THERMO_HEIGHT_308; i++) {
        for (j = 0; j < THERMO_WIDTH_400; j++) {
            data[j] = ooc_data[i][j] * 4;   /* multi 4 */
        }
        data = data + v_buf1->stride[0];
    }

    sns_attr.work_mode = HI_VI_THERMO_WORK_MODE_T3;
    sns_attr.cfg_num = (hi_u32)(sizeof(g_t3_sns_cfg)/sizeof(hi_u8));
    (hi_void)memcpy_s(&sns_attr.sns_cfg, sizeof(hi_u8) * HI_VI_MAX_SNS_CFG_NUM, g_t3_sns_cfg, sizeof(g_t3_sns_cfg));

    memcpy_s(&sns_attr.ooc_frame_info, sizeof(hi_video_frame_info),
             &user_frame_info->frame_info, sizeof(hi_video_frame_info));
    sns_attr.frame_rate = 30;                  /* framerate 30 */
    sns_attr.sd_mux[0] = HI_VI_SD_MUX_T2_SDA0; /* pin_mux0:4 */
    sns_attr.sd_mux[1] = HI_VI_SD_MUX_T2_SDA1; /* pin_mux1:5 */
    sns_attr.sd_mux[2] = HI_VI_SD_MUX_T2_SDA2; /* pin_mux2:6 */
    sns_attr.sd_mux[3] = HI_VI_SD_MUX_T2_FS;   /* pin_mux3:7 */
    hi_mpi_vi_set_thermo_sns_attr(vi_dev, &sns_attr);
}

hi_void init_data(hi_u8 ooc_data[][400], hi_u32 tmp_raw[][400], hi_u32 *ooc_step)
{
    int i, j;
    for (i = 0; i < THERMO_HEIGHT_308; i++) {
        for (j = 0; j < THERMO_WIDTH_400; j++) {
            ooc_data[i][j] = 32;  /* default 32 */
            tmp_raw[i][j] = 0;
        }
    }
    *ooc_step = 16;  /* default 16 */
}

hi_void update_ooc(hi_u8 ooc_data[][400], hi_u32 tmp_raw[][400], hi_u32 ooc_step)
{
    hi_u32 i, j;
    hi_u32 data;
    for (i = 0; i < THERMO_HEIGHT; i++) {
        for (j = 0; j < THERMO_WIDTH; j++) {
            if ((tmp_raw[10 + i][8 + j] <= 8600) &&  /* bias 10 8 value 8600 */
                (tmp_raw[10 + i][8 + j] >= 7800)) { /* bias 10 8 value 7800 */
            } else if (tmp_raw[10 + i][8 + j] > 8600) {  /* bias 10 bias 8 value 8600 */
                data = ooc_data[10 + i][8 + j] + ooc_step;  /* bias 10 8 */
                ooc_data[10 + i][8 + j] = (data > 62 ? 62 : data);    /* bias 10 8 value 62 62 */
            } else if (tmp_raw[10 + i][8 + j] < 7800) {  /* bias 10 bias 8 value 7800 */
                ooc_data[10 + i][8 + j] = ooc_data[10 + i][8 + j] - ooc_step;   /* bias 10 8 10 8 */
            } else {}
        }
    }
}

static hi_void sample_thermo_restart_get_venc_stream(hi_venc_chn venc_chn[], hi_u32 chn_num)
{
    hi_u32 i;
    hi_s32 ret;
    hi_venc_start_param start_param;

    for (i = 0; i < chn_num; i++) {
        start_param.recv_pic_num = -1;
        if ((ret = hi_mpi_venc_start_chn(venc_chn[i], &start_param)) != HI_SUCCESS) {
            sample_print("hi_mpi_venc_start_recv_pic failed with%#x! \n", ret);
            return;
        }
    }

    ret = sample_comm_venc_start_get_stream(venc_chn, chn_num);
    if (ret != HI_SUCCESS) {
        for (i = 0; i < chn_num; i++) {
            if ((ret = hi_mpi_venc_stop_chn(venc_chn[i])) != HI_SUCCESS) {
                sample_print("hi_mpi_venc_stop_recv_pic failed with%#x! \n", ret);
            }
        }
    }
}

hi_u32 sample_thermo_temperature_change(hi_vi_pipe vi_pipe)
{
    hi_u32 dvtemp_d1, dvtemp_d2, dvtemp_d3, dvtemp_d4, dvtemp_d5, dvtemp_d6;
    hi_u32 d16_vtemp_d1, d16_vtemp_d2, dvtemp;
    hi_u32 wr_data, delt_volt;
    hi_s32 t17_vtemp_v, dvtemp0, diff;
    static hi_s32 temperature = 0;

    thermo_gst412c_write_register(vi_pipe, 0x7c, 0xa8);
    thermo_gst412c_write_register(vi_pipe, 0x47, 0x01);
    dvtemp_d1 = thermo_gst412c_read_register(vi_pipe, 0x3d);
    dvtemp_d2 = thermo_gst412c_read_register(vi_pipe, 0x3e);
    d16_vtemp_d2 = ((dvtemp_d2 & 0xff) << 8) | (dvtemp_d1 & 0xff);  /* shift 8 */

    dvtemp_d3 = thermo_gst412c_read_register(vi_pipe, 0x3f);
    dvtemp_d4 = thermo_gst412c_read_register(vi_pipe, 0x40);
    d16_vtemp_d1 = ((dvtemp_d4 & 0xff) << 8) | (dvtemp_d3 & 0xff);  /* shift 8 */

    dvtemp_d5 = thermo_gst412c_read_register(vi_pipe, 0x41);
    dvtemp_d6 = thermo_gst412c_read_register(vi_pipe, 0x42);
    dvtemp = ((dvtemp_d6 & 0xff) << 8) | (dvtemp_d5 & 0xff);        /* shift 8 */

    thermo_gst412c_write_register(vi_pipe, 0x47, 0x00);
    thermo_gst412c_write_register(vi_pipe, 0x7c, 0xa9);
    wr_data = (thermo_gst412c_read_register(vi_pipe, 0x06) & 0x30) >> 4;  /* shift 4 */

    thermo_gst412c_write_register(vi_pipe, 0x7c, 0xa8);

    delt_volt =  250 + 100 * wr_data;   /* value 250 100 */
    t17_vtemp_v = (dvtemp - d16_vtemp_d1) * 1000 /div_0_to_1(d16_vtemp_d2 - d16_vtemp_d1) + delt_volt;  /* value 1000 */
    dvtemp0 = (t17_vtemp_v - 1546) * 10000 / 1200 + 3000;   /* value 1546 10000 1200 3000 */
    if (temperature == 0) {
        temperature = dvtemp0;
    } else if (temperature > dvtemp0) {
        diff = temperature - dvtemp0;
        if (diff > THERMO_TEMP_DIFF) {
            temperature = dvtemp0;
            return 1;
        }
    } else if (temperature < dvtemp0) {
        diff = dvtemp0 - temperature;
        if (diff > THERMO_TEMP_DIFF) {
            temperature = dvtemp0;
            return 1;
        }
    } else {}

    return 0;
}


static hi_void sample_thermo_ooc_correction(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    hi_u32 ooc_step = 16;
    hi_vi_pipe phys_pipe = 0;
    hi_video_frame_info frame_info;
    sample_vi_user_frame_info *user_frame_info = &g_user_frame_info;

    hi_vi_frame_dump_attr dump_attr = {
        .depth = 2,
        .enable = HI_TRUE
    };
    hi_mpi_vi_set_pipe_frame_dump_attr(phys_pipe, &dump_attr);

    init_data(ooc_data, tmp_raw, &ooc_step);
    set_ooc_data(user_frame_info, ooc_data);
    sample_thermo_iris_control(0);
    sleep(3);   // sleep 3s
    while (ooc_step > 0) {
        ret = hi_mpi_vi_get_pipe_frame(phys_pipe, &frame_info, 1000);   /* 1000ms */
        if (ret != HI_SUCCESS) {
            continue;
        }
        get_raw_data(&frame_info.video_frame, 14, tmp_raw, THERMO_HEIGHT_308, THERMO_WIDTH_400);  /* 14 bit */
        update_ooc(ooc_data, tmp_raw, ooc_step);
        ooc_step = ooc_step >> 1;
        set_ooc_data(user_frame_info, ooc_data);
        hi_mpi_vi_release_pipe_frame(phys_pipe, &frame_info);
        sleep(1);
    }
}

static hi_void sample_thermo_set_blacklevel(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    hi_u32 i, j;
    ot_isp_black_level_attr black_level;
    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &black_level);
    for (i = 0; i < OT_ISP_WDR_MAX_FRAME_NUM; i++) {
        for (j = 0; j < OT_ISP_BAYER_CHN_NUM; j++) {
            black_level.user_black_level[i][j] = 0;
            black_level.manual_attr.black_level[i][j] = USER_BLC;
        }
    }
    black_level.user_black_level_en = TD_TRUE;
    black_level.black_level_mode = OT_ISP_BLACK_LEVEL_MODE_MANUAL;
    ret = hi_mpi_isp_set_black_level_attr(vi_pipe, &black_level);
    if (ret != TD_SUCCESS) {
        sample_print("hi_mpi_isp_set_black_level_attr failed.\n");
    }
}

hi_void *sample_thermo_ooc_fpn_correction(hi_void *p)
{
    hi_u8  bupdata = 1;
    hi_vi_pipe vi_pipe = 0;
    const hi_u32 chn_num = 1;
    hi_venc_chn venc_chn[1] = {0};
    static time_t time_s = 0;
    time_t time_e = 0;
    hi_u32 time_d = 0;
    hi_s32 ret;
    thermo_gst412c_i2c_init(vi_pipe);
    sample_comm_vi_disable_fpn_correction_for_thermo(vi_pipe, &g_correction_cfg);
    while (g_thread_ooc_fpn_start) {
        if (bupdata == 1 || time_d > THERMO_TIME_DIFF) {
            sample_comm_venc_stop_get_stream(chn_num);
            printf(" begin ooc correction.\n");
            sample_thermo_ooc_correction(vi_pipe);

            printf("ooc correction done, begin fpn calibrate.\n");
            g_fpn_calibrate_done = HI_FALSE;
            sleep(1);
            hi_mpi_vi_set_pipe_frame_source(vi_pipe, OT_VI_PIPE_FRAME_SOURCE_FE);
            
            ot_isp_black_level_attr saved_lack_level;
            ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &saved_lack_level);
            sample_thermo_set_blacklevel(vi_pipe);
            sleep(1);
            sample_comm_vi_fpn_calibrate_for_thermo(vi_pipe, &g_calibration_cfg);

            ret = hi_mpi_isp_set_black_level_attr(vi_pipe, &saved_lack_level);
            if (ret != TD_SUCCESS) {
                return HI_NULL;
            }
            
            sample_print("fpn calibrate done.\n");
            sample_thermo_iris_control(1);
            hi_mpi_vi_set_pipe_frame_source(vi_pipe, HI_VI_PIPE_FRAME_SOURCE_USER);
            sleep(3);  // sleep 3s
            g_fpn_calibrate_done = HI_TRUE;
            sample_thermo_restart_get_venc_stream(venc_chn, chn_num);
            bupdata = 0;
            time_s = time(NULL);
        }
        time_e = time(NULL);
        time_d = (hi_u32)difftime(time_e, time_s);
        sleep(1);
        bupdata = sample_thermo_temperature_change(vi_pipe);
    }
    return HI_NULL;
}

static hi_void sample_thermo_set_vi_crop(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    hi_vi_crop_info crop_info = {0};

    crop_info.enable = HI_TRUE;
    crop_info.crop_mode = HI_COORD_ABS;
    crop_info.rect.x = 0;
    crop_info.rect.y = 0;
    crop_info.rect.width = THERMO_WIDTH;
    crop_info.rect.height = THERMO_HEIGHT;
    printf("Seting chn crop-->Width: %u---Height: %u!\n", crop_info.rect.width, crop_info.rect.height);
    ret = hi_mpi_vi_set_chn_crop(vi_pipe, 0, &crop_info);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_vi_set_chn_crop failed!\n");
    }
}

static hi_void sample_thermo_set_awb(hi_vi_pipe vi_pipe)
{
    hi_s32 ret;
    hi_isp_wb_attr wb_attr;

    ret = hi_mpi_isp_get_wb_attr(vi_pipe, &wb_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_isp_get_wb_attr failed!\n");
    }
    wb_attr.op_type = 1;
    wb_attr.manual_attr.r_gain = 0x100;
    wb_attr.manual_attr.gr_gain = 0x100;
    wb_attr.manual_attr.gb_gain = 0x100;
    wb_attr.manual_attr.b_gain = 0x100;
    ret = hi_mpi_isp_set_wb_attr(vi_pipe, &wb_attr);
    if (ret != HI_SUCCESS) {
        sample_print("hi_mpi_isp_set_wb_attr failed!\n");
    }
}

static hi_void sample_thermo_read_fpn_file(hi_isp_fpn_frame_info *fpn_frame_info, FILE *pfd)
{
    hi_s32 i;
    hi_video_frame_info *frame_info;

    frame_info = &fpn_frame_info->fpn_frame;
    (hi_void)fread((hi_u8 *)frame_info->video_frame.virt_addr[0], fpn_frame_info->frm_size, 1, pfd);

    for (i = 0; i < HI_VI_MAX_SPLIT_NODE_NUM; i++) {
        (hi_void)fread((hi_u8 *)&fpn_frame_info->offset[i], 4, 1, pfd); /* 4: 4byte */
    }

    (hi_void)fread((hi_u8 *)&frame_info->video_frame.compress_mode, 4, 1, pfd); /* 4: 4byte */
    (hi_void)fread((hi_u8 *)&fpn_frame_info->frm_size, 4, 1, pfd); /* 4: 4byte */
    (hi_void)fread((hi_u8 *)&fpn_frame_info->iso, 4, 1, pfd); /* 4: 4byte */
}

static hi_s32 sample_thermo_read_k_file(hi_u16 *k_matrix)
{
    FILE *file;
    hi_s32 ret;
    hi_u32 k_width = THERMO_WIDTH;
    hi_u32 k_height = THERMO_HEIGHT;
    hi_u32 k_stride = THERMO_WIDTH;

    hi_u8 *dst = HI_NULL;
    hi_u32 row;
    file = fopen("./raw/k.raw", "rb");
    if (!file) {
        sample_print("open file:%s failed!\n", "k.raw");
        return HI_NULL;
    }

    ret = fseek(file, 0, SEEK_SET);
    if (ret != HI_SUCCESS) {
        sample_print("fseek failed!\n");
        fclose(file);
        return ret;
    }

    dst = (hi_u8 *)k_matrix;
    for (row = 0; row < k_height; ++row) {
        ret = fread(dst, k_width, THERMO_DATA_COUNT, file);
        if (ret != THERMO_DATA_COUNT) {
            sample_print("fread failed!\n");
            fclose(file);
            return ret;
        }
        dst += k_stride * THERMO_DATA_COUNT;
    }

    ret = fclose(file);
    if (ret != HI_SUCCESS) {
        sample_print("fclose failed!\n");
        return ret;
    }
    return HI_SUCCESS;
}

static hi_s32 sample_nuc_init_cfg(hi_void)
{
    td_s32 ret;
    hi_nuc_cfg cfg = {0};
    ret = hi_mpi_sys_mmz_alloc(&g_k_phys_addr, &g_k_virt_addr, "nuc_k_matrix", HI_NULL,
        THERMO_WIDTH * THERMO_HEIGHT *sizeof(hi_u16));
    if (ret != HI_SUCCESS) {
        sample_print(" k mmz alloc failed!\n");
        return ret;
    }
    ret = hi_mpi_nuc_get_cfg(0, &cfg);
    if (ret != HI_SUCCESS) {
        sample_print(" get cfg failed!\n");
        goto exit_get_cfg;
    }

    cfg.param.bias = NUC_BIAS;
    cfg.param.nuc_black_level = USER_BLC;
    cfg.param.k_matrix_size = THERMO_WIDTH * THERMO_HEIGHT;
    cfg.param.k_matrix = g_k_virt_addr;

    ret = sample_thermo_read_k_file(g_k_virt_addr);
    if (ret != HI_SUCCESS) {
        sample_print("read_k_file failed!\n");
        goto exit_get_cfg;
    }

    ret = hi_mpi_nuc_set_cfg(0, &cfg);
    if (ret != HI_SUCCESS) {
        sample_print("read_k_file failed!\n");
        goto exit_get_cfg;
    }
    return ret;

exit_get_cfg:
    hi_mpi_sys_mmz_free(g_k_phys_addr, g_k_virt_addr);
    return ret;
}

static hi_s32 sample_nuc_start(hi_vi_pipe pipe)
{
    hi_s32 ret;
    ret = hi_mpi_nuc_init();
    if (ret != HI_SUCCESS) {
        sample_print("nuc init failed!\n");
        return ret;
    }
    ret = sample_nuc_init_cfg();
    if (ret != HI_SUCCESS) {
        sample_print("nuc init cfg failed!\n");
        goto exit_load_cfg;
    }
    ret = sample_thermo_common_load_mem(&g_nuc_model_info.model.mem_info, g_nuc_model_file);
    if (ret != HI_SUCCESS) {
        sample_print("nuc init cfg failed!\n");
        goto exit_load_mem;
    }
    g_nuc_model_info.model.image_size.width = THERMO_WIDTH;
    g_nuc_model_info.model.image_size.height = THERMO_HEIGHT;

    ret = hi_mpi_nuc_load_model(&g_nuc_model_info, &g_nuc_model_id);
    if (ret != HI_SUCCESS) {
        sample_print("nuc load model failed!\n");
        goto exit_load_model;
    }

    ret = hi_mpi_nuc_enable(pipe);
    if (ret != HI_SUCCESS) {
        sample_print("nuc enable failed!\n");
        goto exit_enable;
    }
    return ret;
exit_enable:
    hi_mpi_nuc_disable(pipe);
exit_load_model:
    hi_mpi_nuc_unload_model(g_nuc_model_id);
exit_load_mem:
    sample_aiisp_unload_mem(&g_nuc_model_info.model.mem_info);
exit_load_cfg:
    hi_mpi_nuc_exit();

    return HI_FAILURE;
}

hi_void *sample_nuc_send_frame_proc(hi_void *p)
{
    hi_s32 ret;
    hi_vi_pipe vi_pipe = 0;
    hi_s32 milli_sec = 1000; // 1000 ms
    hi_s32 first_start = 1;
    hi_isp_fpn_frame_info fpn_info = {0};
    hi_video_frame_info frame_info;

    hi_mpi_sys_mmz_alloc(&g_fpn_phys_addr, &g_fpn_virt_addr, "FPN_frame", HI_NULL,
        THERMO_WIDTH * THERMO_HEIGHT *sizeof(hi_u16));
    fpn_info.fpn_frame.video_frame.phys_addr[0] = g_fpn_phys_addr;
    fpn_info.fpn_frame.video_frame.virt_addr[0] = g_fpn_virt_addr;
    fpn_info.fpn_frame.video_frame.stride[0]    = THERMO_WIDTH*THERMO_DATA_COUNT;
    fpn_info.fpn_frame.video_frame.width         = THERMO_WIDTH;
    fpn_info.fpn_frame.video_frame.height        = THERMO_HEIGHT;
    fpn_info.frm_size       = THERMO_WIDTH*THERMO_HEIGHT*THERMO_DATA_COUNT;
   
    FILE *pfd = HI_NULL;
    while (g_thread_start) {
        if (g_fpn_calibrate_done == HI_FALSE || first_start == 1) {
            while (g_fpn_calibrate_done == HI_FALSE) {
                sleep(1);
            }

            pfd = fopen("./FPN_frame_384x288_16bit.raw", "rb");
            if (pfd == HI_NULL) {
                sample_print("open FPN failed!\n");
                return HI_NULL;
            }
            sample_thermo_read_fpn_file(&fpn_info, pfd);
            fpn_info.fpn_frame.video_frame.compress_mode = HI_COMPRESS_MODE_NONE;
            fpn_info.fpn_frame.video_frame.pixel_format = OT_PIXEL_FORMAT_RGB_BAYER_14BPP;
            first_start = 0;
            fclose(pfd);
            pfd = HI_NULL;
        }

        ret = hi_mpi_vi_get_pipe_frame(vi_pipe, &frame_info, milli_sec);
        if (ret != HI_SUCCESS) {
            continue;
        }
        
        ret = hi_mpi_nuc_send_frame(vi_pipe, &frame_info, &fpn_info.fpn_frame, milli_sec);
        if (ret != HI_SUCCESS) {
            sample_print("nuc send frame failed!\n");
        }

        ret = hi_mpi_vi_release_pipe_frame(vi_pipe, &frame_info);
        if (ret != TD_SUCCESS) {
            sample_print("hi_mpi_vi_release_frame failed!\n");
        }
    }
    return HI_NULL;
}

hi_void *sample_nuc_get_frame_proc(hi_void *p)
{
    td_s32 ret;
    hi_s32 milli_sec = 1000; // 1000 ms
    hi_vi_pipe vi_pipe = 0;

    hi_video_frame_info frame_info;
    const hi_video_frame_info *send_info[1];
    send_info[0] = &frame_info;
    hi_mpi_vi_set_pipe_frame_source(vi_pipe, HI_VI_PIPE_FRAME_SOURCE_USER);

    while (g_thread_start) {
        while (g_fpn_calibrate_done == HI_FALSE) {
            sleep(1);
        }

        ret = hi_mpi_nuc_get_frame(vi_pipe, &frame_info, milli_sec);
        if (ret != TD_SUCCESS) {
            sample_print("hi_mpi_nuc_get_frame failed!\n");
        }

        ret = hi_mpi_vi_send_pipe_raw(vi_pipe, send_info, 1, milli_sec);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_vi_send_pipe_raw failed!\n");
        }

        ret = hi_mpi_nuc_release_frame(vi_pipe, &frame_info);
        if (ret != HI_SUCCESS) {
            sample_print("nuc release frame failed!\n");
        }
    }

    return HI_NULL;
}

static hi_void sample_nuc_loop()
{
    td_s32 ret;
    
    while (g_fpn_calibrate_done == HI_FALSE) {
        sleep(1);
    }
    sample_print("create nuc pthread!\n");

    ret = pthread_create(&g_send_pid_nuc, HI_NULL, sample_nuc_send_frame_proc, HI_NULL);
    if (ret != HI_SUCCESS) {
        sample_print("create pthread failed!\n");
        return;
    }

    ret = pthread_create(&g_get_pid_nuc, HI_NULL, sample_nuc_get_frame_proc, HI_NULL);
    if (ret != HI_SUCCESS) {
        sample_print("create pthread failed!\n");
        return;
    }
    return;
}

static hi_s32 sample_thermo_basic_route(hi_bool aidestrip_en)
{
    hi_s32 ret;
    hi_vi_dev vi_dev = 1; // thermo can only use vi dev1
    sample_vi_cfg vi_cfg[1];
    sample_vpss_cfg vpss_cfg;
    sample_sns_type sns_type = SENSOR1_TYPE;
    hi_s32 model_id;
    hi_vi_pipe pipe = 0;
    hi_aidestrip_model model_info = {0};
    pthread_t ooc_fpn_correction;

    g_thermo_sys_cfg.mode_type = HI_VI_OFFLINE_VPSS_OFFLINE;
    sample_comm_vi_get_vi_cfg_by_fmu_mode(sns_type, g_thermo_sys_cfg.vi_fmu[0], &vi_cfg[0]);
    vi_cfg[0].pipe_info[0].nr_attr.enable = HI_FALSE; // disable 3DNR
    sample_comm_vpss_get_default_vpss_cfg(&vpss_cfg, g_thermo_sys_cfg.vpss_fmu[0]);

    if (sample_thermo_start_route(vi_cfg, &vpss_cfg, g_thermo_sys_cfg.route_num, aidestrip_en) != HI_SUCCESS) {
        return HI_FAILURE;
    }

    if (aidestrip_en) {
        ret = sample_aidestrip_start(pipe, &model_info, &model_id);
        if (ret != HI_SUCCESS) {
            sample_print("sample_aidestrip_start failed!\n");
            goto exit_route;
        }
    }

    ret = sample_nuc_start(pipe);
    if (ret != HI_SUCCESS) {
        sample_print("nuc start failed!\n");
        goto exit_route;
    }

    sample_thermo_set_thermo_t3_cfg(vi_dev, &g_user_frame_info);
    sample_thermo_set_awb(pipe);

    ret = pthread_create(&ooc_fpn_correction, HI_NULL, sample_thermo_ooc_fpn_correction, HI_NULL);
    if (ret != HI_SUCCESS) {
        sample_print("create pthread failed!\n");
        goto exit_route;
    }
    sample_thermo_set_vi_crop(pipe);

    sample_nuc_loop();

    if (aidestrip_en) {
        sample_aidestrip_loop(pipe, &model_info, &model_id);
    } else {
        sample_get_char("exit");
    }

    g_thread_ooc_fpn_start = 0;
    pthread_join(ooc_fpn_correction, HI_NULL);
    sample_nuc_stop(pipe);

exit_route:
    sample_thermo_stop_route(vi_cfg, g_thermo_sys_cfg.route_num, aidestrip_en, pipe);
    return ret;
}

static hi_void sample_thermo_usage(const char *prg_name)
{
    printf("usage : %s <index> \n", prg_name);
    printf("index:\n");
    printf("    (0) t3 thermo          :thermo -> vi -> vpss -> venc && vo.\n");
    printf("    (1) t3 thermo          :thermo -> vi -> aidestrip -> vpss -> venc && vo.\n");
}

static hi_void sample_thermo_handle_sig(hi_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        g_sig_flag = 1;
    }
}

static hi_void sample_register_sig_handler(hi_void (*sig_handle)(hi_s32))
{
    struct sigaction sa;

    (hi_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sig_handle;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, HI_NULL);
    sigaction(SIGTERM, &sa, HI_NULL);
}

static hi_s32 sample_thermo_execute_case(hi_u32 case_index)
{
    hi_s32 ret;
    hi_bool aidestrip_en = HI_FALSE;

    switch (case_index) {
        case 0: /* 0 t3 sensor */
            aidestrip_en = HI_FALSE;
            ret = sample_thermo_basic_route(aidestrip_en);
            break;
        case 1: /* 1 t3 sensor and aidestrip */
            aidestrip_en = HI_TRUE;
            ret = sample_thermo_basic_route(aidestrip_en);
            break;
        default:
            ret = HI_FAILURE;
            break;
    }

    return ret;
}

static hi_s32 sample_thermo_msg_proc_vb_pool_share(hi_s32 pid)
{
    hi_s32 ret;
    hi_u32 i;
    hi_bool isp_states[HI_VI_MAX_PIPE_NUM];
#ifndef SAMPLE_MEM_SHARE_ENABLE
    hi_vb_common_pools_id pools_id = {0};

    if (hi_mpi_vb_get_common_pool_id(&pools_id) != HI_SUCCESS) {
        sample_print("get common pool_id failed!\n");
        return HI_FAILURE;
    }

    for (i = 0; i < pools_id.pool_cnt; ++i) {
        if (hi_mpi_vb_pool_share(pools_id.pool[i], pid) != HI_SUCCESS) {
            sample_print("vb pool share failed!\n");
            return HI_FAILURE;
        }
    }
#endif
    ret = sample_comm_vi_get_isp_run_state(isp_states, HI_VI_MAX_PIPE_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("get isp states fail\n");
        return HI_FAILURE;
    }

    for (i = 0; i < HI_VI_MAX_PIPE_NUM; i++) {
        if (!isp_states[i]) {
            continue;
        }
        ret = hi_mpi_isp_mem_share(i, pid);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_isp_mem_share vi_pipe %u, pid %d fail\n", i, pid);
        }
    }

    return HI_SUCCESS;
}

static hi_void sample_thermo_msg_proc_vb_pool_unshare(hi_s32 pid)
{
    hi_s32 ret;
    hi_u32 i;
    hi_bool isp_states[HI_VI_MAX_PIPE_NUM];
#ifndef SAMPLE_MEM_SHARE_ENABLE
    hi_vb_common_pools_id pools_id = {0};
    if (hi_mpi_vb_get_common_pool_id(&pools_id) == HI_SUCCESS) {
        for (i = 0; i < pools_id.pool_cnt; ++i) {
            ret = hi_mpi_vb_pool_unshare(pools_id.pool[i], pid);
            if (ret != HI_SUCCESS) {
                sample_print("hi_mpi_vb_pool_unshare vi_pipe %u, pid %d fail\n", pools_id.pool[i], pid);
            }
        }
    }
#endif
    ret = sample_comm_vi_get_isp_run_state(isp_states, HI_VI_MAX_PIPE_NUM);
    if (ret != HI_SUCCESS) {
        sample_print("get isp states fail\n");
        return;
    }

    for (i = 0; i < HI_VI_MAX_PIPE_NUM; i++) {
        if (!isp_states[i]) {
            continue;
        }
        ret = hi_mpi_isp_mem_unshare(i, pid);
        if (ret != HI_SUCCESS) {
            sample_print("hi_mpi_isp_mem_unshare vi_pipe %u, pid %d fail\n", i, pid);
        }
    }
}

static hi_s32 sample_thermo_ipc_msg_proc(const sample_ipc_msg_req_buf *msg_req_buf,
    hi_bool *is_need_fb, sample_ipc_msg_res_buf *msg_res_buf)
{
    hi_s32 ret;

    if (msg_req_buf == HI_NULL || is_need_fb == HI_NULL) {
        return HI_FAILURE;
    }

    /* need feedback default */
    *is_need_fb = HI_TRUE;

    switch ((sample_msg_type)msg_req_buf->msg_type) {
        case SAMPLE_MSG_TYPE_VB_POOL_SHARE_REQ: {
            if (msg_res_buf == HI_NULL) {
                return HI_FAILURE;
            }
            ret = sample_thermo_msg_proc_vb_pool_share(msg_req_buf->msg_data.pid);
            msg_res_buf->msg_type = SAMPLE_MSG_TYPE_VB_POOL_SHARE_RES;
            msg_res_buf->msg_data.is_req_success = (ret == HI_SUCCESS) ? HI_TRUE : HI_FALSE;
            break;
        }
        case SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_REQ: {
            if (msg_res_buf == HI_NULL) {
                return HI_FAILURE;
            }
            sample_thermo_msg_proc_vb_pool_unshare(msg_req_buf->msg_data.pid);
            msg_res_buf->msg_type = SAMPLE_MSG_TYPE_VB_POOL_UNSHARE_RES;
            msg_res_buf->msg_data.is_req_success = HI_TRUE;
            break;
        }
        default: {
            printf("unsupported msg type(%ld)!\n", msg_req_buf->msg_type);
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}

#ifdef __LITEOS__
hi_s32 app_main(hi_s32 argc, hi_char *argv[])
#else
hi_s32 main(hi_s32 argc, hi_char *argv[])
#endif
{
    hi_s32 ret;
    hi_u32 index;
    hi_char *para_stop;

    if (argc != 2) { /* 2:arg num */
        sample_thermo_usage(argv[0]);
        return HI_FAILURE;
    }

    if (!strncmp(argv[1], "-h", 2)) { /* 2:arg num */
        sample_thermo_usage(argv[0]);
        return HI_FAILURE;
    }

    if (strlen(argv[1]) > 2 || strlen(argv[1]) == 0 || !check_digit(argv[1][0]) || /* 2:arg len */
        (strlen(argv[1]) == 2 && (!check_digit(argv[1][1]) || argv[1][0] == '0'))) { /* 2:arg len */
        sample_thermo_usage(argv[0]);
        return HI_FAILURE;
    }

    if (strlen(argv[1]) == 2 && argv[1][1] != '0') { /* 2:arg len, max: 10 */
        sample_thermo_usage(argv[0]);
        return HI_FAILURE;
    }

#ifndef __LITEOS__
    sample_register_sig_handler(sample_thermo_handle_sig);
#endif

    if (sample_ipc_server_init(sample_thermo_ipc_msg_proc) != HI_SUCCESS) {
        printf("sample_ipc_server_init failed!!!\n");
    }

    index = (hi_u32)strtol(argv[1], &para_stop, 10); /* index : 1 dec : 10 */
    ret = sample_thermo_execute_case(index);
    if ((ret == HI_SUCCESS) && (g_sig_flag == 0)) {
        printf("\033[0;32mprogram exit normally!\033[0;39m\n");
    } else {
        printf("\033[0;31mprogram exit abnormally!\033[0;39m\n");
    }

    sample_ipc_server_deinit();

#ifdef __LITEOS__
    return ret;
#else
    exit(ret);
#endif
}
