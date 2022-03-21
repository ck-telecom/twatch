#include <drivers/sensor.h>
#include <time.h>

#include "tick_timer_service.h"

static struct tick_timer_service_data prv_data;

static void tick_timer_service_cb(uint32_t command, void *data, void *context)
{
	TickHandler *handler = context;
	struct tick_timer_service_data *msg = (struct tick_timer_service_data *)data;

	if (handler && msg) {
		handler(msg->tick_time, msg->units_changed);
	}
}

struct sensor_trigger trig = {
	.type = SENSOR_TRIG_RTC_TIMER,
	.chan = SENSOR_CHAN_ALL
};

static void timer_handler(const struct device *dev,
			  const struct sensor_trigger *trig)
{
	struct tm tm = prv_data.tick_time;
	struct sensor_value val[7];

	sensor_sample_fetch(dev);
	sensor_channel_get(dev, SENSOR_CHAN_ALL, val);

	if (val[0].val1 != sec) { /* second updated */
		tm.tm_sec = val[0].val1;
		units_changed |= SECOND_UNIT;
	}
	if (val[1].val1 != min) { /* minute updated */
		tm.tm_min = val[1].val1;
		units_changed |= MINUTE_UNIT;
	}
	if (val[2].val != hour) { /* hour updated */
		tm.tm_hour = val[2].val1;
		units_changed |= HOUR_UNIT;
	}
	if (val[0].val1 != day) { /* new day */
		tm.tm_mday = val[0].val1;
		units_changed |= DAY_UNIT;
	}
	if (val[4].val1 != mon) { /* new month */
		tm.tm_mon = val[0].val1;
		units_changed |= MONTH_UNIT;
	}
	if (val[5].val1 != year) { /* new year */
		tm.tm_year = val[0].val1;
		units_changed |= YEAR_UNIT;
	}
	prv_data.tick_time = tm;
	prv_data.units_changed = units_changed;
	event_service_post(TICK_EVENT, &prv_data);
}

void tick_timer_service_subscribe(TimeUnits tick_units, TickHandler handler)
{
	event_service_subscribe_with_context(TICK_EVENT, tick_timer_service_cb, handler);
}

void tick_timer_service_unsubscribe(void)
{
	event_service_unsubscribe(TICK_EVENT);
}

static int tick_timer_service_init(const struct device *dev)
{
	dev = device_get_binding("PCF8653");
	if (!dev) {
		return -ENODEV;
	}
	sensor_trigger_set(dev, &trig, timer_handler);
}
SYS_INIT(tick_timer_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY)