/*
  Copyright (c), 2001-2024, Shenshu Tech. Co., Ltd.
 */
#include "sample_common_dpu.h"

#include <limits.h>
#include "securec.h"
#include "hi_mpi_sys.h"
#include "hi_mpi_sys_mem.h"
#include "hi_mpi_sys_bind.h"
#include "hi_mpi_vb.h"
#include "hi_common_dpu_rect.h"
#include "hi_mpi_dpu_rect.h"
#include "hi_mpi_dpu_match.h"

hi_u32 sample_common_svp_dpu_calc_stride(hi_u32 width, hi_u8 align)
{
    sample_svp_dpu_check_exps_return(align == 0, 0, "align can't be zero!\n");
    return (width + (align - width % align) % align);
}

/* function : create dpu rect memory info */
static hi_s32 sample_common_svp_dpu_rect_create_mem_info(hi_dpu_rect_mem_info *mem_info,
    hi_char *mmb, hi_char *zone, hi_u32 size)
{
    hi_s32 ret;

    mem_info->size = size;
    ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)(&mem_info->phys_addr),
        (hi_void **)&mem_info->virt_addr, mmb, zone, size);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "Err(%#x), hi_mpi_sys_mmz_alloc failed!\n", ret);
    return ret;
}

static hi_s32 sample_svp_dpu_get_lut_from_file(const hi_char *file_name,
    hi_dpu_rect_mem_info *mem_info)
{
    hi_s32 ret;
    hi_u32 size;
    hi_void *virt_addr = HI_NULL;
    FILE *fp = HI_NULL;
    hi_char path[PATH_MAX + 1] = {0};

    sample_svp_dpu_check_exps_return(((strlen(file_name) > PATH_MAX) || (realpath(file_name, path) == HI_NULL)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Error, file_name is invalid!\n");
    fp = fopen(file_name, "rb");
    sample_svp_dpu_check_exps_return(fp == HI_NULL,
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, open lut file failed!\n");
    ret = fseek(fp, 0L, SEEK_END);
    sample_svp_dpu_check_exps_goto(ret == -1, fail_0, "Error, fseek failed!\n");
    size = ftell(fp);
    sample_svp_dpu_check_exps_goto(size <= 0, fail_0, "Error, ftell failed!\n");
    ret = fseek(fp, 0L, SEEK_SET);
    sample_svp_dpu_check_exps_goto(ret == -1, fail_0, "Error, fseek failed!\n");

    ret = memset_s(mem_info, sizeof(hi_dpu_rect_mem_info), 0, sizeof(hi_dpu_rect_mem_info));
    sample_svp_dpu_check_exps_goto(ret != EOK, fail_0, "Err, memset_s mem_info failed!\n");
    ret = sample_common_svp_dpu_rect_create_mem_info(mem_info, "sampe_dpu_rect_lut", HI_NULL, size);
    sample_svp_dpu_check_exps_goto(ret != EOK, fail_0, "Err, dpu_rect_create_mem_info failed!\n");

    virt_addr = sample_svp_dpu_convert_addr_to_ptr(hi_void, mem_info->virt_addr);
    ret = fread(virt_addr, size, 1, fp);
    sample_svp_dpu_check_exps_goto(ret != 1, fail_1, "Error,read lut file failed!\n");
    if (fp != HI_NULL) {
        fclose(fp);
    }

    return HI_SUCCESS;
fail_1:
    sample_svp_dpu_mmz_free(mem_info->phys_addr, mem_info->virt_addr);
fail_0:
    if (fp != HI_NULL) {
        fclose(fp);
    }
    return HI_ERR_DPU_RECT_ILLEGAL_PARAM;
}

hi_s32 sample_common_svp_dpu_rect_load_lut(const hi_char *file_name,
    hi_dpu_rect_mem_info *mem_info, hi_dpu_rect_lut_id *lut_id)
{
    hi_s32 ret;
    hi_char path[PATH_MAX + 1] = {0};

    sample_svp_dpu_check_exps_return(((file_name == HI_NULL) || (mem_info == HI_NULL) || (lut_id == HI_NULL)),
        HI_ERR_DPU_RECT_NULL_PTR, "Error, model_file or nnie_model or lut_id is NULL!\n");
    sample_svp_dpu_check_exps_return(((strlen(file_name) > PATH_MAX) || (realpath(file_name, path) == HI_NULL)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Error, file_name is invalid!\n");

    ret = sample_svp_dpu_get_lut_from_file(file_name, mem_info);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS,
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, get_lut_from_file failed!\n");

    ret = hi_mpi_dpu_rect_load_lut(mem_info, lut_id);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail,
        "Error(%#x), hi_mpi_dpu_rect_load_lut failed!\n", ret);

    return ret;
fail:
    sample_svp_dpu_mmz_free(mem_info->phys_addr, mem_info->virt_addr);
    return ret;
}

hi_void sample_common_svp_dpu_rect_unload_lut(hi_dpu_rect_mem_info *mem_info,
    hi_dpu_rect_lut_id lut_id)
{
    if (mem_info == HI_NULL) {
        sample_svp_dpu_trace_err("Error, mem_info can't be HI_NULL!\n");
        return;
    }
    (hi_void)hi_mpi_dpu_rect_unload_lut(lut_id);
    sample_svp_dpu_mmz_free(mem_info->phys_addr, mem_info->virt_addr);
}

/* function : start dpu rect grp. */
hi_s32 sample_common_svp_dpu_rect_start(hi_dpu_rect_grp dpu_rect_grp,
    hi_dpu_rect_grp_attr *grp_attr, hi_dpu_rect_chn_attr *chn_attr, hi_u32 chn_num)
{
    hi_dpu_rect_chn dpu_rect_chn;
    hi_s32 ret;
    hi_s32 j;
    hi_s32 dpu_rect_chn_num;

    sample_svp_dpu_check_exps_return(((grp_attr == HI_NULL) || (chn_attr == HI_NULL)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Error, grp_attr or chn_attr is HI_NULL!\n");
    sample_svp_dpu_check_exps_return(((dpu_rect_grp < 0) || (dpu_rect_grp >= HI_DPU_RECT_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_grp(%d) must be in [0,%d)!\n",
        dpu_rect_grp, HI_DPU_RECT_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(chn_num < HI_DPU_RECT_MAX_CHN_NUM, HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "Err, chn_num(%u) can' less than %u!\n", chn_num, HI_DPU_RECT_MAX_CHN_NUM);

    ret = hi_mpi_dpu_rect_create_grp(dpu_rect_grp, grp_attr);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_rect_create_grp failed!\n", ret);

    if (grp_attr->rect_mode == HI_DPU_RECT_MODE_SINGLE) {
        dpu_rect_chn_num = 1;
    } else  {
        dpu_rect_chn_num = HI_DPU_RECT_MAX_CHN_NUM;
    }

    for (j = 0; j < dpu_rect_chn_num; j++) {
        dpu_rect_chn = j;
        ret = hi_mpi_dpu_rect_set_chn_attr(dpu_rect_grp, dpu_rect_chn, &chn_attr[dpu_rect_chn]);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
            "Err(%#x), hi_mpi_dpu_rect_set_chn_attr failed!\n", ret);

        ret = hi_mpi_dpu_rect_enable_chn(dpu_rect_grp, dpu_rect_chn);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
            "Err(%#x), hi_mpi_dpu_rect_enable_chn failed!\n", ret);
    }

    ret = hi_mpi_dpu_rect_start_grp(dpu_rect_grp);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_rect_start_grp failed!\n", ret);

    return HI_SUCCESS;
}

/* function : stop dpu rect grp */
hi_s32 sample_common_svp_dpu_rect_stop(hi_dpu_rect_grp dpu_rect_grp, hi_dpu_rect_mode rect_mode)
{
    hi_s32 j;
    hi_s32 ret;
    hi_dpu_rect_chn dpu_rect_chn;
    hi_s32 dpu_rect_chn_num;

    sample_svp_dpu_check_exps_return(((dpu_rect_grp < 0) || (dpu_rect_grp >= HI_DPU_RECT_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_grp(%d) must be in [0,%d)!\n",
        dpu_rect_grp, HI_DPU_RECT_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(((rect_mode < HI_DPU_RECT_MODE_SINGLE) ||
        (dpu_rect_grp >= HI_DPU_RECT_MODE_BUTT)), HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "Err, rect_mode(%d) must be in [%d,%d)!\n", rect_mode, HI_DPU_RECT_MODE_SINGLE, HI_DPU_RECT_MODE_BUTT);

    ret = hi_mpi_dpu_rect_stop_grp(dpu_rect_grp);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_rect_stop_grp failed!\n", ret);

    if (rect_mode == HI_DPU_RECT_MODE_SINGLE) {
        dpu_rect_chn_num = 1;
    } else {
        dpu_rect_chn_num = HI_DPU_RECT_MAX_CHN_NUM;
    }

    for (j = 0; j < dpu_rect_chn_num; j++) {
        dpu_rect_chn = j;
        ret = hi_mpi_dpu_rect_disable_chn(dpu_rect_grp, dpu_rect_chn);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
            "Err(%#x), hi_mpi_dpu_rect_disable_chn failed!\n", ret);
    }

    ret = hi_mpi_dpu_rect_destroy_grp(dpu_rect_grp);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_rect_disable_chn failed!\n", ret);

    return HI_SUCCESS;
}

static hi_s32 sample_svp_dpu_rect_read_one_frame(FILE *fp, hi_u8 *y,
    hi_u32 width, hi_u32 height, hi_u32 stride)
{
    hi_u8 *dst = HI_NULL;
    hi_u32 row;
    hi_s32 ret;

    dst = y;
    for (row = 0; row < height; row++) {
        ret = fread(dst, width, 1, fp);
        sample_svp_dpu_check_exps_return(ret != 1, HI_ERR_DPU_RECT_ILLEGAL_PARAM, "fread failed!\n");
        dst += stride;
    }

    return HI_SUCCESS;
}

/* function : get frame from file */
hi_s32 sample_common_svp_dpu_rect_get_frame_from_file(FILE *fp, const hi_size *pic_size,
    hi_u32 stride, hi_video_frame_info *frame_info, hi_vb_blk vb_blk)
{
    hi_u32 size;
    hi_phys_addr_t phys_addr;
    hi_void *virt_addr = HI_NULL;
    hi_s32 ret;

    sample_svp_dpu_check_exps_return(pic_size == HI_NULL, HI_ERR_DPU_RECT_NULL_PTR, "pic_size can't be null!\n");
    phys_addr = hi_mpi_vb_handle_to_phys_addr(vb_blk);
    sample_svp_dpu_check_exps_return(phys_addr == 0,
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "hi_mpi_vb_handle_to_phys_addr failed!\n");

    size = stride * pic_size->height;
    virt_addr = hi_mpi_sys_mmap(phys_addr, size);
    sample_svp_dpu_check_exps_return(virt_addr == HI_NULL,
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "hi_mpi_sys_mmap failed!\n");

    frame_info->pool_id = hi_mpi_vb_handle_to_pool_id(vb_blk);
    sample_svp_dpu_check_exps_goto(frame_info->pool_id == HI_VB_INVALID_POOL_ID,
        fail, "hi_mpi_vb_handle_to_pool_id failed!\n");

    frame_info->video_frame.phys_addr[0] = phys_addr;
    frame_info->video_frame.virt_addr[0] = virt_addr;
    frame_info->video_frame.width  = pic_size->width;
    frame_info->video_frame.height = pic_size->height;
    frame_info->video_frame.stride[0] = stride;
    frame_info->video_frame.compress_mode = HI_COMPRESS_MODE_NONE;
    frame_info->video_frame.video_format = HI_VIDEO_FORMAT_LINEAR;
    frame_info->video_frame.dynamic_range = HI_DYNAMIC_RANGE_SDR8;
    frame_info->video_frame.pixel_format = HI_PIXEL_FORMAT_YUV_400;
    frame_info->video_frame.field = HI_VIDEO_FIELD_FRAME;

    ret = sample_svp_dpu_rect_read_one_frame(fp, (hi_u8*)frame_info->video_frame.virt_addr[0],
        frame_info->video_frame.width, frame_info->video_frame.height, frame_info->video_frame.stride[0]);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail,
        "hi_mpi_vb_handle_to_pool_id failed!\n");

    (hi_void)hi_mpi_sys_munmap(virt_addr, size);
    frame_info->video_frame.virt_addr[0] = HI_NULL;
    virt_addr = HI_NULL;

    return HI_SUCCESS;
fail:
    (hi_void)hi_mpi_sys_munmap(virt_addr, size);
    return HI_ERR_DPU_RECT_ILLEGAL_PARAM;
}

/* function : write  image to binary file */
static hi_s32 sample_svp_dpu_write_to_bin_file(FILE *fp, hi_u8 *src,
    const hi_video_frame *frame_info, hi_u32 ele_size)
{
    hi_u32 i;
    hi_s32 ret;

    for (i = 0; i < frame_info->height; i++) {
        ret = fwrite(src, frame_info->width * ele_size, 1, fp);
        sample_svp_dpu_check_exps_return(ret != 1, HI_ERR_DPU_RECT_ILLEGAL_PARAM, "fwrite file failed!\n");
        src += frame_info->stride[0];
    }

    (void)fflush(fp);
    return  HI_SUCCESS;
}

/* function : write frame to file. */
hi_s32 sample_common_svp_dpu_write_frame_to_file(FILE *fp, hi_u32 ele_size,
    hi_video_frame_info *frame_info)
{
    hi_u32 stride;
    hi_u32 height;
    hi_void *virt_addr = HI_NULL;
    hi_u32 size;
    hi_s32 ret;

    height = frame_info->video_frame.height;
    stride = frame_info->video_frame.stride[0];
    size = stride * height;

    virt_addr = hi_mpi_sys_mmap(frame_info->video_frame.phys_addr[0], size);
    sample_svp_dpu_check_exps_return(virt_addr == HI_NULL,
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "hi_mpi_sys_mmap failed!\n");

    ret = sample_svp_dpu_write_to_bin_file(fp, (hi_u8*)virt_addr, &frame_info->video_frame, ele_size);
    sample_svp_dpu_check_exps_goto(ret != HI_SUCCESS, fail, "write_to_bin_filp failed!\n");
fail:
    (hi_void)hi_mpi_sys_munmap(virt_addr, size);
    virt_addr = HI_NULL;

    return ret;
}

/* function : create dpu match memory info */
hi_s32 sample_common_svp_dpu_match_create_mem_info(hi_dpu_match_mem_info *mem_info,
    hi_char *mmb, hi_char *zone, hi_u32 size)
{
    hi_s32 ret;

    sample_svp_dpu_check_exps_return(mem_info == HI_NULL,
        HI_ERR_DPU_MATCH_ILLEGAL_PARAM, "mem_info can't be NULL\n");
    ret = memset_s(mem_info, sizeof(hi_dpu_rect_mem_info), 0, sizeof(hi_dpu_rect_mem_info));
    sample_svp_dpu_check_exps_return(ret != EOK,
        HI_ERR_DPU_MATCH_ILLEGAL_PARAM, "Err, memset_s mem_info failed!\n");

    mem_info->size = size;
    ret = hi_mpi_sys_mmz_alloc((hi_phys_addr_t *)&mem_info->phys_addr,
        (hi_void **)&mem_info->virt_addr, mmb, zone, size);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS,
        HI_ERR_DPU_MATCH_ILLEGAL_PARAM, "Err(%#x), hi_mpi_sys_mmz_alloc failed\n", ret);

    return HI_SUCCESS;
}

/* function : start dpu match grp. */
hi_s32 sample_common_svp_dpu_match_start(hi_dpu_match_grp dpu_match_grp,
    hi_dpu_match_grp_attr *grp_attr, hi_dpu_match_chn_attr *chn_attr)
{
    hi_dpu_match_chn dpu_match_chn;
    hi_s32 ret;
    hi_s32 j;

    sample_svp_dpu_check_exps_return((grp_attr == HI_NULL) || (chn_attr == HI_NULL),
        HI_ERR_DPU_MATCH_NULL_PTR, "grp_attr or chn_attr can't be NULL\n");
    sample_svp_dpu_check_exps_return(((dpu_match_grp < 0) || (dpu_match_grp >= HI_DPU_MATCH_MAX_GRP_NUM)),
        HI_ERR_DPU_MATCH_ILLEGAL_PARAM, "Err, dpu_match_grp(%d) must be in [0,%d)!\n",
        dpu_match_grp, HI_DPU_MATCH_MAX_GRP_NUM);

    ret = hi_mpi_dpu_match_create_grp(dpu_match_grp, grp_attr);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_match_create_grp failed\n", ret);

    for (j = 0; j < HI_DPU_MATCH_MAX_CHN_NUM; j++) {
        dpu_match_chn = j;
        ret = hi_mpi_dpu_match_set_chn_attr(dpu_match_grp, dpu_match_chn, chn_attr);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
            "Err(%#x), hi_mpi_dpu_match_set_chn_attr failed\n", ret);

        ret = hi_mpi_dpu_match_enable_chn(dpu_match_grp, dpu_match_chn);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
            "Err(%#x), hi_mpi_dpu_match_enable_chn failed\n", ret);
    }

    ret = hi_mpi_dpu_match_start_grp(dpu_match_grp);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_match_start_grp failed\n", ret);

    return HI_SUCCESS;
}

/* function : stop dpu match grp */
hi_s32 sample_common_svp_dpu_match_stop(hi_dpu_match_grp dpu_match_grp)
{
    hi_s32 j;
    hi_s32 ret;
    hi_dpu_match_chn dpu_match_chn;

    sample_svp_dpu_check_exps_return(((dpu_match_grp < 0) || (dpu_match_grp >= HI_DPU_MATCH_MAX_GRP_NUM)),
        HI_ERR_DPU_MATCH_ILLEGAL_PARAM, "Err, dpu_match_grp(%d) must be in [0,%d)!\n",
        dpu_match_grp, HI_DPU_MATCH_MAX_GRP_NUM);

    ret = hi_mpi_dpu_match_stop_grp(dpu_match_grp);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_match_stop_grp failed\n", ret);

    for (j = 0; j < HI_DPU_MATCH_MAX_CHN_NUM; j++) {
        dpu_match_chn = j;
        ret = hi_mpi_dpu_match_disable_chn(dpu_match_grp, dpu_match_chn);
        sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
            "Err(%#x), hi_mpi_dpu_match_disable_chn failed\n", ret);
    }

    ret = hi_mpi_dpu_match_destroy_grp(dpu_match_grp);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, ret,
        "Err(%#x), hi_mpi_dpu_match_destroy_grp failed\n", ret);

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_dpu_rect_bind_match(hi_dpu_rect_grp dpu_rect_grp,
    hi_dpu_match_grp dpu_match_grp)
{
    hi_s32 ret;
    hi_mpp_chn src_chn;
    hi_mpp_chn dst_chn;

    sample_svp_dpu_check_exps_return(((dpu_rect_grp < 0) || (dpu_rect_grp >= HI_DPU_RECT_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_grp(%d) must be in [0,%d)!\n",
        dpu_rect_grp, HI_DPU_MATCH_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(((dpu_match_grp < 0) || (dpu_match_grp >= HI_DPU_MATCH_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_match_grp(%d) must be in [0,%d)!\n",
        dpu_match_grp, HI_DPU_MATCH_MAX_GRP_NUM);

    src_chn.mod_id = HI_ID_DPU_RECT;
    src_chn.dev_id = dpu_rect_grp;
    src_chn.chn_id  = 0;

    dst_chn.mod_id  = HI_ID_DPU_MATCH;
    dst_chn.dev_id = dpu_match_grp;
    dst_chn.chn_id = 0;

    ret = hi_mpi_sys_bind(&src_chn, &dst_chn);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "Err(%#x), hi_mpi_sys_bind(RECT-MATCH) failede!\n", ret);

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_dpu_rect_unbind_match(hi_dpu_rect_grp dpu_rect_grp,
    hi_dpu_match_grp dpu_match_grp)
{
    hi_s32 ret;
    hi_mpp_chn src_chn;
    hi_mpp_chn dst_chn;

    sample_svp_dpu_check_exps_return(((dpu_rect_grp < 0) || (dpu_rect_grp >= HI_DPU_RECT_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_grp(%d) must be in [0,%d)!\n",
        dpu_rect_grp, HI_DPU_MATCH_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(((dpu_match_grp < 0) || (dpu_match_grp >= HI_DPU_MATCH_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_match_grp(%d) must be in [0,%d)!\n",
        dpu_match_grp, HI_DPU_MATCH_MAX_GRP_NUM);

    src_chn.mod_id  = HI_ID_DPU_RECT;
    src_chn.dev_id = dpu_rect_grp;
    src_chn.chn_id = 0;

    dst_chn.mod_id  = HI_ID_DPU_MATCH;
    dst_chn.dev_id = dpu_match_grp;
    dst_chn.chn_id = 0;

    ret = hi_mpi_sys_unbind(&src_chn, &dst_chn);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "Err(%#x), hi_mpi_sys_unbind(RECT-MATCH) failede!\n", ret);

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_dpu_vpss_bind_rect(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn,
    hi_dpu_rect_grp dpu_rect_grp, hi_dpu_rect_pipe dpu_rect_pipe)
{
    hi_s32 ret;
    hi_mpp_chn src_chn;
    hi_mpp_chn dst_chn;

    sample_svp_dpu_check_exps_return(((vpss_grp < 0) || (vpss_grp >= HI_VPSS_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, vpss_grp(%d) must be in [0,%d)!\n",
        vpss_grp, HI_VPSS_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(((vpss_chn < 0) || (vpss_chn >= HI_VPSS_MAX_CHN_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, vpss_chn(%d) must be in [0,%d)!\n",
        vpss_chn, HI_VPSS_MAX_CHN_NUM);

    sample_svp_dpu_check_exps_return(((dpu_rect_grp < 0) || (dpu_rect_grp >= HI_DPU_RECT_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_grp(%d) must be in [0,%d)!\n",
        dpu_rect_grp, HI_DPU_MATCH_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(((dpu_rect_pipe < 0) || (dpu_rect_pipe >= HI_DPU_RECT_MAX_PIPE_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_pipe(%d) must be in [0,%d)!\n",
        dpu_rect_pipe, HI_DPU_RECT_MAX_PIPE_NUM);

    src_chn.mod_id   = HI_ID_VPSS;
    src_chn.dev_id  = vpss_grp;
    src_chn.chn_id  = vpss_chn;

    dst_chn.mod_id  = HI_ID_DPU_RECT;
    dst_chn.dev_id = dpu_rect_grp;
    dst_chn.chn_id = dpu_rect_pipe;

    ret = hi_mpi_sys_bind(&src_chn, &dst_chn);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "Err(%#x), hi_mpi_sys_unbind(VPSS-RECT) failede!\n", ret);

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_dpu_vpss_unbind_rect(hi_vpss_grp vpss_grp, hi_vpss_chn vpss_chn,
    hi_dpu_rect_grp dpu_rect_grp, hi_dpu_rect_pipe dpu_rect_pipe)
{
    hi_s32 ret;
    hi_mpp_chn src_chn;
    hi_mpp_chn dst_chn;

    sample_svp_dpu_check_exps_return(((vpss_grp < 0) || (vpss_grp >= HI_VPSS_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, vpss_grp(%d) must be in [0,%d)!\n",
        vpss_grp, HI_VPSS_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(((vpss_chn < 0) || (vpss_chn >= HI_VPSS_MAX_CHN_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, vpss_chn(%d) must be in [0,%d)!\n",
        vpss_chn, HI_VPSS_MAX_CHN_NUM);

    sample_svp_dpu_check_exps_return(((dpu_rect_grp < 0) || (dpu_rect_grp >= HI_DPU_RECT_MAX_GRP_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_grp(%d) must be in [0,%d)!\n",
        dpu_rect_grp, HI_DPU_MATCH_MAX_GRP_NUM);
    sample_svp_dpu_check_exps_return(((dpu_rect_pipe < 0) || (dpu_rect_pipe >= HI_DPU_RECT_MAX_PIPE_NUM)),
        HI_ERR_DPU_RECT_ILLEGAL_PARAM, "Err, dpu_rect_pipe(%d) must be in [0,%d)!\n",
        dpu_rect_pipe, HI_DPU_RECT_MAX_PIPE_NUM);

    src_chn.mod_id  = HI_ID_VPSS;
    src_chn.dev_id = vpss_grp;
    src_chn.chn_id = vpss_chn;

    dst_chn.mod_id  = HI_ID_DPU_RECT;
    dst_chn.dev_id = dpu_rect_grp;
    dst_chn.chn_id = dpu_rect_grp;

    ret = hi_mpi_sys_unbind(&src_chn, &dst_chn);
    sample_svp_dpu_check_exps_return(ret != HI_SUCCESS, HI_ERR_DPU_RECT_ILLEGAL_PARAM,
        "Err(%#x), hi_mpi_sys_unbind(VPSS-RECT) failede!\n", ret);

    return HI_SUCCESS;
}

hi_s32 sample_common_svp_dpu_get_fg_aggregate_coef_and_unique_ratio(
    sample_svp_dpu_match_density_accuracy_mode density_accuracy_mode, hi_u8 *aggregate_coef, hi_u8 *unique_ratio)
{
    sample_svp_dpu_check_exps_return((aggregate_coef == HI_NULL) || (unique_ratio == HI_NULL),
        HI_ERR_DPU_MATCH_NULL_PTR, "aggregate_coef or unique_ratio can't be NULL\n");

    switch (density_accuracy_mode) {
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D0_A11: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D0_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A0_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D1_A10: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D1_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A0_UNIQ_RATIO;
             break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D2_A9: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D0_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D3_A8: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D1_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D4_A7: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D0_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D5_A6: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D2_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A0_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D6_A5: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D1_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D7_A4: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D2_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D8_A3: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D2_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D9_A2: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D3_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A0_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D10_A1: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D3_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A1_UNIQ_RATIO;
            break;
        }
        case SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D11_A0: {
            *aggregate_coef = SAMPLE_SVP_DPU_MATCH_D3_AGGREGATE_COEFF;
            *unique_ratio = SAMPLE_SVP_DPU_MATCH_A2_UNIQ_RATIO;
            break;
        }
        default: {
            sample_svp_dpu_trace_err("Err, density_accuracy_mode(%d) must be [%d, %d)\n", density_accuracy_mode,
                SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_D0_A11, SAMPLE_SVP_DPU_MATCH_DENSITY_ACCURACY_MODE_BUTT);
            return HI_ERR_DPU_RECT_ILLEGAL_PARAM;
        }
    }
    return HI_SUCCESS;
}
