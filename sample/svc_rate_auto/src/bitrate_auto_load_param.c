/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include "ini_parser.h"
#include "hi_type.h"
#include "ot_bitrate_auto.h"
#include "securec.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

#define ITEM_LEN  64

typedef enum {
    RATE_AUTO_MODE_BITRATE_FIRST = 0,
    RATE_AUTO_MODE_QUALITY_FIRST,
    RATE_AUTO_MODE_BUTT
}rate_auto_mode;

#define bitrate_assert_return(condition, value) \
        do { \
            if (!(condition)) { \
                printf("bitrate load param assert '%s' error, func:%s, line:%d\n", #condition, __func__, __LINE__); \
                return value; \
            } \
        } while (0)

static hi_s32 bitrate_auto_load_fg_qpmap_val_p(const ini_dictionary *dict, rate_auto_mode mode,
    rate_auto_param *rate_auto_para, hi_char item[], hi_u32 type)
{
    hi_s32 value;

    value = ini_get_int(dict, item, HI_FAILURE);
    if (value == HI_FAILURE) {
        printf("rate_auto_base.%d:svc_fg%u_qpmap_val_p failed\n", mode, type);
        return HI_FAILURE;
    }
    if (value < 0 || value > 115) { /* max value: 115, 64+51=115 */
        printf("rate_auto_base.%d:svc_fg%u_qpmap_val_p over[0, 115]\n", mode, type);
        return HI_FAILURE;
    }
    rate_auto_para->svc_fg_qpmap_val_p[type] = (hi_u8)value;

    return HI_SUCCESS;
}

static hi_s32 load_fg_qpmap_val_p(const ini_dictionary *dict, rate_auto_mode mode, rate_auto_param *rate_auto_para)
{
    hi_s32 ret;
    hi_char item[ITEM_LEN] = {0};
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg0_qpmap_val_p", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_qpmap_val_p(dict, mode, rate_auto_para, item, FG_TYPE_0);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg1_qpmap_val_p", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_qpmap_val_p(dict, mode, rate_auto_para, item, FG_TYPE_1);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg2_qpmap_val_p", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_qpmap_val_p(dict, mode, rate_auto_para, item, FG_TYPE_2);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg3_qpmap_val_p", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_qpmap_val_p(dict, mode, rate_auto_para, item, FG_TYPE_3);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg4_qpmap_val_p", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_qpmap_val_p(dict, mode, rate_auto_para, item, FG_TYPE_4);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_svc_fg_qpmap_val_i(const ini_dictionary *dict, rate_auto_mode mode,
    rate_auto_param *rate_auto_para, hi_char item[], hi_u32 type)
{
    hi_s32 value;

    value = ini_get_int(dict, item, HI_FAILURE);
    if (value == HI_FAILURE) {
        printf("rate_auto_base.%d:svc_fg%u_qpmap_val_i failed\n", mode, type);
        return HI_FAILURE;
    }

    if (value < 0 || value > 115) { /* max value: 115, 64+51=115 */
        printf("rate_auto_base.%d:svc_fg%u_qpmap_val_i over[0, 115]\n", mode, type);
        return HI_FAILURE;
    }

    rate_auto_para->svc_fg_qpmap_val_i[type] = (hi_u8)value;

    return HI_SUCCESS;
}

static hi_s32 load_fg_qpmap_val_i(const ini_dictionary *dict, rate_auto_mode mode, rate_auto_param *rate_auto_para)
{
    hi_s32 ret;
    hi_char item[ITEM_LEN] = {0};
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg0_qpmap_val_i", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_svc_fg_qpmap_val_i(dict, mode, rate_auto_para, item, FG_TYPE_0);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg1_qpmap_val_i", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_svc_fg_qpmap_val_i(dict, mode, rate_auto_para, item, FG_TYPE_1);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg2_qpmap_val_i", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_svc_fg_qpmap_val_i(dict, mode, rate_auto_para, item, FG_TYPE_2);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg3_qpmap_val_i", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_svc_fg_qpmap_val_i(dict, mode, rate_auto_para, item, FG_TYPE_3);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg4_qpmap_val_i", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_svc_fg_qpmap_val_i(dict, mode, rate_auto_para, item, FG_TYPE_4);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_load_fg_skipmap_val(const ini_dictionary *dict, rate_auto_mode mode,
    rate_auto_param *rate_auto_para, hi_char item[], hi_u32 type)
{
    hi_s32 value;

    value = ini_get_int(dict, item, HI_FAILURE);
    if (value == HI_FAILURE) {
        printf("rate_auto_base.%d:svc_fg%u_skipmap_val failed\n", mode, type);
        return HI_FAILURE;
    }

    if (value < 0 || value > 115) { /* max value: 115, 64+51=115 */
        printf("rate_auto_base.%d:svc_fg%u_skipmap_val over[0, 115]\n", mode, type);
        return HI_FAILURE;
    }

    rate_auto_para->svc_fg_skipmap_val[type] = (hi_u8)value;

    return HI_SUCCESS;
}

static hi_s32 load_fg_skipmap_val(const ini_dictionary *dict, rate_auto_mode mode, rate_auto_param *rate_auto_para)
{
    hi_s32 ret;
    hi_char item[ITEM_LEN] = {0};
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg0_skipmap_val", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_skipmap_val(dict, mode, rate_auto_para, item, FG_TYPE_0);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg1_skipmap_val", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_skipmap_val(dict, mode, rate_auto_para, item, FG_TYPE_1);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg2_skipmap_val", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_skipmap_val(dict, mode, rate_auto_para, item, FG_TYPE_2);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg3_skipmap_val", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_skipmap_val(dict, mode, rate_auto_para, item, FG_TYPE_3);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_fg4_skipmap_val", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_skipmap_val(dict, mode, rate_auto_para, item, FG_TYPE_4);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_load_param(const ini_dictionary *dict, rate_auto_mode mode,
    rate_auto_param *rate_auto_para, hi_char item[], hi_s32 *val)
{
    hi_s32 value;

    value = ini_get_int(dict, item, HI_FAILURE);
    if (value == HI_FAILURE) {
        printf("rate_auto_base.%d:load param failed\n", mode);
        return HI_FAILURE;
    }

    if (value < 0 || value > 115) { /* max value: 115, 64+51=115 */
        printf("rate_auto_base.%d:load param failed over[0, 115]\n", mode);
        return HI_FAILURE;
    }

    *val = value;

    return HI_SUCCESS;
}

static hi_s32 load_bg_val(const ini_dictionary *dict, rate_auto_mode mode, rate_auto_param *rate_auto_para)
{
    hi_s32 value, ret;
    hi_char item[ITEM_LEN] = {0};

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_bg_qpmap_val_p", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_param(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);
    rate_auto_para->svc_bg_qpmap_val_p = (hi_u8)value;

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_bg_qpmap_val_i", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_param(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->svc_bg_qpmap_val_i = (hi_u8)value;

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_bg_skipmap_val", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_param(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->svc_bg_skipmap_val = (hi_u8)value;
    return HI_SUCCESS;
}

static hi_s32 load_roi_val(const ini_dictionary *dict, rate_auto_mode mode, rate_auto_param *rate_auto_para)
{
    hi_s32 value, ret;
    hi_char item[ITEM_LEN] = {0};

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_roi_qpmap_val_p", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_param(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->svc_roi_qpmap_val_p = (hi_u8)value;

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_roi_qpmap_val_i", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_param(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->svc_roi_qpmap_val_i = (hi_u8)value;

    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:svc_roi_skipmap_val", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_param(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->svc_roi_skipmap_val = (hi_u8)value;
    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_load_avbr_rate_control(const ini_dictionary *dict, rate_auto_mode mode,
    rate_auto_param *rate_auto_para, hi_char item[], hi_s32 *val)
{
    hi_s32 value;

    value = ini_get_int(dict, item, HI_FAILURE);
    if (value == HI_FAILURE) {
        printf("rate_auto_base.%d:load param failed\n", mode);
        return HI_FAILURE;
    }

    if (value < 0 || value > 2) { /* avbr rate control value: 1, 2 */
        printf("rate_auto_base.%d:load param failed over[0,2]\n", mode);
        return HI_FAILURE;
    }

    *val = value;

    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_load_fg_protest_adjust(const ini_dictionary *dict, rate_auto_mode mode,
    rate_auto_param *rate_auto_para, hi_char item[], hi_s32 *val)
{
    hi_s32 value;

    value = ini_get_int(dict, item, HI_FAILURE);
    if (value == HI_FAILURE) {
        printf("rate_auto_base.%d:load param failed\n", mode);
        return HI_FAILURE;
    }

    if (value < 0 || value > 1) {
        printf("rate_auto_base.%d:load param failed over[0,1]\n", mode);
        return HI_FAILURE;
    }

    *val = value;

    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_load_qp(const ini_dictionary *dict, rate_auto_mode mode,
    rate_auto_param *rate_auto_para, hi_char item[], hi_s32 *val)
{
    hi_s32 value;

    value = ini_get_int(dict, item, HI_FAILURE);
    if (value == HI_FAILURE) {
        printf("rate_auto_base.%d:load param failed\n", mode);
        return HI_FAILURE;
    }

    if (value < 0 || value > 51) { /* 8bit max qp: 51 */
        printf("rate_auto_base.%d:load param failed over[0, 51]\n", mode);
        return HI_FAILURE;
    }

    *val = value;

    return HI_SUCCESS;
}

static hi_s32 rate_auto_load_param(const ini_dictionary *dict, rate_auto_mode mode, rate_auto_param *rate_auto_para)
{
    hi_s32 value, ret;
    hi_char item[ITEM_LEN] = {0};
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:avbr_rate_control", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_avbr_rate_control(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->avbr_rate_control = value;
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:max_bg_qp", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_qp(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->max_bg_qp = value;
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:min_fg_qp", mode) < 0) {
        return HI_FAILURE;
    }
    ret = bitrate_auto_load_qp(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->min_fg_qp = value;
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:max_fg_qp", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_qp(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->max_fg_qp = value;
    if (snprintf_s(item, ITEM_LEN, ITEM_LEN - 1, "rate_auto_base.%d:fg_protect_adjust", mode) < 0) {
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_fg_protest_adjust(dict, mode, rate_auto_para, item, &value);
    bitrate_assert_return(ret == HI_SUCCESS, HI_FAILURE);

    rate_auto_para->fg_protect_adjust = value;
    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_check_value(hi_s32 value)
{
    if (value < RATE_AUTO_MODE_BITRATE_FIRST || value > RATE_AUTO_MODE_QUALITY_FIRST) {
        printf("rate auto mode:%d invalid\n", value);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

static hi_s32 bitrate_auto_load_all_param(const ini_dictionary *dic, hi_s32 value, rate_auto_param *rate_auto_para)
{
    hi_s32 ret, ret1, ret2, ret3, ret4, ret5;

    ret = rate_auto_load_param(dic, value, rate_auto_para);
    ret1 = load_fg_qpmap_val_p(dic, value, rate_auto_para);
    ret2 = load_fg_qpmap_val_i(dic, value, rate_auto_para);
    ret3 = load_fg_skipmap_val(dic, value, rate_auto_para);
    ret4 = load_bg_val(dic, value, rate_auto_para);
    ret5 = load_roi_val(dic, value, rate_auto_para);
    ret = ret || ret1 || ret2 || ret3 || ret4 || ret5;

    return ret;
}
hi_s32 hi_rate_auto_load_param(hi_char *module_name, rate_auto_param *rate_auto_para)
{
    ini_dictionary *dic = NULL;
    hi_s32 value, ret;

    dic = ini_process_file(module_name);
    if (dic == NULL) {
        printf("ini_process_file fail\n");
        free(dic);
        return HI_FAILURE;
    }

    value = ini_get_int(dic, "rate_auto_mode:mode", HI_FAILURE);
    if (value == HI_FAILURE && dic != NULL) {
        printf("rate_auto_base:rate_auto_mode failed\n");
        free_ini_info_dict(dic);
        return HI_FAILURE;
    }

    printf("rate auto mode:%d\n", value);

    ret = bitrate_auto_check_value(value);
    if (ret == HI_FAILURE && dic != NULL) {
        free_ini_info_dict(dic);
        return HI_FAILURE;
    }

    ret = bitrate_auto_load_all_param(dic, value, rate_auto_para);
    if (ret != HI_SUCCESS) {
        printf("rate auto load param failed\n");
        ret = HI_FAILURE;
    }

    if (dic != NULL) {
        free_ini_info_dict(dic);
    }
    return ret;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */
