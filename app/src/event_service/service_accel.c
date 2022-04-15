#include <zephyr.h>
#include <device.h>
#include <drivers/sensor.h>
#include <logging/log.h>

#include "event_service.h"
#include "event_service_mem.h"
#include "service_accel.h"

#include "bma423.h"

#define ACCEL_DEV "BMA423"

LOG_MODULE_REGISTER(accel_service, LOG_LEVEL_INF);

const struct device *accel_dev;

struct accel_msg {
	AccelData *sensor_data;
	uint32_t num_samples;
};

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
	LOG_DBG("bma423 data ready handler");
}

struct sensor_trigger data_ready_trig = {
	.type = SENSOR_TRIG_DATA_READY,
	.chan = SENSOR_CHAN_ALL,
};

static void bma423_fifo_handler(const struct device *dev,
				const struct sensor_trigger *trig)
{/*
	struct bma423_data *drv_data = dev->data;
	struct bma4_dev *bma_dev = &drv_data->bma_dev;
	int ret = 0;
	uint16_t fifo_length = 0;
	struct bma4_fifo_frame fifoframe = { 0 };
fifoframe.data = drv_data->fifo_data;
fifoframe.length = BMA423_FIFO_RAW_DATA_USER_LENGTH;

	ret = bma4_get_fifo_length(&fifo_length, bma_dev);
	if (ret) {
		LOG_ERR("bma4_get_fifo_length error:%d", ret);
	}
	ret = bma4_read_fifo_data(&fifoframe, bma_dev);
	if (ret) {
		LOG_ERR("bma4_read_fifo_data error:%d", ret);
	}
	bma4_extract_accel();

	frame_count = fifo_length / BMA4_FIFO_A_LENGTH;
static struct accel_data msg;
msg.sensor_data = sensor_data;
msg.num_samples = frame_count;
event_service_post(GX_EVENT_ACCEL_SERVICE, &msg);
*/
	LOG_INF("bma423 fifo handler");
}

struct sensor_trigger fifo_trig = {
	.type = BMA423_TRIG_FIFO_WM,
	.chan = SENSOR_CHAN_ALL,
};

#endif

static void accel_data_service_cb(uint32_t command, void *data, void *context)
{
	AccelDataHandler handler = context;
	struct accel_msg *msg = (struct accel_msg *)data;

	if (handler && msg) {
		handler(msg->sensor_data, msg->num_samples);
	}
}

int accel_service_peek(AccelData *data)
{
	int err = 0;
	struct sensor_value accel_data[3] = { 0 };

	if (!accel_dev) {
		return -ENODEV;
	}

	err = sensor_sample_fetch(accel_dev);
	if (err < 0) {
		LOG_ERR("sensor_sample_fetch error");
	}

	err = sensor_channel_get(accel_dev, SENSOR_CHAN_ACCEL_XYZ, accel_data);
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

	return sensor_attr_set(accel_dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_SAMPLING_FREQUENCY, &setting);
}

void accel_data_service_subscribe(uint32_t samples_per_update, AccelDataHandler handler)
{
	struct sensor_value setting = { 0 };
	setting.val1 = samples_per_update;
	//(void)sensor_value_from_double(&setting, (double)samples_per_update);

	sensor_attr_set(accel_dev, SENSOR_CHAN_ACCEL_XYZ, SENSOR_ATTR_FIFO_WM, &setting);
	event_service_subscribe_with_context(GX_EVENT_ACCEL_SERVICE, accel_data_service_cb, handler);
}

void accel_data_service_unsubscribe(void)
{
	void *context = event_service_get_context(GX_EVENT_ACCEL_SERVICE);
	if (context)
		app_free(context);

	event_service_unsubscribe(GX_EVENT_ACCEL_SERVICE);
}

void accel_tap_service_subscribe(AccelTapHandler handler)
{

}

static int accel_service_init(const struct device *dev)
{
	int ret = 0;
	dev = device_get_binding("BMA423");

	if (!device_is_ready(dev)) {
		LOG_ERR("Sensor %s is not ready\n", dev->name);
		return -ENODEV;
	}

	if (IS_ENABLED(CONFIG_BMA423_TRIGGER)) {
		ret = sensor_trigger_set(dev, &fifo_trig, bma423_fifo_handler);
		if (ret < 0) {
			LOG_ERR("Cannot configure trigger");
			return ret;
		}
	}
	accel_dev = dev;
	return ret;
}
SYS_INIT(accel_service_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
