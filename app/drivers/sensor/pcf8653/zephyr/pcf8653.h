#pragma once

#include <zephyr.h>

#include <drivers/gpio.h>
#include <drivers/sensor.h>
#include <drivers/i2c.h>
//! REG MAP
#define PCF8563_STAT1_REG       (0x00)
#define PCF8563_STAT2_REG       (0x01)
#define PCF8563_SEC_REG         (0x02)
#define PCF8563_MIN_REG         (0x03)
#define PCF8563_HR_REG          (0x04)
#define PCF8563_DAY_REG         (0x05)
#define PCF8563_WEEKDAY_REG     (0x06)
#define PCF8563_MONTH_REG       (0x07)
#define PCF8563_YEAR_REG        (0x08)
#define PCF8563_ALRM_MIN_REG    (0x09)
#define PCF8563_SQW_REG         (0x0D)
#define PCF8563_TIMER1_REG      (0x0E)
#define PCF8563_TIMER2_REG      (0x0F)

#define PCF8563_VOL_LOW_MASK    (0x80)
#define PCF8563_SENCONDS_MASK   (0x3F)
#define PCF8563_MINUTES_MASK    (0x7F)
#define PCF8563_HOUR_MASK       (0x3F)
#define PCF8563_WEEKDAY_MASK    (0x07)
#define PCF8563_CENTURY_MASK    (0x80)
#define PCF8563_DAY_MASK        (0x3F)
#define PCF8563_MONTH_MASK      (0x1F)
#define PCF8563_TIMER_CTL_MASK  (0x03)

#define PCF8563_ALARM_AF        (0x08)
#define PCF8563_TIMER_TF        (0x04)
#define PCF8563_ALARM_AIE       (0x02)
#define PCF8563_TIMER_TIE       (0x01)

#define PCF8563_TIMER_TE        (0x80)
#define PCF8563_TIMER_TD10      (0x03)

#define PCF8563_NO_ALARM        (0xFF)
#define PCF8563_ALARM_ENABLE    (0x80)
#define PCF8563_CLK_ENABLE      (0x80)

#define SENSOR_CHAN_SEC         (SENSOR_CHAN_PRIV_START + 1)
#define SENSOR_CHAN_MIN         (SENSOR_CHAN_PRIV_START + 2)
#define SENSOR_CHAN_HOUR        (SENSOR_CHAN_PRIV_START + 3)
#define SENSOR_CHAN_DAY         (SENSOR_CHAN_PRIV_START + 4)
#define SENSOR_CHAN_MON         (SENSOR_CHAN_PRIV_START + 5)
#define SENSOR_CHAN_YEAR        (SENSOR_CHAN_PRIV_START + 6)
#define SENSOR_CHAN_WDAY        (SENSOR_CHAN_PRIV_START + 7)

struct pcf8653_config {
	struct i2c_dt_spec i2c;
#if CONFIG_PCF8653_TRIGGER
	const struct gpio_dt_spec int_gpio;
#endif /* CONFIG_PCF8653_TRIGGER */
};

struct pcf8653_data {
	uint16_t century;
	uint8_t sec;
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t wday;
	uint8_t mon;
	uint16_t year;

#if CONFIG_PCF8653_TRIGGER
	const struct device *gpio;
	struct gpio_callback gpio_cb;

	struct sensor_trigger alarm_trigger;
	sensor_trigger_handler_t alarm_handler;

	struct sensor_trigger timer_trigger;
	sensor_trigger_handler_t timer_handler;
#if defined(CONFIG_PCF8653_TRIGGER_OWN_THREAD)
	K_THREAD_STACK_MEMBER(thread_stack, CONFIG_PCF8653_THREAD_STACK_SIZE);
	struct k_thread thread;
	struct k_sem gpio_sem;
#elif defined(CONFIG_PCF8653_TRIGGER_GLOBAL_THREAD)
	struct k_work work;
	const struct device *dev;
#endif
#endif /* CONFIG_PCF8653_TRIGGER */

};

#ifdef CONFIG_PCF8653_TRIGGER
int pcf8653_trigger_set(const struct device *dev,
		const struct sensor_trigger *trig,
		sensor_trigger_handler_t handler);
#endif

static inline uint8_t _bcd_to_dec(uint8_t val)
{
	return ( (val / 16 * 10) + (val % 16) );
}

static inline uint8_t _dec_to_bcd(uint8_t val)
{
	return ( (val / 10 * 16) + (val % 10) );
}
