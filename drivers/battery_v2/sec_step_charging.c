/*
 *  sec_step_charging.c
 *  Samsung Mobile Battery Driver
 *
 *  Copyright (C) 2018 Samsung Electronics
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include "include/sec_battery.h"

#if defined(CONFIG_DIRECT_CHARGING)
#include "include/sec_direct_charger.h"
#endif

#define STEP_CHARGING_CONDITION_VOLTAGE			0x01
#define STEP_CHARGING_CONDITION_SOC				0x02
#define STEP_CHARGING_CONDITION_CHARGE_POWER 	0x04
#define STEP_CHARGING_CONDITION_ONLINE 			0x08
#define STEP_CHARGING_CONDITION_CURRENT_NOW		0x10
#define STEP_CHARGING_CONDITION_FLOAT_VOLTAGE	0x20
#define STEP_CHARGING_CONDITION_INPUT_CURRENT		0x40
#define STEP_CHARGING_CONDITION_MAX		0x43

#define DIRECT_CHARGING_FLOAT_VOLTAGE_MARGIN		20

void sec_bat_reset_step_charging(struct sec_battery_info *battery)
{
	pr_info("%s: called\n", __func__);
	battery->step_charging_status = -1;
#if defined(CONFIG_DIRECT_CHARGING)
	battery->pdata->direct_float_voltage_set = false;
#endif
}
/*
 * true: step is changed
 * false: not changed
 */
bool sec_bat_check_step_charging(struct sec_battery_info *battery)
{
	int i, value, step_vol, step_input, step_soc;
	int step = -1;

	union power_supply_propval val;

#if 0//defined(CONFIG_SEC_FACTORY)
	return false;
#endif

	if (!battery->step_charging_type)
		return false;

	if (battery->step_charging_type & STEP_CHARGING_CONDITION_CHARGE_POWER)
		if (battery->charge_power < battery->step_charging_charge_power)
			return false;

	if (battery->step_charging_type & STEP_CHARGING_CONDITION_ONLINE) {
#if defined(CONFIG_DIRECT_CHARGING_TEST_CODE)
		if (!is_pd_apdo_wire_type(battery->cable_type))
#else
		if (!is_hv_wire_type(battery->cable_type))
#endif
			return false;
	}

	if (battery->step_charging_status < 0)
		i = 0;
	else
		i = battery->step_charging_status;

	if (!(battery->step_charging_type & STEP_CHARGING_CONDITION_MAX))
		return false;

	if (battery->step_charging_type & STEP_CHARGING_CONDITION_VOLTAGE) {
		step_vol = i;
#if defined(CONFIG_DIRECT_CHARGING)
		value = battery->voltage_now;
#else
		value = battery->voltage_avg;
#endif
		while(step_vol < battery->step_charging_step - 1) {
			if (value + DIRECT_CHARGING_FLOAT_VOLTAGE_MARGIN <
				battery->pdata->step_charging_voltage_condition[step_vol])
				break;
			step_vol++;
			if (battery->step_charging_status != -1)
				break;
		}
		step = step_vol;
		pr_info("%s : step_vol = %d \n", __func__, step_vol);
	}
	
	if (battery->step_charging_type & STEP_CHARGING_CONDITION_INPUT_CURRENT) {
		step_input = i;
#if defined(CONFIG_DIRECT_CHARGING)
		psy_do_property(battery->pdata->charger_name, get,
			POWER_SUPPLY_EXT_PROP_DIRECT_CHARGER_MODE, val);
		if (val.intval != SEC_DIRECT_CHG_MODE_DIRECT_ON) {	
			pr_info("%s : dc no charging status = %d \n", __func__, val.intval);
			return false;
		}
		val.intval = SEC_BATTERY_IIN_MA;
		psy_do_property(battery->pdata->charger_name, get,
				POWER_SUPPLY_EXT_PROP_MEASURE_INPUT, val);
		pr_info("%s : iin = %d \n", __func__, val.intval);
		value = val.intval;
#else
		value = battery->current_now;
#endif
		
		while(step_input < battery->step_charging_step - 1) {
			if (value > battery->pdata->step_charging_condition[step_input])
				break;
			step_input++;
			if (battery->step_charging_status != -1)
				break;
		}
		pr_info("%s : step_input = %d \n", __func__, step_input);
#if defined(CONFIG_DIRECT_CHARGING)
		if ((battery->step_charging_status == -1) &&
			(battery->step_charging_type & STEP_CHARGING_CONDITION_VOLTAGE))
			step = step_vol;
		else
#endif
			if ((step_input < step) || (step < 0))
				step = step_input;
	}
		
	if (battery->step_charging_type & STEP_CHARGING_CONDITION_SOC) {
		step_soc = i;
		value = battery->capacity;
		while(step_soc < battery->step_charging_step - 1) {
			if (value < battery->pdata->step_charging_soc_condition[step_soc])
				break;
			step_soc++;
			if (battery->step_charging_status != -1)
				break;
		}
		pr_info("%s : step_soc = %d \n", __func__, step_soc);
#if defined(CONFIG_DIRECT_CHARGING)
		if ((battery->step_charging_status == -1) &&
			(battery->step_charging_type & STEP_CHARGING_CONDITION_VOLTAGE))
			step = step_vol;
		else
#endif
			if ((step_soc < step) || (step < 0))
				step = step_soc;
	}

	pr_info("%s : step = %d \n", __func__, step);
	if (step != battery->step_charging_status) {
#if defined(CONFIG_DIRECT_CHARGING)
		if ((battery->step_charging_type & STEP_CHARGING_CONDITION_INPUT_CURRENT) &&
			(battery->step_charging_status != -1)) {
			battery->pdata->current_cnt++;
		} else {
#endif
			pr_info("%s : cable(%d), prev=%d, new=%d, value=%d, current=%d\n",
				__func__, battery->cable_type,
				battery->step_charging_status, step, value, battery->pdata->step_charging_current[step]);
			battery->pdata->charging_current[battery->cable_type].fast_charging_current = battery->pdata->step_charging_current[step];
			battery->step_charging_status = step;

			if ((battery->step_charging_type & STEP_CHARGING_CONDITION_FLOAT_VOLTAGE) &&
				(battery->swelling_mode == SWELLING_MODE_NONE)) {

				pr_info("%s : float voltage = %d \n", __func__, battery->pdata->step_charging_float_voltage[step]);
				val.intval = battery->pdata->step_charging_float_voltage[step];
				psy_do_property(battery->pdata->charger_name, set,
					POWER_SUPPLY_PROP_VOLTAGE_MAX, val);
			}
		return true;
		}
#if defined(CONFIG_DIRECT_CHARGING)
	} else {
		battery->pdata->current_cnt = 0; 
	}
	if (battery->pdata->current_cnt >= 2) {
		pr_info("%s : cable(%d), prev=%d, new=%d, value=%d, current=%d\n",
				__func__, battery->cable_type,
			battery->step_charging_status, step, value, battery->pdata->step_charging_current[step]);
		battery->pdata->charging_current[battery->cable_type].fast_charging_current = battery->pdata->step_charging_current[step];
		battery->step_charging_status = step;

		if ((battery->step_charging_type & STEP_CHARGING_CONDITION_FLOAT_VOLTAGE) &&
			(battery->swelling_mode == SWELLING_MODE_NONE) &&
			(battery->step_charging_status != -1)) {
			pr_info("%s : step float voltage = %d \n", __func__, battery->pdata->step_charging_float_voltage[step]);
			battery->pdata->direct_float_voltage_set = true;
		}
		battery->pdata->current_cnt = 0; 
	}
#endif
	return false;
}

#if defined(CONFIG_BATTERY_AGE_FORECAST)
void sec_bat_set_aging_info_step_charging(struct sec_battery_info *battery)
{
	if (battery->step_charging_type & STEP_CHARGING_CONDITION_FLOAT_VOLTAGE)
		battery->pdata->step_charging_float_voltage[battery->step_charging_step-1] = battery->pdata->chg_float_voltage;
	battery->pdata->step_charging_condition[0] = 
		battery->pdata->age_data[battery->pdata->age_step].step_charging_condition;

	dev_info(battery->dev,
		 "%s: float_v(%d), step_conditon(%d)\n",
		 __func__,
		 battery->pdata->step_charging_float_voltage[battery->step_charging_step-1],
		 battery->pdata->step_charging_condition[0]);	
}
#endif

void sec_step_charging_init(struct sec_battery_info *battery, struct device *dev)
{
	struct device_node *np = dev->of_node;
	int ret, len;
	sec_battery_platform_data_t *pdata = battery->pdata;
	unsigned int i;
	const u32 *p;

	ret = of_property_read_u32(np, "battery,step_charging_type",
			&battery->step_charging_type);
	pr_err("%s: step_charging_type 0x%x\n", __func__, battery->step_charging_type);
	if (ret) {
		pr_err("%s: step_charging_type is Empty\n", __func__);
		battery->step_charging_type = 0;
		return;
	}
	ret = of_property_read_u32(np, "battery,step_charging_charge_power",
			&battery->step_charging_charge_power);
	if (ret) {
		pr_err("%s: step_charging_charge_power is Empty\n", __func__);
		battery->step_charging_charge_power = 20000;
	}
	p = of_get_property(np, "battery,step_charging_condtion", &len);
	if (!p) {
		battery->step_charging_step = 0;
	} else {
		len = len / sizeof(u32);
		battery->step_charging_step = len;
		pdata->step_charging_condition = kzalloc(sizeof(u32) * len, GFP_KERNEL);
		ret = of_property_read_u32_array(np, "battery,step_charging_condtion",
				pdata->step_charging_condition, len);
		if (ret) {
			pr_info("%s : step_charging_condtion read fail\n", __func__);
			battery->step_charging_step = 0;
		} else {
			pdata->step_charging_float_voltage = kzalloc(sizeof(u32) * len, GFP_KERNEL);
			ret = of_property_read_u32_array(np, "battery,step_charging_float_voltage",
					pdata->step_charging_float_voltage, len);
			if (ret) {
				pr_info("%s : step_charging_float_voltage read fail\n", __func__);
			} else {
				for (i = 0; i < len; i++) {
					pr_info("%s : [%d]step condition(%d), float voltage(%d)\n",
					__func__, i, pdata->step_charging_condition[i],
					pdata->step_charging_float_voltage[i]);
				}
			}

			pdata->step_charging_current = kzalloc(sizeof(u32) * len, GFP_KERNEL);
			ret = of_property_read_u32_array(np, "battery,step_charging_current",
					pdata->step_charging_current, len);
			if (ret) {
				pr_info("%s : step_charging_current read fail\n", __func__);
				battery->step_charging_step = 0;
			} else {
				battery->step_charging_status = -1;
				for (i = 0; i < len; i++) {
					pr_info("%s : [%d]step condition(%d), current(%d)\n",
					__func__, i, pdata->step_charging_condition[i],
					pdata->step_charging_current[i]);
				}
			}
			pdata->step_charging_soc_condition = kzalloc(sizeof(u32) * len, GFP_KERNEL);
			ret = of_property_read_u32_array(np, "battery,step_charging_soc_condtion",
					pdata->step_charging_soc_condition, len);
			if (ret) {
				pr_info("%s : step_charging_soc_condtion read fail\n", __func__);
				battery->step_charging_step = 0;
			}

			pdata->step_charging_voltage_condition = kzalloc(sizeof(u32) * len, GFP_KERNEL);
			ret = of_property_read_u32_array(np, "battery,step_charging_float_voltage",
					pdata->step_charging_voltage_condition, len);
			if (ret) {
				pr_info("%s : step_charging_voltage_condtion read fail\n", __func__);
				battery->step_charging_step = 0;
			}
		}
	}
}
