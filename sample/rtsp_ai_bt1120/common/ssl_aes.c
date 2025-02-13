
#include "ssl_aes.h"






static symc_data_root_key_t g_aes_data = 
    {
        .name = "AES-CBC-256BITS", .data_len = TEST_DATA_LEN,
        .rootkey_type = KM_KLAD_KEY_TYPE_ABRK2, .crypto_alg = KM_CRYPTO_ALG_AES,
        .clear_key = {0},
        .clear_alg = KM_KLAD_ALG_TYPE_AES, .clear_len = PLAINT_KEY_LEN,
        .symc_attr = {
            .symc_type = CRYPTO_SYMC_TYPE_NORMAL,
            .is_long_term = HI_FALSE,
        },
        .symc_ctrl = {
            .symc_alg = CRYPTO_SYMC_ALG_AES,
            .work_mode = CRYPTO_SYMC_WORK_MODE_CBC,
            .symc_key_length = CRYPTO_SYMC_KEY_256BIT,
            .symc_bit_width = CRYPTO_SYMC_BIT_WIDTH_128BIT,
            .iv_length = IV_LEN,
        },
    };

static char clear_src_data[MAX_DATA_LEN] = {0x5e,0xa0,0xd4,0x2f,0x8b,0x3c,0x79,0x1a,\
                                            0xf6,0x0d,0x9e,0x4b,0xc2,0x57,0x68,0x31,\
                                            0xe7,0xb9,0x4a,0xcf,0x06,0x21,0x9d,0x85,\
                                            0x3f,0xa2,0x7c,0x1b,0x64,0x90,0x0f,0x8c };



hi_s32 encrypt_func(user_crypt type)
{
    hi_s32 ret = HI_SUCCESS;
    hi_handle symc_handle = 0;
    crypto_handle keyslot_handle = 0;
    crypto_buf_attr src_buf = {0};
    crypto_buf_attr dst_buf = {0};
    symc_data_root_key_t *data = &g_aes_data;
    hi_u32 length = data->data_len;
    hi_void *src_virt_addr = HI_NULL;
    hi_void *dst_virt_addr = HI_NULL;

    unsigned int num = 0;

    FILE* pw = fopen("./dst_encrypted_file", "rwb"); //打开加密后文件
    if(pw == NULL) {
        printf("open dst_file failed!\n");
        return -1;
    }


    sample_chk_expr_goto(cipher_alloc(&src_buf, (hi_void **)&src_virt_addr, length), HI_SUCCESS, CIPHER_FREE);
    sample_chk_expr_goto(cipher_alloc(&dst_buf, (hi_void **)&dst_virt_addr, length), HI_SUCCESS, CIPHER_FREE);


    //映射芯片uid的寄存器
    unsigned char* addr = NULL;
    addr = memmap(KEY_ADDR,KEY_ADDR_LEN);

    if (addr == NULL) {
        printf("map mem failed!\n");
        goto CIPHER_FREE;
    }
    
    sample_chk_expr_goto(memcpy_s(data->clear_key,data->clear_len,addr,min(KEY_ADDR_LEN,data->clear_len)), 0, MEMUNMAP);//拷贝明文密钥 以芯片uid为密钥
#ifdef DEBUG
    //打印密钥
    printf("clear_key is: \n");
    for(int i = 0; i < data->clear_len; i++) {
        printf("%02hhx ",data->clear_key[i]);
    }
    printf("\n");
#endif
#ifdef DEBUG
    //打印iv
    printf("iv is: \n");
    for(int i = 0; i < IV_LEN; i++) {
        data->symc_ctrl.iv[i] = i;
        printf("%02hhx ",data->symc_ctrl.iv[i]);
    }
    printf("\n");
#endif

    /* 1. cipher init */
    sample_chk_expr_goto(hi_mpi_cipher_symc_init(), HI_SUCCESS, MEMUNMAP);

    /* 2. km init */
    sample_chk_expr_goto(hi_mpi_km_init(), HI_SUCCESS, CIPHER_DEINIT);

    /* 3. cipher create handle */
    sample_chk_expr_goto(hi_mpi_cipher_symc_create(&symc_handle, &data->symc_attr), HI_SUCCESS, KM_DEINIT);

    /* 4. create keyslot handle */
    sample_chk_expr_goto(hi_mpi_keyslot_create(&keyslot_handle, KM_KEYSLOT_TYPE_MCIPHER), HI_SUCCESS,
        CIPHER_DESTROY);

    /* 5. attach cipher handle & kslot handle */
    sample_chk_expr_goto(hi_mpi_cipher_symc_attach(symc_handle, (hi_handle)keyslot_handle), HI_SUCCESS,
        KEYSLOT_DESTROY);
    
    /* 6. set clear key */
    sample_chk_expr_goto(cipher_set_rootkey(keyslot_handle, data), HI_SUCCESS, KEYSLOT_DESTROY);
    if(type == USER_ENCRYPT) {
    /* 7. encrypt */  //加密
    /* 7.1 set config for encrypt */

        FILE* pr = fopen("./src_encrypted_file", "rb");//打开加密前文件
        if(pr == NULL) {
            printf("open src_file failed!\n");
            goto KEYSLOT_DESTROY;
        }
        num = fread(data->src_data, 1, length, pr);
        if(num != length) {
            printf("fread failed!\n");
            ret = -1;
            fclose(pr);
            goto KEYSLOT_DESTROY;
        }
        fclose(pr);

        sample_chk_expr_goto(hi_mpi_cipher_symc_set_config(symc_handle, &data->symc_ctrl), HI_SUCCESS, KEYSLOT_DESTROY);

        /* 7.2. encrypt */
        sample_chk_expr_goto_with_ret(memcpy_s(src_virt_addr, length, data->src_data, length),
           0, ret, HI_FAILURE, KEYSLOT_DESTROY);
        (hi_void)memset_s(dst_virt_addr, length, 0, length);
#ifdef DEBUG
        //打印明文
        printf("src_data:\n");
        for(int i = 0; i < length; i++) {
            printf("%02hhx ", ((char *)src_virt_addr)[i]);
        }
        printf("\n");
#endif
        sample_chk_expr_goto(hi_mpi_cipher_symc_encrypt(symc_handle, &src_buf, &dst_buf, length), HI_SUCCESS,
            KEYSLOT_DESTROY);
        //保存密文
        num = fwrite(dst_virt_addr, 1, length, pw);
        if(num != length) {
            printf("write file failed!\n");
            ret = -1;
            goto KEYSLOT_DESTROY;
        }
    #ifdef DEBUG
        //打印密文
        printf("dst_data:\n");
        for(int i = 0; i < length; i++) {
            printf("%02hhx ", ((char*)dst_virt_addr)[i]);
        }
        printf("\n");
    #endif
    }else if(type == USER_DECRYPT) {
        /* 8. decrypt */ //解密
        /* 8.1 set config for decrypt */
        sample_chk_expr_goto(hi_mpi_cipher_symc_set_config(symc_handle, &data->symc_ctrl), HI_SUCCESS, KEYSLOT_DESTROY);

        num = fread(dst_virt_addr, 1, length, pw);
        if(num != length) {
            printf("fread failed!\n");
            ret = -1;
            goto KEYSLOT_DESTROY;
        }

        /* 8.2. decrypt */
        sample_chk_expr_goto_with_ret(memcpy_s(src_virt_addr, length, dst_virt_addr, length), 0, ret,
            HI_FAILURE, KEYSLOT_DESTROY);
        (hi_void)memset_s(dst_virt_addr, length, 0, length);
        sample_chk_expr_goto(hi_mpi_cipher_symc_decrypt(symc_handle, &src_buf, &dst_buf, length), HI_SUCCESS,
            KEYSLOT_DESTROY);
#ifdef DEBUG

        //打印明文
        printf("src_data:\n");
        for(int i = 0; i < length; i++) {
            printf("%02hhx ", ((char *)src_virt_addr)[i]);
        }
        printf("\n");
        //打印密文
        printf("dst_data:\n");
        for(int i = 0; i < length; i++) {//打印解密结果
            printf("%02hhx ", ((char*)dst_virt_addr)[i]);
        }
        printf("\n");
#endif
        //对比结果
        num = memcmp(dst_virt_addr, clear_src_data, length);
        if(num != 0) {
            printf("decrypt failed!\n");
            ret = -1;
            goto KEYSLOT_DESTROY;
        }else {
            ret = 0;
            printf("decrypt success!\n");
        }

    // /* 9. compare */
    // sample_chk_expr_goto_with_ret(memcmp(dst_virt_addr, data->src_data, length),
    //     0, ret, HI_FAILURE, KEYSLOT_DESTROY);
    }



KEYSLOT_DESTROY:
    hi_mpi_keyslot_destroy(keyslot_handle);
CIPHER_DESTROY:
    hi_mpi_cipher_symc_destroy(symc_handle);
KM_DEINIT:
    hi_mpi_km_deinit();
CIPHER_DEINIT:
    hi_mpi_cipher_symc_deinit();
MEMUNMAP:
    memunmap(addr);
CIPHER_FREE:
    cipher_free(&src_buf, src_virt_addr);
    cipher_free(&dst_buf, dst_virt_addr);
    fclose(pw);
    return ret;
}


hi_s32 cipher_alloc(crypto_buf_attr *buf_attr, void **virt_addr, unsigned int size)
{
    hi_s32 ret;
    hi_phys_addr_t phys_addr;
    ret = hi_mpi_sys_mmz_alloc(&phys_addr, virt_addr, NULL, NULL, size);
    if (ret != HI_SUCCESS) {
        sample_err("hi_mpi_sys_mmz_alloc failed\n");
        return HI_FAILURE;
    }
    buf_attr->phys_addr = (unsigned long) phys_addr;
    return HI_SUCCESS;
}

void cipher_free(const crypto_buf_attr *buf_attr, const void *virt_addr)
{
    if (buf_attr->phys_addr != HI_NULL && virt_addr != HI_NULL) {
        hi_mpi_sys_mmz_free(buf_attr->phys_addr, virt_addr);
    }
}


hi_s32 cipher_set_rootkey(crypto_handle keyslot_handle, symc_data_root_key_t *data)
{
    hi_s32 ret = HI_SUCCESS;
    crypto_handle klad_handle = 0;
    #if 1
    td_u32 offset = 0x12;
    td_u8 tee_enable = 0;
    #endif
    km_klad_attr klad_attr = {
        .klad_cfg = {
            .rootkey_type = data->rootkey_type
        },
        .key_cfg = {
            .engine = data->crypto_alg,//aes
            .decrypt_support = TD_TRUE,
            .encrypt_support = TD_TRUE
        }
    };
    km_klad_clear_key klad_clear_key = {
        //.hmac_type = data->clear_alg,
        .key_size = data->clear_len,
        .key = data->clear_key
    };


#if 1
    (td_void)hi_mpi_otp_init();
    (td_void)hi_mpi_otp_read_byte(offset, &tee_enable);
    (td_void)hi_mpi_otp_deinit();
    if (tee_enable == 0x42) {
        klad_attr.key_sec_cfg.key_sec = KM_KLAD_SEC_ENABLE;
        klad_attr.key_sec_cfg.master_only_enable = HI_TRUE;
        klad_attr.key_sec_cfg.dest_buf_sec_support = HI_TRUE;
        klad_attr.key_sec_cfg.src_buf_sec_support = HI_TRUE;
        klad_attr.key_sec_cfg.src_buf_non_sec_support = HI_FALSE;
        klad_attr.key_sec_cfg.dest_buf_non_sec_support = HI_FALSE;
    } else {
    #endif
        klad_attr.key_sec_cfg.key_sec = KM_KLAD_SEC_DISABLE;
        klad_attr.key_sec_cfg.master_only_enable = HI_FALSE;
        klad_attr.key_sec_cfg.dest_buf_sec_support = HI_FALSE;
        klad_attr.key_sec_cfg.dest_buf_non_sec_support = HI_TRUE;
        klad_attr.key_sec_cfg.src_buf_sec_support = HI_FALSE;
        klad_attr.key_sec_cfg.src_buf_non_sec_support = HI_TRUE;
    #if 1
     }
     #endif

    /* 1. klad create handle */
    sample_chk_expr_return(hi_mpi_klad_create(&klad_handle), HI_SUCCESS);

    /* 2. klad set attr for rootkey */
    sample_chk_expr_goto(hi_mpi_klad_set_attr(klad_handle, &klad_attr), HI_SUCCESS, __KLAD_DESTORY__);

    /* 3. attach klad handle & kslot handle */
    sample_chk_expr_goto(hi_mpi_klad_attach(klad_handle, KM_KLAD_DEST_TYPE_MCIPHER, keyslot_handle),
        HI_SUCCESS, __KLAD_DESTORY__);

    /* 4. set clear key */
    sample_chk_expr_goto(hi_mpi_klad_set_clear_key(klad_handle, &klad_clear_key), HI_SUCCESS, __KLAD_DETACH__);

__KLAD_DETACH__:
    hi_mpi_klad_detach(klad_handle, KM_KLAD_DEST_TYPE_MCIPHER, keyslot_handle);
__KLAD_DESTORY__:
    hi_mpi_klad_destroy(klad_handle);
    return ret;
}