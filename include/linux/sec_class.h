#ifndef __SEC_CLASS_H__
#define __SEC_CLASS_H__

#ifdef CONFIG_DRV_SAMSUNG
extern struct device *sec_dev_get_by_name(char *name);
extern struct device *sec_device_create(dev_t devt,
			void *drvdata, const char *fmt);
extern void sec_device_destroy(dev_t devt);
#else
#define sec_dev_get_by_name(name)		NULL
#define sec_device_create(devt, drvdata, fmt)	NULL
#define sec_device_destroy(devt)		NULL
#endif /* CONFIG_DRV_SAMSUNG */

#endif	/* __SEC_CLASS_H__ */
