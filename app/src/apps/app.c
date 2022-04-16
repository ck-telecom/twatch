#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>
#include <logging/log.h>

#include <lvgl.h>

#define APP_STACK_SIZE      1024
#define APP_PRIORITY        5

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

void app_thread(void* arg1, void *arg2, void *arg3)
{
	const struct device *display_dev = NULL;

	display_dev = device_get_binding(CONFIG_LV_Z_DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device %s not found", CONFIG_LV_Z_DISPLAY_DEV_NAME);
		return;
	}

	display_blanking_off(display_dev);

	while (1) {
		lv_timer_handler();
		k_sleep(K_MSEC(20));
		LOG_DBG("lv_timer_handler");
	}
}
K_THREAD_DEFINE(app, APP_STACK_SIZE, app_thread, NULL, NULL, NULL, APP_PRIORITY, 0, 0);
