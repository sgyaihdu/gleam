#ifdef __cplusplus
#if __cplusplus
extern "C"
{
#endif
#endif /* End of #ifdef __cplusplus */

#include <time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <netinet/if_ether.h>
#define __USE_MISC
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sample_rtsp.h"

#include "ssl_aes.h"

#define IS_CRYPT 0


    // #define nalu_sent_len 50000
#define nalu_sent_len 1400 // 默认 NAL 单元发送长度
    //  #define nalu_sent_len        14000

#define RTP_H265 96               // H.265 RTP 负载类型
#define MAX_CHAN 8                // 最大通道数
// #define RTP_AUDIO 97              // 音频 RTP 负载类型
#define MAX_RTSP_CLIENT 1         // 最大 RTSP 客户端数量
#define RTSP_SERVER_PORT 9554      // RTSP 服务器端口号
#define FFMPEG_CLIENT
//#define VLC_CLIENT
#ifdef VLC_CLIENT
#define RTSP_RECV_SIZE (2 * 1024) // 接收缓冲区大小
// #define RTSP_RECV_SIZE (1024)        // def:1024 VLC:2*1024 ffmpeg:1024
#define RTSP_MAX_VID (2 * 1024) // 视频数据最大长度
// #define RTSP_MAX_VID (1024*1024) // def:1024*1024  VLC:2*1024 ffmpeg:1152*1024
// #define RTSP_MAX_AUD (15 * 1024) // 音频数据最大长度
#else
#define RTSP_RECV_SIZE (1024) // 接收缓冲区大小
// #define RTSP_RECV_SIZE (1024)        // def:1024 VLC:2*1024 ffmpeg:1024
#define RTSP_MAX_VID (1152 * 1024) // 视频数据最大长度
// #define RTSP_MAX_VID (1024*1024) // def:1024*1024  VLC:2*1024 ffmpeg:1152*1024
// #define RTSP_MAX_AUD (15 * 1024) // 音频数据最大长度
#endif

#define AU_HEADER_SIZE 4
#define PARAM_STRING_MAX 100

    // static sample_venc_getstream_para gs_stPara;
    // static pthread_t gs_VencPid;

    typedef unsigned short u_int16_t;
    typedef unsigned char u_int8_t;
    typedef u_int16_t portNumBits;
    typedef u_int32_t netAddressBits;
    typedef long long _int64;
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif
#define AUDIO_RATE 8000
#define PACKET_BUFFER_END (unsigned int)0x00000000

    typedef struct
    {
        int startblock;
        int BlockFileNum;

    } IDXFILEHEAD_INFO;

    typedef struct
    {
        _int64 starttime;
        _int64 endtime;
        int startblock;
        int stampnum;
    } IDXFILEBLOCK_INFO;

    typedef struct
    {
        int blockindex;
    } IDXSTAMP_INFO;

    typedef struct
    {
        char filename[150];
    } FILESTAMP_INFO;

    typedef struct
    {
        char channelid[9];
        _int64 starttime;
        _int64 endtime;
        _int64 session;
        int type;
        int encodetype;
    } FIND_INFO;

    typedef enum
    {
        RTP_UDP,
        RTP_TCP,
        RAW_UDP
    } StreamingMode;

    extern char g_rtp_playload[20];

    extern int g_audio_rate;

    typedef enum
    {
        RTSP_IDLE = 0,      // 空闲状态
        RTSP_CONNECTED = 1, // 已连接状态
        RTSP_SENDING = 2,   // 数据发送状态
    } RTSP_STATUS;

    typedef struct
    {
        int nVidLen;               // 视频数据长度
        int nAudLen;               // 音频数据长度
        int bIsIFrm;               // 是否为 I 帧
        int bWaitIFrm;             // 是否等待 I 帧
        int bIsFree;               // 是否空闲
        char vidBuf[RTSP_MAX_VID]; // 视频数据缓冲区
        // char audBuf[RTSP_MAX_AUD]; // 音频数据缓冲区
    } RTSP_PACK;

    typedef struct
    {
        int index;  // 客户端索引
        int socket; // 套接字描述符
        int reqchn; // 请求的通道 ID
        int seqnum; // RTP 视频序列号
        // int seqnum2;      // RTP 音频序列号
        unsigned int tsvid; // 视频时间戳
        // unsigned int tsaud; // 音频时间戳
        int status;    // RTSP 状态
        int sessionid; // 会话 ID
        // int rtpport[2];    // RTP 端口
        int rtpport[1];                // RTP 端口
        int rtcpport;                  // RTCP 端口
        char IP[20];                   // 客户端 IP 地址
        char urlPre[PARAM_STRING_MAX]; // URL 前缀
    } RTSP_CLIENT;

    typedef struct
    {
        int vidLen; // 视频数据长度
        // int audLen;       // 音频数据长度
        int nFrameID;              // 帧 ID
        char vidBuf[RTSP_MAX_VID]; // 视频数据缓冲区
        // char audBuf[RTSP_MAX_AUD]; // 音频数据缓冲区
    } FRAME_PACK;

    typedef struct
    {
        int startcodeprefix_len;     //! 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
        unsigned len;                //! Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
        unsigned max_size;           //! Nal Unit Buffer size
        int forbidden_bit;           //! should be always FALSE
        int nal_reference_idc;       //! NALU_PRIORITY_xxxx
        int nal_unit_type;           //! NALU_TYPE_xxxx
        char *buf;                   //! contains the first byte followed by the EBSP
        unsigned short lost_packets; //! true, if packet loss is detected
    } NALU_t;

    RTP_FIXED_HEADER *rtp_hdr;

    H265_NALU_HEADER *h265_nalu_hdr;
    H265_FU_INDICATOR *h265_fu_ind;
    H265_FU_HEADER *h265_fu_hdr;

    FRAME_PACK g_FrmPack[MAX_CHAN]; // 存放每一路视频的数据
    RTSP_PACK g_rtpPack[MAX_CHAN];
    RTSP_CLIENT g_rtspClients[MAX_RTSP_CLIENT]; // RTSP 客户端数组

    pthread_mutex_t g_mutex;     // 互斥锁
    pthread_cond_t g_cond;       // 条件变量
    pthread_mutex_t g_sendmutex; // 发送互斥锁
    pthread_t g_SendDataThreadId = 0;

    int g_nSendDataChn = -1;
    char g_rtp_playload[20];
    // int g_audio_rate = 8000;
    int g_nframerate;
    int exitok = 0;
    int udpfd = 0;
    //int jishu = 0;
    static time_t timestamp = 0;
    static time_t temp_time = 0;






    void rtsp_venc_sent(unsigned char *buffer, int buflen)
    {
        int is = 0;
        int nChanNum = 0;

        for (is = 0; is < MAX_RTSP_CLIENT; is++)
        {
            if (g_rtspClients[is].status != RTSP_SENDING)
            {
                continue;
            }

            char *nalu_payload;
            int nAvFrmLen = 0;
            int nNaluType = 0;
            char sendbuf[1920 * 1080 + 32];

            nChanNum = g_rtspClients[is].reqchn;
            if (nChanNum < 0 || nChanNum >= MAX_CHAN)
            {
                printf("Invalid channel ID: %d\n", nChanNum); // 添加错误处理
                continue;
            }

            if(buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 1 ) {
                buffer+=3;
                buflen-=3;
            }

            if(buffer[0] == 0 && buffer[1] == 0 && buffer[2] == 0 && buffer[3] == 1){
                buffer+=4;
                buflen-=4;
            }

            nAvFrmLen = buflen;

            nNaluType = (buffer[0] & 0x7e) >> 1;

            // if(nNaluType == 39 )
            // {
            //     return;
            // }
            // switch(nNaluType)
            // {
            //     case 32:
            //         return;
            //         break;
            //     case 33:
            //         return;
            //         break;
            //     case 34:
            //         return;
            //         break;
            //     case 39:
            //         return;
            //         break;
            //     default:
            //         break;
            // }

            //printf("nNaluType = %d\n", nNaluType);
    
            struct sockaddr_in server;
            server.sin_family = AF_INET;
            server.sin_port = htons(g_rtspClients[is].rtpport[0]);
            server.sin_addr.s_addr = inet_addr(g_rtspClients[is].IP);
            int bytes = 0;
            // unsigned int timestamp_increse = 0;

            // g_nframerate = 25; // 25fps

            // timestamp_increse = (unsigned int)(90000.0 / g_nframerate);

            rtp_hdr = (RTP_FIXED_HEADER *)&sendbuf[0];
            rtp_hdr->payload = RTP_H265;
            rtp_hdr->version = 2;
            rtp_hdr->marker = 0;
            rtp_hdr->ssrc = htonl(10); //
            rtp_hdr->timestamp = htonl(g_rtspClients[is].tsvid);
            if (nAvFrmLen <= nalu_sent_len)
            {

                /*设置rtp M位 */
                rtp_hdr->marker = 1;
                rtp_hdr->seq_no = htons(g_rtspClients[is].seqnum++);

                nalu_payload = &sendbuf[12];
                memcpy(nalu_payload, buffer, nAvFrmLen);

                // g_rtspClients[is].tsvid = g_rtspClients[is].tsvid + timestamp_increse;

                rtp_hdr->timestamp = htonl(g_rtspClients[is].tsvid);
                bytes = nAvFrmLen + 12;
                // BUGFIX: 检查 sendto 返回值
                if (sendto(udpfd, sendbuf, bytes, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
                {
                    perror("sendto failed");
                }
            }
            else if (nAvFrmLen > nalu_sent_len)
            {
                // #region
#if 1
                int k = 0, l = 0;
                k = nAvFrmLen / nalu_sent_len;
                l = nAvFrmLen % nalu_sent_len;
                int t = 0;
                // g_rtspClients[is].tsvid = g_rtspClients[is].tsvid + timestamp_increse;

                while (t <= k)
                {
                    rtp_hdr->seq_no = htons(g_rtspClients[is].seqnum++);
                    rtp_hdr->marker = 0;
                    h265_fu_ind = (H265_FU_INDICATOR *)&sendbuf[12];
                    h265_fu_ind->F = 0;
                    h265_fu_ind->TYPE = 49;
                    h265_fu_ind->LayerId_h = 0;
                    h265_fu_ind->LayerId_l = 0;
                    h265_fu_ind->TID = 0x01;

                    h265_fu_hdr = (H265_FU_HEADER *)&sendbuf[14];
                    h265_fu_hdr->FuTYPE = nNaluType;
                    if (t == 0)
                    {
                        h265_fu_hdr->E = 0;
                        h265_fu_hdr->S = 1;
                        nalu_payload = &sendbuf[15];
                        memcpy(nalu_payload, buffer+2, nalu_sent_len - 2);//跳过nalu header

                        bytes = nalu_sent_len - 2 + 15;
                        // BUGFIX: 检查 sendto 返回值
                        if (sendto(udpfd, sendbuf, bytes, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
                        {
                            perror("sendto failed");
                        }
                        t++;
                    }
                    else if (k == t)
                    {
                        rtp_hdr->marker = 1;
                        h265_fu_hdr->S = 0;
                        h265_fu_hdr->E = 1;
                        nalu_payload = &sendbuf[15];
                        memcpy(nalu_payload, buffer + t * nalu_sent_len, l);
                        bytes = l + 15;
                        // BUGFIX: 检查 sendto 返回值
                        if (sendto(udpfd, sendbuf, bytes, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
                        {
                            perror("sendto failed");
                        }
                        t++;
                    }
                    else if (t < k && t != 0)
                    {
                        rtp_hdr->marker = 0;

                        h265_fu_hdr->S = 0;
                        h265_fu_hdr->E = 0;
                        nalu_payload = &sendbuf[15];
                        memcpy(nalu_payload, buffer+ t * nalu_sent_len, nalu_sent_len);
                        bytes = nalu_sent_len + 15;
                        // BUGFIX: 检查 sendto 返回值
                        if (sendto(udpfd, sendbuf, bytes, 0, (struct sockaddr *)&server, sizeof(server)) < 0)
                        {
                            perror("sendto failed");
                        }

                        t++;
                    }
                }
                #endif
                // #endregion
            }
        }
        //------------------------------------------------------------
    }

    /******************************************************************************
     * funciton : sent H265 stream   sentjin
     ******************************************************************************/
    hi_s32 sample_comm_rtsp_sentjin(ot_venc_stream *pstStream, ot_venc_stream_buf_info *stream_buf_info)
    {
        // if (timestamp == 0)
        // {
        //     timestamp = time(NULL);
        // }else
        // {
        //     temp_time = time(NULL);
        //     if(temp_time == timestamp)
        //     {
        //         jishu++;
        //     }else
        //     {
        //         sample_print("jishu = %d\n",jishu);
        //         jishu = 0;
        //         timestamp = temp_time;
        //     }
        // }
        //    
        td_u32 i, flag = 0;
        unsigned int timestamp_increse = 0;

        g_nframerate = 25; // 25fps 

        timestamp_increse = (unsigned int)(90000.0 / g_nframerate);

        // BUGFIX: 需要在 g_sendmutex 的保护下访问共享资源 g_rtspClients
        pthread_mutex_lock(&g_sendmutex);
        for (i = 0; i < MAX_RTSP_CLIENT; i++) // have atleast a connect
        {
            if (g_rtspClients[i].status == RTSP_SENDING)
            {
                flag = 1;
                g_rtspClients[i].tsvid = g_rtspClients[i].tsvid + timestamp_increse;
                break;
            }
        }
        if (flag)
        {
            for (i = 0; i < pstStream->pack_cnt; i++)
            {
                rtsp_venc_sent(pstStream->pack[i].addr + pstStream->pack[i].offset, pstStream->pack[i].len - pstStream->pack[i].offset);
            }
        }
        pthread_mutex_unlock(&g_sendmutex); // 解锁
        return TD_SUCCESS;
    }

    static char const *dateHeader()
    {
        static char buf[200];
#if !defined(_WIN32_WCE)
        time_t tt = time(NULL);
        strftime(buf, sizeof buf, "Date: %a, %b %d %Y %H:%M:%S GMT\r\n", gmtime(&tt));
#endif

        return buf;
    }

    static char *GetLocalIP(int sock)
    {
        struct ifreq ifreq;
        struct sockaddr_in *sin;
        char *LocalIP = (char *)malloc(20); // BUGFIX:  使用 malloc 分配内存

        // BUGFIX: 检查 malloc 返回值
        if (LocalIP == NULL)
        {
            perror("malloc failed");
            return NULL;
        }

        strcpy(ifreq.ifr_name, "eth0");
        if (!(ioctl(sock, SIOCGIFADDR, &ifreq)))
        {
            sin = (struct sockaddr_in *)&ifreq.ifr_addr;
            sin->sin_family = AF_INET;
            strcpy(LocalIP, inet_ntoa(sin->sin_addr));
            // inet_ntop(AF_INET, &sin->sin_addr,LocalIP, 16);
        }
        printf("--------------------------------------------%s\n", LocalIP);

        return LocalIP;
    }

    char *strDupSize(char const *str)
    {
        if (str == NULL)
            return NULL;
        size_t len = strlen(str) + 1;
        char *copy = (char *)malloc(len); // BUGFIX: 使用 malloc 分配内存
        // BUGFIX: 检查 malloc 返回值
        if (copy == NULL)
        {
            perror("malloc failed");
            return NULL;
        }
        return copy;
    }

    // 解析 RTSP 请求字符串
    // reqStr: RTSP 请求字符串
    // reqStrSize: RTSP 请求字符串长度
    // resultCmdName: 返回的 RTSP 命令名称
    // resultCmdNameMaxSize:  RTSP 命令名称最大长度
    // resultURLPreSuffix: 返回的 URL 前缀
    // resultURLPreSuffixMaxSize: URL 前缀最大长度
    // resultURLSuffix: 返回的 URL 后缀
    // resultURLSuffixMaxSize: URL 后缀最大长度
    // resultCSeq: 返回的 CSeq 值
    // resultCSeqMaxSize: CSeq 值最大长度
    // 返回值: 解析成功返回 TRUE，否则返回 FALSE
    int ParseRequestString(char const *reqStr,
                           unsigned reqStrSize,
                           char *resultCmdName,
                           unsigned resultCmdNameMaxSize,
                           char *resultURLPreSuffix,
                           unsigned resultURLPreSuffixMaxSize,
                           char *resultURLSuffix,
                           unsigned resultURLSuffixMaxSize,
                           char *resultCSeq,
                           unsigned resultCSeqMaxSize)
    {

        int parseSucceeded = FALSE;
        unsigned i;
        for (i = 0; i < resultCmdNameMaxSize - 1 && i < reqStrSize; ++i)
        {
            char c = reqStr[i];
            if (c == ' ' || c == '\t')
            {
                parseSucceeded = TRUE;
                break;
            }

            resultCmdName[i] = c;
        }
        resultCmdName[i] = '\0';
        if (!parseSucceeded)
            return FALSE;

        // Skip over the prefix of any "rtsp://" or "rtsp:/" URL that follows:
        unsigned j = i + 1;
        while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t'))
            ++j; // skip over any additional white space
        for (j = i + 1; j < reqStrSize - 8; ++j)
        {
            if ((reqStr[j] == 'r' || reqStr[j] == 'R') && (reqStr[j + 1] == 't' || reqStr[j + 1] == 'T') && (reqStr[j + 2] == 's' || reqStr[j + 2] == 'S') && (reqStr[j + 3] == 'p' || reqStr[j + 3] == 'P') && reqStr[j + 4] == ':' && reqStr[j + 5] == '/')
            {
                j += 6;
                if (reqStr[j] == '/')
                {
                    // This is a "rtsp://" URL; skip over the host:port part that follows:
                    ++j;
                    while (j < reqStrSize && reqStr[j] != '/' && reqStr[j] != ' ')
                        ++j;
                }
                else
                {
                    // This is a "rtsp:/" URL; back up to the "/":
                    --j;
                }
                i = j;
                break;
            }
        }

        // BUGFIX: 修改解析逻辑
        // Look for the URL suffix (before the following "RTSP/"):
        parseSucceeded = FALSE;
        unsigned k;
        for (k = i + 1; k < reqStrSize - 5; ++k)
        {
            if (reqStr[k] == 'R' && reqStr[k + 1] == 'T' &&
                reqStr[k + 2] == 'S' && reqStr[k + 3] == 'P' && reqStr[k + 4] == '/')
            {
                // 在这里修改解析逻辑
                unsigned k1 = k;
                while (k1 > i && reqStr[k1] != '/')
                    --k1;

                // resultURLSuffix 从 k1+1 到 k
                if (k - k1 > resultURLSuffixMaxSize)
                    return FALSE;
                unsigned n = 0, k2 = k1 + 1;
                while (k2 <= k)
                    resultURLSuffix[n++] = reqStr[k2++];
                resultURLSuffix[n] = '\0';

                // resultURLPreSuffix 从 i 到 k1-1
                if (k1 - i > resultURLPreSuffixMaxSize)
                    return FALSE;
                n = 0;
                k2 = i;
                while (k2 < k1)
                    resultURLPreSuffix[n++] = reqStr[k2++];
                resultURLPreSuffix[n] = '\0';

                i = k + 7;
                parseSucceeded = TRUE;
                break;
            }
        }

        if (!parseSucceeded)
            return FALSE;

        // Look for "CSeq:", skip whitespace,
        // then read everything up to the next \r or \n as 'CSeq':
        parseSucceeded = FALSE;
        for (j = i; j < reqStrSize - 5; ++j)
        {
            if (reqStr[j] == 'C' && reqStr[j + 1] == 'S' && reqStr[j + 2] == 'e' &&
                reqStr[j + 3] == 'q' && reqStr[j + 4] == ':')
            {
                j += 5;
                unsigned n;
                while (j < reqStrSize && (reqStr[j] == ' ' || reqStr[j] == '\t'))
                    ++j;
                for (n = 0; n < resultCSeqMaxSize - 1 && j < reqStrSize; ++n, ++j)
                {
                    char c = reqStr[j];
                    if (c == '\r' || c == '\n')
                    {
                        parseSucceeded = TRUE;
                        break;
                    }

                    resultCSeq[n] = c;
                }
                resultCSeq[n] = '\0';
                break;
            }
        }
        if (!parseSucceeded)
            return FALSE;

        return TRUE;
    }

    int OptionAnswer(char *cseq, int sock)
    {
        if (sock != 0)
        {
            char buf[1024];
            memset(buf, 0, 1024);
            char *pTemp = buf;
            pTemp += sprintf(pTemp, "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sPublic: %s\r\n\r\n",
                             cseq, dateHeader(), "OPTIONS,DESCRIBE,SETUP,PLAY,PAUSE,TEARDOWN");
            // BUGFIX: 检查 send 返回值
            int reg = send(sock, buf, strlen(buf), 0);
            if (reg <= 0)
            {
                perror("send failed"); // 打印错误信息
                return FALSE;
            }
            else
            {
                printf(">>>>>%s\n", buf);
            }
            return TRUE;
        }
        return FALSE;
    }

    int DescribeAnswer(char *cseq, int sock, char *urlSuffix, char *recvbuf)
    {
        if (sock != 0)
        {
            char sdpMsg[1024];
            char buf[2048];
            memset(buf, 0, 2048);
            memset(sdpMsg, 0, 1024);
            char *localip;
            localip = GetLocalIP(sock);

            char *pTemp = buf;
            pTemp += sprintf(pTemp, "RTSP/1.0 200 OK\r\nCSeq: %s\r\n", cseq);
            pTemp += sprintf(pTemp, "%s", dateHeader());
            pTemp += sprintf(pTemp, "Content-Type: application/sdp\r\n");

            char *pTemp2 = sdpMsg;
            pTemp2 += sprintf(pTemp2, "v=0\r\n");
            pTemp2 += sprintf(pTemp2, "o=StreamingServer 3331435948 1116907222000 IN IP4 %s\r\n", localip);
            pTemp2 += sprintf(pTemp2, "s=H.265\r\n");
            pTemp2 += sprintf(pTemp2, "c=IN IP4 0.0.0.0\r\n");
            pTemp2 += sprintf(pTemp2, "t=0 0\r\n");
            pTemp2 += sprintf(pTemp2, "a=control:*\r\n");

            /*H264 TrackID=0 RTP_PT 96*/
            pTemp2 += sprintf(pTemp2, "m=video 0 RTP/AVP 96\r\n");
            pTemp2 += sprintf(pTemp2, "a=control:track1\r\n");
            pTemp2 += sprintf(pTemp2, "a=rtpmap:96 H265/90000\r\n");
            pTemp2 += sprintf(pTemp2, "a=framerate:30\r\n");
            // pTemp2 += sprintf(pTemp2,"a=fmtp:96 packetization-mode=1; sprop-parameter-sets=%s\r\n", "AAABBCCC");
            pTemp2 += sprintf(pTemp2, "a=fmtp:96 packetization-mode=1; sprop-parameter-sets=%s\r\n", "QgEBAWAAAAMAoAAAAwAAAwB7oAPAgBDljS7kUvzcBAQE,RAHA8o54Ow==,QAEMAf//AWAAAAMAoAAAAwAAAwB7rA==");
            // pTemp2 += sprintf(pTemp2, "a=fmtp:96 profile-space=0;profile-id=1;tier-flag=0;level-id=123;interop-constraints=B00000000000;");
            // pTemp2 += sprintf(pTemp2, "sprop-vps=QAEMAf//AWAAAAMAoAAAAwAAAwB7rA==;sprop-sps=QgEBAWAAAAMAoAAAAwAAAwB7oAPAgBDljS7kUvzcBAQE;sprop-pps=RAHA8o54Ow==\r\n");
#if 0
                                                 QAEMAf//AWAAAAMAoAAAAwAAAwB7rAk=           QgEBAWAAAAMAoAAAAwAAAwB7oAPAgBDljS7kUvzcBA==           RAHA8o54OzQ=
        /*G726*/

        pTemp2 += sprintf(pTemp2, "m=audio 0 RTP/AVP 97\r\n");
        pTemp2 += sprintf(pTemp2, "a=control:trackID=1\r\n");
        if (strcmp(g_rtp_playload, "AAC") == 0)
        {
            pTemp2 += sprintf(pTemp2, "a=rtpmap:97 MPEG4-GENERIC/%d/2\r\n", 16000);
            pTemp2 += sprintf(pTemp2, "a=fmtp:97 streamtype=5;profile-level-id=1;mode=AAC-hbr;sizelength=13;indexlength=3;indexdeltalength=3;config=1410\r\n");
        }
        else
        {
            pTemp2 += sprintf(pTemp2, "a=rtpmap:97 G726-32/%d/1\r\n", 8000);
            pTemp2 += sprintf(pTemp2, "a=fmtp:97 packetization-mode=1\r\n");
        }
#endif
            pTemp += sprintf(pTemp, "Content-length: %ld\r\n", strlen(sdpMsg));
            pTemp += sprintf(pTemp, "Content-Base: rtsp://%s/%s/\r\n\r\n", localip, urlSuffix);

            // printf("mem ready\n");
            strcat(pTemp, sdpMsg);

            // BUGFIX: 释放 GetLocalIP() 分配的内存
            free(localip);

            // BUGFIX: 检查 send 返回值
            int re = send(sock, buf, strlen(buf), 0);
            if (re <= 0)
            {
                perror("send failed"); // 打印错误信息
                return FALSE;
            }
            else
            {
                printf(">>>>>%s\n", buf);
            }
        }

        return TRUE;
    }

    // 解析 Transport 头部信息
    // buf: 接收到的 RTSP 请求字符串
    // streamingMode: 返回的流媒体传输模式
    // streamingModeString: 返回的流媒体传输模式字符串
    // destinationAddressStr: 返回的目标地址字符串
    // destinationTTL: 返回的目标地址 TTL 值
    // clientRTPPortNum: 返回的客户端 RTP 端口号
    // clientRTCPPortNum: 返回的客户端 RTCP 端口号
    // rtpChannelId: 返回的 RTP 通道 ID
    // rtcpChannelId: 返回的 RTCP 通道 ID
    void ParseTransportHeader(char const *buf,
                              StreamingMode *streamingMode,
                              char **streamingModeString,
                              char **destinationAddressStr,
                              u_int8_t *destinationTTL,
                              portNumBits *clientRTPPortNum,
                              portNumBits *clientRTCPPortNum,
                              unsigned char *rtpChannelId,
                              unsigned char *rtcpChannelId)
    {
        *streamingMode = RTP_UDP;
        *streamingModeString = NULL;
        *destinationAddressStr = NULL;
        *destinationTTL = 255;
        *clientRTPPortNum = 0;
        *clientRTCPPortNum = 1;
        *rtpChannelId = *rtcpChannelId = 0xFF;

        portNumBits p1, p2;
        unsigned ttl, rtpCid, rtcpCid;

        // 查找 "Transport:" 头部字段
        while (1)
        {
            if (*buf == '\0')
                return; // 未找到，直接返回
            if (strncasecmp(buf, "Transport: ", 11) == 0)
                break;
            ++buf;
        }

        // 解析 Transport 头部字段
        char const *fields = buf + 11;
        char *field = strDupSize(fields);
        while (sscanf(fields, "%[^;]", field) == 1)
        {
            if (strcmp(field, "RTP/AVP/TCP") == 0)
            {
                *streamingMode = RTP_TCP;
            }
            else if (strcmp(field, "RAW/RAW/UDP") == 0 ||
                     strcmp(field, "MP2T/H2221/UDP") == 0)
            {
                *streamingMode = RAW_UDP;
                //*streamingModeString = strDup(field);
            }
            else if (strncasecmp(field, "destination=", 12) == 0)
            {
                // delete[] destinationAddressStr;
                free(*destinationAddressStr);
                *destinationAddressStr = strDupSize(field + 12);
            }
            else if (sscanf(field, "ttl%u", &ttl) == 1)
            {
                *destinationTTL = (u_int8_t)ttl;
            }
            else if (sscanf(field, "client_port=%hu-%hu", &p1, &p2) == 2)
            {
                *clientRTPPortNum = p1;
                *clientRTCPPortNum = p2;
            }
            else if (sscanf(field, "client_port=%hu", &p1) == 1)
            {
                *clientRTPPortNum = p1;
                *clientRTCPPortNum = *streamingMode == RAW_UDP ? 0 : p1 + 1;
            }
            else if (sscanf(field, "interleaved=%u-%u", &rtpCid, &rtcpCid) == 2)
            {
                *rtpChannelId = (unsigned char)rtpCid;
                *rtcpChannelId = (unsigned char)rtcpCid;
            }

            fields += strlen(field);
            while (*fields == ';')
                ++fields; // skip over separating ';' chars
            if (*fields == '\0' || *fields == '\r' || *fields == '\n')
                break;
        }
        free(field);
    }

    int SetupAnswer(char *cseq, int sock, int SessionId, char *urlSuffix, char *recvbuf, int *rtpport, int *rtcpport)
    {
        if (sock != 0)
        {
            char buf[1024];
            memset(buf, 0, 1024);

            StreamingMode streamingMode;
            char *streamingModeString; // set when RAW_UDP streaming is specified
            char *clientsDestinationAddressStr;
            u_int8_t clientsDestinationTTL;
            portNumBits clientRTPPortNum, clientRTCPPortNum;
            unsigned char rtpChannelId, rtcpChannelId;
            ParseTransportHeader(recvbuf, &streamingMode, &streamingModeString,
                                 &clientsDestinationAddressStr, &clientsDestinationTTL,
                                 &clientRTPPortNum, &clientRTCPPortNum,
                                 &rtpChannelId, &rtcpChannelId);

            // Port clientRTPPort(clientRTPPortNum);
            // Port clientRTCPPort(clientRTCPPortNum);
            *rtpport = clientRTPPortNum;
            *rtcpport = clientRTCPPortNum;

            char *pTemp = buf;
            char *localip;
            localip = GetLocalIP(sock);
            pTemp += sprintf(pTemp, "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sTransport: RTP/AVP;unicast;destination=%s;client_port=%d-%d;server_port=%d-%d\r\nSession: %d\r\n\r\n",
                             cseq, dateHeader(), localip,
                             ntohs(htons(clientRTPPortNum)),
                             ntohs(htons(clientRTCPPortNum)),
                             ntohs(2000),
                             ntohs(2001),
                             SessionId);

            // BUGFIX: 释放 GetLocalIP() 分配的内存
            free(localip);

            // BUGFIX: 检查 send 返回值
            int reg = send(sock, buf, strlen(buf), 0);
            if (reg <= 0)
            {
                perror("send failed"); // 打印错误信息
                return FALSE;
            }
            else
            {
                printf(">>>>>%s", buf);
            }
            return TRUE;
        }
        return FALSE;
    }

    int PlayAnswer(char *cseq, int sock, int SessionId, char *urlPre, char *recvbuf)
    {
        if (sock != 0)
        {
            char buf[1024];
            memset(buf, 0, 1024);
            char *pTemp = buf;
            char *localip;
            localip = GetLocalIP(sock);
            pTemp += sprintf(pTemp, "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sRange: npt=0.000-\r\nSession: %d\r\nRTP-Info: url=rtsp://%s/%s;seq=0\r\n\r\n",
                             cseq, dateHeader(), SessionId, localip, urlPre);
            // BUGFIX: 释放 GetLocalIP() 分配的内存
            free(localip);
            // BUGFIX: 检查 send 返回值
            int reg = send(sock, buf, strlen(buf), 0);
            if (reg <= 0)
            {
                perror("send failed");
                return FALSE;
            }
            else
            {
                printf(">>>>>%s", buf);
            }
            return TRUE;
        }
        return FALSE;
    }

    int PauseAnswer(char *cseq, int sock, char *recvbuf)
    {
        if (sock != 0)
        {
            char buf[1024];
            memset(buf, 0, 1024);
            char *pTemp = buf;
            pTemp += sprintf(pTemp, "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%s\r\n\r\n",
                             cseq, dateHeader());
            // BUGFIX: 检查 send 返回值
            int reg = send(sock, buf, strlen(buf), 0);
            if (reg <= 0)
            {
                perror("send failed");
                return FALSE;
            }
            else
            {
                printf(">>>>>%s", buf);
            }
            return TRUE;
        }
        return FALSE;
    }

    int TeardownAnswer(char *cseq, int sock, int SessionId, char *recvbuf)
    {
        if (sock != 0)
        {
            char buf[1024];
            memset(buf, 0, 1024);
            char *pTemp = buf;
            pTemp += sprintf(pTemp, "RTSP/1.0 200 OK\r\nCSeq: %s\r\n%sSession: %d\r\n\r\n",
                             cseq, dateHeader(), SessionId);

            // BUGFIX: 检查 send 返回值
            int reg = send(sock, buf, strlen(buf), 0);
            if (reg <= 0)
            {
                perror("send failed");
                return FALSE;
            }
            else
            {
                printf(">>>>>%s", buf);
            }
            return TRUE;
        }
        return FALSE;
    }

    // 处理 RTSP 客户端消息
    // pParam: 线程参数，指向 RTSP_CLIENT 结构体
    void *RtspClientMsg(void *pParam)
    {
        pthread_detach(pthread_self()); // 设置线程为分离状态
        int nRes;
        char pRecvBuf[RTSP_RECV_SIZE];
        RTSP_CLIENT *pClient = (RTSP_CLIENT *)pParam;
        memset(pRecvBuf, 0, sizeof(pRecvBuf));
        printf("RTSP:-----Create Client %s\n", pClient->IP);

        while (pClient->status != RTSP_IDLE)
        {
            // 接收客户端数据
            nRes = recv(pClient->socket, pRecvBuf, RTSP_RECV_SIZE, 0);
            if (nRes <= 0)
            {
                // 发生错误，关闭连接
                perror("recv failed");
                g_rtspClients[pClient->index].status = RTSP_IDLE;
                g_rtspClients[pClient->index].seqnum = 0;
                g_rtspClients[pClient->index].tsvid = 0;
                // g_rtspClients[pClient->index].tsaud = 0;
                close(pClient->socket);
                break;
            }

            char cmdName[PARAM_STRING_MAX];      // RTSP 命令名称
            char urlPreSuffix[PARAM_STRING_MAX]; // URL 前缀
            char urlSuffix[PARAM_STRING_MAX];    // URL 后缀
            char cseq[PARAM_STRING_MAX];         // CSeq 值

            // 解析 RTSP 请求字符串
            ParseRequestString(pRecvBuf, nRes, cmdName, sizeof(cmdName), urlPreSuffix, sizeof(urlPreSuffix),
                               urlSuffix, sizeof(urlSuffix), cseq, sizeof(cseq));

            char *p = pRecvBuf;

            printf("<<<<<%s\n", p);

            // 根据 RTSP 命令类型进行处理
            if (strstr(cmdName, "OPTIONS"))
            {
                OptionAnswer(cseq, pClient->socket);
            }
            else if (strstr(cmdName, "DESCRIBE"))
            {
                // BUGFIX: 需要在 g_sendmutex 的保护下访问共享资源 g_rtspClients
                pthread_mutex_lock(&g_sendmutex);
                DescribeAnswer(cseq, pClient->socket, urlSuffix, p);
                pthread_mutex_unlock(&g_sendmutex);
                // printf("-----------------------------DescribeAnswer %s %s\n",
                //	urlPreSuffix,urlSuffix);
            }
            else if (strstr(cmdName, "SETUP"))
            {
                int rtpport, rtcpport;
                int trackID = 0;
                SetupAnswer(cseq, pClient->socket, pClient->sessionid, urlSuffix, p, &rtpport, &rtcpport);

                // BUGFIX: urlPreSuffix 现在包含通道 ID，直接使用 atoi 转换
                trackID = atoi(urlPreSuffix);
                // sscanf(urlSuffix, "trackID=%u", &trackID);
                // printf("----------------------------------------------TrackId %d\n",trackID);
                if (trackID < 0 || trackID >= 2)
                    trackID = 0;

                // BUGFIX: 需要在 g_sendmutex 的保护下访问共享资源 g_rtspClients
                pthread_mutex_lock(&g_sendmutex);
                g_rtspClients[pClient->index].rtpport[trackID] = rtpport;
                g_rtspClients[pClient->index].rtcpport = rtcpport;
                g_rtspClients[pClient->index].reqchn = atoi(urlPreSuffix);
                if (strlen(urlPreSuffix) < 100)
                    strcpy(g_rtspClients[pClient->index].urlPre, urlPreSuffix);
                pthread_mutex_unlock(&g_sendmutex);

                // printf("-----------------------------SetupAnswer %s-%d-%d\n",
                //	urlPreSuffix,g_rtspClients[pClient->index].reqchn,rtpport);
            }
            else if (strstr(cmdName, "PLAY"))
            {
                // BUGFIX: 需要在 g_sendmutex 的保护下访问共享资源 g_rtspClients
                pthread_mutex_lock(&g_sendmutex);
                PlayAnswer(cseq, pClient->socket, pClient->sessionid, g_rtspClients[pClient->index].urlPre, p);
                g_rtspClients[pClient->index].status = RTSP_SENDING;
                pthread_mutex_unlock(&g_sendmutex);
                printf("Start Play\n");
                // printf("-----------------------------PlayAnswer %d %d\n",pClient->index);
                // usleep(100);
            }
            else if (strstr(cmdName, "PAUSE"))
            {
                PauseAnswer(cseq, pClient->socket, p);
            }
            else if (strstr(cmdName, "TEARDOWN"))
            {
                // BUGFIX: 需要在 g_sendmutex 的保护下访问共享资源 g_rtspClients
                pthread_mutex_lock(&g_sendmutex);
                TeardownAnswer(cseq, pClient->socket, pClient->sessionid, p);
                g_rtspClients[pClient->index].status = RTSP_IDLE;
                g_rtspClients[pClient->index].seqnum = 0;
                g_rtspClients[pClient->index].tsvid = 0;
                // g_rtspClients[pClient->index].tsaud = 0;
                close(pClient->socket);
                pthread_mutex_unlock(&g_sendmutex);
            }
            if (exitok)
            {
                exitok++;
                return NULL;
            }
        }
        printf("RTSP:-----Exit Client %s\n", pClient->IP);
        return NULL;
    }

    // RTSP 服务器监听线程函数
    // pParam: 线程参数，未使用
    void *RtspServerListen(void *pParam)
    {
        int s32Socket;
        struct sockaddr_in servaddr;
        int s32CSocket;
        int s32Rtn;
        int s32Socket_opt_value = 1;
        unsigned int nAddrLen;
        struct sockaddr_in addrAccept;
        // int bResult;

        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port = htons(RTSP_SERVER_PORT);

        // 创建套接字
        s32Socket = socket(AF_INET, SOCK_STREAM, 0);
        if (s32Socket < 0)
        {
            perror("socket creation failed");
            return (void *)(-1);
        }

        // 设置套接字选项，允许地址重用
        if (setsockopt(s32Socket, SOL_SOCKET, SO_REUSEADDR, &s32Socket_opt_value, sizeof(int)) == -1)
        {
            perror("setsockopt failed");
            close(s32Socket); // 关闭套接字
            return (void *)(-1);
        }

        // 绑定地址和端口
        s32Rtn = bind(s32Socket, (struct sockaddr *)&servaddr, sizeof(struct sockaddr_in));
        if (s32Rtn < 0)
        {
            perror("bind failed");
            close(s32Socket); // 关闭套接字
            return (void *)(-2);
        }

        // 监听连接
        s32Rtn = listen(s32Socket, 50); /*50,*/
        if (s32Rtn < 0)
        {
            perror("listen failed");
            close(s32Socket); // 关闭套接字
            return (void *)(-2);
        }

        nAddrLen = sizeof(struct sockaddr_in);
        int nSessionId = 1000;
        while ((s32CSocket = accept(s32Socket, (struct sockaddr *)&addrAccept, &nAddrLen)) >= 0)
        {
            printf("<<<<RTSP Client %s Connected...\n", inet_ntoa(addrAccept.sin_addr));

            int nMaxBuf = 10 * 1024; //
            if (setsockopt(s32CSocket, SOL_SOCKET, SO_SNDBUF, (char *)&nMaxBuf, sizeof(nMaxBuf)) == -1)
                printf("RTSP:!!!!!! Enalarge socket sending buffer error !!!!!!\n");
            int i;
            int bAdd = FALSE;
            for (i = 0; i < MAX_RTSP_CLIENT; i++)
            {
                if (g_rtspClients[i].status == RTSP_IDLE)
                {
                    memset(&g_rtspClients[i], 0, sizeof(RTSP_CLIENT));
                    g_rtspClients[i].index = i;
                    g_rtspClients[i].socket = s32CSocket;
                    g_rtspClients[i].status = RTSP_CONNECTED; // RTSP_SENDING;
                    g_rtspClients[i].sessionid = nSessionId++;
                    strcpy(g_rtspClients[i].IP, inet_ntoa(addrAccept.sin_addr));
                    pthread_t threadIdlsn = 0;

                    struct sched_param sched;
                    sched.sched_priority = 1; // 设置线程优先级
                    // to return ACKecho
                    pthread_create(&threadIdlsn, NULL, RtspClientMsg, &g_rtspClients[i]);
                    pthread_setschedparam(threadIdlsn, SCHED_RR, &sched);

                    bAdd = TRUE;
                    break;
                }
            }
            if (bAdd == FALSE)
            {
                memset(&g_rtspClients[0], 0, sizeof(RTSP_CLIENT));
                g_rtspClients[0].index = 0;
                g_rtspClients[0].socket = s32CSocket;
                g_rtspClients[0].status = RTSP_CONNECTED; // RTSP_SENDING;
                g_rtspClients[0].sessionid = nSessionId++;
                strcpy(g_rtspClients[0].IP, inet_ntoa(addrAccept.sin_addr));
                pthread_t threadIdlsn = 0;
                struct sched_param sched;
                sched.sched_priority = 1;
                // to return ACKecho
                pthread_create(&threadIdlsn, NULL, RtspClientMsg, &g_rtspClients[0]);
                pthread_setschedparam(threadIdlsn, SCHED_RR, &sched);
                bAdd = TRUE;
            }
            if (exitok)
            {
                exitok++;
                return NULL;
            }
        }
        if (s32CSocket < 0)
        {
            // HI_OUT_Printf(0, "RTSP listening on port %d,accept err, %d\n", RTSP_SERVER_PORT, s32CSocket);
        }

        printf("----- INIT_RTSP_Listen() Exit !! \n");

        // BUGFIX: 关闭套接字
        close(s32Socket);
        return NULL;
    }


    void InitRtspServer()
    {
        int i;
#if IS_CRYPT
        int ret =encrypt_func(USER_DECRYPT);
        if(ret != HI_SUCCESS)
        {
            printf("crpyt error !!! please check your key\n");
            return;
        }
#endif
        pthread_t threadId = 0;
        for (i = 0; i < MAX_CHAN; i++)
        {
            memset(&g_rtpPack[i], 0, sizeof(RTSP_PACK));
            g_rtpPack[i].bIsFree = TRUE;
            // g_rtpPack.bWaitIFrm = TRUE;
            memset(&g_FrmPack[i], 0, sizeof(FRAME_PACK));
        }
        // memset(g_rtp_playload, 0, sizeof(g_rtp_playload));
        // strcpy(g_rtp_playload, "G726-32");
        // g_audio_rate = 8000;
        pthread_mutex_init(&g_sendmutex, NULL);
        pthread_mutex_init(&g_mutex, NULL);
        pthread_cond_init(&g_cond, NULL);
        memset(g_rtspClients, 0, sizeof(RTSP_CLIENT) * MAX_RTSP_CLIENT);

        udpfd = socket(AF_INET, SOCK_DGRAM, 0); // UDP
        if (udpfd < 0)
        {
            perror("socket creation failed");
            exit(1);
        }

        printf("udp up\n");

        struct sched_param thdsched;
        thdsched.sched_priority = 2; // 设置线程优先级
        // to listen visiting
        pthread_create(&threadId, NULL, RtspServerListen, NULL);
        pthread_setschedparam(threadId, SCHED_RR, &thdsched);
        printf("RTSP:-----Init Rtsp server\n");
    }

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */