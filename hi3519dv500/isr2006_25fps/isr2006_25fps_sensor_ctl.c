/*
  Copyright (c), 2001-2022, Shenshu Tech. Co., Ltd.
 */

#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#ifdef OT_GPIO_I2C
#include "gpioi2c_ex.h"
#else
#include "ot_i2c.h"
#endif
#include "securec.h"

#include "isr2006_25fps_cmos.h"

#define I2C_DEV_FILE_NUM     16
#define I2C_BUF_NUM          8

static int g_fd[OT_ISP_MAX_PIPE_NUM] = {[0 ...(OT_ISP_MAX_PIPE_NUM - 1)] = -1};

int isr2006_i2c_init(ot_vi_pipe vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) { //表明已经完成了i2c init 不需要后续操作
        return TD_SUCCESS;
    }
#ifdef OT_GPIO_I2C
    g_fd[vi_pipe] = open("/dev/gpioi2c_ex", O_RDONLY, S_IRUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open gpioi2c_ex error!\n");
        return TD_FAILURE;
    }
#else
    int ret;
    char dev_file[I2C_DEV_FILE_NUM] = {0};
    td_u8 dev_num;
    ot_isp_sns_commbus *isr2006businfo = TD_NULL;
    isr2006businfo = isr2006_get_bus_info(vi_pipe);
    dev_num = isr2006businfo->i2c_dev;
    (td_void)snprintf_s(dev_file, sizeof(dev_file), sizeof(dev_file) - 1, "/dev/i2c-%u", dev_num);// /dev/i2c-0

    g_fd[vi_pipe] = open(dev_file, O_RDWR, S_IRUSR | S_IWUSR);
    if (g_fd[vi_pipe] < 0) {
        isp_err_trace("Open /dev/ot_i2c_drv-%u error!\n", dev_num);
        return TD_FAILURE;
    }

    ret = ioctl(g_fd[vi_pipe], OT_I2C_SLAVE_FORCE, (ISR2006_I2C_ADDR)); // 设定i2c地址 将i2c操作 转化为对文件的读写操作 
    if (ret < 0) {
        isp_err_trace("I2C_SLAVE_FORCE error!\n");
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return ret;
    }
#endif

    return TD_SUCCESS;
}

int isr2006_i2c_exit(ot_vi_pipe vi_pipe)
{
    if (g_fd[vi_pipe] >= 0) {
        close(g_fd[vi_pipe]);
        g_fd[vi_pipe] = -1;
        return TD_SUCCESS;
    }
    return TD_FAILURE;
}

td_s32 isr2006_read_register(ot_vi_pipe vi_pipe, td_u32 addr)
{
    ot_unused(vi_pipe);
    ot_unused(addr);
    return TD_SUCCESS;
}

td_s32 isr2006_write_register(ot_vi_pipe vi_pipe, td_u32 addr, td_u32 data)
{
    if (g_fd[vi_pipe] < 0) {
        return TD_SUCCESS;
    }

#ifdef OT_GPIO_I2C
    i2c_data.dev_addr = ISR2006_I2C_ADDR;
    i2c_data.reg_addr = addr;
    i2c_data.addr_byte_num = ISR2006_ADDR_BYTE;
    i2c_data.data = data;
    i2c_data.data_byte_num = ISR2006_DATA_BYTE;

    ret = ioctl(g_fd[vi_pipe], GPIO_I2C_WRITE, &i2c_data);
    if (ret) {
        isp_err_trace("GPIO-I2C write failed!\n");
        return ret;
    }
#else
    td_u32 idx = 0;
    td_s32 ret;
    td_u8 buf[I2C_BUF_NUM];

    if (ISR2006_ADDR_BYTE == 2) {  /* 2 byte */
        buf[idx] = (addr >> 8) & 0xff;  /* shift 8 */
        idx++;
        buf[idx] = addr & 0xff;
        idx++;
    } else {
    }

    if (ISR2006_DATA_BYTE == 2) {  /* 2 byte */
    } else {
        buf[idx] = data & 0xff;
        idx++;
    }

    ret = write(g_fd[vi_pipe], buf, ISR2006_ADDR_BYTE + ISR2006_DATA_BYTE);
    if (ret < 0) {
        isp_err_trace("I2C_WRITE error!\n");
        return TD_FAILURE;
    }

#endif
    return TD_SUCCESS;
}

static void delay_ms(int ms)
{
    usleep(ms * 1000); /* 1ms: 1000us */
    return;
}

void isr2006_prog(ot_vi_pipe vi_pipe, const td_u32 *rom)
{
    ot_unused(vi_pipe);
    ot_unused(rom);
}

void isr2006_standby(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x00);  /* STANDBY */
    // ret += isr2006_write_register(vi_pipe,0X00a5,0x00);
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}

void isr2006_restart(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x01);  /* standby */
    // ret += isr2006_write_register(vi_pipe,0X00a5,0x01);
    delay_ms(20); /* 20ms */
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}

void isr2006_mirror_flip(ot_vi_pipe vi_pipe, ot_isp_sns_mirrorflip_type sns_mirror_flip)
{
    switch (sns_mirror_flip) {
        case ISP_SNS_NORMAL:
            isr2006_write_register(vi_pipe, 0x3030, 0x00);
            break;
        case ISP_SNS_MIRROR:
            isr2006_write_register(vi_pipe, 0x3030, 0x01);
            break;
        case ISP_SNS_FLIP:
            isr2006_write_register(vi_pipe, 0x3030, 0x02);
            break;
        case ISP_SNS_MIRROR_FLIP:
            isr2006_write_register(vi_pipe, 0x3030, 0x03);
            break;
        default:
            break;
    }
    return;
}

td_void isr2006_blc_clamp(ot_vi_pipe vi_pipe, ot_isp_sns_blc_clamp blc_clamp)
{
    td_s32 ret = TD_SUCCESS;

    isr2006_set_blc_clamp_value(vi_pipe, blc_clamp.blc_clamp_en);

    if (blc_clamp.blc_clamp_en == TD_TRUE) {
        ret += isr2006_write_register(vi_pipe, 0x00e2, 0x01);
    } else {
        ret += isr2006_write_register(vi_pipe, 0x00e2, 0x00);
    }

    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}

//void isr2006_comm_init(ot_vi_pipe vi_pipe);
void isr2006_linear_1936_1280_init(ot_vi_pipe vi_pipe);

void isr2006_default_reg_init(ot_vi_pipe vi_pipe)
{
    td_u32 i;
    td_s32 ret = TD_SUCCESS;
    ot_isp_sns_state *pastisr2006 = TD_NULL;
    pastisr2006 = isr2006_get_ctx(vi_pipe);
    
    isp_err_trace("reg_num = %d\n",pastisr2006->regs_info[0].reg_num); 

    for (i = 0; i < pastisr2006->regs_info[0].reg_num; i++) {
        ret += isr2006_write_register(vi_pipe,
                                     pastisr2006->regs_info[0].i2c_data[i].reg_addr,
                                     pastisr2006->regs_info[0].i2c_data[i].data);
    }
    if (ret != TD_SUCCESS) {
        isp_err_trace("write register failed!\n");
    }
    return;
}







void isr2006_init(ot_vi_pipe vi_pipe)
{
    printf("[%s]-%d: ", __FUNCTION__, __LINE__);
    ot_wdr_mode wdr_mode;
    td_bool init;
    td_s32 ret;
    ot_isp_sns_state *pastisr2006 = TD_NULL;
    pastisr2006 = isr2006_get_ctx(vi_pipe);
    init       = pastisr2006->init;
    wdr_mode   = pastisr2006->wdr_mode;

    isp_err_trace("isr2006_init start\n");

    ret = isr2006_i2c_init(vi_pipe); // 初始化i2c,打开一个文件，并为它设定i2c地址，后续对i2c的操作通过读写文件完成
    if (ret != TD_SUCCESS) {
        isp_err_trace("i2c init failed!\n");
        return;
    }
    /* When sensor first init, config all registers */
    if (init == TD_FALSE) {
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
        } else {
            isr2006_linear_1936_1280_init(vi_pipe);// 初始化了时钟 输出模式为mipi 输出格式为12bit等
        }
    } else {
        /* When sensor switch mode(linear<->WDR or resolution), config different registers(if possible) */
        if (OT_WDR_MODE_2To1_LINE == wdr_mode) {
        } else {
            isr2006_linear_1936_1280_init(vi_pipe);
        }
    }

    pastisr2006->init = TD_TRUE;
    return;
}

void isr2006_exit(ot_vi_pipe vi_pipe)
{
    printf("[%s]-%d: \n", __FUNCTION__, __LINE__);
    td_s32 ret;
    ret = isr2006_i2c_exit(vi_pipe);
    if (ret != TD_SUCCESS) {
        isp_err_trace("ISR2006 exit failed!\n");
    }
    return;
}




static td_s32 isr2006_gpio_rst(td_u32 gpio_chip_num,td_u32 gpio_offset_num)
{
    FILE* fp;
    td_u32 gpio_num = gpio_chip_num * 8 + gpio_offset_num;
    char file_name[200] = {0};
    char buf[10] = {0};

    sprintf(file_name, "/sys/class/gpio/export");
    fp = fopen(file_name, "w");
    if (fp == NULL) {
        printf("Cannot open %s.\n", file_name);
        return -1;
    }
    fprintf(fp, "%d", gpio_num);
    fclose(fp);

    sprintf(file_name, "/sys/class/gpio/gpio%d/direction", gpio_num);
    fp = fopen(file_name,"rb+");
    if (fp == NULL) {
        printf("Cannot open %s.\n", file_name);
        return -1;
    }
    fprintf(fp, "out");
    fclose(fp);

    sprintf(file_name, "/sys/class/gpio/gpio%d/value", gpio_num);
    fp = fopen(file_name, "rb+");
    if (fp == NULL) {
        printf("Cannot open %s.\n", file_name);
        return -1;
    }
    strcpy(buf,"0");
    fwrite(buf, sizeof(char), sizeof(buf) - 1, fp);

    delay_ms(2);

    strcpy(buf,"1");
    fwrite(buf, sizeof(char), sizeof(buf) - 1, fp);
    fclose(fp);

    fp = fopen("/sys/class/gpio/unexport", "w");
    if (fp == NULL) {
        printf("Cannot open %s.\n", file_name);
        return -1;
    }
    fprintf(fp, "%d", gpio_num);
    fclose(fp);

    return 0;
}





static td_s32 isr2006_linear_1936_1280_init_part(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    //isr2006 的 初始化 mipi 30fps 1936*1280  12bit

    //ret += isr2006_gpio_rst(11,7);

    

    ret += isr2006_write_register(vi_pipe, 0x007f, 0x00);//pll_pd = 0 
    ret += isr2006_write_register(vi_pipe, 0x0082, 0x04);//pll_refdiv = 4
    ret += isr2006_write_register(vi_pipe, 0x0083, 0xc0);//pll_fbdiv = 0xc0
    ret += isr2006_write_register(vi_pipe, 0x0084, 0x05);//pll_div_adc = 5
    ret += isr2006_write_register(vi_pipe, 0x0086, 0x03);//pll_div_bitclk = 3
    ret += isr2006_write_register(vi_pipe, 0x0089, 0x05);//pll_div_pclk = 0x05
    ret += isr2006_write_register(vi_pipe, 0x008b, 0x05); //pll_div_cpclk = 0x05

    /*以上部分配置了pll 时钟 适用于 1936*1280 30fps 12bit mipi输出 模式*/

    //ret += isr2006_write_register(vi_pipe, 0x01df, 0x00); //

    ret += isr2006_write_register(vi_pipe, 0x01e0, 0x00);

   // ret += isr2006_write_register(vi_pipe, 0x01e1, 0x8f); //

    ret += isr2006_write_register(vi_pipe, 0x01e2, 0x07);

    ret += isr2006_write_register(vi_pipe, 0x00ac, (td_u8)ISR2006_HEIGHT); //高度 vmax 低位
    ret += isr2006_write_register(vi_pipe, 0x00ad, (td_u8)(ISR2006_HEIGHT>>8)); //高度 vmax 高位
    ret += isr2006_write_register(vi_pipe, 0x01e5, 0x00);//不开启dol模式


    ret += isr2006_write_register(vi_pipe, 0x01e6, 0x00);// 配置输出模式为mipi
    ret += isr2006_write_register(vi_pipe, 0x01e7, 0x01);// 配置输出 bit_mode = 12bit
    ret += isr2006_write_register(vi_pipe, 0x01e8, 0x01);// 配置输出 timing_mode = 12bit



    ret += isr2006_write_register(vi_pipe, 0x00a5, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x00a7, 0x00); // 曝光模式控制 0:内部寄存器曝光，1：外部控制曝光
    ret += isr2006_write_register(vi_pipe, 0x00a4, 0x00);

    ret += isr2006_write_register(vi_pipe, 0x00a9, 0x01); // 一次曝光触发输出的帧数量

    ret += isr2006_write_register(vi_pipe, 0x00aa, 0x00); 
    ret += isr2006_write_register(vi_pipe, 0x00ab, 0x2d); 

    ret += isr2006_write_register(vi_pipe, 0x008f, 0x08); //曝光时间 低位 8行
    ret += isr2006_write_register(vi_pipe, 0x0090, 0x00); //曝光时间 中间
    ret += isr2006_write_register(vi_pipe, 0x0091, 0x00); //曝光时间 高位

    // ret += isr2006_write_register(vi_pipe, 0x00dc, 0xc0);

    // ret += isr2006_write_register(vi_pipe, 0x00de, 0xc0);

    // ret += isr2006_write_register(vi_pipe, 0x00d0, 0x01);

    // ret += isr2006_write_register(vi_pipe, 0x00d3, 0x00);

    // ret += isr2006_write_register(vi_pipe, 0x01de, 0x00);


    // ret += isr2006_write_register(vi_pipe,0x0092,(1280 + 12 - 200 -1)&(0x00ff)); //reg_a
    // ret += isr2006_write_register(vi_pipe,0x0093,(1280 + 12- 200 -1)>>8); // reg_a

    ret += isr2006_write_register(vi_pipe, 0x00e2, 0x01); // 开启了内部黑电平矫正
    ret += isr2006_write_register(vi_pipe, 0x00e3, 0x01); // 开启了黑电平统计
    ret += isr2006_write_register(vi_pipe, 0x00e4, 0x01); // 开启了ABLC 中值滤波器


    // ret += isr2006_write_register(vi_pipe, 0x00d0, 0x00); // 关闭row_dark_en

    ret += isr2006_write_register(vi_pipe, 0x014f, 0x00); //
    ret += isr2006_write_register(vi_pipe, 0x0143, 0x00);
    /*彩条 模式 使能 */

    ret += isr2006_write_register(vi_pipe, 0x0150, 0x00); // 不使能坏点矫正
    ret += isr2006_write_register(vi_pipe, 0x01d7, 0x00); //不开启数字增益
    ret += isr2006_write_register(vi_pipe, 0x01d6, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x0058, 0x01);
    ret += isr2006_write_register(vi_pipe, 0x0059, 0x0f);
    ret += isr2006_write_register(vi_pipe, 0x005a, 0x01);
    ret += isr2006_write_register(vi_pipe, 0x005b, 0x03);
    ret += isr2006_write_register(vi_pipe, 0x1201, 0xf0);
    ret += isr2006_write_register(vi_pipe, 0x1202, 0x70);
    ret += isr2006_write_register(vi_pipe, 0x1203, 0x10);
    ret += isr2006_write_register(vi_pipe, 0x1204, 0x10);
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x02);
    ret += isr2006_write_register(vi_pipe, 0x1205, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1208, 0x01);
    ret += isr2006_write_register(vi_pipe, 0x1000, 0x10);
    ret += isr2006_write_register(vi_pipe, 0x1001, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x12);
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x02);

    ret += isr2006_write_register(vi_pipe, 0x1024, (td_u8)ISR2006_WIDTH); //
    ret += isr2006_write_register(vi_pipe, 0x1025, (td_u8)(ISR2006_WIDTH>>8));
    ret += isr2006_write_register(vi_pipe, 0x1026, (td_u8)ISR2006_HEIGHT);
    ret += isr2006_write_register(vi_pipe, 0x1027, (td_u8)(ISR2006_HEIGHT>>8));
    ret += isr2006_write_register(vi_pipe, 0x1040, 0x8d);
    ret += isr2006_write_register(vi_pipe, 0x1020, 0x2a);
    ret += isr2006_write_register(vi_pipe, 0x1042, 0x0f);
    ret += isr2006_write_register(vi_pipe, 0x1028, (td_u8)ISR2006_WIDTH);
    ret += isr2006_write_register(vi_pipe, 0x1029, (td_u8)(ISR2006_WIDTH>>8));
    ret += isr2006_write_register(vi_pipe, 0x102a, (td_u8)ISR2006_HEIGHT);
    ret += isr2006_write_register(vi_pipe, 0x102b, (td_u8)(ISR2006_HEIGHT>>8));
    ret += isr2006_write_register(vi_pipe, 0x102c, (td_u8)ISR2006_WIDTH);
    ret += isr2006_write_register(vi_pipe, 0x102d, (td_u8)(ISR2006_WIDTH>>8));
    ret += isr2006_write_register(vi_pipe, 0x102e, (td_u8)ISR2006_HEIGHT);
    ret += isr2006_write_register(vi_pipe, 0x102f, (td_u8)(ISR2006_HEIGHT>>8));
    ret += isr2006_write_register(vi_pipe, 0x1030, (td_u8)ISR2006_WIDTH);
    ret += isr2006_write_register(vi_pipe, 0x1031, (td_u8)(ISR2006_WIDTH>>8));
    ret += isr2006_write_register(vi_pipe, 0x1032, (td_u8)ISR2006_HEIGHT);
    ret += isr2006_write_register(vi_pipe, 0x1033, (td_u8)(ISR2006_HEIGHT>>8));
    ret += isr2006_write_register(vi_pipe, 0x01e3, 0x1e);

    ret += isr2006_write_register(vi_pipe, 0x00a4, 0x01);// 启用连续帧模式 

    // ret += isr2006_write_register(vi_pipe, 0x1040, 0x8c);
    ret += isr2006_write_register(vi_pipe, 0x1040, 0x8d);
    ret += isr2006_write_register(vi_pipe, 0x005b, 0x03);
    ret += isr2006_write_register(vi_pipe, 0x005d, 0x0f);
    ret += isr2006_write_register(vi_pipe, 0x0042, 0xaa);
    ret += isr2006_write_register(vi_pipe, 0x009b, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x009c, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x009d, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x009e, 0x00);

    ret += isr2006_write_register(vi_pipe, 0x00a5, 0x01); //寄存器曝光使能

    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x01); //寄存器曝光触发信号  00a6 写0停流 写1再触发
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x00); //寄存器曝光触发信号
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x01);
    ret += isr2006_write_register(vi_pipe, 0x0261, 0x0f);
    ret += isr2006_write_register(vi_pipe, 0x0262, 0x81);


    ret += isr2006_write_register(vi_pipe, 0x002b, 0x70);
    ret += isr2006_write_register(vi_pipe, 0x002c, 0x01);
    ret += isr2006_write_register(vi_pipe, 0x0030, 0x32);

    /*上述三行是模拟增益*/

    ret += isr2006_write_register(vi_pipe, 0x01d7, 0x01);// 开启数字增益

    ret += isr2006_write_register(vi_pipe, 0x01d8, 0x00);//数字增益倍率 粗调 1倍

    ret += isr2006_write_register(vi_pipe, 0x0045, 0x88);
    ret += isr2006_write_register(vi_pipe, 0x0046, 0x7d);
    ret += isr2006_write_register(vi_pipe, 0x0150, 0x01);
    ret += isr2006_write_register(vi_pipe, 0x0090, (td_u8)(ISR2006_HEIGHT>>8));
    ret += isr2006_write_register(vi_pipe, 0x008f, (td_u8)ISR2006_HEIGHT);
    ret += isr2006_write_register(vi_pipe, 0x0027, 0x01);
    ret += isr2006_write_register(vi_pipe, 0x0025, 0x08);

    ret += isr2006_write_register(vi_pipe, 0x01e9, 0x01);//配置曝光时间精度为1行

    return ret;
}


static td_s32 isr_linear_mipi_time(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;

    ret += isr2006_write_register(vi_pipe, 0x1000, 0x10);
    ret += isr2006_write_register(vi_pipe, 0x1001, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1002, 0x50);  //80d
    ret += isr2006_write_register(vi_pipe, 0x1003, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1004, 0xdc); //220d  clk _zero 默认
    //ret += isr2006_write_register(vi_pipe, 0x1004, 0xff); //255d  clk _zero 
    ret += isr2006_write_register(vi_pipe, 0x1005, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1006, 0x3c); //60d
    ret += isr2006_write_register(vi_pipe, 0x1007, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1008, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1009, 0x08); //8d
    ret += isr2006_write_register(vi_pipe, 0x100a, 0x3c); //60d
    ret += isr2006_write_register(vi_pipe, 0x100b, 0x34); //52d
    ret += isr2006_write_register(vi_pipe, 0x100c, 0x28); //40d
    ret += isr2006_write_register(vi_pipe, 0x100d, 0x04); // 4d
    ret += isr2006_write_register(vi_pipe, 0x100e, 0x69); //105d hs_zero 默认

    //ret += isr2006_write_register(vi_pipe, 0x100e, 0xdc); //200d hs_zero

    ret += isr2006_write_register(vi_pipe, 0x100f, 0x06); //6d
    ret += isr2006_write_register(vi_pipe, 0x1010, 0x3c); // 60d
    ret += isr2006_write_register(vi_pipe, 0x1011, 0x04); // 4d
    ret += isr2006_write_register(vi_pipe, 0x1012, 0x64); // 100d
    ret += isr2006_write_register(vi_pipe, 0x1013, 0x00); 
    ret += isr2006_write_register(vi_pipe, 0x1014, 0x32); // 50d
    ret += isr2006_write_register(vi_pipe, 0x1015, 0x00);
    ret += isr2006_write_register(vi_pipe, 0x1016, 0x07); // 7d

    return ret;

}


td_s32 isr2006_linear_1936_1280_25fps_init_part(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    ret += isr2006_write_register(vi_pipe, 0x007f, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0082, 0x04); // 4d
    ret += isr2006_write_register(vi_pipe, 0x0083, 0xa0); // 160d
    ret += isr2006_write_register(vi_pipe, 0x0084, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x0086, 0x03); // 3d
    ret += isr2006_write_register(vi_pipe, 0x0089, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x008b, 0x05); // 5d
    // 0x01df,0x03 //diff
    ret += isr2006_write_register(vi_pipe, 0x01e0, 0x00); // 0d
    // 0x01e1,0x8c //diff
    ret += isr2006_write_register(vi_pipe, 0x01e2, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x00ac, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00ad, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x01e5, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01e6, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01e7, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x01e8, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00a5, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a7, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a4, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a9, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00aa, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00ab, 0x2d); // 45d
    ret += isr2006_write_register(vi_pipe, 0x008f, 0x4f); // 79d
    ret += isr2006_write_register(vi_pipe, 0x0090, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0091, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0092, 0x20); // 32d
    ret += isr2006_write_register(vi_pipe, 0x0093, 0x00); // 0d           32 + exptime > 1280
    ret += isr2006_write_register(vi_pipe, 0x00e2, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x014f, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0150, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01d7, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x01d6, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0058, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x0059, 0x0f); // 15d
    ret += isr2006_write_register(vi_pipe, 0x005a, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x005b, 0x03); // 3d
    ret += isr2006_write_register(vi_pipe, 0x1201, 0xf0); // 240d
    ret += isr2006_write_register(vi_pipe, 0x1202, 0x70); // 112d
    ret += isr2006_write_register(vi_pipe, 0x1203, 0x10); // 16d
    ret += isr2006_write_register(vi_pipe, 0x1204, 0x10); // 16d
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x02); // 2d
    ret += isr2006_write_register(vi_pipe, 0x1205, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1208, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x1000, 0x10); // 16d
    ret += isr2006_write_register(vi_pipe, 0x1001, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x12); // 18d
    ret += isr2006_write_register(vi_pipe, 0x1070, 0x02); // 2d
    ret += isr2006_write_register(vi_pipe, 0x1024, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x1025, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x1026, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1027, 0x05); // 5d

    ret += isr2006_write_register(vi_pipe, 0x1040, 0x8d); // 141d

    ret += isr2006_write_register(vi_pipe, 0x1020, 0x2a); // 42d
    ret += isr2006_write_register(vi_pipe, 0x1042, 0x0f); // 15d
    ret += isr2006_write_register(vi_pipe, 0x1028, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x1029, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x102a, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x102b, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x102c, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x102d, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x102e, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x102f, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x1030, 0x90); // 144d
    ret += isr2006_write_register(vi_pipe, 0x1031, 0x07); // 7d
    ret += isr2006_write_register(vi_pipe, 0x1032, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x1033, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x01e3, 0x1e); // 30d
    ret += isr2006_write_register(vi_pipe, 0x00a4, 0x01); // 1d

    ret += isr2006_write_register(vi_pipe, 0x1040, 0x8d); // 140d

    ret += isr2006_write_register(vi_pipe, 0x005b, 0x03); // 3d
    ret += isr2006_write_register(vi_pipe, 0x005d, 0x0f); // 15d
    ret += isr2006_write_register(vi_pipe, 0x0042, 0xaa); // 170d
    ret += isr2006_write_register(vi_pipe, 0x009b, 0x00); // 0d // 9b 9c 9d 9e 写0为HCG 写1为LCG
    ret += isr2006_write_register(vi_pipe, 0x009c, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x009d, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x009e, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x00a5, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x00a6, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0261, 0x0f); // 15d //调整列条纹
    ret += isr2006_write_register(vi_pipe, 0x0262, 0x81); // 129d //调整列条纹
    ret += isr2006_write_register(vi_pipe, 0x002b, 0x70); // 112d
    ret += isr2006_write_register(vi_pipe, 0x002c, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x0030, 0x32); // 50d // 低帧率 30寄存器的值需要往大写
    ret += isr2006_write_register(vi_pipe, 0x01d7, 0x01); // 1d
    ret += isr2006_write_register(vi_pipe, 0x01d8, 0x03); // 3d
    ret += isr2006_write_register(vi_pipe, 0x0045, 0x88); // 136d //ADC的ADC_DRST值,对应时序1，12bit,tim_mode1
    ret += isr2006_write_register(vi_pipe, 0x0046, 0x7d); // 125d //ADC的ADC_DRST值,对应时序1，12bit,tim_mode1 
    ret += isr2006_write_register(vi_pipe, 0x0150, 0x01); // 1d //DPC 模块使能
    ret += isr2006_write_register(vi_pipe, 0x0090, 0x05); // 5d
    ret += isr2006_write_register(vi_pipe, 0x008f, 0x00); // 0d
    ret += isr2006_write_register(vi_pipe, 0x0027, 0x01); // 1d //矫正太阳黑子使能
    ret += isr2006_write_register(vi_pipe, 0x0025, 0x08); // 8d //调太阳黑子阈值 默认为08


    printf("===ISR2006 1936*1280 25fps 12bit LINE Init OK!===\n");
    return ret;
}

void isr2006_linear_1936_1280_init(ot_vi_pipe vi_pipe)
{
    td_s32 ret = TD_SUCCESS;
    // ot_isp_sns_state *pastisr2006 = TD_NULL;
    // pastisr2006 = isr2006_get_ctx(vi_pipe);
    // td_u8 img_mode = pastisr2006->img_mode ;

    // if(img_mode == ISR2006_SENSOR_1936_1280_30FPS_12BIT_LINEAR_MODE)
    // {
    //     ret += isr2006_linear_1936_1280_init_part(vi_pipe); 
    // }else if(img_mode == ISR2006_SENSOR_1936_1280_25FPS_12BIT_LINEAR_MODE)
    // {
    //     ret += isr2006_linear_1936_1280_25fps_init_part(vi_pipe);
    // }
//  ret += isr2006_linear_1936_1280_init_part(vi_pipe);


    ret += isr2006_linear_1936_1280_25fps_init_part(vi_pipe);


    
    //isr2006_default_reg_init(vi_pipe);

    //ret += isr2006_write_register(vi_pipe, 0x0140, 0x00);//不开启休眠
    
    if (ret != TD_SUCCESS) {
        isp_err_trace("isr2006 write register failed!\n");
        return;
    }
    printf("===ISR2006 1936*1280 25fps 12bit LINE Init OK!===\n");
    return;
}
