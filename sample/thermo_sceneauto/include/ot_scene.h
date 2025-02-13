/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */

#ifndef HI_SCENE_H
#define HI_SCENE_H

#include <sys/ioctl.h>
#include <sys/time.h>
#include <linux/fb.h>
#ifdef OT_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ot_i2c.h"
#endif


#include "sample_comm.h"
#include "ot_scene_setparam.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* * \addtogroup     SCENE */
/* * @{ */ /* * <!-- [SCENE] */

/* error code define */
#define HI_SCENE_EINVAL HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 1)       /* Invalid argument */
#define HI_SCENE_ENOTINIT HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 2)     /* Not inited */
#define HI_SCENE_ENONPTR HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 3)      /* null pointer error */
#define HI_SCENE_EOUTOFRANGE HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 4)  /* out of range */
#define HI_SCENE_EINTER HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 5)       /* Internal error */
#define HI_SCENE_EINITIALIZED HI_SCENECOMM_ERR_ID(HI_SCENE_MOD_SCENE, 6) /* Already initialized */

/* macro define */
#define HI_SCENE_PIPE_MAX_NUM HI_VI_MAX_PIPE_NUM            /* *< pipe max count */
#define HI_SCENE_HDRFRAME_MAX_COUNT 3      /* *< HDR max frame count */
#define HI_SCENE_LONGEXP_AEROUTE_MAX_NUM 4 /* *< LongExp AeRoute Number */

#define THERMO_WIDTH_400  400
#define THERMO_HEIGHT_308 308
#define THERMO_WIDTH      384
#define THERMO_HEIGHT     288
#define THERMO_TEMP_DIFF  100
#define THERMO_TIME_DIFF  120
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
#define LEN 10
#define THERMO_FILE_NAME_LEN 50

struct i2c_rdwr_args {
    unsigned int i2c_num;
    unsigned int dev_addr;
    unsigned int reg_addr;
    union {
        unsigned int data;
        unsigned int reg_addr_end;
    } w_r_union;
    unsigned int reg_width;
    unsigned int data_width;
    unsigned int reg_step;
};

struct i2c_rdwr_ioctl_data {
    struct i2c_msg  *msgs;  /* pointers to i2c_msgs */
    __u32 nmsgs;            /* number of i2c_msgs */
};

/* pipe type */
typedef enum {
    HI_SCENE_PIPE_TYPE_SNAP = 0,
    HI_SCENE_PIPE_TYPE_VIDEO,
    HI_SCENE_PIPE_TYPE_MCF_3DNR,
    HI_SCENE_PIPE_TYPE_BUTT
} ot_scene_pipe_type;

typedef hi_s32 (*ot_scene_set_static_param)(hi_vi_pipe vi_pipe, hi_u8 index);

/* pipe attr */
typedef struct {
    hi_bool enable;
    hi_handle main_pipe_hdl;
    hi_handle vcap_pipe_hdl;
    hi_handle pipe_chn_hdl;
    hi_handle venc_hdl;
    hi_handle vpss_hdl;  /* vpss group hdl */
    hi_handle vport_hdl; /* vpss chn hdl */
    hi_u8 pipe_param_index;
    ot_scene_pipe_type pipe_type;
} ot_scene_pipe_attr;

typedef enum {
    HI_SCENE_PIPE_MODE_LINEAR = 0,
    HI_SCENE_PIPE_MODE_WDR,
    HI_SCENE_PIPE_MODE_HDR, /* HDR means , WDR existed at same time */
    HI_SCENE_PIPE_MODE_BUTT
} ot_scene_pipe_mode;

/* scene mode, an encapsulation of pipe attr array */
typedef struct {
    ot_scene_pipe_mode pipe_mode;
    ot_scene_pipe_attr pipe_attr[HI_SCENE_PIPE_MAX_NUM]; /* Pipe Attr */
} ot_scene_mode;

typedef struct {
    ot_scene_pipe_param pipe_param[HI_SCENE_PIPETYPE_NUM];
} ot_scene_param;

hi_s32 ot_scene_init(const ot_scene_param *scene_param);

hi_s32 ot_scene_deinit(hi_void);

hi_s32 ot_scene_set_scene_mode(const ot_scene_mode *scene_mode);

hi_s32 ot_scene_get_scene_mode(ot_scene_mode *scene_mode);

hi_s32 ot_scene_get_scene_fps(ot_scene_fps *scene_fps);

hi_s32 ot_scene_set_scene_init_exp(hi_s32 iso, hi_s64 exp);

hi_s32 ot_scene_pause(hi_bool enable);

hi_void getinput();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* End of #ifndef HI_SCENE_H */
