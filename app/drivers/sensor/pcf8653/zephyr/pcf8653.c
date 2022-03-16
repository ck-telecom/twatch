/*
 * Copyright (c) 2022 Qingsong Gou <gouqs@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define DT_DRV_COMPAT nxp_pcf8653

#include <zephyr.h>
#include <logging/log.h>

#include "pcf8653.h"

LOG_MODULE_REGISTER(pcf8653, CONFIG_SENSOR_LOG_LEVEL);

static int pcf8653_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	const struct pcf8653_config *cfg = dev->config;
	struct pcf8653_data *drv_data = dev->data;
	uint8_t data[7];
	int ret = 0;

	switch (chan) {
	case SENSOR_CHAN_ALL:
		ret = i2c_burst_read_dt(&cfg->i2c, PCF8563_SEC_REG, data, sizeof(data));
		if (ret) {
			return ret;
		}

		drv_data->sec = _bcd_to_dec(data[0] & PCF8563_SENCONDS_MASK);
		drv_data->min = _bcd_to_dec(data[1] & PCF8563_MINUTES_MASK);
		drv_data->hour = _bcd_to_dec(data[2] & PCF8563_HOUR_MASK);
		drv_data->day  = _bcd_to_dec(data[3] & PCF8563_DAY_MASK);
		drv_data->wday = _bcd_to_dec(data[4] & PCF8563_WEEKDAY_MASK);
		drv_data->mon = _bcd_to_dec(data[5] & PCF8563_MONTH_MASK);
		drv_data->century = _bcd_to_dec(data[5] & PCF8563_CENTURY_MASK);
		drv_data->year = _bcd_to_dec(data[6]);
		break;

	default:
		return -ENOTSUP;
	}
	return ret;
}

static int pcf8653_channel_get(const struct device *dev,
			       enum sensor_channel chan,
			       struct sensor_value *val)
{
	struct pcf8653_data *drv_data = dev->data;

	switch (chan) {
	case SENSOR_CHAN_SEC:
		val->val1 = drv_data->sec;
		break;
	case SENSOR_CHAN_MIN:
		val->val1 = drv_data->min;
		break;
	case SENSOR_CHAN_HOUR:
		val->val1 = drv_data->hour;
		break;
	case SENSOR_CHAN_DAY:
		val->val1 = drv_data->day;
		break;
	case SENSOR_CHAN_WDAY:
		val->val1 = drv_data->wday;
		break;
	case SENSOR_CHAN_MON:
		val->val1 = drv_data->mon;
		break;
	case SENSOR_CHAN_YEAR:
		val->val1 = drv_data->year;
		break;
	default:
		return -ENOTSUP;
	}
	return 0;
}

int pcf8653_attr_set(const struct device *dev,
		     enum sensor_channel chan,
		     enum sensor_attribute attr,
		     const struct sensor_value *val)
{
}

static const struct sensor_driver_api pcf8653_driver_api = {
	.attr_set = pcf8653_attr_set,
#if CONFIG_PCF8653_TRIGGER
	.trigger_set = pcf8653_trigger_set,
#endif
	.sample_fetch = pcf8653_sample_fetch,
	.channel_get = pcf8653_channel_get,
};

int pcf8653_init_driver(const struct device *dev)
{
	const struct pcf8653_config *cfg = dev->config;
	struct pcf8653_data *drv_data = dev->data;
	int ret = 0;

#if CONFIG_PCF8653_TRIGGER
	ret = pcf8653_interrupt_init(dev);
#endif

	return ret;
}

#define PCF8653_INIT(index)   \
	static struct pcf8653_data pcf8653_data_##index; \
	static const struct pcf8653_config pcf8653_cfg_##index = { \
		.i2c = I2C_DT_SPEC_INST_GET(index), \
		COND_CODE_1(CONFIG_PCF8653_TRIGGER, \
                (.int_gpio = GPIO_DT_SPEC_INST_GET(index, int_gpios)), ()) \
	}; \
	DEVICE_DT_INST_DEFINE(index, pcf8653_init_driver, NULL, \
				&pcf8653_data_##index, &pcf8653_cfg_##index, \
				POST_KERNEL, CONFIG_SENSOR_INIT_PRIORITY, \
				&pcf8653_driver_api);

DT_INST_FOREACH_STATUS_OKAY(PCF8653_INIT)