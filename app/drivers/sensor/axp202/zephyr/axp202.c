/*
 * SPDX-License-Identifier: Apache-2.0
 */
#define DT_DRV_COMPAT xpowers_axp202

#include <zephyr.h>
#include <logging/log.h>
#include <drivers/sensor.h>
#include <drivers/i2c.h>

#include "axp202.h"

LOG_MODULE_REGISTER(axp202, CONFIG_SENSOR_LOG_LEVEL);

static int axp202_sample_fetch(const struct device *dev, enum sensor_channel chan)
{
	struct axp202_data *data = dev->data;
	const struct axp202_config *config = dev->config;
	int err = 0;

	switch (chan) {
	case SENSOR_CHAN_VOLTAGE:
		err = i2c_burst_read_dt(&config->i2c, AXP202_BAT_AVERVOL_H8, &data->vol, sizeof(data->vol));
		if (err < 0) {
			LOG_ERR("Failed to read voltage (err:%d)", err);
			return err;
		}
		break;

	case SENSOR_CHAN_CURRENT:
		err = i2c_burst_read_dt(&config->i2c, AXP202_BAT_AVERCHGCUR_H8, &data->cur, sizeof(data->cur));
		if (err < 0) {
			LOG_ERR("Failed to read current (err:%d)", err);
			return err;
		}
		break;

	default:
		break;

	}

	return err;
}

static int axp202_channel_get(const struct device *dev, enum sensor_channel chan,
			      struct sensor_value *val)
{
	struct axp202_data *data = dev->data;
	int err = 0;

	switch (chan) {
	case SENSOR_CHAN_VOLTAGE:
		val->val1 = data->vol;
		break;

	case SENSOR_CHAN_CURRENT:
		val->val1 = data->cur;
		break;

	default:
		err = -ENOTSUP;
	}

	return err;
}

int axp202_attr_set(const struct device *dev,
		    enum sensor_channel chan,
		    enum sensor_attribute attr,
		    const struct sensor_value *val)
{
	return 0;
}

static const struct sensor_driver_api axp202_driver_api = {
	.sample_fetch = axp202_sample_fetch,
	.channel_get = axp202_channel_get,
	.attr_set = axp202_attr_set,
};

static int axp202_init(const struct device *dev)
{
	struct axp202_data *data = dev->data;
	const struct axp202_config *config = dev->config;
	uint8_t id;
	int err = 0;

	data->dev = dev;

	err = i2c_reg_read_byte_dt(&config->i2c, AXP202_IC_TYPE, &id);
	if (err < 0) {
		LOG_ERR("Failed to read chip id (err:%d)", err);
		return err;
	}
	if (id != AXP202_CHIP_ID) {
		LOG_ERR("unkown chip id: 0x%d", id);
		return -ENODEV;
	}

#if CONFIG_AXP202_TRIGGER
	err = axp202_interrupt_init(dev);
	if (err) {
		LOG_ERR("interrupt init error: %d", err);
		return err;
	}
#endif
	LOG_INF("axp202 init done");
	return err;
}

#define AXP202_INIT(index)                                                  \
	static struct axp202_data axp202_data_##index;                      \
	static const struct axp202_config axp202_config_##index = {         \
		.i2c = I2C_DT_SPEC_INST_GET(index),                         \
		COND_CODE_1(CONFIG_AXP202_TRIGGER,                          \
		(.int_gpio = GPIO_DT_SPEC_INST_GET(index, int_gpios)), ())  \
	};                                                                  \
	DEVICE_DT_INST_DEFINE(index, axp202_init, NULL,                     \
		&axp202_data_##index, &axp202_config_##index, POST_KERNEL,  \
		CONFIG_SENSOR_INIT_PRIORITY, &axp202_driver_api);

DT_INST_FOREACH_STATUS_OKAY(AXP202_INIT)
