/* drivers/debug/sec_debug_internal.h */

#ifndef __SEC_DEBUG_INTERNAL_H__
#define __SEC_DEBUG_INTERNAL_H__

/* [[BEGIN>> sec_debug.c */
extern void dump_all_task_info(void);
extern void dump_cpu_stat(void);
/* <<END]] sec_debug.c */

/* [[BEGIN>> sec_debug_sched_log.c */
extern struct sec_debug_log *secdbg_log;
extern phys_addr_t secdbg_paddr;
extern size_t secdbg_size;
/* <<END]] sec_debug_sched_log.c */

/* out-of-sec_debug */

/* drivers/power/reset/msm-poweroff.c */
extern void set_dload_mode(int on);

/* kernel/init/version.c */
#include <linux/utsname.h>
extern struct uts_namespace init_uts_ns;

/* drivers/base/core.c */
extern struct kset *devices_kset;

/* kernel/printk/printk.c */
extern unsigned int get_sec_log_idx(void);

/* fake pr_xxx macros to prevent checkpatch fails */
#define __printx	printk
#define __pr_info(fmt, ...) \
	__printx(KERN_INFO fmt, ##__VA_ARGS__)
#define __pr_err(fmt, ...) \
	__printx(KERN_ERR fmt, ##__VA_ARGS__)

#endif /* __SEC_DEBUG_INTERNAL_H__ */
