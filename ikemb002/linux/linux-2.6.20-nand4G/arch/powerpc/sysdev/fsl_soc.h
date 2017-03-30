#ifndef __PPC_FSL_SOC_H
#define __PPC_FSL_SOC_H
#ifdef __KERNEL__

#include <asm/mmu.h>

extern phys_addr_t get_immrbase(void);
extern u32 get_brgfreq(void);
extern u32 get_baudrate(void);
extern u32 fsl_get_sys_freq(void);

struct fsl_sleep_platform_data;
struct device_node;

int fsl_sleep_init(struct fsl_sleep_platform_data *sleep,
			struct device_node *node);

/* Calls to fsl_sleep_dev and fsl_wake_dev cannot be nested. */
void fsl_sleep_dev(struct fsl_sleep_platform_data *sleep);
void fsl_wake_dev(struct fsl_sleep_platform_data *sleep);

#endif
#endif
