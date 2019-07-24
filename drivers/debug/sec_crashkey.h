/* drivers/debug/sec_crashkey.h */

#ifndef SEC_CRASHKEY_H
#define SEC_CRASHKEY_H

#define MAX_NAME_SIZE		64

#define SEC_CRASHKEY_SHORTKEY	0
#define SEC_CRASHKEY_LONGKEY	1

struct sec_crash_key {
	char name[MAX_NAME_SIZE];		/* name */
	unsigned int *keycode;			/* keycode array */
	unsigned int size;			/* number of used keycode */
	unsigned int timeout;			/* msec timeout */
	unsigned long long_keypress;		/* trigger by pressing key combination longger */
	unsigned int unlock;			/* unlocking mask value */
	unsigned int trigger;			/* trigger key code */
	unsigned int knock;			/* number of triggered */
	void (*callback)(unsigned long);	/* callback function when the key triggered */
	struct list_head node;
};

#ifdef CONFIG_SEC_DEBUG
extern int sec_debug_register_crash_key(struct sec_crash_key *crash_key);

#ifdef CONFIG_INPUT_QPNP_POWER_ON
extern int qpnp_control_s2_reset_onoff(int on);
#else
static int qpnp_control_s2_reset_onoff(int on) { return 0; };
#endif

#else
static inline int sec_debug_register_crash_key(
				struct sec_crash_key *crash_key) { }
#endif /* CONFIG_SEC_DEBUG */

#endif /* SEC_CRASHKEY_H */
