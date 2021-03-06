/*
 * Copyright (c) 2022 Qingsong Gou <gouqs@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <device.h>
#include <drivers/i2c.h>
#include <sys/util.h>
#include <kernel.h>
#include <drivers/sensor.h>
#include <logging/log.h>
#include <stdbool.h>

#include "bma423.h"
#include "libbma423/bma423.h"

LOG_MODULE_DECLARE(bma423, CONFIG_SENSOR_LOG_LEVEL);

static void bma423_gpio_callback(const struct device *dev,
				 struct gpio_callback *cb, uint32_t pins)
{
	struct bma423_data *drv_data = CONTAINER_OF(cb, struct bma423_data, gpio_cb);

	ARG_UNUSED(pins);

#if defined(CONFIG_BMA423_TRIGGER_OWN_THREAD)
	k_sem_give(&drv_data->gpio_sem);
#elif defined(CONFIG_BMA423_TRIGGER_GLOBAL_THREAD)
	k_work_submit(&drv_data->work);
#endif
}

static void bma423_thread_cb(const struct device *dev)
{
	struct bma423_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;

	uint16_t int_status = 0xffffu;
	int ret = 0;

	ret = bma423_read_int_status(&int_status, bma_dev);
	if (ret) {
		LOG_ERR("read int status error: %d", ret);
		return;
	}
	LOG_DBG("int status:0x%x", int_status);

	/* check for fifo wm */
	if (((int_status & BMA4_FIFO_WM_INT) == BMA4_FIFO_WM_INT)
		&& drv_data->fifo_wm_handler != NULL) {
		drv_data->fifo_wm_handler(dev, &drv_data->fifo_wm_trigger);
	}
	/* check for data ready */
	if (((int_status & BMA4_ACCEL_DATA_RDY_INT) == BMA4_ACCEL_DATA_RDY_INT)
		&& drv_data->data_ready_handler != NULL) {
		drv_data->data_ready_handler(dev, &drv_data->data_ready_trigger);
	}
	/* check for single tap */
	if ((int_status & BMA423_SINGLE_TAP_INT) == BMA423_SINGLE_TAP_INT) {
		if (drv_data->single_tap_handler) {
			drv_data->single_tap_handler(dev, &drv_data->any_motion_trigger);
		}
	}
	/* check for double tap */
	if ((int_status & BMA423_DOUBLE_TAP_INT) == BMA423_DOUBLE_TAP_INT) {
		if (drv_data->double_tap_handler) {
			drv_data->double_tap_handler(dev, &drv_data->any_motion_trigger);
		}
	}
	/* check for any motion */
	if ((int_status & BMA423_ANY_MOT_INT) == BMA423_ANY_MOT_INT) {
		if (drv_data->any_motion_handler) {
			drv_data->any_motion_handler(dev, &drv_data->any_motion_trigger);
		}
	}
	/* check for no motion */
	if ((int_status & BMA423_NO_MOT_INT) == BMA423_NO_MOT_INT) {
		if (drv_data->no_motion_handler) {
			drv_data->no_motion_handler(dev, &drv_data->no_motion_trigger);
		}
	}
	/* check for no step detection */
	if ((int_status & BMA423_STEP_CNTR_INT) == BMA423_STEP_CNTR_INT) {
		if (drv_data->step_detection_handler) {
			drv_data->step_detection_handler(dev, &drv_data->step_detection_trigger);
			LOG_DBG("Interrupt status 0x%x Step detected", int_status);
		}
	}
	/* check for any error */
	if (((int_status & BMA423_ERROR_INT) == BMA423_ERROR_INT)) {
		LOG_ERR("Interrupt status 0x%x - Error detected!", int_status);
		// TODO: Handle error (maybe soft reset ?)
	}
}

#ifdef CONFIG_BMA423_TRIGGER_OWN_THREAD
static void bma423_thread(int dev_ptr, int unused)
{
	struct device *dev = INT_TO_POINTER(dev_ptr);
	struct bma423_data *drv_data = dev->data;

	ARG_UNUSED(unused);

	while (1) {
		k_sem_take(&drv_data->gpio_sem, K_FOREVER);
		bma423_thread_cb(dev);
	}
}
#endif

#ifdef CONFIG_BMA423_TRIGGER_GLOBAL_THREAD
static void bma423_work_cb(struct k_work *work)
{
	struct bma423_data *drv_data =
		CONTAINER_OF(work, struct bma423_data, work);

	bma423_thread_cb(drv_data->dev);
}
#endif

int bma423_trigger_set(const struct device *dev,
			const struct sensor_trigger *trig,
			sensor_trigger_handler_t handler)
{
	struct bma423_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;
	int8_t ret;
	static uint16_t interrupt_mask = 0;
	uint8_t interrupt_enable = BMA4_ENABLE;

	if (handler == NULL) {
		interrupt_enable = BMA4_DISABLE;
	}

	switch (trig->type) {
	case SENSOR_TRIG_DATA_READY:
		interrupt_mask |= BMA4_DATA_RDY_INT | BMA4_ACCEL_DATA_RDY_INT;
		drv_data->data_ready_handler = handler;
		drv_data->data_ready_trigger = *trig;
		break;
	case SENSOR_TRIG_TAP: /* single tap */
		interrupt_mask |= BMA423_SINGLE_TAP_INT;
		drv_data->single_tap_handler = handler;
		drv_data->single_tap_trigger = *trig;
		break;
	case SENSOR_TRIG_DOUBLE_TAP:
		interrupt_mask |= BMA423_DOUBLE_TAP_INT;
		drv_data->double_tap_handler = handler;
		drv_data->double_tap_trigger = *trig;
		break;
	case SENSOR_TRIG_DELTA: /* Any-Motion Trigger */
		interrupt_mask |= BMA423_ANY_MOT_INT;
		drv_data->any_motion_handler = handler;
		drv_data->any_motion_trigger = *trig;
		break;
	case BMA423_TRIG_NO_MOTION:
		interrupt_mask |= BMA423_NO_MOT_INT;
		drv_data->no_motion_handler = handler;
		drv_data->no_motion_trigger = *trig;
		break;
	case BMA423_TRIG_STEP_COUNT:
		interrupt_mask |= BMA423_STEP_CNTR_INT;
		drv_data->step_counter_handler = handler;
		drv_data->step_counter_trigger = *trig;
		break;
	case BMA423_TRIG_STEP_DETECT:
		interrupt_mask |= BMA423_STEP_CNTR_INT;
		drv_data->step_detection_handler = handler;
		drv_data->step_detection_trigger = *trig;
		break;
	case BMA423_TRIG_FIFO_WM:
		interrupt_mask |= BMA4_FIFO_WM_INT;
		drv_data->fifo_wm_handler = handler;
		drv_data->fifo_wm_trigger = *trig;
		break;
	default:
		LOG_ERR("Unsupported sensor trigger");
		return -ENOTSUP;
	}

	// Add Error interrupt in any case.
	interrupt_mask |= BMA423_ERROR_INT;

	ret = bma423_map_interrupt(BMA4_INTR1_MAP, interrupt_mask, interrupt_enable, bma_dev);
	if (ret) {
		LOG_ERR("Map interrupt failed err %d", ret);
		return ret;
	}
	/* Latch mode means that interrupt flag are only reset once the status is read */
	ret = bma4_set_interrupt_mode(BMA4_LATCH_MODE, bma_dev);
	if (ret) {
		LOG_ERR("bma4_set_interrupt_mode error: %d", ret);
		return ret;
	}

	return 0;
}

int bma423_init_interrupt(const struct device *dev)
{
	const struct bma423_config *cfg = dev->config;
	struct bma423_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;
	int8_t ret;

	if (!device_is_ready(cfg->int1_gpio.port)) {
		LOG_ERR("GPIO device %s not ready", cfg->int1_gpio.port->name);
		return -ENODEV;
	}

	ret = gpio_pin_configure_dt(&cfg->int1_gpio, GPIO_INPUT);
	if (ret < 0) {
		return ret;
	}

	gpio_init_callback(&drv_data->gpio_cb,
			bma423_gpio_callback,
			BIT(cfg->int1_gpio.pin));

	ret = gpio_add_callback(cfg->int1_gpio.port, &drv_data->gpio_cb);
	if (ret < 0) {
		LOG_ERR("Could not set gpio callback");
		return ret;
	}

	ret = gpio_pin_interrupt_configure_dt(&cfg->int1_gpio, GPIO_INT_EDGE_FALLING);
	if (ret < 0) {
		return ret;
	}

	struct bma4_int_pin_config pin_config;
	bma4_get_int_pin_config(&pin_config, BMA4_INTR1_MAP, bma_dev);

	pin_config.output_en = BMA4_OUTPUT_ENABLE;
	pin_config.od = BMA4_PUSH_PULL;
	pin_config.lvl = BMA4_ACTIVE_LOW;
	pin_config.edge_ctrl = BMA4_LEVEL_TRIGGER;
	pin_config.input_en = BMA4_INPUT_DISABLE;

	ret = bma4_set_int_pin_config(&pin_config, BMA4_INTR1_MAP, bma_dev);
	if (ret) {
		LOG_ERR("Set interrupt config err %d", ret);
		return ret;
	}

	uint8_t bma_status = 0xffu;
	bma4_get_status(&bma_status, bma_dev);

#if defined(CONFIG_BMA423_TRIGGER_OWN_THREAD)
	k_sem_init(&drv_data->gpio_sem, 0, UINT_MAX);

	k_thread_create(&drv_data->thread, drv_data->thread_stack,
			CONFIG_BMA423_THREAD_STACK_SIZE,
			(k_thread_entry_t)bma423_thread, dev,
			0, NULL, K_PRIO_COOP(CONFIG_BMA423_THREAD_PRIORITY),
			0, K_NO_WAIT);
#elif defined(CONFIG_BMA423_TRIGGER_GLOBAL_THREAD)
	drv_data->work.handler = bma423_work_cb;
	drv_data->dev = dev;
#endif
	LOG_INF("bma423 trigger init done");
	return ret;
}
