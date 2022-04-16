#include <logging/log.h>
#include <drivers/sensor.h>

#include "axp202.h"

LOG_MODULE_DECLARE(axp202, CONFIG_SENSOR_LOG_LEVEL);

static void axp202_gpio_callback(const struct device *dev,
				 struct gpio_callback *cb, uint32_t pins)
{
	struct axp202_data *data = CONTAINER_OF(cb, struct axp202_data, gpio_cb);

	ARG_UNUSED(pins);

#if defined(CONFIG_AXP202_TRIGGER_OWN_THREAD)
	k_sem_give(&data->gpio_sem);
#elif defined(CONFIG_AXP202_TRIGGER_GLOBAL_THREAD)
	k_work_submit(&data->work);
#endif
}

static void axp202_thread_cb(const struct device *dev)
{
	struct axp202_data *data = dev->data;
	const struct axp202_config *cfg = dev->config;
	uint8_t val[5] = { 0 };

i2c_burst_read_dt(&cfg->i2c, AXP202_INTSTS1, val, sizeof(val));
LOG_INF("int happened!");
i2c_burst_write_dt(&cfg->i2c, AXP202_INTSTS1, val, sizeof(val));
}

#ifdef CONFIG_AXP202_TRIGGER_OWN_THREAD
static void axp202_thread(int dev_ptr, int unused)
{
	struct device *dev = INT_TO_POINTER(dev_ptr);
	struct axp202_data *data = dev->data;

	ARG_UNUSED(unused);

	while (1) {
		k_sem_take(&data->gpio_sem, K_FOREVER);
		axp202_thread_cb(dev);
	}
}
#endif

#ifdef CONFIG_AXP202_TRIGGER_GLOBAL_THREAD
static void axp202_work_cb(struct k_work *work)
{
	struct axp202_data *data =
		CONTAINER_OF(work, struct axp202_data, work);

	axp202_thread_cb(data->dev);
}
#endif

int axp202_trigger_set(const struct device *dev,
		       const struct sensor_trigger *trig,
		       sensor_trigger_handler_t handler)
{
}

int axp202_interrupt_init(const struct device *dev)
{
	int ret = 0;

	const struct axp202_config *cfg = dev->config;
	struct axp202_data *data = dev->data;

	if (!device_is_ready(cfg->int_gpio.port)) {
		LOG_ERR("GPIO device %s not ready", cfg->int_gpio.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&cfg->int_gpio, GPIO_INPUT);
	if (ret < 0) {
		return ret;
	}

	gpio_init_callback(&data->gpio_cb,
			  axp202_gpio_callback,
			  BIT(cfg->int_gpio.pin));

	ret = gpio_add_callback(cfg->int_gpio.port, &data->gpio_cb);
	if (ret < 0) {
		LOG_ERR("Could not set gpio callback");
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&cfg->int_gpio, GPIO_INT_EDGE_TO_ACTIVE);
	if (ret < 0) {
		return ret;
	}

#if defined(CONFIG_AXP202_TRIGGER_OWN_THREAD)
	k_sem_init(&data->gpio_sem, 0, UINT_MAX);

	k_thread_create(&data->thread, data->thread_stack,
			CONFIG_AXP202_THREAD_STACK_SIZE,
			(k_thread_entry_t)axp202_thread, dev,
			0, NULL, K_PRIO_COOP(CONFIG_AXP202_THREAD_PRIORITY),
			0, K_NO_WAIT);
#elif defined(CONFIG_AXP202_TRIGGER_GLOBAL_THREAD)
	data->work.handler = axp202_work_cb;
	data->dev = dev;
#endif
	LOG_INF("axp202 trigger init done");
	return ret;
}