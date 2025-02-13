/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>

#include <unistd.h>
#include <signal.h>
#include "securec.h"
#include "sample_dpu_proc.h"
#include "sample_common_dpu.h"

#define SAMPLE_SVP_DPU_ARG_MAX_NUM 2

static char **g_ch_cmd_argv = HI_NULL;
/* function : to process abnormal case */
#ifndef __LITEOS__
static hi_void sample_svp_dpu_handle_sig(hi_s32 signo)
{
    if (signo == SIGINT || signo == SIGTERM) {
        switch (*g_ch_cmd_argv[1]) {
            case '0': {
                sample_svp_dpu_vi_vpss_rect_match_handle_sig();
                break;
                }
            case '1': {
                sample_svp_dpu_file_rect_match_handle_sig();
                break;
                }
            default:
                break;
        }
    }
}
#endif

/* function : show usage */
static hi_void sample_svp_dpu_usage(const char *prg_name)
{
    printf("Usage : %s <index> \n", prg_name);
    printf("index:\n");
    printf("\t 0) VI->VPSS->RECT->MATCH.\n");
    printf("\t 1) FILE->RECT->MATCH.\n");
}

static hi_s32 sample_svp_dpu_case(hi_char idx)
{
    hi_s32 ret;
    switch (idx) {
        case '0': {
            ret = sample_svp_dpu_vi_vpss_rect_match();
            sample_svp_dpu_check_exps_return(1, ret, "Not Support!\n");
            break;
            }
        case '1': {
            ret = sample_svp_dpu_file_rect_match();
            break;
            }
        default: {
            ret = HI_FAILURE;
            break;
            }
    }
    return ret;
}
/* function : dpu sample */
#ifdef __LITEOS__
int app_main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    hi_s32 ret;
    hi_s32 idx_len;
#ifndef __LITEOS__
    struct sigaction sa;
#endif
    if (argc != SAMPLE_SVP_DPU_ARG_MAX_NUM) {
        sample_svp_dpu_usage(argv[0]);
        return HI_FAILURE;
    }
    if (!strncmp(argv[1], "-h", SAMPLE_SVP_DPU_ARG_MAX_NUM)) {
        sample_svp_dpu_usage(argv[0]);
        return HI_SUCCESS;
    }
    g_ch_cmd_argv = argv;
#ifndef __LITEOS__
    (hi_void)memset_s(&sa, sizeof(struct sigaction), 0, sizeof(struct sigaction));
    sa.sa_handler = sample_svp_dpu_handle_sig;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
#endif
    idx_len = (hi_s32)strlen(argv[1]);
    if (idx_len != 1) {
        sample_svp_dpu_usage(argv[0]);
        return HI_FAILURE;
    }

    ret = sample_svp_dpu_case(*argv[1]);
    if (ret != HI_SUCCESS) {
        sample_svp_dpu_usage(argv[0]);
    }
    return 0;
}