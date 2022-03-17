/*
 * Copyright (c) 2022 Qingsong Gou <gouqs@hotmail.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_DRIVERS_SENSOR_BMA423_BMA423_H_
#define ZEPHYR_DRIVERS_SENSOR_BMA423_BMA423_H_

#include <device.h>
#include <sys/util.h>
#include <zephyr/types.h>
#include <drivers/gpio.h>

#include "bma4_defs.h"

#define BMA423_TRIG_NO_MOTION           (SENSOR_TRIG_PRIV_START + 1)
#define BMA423_TRIG_STEP_COUNT          (SENSOR_TRIG_PRIV_START + 2)
#define BMA423_TRIG_STEP_DETECT         (SENSOR_TRIG_PRIV_START + 3)

#define SENSOR_CHAN_STEP      (SENSOR_CHAN_PRIV_START + 1)
#define SENSOR_CHAN_ACTIVITY  (SENSOR_CHAN_PRIV_START + 2)

#if CONFIG_bma423_ACC_ODR_1
#define bma423_ACC_ODR		0x01
#elif CONFIG_bma423_ACC_ODR_2
#define bma423_ACC_ODR		0x02
#elif CONFIG_bma423_ACC_ODR_3
#define bma423_ACC_ODR		0x03
#elif CONFIG_bma423_ACC_ODR_4
#define bma423_ACC_ODR		0x04
#elif CONFIG_bma423_ACC_ODR_5
#define bma423_ACC_ODR		0x05
#elif CONFIG_bma423_ACC_ODR_6
#define bma423_ACC_ODR		0x06
#elif CONFIG_bma423_ACC_ODR_7
#define bma423_ACC_ODR		0x07
#elif CONFIG_bma423_ACC_ODR_8
#define bma423_ACC_ODR		0x08
#elif CONFIG_bma423_ACC_ODR_9
#define bma423_ACC_ODR		0x09
#elif CONFIG_bma423_ACC_ODR_10
#define bma423_ACC_ODR		0x0a
#elif CONFIG_bma423_ACC_ODR_11
#define bma423_ACC_ODR		0x0b
#elif CONFIG_bma423_ACC_ODR_12
#define bma423_ACC_ODR		0x0c
#elif CONFIG_bma423_ACC_ODR_13
#define bma423_ACC_ODR		0x00
#endif


#if CONFIG_bma423_ACC_RANGE_2G
#define bma423_ACC_RANGE	0x00
#define bma423_ACC_FULL_RANGE	(4 * SENSOR_G)
#elif CONFIG_bma423_ACC_RANGE_4G
#define bma423_ACC_RANGE	0x01
#define bma423_ACC_FULL_RANGE	(8 * SENSOR_G)
#elif CONFIG_bma423_ACC_RANGE_8G
#define bma423_ACC_RANGE	0x02
#define bma423_ACC_FULL_RANGE	(16 * SENSOR_G)
#elif CONFIG_bma423_ACC_RANGE_16G
#define bma423_ACC_RANGE	0x03
#define bma423_ACC_FULL_RANGE	(32 * SENSOR_G)
#endif


#define bma423_THREAD_PRIORITY          10
#define bma423_THREAD_STACKSIZE_UNIT    1024

struct bma423_data {
	const struct device *dev;
	struct bma4_accel accel;
	int32_t temperature;
	uint32_t steps;
	uint8_t activity;
	struct bma4_dev bma_dev;
#ifdef CONFIG_BMA423_TRIGGER
	const struct device *gpio;
	struct gpio_callback gpio_cb;

	struct sensor_trigger data_ready_trigger;
	sensor_trigger_handler_t data_ready_handler;

	struct sensor_trigger single_tap_trigger;
	sensor_trigger_handler_t single_tap_handler;

	struct sensor_trigger double_tap_trigger;
	sensor_trigger_handler_t double_tap_handler;

	struct sensor_trigger any_motion_trigger;
	sensor_trigger_handler_t any_motion_handler;

	struct sensor_trigger no_motion_trigger;
	sensor_trigger_handler_t no_motion_handler;

	struct sensor_trigger step_counter_trigger;
	sensor_trigger_handler_t step_counter_handler;

	struct sensor_trigger step_detection_trigger;
	sensor_trigger_handler_t step_detection_handler;
#if defined(CONFIG_BMA423_TRIGGER_OWN_THREAD)
	K_THREAD_STACK_MEMBER(thread_stack, CONFIG_BMA423_THREAD_STACK_SIZE);
	struct k_thread thread;
	struct k_sem gpio_sem;
#elif defined(CONFIG_BMA423_TRIGGER_GLOBAL_THREAD)
	struct k_work work;
#endif

#endif /* CONFIG_BMA423_TRIGGER */
};

#ifdef CONFIG_BMA423_TRIGGER
int bma423_trigger_set(const struct device *dev,
		const struct sensor_trigger *trig,
		sensor_trigger_handler_t handler);

int bma423_attr_set(const struct device *dev,
		enum sensor_channel chan,
		enum sensor_attribute attr,
		const struct sensor_value *val);

int bma423_init_interrupt(const struct device *dev);

#endif

struct bma423_config {
	struct i2c_dt_spec i2c;
#if CONFIG_BMA423_TRIGGER
	const struct gpio_dt_spec int1_gpio;
#endif /* CONFIG_BMA423_TRIGGER */
};

#endif /* ZEPHYR_DRIVERS_SENSOR_BMA423_BMA423_H_ */
