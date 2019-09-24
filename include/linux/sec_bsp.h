#ifndef SEC_BSP_H
#define SEC_BSP_H

#ifdef CONFIG_SEC_BSP

#include <linux/dma-mapping.h>
#include <linux/miscdevice.h>

#define DEVICE_INIT_TIME_100MS		100000

struct device_init_time_entry {
	struct list_head next;
	char *buf;
	unsigned long long duration;
};

#define MAX_LENGTH_OF_SYSTEMSERVER_LOG 90
struct systemserver_init_time_entry {
	struct list_head next;
	char buf[MAX_LENGTH_OF_SYSTEMSERVER_LOG];
};


/* init/main.c */
extern struct list_head device_init_time_list;

/* [[BEGIN>> drivers/soc/qcom/boot_stat.c */
extern uint32_t bs_linuxloader_start;
extern uint32_t bs_linux_start;
extern uint32_t bs_uefi_start;
extern uint32_t bs_bootloader_load_kernel;
/* <<END]] drivers/soc/qcom/boot_stat.c */

extern unsigned int is_boot_recovery(void);
extern unsigned int get_boot_stat_time(void);
extern unsigned int get_boot_stat_freq(void);
extern void sec_boot_stat_add(const char * c);
extern void sec_bootstat_add_initcall(const char *s);
extern void sec_suspend_resume_add(const char * c);
extern void sec_bsp_enable_console(void);
extern bool sec_bsp_is_console_enabled(void);

extern unsigned int sec_hw_rev(void);

#else /* CONFIG_SEC_BSP */

#define is_boot_recovery()		(0)
#define get_boot_stat_time()
#define get_boot_stat_freq()
#define sec_boot_stat_add(c)
#define sec_bootstat_add_initcall(s)
#define sec_suspend_resume_add(c)
#define sec_bsp_enable_console()
#define sec_bsp_is_console_enabled()	(0)
#define sec_hw_rev()			(0)

#endif /* CONFIG_SEC_BSP */

#endif /* SEC_BSP_H */
