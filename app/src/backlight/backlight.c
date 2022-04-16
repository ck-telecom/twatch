#if 0
#include <stdbool.h>
#include <drivers/gpio.h>

#include "backlight.h"

static const struct device* backlight_dev;

#define BACKLIGHT_PORT  DT_GPIO_LABEL(DT_ALIAS(led1), gpios)

#define BACKLIGHT_1     DT_GPIO_PIN(DT_ALIAS(led0), gpios)
#define BACKLIGHT_2     DT_GPIO_PIN(DT_ALIAS(led1), gpios)
#define BACKLIGHT_3     DT_GPIO_PIN(DT_ALIAS(led2), gpios)

int backlight_init(const struct device *dev)
{
    backlight_dev = device_get_binding(BACKLIGHT_PORT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_1, GPIO_OUTPUT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_2, GPIO_OUTPUT);
    gpio_pin_configure(backlight_dev, BACKLIGHT_3, GPIO_OUTPUT);
    backlight_enable(true);

    return 0;
}

int backlight_enable(bool enable)
{
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_1, enable ? 0 : 1);
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_2, enable ? 0 : 1);
    gpio_pin_set_raw(backlight_dev, BACKLIGHT_3, enable ? 0 : 1);

    return 0;
}
#else
#include <drivers/regulator.h>
#include <drivers/pwm.h>
#include <drivers/regulator/consumer.h>

#include <logging/log.h>

#include "backlight.h"

#define BACKLIGHT_STACK_SIZE   1024
#define BACKLIGHT_PRIORITY     5

LOG_MODULE_REGISTER(backlight, LOG_LEVEL_INF);

//#define LDO2
static const struct device *ldo2_dev;
static const struct device* backlight_dev;
#if 0
#if DT_NODE_HAS_STATUS(DT_INST(0, pwm_leds), okay)
#define LED_PWM_NODE_ID		DT_INST(0, pwm_leds)
#define LED_PWM_DEV_NAME	DEVICE_DT_NAME(LED_PWM_NODE_ID)
#else
#error "No LED PWM device found"
#endif
#endif
static int backlight_init(const struct device *dev)
{
    //backlight_dev = device_get_binding(LED_PWM_DEV_NAME);

	backlight_dev = device_get_binding("LEDC_0");
	if (!backlight_dev) {
		LOG_ERR("open device error");
		return -ENODEV;
	}

	ldo2_dev = device_get_binding("ldo2_reg");
	int count = regulator_count_voltages(ldo2_dev);
	LOG_INF("voltage count = %d", count);
	//regulator_set_voltage(ldo2_dev, 1800000, 1800000);
	LOG_INF("vol: %d", regulator_get_voltage(ldo2_dev));
	regulator_enable(ldo2_dev, NULL);
    //led_set_brightness(backlight_dev, 0, 100);

	//pwm_pin_set_usec(backlight_dev, 0, 200, 100, 0);
    return 0;
}

int backlight_enable(bool enable)
{
    if (enable) {
        return led_on(backlight_dev, 0);
    } else {
        return led_off(backlight_dev, 0);
    }
}

int backlight_set_brighness(uint8_t value)
{
    return led_set_brightness(backlight_dev, 0, value);
}
#endif

SYS_INIT(backlight_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);



static void backlight_thread()
{

}

K_THREAD_DEFINE(backlight, BACKLIGHT_STACK_SIZE, backlight_thread,
		NULL, NULL, NULL,
		BACKLIGHT_PRIORITY, 0, 0);