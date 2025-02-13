#ifndef OT_REG_READ_H
#define OT_REG_READ_H

#include "ot_type.h"
#include "osal_ioctl.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OT_REG_READ_DEV_NAME "ot_read_reg"

#define ot_unused(x) (void)(x)

typedef struct {
    unsigned int reg_addr;
    unsigned int reg_val;
} ot_reg_read_arg;

#define OT_REG_READ_IOCTL_BASE 'R'
#define IOC_NR_OT_REG_READ 0

#define OT_REG_READ_IOC_READ_REG    _IOWR(OT_REG_READ_IOCTL_BASE, IOC_NR_OT_REG_READ, ot_reg_read_arg)

int ot_reg_read_init(void);
void ot_reg_read_exit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __OT_REG_READ_H__ */