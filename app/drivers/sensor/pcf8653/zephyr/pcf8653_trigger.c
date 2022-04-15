#include <zephyr.h>
#include <drivers/gpio.h>
#include <drivers/i2c.h>
#include <logging/log.h>

#include "pcf8653.h"

LOG_MODULE_DECLARE(pcf8653, CONFIG_SENSOR_LOG_LEVEL);

static void pcf8653_gpio_callback(const struct device *dev,
				 struct gpio_callback *cb, uint32_t pins)
{
	struct pcf8653_data *drv_data = CONTAINER_OF(cb, struct pcf8653_data, gpio_cb);

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
	if (ret) {
		LOG_ERR("read STAT2 error");
	}
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

#ifdef CONFIG_PCF8653_TRIGGER_OWN_THREAD
static void pcf8653_thread(int dev_ptr, int unused)
{
	struct device *dev = INT_TO_POINTER(dev_ptr);
	struct pcf8653_data *drv_data = dev->data;

	ARG_UNUSED(unused);

	while (1) {
		k_sem_take(&drv_data->gpio_sem, K_FOREVER);
		pcf8653_thread_cb(dev);
	}
}
#endif

#ifdef CONFIG_PCF8653_TRIGGER_GLOBAL_THREAD
static void pcf8653_work_cb(struct k_work *work)
{
	struct pcf8653_data *drv_data =
		CONTAINER_OF(work, struct pcf8653_data, work);

	pcf8653_thread_cb(drv_data->dev);
}
#endif

int pcf8653_trigger_set(const struct device *dev,
			const struct sensor_trigger *trig,
			sensor_trigger_handler_t handler)
{
	const struct pcf8653_config *cfg = dev->config;
	struct pcf8653_data *drv_data = dev->data;
	int ret = 0;
	uint8_t val = 0;
	if (!handler || !trig) {
		return -EINVAL;
	}

	ret = i2c_reg_read_byte_dt(&cfg->i2c, PCF8563_STAT2_REG, &val);
	ret = i2c_reg_update_byte_dt(&cfg->i2c, PCF8563_STAT2_REG,
				     PCF8563_ALARM_AF | PCF8563_TIMER_TF,
				     PCF8563_ALARM_AF | PCF8563_TIMER_TF);
	switch (trig->type) {
	case SENSOR_TRIG_RTC_TIMER: {
		uint8_t data = BIT(7) | 0x02;
		uint8_t val = 1;

		ret = i2c_reg_write_byte_dt(&cfg->i2c, PCF8563_TIMER2_REG, val);
		if (ret) {
			LOG_ERR("set timer counter error:%d", ret);
		}
		ret = i2c_reg_update_byte_dt(&cfg->i2c, PCF8563_TIMER1_REG, PCF8563_TIMER_CTL_MASK, data);
		if (ret) {
			LOG_ERR("set timer error:%d", ret);
		}
		drv_data->timer_handler = handler;
		drv_data->timer_trigger = *trig;
		break;
	}
	default:
		break;
	}

	return ret;
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

	ret = gpio_pin_interrupt_configure_dt(&cfg->int_gpio, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		return ret;
	}

	ret = i2c_reg_update_byte_dt(&cfg->i2c, PCF8563_STAT2_REG,
				     PCF8563_ALARM_AIE | PCF8563_TIMER_TIE | PCF8653_TI_TP,
				     PCF8563_ALARM_AIE | PCF8563_TIMER_TIE | PCF8653_TI_TP);
	if (ret < 0) {
		LOG_ERR("Could not enable int");
		return ret;
	}

#if defined(CONFIG_PCF8653_TRIGGER_OWN_THREAD)
	k_sem_init(&drv_data->gpio_sem, 0, UINT_MAX);

	k_thread_create(&drv_data->thread, drv_data->thread_stack,
			CONFIG_PCF8653_THREAD_STACK_SIZE,
			(k_thread_entry_t)pcf8653_thread, dev,
			0, NULL, K_PRIO_COOP(CONFIG_PCF8653_THREAD_PRIORITY),
			0, K_NO_WAIT);
#elif defined(CONFIG_PCF8653_TRIGGER_GLOBAL_THREAD)
	drv_data->work.handler = pcf8653_work_cb;
	drv_data->dev = dev;
#endif
	return ret;
}