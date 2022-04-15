#ifndef _SERVICE_ACCEL_H
#define _SERVICE_ACCEL_H

typedef struct accel_data {
	int16_t x;
	int16_t y;
	int16_t z;
	bool did_vibrate;
	uint64_t timestamp;
} AccelData;

typedef struct accel_raw_data {
	int16_t x;
	int16_t y;
	int16_t z;
	bool did_vibrate;
	uint64_t timestamp;
} AccelRawData;

typedef enum accel_axis_type {
	ACCEL_AXIS_X,
	ACCEL_AXIS_Y,
	ACCEL_AXIS_Z
} AccelAxisType;

typedef enum accel_sampling_rate {
	ACCEL_SAMPLING_10HZ,
	ACCEL_SAMPLING_25HZ,
	ACCEL_SAMPLING_50HZ,
	ACCEL_SAMPLING_100HZ,
} AccelSamplingRate;

typedef void (* AccelDataHandler)(AccelData *data, uint32_t num_samples);
typedef void (* AccelRawDataHandler)(AccelRawData *data, uint32_t num_samples, uint64_t timestamp);
typedef void (* AccelTapHandler)(AccelAxisType axis, int32_t direction);

int accel_service_peek(AccelData *data);

int accel_service_set_sampling_rate(AccelSamplingRate rate);

int accel_service_set_samples_per_update(uint32_t num_samples);

void accel_data_service_subscribe(uint32_t samples_per_update, AccelDataHandler handler);

void accel_data_service_unsubscribe(void);

void accel_tap_service_subscribe(AccelTapHandler handler);

void accel_tap_service_unsubscribe(void);

void accel_raw_data_service_subscribe(uint32_t samples_per_update, AccelRawDataHandler handler);

#endif /* _SERVICE_ACCEL_H */