#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/prctl.h>
#include <limits.h>

#include "sample_comm.h"


#ifdef __cplusplus
extern "C"{
#endif

// #region rtsp.h 相关
#define __PACKED__

#define CC_test

typedef enum
{
    RTSP_VIDEO = 0,
    RTSP_VIDEOSUB = 1,
    RTSP_AUDIO = 2,
    RTSP_YUV422 = 3,
    RTSP_RGB = 4,
    RTSP_VIDEOPS = 5,
    RTSP_VIDEOSUBPS = 6
} enRTSP_MonBlockType;

struct _RTP_FIXED_HEADER
{
    /**/                              /* byte 0 */
    unsigned char csrc_len : 4; /**/  /* expect 0 */
    unsigned char extension : 1; /**/ /* expect 1, see RTP_OP below */
    unsigned char padding : 1; /**/   /* expect 0 */
    unsigned char version : 2; /**/   /* expect 2 */
    /**/                              /* byte 1 */
    unsigned char payload : 7; /**/   /* RTP_PAYLOAD_RTSP */
    unsigned char marker : 1; /**/    /* expect 1 */
    /**/                              /* bytes 2, 3 */
    unsigned short seq_no;
    /**/ /* bytes 4-7 */
    unsigned int timestamp;
    /**/                     /* bytes 8-11 */
    unsigned int ssrc; /**/ /* stream number is used here. */
} __PACKED__;
typedef struct _RTP_FIXED_HEADER RTP_FIXED_HEADER;


struct _H265_NALU_HEADER
{
    // byte 0
    unsigned char LayerId_h : 1;
    unsigned char TYPE : 6;
    unsigned char F : 1;

    // byte 1
    unsigned char TID : 3;
    unsigned char LayerId_l : 5;

} __PACKED__; /**/ /* 2 BYTES */
typedef struct _H265_NALU_HEADER H265_NALU_HEADER;

struct _H265_FU_INDICATOR
{

    // byte 0
    unsigned char LayerId_h : 1;
    unsigned char TYPE : 6;
    unsigned char F : 1;

    // byte 1
    unsigned char TID : 3;
    unsigned char LayerId_l : 5;

} __PACKED__; /**/ /* 2 BYTES */
typedef struct _H265_FU_INDICATOR H265_FU_INDICATOR;

struct _H265_FU_HEADER
{
    // byte 0
    unsigned char FuTYPE : 6;
    unsigned char E : 1;
    unsigned char S : 1;
} __PACKED__; /**/ /* 1 BYTES */
typedef struct _H265_FU_HEADER H265_FU_HEADER;





void InitRtspServer();// 初始化 RTSP 服务器
hi_s32 sample_comm_rtsp_sentjin(ot_venc_stream *pstStream,ot_venc_stream_buf_info *stream_buf_info);// RTSP 服务器推流

#endif

#ifdef __cplusplus
}
#endif
// #endregion


