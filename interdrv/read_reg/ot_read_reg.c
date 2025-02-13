#include "ot_read_reg.h"
#include "ot_osal.h"


#include "memmap.h"

#ifndef NULL
#define NULL  ((void *)0)
#endif

#define reg_read_print(fmt, ...) osal_printk("Func:%s, Line:%d, "fmt"\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

static osal_dev *g_reg_read_device = NULL;

static int reg_read_do_ioctl(unsigned int cmd, void *arg, void *private_data)
{
    ot_reg_read_arg *reg_arg = (ot_reg_read_arg *)arg;
    ot_unused(private_data);

    if (reg_arg == NULL) {
        reg_read_print("invalid arg.\n");
        return -1;
    }

    switch (cmd) {
        case OT_REG_READ_IOC_READ_REG:
            reg_arg->reg_val = *(volatile unsigned int *)osal_ioremap(reg_arg->reg_addr, 4); /* 4: reg size */
            osal_iounmap((void *)reg_arg->reg_addr, 4);
            break;

        default:
            reg_read_print("error cmd:%08x\n", cmd);
            return -1;
            break;
    }

    return 0;
}

static osal_ioctl_cmd g_ot_reg_read_ioctl_cmd_list[] = {
    { OT_REG_READ_IOC_READ_REG, reg_read_do_ioctl },
};

static osal_fileops g_ot_reg_read_fops = {
    .cmd_list = g_ot_reg_read_ioctl_cmd_list,
    .cmd_cnt  = sizeof(g_ot_reg_read_ioctl_cmd_list) / sizeof(g_ot_reg_read_ioctl_cmd_list[0]),
};

static int reg_read_device_init(void)
{
    g_reg_read_device = osal_dev_create(OT_REG_READ_DEV_NAME);
    if (g_reg_read_device == NULL) {
        osal_printk("fail to create dev\n");
        printk("fail to create dev\n");
        return -1;
    }
    g_reg_read_device->minor = 254; /* dev_minor 254 */
    g_reg_read_device->fops = &g_ot_reg_read_fops;
    if (osal_dev_register(g_reg_read_device) != 0) {
        reg_read_print("register device error.\n");
        osal_dev_destroy(g_reg_read_device);
        return -1;
    }

    return 0;
}

int ot_reg_read_init(void)
{
    return reg_read_device_init();
}

void ot_reg_read_exit(void)
{
    osal_dev_unregister(g_reg_read_device);
    osal_dev_destroy(g_reg_read_device);
}