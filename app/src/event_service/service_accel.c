#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>

#include "event_service.h"
#include "service_accel.h"

#include "msg_def.h"

#define ACCEL_DEV "BMA423"

const struct device *dev;

#ifdef CONFIG_BMA423_TRIGGER
static void bma423_data_ready_handler(const struct device *dev,
				      const struct sensor_trigger *trig)
{
	struct sensor_value val[3];

	if (sensor_sample_fetch(dev) < 0) {
		LOG_ERR("sensor_sample_fetch error");
		return;
	}

	if (sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, val) < 0) {
		LOG_ERR("sensor_channel_get error");
		return;
	}
	LOG_INF("bma423 data ready handler");
}

struct sensor_trigger data_ready_trig = {
	.type = SENSOR_TRIG_DATA_READY,
	.chan = SENSOR_CHAN_ALL,
};
#endif

static void accel_data_service_cb(uint32_t command, void *data, void *context)
{
	AccelDataHandler *handler = context;
	struct accel_data *msg = (struct msg_accel_data *)data;

	if (handler && msg) {
		handler(msg->sensor_data, msg->num_samples);
	}
}

int accel_service_peek(AccelData *data)
{
	int err = 0;
	struct sensor_value accel_data[3] = { 0 };

	if (!dev) {
		return -ENODEV;
	}

	err = sensor_sample_fetch(dev);
	if (err < 0) {
		LOG_ERR("sensor_sample_fetch error");
	}

	err = sensor_channel_get(dev, SENSOR_CHAN_ACCEL_XYZ, accel_data);
	if (err < 0) {
		LOG_ERR("sensor_channel_get error");
	}

	data->x = (int16_t)accel_data[0].val1;
	data->y = (int16_t)accel_data[1].val1;
	data->z = (int16_t)accel_data[2].val1;

	return err;
}

int accel_service_set_sampling_rate(AccelSamplingRate rate)
{
	struct sensor_value setting;

	(void)sensor_value_from_double(&setting, (double)rate);

	return sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &setting);
}

void accel_data_service_subscribe(uint32_t samples_per_update, AccelDataHandler handler)
{
	struct sensor_value setting;
	(void)sensor_value_from_double(&setting, (double)samples_per_update);

	//TODO: set sensor attr samples_per_update;
	sensor_attr_set(dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_FIFO_LENGTH, &setting);
	event_service_subscribe_with_context(MSG_TYPE_ACCEL_RAW, accel_data_service_cb, handler);
}

void accel_data_service_unsubscribe(void)
{
	void *context = event_service_get_context(MSG_TYPE_ACCEL_RAW);
	if (context)
		app_free(context);

	event_service_unsubscribe(MSG_TYPE_ACCEL_RAW);
}

void accel_tap_service_subscribe(AccelTapHandler handler)
{

}

static int accel_service_init(const struct device *dev)
{
	int ret = 0;
	dev = DEVICE_DT_GET_ONE(ACCEL_DEV);

	if (!device_is_ready(dev)) {
		printf("Sensor %s is not ready\n", dev->name);
		return -ENODEV;
	}

	if (IS_ENABLED(CONFIG_BMA423_TRIGGER)) {
		ret = sensor_trigger_set(dev, &data_ready_trig, bma423_data_ready_handler);
		if (ret) < 0) {
			LOG_ERR("Cannot configure trigger");
			return ret;
		}
	}
	return ret;
}
SYS_INIT(accel_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
