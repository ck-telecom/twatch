#include "pcf8653.h"
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <logging/log.h>

LOG_MODULE_DECLARE(pcf8653, CONFIG_SENSOR_LOG_LEVEL);

static void pcf8653_gpio_callback(const struct device *dev,
				 struct gpio_callback *cb, uint32_t pins)
{
	struct pcf8653_data *drv_data = dev->data;

	ARG_UNUSED(pins);

#if defined(CONFIG_PCF8653_TRIGGER_OWN_THREAD)
	k_sem_give(&drv_data->gpio_sem);
#elif defined(CONFIG_PCF8653_TRIGGER_GLOBAL_THREAD)
	k_work_submit(&drv_data->work);
#endif
}

static void pcf8653_thread_cb(const struct device *dev)
{
	const struct pcf8653_config *cfg = dev->config;
	struct pcf8653_data *drv_data = dev->data;
	uint8_t val = 0;
	int ret = 0;

	ret = i2c_reg_read_byte_dt(&cfg->i2c, PCF8563_STAT2_REG, &val);
	if (val & PCF8563_ALARM_AF && drv_data->alarm_handler) {
		drv_data->alarm_handler(dev, &drv_data->alarm_trigger);
	}
	if (val & PCF8563_TIMER_TF && drv_data->timer_handler) {
		drv_data->timer_handler(dev, &drv_data->timer_trigger);
	}
	ret = i2c_reg_update_byte_dt(&cfg->i2c, PCF8563_STAT2_REG,
				     PCF8563_ALARM_AF | PCF8563_TIMER_TF,
				     PCF8563_ALARM_AF | PCF8563_TIMER_TF);
}

int pcf8653_trigger_set(const struct device *dev,
			const struct sensor_trigger *trig,
			sensor_trigger_handler_t handler)
{
	const struct pcf8653_config *cfg = dev->config;
	struct pcf8653_data *drv_data = dev->data;
	/*
	switch (trig->type) {
		uint8_t data = _dec_to_bcd(30);
data &= ~PCF8563_ALARM_ENABLE;
	ret = i2c_reg_write_byte_dt(&cfg->i2c, PCF8563_ALRM_MIN_REG, data);
	}*/
}

int pcf8653_interrupt_init(const struct device *dev)
{
	const struct pcf8653_config *cfg = dev->config;
	struct pcf8653_data *drv_data = dev->data;
	int ret = 0;

	if (!device_is_ready(cfg->int_gpio.port)) {
		LOG_ERR("GPIO device %s not ready", cfg->int_gpio.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&cfg->int_gpio, GPIO_INPUT);
	if (ret < 0) {
		return ret;
	}

	gpio_init_callback(&drv_data->gpio_cb,
			pcf8653_gpio_callback,
			BIT(cfg->int_gpio.pin));

	ret = gpio_add_callback(cfg->int_gpio.port, &drv_data->gpio_cb);
	if (ret < 0) {
		LOG_ERR("Could not set gpio callback");
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&cfg->int_gpio, GPIO_INT_EDGE_FALLING);
	if (ret < 0) {
		return ret;
	}

	ret = i2c_reg_update_byte_dt(&cfg->i2c, PCF8563_STAT2_REG,
				     PCF8563_ALARM_AIE | PCF8563_TIMER_TIE,
				     PCF8563_ALARM_AIE | PCF8563_TIMER_TIE);

	return ret;
}