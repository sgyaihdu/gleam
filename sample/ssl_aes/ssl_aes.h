#ifndef SAMPLE_SSL_AES_H
#define SAMPLE_SSL_AES_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "securec.h"
#include "hi_mpi_cipher.h"
#include "hi_mpi_km.h"

#include "hi_mpi_sys_mem.h"
#include "hi_mpi_otp.h"
#include "memmap.h"

#ifndef sample_chk_expr_goto
#define sample_chk_expr_goto(expr, expected_ret, label) do { \
    int _ret = expr;  \
    if (_ret != (expected_ret)) { \
        sample_err("%s return 0x%x\n", #expr, _ret);    \
        ret = _ret;  \
        goto label;    \
    }   \
} while (0)
#endif


#ifndef sample_chk_expr_goto_with_ret
#define sample_chk_expr_goto_with_ret(expr, expected_ret, ret, err_ret, label) do { \
    int _ret = expr;  \
    if (_ret != (expected_ret)) { \
        sample_err("%s return 0x%x\n", #expr, _ret);    \
        ret = (err_ret);  \
        goto label;    \
    }   \
} while (0)
#endif

#ifndef sample_chk_expr_return
#define sample_chk_expr_return(expr, expected_ret) do { \
    int _ret = expr;  \
    if (_ret != (expected_ret)) { \
        sample_err("%s return 0x%x\n", #expr, _ret);    \
        return _ret;    \
    }   \
} while (0)
#endif

#define sample_err(format, arg...) printf("Error: [%s-%d]" format, __FUNCTION__, __LINE__, ##arg)

#define MAX_DATA_LEN        128
#define TEST_DATA_LEN       32
#define PLAINT_KEY_LEN     32

#define IV_LEN              16

#define MAX_PLAINT_KEY     32


//芯片uid的寄存器地址
#define KEY_ADDR  0x11021200 
#define KEY_ADDR_LEN 24

#define min(x,y)   ((x) < (y) ? (x) : (y))

#define DEBUG

typedef enum {
    USER_ENCRYPT = 0,
    USER_DECRYPT = 1
}user_crypt;

typedef struct {
    const hi_char *name;
    hi_u8 clear_key[MAX_PLAINT_KEY];//明文密钥
    hi_u32 clear_len;
    hi_u8 src_data[MAX_DATA_LEN];//需要加密的数据
    hi_u32 data_len;  // 申请物理内存的长度
    hi_u32 rootkey_type;
    km_crypto_alg crypto_alg;
    km_klad_alg_type clear_alg;//密钥类型 
    crypto_symc_attr symc_attr;
    crypto_symc_ctrl_t symc_ctrl;
} symc_data_root_key_t;

hi_s32 cipher_alloc(crypto_buf_attr *buf_attr, void **virt_addr, unsigned int size);
void cipher_free(const crypto_buf_attr *buf_attr, const void *virt_addr);

hi_s32 cipher_set_rootkey(crypto_handle keyslot_handle, symc_data_root_key_t *data);

hi_s32 encrypt_func(user_crypt type);


#endif