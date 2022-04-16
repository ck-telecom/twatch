#include <zephyr.h>
#include <device.h>
#include <drivers/display.h>
#include <logging/log.h>

#include <lvgl.h>

#include "app.h"
#include "app/clock.h"

#define APP_STACK_SIZE      1024
#define APP_PRIORITY        5

static struct pinetimecos pinetimecosapp;

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);


int app_init(app_t *app, lv_obj_t *parent)
{
	if (app->spec->init)
		return app->spec->init(app, parent);

	return 0;
}

int app_exit(app_t *app)
{
	if (app->spec->exit)
		return app->spec->exit(app);

	return 0;
}

static void run_app(app_t *app)
{
	if (app == pinetimecosapp.running_app) {
		return;
	}

	if (pinetimecosapp.running_app) {
		app_exit(pinetimecosapp.running_app);
	}
	pinetimecosapp.running_app = app;

	app_init(pinetimecosapp.running_app, lv_scr_act());
}

static void return_app(enum apps app/*, enum appGestures gesture, enum RefreshDirections dir*/)
{
	pinetimecosapp.return_app = app;
}

void load_application(enum apps app/*, enum RefreshDirections dir*/)
{
	switch (app) {
	case CLOCK:
		run_app(APP_CLOCK);
		//return_app(CLOCK/*, DirNone, AnimNone*/);
		break;

	default:
		break;
	}
}

void app_thread(void* arg1, void *arg2, void *arg3)
{
	const struct device *display_dev = NULL;

	display_dev = device_get_binding(CONFIG_LV_Z_DISPLAY_DEV_NAME);
	if (display_dev == NULL) {
		LOG_ERR("device %s not found", CONFIG_LV_Z_DISPLAY_DEV_NAME);
		return;
	}

	display_blanking_off(display_dev);

	load_application(CLOCK);

	while (1) {
		lv_timer_handler();
		k_sleep(K_MSEC(20));
		LOG_DBG("lv_timer_handler");
	}
}
K_THREAD_DEFINE(app, APP_STACK_SIZE, app_thread, NULL, NULL, NULL, APP_PRIORITY, 0, 0);
