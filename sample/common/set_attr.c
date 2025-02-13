#include "set_attr.h"


void set_array_value_u32(td_u32 *target, const char *input_str, int array_size) {

    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        SET_VALUE(td_u32, &target[i], token);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL) ){
        printf("str too long\n");
    }
    else if ((i < array_size && token == NULL)){
        printf("str too short\n");
    }
    free(str_copy);
}
void set_array_value_u16(td_u16 *target, const char *input_str, int array_size) {
    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        SET_VALUE(td_u16, &target[i], token);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL)  ){
        printf("str too long\n");
    }
    else if ((i < array_size && token == NULL)){
        printf("str too short\n");
    }
    free(str_copy);
}
void set_array_value_s32(td_s32 *target, const char *input_str, int array_size) {
    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        SET_VALUE(td_s32, &target[i], token);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL) ){
        printf("str too long\n");
    }
    else if ((i < array_size && token == NULL)){
        printf("str too short\n");
    }
    free(str_copy);
}
void set_array_value_s16(td_s16 *target, const char *input_str, int array_size) {
    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        SET_VALUE(td_s16, &target[i], token);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL)  ){
        printf("str too long\n");
    }
    else if ((i < array_size && token == NULL)){
        printf("str too short\n");
    }
    free(str_copy);
}
void set_array_value_u8(td_u8 *target, const char *input_str, int array_size) {
    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        SET_VALUE(td_u8, &target[i], token);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL) ){
        printf("str too long\n");
    }
    else if ((i < array_size && token == NULL)){
        printf("str too short\n");
    }
    free(str_copy);
}
int set_2d_array_value(void *target, const char *input_str, int row_size, int col_size, const char *type) {
    if (target == NULL || input_str == NULL || row_size <= 0 || col_size <= 0 || type == NULL) {
        return -1;
    }

    char *str_copy = strdup(input_str); // 复制字符串，避免修改原始字符串
    if (str_copy == NULL) {
        return -1;
    }

    char *token = strtok(str_copy, ",");
    int i = 0, j = 0;

    while (token != NULL && i < row_size) {
        if (strcmp(type, "u8") == 0) {
            ((td_u8 *)target)[i * col_size + j] = (td_u8)atoi(token);
        } else if (strcmp(type, "u16") == 0) {
            ((td_u16 *)target)[i * col_size + j] = (td_u16)atoi(token);
        } else if (strcmp(type, "s32") == 0) {
            ((td_s32 *)target)[i * col_size + j] = (td_s32)atoi(token);
        } else if (strcmp(type, "u32") == 0) {
            ((td_u32 *)target)[i * col_size + j] = (td_u32)atoi(token);
        } else {
            free(str_copy);
            return -1;
        }

        j++;
        if (j >= col_size) {
            j = 0;
            i++;
        }
        token = strtok(NULL, ",");
    }
    if ((i == row_size && j == col_size && token != NULL) ){
        printf("2d str too long!\n");
    }else if( (i*col_size+j) < (row_size * col_size) && token == NULL){
        printf("2d str too short!\n");
    }

    free(str_copy);
    return 0;
}

void check_array_value_u32(td_u32 *target, const char *input_str, int array_size, const char* key) {

    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        CHECK_VALUE(td_u32, &target[i], token, key);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL) ){
        printf("%s str too long\n", key);
    }
    else if ((i < array_size && token == NULL)){
        printf("%s str too short\n", key);
    }
    free(str_copy);
}

void check_array_value_u16(td_u16 *target, const char *input_str, int array_size, const char *key) {
    char *token = strtok((char *)input_str, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        CHECK_VALUE(td_u16, &target[i], token, key);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL) ){
        printf("%s str too long\n", key);
    }
    else if ((i < array_size && token == NULL)){
        printf("%s str too short\n", key);
    }
}
void check_array_value_s16(td_s16 *target, const char *input_str, int array_size, const char *key) {
    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        CHECK_VALUE(td_s16, &target[i], token, key);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL) ){
        printf("%s str too long\n", key);
    }
    else if ((i < array_size && token == NULL)){
        printf("%s str too short\n", key);
    }
    free(str_copy);
}
void check_array_value_s32(td_s32 *target, const char *input_str, int array_size, const char *key) {
    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        CHECK_VALUE(td_s32, &target[i], token, key);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL) ){
        printf("%s str too long\n", key);
    }
    else if ((i < array_size && token == NULL)){
        printf("%s str too short\n", key);
    }
    free(str_copy);
}
void check_array_value_u8(td_u8 *target, const char *input_str, int array_size, const char *key) {
    char *str_copy = strdup(input_str);
    char *token = strtok(str_copy, ",");
    int i = 0;
    while (token != NULL && i < array_size) {
        CHECK_VALUE(td_u8, &target[i], token, key);
        token = strtok(NULL, ",");
        i++;
    }
    if ((i == array_size && token != NULL)){
        printf("%s str too long\n", key);
    }
    else if ((i < array_size && token == NULL)){
        printf("%s str too short\n", key);
    }
    free(str_copy);
}
int check_2d_array_value(void *target, const char *input_str, int row_size, int col_size, const char *type, const char* key) {
    if (target == NULL || input_str == NULL || row_size <= 0 || col_size <= 0 || type == NULL) {
        return -1;
    }

    char *str_copy = strdup(input_str);
    if (str_copy == NULL) {
        return -1;
    }

    char *token = strtok(str_copy, ",");
    int i = 0, j = 0;

    while (token != NULL && i < row_size) {
        if (strcmp(type, "u8") == 0) {
            CHECK_VALUE(td_u8, &(((td_u8 *)target)[i * col_size + j]), token, key);
        } else if (strcmp(type, "u16") == 0) {
            CHECK_VALUE(td_u16, &(((td_u16 *)target)[i * col_size + j]), token, key);
        } else if (strcmp(type, "s32") == 0) {
            CHECK_VALUE(td_s32, &(((td_s32 *)target)[i * col_size + j]), token, key);
        } else if (strcmp(type, "u32") == 0) {
            CHECK_VALUE(td_u32, &(((td_u32 *)target)[i * col_size + j]), token, key);
        } else {
            free(str_copy);
            return -1;
        }

        j++;
        if (j >= col_size) {
            j = 0;
            i++;
        }
        token = strtok(NULL, ",");
    }
    if ((i == row_size && j == col_size && token != NULL) ){
        printf("2d str too long!\n");
    }else if( (i*col_size+j) < (row_size * col_size) && token == NULL){
        printf("2d str too short!\n");
    }

    free(str_copy);
    return 0;
}

//设置AE曝光属性
hi_s32 set_isp_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // Set ISP exposure attributes
    //printf("set isp ae attr start\n");

    ot_isp_exposure_attr isp_exposure_attr = {0};
    ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &isp_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp exposure attr failed with 0x%x!\n", ret);
        return ret;
    }

    ot_isp_ae_attr *isp_ae_attr = &isp_exposure_attr.auto_attr;
    ot_isp_me_attr *isp_me_attr = &isp_exposure_attr.manual_attr;
    ot_isp_antiflicker *antiflicker = &isp_ae_attr->antiflicker;
    ot_isp_subflicker *subflicker = &isp_ae_attr->subflicker;
    ot_isp_ae_delay *ae_delay_attr = &isp_ae_attr->ae_delay_attr;

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.bypass");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_exposure_attr.bypass), name->valuestring);
    } else {
        printf("set isp_exposure_attr.bypass failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_exposure_attr.op_type), name->valuestring);
    } else {
        printf("set isp_exposure_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.ae_run_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_exposure_attr.ae_run_interval), name->valuestring);
    } else {
        printf("set isp_exposure_attr.ae_run_interval failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.hist_stat_adjust");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_exposure_attr.hist_stat_adjust), name->valuestring);
    } else {
        printf("set isp_exposure_attr.hist_stat_adjust failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.ae_route_ex_valid");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_exposure_attr.ae_route_ex_valid), name->valuestring);
    } else {
        printf("set isp_exposure_attr.ae_route_ex_valid failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.prior_frame");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_prior_frame, &(isp_exposure_attr.prior_frame), name->valuestring);
    } else {
        printf("set isp_exposure_attr.prior_frame failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.ae_gain_sep_cfg");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_exposure_attr.ae_gain_sep_cfg), name->valuestring);
    } else {
        printf("set isp_exposure_attr.ae_gain_sep_cfg failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.advance_ae");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_exposure_attr.advance_ae), name->valuestring);
    } else {
        printf("set isp_exposure_attr.advance_ae failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.exp_time_range.max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->exp_time_range.max), name->valuestring);
    } else {
        printf("set isp_ae_attr.exp_time_range.max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.exp_time_range.min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->exp_time_range.min), name->valuestring);
    } else {
        printf("set isp_ae_attr.exp_time_range.min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.a_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->a_gain_range.max), name->valuestring);
    } else {
        printf("set isp_ae_attr.a_gain_range.u32Max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.a_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->a_gain_range.min), name->valuestring);
    } else {
        printf("set isp_ae_attr.a_gain_range.u32Min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.d_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->d_gain_range.max), name->valuestring);
    } else {
        printf("set isp_ae_attr.d_gain_range.u32Max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.d_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->d_gain_range.min), name->valuestring);
    } else {
        printf("set isp_ae_attr.d_gain_range.u32Min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ispd_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->ispd_gain_range.max), name->valuestring);
    } else {
        printf("set isp_ae_attr.ispd_gain_range.u32Max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ispd_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->ispd_gain_range.min), name->valuestring);
    } else {
        printf("set isp_ae_attr.ispd_gain_range.u32Min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.sys_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->sys_gain_range.max), name->valuestring);
    } else {
        printf("set isp_ae_attr.sys_gain_range.u32Max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.sys_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->sys_gain_range.min), name->valuestring);
    } else {
        printf("set isp_ae_attr.sys_gain_range.u32Min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.gain_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->gain_threshold), name->valuestring);
    } else {
        printf("set isp_ae_attr.gain_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_ae_attr->speed), name->valuestring);
    } else {
        printf("set isp_ae_attr.speed failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.black_speed_bias");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_ae_attr->black_speed_bias), name->valuestring);
    } else {
        printf("set isp_ae_attr.black_speed_bias failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_ae_attr->tolerance), name->valuestring);
    } else {
        printf("set isp_ae_attr.tolerance failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.compensation");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_ae_attr->compensation), name->valuestring);
    } else {
        printf("set isp_ae_attr.compensation failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ev_bias");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_ae_attr->ev_bias), name->valuestring);
    } else {
        printf("set isp_ae_attr.ev_bias failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_strategy_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_ae_strategy, &(isp_ae_attr->ae_strategy_mode), name->valuestring);
    } else {
        printf("set isp_ae_attr.ae_strategy_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.hist_ratio_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_ae_attr->hist_ratio_slope), name->valuestring);
    } else {
        printf("set isp_ae_attr.hist_ratio_slope failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.max_hist_offset");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_ae_attr->max_hist_offset), name->valuestring);
    } else {
        printf("set isp_ae_attr.max_hist_offset failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_ae_mode, &(isp_ae_attr->ae_mode), name->valuestring);
    } else {
        printf("set isp_ae_attr.ae_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.antiflicker.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(antiflicker->enable), name->valuestring);
    } else {
        printf("set isp_ae_attr.antiflicker.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.antiflicker.frequency");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(antiflicker->frequency), name->valuestring);
    } else {
        printf("set isp_ae_attr.antiflicker.frequency failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.antiflicker.mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_antiflicker_mode, &(antiflicker->mode), name->valuestring);
    } else {
        printf("set isp_ae_attr.antiflicker.mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.subflicker.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(subflicker->enable), name->valuestring);
    } else {
        printf("set isp_ae_attr.subflicker.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.subflicker.luma_diff");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(subflicker->luma_diff), name->valuestring);
    } else {
        printf("set isp_ae_attr.subflicker.luma_diff failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_delay_attr.black_delay_frame");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(ae_delay_attr->black_delay_frame), name->valuestring);
    } else {
        printf("set isp_ae_attr.ae_delay_attr.black_delay_frame failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_delay_attr.white_delay_frame");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(ae_delay_attr->white_delay_frame), name->valuestring);
    } else {
        printf("set isp_ae_attr.ae_delay_attr.white_delay_frame failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.manual_exp_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_ae_attr->manual_exp_value), name->valuestring);
    } else {
        printf("set isp_ae_attr.manual_exp_value failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.exp_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_ae_attr->exp_value), name->valuestring);
    } else {
        printf("set isp_ae_attr.exp_value failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.fswdr_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_fswdr_mode, &(isp_ae_attr->fswdr_mode), name->valuestring);
    } else {
        printf("set isp_ae_attr.fswdr_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.wdr_quick");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_ae_attr->wdr_quick), name->valuestring);
    } else {
        printf("set isp_ae_attr.wdr_quick failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.iso_cal_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_ae_attr->iso_cal_coef), name->valuestring);
    } else {
        printf("set isp_ae_attr.iso_cal_coef failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.exp_time_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_me_attr->exp_time_op_type), name->valuestring);
    } else {
        printf("set isp_me_attr.exp_time_op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.a_gain_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_me_attr->a_gain_op_type), name->valuestring);
    } else {
        printf("set isp_me_attr.a_gain_op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.d_gain_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_me_attr->d_gain_op_type), name->valuestring);
    } else {
        printf("set isp_me_attr.d_gain_op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.ispd_gain_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_me_attr->ispd_gain_op_type), name->valuestring);
    } else {
        printf("set isp_me_attr.ispd_gain_op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.exp_time");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_me_attr->exp_time), name->valuestring);
    } else {
        printf("set isp_me_attr.exp_time failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.a_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_me_attr->a_gain), name->valuestring);
    } else {
        printf("set isp_me_attr.a_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.d_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_me_attr->d_gain), name->valuestring);
    } else {
        printf("set isp_me_attr.d_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.isp_d_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_me_attr->isp_d_gain), name->valuestring);
    } else {
        printf("set isp_me_attr.isp_d_gain failed\n");
    }

    ret = hi_mpi_isp_set_exposure_attr(vi_pipe, &isp_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp exposure attr failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp exposure attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // Check ISP exposure attributes
    //printf("check isp ae attr start\n");

    ot_isp_exposure_attr isp_exposure_attr = {0};
    ret = hi_mpi_isp_get_exposure_attr(vi_pipe, &isp_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp exposure attr failed with 0x%x!\n", ret);
        return ret;
    }

    ot_isp_ae_attr *isp_ae_attr = &isp_exposure_attr.auto_attr;
    ot_isp_me_attr *isp_me_attr = &isp_exposure_attr.manual_attr;
    ot_isp_antiflicker *antiflicker = &isp_ae_attr->antiflicker;
    ot_isp_subflicker *subflicker = &isp_ae_attr->subflicker;
    ot_isp_ae_delay *ae_delay_attr = &isp_ae_attr->ae_delay_attr;

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.bypass");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_exposure_attr.bypass), name->valuestring, "isp_exposure_attr.bypass");
    } else {
        printf("no attr isp_exposure_attr.bypass\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_exposure_attr.op_type), name->valuestring, "isp_exposure_attr.op_type");
    } else {
        printf("no attr isp_exposure_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.ae_run_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_exposure_attr.ae_run_interval), name->valuestring, "isp_exposure_attr.ae_run_interval");
    } else {
        printf("no attr isp_exposure_attr.ae_run_interval\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.hist_stat_adjust");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_exposure_attr.hist_stat_adjust), name->valuestring, "isp_exposure_attr.hist_stat_adjust");
    } else {
        printf("no attr isp_exposure_attr.hist_stat_adjust\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.ae_route_ex_valid");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_exposure_attr.ae_route_ex_valid), name->valuestring, "isp_exposure_attr.ae_route_ex_valid");
    } else {
        printf("no attr isp_exposure_attr.ae_route_ex_valid\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.prior_frame");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_prior_frame, &(isp_exposure_attr.prior_frame), name->valuestring, "isp_exposure_attr.prior_frame");
    } else {
        printf("no attr isp_exposure_attr.prior_frame\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.ae_gain_sep_cfg");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_exposure_attr.ae_gain_sep_cfg), name->valuestring, "isp_exposure_attr.ae_gain_sep_cfg");
    } else {
        printf("no attr isp_exposure_attr.ae_gain_sep_cfg\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_exposure_attr.advance_ae");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_exposure_attr.advance_ae), name->valuestring, "isp_exposure_attr.advance_ae");
    } else {
        printf("no attr isp_exposure_attr.advance_ae\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.exp_time_range.max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->exp_time_range.max), name->valuestring, "isp_ae_attr.exp_time_range.max");
    } else {
        printf("no attr isp_ae_attr.exp_time_range.max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.exp_time_range.min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->exp_time_range.min), name->valuestring, "isp_ae_attr.exp_time_range.min");
    } else {
        printf("no attr isp_ae_attr.exp_time_range.min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.a_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->a_gain_range.max), name->valuestring, "isp_ae_attr.a_gain_range.u32Max");
    } else {
        printf("no attr isp_ae_attr.a_gain_range.u32Max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.a_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->a_gain_range.min), name->valuestring, "isp_ae_attr.a_gain_range.u32Min");
    } else {
        printf("no attr isp_ae_attr.a_gain_range.u32Min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.d_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->d_gain_range.max), name->valuestring, "isp_ae_attr.d_gain_range.u32Max");
    } else {
        printf("no attr isp_ae_attr.d_gain_range.u32Max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.d_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->d_gain_range.min), name->valuestring, "isp_ae_attr.d_gain_range.u32Min");
    } else {
        printf("no attr isp_ae_attr.d_gain_range.u32Min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ispd_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->ispd_gain_range.max), name->valuestring, "isp_ae_attr.ispd_gain_range.u32Max");
    } else {
        printf("no attr isp_ae_attr.ispd_gain_range.u32Max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ispd_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->ispd_gain_range.min), name->valuestring, "isp_ae_attr.ispd_gain_range.u32Min");
    } else {
        printf("no attr isp_ae_attr.ispd_gain_range.u32Min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.sys_gain_range.u32Max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->sys_gain_range.max), name->valuestring, "isp_ae_attr.sys_gain_range.u32Max");
    } else {
        printf("no attr isp_ae_attr.sys_gain_range.u32Max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.sys_gain_range.u32Min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->sys_gain_range.min), name->valuestring, "isp_ae_attr.sys_gain_range.u32Min");
    } else {
        printf("no attr isp_ae_attr.sys_gain_range.u32Min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.gain_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->gain_threshold), name->valuestring, "isp_ae_attr.gain_threshold");
    } else {
        printf("no attr isp_ae_attr.gain_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_ae_attr->speed), name->valuestring, "isp_ae_attr.speed");
    } else {
        printf("no attr isp_ae_attr.speed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.black_speed_bias");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_ae_attr->black_speed_bias), name->valuestring, "isp_ae_attr.black_speed_bias");
    } else {
        printf("no attr isp_ae_attr.black_speed_bias\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_ae_attr->tolerance), name->valuestring, "isp_ae_attr.tolerance");
    } else {
        printf("no attr isp_ae_attr.tolerance\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.compensation");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_ae_attr->compensation), name->valuestring, "isp_ae_attr.compensation");
    } else {
        printf("no attr isp_ae_attr.compensation\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ev_bias");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_ae_attr->ev_bias), name->valuestring, "isp_ae_attr.ev_bias");
    } else {
        printf("no attr isp_ae_attr.ev_bias\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_strategy_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_ae_strategy, &(isp_ae_attr->ae_strategy_mode), name->valuestring, "isp_ae_attr.ae_strategy_mode");
    } else {
        printf("no attr isp_ae_attr.ae_strategy_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.hist_ratio_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_ae_attr->hist_ratio_slope), name->valuestring, "isp_ae_attr.hist_ratio_slope");
    } else {
        printf("no attr isp_ae_attr.hist_ratio_slope\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.max_hist_offset");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_ae_attr->max_hist_offset), name->valuestring, "isp_ae_attr.max_hist_offset");
    } else {
        printf("no attr isp_ae_attr.max_hist_offset\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_ae_mode, &(isp_ae_attr->ae_mode), name->valuestring, "isp_ae_attr.ae_mode");
    } else {
        printf("no attr isp_ae_attr.ae_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.antiflicker.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(antiflicker->enable), name->valuestring, "isp_ae_attr.antiflicker.enable");
    } else {
        printf("no attr isp_ae_attr.antiflicker.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.antiflicker.frequency");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(antiflicker->frequency), name->valuestring, "isp_ae_attr.antiflicker.frequency");
    } else {
        printf("no attr isp_ae_attr.antiflicker.frequency\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.antiflicker.mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_antiflicker_mode, &(antiflicker->mode), name->valuestring, "isp_ae_attr.antiflicker.mode");
    } else {
        printf("no attr isp_ae_attr.antiflicker.mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.subflicker.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(subflicker->enable), name->valuestring, "isp_ae_attr.subflicker.enable");
    } else {
        printf("no attr isp_ae_attr.subflicker.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.subflicker.luma_diff");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(subflicker->luma_diff), name->valuestring, "isp_ae_attr.subflicker.luma_diff");
    } else {
        printf("no attr isp_ae_attr.subflicker.luma_diff\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_delay_attr.black_delay_frame");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(ae_delay_attr->black_delay_frame), name->valuestring, "isp_ae_attr.ae_delay_attr.black_delay_frame");
    } else {
        printf("no attr isp_ae_attr.ae_delay_attr.black_delay_frame\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.ae_delay_attr.white_delay_frame");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(ae_delay_attr->white_delay_frame), name->valuestring, "isp_ae_attr.ae_delay_attr.white_delay_frame");
    } else {
        printf("no attr isp_ae_attr.ae_delay_attr.white_delay_frame\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.manual_exp_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_ae_attr->manual_exp_value), name->valuestring, "isp_ae_attr.manual_exp_value");
    } else {
        printf("no attr isp_ae_attr.manual_exp_value\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.exp_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_ae_attr->exp_value), name->valuestring, "isp_ae_attr.exp_value");
    } else {
        printf("no attr isp_ae_attr.exp_value\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.fswdr_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_fswdr_mode, &(isp_ae_attr->fswdr_mode), name->valuestring, "isp_ae_attr.fswdr_mode");
    } else {
        printf("no attr isp_ae_attr.fswdr_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.wdr_quick");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_ae_attr->wdr_quick), name->valuestring, "isp_ae_attr.wdr_quick");
    } else {
        printf("no attr isp_ae_attr.wdr_quick\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ae_attr.iso_cal_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_ae_attr->iso_cal_coef), name->valuestring, "isp_ae_attr.iso_cal_coef");
    } else {
        printf("no attr isp_ae_attr.iso_cal_coef\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.exp_time_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_me_attr->exp_time_op_type), name->valuestring, "isp_me_attr.exp_time_op_type");
    } else {
        printf("no attr isp_me_attr.exp_time_op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.a_gain_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_me_attr->a_gain_op_type), name->valuestring, "isp_me_attr.a_gain_op_type");
    } else {
        printf("no attr isp_me_attr.a_gain_op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.d_gain_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_me_attr->d_gain_op_type), name->valuestring, "isp_me_attr.d_gain_op_type");
    } else {
        printf("no attr isp_me_attr.d_gain_op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.ispd_gain_op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_me_attr->ispd_gain_op_type), name->valuestring, "isp_me_attr.ispd_gain_op_type");
    } else {
        printf("no attr isp_me_attr.ispd_gain_op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.exp_time");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_me_attr->exp_time), name->valuestring, "isp_me_attr.exp_time");
    } else {
        printf("no attr isp_me_attr.exp_time\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.a_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_me_attr->a_gain), name->valuestring, "isp_me_attr.a_gain");
    } else {
        printf("no attr isp_me_attr.a_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.d_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_me_attr->d_gain), name->valuestring, "isp_me_attr.d_gain");
    } else {
        printf("no attr isp_me_attr.d_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_me_attr.isp_d_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_me_attr->isp_d_gain), name->valuestring, "isp_me_attr.isp_d_gain");
    } else {
        printf("no attr isp_me_attr.isp_d_gain\n");
    }

    //printf("check write isp exposure attr success\n");
    return HI_SUCCESS;
}

//设置wdr模式、智能模式、人脸快速收敛模式下的AE曝光属性
hi_s32 set_isp_wdr_smart_fastface_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;    
    //set isp wdr_exposure_attr/smart_exposure_attr/fast_face_ae_attr
    //printf("set isp wdr_exposure_attr/smart_exposure_attr/fast_face_ae_attr start\n");
    ot_isp_wdr_exposure_attr isp_wdr_exposure_attr = {0};
    ot_isp_smart_exposure_attr isp_smart_exposure_attr = {0};
    ot_isp_fast_face_ae_attr isp_fast_face_ae_attr = {0};

    ret = hi_mpi_isp_get_wdr_exposure_attr(vi_pipe, &isp_wdr_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get wdr exposure attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_smart_exposure_attr(vi_pipe, &isp_smart_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get smart exposure attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_fast_face_ae_attr(vi_pipe, &isp_fast_face_ae_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get fast face ae attr failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_wdr_exposure_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_wdr_exposure_attr.exp_ratio_type), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.exp_ratio_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u32(isp_wdr_exposure_attr.exp_ratio, name->valuestring, OT_ISP_EXP_RATIO_NUM); 
    } else {
        printf("set isp_wdr_exposure_attr.exp_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u32, &(isp_wdr_exposure_attr.exp_ratio_max), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.exp_ratio_max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u32, &(isp_wdr_exposure_attr.exp_ratio_min), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.exp_ratio_min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_exposure_attr.tolerance), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.tolerance failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_exposure_attr.speed), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.speed failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.ratio_bias");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_exposure_attr.ratio_bias), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.ratio_bias failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.high_light_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_exposure_attr.high_light_target), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.high_light_target failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_coef_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_exposure_attr.exp_coef_min), name->valuestring); 
    } else {
        printf("set isp_wdr_exposure_attr.exp_coef_min failed\n");
    }

    // isp_smart_exposure_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_smart_exposure_attr.enable), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.ir_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_smart_exposure_attr.ir_mode), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.ir_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_exp_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_smart_exposure_attr.smart_exp_type), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.smart_exp_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.exp_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_smart_exposure_attr.exp_coef), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.exp_coef failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.luma_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_smart_exposure_attr.luma_target), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.luma_target failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.exp_coef_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_smart_exposure_attr.exp_coef_max), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.exp_coef_max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.exp_coef_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_smart_exposure_attr.exp_coef_min), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.exp_coef_min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_smart_exposure_attr.smart_interval), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.smart_interval failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_smart_exposure_attr.smart_speed), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.smart_speed failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_delay_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_smart_exposure_attr.smart_delay_num), name->valuestring); 
    } else {
        printf("set isp_smart_exposure_attr.smart_delay_num failed\n");
    }

    // isp_fast_face_ae_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_fast_face_ae_attr.enable), name->valuestring); 
    } else {
        printf("set isp_fast_face_ae_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.face_tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_fast_face_ae_attr.face_tolerance), name->valuestring); 
    } else {
        printf("set isp_fast_face_ae_attr.face_tolerance failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.face_comp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_fast_face_ae_attr.face_comp), name->valuestring); 
    } else {
        printf("set isp_fast_face_ae_attr.face_comp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.stat_delay_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_fast_face_ae_attr.stat_delay_num), name->valuestring); 
    } else {
        printf("set isp_fast_face_ae_attr.stat_delay_num failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_fast_face_ae_attr.speed), name->valuestring); 
    } else {
        printf("set isp_fast_face_ae_attr.speed failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.face_delay_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_fast_face_ae_attr.face_delay_num), name->valuestring); 
    } else {
        printf("set isp_fast_face_ae_attr.face_delay_num failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_wdr_exposure_attr(vi_pipe, &isp_wdr_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp wdr exposure attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_smart_exposure_attr(vi_pipe, &isp_smart_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp smart exposure attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_fast_face_ae_attr(vi_pipe, &isp_fast_face_ae_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp fast face ae attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp wdr_exposure_attr/smart_exposure_attr/fast_face_ae_attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_wdr_smart_fastface_exposure_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;    
    //check isp wdr_exposure_attr/smart_exposure_attr/fast_face_ae_attr
    //printf("check isp wdr_exposure_attr/smart_exposure_attr/fast_face_ae_attr start\n");
    ot_isp_wdr_exposure_attr isp_wdr_exposure_attr = {0};
    ot_isp_smart_exposure_attr isp_smart_exposure_attr = {0};
    ot_isp_fast_face_ae_attr isp_fast_face_ae_attr = {0};

    ret = hi_mpi_isp_get_wdr_exposure_attr(vi_pipe, &isp_wdr_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get wdr exposure attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_smart_exposure_attr(vi_pipe, &isp_smart_exposure_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get smart exposure attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_fast_face_ae_attr(vi_pipe, &isp_fast_face_ae_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get fast face ae attr failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_wdr_exposure_attr 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_wdr_exposure_attr.exp_ratio_type), name->valuestring, "isp_wdr_exposure_attr.exp_ratio_type"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.exp_ratio_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u32(isp_wdr_exposure_attr.exp_ratio, name->valuestring, OT_ISP_EXP_RATIO_NUM, "isp_wdr_exposure_attr.exp_ratio"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.exp_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u32, &(isp_wdr_exposure_attr.exp_ratio_max), name->valuestring, "isp_wdr_exposure_attr.exp_ratio_max"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.exp_ratio_max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_ratio_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u32, &(isp_wdr_exposure_attr.exp_ratio_min), name->valuestring, "isp_wdr_exposure_attr.exp_ratio_min"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.exp_ratio_min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_wdr_exposure_attr.tolerance), name->valuestring, "isp_wdr_exposure_attr.tolerance"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.tolerance\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_wdr_exposure_attr.speed), name->valuestring, "isp_wdr_exposure_attr.speed"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.speed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.ratio_bias");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_wdr_exposure_attr.ratio_bias), name->valuestring, "isp_wdr_exposure_attr.ratio_bias"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.ratio_bias\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.high_light_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_wdr_exposure_attr.high_light_target), name->valuestring, "isp_wdr_exposure_attr.high_light_target"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.high_light_target\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_exposure_attr.exp_coef_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_wdr_exposure_attr.exp_coef_min), name->valuestring, "isp_wdr_exposure_attr.exp_coef_min"); 
    } else {
        printf("no attr isp_wdr_exposure_attr.exp_coef_min\n");
    }

    // isp_smart_exposure_attr 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_smart_exposure_attr.enable), name->valuestring, "isp_smart_exposure_attr.enable"); 
    } else {
        printf("no attr isp_smart_exposure_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.ir_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_smart_exposure_attr.ir_mode), name->valuestring, "isp_smart_exposure_attr.ir_mode"); 
    } else {
        printf("no attr isp_smart_exposure_attr.ir_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_exp_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_smart_exposure_attr.smart_exp_type), name->valuestring, "isp_smart_exposure_attr.smart_exp_type"); 
    } else {
        printf("no attr isp_smart_exposure_attr.smart_exp_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.exp_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_smart_exposure_attr.exp_coef), name->valuestring, "isp_smart_exposure_attr.exp_coef"); 
    } else {
        printf("no attr isp_smart_exposure_attr.exp_coef\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.luma_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_smart_exposure_attr.luma_target), name->valuestring, "isp_smart_exposure_attr.luma_target"); 
    } else {
        printf("no attr isp_smart_exposure_attr.luma_target\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.exp_coef_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_smart_exposure_attr.exp_coef_max), name->valuestring, "isp_smart_exposure_attr.exp_coef_max"); 
    } else {
        printf("no attr isp_smart_exposure_attr.exp_coef_max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.exp_coef_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_smart_exposure_attr.exp_coef_min), name->valuestring, "isp_smart_exposure_attr.exp_coef_min"); 
    } else {
        printf("no attr isp_smart_exposure_attr.exp_coef_min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_smart_exposure_attr.smart_interval), name->valuestring, "isp_smart_exposure_attr.smart_interval"); 
    } else {
        printf("no attr isp_smart_exposure_attr.smart_interval\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_smart_exposure_attr.smart_speed), name->valuestring, "isp_smart_exposure_attr.smart_speed"); 
    } else {
        printf("no attr isp_smart_exposure_attr.smart_speed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_smart_exposure_attr.smart_delay_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_smart_exposure_attr.smart_delay_num), name->valuestring, "isp_smart_exposure_attr.smart_delay_num"); 
    } else {
        printf("no attr isp_smart_exposure_attr.smart_delay_num\n");
    }

    // isp_fast_face_ae_attr 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_fast_face_ae_attr.enable), name->valuestring, "isp_fast_face_ae_attr.enable"); 
    } else {
        printf("no attr isp_fast_face_ae_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.face_tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_fast_face_ae_attr.face_tolerance), name->valuestring, "isp_fast_face_ae_attr.face_tolerance"); 
    } else {
        printf("no attr isp_fast_face_ae_attr.face_tolerance\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.face_comp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_fast_face_ae_attr.face_comp), name->valuestring, "isp_fast_face_ae_attr.face_comp"); 
    } else {
        printf("no attr isp_fast_face_ae_attr.face_comp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.stat_delay_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_fast_face_ae_attr.stat_delay_num), name->valuestring, "isp_fast_face_ae_attr.stat_delay_num"); 
    } else {
        printf("no attr isp_fast_face_ae_attr.stat_delay_num\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_fast_face_ae_attr.speed), name->valuestring, "isp_fast_face_ae_attr.speed"); 
    } else {
        printf("no attr isp_fast_face_ae_attr.speed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_fast_face_ae_attr.face_delay_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_fast_face_ae_attr.face_delay_num), name->valuestring, "isp_fast_face_ae_attr.face_delay_num"); 
    } else {
        printf("no attr isp_fast_face_ae_attr.face_delay_num\n");
    }
    //printf("check isp wdr_exposure_attr/smart_exposure_attr/fast_face_ae_attr success\n");
    return HI_SUCCESS;
}

//设置光圈属性
hi_s32 set_isp_ai_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // Set ISP iris/dciris/piris attributes
    //printf("set isp ai attr start\n");

    ot_isp_iris_attr isp_iris_attr = {0};
    ot_isp_dciris_attr isp_dciris_attr = {0};
    ot_isp_piris_attr isp_piris_attr = {0};

    ret = hi_mpi_isp_get_iris_attr(vi_pipe, &isp_iris_attr);
    if (ret != HI_SUCCESS) {
        printf("iris_attr get failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dciris_attr(vi_pipe, &isp_dciris_attr);
    if (ret != HI_SUCCESS) {
        printf("dciris_attr get failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_piris_attr(vi_pipe, &isp_piris_attr);
    if (ret != HI_SUCCESS) {
        printf("piris_attr get failed with 0x%x!\n", ret);
        return ret;
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_iris_attr.enable), name->valuestring);
    } else {
        printf("set isp_iris_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_iris_attr.op_type), name->valuestring);
    } else {
        printf("set isp_iris_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.iris_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_iris_type, &(isp_iris_attr.iris_type), name->valuestring);
    } else {
        printf("set isp_iris_attr.iris_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.iris_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_iris_status, &(isp_iris_attr.iris_status), name->valuestring);
    } else {
        printf("set isp_iris_attr.iris_status failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.mi_attr.hold_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_iris_attr.mi_attr.hold_value), name->valuestring);
    } else {
        printf("set isp_iris_attr.mi_attr.hold_value failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.mi_attr.iris_fno");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_iris_f_no, &(isp_iris_attr.mi_attr.iris_fno), name->valuestring);
    } else {
        printf("set isp_iris_attr.mi_attr.iris_fno failed\n");
    }

    // isp_dciris_attr assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.kp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_s32, &(isp_dciris_attr.kp), name->valuestring);
    } else {
        printf("set isp_dciris_attr.kp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.ki");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_s32, &(isp_dciris_attr.ki), name->valuestring);
    } else {
        printf("set isp_dciris_attr.ki failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.kd");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_s32, &(isp_dciris_attr.kd), name->valuestring);
    } else {
        printf("set isp_dciris_attr.kd failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.min_pwm_duty");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_dciris_attr.min_pwm_duty), name->valuestring);
    } else {
        printf("set isp_dciris_attr.min_pwm_duty failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.max_pwm_duty");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_dciris_attr.max_pwm_duty), name->valuestring);
    } else {
        printf("set isp_dciris_attr.max_pwm_duty failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.open_pwm_duty");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_dciris_attr.open_pwm_duty), name->valuestring);
    } else {
        printf("set isp_dciris_attr.open_pwm_duty failed\n");
    }

    // isp_piris_attr assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.step_fno_table_change");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_piris_attr.step_fno_table_change), name->valuestring);
    } else {
        printf("set isp_piris_attr.step_fno_table_change failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.zero_is_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_piris_attr.zero_is_max), name->valuestring);
    } else {
        printf("set isp_piris_attr.zero_is_max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.total_step");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_piris_attr.total_step), name->valuestring);
    } else {
        printf("set isp_piris_attr.total_step failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.step_count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_piris_attr.step_count), name->valuestring);
    } else {
        printf("set isp_piris_attr.step_count failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.step_fno_table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_piris_attr.step_fno_table, name->valuestring, OT_ISP_AI_MAX_STEP_FNO_NUM);
    } else {
        printf("set isp_piris_attr.step_fno_table failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.max_iris_fno_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_iris_f_no, &(isp_piris_attr.max_iris_fno_target), name->valuestring);
    } else {
        printf("set isp_piris_attr.max_iris_fno_target failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.min_iris_fno_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_iris_f_no, &(isp_piris_attr.min_iris_fno_target), name->valuestring);
    } else {
        printf("set isp_piris_attr.min_iris_fno_target failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.fno_ex_valid");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_piris_attr.fno_ex_valid), name->valuestring);
    } else {
        printf("set isp_piris_attr.fno_ex_valid failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.max_iris_fno_target_linear");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_piris_attr.max_iris_fno_target_linear), name->valuestring);
    } else {
        printf("set isp_piris_attr.max_iris_fno_target_linear failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.min_iris_fno_target_linear");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_piris_attr.min_iris_fno_target_linear), name->valuestring);
    } else {
        printf("set isp_piris_attr.min_iris_fno_target_linear failed\n");
    }

    ret = hi_mpi_isp_set_iris_attr(vi_pipe, &isp_iris_attr);
    if (ret != HI_SUCCESS) {
        printf("iris_attr set failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_dciris_attr(vi_pipe, &isp_dciris_attr);
    if (ret != HI_SUCCESS) {
        printf("dciris_attr set failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_piris_attr(vi_pipe, &isp_piris_attr);
    if (ret != HI_SUCCESS) {
        printf("piris_attr set failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp iris/dciris/piris attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_ai_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // Check ISP iris/dciris/piris attributes
    //printf("check isp ai attr start\n");

    ot_isp_iris_attr isp_iris_attr = {0};
    ot_isp_dciris_attr isp_dciris_attr = {0};
    ot_isp_piris_attr isp_piris_attr = {0};

    ret = hi_mpi_isp_get_iris_attr(vi_pipe, &isp_iris_attr);
    if (ret != HI_SUCCESS) {
        printf("iris_attr get failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dciris_attr(vi_pipe, &isp_dciris_attr);
    if (ret != HI_SUCCESS) {
        printf("dciris_attr get failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_piris_attr(vi_pipe, &isp_piris_attr);
    if (ret != HI_SUCCESS) {
        printf("piris_attr get failed with 0x%x!\n", ret);
        return ret;
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_iris_attr.enable), name->valuestring, "isp_iris_attr.enable");
    } else {
        printf("no attr isp_iris_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_iris_attr.op_type), name->valuestring, "isp_iris_attr.op_type");
    } else {
        printf("no attr isp_iris_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.iris_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_iris_type, &(isp_iris_attr.iris_type), name->valuestring, "isp_iris_attr.iris_type");
    } else {
        printf("no attr isp_iris_attr.iris_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.iris_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_iris_status, &(isp_iris_attr.iris_status), name->valuestring, "isp_iris_attr.iris_status");
    } else {
        printf("no attr isp_iris_attr.iris_status\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.mi_attr.hold_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_iris_attr.mi_attr.hold_value), name->valuestring, "isp_iris_attr.mi_attr.hold_value");
    } else {
        printf("no attr isp_iris_attr.mi_attr.hold_value\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_iris_attr.mi_attr.iris_fno");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_iris_f_no, &(isp_iris_attr.mi_attr.iris_fno), name->valuestring, "isp_iris_attr.mi_attr.iris_fno");
    } else {
        printf("no attr isp_iris_attr.mi_attr.iris_fno\n");
    }

    // isp_dciris_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.kp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s32, &(isp_dciris_attr.kp), name->valuestring, "isp_dciris_attr.kp");
    } else {
        printf("no attr isp_dciris_attr.kp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.ki");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s32, &(isp_dciris_attr.ki), name->valuestring, "isp_dciris_attr.ki");
    } else {
        printf("no attr isp_dciris_attr.ki\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.kd");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s32, &(isp_dciris_attr.kd), name->valuestring, "isp_dciris_attr.kd");
    } else {
        printf("no attr isp_dciris_attr.kd\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.min_pwm_duty");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_dciris_attr.min_pwm_duty), name->valuestring, "isp_dciris_attr.min_pwm_duty");
    } else {
        printf("no attr isp_dciris_attr.min_pwm_duty\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.max_pwm_duty");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_dciris_attr.max_pwm_duty), name->valuestring, "isp_dciris_attr.max_pwm_duty");
    } else {
        printf("no attr isp_dciris_attr.max_pwm_duty\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dciris_attr.open_pwm_duty");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_dciris_attr.open_pwm_duty), name->valuestring, "isp_dciris_attr.open_pwm_duty");
    } else {
        printf("no attr isp_dciris_attr.open_pwm_duty\n");
    }

    // isp_piris_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.step_fno_table_change");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_piris_attr.step_fno_table_change), name->valuestring, "isp_piris_attr.step_fno_table_change");
    } else {
        printf("no attr isp_piris_attr.step_fno_table_change\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.zero_is_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_piris_attr.zero_is_max), name->valuestring, "isp_piris_attr.zero_is_max");
    } else {
        printf("no attr isp_piris_attr.zero_is_max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.total_step");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_piris_attr.total_step), name->valuestring, "isp_piris_attr.total_step");
    } else {
        printf("no attr isp_piris_attr.total_step\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.step_count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_piris_attr.step_count), name->valuestring, "isp_piris_attr.step_count");
    } else {
        printf("no attr isp_piris_attr.step_count\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.step_fno_table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_piris_attr.step_fno_table, name->valuestring, OT_ISP_AI_MAX_STEP_FNO_NUM, "isp_piris_attr.step_fno_table");
    } else {
        printf("no attr isp_piris_attr.step_fno_table\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.max_iris_fno_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_iris_f_no, &(isp_piris_attr.max_iris_fno_target), name->valuestring, "isp_piris_attr.max_iris_fno_target");
    } else {
        printf("no attr isp_piris_attr.max_iris_fno_target\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.min_iris_fno_target");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_iris_f_no, &(isp_piris_attr.min_iris_fno_target), name->valuestring, "isp_piris_attr.min_iris_fno_target");
    } else {
        printf("no attr isp_piris_attr.min_iris_fno_target\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.fno_ex_valid");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_piris_attr.fno_ex_valid), name->valuestring, "isp_piris_attr.fno_ex_valid");
    } else {
        printf("no attr isp_piris_attr.fno_ex_valid\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.max_iris_fno_target_linear");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_piris_attr.max_iris_fno_target_linear), name->valuestring, "isp_piris_attr.max_iris_fno_target_linear");
    } else {
        printf("no attr isp_piris_attr.max_iris_fno_target_linear\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_piris_attr.min_iris_fno_target_linear");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_piris_attr.min_iris_fno_target_linear), name->valuestring, "isp_piris_attr.min_iris_fno_target_linear");
    } else {
        printf("no attr isp_piris_attr.min_iris_fno_target_linear\n");
    }

    //printf("check write isp iris/dciris/piris attr success\n");
    return HI_SUCCESS;
}


//设置白平衡属性
hi_s32 set_isp_wb_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // Set ISP wb attributes
    //printf("set isp wb attr start\n");

    ot_isp_wb_attr isp_wb_attr = {0};

    ret = hi_mpi_isp_get_wb_attr(vi_pipe, &isp_wb_attr);
    if (ret != HI_SUCCESS) {
        printf("wb_attr get failed with 0x%x!\n", ret);
        return ret;
    }
    // isp_wb_attr assignment

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.bypass");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.bypass), name->valuestring);
    } else {
        printf("set isp_wb_attr.bypass failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.awb_run_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_wb_attr.awb_run_interval), name->valuestring);
    } else {
        printf("set isp_wb_attr.awb_run_interval failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_wb_attr.op_type), name->valuestring);
    } else {
        printf("set isp_wb_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.alg_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_awb_alg, &(isp_wb_attr.alg_type), name->valuestring);
    } else {
        printf("set isp_wb_attr.alg_type failed\n");
    }

    // isp_awb_attr assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.enable), name->valuestring);
    } else {
        printf("set isp_awb_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ref_color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.ref_color_temp), name->valuestring);
    } else {
        printf("set isp_awb_attr.ref_color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.static_wb");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_wb_attr.auto_attr.static_wb, name->valuestring, OT_ISP_BAYER_CHN_NUM);
    } else {
        printf("set isp_awb_attr.static_wb failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.curve_para");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_s32(isp_wb_attr.auto_attr.curve_para, name->valuestring, OT_ISP_AWB_CURVE_PARA_NUM);
    } else {
        printf("set isp_awb_attr.curve_para failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.alg_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_awb_alg_type, &(isp_wb_attr.auto_attr.alg_type), name->valuestring);
    } else {
        printf("set isp_awb_attr.alg_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.rg_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_wb_attr.auto_attr.rg_strength), name->valuestring);
    } else {
        printf("set isp_awb_attr.rg_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.bg_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_wb_attr.auto_attr.bg_strength), name->valuestring);
    } else {
        printf("set isp_awb_attr.bg_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.speed), name->valuestring);
    } else {
        printf("set isp_awb_attr.speed failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.zone_sel");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.zone_sel), name->valuestring);
    } else {
        printf("set isp_awb_attr.zone_sel failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.high_color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.high_color_temp), name->valuestring);
    } else {
        printf("set isp_awb_attr.high_color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.low_color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.low_color_temp), name->valuestring);
    } else {
        printf("set isp_awb_attr.low_color_temp failed\n");
    }

    // isp_awb_attr.ct_limit assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.ct_limit.enable), name->valuestring);
    } else {
        printf("set isp_awb_attr.ct_limit.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_wb_attr.auto_attr.ct_limit.op_type), name->valuestring);
    } else {
        printf("set isp_awb_attr.ct_limit.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.high_rg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.high_rg_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr.ct_limit.high_rg_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.high_bg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.high_bg_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr.ct_limit.high_bg_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.low_rg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.low_rg_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr.ct_limit.low_rg_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.low_bg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.low_bg_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr.ct_limit.low_bg_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.shift_limit_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.shift_limit_en), name->valuestring);
    } else {
        printf("set isp_awb_attr.shift_limit_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.shift_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_wb_attr.auto_attr.shift_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr.shift_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.gain_norm_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.gain_norm_en), name->valuestring);
    } else {
        printf("set isp_awb_attr.gain_norm_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.natural_cast_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.natural_cast_en), name->valuestring);
    } else {
        printf("set isp_awb_attr.natural_cast_en failed\n");
    }

    // isp_awb_attr.cb_cr_track assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.cb_cr_track.enable), name->valuestring);
    } else {
        printf("set isp_awb_attr.cb_cr_track.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cr_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cr_max, name->valuestring, OT_ISP_AUTO_ISO_NUM);
    } else {
        printf("set isp_awb_attr.cb_cr_track.cr_max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cr_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cr_min, name->valuestring, OT_ISP_AUTO_ISO_NUM);
    } else {
        printf("set isp_awb_attr.cb_cr_track.cr_min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cb_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cb_max, name->valuestring, OT_ISP_AUTO_ISO_NUM);
    } else {
        printf("set isp_awb_attr.cb_max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cb_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cb_min, name->valuestring, OT_ISP_AUTO_ISO_NUM);
    } else {
        printf("set isp_awb_attr.cb_cr_track.cb_min failed\n");
    }

    // isp_awb_attr.luma_hist assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.luma_hist.enable), name->valuestring);
    } else {
        printf("set isp_awb_attr.luma_hist.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_wb_attr.auto_attr.luma_hist.op_type), name->valuestring);
    } else {
        printf("set isp_awb_attr.luma_hist.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.hist_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u8(isp_wb_attr.auto_attr.luma_hist.hist_thresh, name->valuestring, OT_ISP_AWB_LUM_HIST_NUM);
    } else {
        printf("set isp_awb_attr.luma_hist.hist_thresh failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.hist_wt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_wb_attr.auto_attr.luma_hist.hist_wt, name->valuestring, OT_ISP_AWB_LUM_HIST_NUM);
    } else {
        printf("set isp_awb_attr.luma_hist.hist_wt failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.awb_zone_wt_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_wb_attr.auto_attr.awb_zone_wt_en), name->valuestring);
    } else {
        printf("set isp_awb_attr.awb_zone_wt_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.zone_wt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u8(isp_wb_attr.auto_attr.zone_wt, name->valuestring, OT_ISP_AWB_ZONE_NUM);
    } else {
        printf("set isp_awb_attr.zone_wt failed\n");
    }

    // isp_mwb_attr assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.manual_attr.r_gain), name->valuestring);
    } else {
        printf("set isp_mwb_attr.r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.gr_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.manual_attr.gr_gain), name->valuestring);
    } else {
        printf("set isp_mwb_attr.gr_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.gb_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.manual_attr.gb_gain), name->valuestring);
    } else {
        printf("set isp_mwb_attr.gb_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_wb_attr.manual_attr.b_gain), name->valuestring);
    } else {
        printf("set isp_mwb_attr.b_gain failed\n");
    }

    ret = hi_mpi_isp_set_wb_attr(vi_pipe, &isp_wb_attr);
    if (ret != HI_SUCCESS) {
        printf("wb_attr set failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp wb attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_wb_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // Check ISP wb attributes
    //printf("check isp wb attr start\n");

    ot_isp_wb_attr isp_wb_attr = {0};

    ret = hi_mpi_isp_get_wb_attr(vi_pipe, &isp_wb_attr);
    if (ret != HI_SUCCESS) {
        printf("wb_attr get failed with 0x%x!\n", ret);
        return ret;
    }
    // isp_wb_attr checking

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.bypass");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.bypass), name->valuestring, "isp_wb_attr.bypass");
    } else {
        printf("no attr isp_wb_attr.bypass\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.awb_run_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wb_attr.awb_run_interval), name->valuestring, "isp_wb_attr.awb_run_interval");
    } else {
        printf("no attr isp_wb_attr.awb_run_interval\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_wb_attr.op_type), name->valuestring, "isp_wb_attr.op_type");
    } else {
        printf("no attr isp_wb_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wb_attr.alg_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_awb_alg, &(isp_wb_attr.alg_type), name->valuestring, "isp_wb_attr.alg_type");
    } else {
        printf("no attr isp_wb_attr.alg_type\n");
    }

    // isp_awb_attr checking
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.enable), name->valuestring, "isp_awb_attr.enable");
    } else {
        printf("no attr isp_awb_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ref_color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.ref_color_temp), name->valuestring, "isp_awb_attr.ref_color_temp");
    } else {
        printf("no attr isp_awb_attr.ref_color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.static_wb");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_wb_attr.auto_attr.static_wb, name->valuestring, OT_ISP_BAYER_CHN_NUM, "isp_awb_attr.static_wb");
    } else {
        printf("no attr isp_awb_attr.static_wb\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.curve_para");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_s32(isp_wb_attr.auto_attr.curve_para, name->valuestring, OT_ISP_AWB_CURVE_PARA_NUM, "isp_awb_attr.curve_para");
    } else {
        printf("no attr isp_awb_attr.curve_para\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.alg_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_awb_alg_type, &(isp_wb_attr.auto_attr.alg_type), name->valuestring, "isp_awb_attr.alg_type");
    } else {
        printf("no attr isp_awb_attr.alg_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.rg_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wb_attr.auto_attr.rg_strength), name->valuestring, "isp_awb_attr.rg_strength");
    } else {
        printf("no attr isp_awb_attr.rg_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.bg_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wb_attr.auto_attr.bg_strength), name->valuestring, "isp_awb_attr.bg_strength");
    } else {
        printf("no attr isp_awb_attr.bg_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.speed");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.speed), name->valuestring, "isp_awb_attr.speed");
    } else {
        printf("no attr isp_awb_attr.speed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.zone_sel");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.zone_sel), name->valuestring, "isp_awb_attr.zone_sel");
    } else {
        printf("no attr isp_awb_attr.zone_sel\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.high_color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.high_color_temp), name->valuestring, "isp_awb_attr.high_color_temp");
    } else {
        printf("no attr isp_awb_attr.high_color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.low_color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.low_color_temp), name->valuestring, "isp_awb_attr.low_color_temp");
    } else {
        printf("no attr isp_awb_attr.low_color_temp\n");
    }

    // isp_awb_attr.ct_limit checking
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.ct_limit.enable), name->valuestring, "isp_awb_attr.ct_limit.enable");
    } else {
        printf("no attr isp_awb_attr.ct_limit.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_wb_attr.auto_attr.ct_limit.op_type), name->valuestring, "isp_awb_attr.ct_limit.op_type");
    } else {
        printf("no attr isp_awb_attr.ct_limit.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.high_rg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.high_rg_limit), name->valuestring, "isp_awb_attr.ct_limit.high_rg_limit");
    } else {
        printf("no attr isp_awb_attr.ct_limit.high_rg_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.high_bg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.high_bg_limit), name->valuestring, "isp_awb_attr.ct_limit.high_bg_limit");
    } else {
        printf("no attr isp_awb_attr.ct_limit.high_bg_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.low_rg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.low_rg_limit), name->valuestring, "isp_awb_attr.ct_limit.low_rg_limit");
    } else {
        printf("no attr isp_awb_attr.ct_limit.low_rg_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.ct_limit.low_bg_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.auto_attr.ct_limit.low_bg_limit), name->valuestring, "isp_awb_attr.ct_limit.low_bg_limit");
    } else {
        printf("no attr isp_awb_attr.ct_limit.low_bg_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.shift_limit_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.shift_limit_en), name->valuestring, "isp_awb_attr.shift_limit_en");
    } else {
        printf("no attr isp_awb_attr.shift_limit_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.shift_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wb_attr.auto_attr.shift_limit), name->valuestring, "isp_awb_attr.shift_limit");
    } else {
        printf("no attr isp_awb_attr.shift_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.gain_norm_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.gain_norm_en), name->valuestring, "isp_awb_attr.gain_norm_en");
    } else {
        printf("no attr isp_awb_attr.gain_norm_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.natural_cast_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.natural_cast_en), name->valuestring, "isp_awb_attr.natural_cast_en");
    } else {
        printf("no attr isp_awb_attr.natural_cast_en\n");
    }

    // isp_awb_attr.cb_cr_track checking
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.cb_cr_track.enable), name->valuestring, "isp_awb_attr.cb_cr_track.enable");
    } else {
        printf("no attr isp_awb_attr.cb_cr_track.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cr_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cr_max, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_awb_attr.cb_cr_track.cr_max");
    } else {
        printf("no attr isp_awb_attr.cb_cr_track.cr_max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cr_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cr_min, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_awb_attr.cb_cr_track.cr_min");
    } else {
        printf("no attr isp_awb_attr.cb_cr_track.cr_min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cb_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cb_max, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_awb_attr.cb_max");
    } else {
        printf("no attr isp_awb_attr.cb_max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.cb_cr_track.cb_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_wb_attr.auto_attr.cb_cr_track.cb_min, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_awb_attr.cb_cr_track.cb_min");
    } else {
        printf("no attr isp_awb_attr.cb_cr_track.cb_min\n");
    }

    // isp_awb_attr.luma_hist checking
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.luma_hist.enable), name->valuestring, "isp_awb_attr.luma_hist.enable");
    } else {
        printf("no attr isp_awb_attr.luma_hist.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_wb_attr.auto_attr.luma_hist.op_type), name->valuestring, "isp_awb_attr.luma_hist.op_type");
    } else {
        printf("no attr isp_awb_attr.luma_hist.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.hist_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u8(isp_wb_attr.auto_attr.luma_hist.hist_thresh, name->valuestring, OT_ISP_AWB_LUM_HIST_NUM, "isp_awb_attr.luma_hist.hist_thresh");
    } else {
        printf("no attr isp_awb_attr.luma_hist.hist_thresh\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.luma_hist.hist_wt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_wb_attr.auto_attr.luma_hist.hist_wt, name->valuestring, OT_ISP_AWB_LUM_HIST_NUM, "isp_awb_attr.luma_hist.hist_wt");
    } else {
        printf("no attr isp_awb_attr.luma_hist.hist_wt\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.awb_zone_wt_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wb_attr.auto_attr.awb_zone_wt_en), name->valuestring, "isp_awb_attr.awb_zone_wt_en");
    } else {
        printf("no attr isp_awb_attr.awb_zone_wt_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr.zone_wt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u8(isp_wb_attr.auto_attr.zone_wt, name->valuestring, OT_ISP_AWB_ZONE_NUM, "isp_awb_attr.zone_wt");
    } else {
        printf("no attr isp_awb_attr.zone_wt\n");
    }

    // isp_mwb_attr assignment
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.manual_attr.r_gain), name->valuestring, "isp_mwb_attr.r_gain");
    } else {
        printf("no attr isp_mwb_attr.r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.gr_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.manual_attr.gr_gain), name->valuestring, "isp_mwb_attr.gr_gain");
    } else {
        printf("no attr isp_mwb_attr.gr_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.gb_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.manual_attr.gb_gain), name->valuestring, "isp_mwb_attr.gb_gain");
    } else {
        printf("no attr isp_mwb_attr.gb_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_mwb_attr.b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wb_attr.manual_attr.b_gain), name->valuestring, "isp_mwb_attr.b_gain");
    } else {
        printf("no attr isp_mwb_attr.b_gain\n");
    }
    //printf("check isp wb attr success\n");
    return HI_SUCCESS;
}


//设置自动白平衡扩展属性
hi_s32 set_isp_awb_attr_ex_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // set isp awb_attr_ex
    //printf("set isp awb_attr_ex start\n");
    ot_isp_awb_attr_ex isp_awb_attr_ex = {0};
    ret = hi_mpi_isp_get_awb_attr_ex(vi_pipe, &isp_awb_attr_ex);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_isp_get_awb_attr_ex failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_awb_attr_ex 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.tolerance), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.tolerance failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.zone_radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.zone_radius), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.zone_radius failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.curve_l_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.curve_l_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.curve_l_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.curve_r_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.curve_r_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.curve_r_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.extra_light_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_awb_attr_ex.extra_light_en), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.extra_light_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[0].white_r_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[0].white_r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[0].white_b_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[0].white_b_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[0].exp_quant), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[0].exp_quant failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[0].light_status), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[0].light_status failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[0].radius), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[0].radius failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[1].white_r_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[1].white_r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[1].white_b_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[1].white_b_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[1].exp_quant), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[1].exp_quant failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[1].light_status), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[1].light_status failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[1].radius), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[1].radius failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[2].white_r_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[2].white_r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[2].white_b_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[2].white_b_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[2].exp_quant), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[2].exp_quant failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[2].light_status), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[2].light_status failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[2].radius), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[2].radius failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[3].white_r_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[3].white_r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[3].white_b_gain), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[3].white_b_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.light_info[3].exp_quant), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[3].exp_quant failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[3].light_status), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[3].light_status failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.light_info[3].radius), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.light_info[3].radius failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_awb_attr_ex.in_or_out.enable), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_awb_attr_ex.in_or_out.op_type), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.scene_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_awb_scene_mode_status, &(isp_awb_attr_ex.in_or_out.scene_status), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.scene_status failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.out_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_awb_attr_ex.in_or_out.out_thresh), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.out_thresh failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.low_start");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.low_start), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.low_start failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.low_stop");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.low_stop), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.low_stop failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.high_start");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.high_start), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.high_start failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.high_stop");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.high_stop), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.high_stop failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.green_enhance_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_awb_attr_ex.in_or_out.green_enhance_en), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.green_enhance_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.out_shift_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.in_or_out.out_shift_limit), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.in_or_out.out_shift_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_light_source_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_awb_attr_ex.multi_light_source_en), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.multi_light_source_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ls_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_awb_multi_ls_type, &(isp_awb_attr_ex.multi_ls_type), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.multi_ls_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ls_scaler");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_awb_attr_ex.multi_ls_scaler), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.multi_ls_scaler failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ct_bin");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_awb_attr_ex.multi_ct_bin, name->valuestring, OT_ISP_AWB_MULTI_CT_NUM);
    } else {
        printf("set isp_awb_attr_ex.multi_ct_bin failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ct_wt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_awb_attr_ex.multi_ct_wt, name->valuestring, OT_ISP_AWB_MULTI_CT_NUM);
    } else {
        printf("set isp_awb_attr_ex.multi_ct_wt failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.fine_tun_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_awb_attr_ex.fine_tun_en), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.fine_tun_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.fine_tun_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_awb_attr_ex.fine_tun_strength), name->valuestring);
    } else {
        printf("set isp_awb_attr_ex.fine_tun_strength failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_awb_attr_ex(vi_pipe, &isp_awb_attr_ex);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_isp_set_awb_attr_ex failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp awb_attr_ex success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_awb_attr_ex_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // check isp awb_attr_ex
    //printf("check isp awb_attr_ex start\n");
    ot_isp_awb_attr_ex isp_awb_attr_ex = {0};
    ret = hi_mpi_isp_get_awb_attr_ex(vi_pipe, &isp_awb_attr_ex);
    if (ret != HI_SUCCESS) {
        printf("hi_mpi_isp_get_awb_attr_ex failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_awb_attr_ex 检查值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.tolerance), name->valuestring, "isp_awb_attr_ex.tolerance");
    } else {
        printf("no attr isp_awb_attr_ex.tolerance\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.zone_radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.zone_radius), name->valuestring, "isp_awb_attr_ex.zone_radius");
    } else {
        printf("no attr isp_awb_attr_ex.zone_radius\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.curve_l_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.curve_l_limit), name->valuestring, "isp_awb_attr_ex.curve_l_limit");
    } else {
        printf("no attr isp_awb_attr_ex.curve_l_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.curve_r_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.curve_r_limit), name->valuestring, "isp_awb_attr_ex.curve_r_limit");
    } else {
        printf("no attr isp_awb_attr_ex.curve_r_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.extra_light_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_awb_attr_ex.extra_light_en), name->valuestring, "isp_awb_attr_ex.extra_light_en");
    } else {
        printf("no attr isp_awb_attr_ex.extra_light_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[0].white_r_gain), name->valuestring, "isp_awb_attr_ex.light_info[0].white_r_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[0].white_r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[0].white_b_gain), name->valuestring, "isp_awb_attr_ex.light_info[0].white_b_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[0].white_b_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[0].exp_quant), name->valuestring, "isp_awb_attr_ex.light_info[0].exp_quant");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[0].exp_quant\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[0].light_status), name->valuestring, "isp_awb_attr_ex.light_info[0].light_status");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[0].light_status\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[0].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[0].radius), name->valuestring, "isp_awb_attr_ex.light_info[0].radius");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[0].radius\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[1].white_r_gain), name->valuestring, "isp_awb_attr_ex.light_info[1].white_r_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[1].white_r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[1].white_b_gain), name->valuestring, "isp_awb_attr_ex.light_info[1].white_b_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[1].white_b_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[1].exp_quant), name->valuestring, "isp_awb_attr_ex.light_info[1].exp_quant");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[1].exp_quant\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[1].light_status), name->valuestring, "isp_awb_attr_ex.light_info[1].light_status");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[1].light_status\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[1].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[1].radius), name->valuestring, "isp_awb_attr_ex.light_info[1].radius");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[1].radius\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[2].white_r_gain), name->valuestring, "isp_awb_attr_ex.light_info[2].white_r_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[2].white_r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[2].white_b_gain), name->valuestring, "isp_awb_attr_ex.light_info[2].white_b_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[2].white_b_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[2].exp_quant), name->valuestring, "isp_awb_attr_ex.light_info[2].exp_quant");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[2].exp_quant\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[2].light_status), name->valuestring, "isp_awb_attr_ex.light_info[2].light_status");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[2].light_status\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[2].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[2].radius), name->valuestring, "isp_awb_attr_ex.light_info[2].radius");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[2].radius\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].white_r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[3].white_r_gain), name->valuestring, "isp_awb_attr_ex.light_info[3].white_r_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[3].white_r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].white_b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[3].white_b_gain), name->valuestring, "isp_awb_attr_ex.light_info[3].white_b_gain");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[3].white_b_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].exp_quant");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.light_info[3].exp_quant), name->valuestring, "isp_awb_attr_ex.light_info[3].exp_quant");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[3].exp_quant\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].light_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[3].light_status), name->valuestring, "isp_awb_attr_ex.light_info[3].light_status");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[3].light_status\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.light_info[3].radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.light_info[3].radius), name->valuestring, "isp_awb_attr_ex.light_info[3].radius");
    } else {
        printf("no attr isp_awb_attr_ex.light_info[3].radius\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_awb_attr_ex.in_or_out.enable), name->valuestring, "isp_awb_attr_ex.in_or_out.enable");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_awb_attr_ex.in_or_out.op_type), name->valuestring, "isp_awb_attr_ex.in_or_out.op_type");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.scene_status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_awb_scene_mode_status, &(isp_awb_attr_ex.in_or_out.scene_status), name->valuestring, "isp_awb_attr_ex.in_or_out.scene_status");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.scene_status\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.out_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_awb_attr_ex.in_or_out.out_thresh), name->valuestring, "isp_awb_attr_ex.in_or_out.out_thresh");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.out_thresh\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.low_start");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.low_start), name->valuestring, "isp_awb_attr_ex.in_or_out.low_start");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.low_start\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.low_stop");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.low_stop), name->valuestring, "isp_awb_attr_ex.in_or_out.low_stop");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.low_stop\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.high_start");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.high_start), name->valuestring, "isp_awb_attr_ex.in_or_out.high_start");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.high_start\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.high_stop");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.in_or_out.high_stop), name->valuestring, "isp_awb_attr_ex.in_or_out.high_stop");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.high_stop\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.green_enhance_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_awb_attr_ex.in_or_out.green_enhance_en), name->valuestring, "isp_awb_attr_ex.in_or_out.green_enhance_en");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.green_enhance_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.in_or_out.out_shift_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.in_or_out.out_shift_limit), name->valuestring, "isp_awb_attr_ex.in_or_out.out_shift_limit");
    } else {
        printf("no attr isp_awb_attr_ex.in_or_out.out_shift_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_light_source_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_awb_attr_ex.multi_light_source_en), name->valuestring, "isp_awb_attr_ex.multi_light_source_en");
    } else {
        printf("no attr isp_awb_attr_ex.multi_light_source_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ls_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_awb_multi_ls_type, &(isp_awb_attr_ex.multi_ls_type), name->valuestring, "isp_awb_attr_ex.multi_ls_type");
    } else {
        printf("no attr isp_awb_attr_ex.multi_ls_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ls_scaler");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_awb_attr_ex.multi_ls_scaler), name->valuestring, "isp_awb_attr_ex.multi_ls_scaler");
    } else {
        printf("no attr isp_awb_attr_ex.multi_ls_scaler\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ct_bin");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_awb_attr_ex.multi_ct_bin, name->valuestring, OT_ISP_AWB_MULTI_CT_NUM, "isp_awb_attr_ex.multi_ct_bin");
    } else {
        printf("no attr isp_awb_attr_ex.multi_ct_bin\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.multi_ct_wt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_awb_attr_ex.multi_ct_wt, name->valuestring, OT_ISP_AWB_MULTI_CT_NUM, "isp_awb_attr_ex.multi_ct_wt");
    } else {
        printf("no attr isp_awb_attr_ex.multi_ct_wt\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.fine_tun_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_awb_attr_ex.fine_tun_en), name->valuestring, "isp_awb_attr_ex.fine_tun_en");
    } else {
        printf("no attr isp_awb_attr_ex.fine_tun_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_awb_attr_ex.fine_tun_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_awb_attr_ex.fine_tun_strength), name->valuestring, "isp_awb_attr_ex.fine_tun_strength");
    } else {
        printf("no attr isp_awb_attr_ex.fine_tun_strength\n");
    }
    //printf("check write isp awb_attr_ex success\n");

    return HI_SUCCESS;
}


//设置色彩相关矩阵(CCM)属性
hi_s32 set_isp_ccm_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // set isp saturation/color_matrix attr
    //printf("set isp saturation/color_matrix attr start\n");

    ot_isp_saturation_attr isp_saturation_attr = {0};
    ot_isp_color_matrix_attr isp_color_matrix_attr = {0};
    ret = hi_mpi_isp_get_saturation_attr(vi_pipe, &isp_saturation_attr);
    if (ret != HI_SUCCESS) {
        printf("Saturation get failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_ccm_attr(vi_pipe, &isp_color_matrix_attr);
    if (ret != HI_SUCCESS) {
        printf("Color matrix get failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_saturation_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_saturation_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_saturation_attr.op_type), name->valuestring);
    } else {
        printf("set isp_saturation_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_saturation_attr.manual_attr.saturation");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_saturation_attr.manual_attr.saturation), name->valuestring);
    } else {
        printf("set isp_saturation_attr.manual_attr.saturation failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_saturation_attr.auto_attr.sat");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u8(isp_saturation_attr.auto_attr.sat, name->valuestring, OT_ISP_AUTO_ISO_NUM);
    } else {
        printf("set isp_saturation_attr.auto_attr.sat failed\n");
    }

    // isp_color_matrix_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_color_matrix_attr.op_type), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.manual_attr.sat_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_color_matrix_attr.manual_attr.sat_en), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.manual_attr.sat_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.manual_attr.ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.manual_attr.ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.manual_attr.ccm failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.iso_act_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_color_matrix_attr.auto_attr.iso_act_en), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.iso_act_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.temp_act_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_color_matrix_attr.auto_attr.temp_act_en), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.temp_act_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab_num), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab_num failed\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[0] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[0].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[0].color_temp), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[0].color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[0].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[0].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[0].ccm failed\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[1] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[1].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[1].color_temp), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[1].color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[1].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[1].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[1].ccm failed\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[2] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[2].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[2].color_temp), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[2].color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[2].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[2].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[2].ccm failed\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[3] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[3].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[3].color_temp), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[3].color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[3].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[3].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[3].ccm failed\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[4] ~ isp_color_matrix_attr.auto_attr.ccm_tab[6] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[4].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[4].color_temp), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[4].color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[4].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[4].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[4].ccm failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[5].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[5].color_temp), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[5].color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[5].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[5].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[5].ccm failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[6].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[6].color_temp), name->valuestring);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[6].color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[6].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[6].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_color_matrix_attr.auto_attr.ccm_tab[6].ccm failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_saturation_attr(vi_pipe, &isp_saturation_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp saturation attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_ccm_attr(vi_pipe, &isp_color_matrix_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp ccm attr failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp saturation/color_matrix attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_ccm_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // check isp saturation/color_matrix attr
    //printf("check isp saturation/color_matrix attr start\n");

    ot_isp_saturation_attr isp_saturation_attr = {0};
    ot_isp_color_matrix_attr isp_color_matrix_attr = {0};
    ret = hi_mpi_isp_get_saturation_attr(vi_pipe, &isp_saturation_attr);
    if (ret != HI_SUCCESS) {
        printf("Saturation get failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_ccm_attr(vi_pipe, &isp_color_matrix_attr);
    if (ret != HI_SUCCESS) {
        printf("Color matrix get failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_saturation_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_saturation_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_saturation_attr.op_type), name->valuestring, "isp_saturation_attr.op_type");
    } else {
        printf("no attr isp_saturation_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_saturation_attr.manual_attr.saturation");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_saturation_attr.manual_attr.saturation), name->valuestring, "isp_saturation_attr.manual_attr.saturation");
    } else {
        printf("no attr isp_saturation_attr.manual_attr.saturation\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_saturation_attr.auto_attr.sat");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u8(isp_saturation_attr.auto_attr.sat, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_saturation_attr.auto_attr.sat");
    } else {
        printf("no attr isp_saturation_attr.auto_attr.sat\n");
    }

    // isp_color_matrix_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_color_matrix_attr.op_type), name->valuestring, "isp_color_matrix_attr.op_type");
    } else {
        printf("no attr isp_color_matrix_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.manual_attr.sat_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_color_matrix_attr.manual_attr.sat_en), name->valuestring, "isp_color_matrix_attr.manual_attr.sat_en");
    } else {
        printf("no attr isp_color_matrix_attr.manual_attr.sat_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.manual_attr.ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.manual_attr.ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.manual_attr.ccm");
    } else {
        printf("no attr isp_color_matrix_attr.manual_attr.ccm\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.iso_act_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_color_matrix_attr.auto_attr.iso_act_en), name->valuestring, "isp_color_matrix_attr.auto_attr.iso_act_en");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.iso_act_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.temp_act_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_color_matrix_attr.auto_attr.temp_act_en), name->valuestring, "isp_color_matrix_attr.auto_attr.temp_act_en");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.temp_act_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab_num), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab_num");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab_num\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[0] check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[0].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[0].color_temp), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab[0].color_temp");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[0].color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[0].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[0].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.auto_attr.ccm_tab[0].ccm");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[0].ccm\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[1] check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[1].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[1].color_temp), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab[1].color_temp");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[1].color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[1].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[1].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.auto_attr.ccm_tab[1].ccm");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[1].ccm\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[2] check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[2].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[2].color_temp), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab[2].color_temp");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[2].color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[2].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[2].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.auto_attr.ccm_tab[2].ccm");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[2].ccm\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[3] check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[3].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[3].color_temp), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab[3].color_temp");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[3].color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[3].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[3].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.auto_attr.ccm_tab[3].ccm");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[3].ccm\n");
    }

    // isp_color_matrix_attr.auto_attr.ccm_tab[4] ~ isp_color_matrix_attr.auto_attr.ccm_tab[6] check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[4].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[4].color_temp), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab[4].color_temp");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[4].color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[4].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[4].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.auto_attr.ccm_tab[4].ccm");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[4].ccm\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[5].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[5].color_temp), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab[5].color_temp");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[5].color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[5].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[5].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.auto_attr.ccm_tab[5].ccm");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[5].ccm\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[6].color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_matrix_attr.auto_attr.ccm_tab[6].color_temp), name->valuestring, "isp_color_matrix_attr.auto_attr.ccm_tab[6].color_temp");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[6].color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_matrix_attr.auto_attr.ccm_tab[6].ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_color_matrix_attr.auto_attr.ccm_tab[6].ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_color_matrix_attr.auto_attr.ccm_tab[6].ccm");
    } else {
        printf("no attr isp_color_matrix_attr.auto_attr.ccm_tab[6].ccm\n");
    }

    //printf("check isp saturation/color_matrix attr success\n");
    return HI_SUCCESS;
}


//设置黑电平属性
hi_s32 set_isp_black_level_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // set black level attr
    //printf("set isp black level attr start\n");

    ot_isp_black_level_attr isp_black_level_attr = {0};
    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &isp_black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("Black level get failed with 0x%x!\n", ret);
        return ret;
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.user_black_level_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_black_level_attr.user_black_level_en), name->valuestring);
    } else {
        printf("set isp_black_level_attr.user_black_level_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.user_black_level");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_2d_array_value(isp_black_level_attr.user_black_level, name->valuestring, OT_ISP_WDR_MAX_FRAME_NUM, OT_ISP_BAYER_CHN_NUM, "u16");
    } else {
        printf("set isp_black_level_attr.user_black_level failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.manual_attr.black_level");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_2d_array_value(isp_black_level_attr.manual_attr.black_level, name->valuestring, OT_ISP_WDR_MAX_FRAME_NUM, OT_ISP_BAYER_CHN_NUM, "u16");
    } else {
        printf("set isp_black_level_attr.manual_attr.black_level failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.black_level_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_black_level_mode, &(isp_black_level_attr.black_level_mode), name->valuestring);
    } else {
        printf("set isp_black_level_attr.black_level_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.pattern");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_black_level_dynamic_pattern, &(isp_black_level_attr.dynamic_attr.pattern), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.pattern failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.x");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_s32, &(isp_black_level_attr.dynamic_attr.ob_area.x), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.ob_area.x failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.y");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_s32, &(isp_black_level_attr.dynamic_attr.ob_area.y), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.ob_area.y failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.width");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_black_level_attr.dynamic_attr.ob_area.width), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.ob_area.width failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.height");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_black_level_attr.dynamic_attr.ob_area.height), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.ob_area.height failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.low_threshold), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.low_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.high_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.high_threshold), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.high_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.offset");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_s16(isp_black_level_attr.dynamic_attr.offset, name->valuestring, OT_ISP_AUTO_ISO_NUM);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.offset failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.tolerance), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.tolerance failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.filter_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_black_level_attr.dynamic_attr.filter_strength), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.filter_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.separate_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_black_level_attr.dynamic_attr.separate_en), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.separate_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.calibration_black_level");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_black_level_attr.dynamic_attr.calibration_black_level, name->valuestring, OT_ISP_AUTO_ISO_NUM);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.calibration_black_level failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.filter_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.filter_thr), name->valuestring);
    } else {
        printf("set isp_black_level_attr.dynamic_attr.filter_thr failed\n");
    }

    ret = hi_mpi_isp_set_black_level_attr(vi_pipe, &isp_black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("Black level set failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp black level attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_black_level_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // check black level attr
    //printf("check isp black level attr start\n");

    ot_isp_black_level_attr isp_black_level_attr = {0};
    ret = hi_mpi_isp_get_black_level_attr(vi_pipe, &isp_black_level_attr);
    if (ret != HI_SUCCESS) {
        printf("Black level get failed with 0x%x!\n", ret);
        return ret;
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.user_black_level_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_black_level_attr.user_black_level_en), name->valuestring, "isp_black_level_attr.user_black_level_en");
    } else {
        printf("no attr isp_black_level_attr.user_black_level_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.user_black_level");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_2d_array_value(isp_black_level_attr.user_black_level, name->valuestring, OT_ISP_WDR_MAX_FRAME_NUM, OT_ISP_BAYER_CHN_NUM, "u16", "isp_black_level_attr.user_black_level");
    } else {
        printf("no attr isp_black_level_attr.user_black_level\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.manual_attr.black_level");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_2d_array_value(isp_black_level_attr.manual_attr.black_level, name->valuestring, OT_ISP_WDR_MAX_FRAME_NUM, OT_ISP_BAYER_CHN_NUM, "u16", "isp_black_level_attr.manual_attr.black_level");
    } else {
        printf("no attr isp_black_level_attr.manual_attr.black_level\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.black_level_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_black_level_mode, &(isp_black_level_attr.black_level_mode), name->valuestring, "isp_black_level_attr.black_level_mode");
    } else {
        printf("no attr isp_black_level_attr.black_level_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.pattern");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_black_level_dynamic_pattern, &(isp_black_level_attr.dynamic_attr.pattern), name->valuestring, "isp_black_level_attr.dynamic_attr.pattern");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.pattern\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.x");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s32, &(isp_black_level_attr.dynamic_attr.ob_area.x), name->valuestring, "isp_black_level_attr.dynamic_attr.ob_area.x");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.ob_area.x\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.y");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s32, &(isp_black_level_attr.dynamic_attr.ob_area.y), name->valuestring, "isp_black_level_attr.dynamic_attr.ob_area.y");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.ob_area.y\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.width");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_black_level_attr.dynamic_attr.ob_area.width), name->valuestring, "isp_black_level_attr.dynamic_attr.ob_area.width");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.ob_area.width\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.ob_area.height");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_black_level_attr.dynamic_attr.ob_area.height), name->valuestring, "isp_black_level_attr.dynamic_attr.ob_area.height");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.ob_area.height\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.low_threshold), name->valuestring, "isp_black_level_attr.dynamic_attr.low_threshold");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.low_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.high_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.high_threshold), name->valuestring, "isp_black_level_attr.dynamic_attr.high_threshold");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.high_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.offset");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_s16(isp_black_level_attr.dynamic_attr.offset, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_black_level_attr.dynamic_attr.offset");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.offset\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.tolerance");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.tolerance), name->valuestring, "isp_black_level_attr.dynamic_attr.tolerance");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.tolerance\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.filter_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_black_level_attr.dynamic_attr.filter_strength), name->valuestring, "isp_black_level_attr.dynamic_attr.filter_strength");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.filter_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.separate_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_black_level_attr.dynamic_attr.separate_en), name->valuestring, "isp_black_level_attr.dynamic_attr.separate_en");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.separate_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.calibration_black_level");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_black_level_attr.dynamic_attr.calibration_black_level, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_black_level_attr.dynamic_attr.calibration_black_level");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.calibration_black_level\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_black_level_attr.dynamic_attr.filter_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_black_level_attr.dynamic_attr.filter_thr), name->valuestring, "isp_black_level_attr.dynamic_attr.filter_thr");
    } else {
        printf("no attr isp_black_level_attr.dynamic_attr.filter_thr\n");
    }

    //printf("check isp black level attr success\n");
    return HI_SUCCESS;
}

//设置图像锐化属性
hi_s32 set_isp_sharpen_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // set isp sharpen attr
    //printf("set isp sharpen attr start\n");
    ot_isp_sharpen_attr isp_sharpen_attr = {0};
    ret = hi_mpi_isp_get_sharpen_attr(vi_pipe, &isp_sharpen_attr);
    if (ret != HI_SUCCESS) {
        printf("Sharpen get failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_sharpen_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_sharpen_attr.enable), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_sharpen_attr.motion_en), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.motion_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_threshold0");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.motion_threshold0), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.motion_threshold0 failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_threshold1");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.motion_threshold1), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.motion_threshold1 failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_gain0");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_sharpen_attr.motion_gain0), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.motion_gain0 failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_gain1");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_sharpen_attr.motion_gain1), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.motion_gain1 failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_umin");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.skin_umin), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.skin_umin failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_vmin");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.skin_vmin), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.skin_vmin failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_umax");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.skin_umax), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.skin_umax failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_vmax");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.skin_vmax), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.skin_vmax failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_sharpen_attr.op_type), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.detail_map");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_sharpen_detail_map, &(isp_sharpen_attr.detail_map), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.detail_map failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.luma_wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u8(isp_sharpen_attr.manual_attr.luma_wgt, name->valuestring, OT_ISP_SHARPEN_LUMA_NUM);
    } else {
        printf("set isp_sharpen_attr.manual_attr.luma_wgt failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.texture_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_sharpen_attr.manual_attr.texture_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM);
    } else {
        printf("set isp_sharpen_attr.manual_attr.texture_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_sharpen_attr.manual_attr.edge_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM);
    } else {
        printf("set isp_sharpen_attr.manual_attr.edge_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.texture_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.texture_freq), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.texture_freq failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.edge_freq), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.edge_freq failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.over_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.over_shoot), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.over_shoot failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.under_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.under_shoot), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.under_shoot failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_texture_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_sharpen_attr.manual_attr.motion_texture_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM);
    } else {
        printf("set isp_sharpen_attr.manual_attr.motion_texture_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_edge_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_sharpen_attr.manual_attr.motion_edge_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM);
    } else {
        printf("set isp_sharpen_attr.manual_attr.motion_edge_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_texture_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.motion_texture_freq), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.motion_texture_freq failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_edge_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.motion_edge_freq), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.motion_edge_freq failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_over_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.motion_over_shoot), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.motion_over_shoot failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_under_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.motion_under_shoot), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.motion_under_shoot failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.shoot_sup_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.shoot_sup_strength), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.shoot_sup_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.shoot_sup_adj");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.shoot_sup_adj), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.shoot_sup_adj failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.detail_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.detail_ctrl), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.detail_ctrl failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.detail_ctrl_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.detail_ctrl_threshold), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.detail_ctrl_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_filt_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.edge_filt_strength), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.edge_filt_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_filt_max_cap");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.edge_filt_max_cap), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.edge_filt_max_cap failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.r_gain), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.g_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.g_gain), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.g_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.b_gain), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.b_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.skin_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.skin_gain), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.skin_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.max_sharp_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.max_sharp_gain), name->valuestring);
    } else {
        printf("set isp_sharpen_attr.manual_attr.max_sharp_gain failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_sharpen_attr(vi_pipe, &isp_sharpen_attr);
    if (ret != HI_SUCCESS) {
        printf("Sharpen set failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp sharpen attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_sharpen_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    // check isp sharpen attr
    //printf("check isp sharpen attr start\n");
    ot_isp_sharpen_attr isp_sharpen_attr = {0};
    ret = hi_mpi_isp_get_sharpen_attr(vi_pipe, &isp_sharpen_attr);
    if (ret != HI_SUCCESS) {
        printf("Sharpen get failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_sharpen_attr 检查值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_sharpen_attr.enable), name->valuestring, "isp_sharpen_attr.enable");
    } else {
        printf("no attr isp_sharpen_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_sharpen_attr.motion_en), name->valuestring, "isp_sharpen_attr.motion_en");
    } else {
        printf("no attr isp_sharpen_attr.motion_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_threshold0");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.motion_threshold0), name->valuestring, "isp_sharpen_attr.motion_threshold0");
    } else {
        printf("no attr isp_sharpen_attr.motion_threshold0\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_threshold1");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.motion_threshold1), name->valuestring, "isp_sharpen_attr.motion_threshold1");
    } else {
        printf("no attr isp_sharpen_attr.motion_threshold1\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_gain0");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_sharpen_attr.motion_gain0), name->valuestring, "isp_sharpen_attr.motion_gain0");
    } else {
        printf("no attr isp_sharpen_attr.motion_gain0\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.motion_gain1");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_sharpen_attr.motion_gain1), name->valuestring, "isp_sharpen_attr.motion_gain1");
    } else {
        printf("no attr isp_sharpen_attr.motion_gain1\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_umin");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.skin_umin), name->valuestring, "isp_sharpen_attr.skin_umin");
    } else {
        printf("no attr isp_sharpen_attr.skin_umin\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_vmin");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.skin_vmin), name->valuestring, "isp_sharpen_attr.skin_vmin");
    } else {
        printf("no attr isp_sharpen_attr.skin_vmin\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_umax");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.skin_umax), name->valuestring, "isp_sharpen_attr.skin_umax");
    } else {
        printf("no attr isp_sharpen_attr.skin_umax\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.skin_vmax");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.skin_vmax), name->valuestring, "isp_sharpen_attr.skin_vmax");
    } else {
        printf("no attr isp_sharpen_attr.skin_vmax\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_sharpen_attr.op_type), name->valuestring, "isp_sharpen_attr.op_type");
    } else {
        printf("no attr isp_sharpen_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.detail_map");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_sharpen_detail_map, &(isp_sharpen_attr.detail_map), name->valuestring, "isp_sharpen_attr.detail_map");
    } else {
        printf("no attr isp_sharpen_attr.detail_map\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.luma_wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u8(isp_sharpen_attr.manual_attr.luma_wgt, name->valuestring, OT_ISP_SHARPEN_LUMA_NUM, "isp_sharpen_attr.manual_attr.luma_wgt");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.luma_wgt\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.texture_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_sharpen_attr.manual_attr.texture_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM, "isp_sharpen_attr.manual_attr.texture_strength");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.texture_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_sharpen_attr.manual_attr.edge_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM, "isp_sharpen_attr.manual_attr.edge_strength");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.edge_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.texture_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.texture_freq), name->valuestring, "isp_sharpen_attr.manual_attr.texture_freq");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.texture_freq\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.edge_freq), name->valuestring, "isp_sharpen_attr.manual_attr.edge_freq");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.edge_freq\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.over_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.over_shoot), name->valuestring, "isp_sharpen_attr.manual_attr.over_shoot");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.over_shoot\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.under_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.under_shoot), name->valuestring, "isp_sharpen_attr.manual_attr.under_shoot");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.under_shoot\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_texture_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_sharpen_attr.manual_attr.motion_texture_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM, "isp_sharpen_attr.manual_attr.motion_texture_strength");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.motion_texture_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_edge_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_sharpen_attr.manual_attr.motion_edge_strength, name->valuestring, OT_ISP_SHARPEN_GAIN_NUM, "isp_sharpen_attr.manual_attr.motion_edge_strength");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.motion_edge_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_texture_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.motion_texture_freq), name->valuestring, "isp_sharpen_attr.manual_attr.motion_texture_freq");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.motion_texture_freq\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_edge_freq");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.motion_edge_freq), name->valuestring, "isp_sharpen_attr.manual_attr.motion_edge_freq");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.motion_edge_freq\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_over_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.motion_over_shoot), name->valuestring, "isp_sharpen_attr.manual_attr.motion_over_shoot");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.motion_over_shoot\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.motion_under_shoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.motion_under_shoot), name->valuestring, "isp_sharpen_attr.manual_attr.motion_under_shoot");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.motion_under_shoot\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.shoot_sup_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.shoot_sup_strength), name->valuestring, "isp_sharpen_attr.manual_attr.shoot_sup_strength");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.shoot_sup_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.shoot_sup_adj");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.shoot_sup_adj), name->valuestring, "isp_sharpen_attr.manual_attr.shoot_sup_adj");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.shoot_sup_adj\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.detail_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.detail_ctrl), name->valuestring, "isp_sharpen_attr.manual_attr.detail_ctrl");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.detail_ctrl\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.detail_ctrl_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.detail_ctrl_threshold), name->valuestring, "isp_sharpen_attr.manual_attr.detail_ctrl_threshold");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.detail_ctrl_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_filt_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.edge_filt_strength), name->valuestring, "isp_sharpen_attr.manual_attr.edge_filt_strength");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.edge_filt_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.edge_filt_max_cap");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.edge_filt_max_cap), name->valuestring, "isp_sharpen_attr.manual_attr.edge_filt_max_cap");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.edge_filt_max_cap\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.r_gain), name->valuestring, "isp_sharpen_attr.manual_attr.r_gain");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.g_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.g_gain), name->valuestring, "isp_sharpen_attr.manual_attr.g_gain");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.g_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.b_gain), name->valuestring, "isp_sharpen_attr.manual_attr.b_gain");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.b_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.skin_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_sharpen_attr.manual_attr.skin_gain), name->valuestring, "isp_sharpen_attr.manual_attr.skin_gain");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.skin_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_sharpen_attr.manual_attr.max_sharp_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_sharpen_attr.manual_attr.max_sharp_gain), name->valuestring, "isp_sharpen_attr.manual_attr.max_sharp_gain");
    } else {
        printf("no attr isp_sharpen_attr.manual_attr.max_sharp_gain\n");
    }

    //printf("check write isp sharpen attr success\n");
    return HI_SUCCESS;
}

//设置伽马变换属性
hi_s32 set_isp_gamma_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set gamma attr
    //printf("set isp gamma attr start\n");

    ot_isp_gamma_attr isp_gamma_attr = {0};
    ret = hi_mpi_isp_get_gamma_attr(vi_pipe, &isp_gamma_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp gamma attr failed with 0x%x!\n", ret);
        return ret;
    }
    // isp_gamma_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_gamma_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_gamma_attr.enable), name->valuestring);
    } else {
        printf("set isp_gamma_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_gamma_attr.table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_gamma_attr.table, name->valuestring, OT_ISP_GAMMA_NODE_NUM);
    } else {
        printf("set isp_gamma_attr.table failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_gamma_attr.curve_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_gamma_curve_type, &(isp_gamma_attr.curve_type), name->valuestring);
    } else {
        printf("set isp_gamma_attr.curve_type failed\n");
    }

    //设置isp参数
    ret = hi_mpi_isp_set_gamma_attr(vi_pipe, &isp_gamma_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp gamma attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp gamma attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_gamma_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set gamma attr
    //printf("check isp gamma attr start\n");

    ot_isp_gamma_attr isp_gamma_attr = {0};
    ret = hi_mpi_isp_get_gamma_attr(vi_pipe, &isp_gamma_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp gamma attr failed with 0x%x!\n", ret);
        return ret;
    }
    // isp_gamma_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_gamma_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_gamma_attr.enable), name->valuestring, "isp_gamma_attr.enable");
    } else {
        printf("no attr isp_gamma_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_gamma_attr.table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_gamma_attr.table, name->valuestring, OT_ISP_GAMMA_NODE_NUM, "isp_gamma_attr.table");
    } else {
        printf("no attr isp_gamma_attr.table\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_gamma_attr.curve_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_gamma_curve_type, &(isp_gamma_attr.curve_type), name->valuestring, "isp_gamma_attr.curve_type");
    } else {
        printf("no attr isp_gamma_attr.curve_type\n");
    }
    //printf("check write isp gamma attr success\n");
    return HI_SUCCESS;
}


//设置wdr动态范围压缩属性
hi_s32 set_isp_drc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp drc attr
    //printf("set isp drc attr start\n");

    ot_isp_drc_attr isp_drc_attr = {0};

    ret = hi_mpi_isp_get_drc_attr(vi_pipe, &isp_drc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp drc attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_drc_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_drc_attr.enable), name->valuestring);
    } else {
        printf("set isp_drc_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.curve_select");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_isp_drc_curve_select, &(isp_drc_attr.curve_select), name->valuestring);
    } else {
        printf("set isp_drc_attr.curve_select failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.purple_reduction_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.purple_reduction_strength), name->valuestring);
    } else {
        printf("set isp_drc_attr.purple_reduction_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.bright_gain_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.bright_gain_limit), name->valuestring);
    } else {
        printf("set isp_drc_attr.bright_gain_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.bright_gain_limit_step");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.bright_gain_limit_step), name->valuestring);
    } else {
        printf("set isp_drc_attr.bright_gain_limit_step failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.dark_gain_limit_luma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.dark_gain_limit_luma), name->valuestring);
    } else {
        printf("set isp_drc_attr.dark_gain_limit_luma failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.dark_gain_limit_chroma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.dark_gain_limit_chroma), name->valuestring);
    } else {
        printf("set isp_drc_attr.dark_gain_limit_chroma failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.contrast_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.contrast_ctrl), name->valuestring);
    } else {
        printf("set isp_drc_attr.contrast_ctrl failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.color_correction_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_drc_attr.color_correction_lut, name->valuestring, OT_ISP_DRC_CC_NODE_NUM);
    } else {
        printf("set isp_drc_attr.color_correction_lut failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.tone_mapping_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_drc_attr.tone_mapping_value, name->valuestring, OT_ISP_DRC_TM_NODE_NUM);
    } else {
        printf("set isp_drc_attr.tone_mapping_value failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.high_saturation_color_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.high_saturation_color_ctrl), name->valuestring);
    } else {
        printf("set isp_drc_attr.high_saturation_color_ctrl failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.global_color_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.global_color_ctrl), name->valuestring);
    } else {
        printf("set isp_drc_attr.global_color_ctrl failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.shoot_reduction_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_drc_attr.shoot_reduction_en), name->valuestring);
    } else {
        printf("set isp_drc_attr.shoot_reduction_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_op_mode, &(isp_drc_attr.op_type), name->valuestring);
    } else {
        printf("set isp_drc_attr.op_type failed\n");
    }

    // isp_drc_attr.manual_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_drc_attr.manual_attr.strength), name->valuestring);
    } else {
        printf("set isp_drc_attr.manual_attr.strength failed\n");
    }

    // isp_drc_attr.auto_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_drc_attr.auto_attr.strength), name->valuestring);
    } else {
        printf("set isp_drc_attr.auto_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.auto_attr.strength_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_drc_attr.auto_attr.strength_max), name->valuestring);
    } else {
        printf("set isp_drc_attr.auto_attr.strength_max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.auto_attr.strength_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_drc_attr.auto_attr.strength_min), name->valuestring);
    } else {
        printf("set isp_drc_attr.auto_attr.strength_min failed\n");
    }

    // isp_drc_attr.asymmetry_curve 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.asymmetry");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.asymmetry), name->valuestring);
    } else {
        printf("set isp_drc_attr.asymmetry_curve.asymmetry failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.second_pole");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.second_pole), name->valuestring);
    } else {
        printf("set isp_drc_attr.asymmetry_curve.second_pole failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.stretch");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.stretch), name->valuestring);
    } else {
        printf("set isp_drc_attr.asymmetry_curve.stretch failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.compress");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.compress), name->valuestring);
    } else {
        printf("set isp_drc_attr.asymmetry_curve.compress failed\n");
    }

    //设置isp参数
    ret = hi_mpi_isp_set_drc_attr(vi_pipe, &isp_drc_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp drc attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp drc attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_drc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //check isp drc attr
    //printf("check isp drc attr start\n");

    ot_isp_drc_attr isp_drc_attr = {0};

    ret = hi_mpi_isp_get_drc_attr(vi_pipe, &isp_drc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp drc attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_drc_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_drc_attr.enable), name->valuestring, "isp_drc_attr.enable");
    } else {
        printf("no attr isp_drc_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.curve_select");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_drc_curve_select, &(isp_drc_attr.curve_select), name->valuestring, "isp_drc_attr.curve_select");
    } else {
        printf("no attr isp_drc_attr.curve_select\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.purple_reduction_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.purple_reduction_strength), name->valuestring, "isp_drc_attr.purple_reduction_strength");
    } else {
        printf("no attr isp_drc_attr.purple_reduction_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.bright_gain_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.bright_gain_limit), name->valuestring, "isp_drc_attr.bright_gain_limit");
    } else {
        printf("no attr isp_drc_attr.bright_gain_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.bright_gain_limit_step");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.bright_gain_limit_step), name->valuestring, "isp_drc_attr.bright_gain_limit_step");
    } else {
        printf("no attr isp_drc_attr.bright_gain_limit_step\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.dark_gain_limit_luma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.dark_gain_limit_luma), name->valuestring, "isp_drc_attr.dark_gain_limit_luma");
    } else {
        printf("no attr isp_drc_attr.dark_gain_limit_luma\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.dark_gain_limit_chroma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.dark_gain_limit_chroma), name->valuestring, "isp_drc_attr.dark_gain_limit_chroma");
    } else {
        printf("no attr isp_drc_attr.dark_gain_limit_chroma\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.contrast_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.contrast_ctrl), name->valuestring, "isp_drc_attr.contrast_ctrl");
    } else {
        printf("no attr isp_drc_attr.contrast_ctrl\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.color_correction_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_drc_attr.color_correction_lut, name->valuestring, OT_ISP_DRC_CC_NODE_NUM, "isp_drc_attr.color_correction_lut");
    } else {
        printf("no attr isp_drc_attr.color_correction_lut\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.tone_mapping_value");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_drc_attr.tone_mapping_value, name->valuestring, OT_ISP_DRC_TM_NODE_NUM, "isp_drc_attr.tone_mapping_value");
    } else {
        printf("no attr isp_drc_attr.tone_mapping_value\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.high_saturation_color_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.high_saturation_color_ctrl), name->valuestring, "isp_drc_attr.high_saturation_color_ctrl");
    } else {
        printf("no attr isp_drc_attr.high_saturation_color_ctrl\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.global_color_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.global_color_ctrl), name->valuestring, "isp_drc_attr.global_color_ctrl");
    } else {
        printf("no attr isp_drc_attr.global_color_ctrl\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.shoot_reduction_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_drc_attr.shoot_reduction_en), name->valuestring, "isp_drc_attr.shoot_reduction_en");
    } else {
        printf("no attr isp_drc_attr.shoot_reduction_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_drc_attr.op_type), name->valuestring, "isp_drc_attr.op_type");
    } else {
        printf("no attr isp_drc_attr.op_type\n");
    }

    // isp_drc_attr.manual_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_drc_attr.manual_attr.strength), name->valuestring, "isp_drc_attr.manual_attr.strength");
    } else {
        printf("no attr isp_drc_attr.manual_attr.strength\n");
    }

    // isp_drc_attr.auto_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_drc_attr.auto_attr.strength), name->valuestring, "isp_drc_attr.auto_attr.strength");
    } else {
        printf("no attr isp_drc_attr.auto_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.auto_attr.strength_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_drc_attr.auto_attr.strength_max), name->valuestring, "isp_drc_attr.auto_attr.strength_max");
    } else {
        printf("no attr isp_drc_attr.auto_attr.strength_max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.auto_attr.strength_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_drc_attr.auto_attr.strength_min), name->valuestring, "isp_drc_attr.auto_attr.strength_min");
    } else {
        printf("no attr isp_drc_attr.auto_attr.strength_min\n");
    }

    // isp_drc_attr.asymmetry_curve check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.asymmetry");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.asymmetry), name->valuestring, "isp_drc_attr.asymmetry_curve.asymmetry");
    } else {
        printf("no attr isp_drc_attr.asymmetry_curve.asymmetry\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.second_pole");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.second_pole), name->valuestring, "isp_drc_attr.asymmetry_curve.second_pole");
    } else {
        printf("no attr isp_drc_attr.asymmetry_curve.second_pole\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.stretch");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.stretch), name->valuestring, "isp_drc_attr.asymmetry_curve.stretch");
    } else {
        printf("no attr isp_drc_attr.asymmetry_curve.stretch\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_drc_attr.asymmetry_curve.compress");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_drc_attr.asymmetry_curve.compress), name->valuestring, "isp_drc_attr.asymmetry_curve.compress");
    } else {
        printf("no attr isp_drc_attr.asymmetry_curve.compress\n");
    }
    //printf("check isp drc attr success!\n");
    return HI_SUCCESS;
}


//设置Mesh Shading属性
hi_s32 set_isp_shading_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp shading attr
    //printf("set isp shading attr start\n");

    ot_isp_shading_attr isp_shading_attr = {0};
    ot_isp_shading_lut_attr isp_shading_lut_attr = {0};

    ret = hi_mpi_isp_get_mesh_shading_attr(vi_pipe, &isp_shading_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp shading attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_mesh_shading_gain_lut_attr(vi_pipe, &isp_shading_lut_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp shading lut attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_shading_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_shading_attr.enable), name->valuestring);
    } else {
        printf("set isp_shading_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_attr.mesh_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_shading_attr.mesh_strength), name->valuestring);
    } else {
        printf("set isp_shading_attr.mesh_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_shading_attr.blend_ratio), name->valuestring);
    } else {
        printf("set isp_shading_attr.blend_ratio failed\n");
    }

    // isp_shading_lut_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.mesh_scale");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_shading_lut_attr.mesh_scale), name->valuestring);
    } else {
        printf("set isp_shading_lut_attr.mesh_scale failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].r_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[0].r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].gr_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].gr_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[0].gr_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].gb_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].gb_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[0].gb_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].b_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[0].b_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].r_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[1].r_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].gr_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].gr_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[1].gr_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].gb_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].gb_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[1].gb_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].b_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS);
    } else {
        printf("set isp_shading_lut_attr.lsc_gain_lut[1].b_gain failed\n");
    }

    //设置isp参数
    ret = hi_mpi_isp_set_mesh_shading_attr(vi_pipe, &isp_shading_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp shading attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_mesh_shading_gain_lut_attr(vi_pipe, &isp_shading_lut_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp shading lut attr failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp shading attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_shading_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //check isp shading attr
    //printf("check isp shading attr start\n");

    ot_isp_shading_attr isp_shading_attr = {0};
    ot_isp_shading_lut_attr isp_shading_lut_attr = {0};

    ret = hi_mpi_isp_get_mesh_shading_attr(vi_pipe, &isp_shading_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp shading attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_mesh_shading_gain_lut_attr(vi_pipe, &isp_shading_lut_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp shading lut attr failed with %#x!\n", ret);
        return ret;
    }

    // check isp_shading_attr 
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_shading_attr.enable), name->valuestring, "isp_shading_attr.enable");
    } else {
        printf("no attr isp_shading_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_attr.mesh_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_shading_attr.mesh_strength), name->valuestring, "isp_shading_attr.mesh_strength");
    } else {
        printf("no attr isp_shading_attr.mesh_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_shading_attr.blend_ratio), name->valuestring, "isp_shading_attr.blend_ratio");
    } else {
        printf("no attr isp_shading_attr.blend_ratio\n");
    }

    // check isp_shading_lut_attr 
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.mesh_scale");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_shading_lut_attr.mesh_scale), name->valuestring, "isp_shading_lut_attr.mesh_scale");
    } else {
        printf("no attr isp_shading_lut_attr.mesh_scale\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].r_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[0].r_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[0].r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].gr_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].gr_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[0].gr_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[0].gr_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].gb_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].gb_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[0].gb_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[0].gb_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[0].b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[0].b_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[0].b_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[0].b_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].r_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].r_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[1].r_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[1].r_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].gr_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].gr_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[1].gr_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[1].gr_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].gb_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].gb_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[1].gb_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[1].gb_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_shading_lut_attr.lsc_gain_lut[1].b_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_shading_lut_attr.lsc_gain_lut[1].b_gain, name->valuestring, OT_ISP_LSC_GRID_POINTS, "isp_shading_lut_attr.lsc_gain_lut[1].b_gain");
    } else {
        printf("no attr isp_shading_lut_attr.lsc_gain_lut[1].b_gain\n");
    }

    
    //printf("check write isp shading attr success\n");
    return HI_SUCCESS;
}


//设置坏点矫正属性
hi_s32 set_isp_dp_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp dp attr start
    //printf("set isp dp attr start\n");

    ot_isp_dp_dynamic_attr isp_dp_dynamic_attr = {0};
    ot_isp_dp_static_calibrate isp_dp_static_calibrate = {0};
    ot_isp_dp_static_attr isp_dp_static_attr = {0};
    ret = hi_mpi_isp_get_dp_dynamic_attr(vi_pipe, &isp_dp_dynamic_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp dynamic attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dp_calibrate(vi_pipe, &isp_dp_static_calibrate);
    if (ret != HI_SUCCESS) {
        printf("get isp static calibrate failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dp_static_attr(vi_pipe, &isp_dp_static_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp static attr failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_dp_dynamic_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_dynamic_attr.enable), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.enable failed\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[0] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[0].sup_twinkle_en), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].sup_twinkle_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[0].soft_thr), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].soft_thr failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].soft_slope), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].soft_slope failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[0].op_type), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.blend_ratio), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].bright_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].bright_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].dark_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[0].dark_strength failed\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[1] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[1].sup_twinkle_en), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].sup_twinkle_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[1].soft_thr), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].soft_thr failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].soft_slope), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].soft_slope failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[1].op_type), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.blend_ratio), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].bright_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].bright_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].dark_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[1].dark_strength failed\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[2] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[2].sup_twinkle_en), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].sup_twinkle_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[2].soft_thr), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].soft_thr failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].soft_slope), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].soft_slope failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[2].op_type), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.blend_ratio), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].bright_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].bright_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].dark_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[2].dark_strength failed\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[3] 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[3].sup_twinkle_en), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].sup_twinkle_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[3].soft_thr), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].soft_thr failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].soft_slope), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].soft_slope failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[3].op_type), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.blend_ratio), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.blend_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].bright_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].bright_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].dark_strength), name->valuestring); 
    } else {
        printf("set isp_dp_dynamic_attr.frame_dynamic[3].dark_strength failed\n");
    }

    // isp_dp_static_calibrate 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.enable_detect");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_static_calibrate.enable_detect), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.enable_detect failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.static_dp_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_isp_static_dp_type, &(isp_dp_static_calibrate.static_dp_type), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.static_dp_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.start_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_static_calibrate.start_thresh), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.start_thresh failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.count_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dp_static_calibrate.count_max), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.count_max failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.count_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dp_static_calibrate.count_min), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.count_min failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.time_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dp_static_calibrate.time_limit), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.time_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u32(isp_dp_static_calibrate.table, name->valuestring, OT_ISP_STATIC_DP_COUNT_MAX); 
    } else {
        printf("set isp_dp_static_calibrate.table failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.finish_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dp_static_calibrate.finish_thresh), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.finish_thresh failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dp_static_calibrate.count), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.count failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_isp_status, &(isp_dp_static_calibrate.status), name->valuestring); 
    } else {
        printf("set isp_dp_static_calibrate.status failed\n");
    }

    // isp_dp_static_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_static_attr.enable), name->valuestring); 
    } else {
        printf("set isp_dp_static_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.bright_count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dp_static_attr.bright_count), name->valuestring); 
    } else {
        printf("set isp_dp_static_attr.bright_count failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.dark_count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dp_static_attr.dark_count), name->valuestring); 
    } else {
        printf("set isp_dp_static_attr.dark_count failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.bright_table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u32(isp_dp_static_attr.bright_table, name->valuestring, OT_ISP_STATIC_DP_COUNT_MAX); 
    } else {
        printf("set isp_dp_static_attr.bright_table failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.dark_table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u32(isp_dp_static_attr.dark_table, name->valuestring, OT_ISP_STATIC_DP_COUNT_MAX); 
    } else {
        printf("set isp_dp_static_attr.dark_table failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.show");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dp_static_attr.show), name->valuestring); 
    } else {
        printf("set isp_dp_static_attr.show failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_dp_dynamic_attr(vi_pipe, &isp_dp_dynamic_attr);
    if (ret != HI_SUCCESS) {
        printf("isp set dynamic attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret= hi_mpi_isp_set_dp_calibrate(vi_pipe, &isp_dp_static_calibrate);
    if (ret != HI_SUCCESS) {
        printf("isp set static calibrate failed with 0x%x!\n", ret);
        return ret;
    }
    ret= hi_mpi_isp_set_dp_static_attr(vi_pipe, &isp_dp_static_attr);
    if (ret != HI_SUCCESS) {
        printf("isp set static attr failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp dp attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_dp_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //check isp dp attr start
    //printf("check isp dp attr start\n");

    ot_isp_dp_dynamic_attr isp_dp_dynamic_attr = {0};
    ot_isp_dp_static_calibrate isp_dp_static_calibrate = {0};
    ot_isp_dp_static_attr isp_dp_static_attr = {0};
    ret = hi_mpi_isp_get_dp_dynamic_attr(vi_pipe, &isp_dp_dynamic_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp dynamic attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dp_calibrate(vi_pipe, &isp_dp_static_calibrate);
    if (ret != HI_SUCCESS) {
        printf("get isp static calibrate failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dp_static_attr(vi_pipe, &isp_dp_static_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp static attr failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_dp_dynamic_attr 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_dynamic_attr.enable), name->valuestring, "isp_dp_dynamic_attr.enable"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.enable\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[0] 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[0].sup_twinkle_en), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].sup_twinkle_en"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].sup_twinkle_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[0].soft_thr), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].soft_thr"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].soft_thr\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].soft_slope), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].soft_slope"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].soft_slope\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[0].op_type), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].op_type"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.blend_ratio), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].manual_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].auto_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].bright_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].bright_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].bright_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[0].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[0].dark_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[0].dark_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[0].dark_strength\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[1] 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[1].sup_twinkle_en), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].sup_twinkle_en"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].sup_twinkle_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[1].soft_thr), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].soft_thr"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].soft_thr\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].soft_slope), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].soft_slope"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].soft_slope\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[1].op_type), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].op_type"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.blend_ratio), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].manual_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].auto_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].bright_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].bright_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].bright_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[1].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[1].dark_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[1].dark_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[1].dark_strength\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[2] 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[2].sup_twinkle_en), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].sup_twinkle_en"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].sup_twinkle_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[2].soft_thr), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].soft_thr"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].soft_thr\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].soft_slope), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].soft_slope"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].soft_slope\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[2].op_type), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].op_type"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.blend_ratio), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].manual_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].auto_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].bright_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].bright_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].bright_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[2].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[2].dark_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[2].dark_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[2].dark_strength\n");
    }

    // isp_dp_dynamic_attr.frame_dynamic[3] 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].sup_twinkle_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_dynamic_attr.frame_dynamic[3].sup_twinkle_en), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].sup_twinkle_en"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].sup_twinkle_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].soft_thr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_s8, &(isp_dp_dynamic_attr.frame_dynamic[3].soft_thr), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].soft_thr"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].soft_thr\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].soft_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].soft_slope), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].soft_slope"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].soft_slope\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_dp_dynamic_attr.frame_dynamic[3].op_type), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].op_type"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.blend_ratio), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].manual_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.blend_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.blend_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.blend_ratio"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].auto_attr.blend_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].bright_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].bright_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].bright_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].bright_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_dynamic_attr.frame_dynamic[3].dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_dynamic_attr.frame_dynamic[3].dark_strength), name->valuestring, "isp_dp_dynamic_attr.frame_dynamic[3].dark_strength"); 
    } else {
        printf("no attr isp_dp_dynamic_attr.frame_dynamic[3].dark_strength\n");
    }

    // isp_dp_static_calibrate 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.enable_detect");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_static_calibrate.enable_detect), name->valuestring, "isp_dp_static_calibrate.enable_detect"); 
    } else {
        printf("no attr isp_dp_static_calibrate.enable_detect\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.static_dp_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_isp_static_dp_type, &(isp_dp_static_calibrate.static_dp_type), name->valuestring, "isp_dp_static_calibrate.static_dp_type"); 
    } else {
        printf("no attr isp_dp_static_calibrate.static_dp_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.start_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_static_calibrate.start_thresh), name->valuestring, "isp_dp_static_calibrate.start_thresh"); 
    } else {
        printf("no attr isp_dp_static_calibrate.start_thresh\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.count_max");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dp_static_calibrate.count_max), name->valuestring, "isp_dp_static_calibrate.count_max"); 
    } else {
        printf("no attr isp_dp_static_calibrate.count_max\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.count_min");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dp_static_calibrate.count_min), name->valuestring, "isp_dp_static_calibrate.count_min"); 
    } else {
        printf("no attr isp_dp_static_calibrate.count_min\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.time_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dp_static_calibrate.time_limit), name->valuestring, "isp_dp_static_calibrate.time_limit"); 
    } else {
        printf("no attr isp_dp_static_calibrate.time_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u32(isp_dp_static_calibrate.table, name->valuestring, OT_ISP_STATIC_DP_COUNT_MAX, "isp_dp_static_calibrate.table"); 
    } else {
        printf("no attr isp_dp_static_calibrate.table\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.finish_thresh");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dp_static_calibrate.finish_thresh), name->valuestring, "isp_dp_static_calibrate.finish_thresh"); 
    } else {
        printf("no attr isp_dp_static_calibrate.finish_thresh\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dp_static_calibrate.count), name->valuestring, "isp_dp_static_calibrate.count"); 
    } else {
        printf("no attr isp_dp_static_calibrate.count\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_calibrate.status");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_isp_status, &(isp_dp_static_calibrate.status), name->valuestring, "isp_dp_static_calibrate.status"); 
    } else {
        printf("no attr isp_dp_static_calibrate.status\n");
    }

    // isp_dp_static_attr 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_static_attr.enable), name->valuestring, "isp_dp_static_attr.enable"); 
    } else {
        printf("no attr isp_dp_static_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.bright_count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dp_static_attr.bright_count), name->valuestring, "isp_dp_static_attr.bright_count"); 
    } else {
        printf("no attr isp_dp_static_attr.bright_count\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.dark_count");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dp_static_attr.dark_count), name->valuestring, "isp_dp_static_attr.dark_count"); 
    } else {
        printf("no attr isp_dp_static_attr.dark_count\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.bright_table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u32(isp_dp_static_attr.bright_table, name->valuestring, OT_ISP_STATIC_DP_COUNT_MAX, "isp_dp_static_attr.bright_table"); 
    } else {
        printf("no attr isp_dp_static_attr.bright_table\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.dark_table");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u32(isp_dp_static_attr.dark_table, name->valuestring, OT_ISP_STATIC_DP_COUNT_MAX, "isp_dp_static_attr.dark_table"); 
    } else {
        printf("no attr isp_dp_static_attr.dark_table\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dp_static_attr.show");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dp_static_attr.show), name->valuestring, "isp_dp_static_attr.show"); 
    } else {
        printf("no attr isp_dp_static_attr.show\n");
    }

    //printf("check write isp dp attr success\n");
    return HI_SUCCESS;
}


//设置crosstalk和去雾属性
hi_s32 set_isp_cr_dehaze_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;    
    //set isp cr_attr/dehaze_attr
    //printf("set isp cr_attr/dehaze_attr start\n");
    ot_isp_cr_attr isp_cr_attr = {0};
    ot_isp_dehaze_attr isp_dehaze_attr = {0};

    ret = hi_mpi_isp_get_crosstalk_attr(vi_pipe, &isp_cr_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get cr attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dehaze_attr(vi_pipe, &isp_dehaze_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get dehaze attr failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_cr_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_cr_attr.enable), name->valuestring); 
    } else {
        printf("set isp_cr_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_cr_attr.slope), name->valuestring); 
    } else {
        printf("set isp_cr_attr.slope failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.sensi_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_cr_attr.sensi_slope), name->valuestring); 
    } else {
        printf("set isp_cr_attr.sensi_slope failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.sensi_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cr_attr.sensi_threshold), name->valuestring); 
    } else {
        printf("set isp_cr_attr.sensi_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cr_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cr_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cr_attr.threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cr_attr.threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.filter_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_cr_attr.filter_mode, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cr_attr.filter_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.np_offset");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cr_attr.np_offset, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cr_attr.np_offset failed\n");
    }

    // isp_dehaze_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dehaze_attr.enable), name->valuestring); 
    } else {
        printf("set isp_dehaze_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.user_lut_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_dehaze_attr.user_lut_en), name->valuestring); 
    } else {
        printf("set isp_dehaze_attr.user_lut_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.dehaze_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_dehaze_attr.dehaze_lut, name->valuestring, OT_ISP_DEHAZE_LUT_SIZE); 
    } else {
        printf("set isp_dehaze_attr.dehaze_lut failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_dehaze_attr.op_type), name->valuestring); 
    } else {
        printf("set isp_dehaze_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dehaze_attr.manual_attr.strength), name->valuestring); 
    } else {
        printf("set isp_dehaze_attr.manual_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_dehaze_attr.auto_attr.strength), name->valuestring); 
    } else {
        printf("set isp_dehaze_attr.auto_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.tmprflt_incr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dehaze_attr.tmprflt_incr_coef), name->valuestring); 
    } else {
        printf("set isp_dehaze_attr.tmprflt_incr_coef failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.tmprflt_decr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_dehaze_attr.tmprflt_decr_coef), name->valuestring); 
    } else {
        printf("set isp_dehaze_attr.tmprflt_decr_coef failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_crosstalk_attr(vi_pipe, &isp_cr_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp cr attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_dehaze_attr(vi_pipe, &isp_dehaze_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp dehaze attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp cr_attr/dehaze_attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_cr_dehaze_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;    
    //set isp cr_attr/dehaze_attr
    //printf("check isp cr_attr/dehaze_attr start\n");
    ot_isp_cr_attr isp_cr_attr = {0};
    ot_isp_dehaze_attr isp_dehaze_attr = {0};

    ret = hi_mpi_isp_get_crosstalk_attr(vi_pipe, &isp_cr_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get cr attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_dehaze_attr(vi_pipe, &isp_dehaze_attr);
    if (ret != HI_SUCCESS) {
        printf("isp get dehaze attr failed with 0x%x!\n", ret);
        return ret;
    }

    // isp_cr_attr 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_cr_attr.enable), name->valuestring, "isp_cr_attr.enable"); 
    } else {
        printf("no attr isp_cr_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_cr_attr.slope), name->valuestring, "isp_cr_attr.slope"); 
    } else {
        printf("no attr isp_cr_attr.slope\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.sensi_slope");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_cr_attr.sensi_slope), name->valuestring, "isp_cr_attr.sensi_slope"); 
    } else {
        printf("no attr isp_cr_attr.sensi_slope\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.sensi_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_cr_attr.sensi_threshold), name->valuestring, "isp_cr_attr.sensi_threshold"); 
    } else {
        printf("no attr isp_cr_attr.sensi_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_cr_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cr_attr.strength"); 
    } else {
        printf("no attr isp_cr_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_cr_attr.threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cr_attr.threshold"); 
    } else {
        printf("no attr isp_cr_attr.threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.filter_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_cr_attr.filter_mode, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cr_attr.filter_mode"); 
    } else {
        printf("no attr isp_cr_attr.filter_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cr_attr.np_offset");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_cr_attr.np_offset, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cr_attr.np_offset"); 
    } else {
        printf("no attr isp_cr_attr.np_offset\n");
    }

    // isp_dehaze_attr 检查
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dehaze_attr.enable), name->valuestring, "isp_dehaze_attr.enable"); 
    } else {
        printf("no attr isp_dehaze_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.user_lut_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_dehaze_attr.user_lut_en), name->valuestring, "isp_dehaze_attr.user_lut_en"); 
    } else {
        printf("no attr isp_dehaze_attr.user_lut_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.dehaze_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_dehaze_attr.dehaze_lut, name->valuestring, OT_ISP_DEHAZE_LUT_SIZE, "isp_dehaze_attr.dehaze_lut"); 
    } else {
        printf("no attr isp_dehaze_attr.dehaze_lut\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_dehaze_attr.op_type), name->valuestring, "isp_dehaze_attr.op_type"); 
    } else {
        printf("no attr isp_dehaze_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dehaze_attr.manual_attr.strength), name->valuestring, "isp_dehaze_attr.manual_attr.strength"); 
    } else {
        printf("no attr isp_dehaze_attr.manual_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_dehaze_attr.auto_attr.strength), name->valuestring, "isp_dehaze_attr.auto_attr.strength"); 
    } else {
        printf("no attr isp_dehaze_attr.auto_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.tmprflt_incr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dehaze_attr.tmprflt_incr_coef), name->valuestring, "isp_dehaze_attr.tmprflt_incr_coef"); 
    } else {
        printf("no attr isp_dehaze_attr.tmprflt_incr_coef\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dehaze_attr.tmprflt_decr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_dehaze_attr.tmprflt_decr_coef), name->valuestring, "isp_dehaze_attr.tmprflt_decr_coef"); 
    } else {
        printf("no attr isp_dehaze_attr.tmprflt_decr_coef\n");
    }

    //printf("check isp cr_attr/dehaze_attr success\n");
    return HI_SUCCESS;
}


//设置去伪彩和去马赛克属性
hi_s32 set_isp_afc_demosaic_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp_anti_false_color_attr/isp_demosaic_attr
    printf("set isp_anti_false_color_attr/demosaic_attr\n");

    ot_isp_anti_false_color_attr isp_anti_false_color_attr = {0};
    ot_isp_demosaic_attr isp_demosaic_attr = {0};

    ret = hi_mpi_isp_get_anti_false_color_attr(vi_pipe, &isp_anti_false_color_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp anti_false_color attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_demosaic_attr(vi_pipe, &isp_demosaic_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp demosaic attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_anti_false_color_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_anti_false_color_attr.enable), name->valuestring); 
    } else {
        printf("set isp_anti_false_color_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_anti_false_color_attr.op_type), name->valuestring); 
    } else {
        printf("set isp_anti_false_color_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.manual_attr.threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_anti_false_color_attr.manual_attr.threshold), name->valuestring); 
    } else {
        printf("set isp_anti_false_color_attr.manual_attr.threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_anti_false_color_attr.manual_attr.strength), name->valuestring); 
    } else {
        printf("set isp_anti_false_color_attr.manual_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.auto_attr.threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_anti_false_color_attr.auto_attr.threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_anti_false_color_attr.auto_attr.threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_anti_false_color_attr.auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_anti_false_color_attr.auto_attr.strength failed\n");
    }

    // isp_demosaic_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_demosaic_attr.enable), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_demosaic_attr.op_type), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.ai_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_demosaic_attr.ai_detail_strength), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.ai_detail_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.nddm_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.nddm_strength), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.nddm_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.nddm_mf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.nddm_mf_detail_strength), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.nddm_mf_detail_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.nddm_hf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.nddm_hf_detail_strength), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.nddm_hf_detail_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.detail_smooth_range");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.detail_smooth_range), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.detail_smooth_range failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_f_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_f_threshold), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.color_noise_f_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_f_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_f_strength), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.color_noise_f_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_y_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_y_threshold), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.color_noise_y_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_y_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_y_strength), name->valuestring); 
    } else {
        printf("set isp_demosaic_attr.manual_attr.color_noise_y_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.nddm_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.nddm_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.nddm_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.nddm_mf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.nddm_mf_detail_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.nddm_mf_detail_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.nddm_hf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.nddm_hf_detail_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.nddm_hf_detail_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.detail_smooth_range");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.detail_smooth_range, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.detail_smooth_range failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_f_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_f_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.color_noise_f_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_f_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_f_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.color_noise_f_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_y_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_y_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.color_noise_y_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_y_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_y_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_demosaic_attr.auto_attr.color_noise_y_strength failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_anti_false_color_attr(vi_pipe, &isp_anti_false_color_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp anti_false_color_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_demosaic_attr(vi_pipe, &isp_demosaic_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp demosaic_attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp anti_false_color_attr/demosaic_attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_afc_demosaic_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp_anti_false_color_attr/isp_demosaic_attr
    printf("check isp_anti_false_color_attr/demosaic_attr\n");

    ot_isp_anti_false_color_attr isp_anti_false_color_attr = {0};
    ot_isp_demosaic_attr isp_demosaic_attr = {0};

    ret = hi_mpi_isp_get_anti_false_color_attr(vi_pipe, &isp_anti_false_color_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp anti_false_color attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_demosaic_attr(vi_pipe, &isp_demosaic_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp demosaic attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_anti_false_color_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_anti_false_color_attr.enable), name->valuestring, "isp_anti_false_color_attr.enable"); 
    } else {
        printf("no attr isp_anti_false_color_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_anti_false_color_attr.op_type), name->valuestring, "isp_anti_false_color_attr.op_type"); 
    } else {
        printf("no attr isp_anti_false_color_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.manual_attr.threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_anti_false_color_attr.manual_attr.threshold), name->valuestring, "isp_anti_false_color_attr.manual_attr.threshold"); 
    } else {
        printf("no attr isp_anti_false_color_attr.manual_attr.threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.manual_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_anti_false_color_attr.manual_attr.strength), name->valuestring, "isp_anti_false_color_attr.manual_attr.strength"); 
    } else {
        printf("no attr isp_anti_false_color_attr.manual_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.auto_attr.threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_anti_false_color_attr.auto_attr.threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_anti_false_color_attr.auto_attr.threshold"); 
    } else {
        printf("no attr isp_anti_false_color_attr.auto_attr.threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_anti_false_color_attr.auto_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_anti_false_color_attr.auto_attr.strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_anti_false_color_attr.auto_attr.strength"); 
    } else {
        printf("no attr isp_anti_false_color_attr.auto_attr.strength\n");
    }

    // isp_demosaic_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_demosaic_attr.enable), name->valuestring, "isp_demosaic_attr.enable"); 
    } else {
        printf("no attr isp_demosaic_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_demosaic_attr.op_type), name->valuestring, "isp_demosaic_attr.op_type"); 
    } else {
        printf("no attr isp_demosaic_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.ai_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_demosaic_attr.ai_detail_strength), name->valuestring, "isp_demosaic_attr.ai_detail_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.ai_detail_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.nddm_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.nddm_strength), name->valuestring, "isp_demosaic_attr.manual_attr.nddm_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.nddm_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.nddm_mf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.nddm_mf_detail_strength), name->valuestring, "isp_demosaic_attr.manual_attr.nddm_mf_detail_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.nddm_mf_detail_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.nddm_hf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.nddm_hf_detail_strength), name->valuestring, "isp_demosaic_attr.manual_attr.nddm_hf_detail_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.nddm_hf_detail_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.detail_smooth_range");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.detail_smooth_range), name->valuestring, "isp_demosaic_attr.manual_attr.detail_smooth_range"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.detail_smooth_range\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_f_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_f_threshold), name->valuestring, "isp_demosaic_attr.manual_attr.color_noise_f_threshold"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.color_noise_f_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_f_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_f_strength), name->valuestring, "isp_demosaic_attr.manual_attr.color_noise_f_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.color_noise_f_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_y_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_y_threshold), name->valuestring, "isp_demosaic_attr.manual_attr.color_noise_y_threshold"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.color_noise_y_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.manual_attr.color_noise_y_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_demosaic_attr.manual_attr.color_noise_y_strength), name->valuestring, "isp_demosaic_attr.manual_attr.color_noise_y_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.manual_attr.color_noise_y_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.nddm_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.nddm_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.nddm_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.nddm_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.nddm_mf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.nddm_mf_detail_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.nddm_mf_detail_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.nddm_mf_detail_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.nddm_hf_detail_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.nddm_hf_detail_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.nddm_hf_detail_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.nddm_hf_detail_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.detail_smooth_range");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.detail_smooth_range, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.detail_smooth_range"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.detail_smooth_range\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_f_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_f_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.color_noise_f_threshold"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.color_noise_f_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_f_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_f_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.color_noise_f_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.color_noise_f_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_y_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_y_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.color_noise_y_threshold"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.color_noise_y_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_demosaic_attr.auto_attr.color_noise_y_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_demosaic_attr.auto_attr.color_noise_y_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_demosaic_attr.auto_attr.color_noise_y_strength"); 
    } else {
        printf("no attr isp_demosaic_attr.auto_attr.color_noise_y_strength\n");
    }

    //printf("check write isp anti_false_color_attr/demosaic_attr success!\n");
    return HI_SUCCESS;
}


//设置bayersharpen和ca属性
hi_s32 set_isp_bayershp_ca_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp bayershp_attr/ca_attr
    //printf("set isp bayershp_attr/ca_attr start\n");

    ot_isp_bayershp_attr isp_bayershp_attr = {0};
    ot_isp_ca_attr isp_ca_attr = {0};

    ret = hi_mpi_isp_get_bayershp_attr(vi_pipe, &isp_bayershp_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp bayershp_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_ca_attr(vi_pipe, &isp_ca_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp ca_attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_bayershp_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_bayershp_attr.enable), name->valuestring); 
    } else {
        printf("set isp_bayershp_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.dark_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_bayershp_attr.dark_threshold, name->valuestring, OT_ISP_BSHP_THD_NUM); 
    } else {
        printf("set isp_bayershp_attr.dark_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.texture_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_bayershp_attr.texture_threshold, name->valuestring, OT_ISP_BSHP_THD_NUM); 
    } else {
        printf("set isp_bayershp_attr.texture_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_bayershp_attr.op_type), name->valuestring); 
    } else {
        printf("set isp_bayershp_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.mf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_bayershp_attr.manual_attr.mf_strength, name->valuestring, OT_ISP_BSHP_CURVE_NUM); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.mf_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.hf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_bayershp_attr.manual_attr.hf_strength, name->valuestring, OT_ISP_BSHP_CURVE_NUM); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.hf_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_bayershp_attr.manual_attr.dark_strength, name->valuestring, OT_ISP_BSHP_CURVE_NUM); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.dark_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.mf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_bayershp_attr.manual_attr.mf_gain), name->valuestring); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.mf_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.hf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_bayershp_attr.manual_attr.hf_gain), name->valuestring); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.hf_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.dark_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_bayershp_attr.manual_attr.dark_gain), name->valuestring); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.dark_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.overshoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_bayershp_attr.manual_attr.overshoot), name->valuestring); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.overshoot failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.undershoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_bayershp_attr.manual_attr.undershoot), name->valuestring); 
    } else {
        printf("set isp_bayershp_attr.manual_attr.undershoot failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.mf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_2d_array_value(isp_bayershp_attr.auto_attr.mf_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_BSHP_CURVE_NUM, "u8"); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.mf_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.hf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_2d_array_value(isp_bayershp_attr.auto_attr.hf_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_BSHP_CURVE_NUM, "u8"); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.hf_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_2d_array_value(isp_bayershp_attr.auto_attr.dark_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_BSHP_CURVE_NUM, "u8"); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.dark_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.mf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_bayershp_attr.auto_attr.mf_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.mf_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.hf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_bayershp_attr.auto_attr.hf_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.hf_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.dark_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_bayershp_attr.auto_attr.dark_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.dark_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.overshoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_bayershp_attr.auto_attr.overshoot, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.overshoot failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.undershoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_bayershp_attr.auto_attr.undershoot, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_bayershp_attr.auto_attr.undershoot failed\n");
    }

    // isp_ca_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_ca_attr.enable), name->valuestring); 
    } else {
        printf("set isp_ca_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca_cp_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_isp_ca_type, &(isp_ca_attr.ca_cp_en), name->valuestring); 
    } else {
        printf("set isp_ca_attr.ca_cp_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca.y_ratio_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u32(isp_ca_attr.ca.y_ratio_lut, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH); 
    } else {
        printf("set isp_ca_attr.ca.y_ratio_lut failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca.iso_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_s32(isp_ca_attr.ca.iso_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_ca_attr.ca.iso_ratio failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca.y_sat_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u32(isp_ca_attr.ca.y_sat_lut, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH); 
    } else {
        printf("set isp_ca_attr.ca.y_sat_lut failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.cp.cp_lut_y");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_ca_attr.cp.cp_lut_y, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH); 
    } else {
        printf("set isp_ca_attr.cp.cp_lut_y failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.cp.cp_lut_u");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_ca_attr.cp.cp_lut_u, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH); 
    } else {
        printf("set isp_ca_attr.cp.cp_lut_u failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.cp.cp_lut_v");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_ca_attr.cp.cp_lut_v, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH); 
    } else {
        printf("set isp_ca_attr.cp.cp_lut_v failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_bayershp_attr(vi_pipe, &isp_bayershp_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp bayershp attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_ca_attr(vi_pipe, &isp_ca_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp ca attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp ca_attr/bayershp_attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_bayershp_ca_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //check isp bayershp_attr/ca_attr
    //printf("check isp bayershp_attr/ca_attr start\n");

    ot_isp_bayershp_attr isp_bayershp_attr = {0};
    ot_isp_ca_attr isp_ca_attr = {0};

    ret = hi_mpi_isp_get_bayershp_attr(vi_pipe, &isp_bayershp_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp bayershp_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_ca_attr(vi_pipe, &isp_ca_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp ca_attr failed with %#x!\n", ret);
        return ret;
    }

    // check isp_bayershp_attr 
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_bayershp_attr.enable), name->valuestring, "isp_bayershp_attr.enable"); 
    } else {
        printf("no attr isp_bayershp_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.dark_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_bayershp_attr.dark_threshold, name->valuestring, OT_ISP_BSHP_THD_NUM, "isp_bayershp_attr.dark_threshold"); 
    } else {
        printf("no attr isp_bayershp_attr.dark_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.texture_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_bayershp_attr.texture_threshold, name->valuestring, OT_ISP_BSHP_THD_NUM, "isp_bayershp_attr.texture_threshold"); 
    } else {
        printf("no attr isp_bayershp_attr.texture_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_bayershp_attr.op_type), name->valuestring, "isp_bayershp_attr.op_type"); 
    } else {
        printf("no attr isp_bayershp_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.mf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_bayershp_attr.manual_attr.mf_strength, name->valuestring, OT_ISP_BSHP_CURVE_NUM, "isp_bayershp_attr.manual_attr.mf_strength"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.mf_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.hf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_bayershp_attr.manual_attr.hf_strength, name->valuestring, OT_ISP_BSHP_CURVE_NUM, "isp_bayershp_attr.manual_attr.hf_strength"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.hf_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_bayershp_attr.manual_attr.dark_strength, name->valuestring, OT_ISP_BSHP_CURVE_NUM, "isp_bayershp_attr.manual_attr.dark_strength"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.dark_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.mf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_bayershp_attr.manual_attr.mf_gain), name->valuestring, "isp_bayershp_attr.manual_attr.mf_gain"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.mf_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.hf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_bayershp_attr.manual_attr.hf_gain), name->valuestring, "isp_bayershp_attr.manual_attr.hf_gain"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.hf_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.dark_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_bayershp_attr.manual_attr.dark_gain), name->valuestring, "isp_bayershp_attr.manual_attr.dark_gain"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.dark_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.overshoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_bayershp_attr.manual_attr.overshoot), name->valuestring, "isp_bayershp_attr.manual_attr.overshoot"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.overshoot\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.manual_attr.undershoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_bayershp_attr.manual_attr.undershoot), name->valuestring, "isp_bayershp_attr.manual_attr.undershoot"); 
    } else {
        printf("no attr isp_bayershp_attr.manual_attr.undershoot\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.mf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_2d_array_value(isp_bayershp_attr.auto_attr.mf_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_BSHP_CURVE_NUM, "u8", "isp_bayershp_attr.auto_attr.mf_strength"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.mf_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.hf_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_2d_array_value(isp_bayershp_attr.auto_attr.hf_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_BSHP_CURVE_NUM, "u8", "isp_bayershp_attr.auto_attr.hf_strength"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.hf_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.dark_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_2d_array_value(isp_bayershp_attr.auto_attr.dark_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_BSHP_CURVE_NUM, "u8", "isp_bayershp_attr.auto_attr.dark_strength"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.dark_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.mf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_bayershp_attr.auto_attr.mf_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_bayershp_attr.auto_attr.mf_gain"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.mf_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.hf_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_bayershp_attr.auto_attr.hf_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_bayershp_attr.auto_attr.hf_gain"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.hf_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.dark_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_bayershp_attr.auto_attr.dark_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_bayershp_attr.auto_attr.dark_gain"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.dark_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.overshoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_bayershp_attr.auto_attr.overshoot, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_bayershp_attr.auto_attr.overshoot"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.overshoot\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_bayershp_attr.auto_attr.undershoot");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_bayershp_attr.auto_attr.undershoot, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_bayershp_attr.auto_attr.undershoot"); 
    } else {
        printf("no attr isp_bayershp_attr.auto_attr.undershoot\n");
    }

    // check isp_ca_attr 
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_ca_attr.enable), name->valuestring, "isp_ca_attr.enable"); 
    } else {
        printf("no attr isp_ca_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca_cp_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_isp_ca_type, &(isp_ca_attr.ca_cp_en), name->valuestring, "isp_ca_attr.ca_cp_en"); 
    } else {
        printf("no attr isp_ca_attr.ca_cp_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca.y_ratio_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u32(isp_ca_attr.ca.y_ratio_lut, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH, "isp_ca_attr.ca.y_ratio_lut"); 
    } else {
        printf("no attr isp_ca_attr.ca.y_ratio_lut\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca.iso_ratio");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_s32(isp_ca_attr.ca.iso_ratio, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_ca_attr.ca.iso_ratio"); 
    } else {
        printf("no attr isp_ca_attr.ca.iso_ratio\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.ca.y_sat_lut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u32(isp_ca_attr.ca.y_sat_lut, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH, "isp_ca_attr.ca.y_sat_lut"); 
    } else {
        printf("no attr isp_ca_attr.ca.y_sat_lut\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.cp.cp_lut_y");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_ca_attr.cp.cp_lut_y, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH, "isp_ca_attr.cp.cp_lut_y"); 
    } else {
        printf("no attr isp_ca_attr.cp.cp_lut_y\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.cp.cp_lut_u");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_ca_attr.cp.cp_lut_u, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH, "isp_ca_attr.cp.cp_lut_u"); 
    } else {
        printf("no attr isp_ca_attr.cp.cp_lut_u\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ca_attr.cp.cp_lut_v");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u8(isp_ca_attr.cp.cp_lut_v, name->valuestring, OT_ISP_CA_YRATIO_LUT_LENGTH, "isp_ca_attr.cp.cp_lut_v"); 
    } else {
        printf("no attr isp_ca_attr.cp.cp_lut_v\n");
    }

    //printf("check isp ca_attr/bayershp_attr success!\n");
    return HI_SUCCESS;
}


//设置去色差和wdr融合图像压缩属性
hi_s32 set_isp_cac_fswdr_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp_cac_attr/isp_wdr_fs_attr
    //printf("set isp_cac_attr/isp_wdr_fs_attr start\n");

    ot_isp_cac_attr isp_cac_attr = {0};
    ot_isp_wdr_fs_attr isp_wdr_fs_attr = {0};

    ret = hi_mpi_isp_get_cac_attr(vi_pipe, &isp_cac_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp cac attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_fswdr_attr(vi_pipe, &isp_wdr_fs_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp wdr fs attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_cac_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_cac_attr.enable), name->valuestring); 
    } else {
        printf("set isp_cac_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_cac_attr.op_type), name->valuestring); 
    } else {
        printf("set isp_cac_attr.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.detect_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_cac_attr.detect_mode), name->valuestring); 
    } else {
        printf("set isp_cac_attr.detect_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.purple_upper_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_s16, &(isp_cac_attr.purple_upper_limit), name->valuestring); 
    } else {
        printf("set isp_cac_attr.purple_upper_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.purple_lower_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_s16, &(isp_cac_attr.purple_lower_limit), name->valuestring); 
    } else {
        printf("set isp_cac_attr.purple_lower_limit failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.edge_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.acac_cfg.acac_manual.edge_threshold, name->valuestring, OT_ISP_CAC_THR_NUM); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_manual.edge_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.edge_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.edge_gain), name->valuestring); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_manual.edge_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.cac_rb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.cac_rb_strength), name->valuestring); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_manual.cac_rb_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.purple_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.purple_alpha), name->valuestring); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_manual.purple_alpha failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.edge_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.edge_alpha), name->valuestring); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_manual.edge_alpha failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.satu_low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.satu_low_threshold), name->valuestring); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_manual.satu_low_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.satu_high_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.satu_high_threshold), name->valuestring); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_manual.satu_high_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.edge_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_2d_array_value(isp_cac_attr.acac_cfg.acac_auto.edge_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_CAC_THR_NUM, "u16"); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_auto.edge_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.edge_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.edge_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_auto.edge_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.cac_rb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.cac_rb_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_auto.cac_rb_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.purple_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.purple_alpha, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_auto.purple_alpha failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.edge_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.edge_alpha, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_auto.edge_alpha failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.satu_low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.satu_low_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_auto.satu_low_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.satu_high_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.satu_high_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_cac_attr.acac_cfg.acac_auto.satu_high_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.purple_detect_range");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.lcac_cfg.purple_detect_range), name->valuestring); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.purple_detect_range failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.var_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_cac_attr.lcac_cfg.var_threshold), name->valuestring); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.var_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.r_detect_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.lcac_cfg.r_detect_threshold, name->valuestring, OT_ISP_CAC_CURVE_NUM); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.r_detect_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.g_detect_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.lcac_cfg.g_detect_threshold, name->valuestring, OT_ISP_CAC_CURVE_NUM); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.g_detect_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.b_detect_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_cac_attr.lcac_cfg.b_detect_threshold, name->valuestring, OT_ISP_CAC_CURVE_NUM); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.b_detect_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cr_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cr_strength), name->valuestring); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cr_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cb_strength), name->valuestring); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cb_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength, name->valuestring, OT_ISP_CAC_EXP_RATIO_NUM); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u8(isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength, name->valuestring, OT_ISP_CAC_EXP_RATIO_NUM); 
    } else {
        printf("set isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength failed\n");
    }

    // isp_wdr_fs_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_merge_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_isp_wdr_merge_mode, &(isp_wdr_fs_attr.wdr_merge_mode), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_merge_mode failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.motion_comp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.motion_comp), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.motion_comp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.short_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.short_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.short_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.long_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.long_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.long_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.force_long");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.force_long), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.force_long failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.force_long_low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.force_long_low_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.force_long_low_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.force_long_hig_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.force_long_hig_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.force_long_hig_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_expo_chk");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_expo_chk), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_expo_chk failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_check_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_check_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_check_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.md_ref_flicker");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.md_ref_flicker), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.md_ref_flicker failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_still_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_still_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_still_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_full_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_full_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_full_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_long_blend");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_long_blend), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_long_blend failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.op_type), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.op_type failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_low_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_low_gain), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_low_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_hig_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_hig_gain), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_hig_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_low_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_2d_array_value(isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_low_gain, name->valuestring, OT_ISP_WDR_RATIO_NUM, OT_ISP_AUTO_ISO_NUM, "u8"); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_low_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_hig_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_2d_array_value(isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_hig_gain, name->valuestring, OT_ISP_WDR_RATIO_NUM, OT_ISP_AUTO_ISO_NUM, "u8"); 
    } else {
        printf("set isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_hig_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_blend_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_wdr_fs_attr.fusion_attr.fusion_blend_en), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.fusion_attr.fusion_blend_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_blend_wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_wdr_fs_attr.fusion_attr.fusion_blend_wgt), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.fusion_attr.fusion_blend_wgt failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_wdr_fs_attr.fusion_attr.fusion_threshold, name->valuestring, OT_ISP_WDR_MAX_FRAME_NUM); 
    } else {
        printf("set isp_wdr_fs_attr.fusion_attr.fusion_threshold failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_force_gray_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_wdr_fs_attr.fusion_attr.fusion_force_gray_en), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.fusion_attr.fusion_force_gray_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_force_blend_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_wdr_fs_attr.fusion_attr.fusion_force_blend_threshold), name->valuestring); 
    } else {
        printf("set isp_wdr_fs_attr.fusion_attr.fusion_force_blend_threshold failed\n");
    }

    // 设置isp参数
    ret = hi_mpi_isp_set_cac_attr(vi_pipe, &isp_cac_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp cac_attr failed with 0x%x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_fswdr_attr(vi_pipe, &isp_wdr_fs_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp wdr_fs_attr failed with 0x%x!\n", ret);
        return ret;
    }
    //printf("set isp wdr_fs_attr/cac_attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_cac_fswdr_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret;
    //check isp_cac_attr/isp_wdr_fs_attr
    //printf("check isp_cac_attr/isp_wdr_fs_attr start\n");

    ot_isp_cac_attr isp_cac_attr = {0};
    ot_isp_wdr_fs_attr isp_wdr_fs_attr = {0};

    ret = hi_mpi_isp_get_cac_attr(vi_pipe, &isp_cac_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp cac attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_fswdr_attr(vi_pipe, &isp_wdr_fs_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp wdr fs attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_cac_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_cac_attr.enable), name->valuestring, "isp_cac_attr.enable");
    } else {
        printf("no attr isp_cac_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_cac_attr.op_type), name->valuestring, "isp_cac_attr.op_type");
    } else {
        printf("no attr isp_cac_attr.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.detect_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_cac_attr.detect_mode), name->valuestring, "isp_cac_attr.detect_mode");
    } else {
        printf("no attr isp_cac_attr.detect_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.purple_upper_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s16, &(isp_cac_attr.purple_upper_limit), name->valuestring, "isp_cac_attr.purple_upper_limit");
    } else {
        printf("no attr isp_cac_attr.purple_upper_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.purple_lower_limit");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s16, &(isp_cac_attr.purple_lower_limit), name->valuestring, "isp_cac_attr.purple_lower_limit");
    } else {
        printf("no attr isp_cac_attr.purple_lower_limit\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.edge_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.acac_cfg.acac_manual.edge_threshold, name->valuestring, OT_ISP_CAC_THR_NUM, "isp_cac_attr.acac_cfg.acac_manual.edge_threshold");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_manual.edge_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.edge_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.edge_gain), name->valuestring, "isp_cac_attr.acac_cfg.acac_manual.edge_gain");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_manual.edge_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.cac_rb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.cac_rb_strength), name->valuestring, "isp_cac_attr.acac_cfg.acac_manual.cac_rb_strength");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_manual.cac_rb_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.purple_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.purple_alpha), name->valuestring, "isp_cac_attr.acac_cfg.acac_manual.purple_alpha");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_manual.purple_alpha\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.edge_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.edge_alpha), name->valuestring, "isp_cac_attr.acac_cfg.acac_manual.edge_alpha");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_manual.edge_alpha\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.satu_low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.satu_low_threshold), name->valuestring, "isp_cac_attr.acac_cfg.acac_manual.satu_low_threshold");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_manual.satu_low_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_manual.satu_high_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.acac_cfg.acac_manual.satu_high_threshold), name->valuestring, "isp_cac_attr.acac_cfg.acac_manual.satu_high_threshold");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_manual.satu_high_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.edge_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_2d_array_value(isp_cac_attr.acac_cfg.acac_auto.edge_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, OT_ISP_CAC_THR_NUM, "u16", "isp_cac_attr.acac_cfg.acac_auto.edge_threshold");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_auto.edge_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.edge_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.edge_gain, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cac_attr.acac_cfg.acac_auto.edge_gain");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_auto.edge_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.cac_rb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.cac_rb_strength, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cac_attr.acac_cfg.acac_auto.cac_rb_strength");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_auto.cac_rb_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.purple_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.purple_alpha, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cac_attr.acac_cfg.acac_auto.purple_alpha");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_auto.purple_alpha\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.edge_alpha");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.edge_alpha, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cac_attr.acac_cfg.acac_auto.edge_alpha");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_auto.edge_alpha\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.satu_low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.satu_low_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cac_attr.acac_cfg.acac_auto.satu_low_threshold");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_auto.satu_low_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.acac_cfg.acac_auto.satu_high_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.acac_cfg.acac_auto.satu_high_threshold, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_cac_attr.acac_cfg.acac_auto.satu_high_threshold");
    } else {
        printf("no attr isp_cac_attr.acac_cfg.acac_auto.satu_high_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.purple_detect_range");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.lcac_cfg.purple_detect_range), name->valuestring, "isp_cac_attr.lcac_cfg.purple_detect_range");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.purple_detect_range\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.var_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_cac_attr.lcac_cfg.var_threshold), name->valuestring, "isp_cac_attr.lcac_cfg.var_threshold");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.var_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.r_detect_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.lcac_cfg.r_detect_threshold, name->valuestring, OT_ISP_CAC_CURVE_NUM, "isp_cac_attr.lcac_cfg.r_detect_threshold");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.r_detect_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.g_detect_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.lcac_cfg.g_detect_threshold, name->valuestring, OT_ISP_CAC_CURVE_NUM, "isp_cac_attr.lcac_cfg.g_detect_threshold");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.g_detect_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.b_detect_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_cac_attr.lcac_cfg.b_detect_threshold, name->valuestring, OT_ISP_CAC_CURVE_NUM, "isp_cac_attr.lcac_cfg.b_detect_threshold");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.b_detect_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cr_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cr_strength), name->valuestring, "isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cr_strength");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cr_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cb_strength), name->valuestring, "isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cb_strength");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.lcac_manual.de_purple_cb_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u8(isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength, name->valuestring, OT_ISP_CAC_EXP_RATIO_NUM, "isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cr_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u8(isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength, name->valuestring, OT_ISP_CAC_EXP_RATIO_NUM, "isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength");
    } else {
        printf("no attr isp_cac_attr.lcac_cfg.lcac_auto.de_purple_cb_strength\n");
    }

    // isp_wdr_fs_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_merge_mode");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_isp_wdr_merge_mode, &(isp_wdr_fs_attr.wdr_merge_mode), name->valuestring, "isp_wdr_fs_attr.wdr_merge_mode");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_merge_mode\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.motion_comp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.motion_comp), name->valuestring, "isp_wdr_fs_attr.wdr_combine.motion_comp");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.motion_comp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.short_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.short_threshold), name->valuestring, "isp_wdr_fs_attr.wdr_combine.short_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.short_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.long_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.long_threshold), name->valuestring, "isp_wdr_fs_attr.wdr_combine.long_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.long_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.force_long");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.force_long), name->valuestring, "isp_wdr_fs_attr.wdr_combine.force_long");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.force_long\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.force_long_low_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.force_long_low_threshold), name->valuestring, "isp_wdr_fs_attr.wdr_combine.force_long_low_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.force_long_low_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.force_long_hig_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.force_long_hig_threshold), name->valuestring, "isp_wdr_fs_attr.wdr_combine.force_long_hig_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.force_long_hig_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_expo_chk");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_expo_chk), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_expo_chk");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_expo_chk\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_check_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_check_threshold), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_check_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.short_check_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.md_ref_flicker");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.md_ref_flicker), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.md_ref_flicker");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.md_ref_flicker\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_still_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_still_threshold), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_still_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_still_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_full_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_full_threshold), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_full_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_full_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_long_blend");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_long_blend), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_long_blend");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.mdt_long_blend\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_op_mode, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.op_type), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.op_type");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.op_type\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_low_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_low_gain), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_low_gain");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_low_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_hig_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_hig_gain), name->valuestring, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_hig_gain");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.manual_attr.md_thr_hig_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_low_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_2d_array_value(isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_low_gain, name->valuestring, OT_ISP_WDR_RATIO_NUM, OT_ISP_AUTO_ISO_NUM, "u8", "isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_low_gain");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_low_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_hig_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_2d_array_value(isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_hig_gain, name->valuestring, OT_ISP_WDR_RATIO_NUM, OT_ISP_AUTO_ISO_NUM, "u8", "isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_hig_gain");
    } else {
        printf("no attr isp_wdr_fs_attr.wdr_combine.wdr_mdt.auto_attr.md_thr_hig_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_blend_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wdr_fs_attr.fusion_attr.fusion_blend_en), name->valuestring, "isp_wdr_fs_attr.fusion_attr.fusion_blend_en");
    } else {
        printf("no attr isp_wdr_fs_attr.fusion_attr.fusion_blend_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_blend_wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wdr_fs_attr.fusion_attr.fusion_blend_wgt), name->valuestring, "isp_wdr_fs_attr.fusion_attr.fusion_blend_wgt");
    } else {
        printf("no attr isp_wdr_fs_attr.fusion_attr.fusion_blend_wgt\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_wdr_fs_attr.fusion_attr.fusion_threshold, name->valuestring, OT_ISP_WDR_MAX_FRAME_NUM, "isp_wdr_fs_attr.fusion_attr.fusion_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.fusion_attr.fusion_threshold\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_force_gray_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_wdr_fs_attr.fusion_attr.fusion_force_gray_en), name->valuestring, "isp_wdr_fs_attr.fusion_attr.fusion_force_gray_en");
    } else {
        printf("no attr isp_wdr_fs_attr.fusion_attr.fusion_force_gray_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_wdr_fs_attr.fusion_attr.fusion_force_blend_threshold");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_wdr_fs_attr.fusion_attr.fusion_force_blend_threshold), name->valuestring, "isp_wdr_fs_attr.fusion_attr.fusion_force_blend_threshold");
    } else {
        printf("no attr isp_wdr_fs_attr.fusion_attr.fusion_force_blend_threshold\n");
    }

    //printf("check write isp wdr_fs_attr/cac_attr success\n");
    return HI_SUCCESS;
}

//设置局域自动对比度增强属性
hi_s32 set_isp_ldci_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp ldci_attr
    //printf("set isp ldci_attr start\n");

    ot_isp_ldci_attr isp_ldci_attr = {0};

    ret = hi_mpi_isp_get_ldci_attr(vi_pipe, &isp_ldci_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp ldci_attr failed with 0x%x!\n", ret);
        return ret;
    }
    // isp_ldci_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_bool, &(isp_ldci_attr.enable), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.gauss_lpf_sigma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_ldci_attr.gauss_lpf_sigma), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.gauss_lpf_sigma failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(ot_op_mode, &(isp_ldci_attr.op_type), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.op_type failed\n");
    }

    // isp_ldci_attr.manual_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.sigma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.sigma), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.sigma failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.mean");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.mean), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.mean failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.wgt), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.wgt failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.sigma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.sigma), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.sigma failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.blc_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_ldci_attr.manual_attr.blc_ctrl), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.manual_attr.blc_ctrl failed\n");
    }

    // isp_ldci_attr.auto_attr 赋值
    for (int i = 0; i < 16; i++) {
        char item[128];
        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.wgt", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            SET_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.wgt), name->valuestring); 
        } else {
            printf("set isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.wgt failed\n", i);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.sigma", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            SET_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.sigma), name->valuestring); 
        } else {
            printf("set isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.sigma failed\n", i);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.mean", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            SET_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.mean), name->valuestring); 
        } else {
            printf("set isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.mean failed\n", i);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.wgt", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            SET_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.wgt), name->valuestring); 
        } else {
            printf("set isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.wgt failed\n", i);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.sigma", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            SET_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.sigma), name->valuestring); 
        } else {
            printf("set isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.sigma failed\n", i);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.mean", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            SET_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.mean), name->valuestring); 
        } else {
            printf("set isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.mean failed\n", i);
        }
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.auto_attr.blc_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        set_array_value_u16(isp_ldci_attr.auto_attr.blc_ctrl, name->valuestring, OT_ISP_AUTO_ISO_NUM); 
    } else {
        printf("set isp_ldci_attr.auto_attr.blc_ctrl failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.tpr_incr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_ldci_attr.tpr_incr_coef), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.tpr_incr_coef failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.tpr_decr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        SET_VALUE(td_u16, &(isp_ldci_attr.tpr_decr_coef), name->valuestring); 
    } else {
        printf("set isp_ldci_attr.tpr_decr_coef failed\n");
    }

    //设置isp参数
    ret = hi_mpi_isp_set_ldci_attr(vi_pipe, &isp_ldci_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp_ldci_attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp_ldci_attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_ldci_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //check isp ldci_attr
    //printf("check isp ldci_attr start\n");

    ot_isp_ldci_attr isp_ldci_attr = {0};

    ret = hi_mpi_isp_get_ldci_attr(vi_pipe, &isp_ldci_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp ldci_attr failed with 0x%x!\n", ret);
        return ret;
    }
    // isp_ldci_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_bool, &(isp_ldci_attr.enable), name->valuestring, "isp_ldci_attr.enable"); 
    } else {
        printf("no attr isp_ldci_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.gauss_lpf_sigma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_ldci_attr.gauss_lpf_sigma), name->valuestring, "isp_ldci_attr.gauss_lpf_sigma"); 
    } else {
        printf("no attr isp_ldci_attr.gauss_lpf_sigma\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.op_type");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(ot_op_mode, &(isp_ldci_attr.op_type), name->valuestring, "isp_ldci_attr.op_type"); 
    } else {
        printf("no attr isp_ldci_attr.op_type\n");
    }

    // isp_ldci_attr.manual_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt), name->valuestring, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt"); 
    } else {
        printf("no attr isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.wgt\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.sigma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.sigma), name->valuestring, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.sigma"); 
    } else {
        printf("no attr isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.sigma\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.mean");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.mean), name->valuestring, "isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.mean"); 
    } else {
        printf("no attr isp_ldci_attr.manual_attr.he_wgt.he_pos_wgt.mean\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.wgt");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.wgt), name->valuestring, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.wgt"); 
    } else {
        printf("no attr isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.wgt\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.sigma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.sigma), name->valuestring, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.sigma"); 
    } else {
        printf("no attr isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.sigma\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u8, &(isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean), name->valuestring, "isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean"); 
    } else {
        printf("no attr isp_ldci_attr.manual_attr.he_wgt.he_neg_wgt.mean\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.manual_attr.blc_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_ldci_attr.manual_attr.blc_ctrl), name->valuestring, "isp_ldci_attr.manual_attr.blc_ctrl"); 
    } else {
        printf("no attr isp_ldci_attr.manual_attr.blc_ctrl\n");
    }

    // isp_ldci_attr.auto_attr check
    for (int i = 0; i < 16; i++) {
        char item[128];
        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.wgt", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            CHECK_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.wgt), name->valuestring, item); 
        } else {
            printf("no attr %s\n", item);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.sigma", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            CHECK_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.sigma), name->valuestring, item); 
        } else {
            printf("no attr %s\n", item);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_pos_wgt.mean", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            CHECK_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_pos_wgt.mean), name->valuestring, item); 
        } else {
            printf("no attr %s\n", item);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.wgt", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            CHECK_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.wgt), name->valuestring, item); 
        } else {
            printf("no attr %s\n", item);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.sigma", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            CHECK_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.sigma), name->valuestring, item); 
        } else {
            printf("no attr %s\n", item);
        }

        snprintf(item, sizeof(item), "isp_ldci_attr.auto_attr.he_wgt[%d].he_neg_wgt.mean", i);
        name = cJSON_GetObjectItemCaseSensitive(json, item);
        if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
            CHECK_VALUE(td_u8, &(isp_ldci_attr.auto_attr.he_wgt[i].he_neg_wgt.mean), name->valuestring, item); 
        } else {
            printf("no attr %s\n", item);
        }
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.auto_attr.blc_ctrl");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        check_array_value_u16(isp_ldci_attr.auto_attr.blc_ctrl, name->valuestring, OT_ISP_AUTO_ISO_NUM, "isp_ldci_attr.auto_attr.blc_ctrl"); 
    } else {
        printf("no attr isp_ldci_attr.auto_attr.blc_ctrl\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.tpr_incr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_ldci_attr.tpr_incr_coef), name->valuestring, "isp_ldci_attr.tpr_incr_coef"); 
    } else {
        printf("no attr isp_ldci_attr.tpr_incr_coef\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_ldci_attr.tpr_decr_coef");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) { 
        CHECK_VALUE(td_u16, &(isp_ldci_attr.tpr_decr_coef), name->valuestring, "isp_ldci_attr.tpr_decr_coef"); 
    } else {
        printf("no attr isp_ldci_attr.tpr_decr_coef\n");
    }
    //printf("check isp_ldci_attr success!\n");
    return HI_SUCCESS;
}

//设置色调、局部裁剪、色彩空间转换、CLUT属性
hi_s32 set_isp_colortone_rc_csc_clut_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp color tone/rc/csc/clut
    //printf("set isp color tone/rc/csc/clut start\n");

    ot_isp_color_tone_attr isp_color_tone_attr = {0};
    ot_isp_rc_attr isp_rc_attr = {0};
    ot_isp_csc_attr isp_csc_attr = {0};
    ot_isp_clut_attr isp_clut_attr = {0};

    ret = hi_mpi_isp_get_color_tone_attr(vi_pipe, &isp_color_tone_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_color_tone_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_rc_attr(vi_pipe, &isp_rc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_rc_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_csc_attr(vi_pipe, &isp_csc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_csc_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_clut_attr(vi_pipe, &isp_clut_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_clut_attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_color_tone_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_tone_attr.red_cast_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_tone_attr.red_cast_gain), name->valuestring);
    } else {
        printf("set isp_color_tone_attr.red_cast_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_tone_attr.green_cast_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_tone_attr.green_cast_gain), name->valuestring);
    } else {
        printf("set isp_color_tone_attr.green_cast_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_tone_attr.blue_cast_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_color_tone_attr.blue_cast_gain), name->valuestring);
    } else {
        printf("set isp_color_tone_attr.blue_cast_gain failed\n");
    }

    // isp_rc_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_rc_attr.enable), name->valuestring);
    } else {
        printf("set isp_rc_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.center_coord.x");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_s32, &(isp_rc_attr.center_coord.x), name->valuestring);
    } else {
        printf("set isp_rc_attr.center_coord.x failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.center_coord.y");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_s32, &(isp_rc_attr.center_coord.y), name->valuestring);
    } else {
        printf("set isp_rc_attr.center_coord.y failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_rc_attr.radius), name->valuestring);
    } else {
        printf("set isp_rc_attr.radius failed\n");
    }

    // isp_csc_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_csc_attr.enable), name->valuestring);
    } else {
        printf("set isp_csc_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.color_gamut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(ot_color_gamut, &(isp_csc_attr.color_gamut), name->valuestring);
    } else {
        printf("set isp_csc_attr.color_gamut failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.hue");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_csc_attr.hue), name->valuestring);
    } else {
        printf("set isp_csc_attr.hue failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.luma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_csc_attr.luma), name->valuestring);
    } else {
        printf("set isp_csc_attr.luma failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.contr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_csc_attr.contr), name->valuestring);
    } else {
        printf("set isp_csc_attr.contr failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.satu");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_csc_attr.satu), name->valuestring);
    } else {
        printf("set isp_csc_attr.satu failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.limited_range_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_csc_attr.limited_range_en), name->valuestring);
    } else {
        printf("set isp_csc_attr.limited_range_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.ext_csc_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_csc_attr.ext_csc_en), name->valuestring);
    } else {
        printf("set isp_csc_attr.ext_csc_en failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.ct_mode_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_csc_attr.ct_mode_en), name->valuestring);
    } else {
        printf("set isp_csc_attr.ct_mode_en failed\n");
    }


    // isp_clut_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_clut_attr.enable), name->valuestring);
    } else {
        printf("set isp_clut_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.gain_r");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_clut_attr.gain_r), name->valuestring);
    } else {
        printf("set isp_clut_attr.gain_r failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.gain_g");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_clut_attr.gain_g), name->valuestring);
    } else {
        printf("set isp_clut_attr.gain_g failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.gain_b");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u32, &(isp_clut_attr.gain_b), name->valuestring);
    } else {
        printf("set isp_clut_attr.gain_b failed\n");
    }

    //设置isp参数
    ret = hi_mpi_isp_set_color_tone_attr(vi_pipe, &isp_color_tone_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp_color_tone_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_rc_attr(vi_pipe, &isp_rc_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp_rc_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_csc_attr(vi_pipe, &isp_csc_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp_csc_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_clut_attr(vi_pipe, &isp_clut_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp_clut_attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp_color_tone_attr/isp_rc_attr/isp_csc_attr/isp_clut_attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_colortone_rc_csc_clut_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp color tone/rc/csc/clut
    //printf("check isp color tone/rc/csc/clut start\n");

    ot_isp_color_tone_attr isp_color_tone_attr = {0};
    ot_isp_rc_attr isp_rc_attr = {0};
    ot_isp_csc_attr isp_csc_attr = {0};
    ot_isp_clut_attr isp_clut_attr = {0};

    ret = hi_mpi_isp_get_color_tone_attr(vi_pipe, &isp_color_tone_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_color_tone_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_rc_attr(vi_pipe, &isp_rc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_rc_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_csc_attr(vi_pipe, &isp_csc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_csc_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_clut_attr(vi_pipe, &isp_clut_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_clut_attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_color_tone_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_tone_attr.red_cast_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_tone_attr.red_cast_gain), name->valuestring, "isp_color_tone_attr.red_cast_gain");
    } else {
        printf("no attr isp_color_tone_attr.red_cast_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_tone_attr.green_cast_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_tone_attr.green_cast_gain), name->valuestring, "isp_color_tone_attr.green_cast_gain");
    } else {
        printf("no attr isp_color_tone_attr.green_cast_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_color_tone_attr.blue_cast_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_color_tone_attr.blue_cast_gain), name->valuestring, "isp_color_tone_attr.blue_cast_gain");
    } else {
        printf("no attr isp_color_tone_attr.blue_cast_gain\n");
    }

    // isp_rc_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_rc_attr.enable), name->valuestring, "isp_rc_attr.enable");
    } else {
        printf("no attr isp_rc_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.center_coord.x");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s32, &(isp_rc_attr.center_coord.x), name->valuestring, "isp_rc_attr.center_coord.x");
    } else {
        printf("no attr isp_rc_attr.center_coord.x\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.center_coord.y");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_s32, &(isp_rc_attr.center_coord.y), name->valuestring, "isp_rc_attr.center_coord.y");
    } else {
        printf("no attr isp_rc_attr.center_coord.y\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_rc_attr.radius");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_rc_attr.radius), name->valuestring, "isp_rc_attr.radius");
    } else {
        printf("no attr isp_rc_attr.radius\n");
    }

    // isp_csc_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_csc_attr.enable), name->valuestring, "isp_csc_attr.enable");
    } else {
        printf("no attr isp_csc_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.color_gamut");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(ot_color_gamut, &(isp_csc_attr.color_gamut), name->valuestring, "isp_csc_attr.color_gamut");
    } else {
        printf("no attr isp_csc_attr.color_gamut\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.hue");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_csc_attr.hue), name->valuestring, "isp_csc_attr.hue");
    } else {
        printf("no attr isp_csc_attr.hue\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.luma");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_csc_attr.luma), name->valuestring, "isp_csc_attr.luma");
    } else {
        printf("no attr isp_csc_attr.luma\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.contr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_csc_attr.contr), name->valuestring, "isp_csc_attr.contr");
    } else {
        printf("no attr isp_csc_attr.contr\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.satu");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_csc_attr.satu), name->valuestring, "isp_csc_attr.satu");
    } else {
        printf("no attr isp_csc_attr.satu\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.limited_range_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_csc_attr.limited_range_en), name->valuestring, "isp_csc_attr.limited_range_en");
    } else {
        printf("no attr isp_csc_attr.limited_range_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.ext_csc_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_csc_attr.ext_csc_en), name->valuestring, "isp_csc_attr.ext_csc_en");
    } else {
        printf("no attr isp_csc_attr.ext_csc_en\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.ct_mode_en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_csc_attr.ct_mode_en), name->valuestring, "isp_csc_attr.ct_mode_en");
    } else {
        printf("no attr isp_csc_attr.ct_mode_en\n");
    }


    // isp_clut_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.en");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_clut_attr.enable), name->valuestring, "isp_clut_attr.enable");
    } else {
        printf("no attr isp_clut_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.gain_r");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_clut_attr.gain_r), name->valuestring, "isp_clut_attr.gain_r");
    } else {
        printf("no attr isp_clut_attr.gain_r\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.gain_g");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_clut_attr.gain_g), name->valuestring, "isp_clut_attr.gain_g");
    } else {
        printf("no attr isp_clut_attr.gain_g\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_clut_attr.gain_b");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u32, &(isp_clut_attr.gain_b), name->valuestring, "isp_clut_attr.gain_b");
    } else {
        printf("no attr isp_clut_attr.gain_b\n");
    }
    //printf("check write isp_color_tone_attr/isp_rc_attr/isp_csc_attr/isp_clut_attr success\n");
    return HI_SUCCESS;
}

//设置dng色彩属性、自动色彩校正、数据解压缩属性
hi_s32 set_isp_dngcolor_acs_expander_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp_dng_color_param/isp_acs_attr/isp_expander_attr
    //printf("set isp_dng_color_param/isp_acs_attr/isp_expander_attr start\n");
    ot_isp_dng_color_param isp_dng_color_param = {0};
    ot_isp_acs_attr isp_acs_attr = {0};
    ot_isp_expander_attr isp_expander_attr = {0};

    ret = hi_mpi_isp_get_dng_color_param(vi_pipe, &isp_dng_color_param);
    if (ret != HI_SUCCESS) {
        printf("get isp_dng_color_param failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_auto_color_shading_attr(vi_pipe, &isp_acs_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_acs_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_expander_attr(vi_pipe, &isp_expander_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_expander_attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_dng_color_param 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.wb_gain1.g_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_dng_color_param.wb_gain1.g_gain), name->valuestring);
    } else {
        printf("set isp_dng_color_param.wb_gain1.g_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.wb_gain2.g_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_dng_color_param.wb_gain2.g_gain), name->valuestring);
    } else {
        printf("set isp_dng_color_param.wb_gain2.g_gain failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab1.color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_dng_color_param.ccm_tab1.color_temp), name->valuestring);
    } else {
        printf("set isp_dng_color_param.ccm_tab1.color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab1.ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_dng_color_param.ccm_tab1.ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_dng_color_param.ccm_tab1.ccm failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab2.color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_dng_color_param.ccm_tab2.color_temp), name->valuestring);
    } else {
        printf("set isp_dng_color_param.ccm_tab2.color_temp failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab2.ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_dng_color_param.ccm_tab2.ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE);
    } else {
        printf("set isp_dng_color_param.ccm_tab2.ccm failed\n");
    }

    // isp_acs_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_acs_attr.enable), name->valuestring);
    } else {
        printf("set isp_acs_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.y_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_acs_attr.y_strength), name->valuestring);
    } else {
        printf("set isp_acs_attr.y_strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.run_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_acs_attr.run_interval), name->valuestring);
    } else {
        printf("set isp_acs_attr.run_interval failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.lock_enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_acs_attr.lock_enable), name->valuestring);
    } else {
        printf("set isp_acs_attr.lock_enable failed\n");
    }

    // isp_expander_attr 赋值
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_expander_attr.enable), name->valuestring);
    } else {
        printf("set isp_expander_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.bit_depth_in");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_expander_attr.bit_depth_in), name->valuestring);
    } else {
        printf("set isp_expander_attr.bit_depth_in failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.bit_depth_out");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u8, &(isp_expander_attr.bit_depth_out), name->valuestring);
    } else {
        printf("set isp_expander_attr.bit_depth_out failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.knee_point_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_expander_attr.knee_point_num), name->valuestring);
    } else {
        printf("set isp_expander_attr.knee_point_num failed\n");
    }

    // name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.knee_point_coord");
    // if (cJSON_IsString(name) && (name->valuestring != NULL)) set_array_value_u16(isp_expander_attr.knee_point_coord, name->valuestring, OT_ISP_EXPANDER_POINT_NUM_MAX);
    // else printf("set isp_expander_attr.knee_point_coord failed\n");

    //设置isp参数
    ret = hi_mpi_isp_set_dng_color_param(vi_pipe, &isp_dng_color_param);
    if (ret != HI_SUCCESS) {
        printf("set isp dng color param failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_auto_color_shading_attr(vi_pipe, &isp_acs_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp acs attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_expander_attr(vi_pipe, &isp_expander_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp expander attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp dng_color_param/expander/acs attr success!\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_dngcolor_acs_expander_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //check isp_dng_color_param/isp_acs_attr/isp_expander_attr
    //printf("check isp_dng_color_param/isp_acs_attr/isp_expander_attr start\n");
    ot_isp_dng_color_param isp_dng_color_param = {0};
    ot_isp_acs_attr isp_acs_attr = {0};
    ot_isp_expander_attr isp_expander_attr = {0};

    ret = hi_mpi_isp_get_dng_color_param(vi_pipe, &isp_dng_color_param);
    if (ret != HI_SUCCESS) {
        printf("get isp_dng_color_param failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_auto_color_shading_attr(vi_pipe, &isp_acs_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_acs_attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_expander_attr(vi_pipe, &isp_expander_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp_expander_attr failed with %#x!\n", ret);
        return ret;
    }

    // isp_dng_color_param check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.wb_gain1.g_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_dng_color_param.wb_gain1.g_gain), name->valuestring, "isp_dng_color_param.wb_gain1.g_gain");
    } else {
        printf("no attr isp_dng_color_param.wb_gain1.g_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.wb_gain2.g_gain");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_dng_color_param.wb_gain2.g_gain), name->valuestring, "isp_dng_color_param.wb_gain2.g_gain");
    } else {
        printf("no attr isp_dng_color_param.wb_gain2.g_gain\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab1.color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_dng_color_param.ccm_tab1.color_temp), name->valuestring, "isp_dng_color_param.ccm_tab1.color_temp");
    } else {
        printf("no attr isp_dng_color_param.ccm_tab1.color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab1.ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_dng_color_param.ccm_tab1.ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_dng_color_param.ccm_tab1.ccm");
    } else {
        printf("no attr isp_dng_color_param.ccm_tab1.ccm\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab2.color_temp");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_dng_color_param.ccm_tab2.color_temp), name->valuestring, "isp_dng_color_param.ccm_tab2.color_temp");
    } else {
        printf("no attr isp_dng_color_param.ccm_tab2.color_temp\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_dng_color_param.ccm_tab2.ccm");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_dng_color_param.ccm_tab2.ccm, name->valuestring, OT_ISP_CCM_MATRIX_SIZE, "isp_dng_color_param.ccm_tab2.ccm");
    } else {
        printf("no attr isp_dng_color_param.ccm_tab2.ccm\n");
    }

    // isp_acs_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_acs_attr.enable), name->valuestring, "isp_acs_attr.enable");
    } else {
        printf("no attr isp_acs_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.y_strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_acs_attr.y_strength), name->valuestring, "isp_acs_attr.y_strength");
    } else {
        printf("no attr isp_acs_attr.y_strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.run_interval");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_acs_attr.run_interval), name->valuestring, "isp_acs_attr.run_interval");
    } else {
        printf("no attr isp_acs_attr.run_interval\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_acs_attr.lock_enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_acs_attr.lock_enable), name->valuestring, "isp_acs_attr.lock_enable");
    } else {
        printf("no attr isp_acs_attr.lock_enable\n");
    }

    // isp_expander_attr check
    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_expander_attr.enable), name->valuestring, "isp_expander_attr.enable");
    } else {
        printf("no attr isp_expander_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.bit_depth_in");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_expander_attr.bit_depth_in), name->valuestring, "isp_expander_attr.bit_depth_in");
    } else {
        printf("no attr isp_expander_attr.bit_depth_in\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.bit_depth_out");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u8, &(isp_expander_attr.bit_depth_out), name->valuestring, "isp_expander_attr.bit_depth_out");
    } else {
        printf("no attr isp_expander_attr.bit_depth_out\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.knee_point_num");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_expander_attr.knee_point_num), name->valuestring, "isp_expander_attr.knee_point_num");
    } else {
        printf("no attr isp_expander_attr.knee_point_num\n");
    }

    // name = cJSON_GetObjectItemCaseSensitive(json, "isp_expander_attr.knee_point_coord");
    // if (cJSON_IsString(name) && (name->valuestring != NULL)) check_array_value_u16(isp_expander_attr.knee_point_coord, name->valuestring, OT_ISP_EXPANDER_POINT_NUM_MAX);
    // else printf("no attr isp_expander_attr.knee_point_coord\n");

    //printf("check isp dng_color_param/expander/acs attr success!\n");
    return HI_SUCCESS;
}

//设置局部BLC属性
hi_s32 set_isp_lblc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp lblc/lblc_lut attr
    //printf("set isp lblc/lblc_lut attr start\n");

    ot_isp_lblc_attr isp_lblc_attr = {0};
    ot_isp_lblc_lut_attr isp_lblc_lut_attr = {0};

    ret = hi_mpi_isp_get_lblc_attr(vi_pipe, &isp_lblc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp lblc attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_lblc_lut_attr(vi_pipe, &isp_lblc_lut_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp lblc lut attr failed with %#x!\n", ret);
        return ret;
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_bool, &(isp_lblc_attr.enable), name->valuestring);
    } else {
        printf("set isp_lblc_attr.enable failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_lblc_attr.strength), name->valuestring);
    } else {
        printf("set isp_lblc_attr.strength failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_r");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_lblc_lut_attr.offset_r), name->valuestring);
    } else {
        printf("set isp_lblc_lut_attr.offset_r failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_gr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_lblc_lut_attr.offset_gr), name->valuestring);
    } else {
        printf("set isp_lblc_lut_attr.offset_gr failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_gb");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_lblc_lut_attr.offset_gb), name->valuestring);
    } else {
        printf("set isp_lblc_lut_attr.offset_gb failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_b");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        SET_VALUE(td_u16, &(isp_lblc_lut_attr.offset_b), name->valuestring);
    } else {
        printf("set isp_lblc_lut_attr.offset_b failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_r");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_lblc_lut_attr.mesh_blc_r, name->valuestring, OT_ISP_LBLC_GRID_POINTS);
    } else {
        printf("set isp_lblc_lut_attr.mesh_blc_r failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_gr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_lblc_lut_attr.mesh_blc_gr, name->valuestring, OT_ISP_LBLC_GRID_POINTS);
    } else {
        printf("set isp_lblc_lut_attr.mesh_blc_gr failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_gb");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_lblc_lut_attr.mesh_blc_gb, name->valuestring, OT_ISP_LBLC_GRID_POINTS);
    } else {
        printf("set isp_lblc_lut_attr.mesh_blc_gb failed\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_b");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        set_array_value_u16(isp_lblc_lut_attr.mesh_blc_b, name->valuestring, OT_ISP_LBLC_GRID_POINTS);
    } else {
        printf("set isp_lblc_lut_attr.mesh_blc_b failed\n");
    }

    ret = hi_mpi_isp_set_lblc_attr(vi_pipe, &isp_lblc_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp lblc attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_set_lblc_lut_attr(vi_pipe, &isp_lblc_lut_attr);
    if (ret != HI_SUCCESS) {
        printf("set isp lblc lut attr failed with %#x!\n", ret);
        return ret;
    }
    //printf("set isp lblc/lblc_lut attr success\n");
    return HI_SUCCESS;
}
hi_s32 check_isp_lblc_attr_byjson(cJSON* json, hi_vi_pipe vi_pipe)
{
    cJSON* name = NULL;
    hi_s32 ret; 
    //set isp lblc/lblc_lut attr
    //printf("check isp lblc/lblc_lut attr start\n");

    ot_isp_lblc_attr isp_lblc_attr = {0};
    ot_isp_lblc_lut_attr isp_lblc_lut_attr = {0};

    ret = hi_mpi_isp_get_lblc_attr(vi_pipe, &isp_lblc_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp lblc attr failed with %#x!\n", ret);
        return ret;
    }
    ret = hi_mpi_isp_get_lblc_lut_attr(vi_pipe, &isp_lblc_lut_attr);
    if (ret != HI_SUCCESS) {
        printf("get isp lblc lut attr failed with %#x!\n", ret);
        return ret;
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_attr.enable");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_bool, &(isp_lblc_attr.enable), name->valuestring, "isp_lblc_attr.enable");
    } else {
        printf("no attr isp_lblc_attr.enable\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_attr.strength");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_lblc_attr.strength), name->valuestring, "isp_lblc_attr.strength");
    } else {
        printf("no attr isp_lblc_attr.strength\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_r");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_lblc_lut_attr.offset_r), name->valuestring, "isp_lblc_lut_attr.offset_r");
    } else {
        printf("no attr isp_lblc_lut_attr.offset_r\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_gr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_lblc_lut_attr.offset_gr), name->valuestring, "isp_lblc_lut_attr.offset_gr");
    } else {
        printf("no attr isp_lblc_lut_attr.offset_gr\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_gb");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_lblc_lut_attr.offset_gb), name->valuestring, "isp_lblc_lut_attr.offset_gb");
    } else {
        printf("no attr isp_lblc_lut_attr.offset_gb\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.offset_b");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        CHECK_VALUE(td_u16, &(isp_lblc_lut_attr.offset_b), name->valuestring, "isp_lblc_lut_attr.offset_b");
    } else {
        printf("no attr isp_lblc_lut_attr.offset_b\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_r");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_lblc_lut_attr.mesh_blc_r, name->valuestring, OT_ISP_LBLC_GRID_POINTS, "isp_lblc_lut_attr.mesh_blc_r");
    } else {
        printf("no attr isp_lblc_lut_attr.mesh_blc_r\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_gr");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_lblc_lut_attr.mesh_blc_gr, name->valuestring, OT_ISP_LBLC_GRID_POINTS, "isp_lblc_lut_attr.mesh_blc_gr");
    } else {
        printf("no attr isp_lblc_lut_attr.mesh_blc_gr\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_gb");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_lblc_lut_attr.mesh_blc_gb, name->valuestring, OT_ISP_LBLC_GRID_POINTS, "isp_lblc_lut_attr.mesh_blc_gb");
    } else {
        printf("no attr isp_lblc_lut_attr.mesh_blc_gb\n");
    }

    name = cJSON_GetObjectItemCaseSensitive(json, "isp_lblc_lut_attr.mesh_blc_b");
    if (cJSON_IsString(name) && (name->valuestring != NULL)) {
        check_array_value_u16(isp_lblc_lut_attr.mesh_blc_b, name->valuestring, OT_ISP_LBLC_GRID_POINTS, "isp_lblc_lut_attr.mesh_blc_b");
    } else {
        printf("no attr isp_lblc_lut_attr.mesh_blc_b\n");
    }

    //printf("check write isp lblc/lblc_lut attr success\n");
    return HI_SUCCESS;
}



//输入后检查报告错误的属性
// SET_VALUE(td_u16, &(isp_dng_color_param.wb_gain1.r_gain), "378");
// SET_VALUE(td_u16, &(isp_dng_color_param.wb_gain1.b_gain), "430");
// SET_VALUE(td_u16, &(isp_dng_color_param.wb_gain2.r_gain), "439");
// SET_VALUE(td_u16, &(isp_dng_color_param.wb_gain2.b_gain), "439");
// set_array_value_u16(isp_expander_attr.knee_point_coord, "256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576,256,1048576", OT_ISP_EXPANDER_POINT_NUM_MAX);
// set_array_value_u16(isp_shading_lut_attr.x_grid_width, "30,30,30,30,30,30,30,30,30,30,30,30,31,31,31,31", OT_ISP_MLSC_X_HALF_GRID_NUM);
// set_array_value_u16(isp_shading_lut_attr.y_grid_width, "20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20", OT_ISP_MLSC_Y_HALF_GRID_NUM);

//isp_csc_attr.csc_magtrx包含3个数组，长度分别为3，3，9，但是json中为一个15长度的字符串，没有将三个数组分开，因为不知道顺序，所以不对其赋值
// name = cJSON_GetObjectItemCaseSensitive(json, "isp_csc_attr.csc_magtrx");
// if (cJSON_IsString(name) && (name->valuestring != NULL)) {
//     set_array_value_s16(isp_csc_attr.csc_magtrx.csc_coef, name->valuestring, OT_ISP_CSC_COEF_NUM);
// } else {
//     printf("set isp_csc_attr.csc_magtrx.csc_coef failed\n");
// }