/**
 * @file
 * @brief Bio sensor public API header file.
 */

/*
 * Copyright (c) 2021 Intellinium <giuliano.franchetto@intellinium.com>
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_DRIVERS_BIO_SENSOR_H_
#define ZEPHYR_INCLUDE_DRIVERS_BIO_SENSOR_H_

#include <device.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Bio sensor driver APIs
 * @defgroup bio_interface Bio sensor driver APIs
 * @ingroup io_interfaces
 * @{
 */

/**
 * @brief Type of value used for calibration
 */
enum bio_calibration_type {
	/** Diastolic calibration value */
	BIO_CALIB_DIASTOLIC = 0,

	/** Systolic calibration value */
	BIO_CALIB_SYSTOLIC,
};

/**
 * @brief Sets the calibration values used by the bio sensor algorithms
 * See bio_calibrate() for arguments description.
 */
typedef int (*bio_calibrate_t)(const struct device *dev,
			       enum bio_calibration_type type,
			       const uint8_t *value, size_t cnt);

/**
 * @brief Events possibly fired from the bio sensor
 */
enum bio_event_type {
	/** Finger detected by the bio sensor */
	BIO_EVENT_FINGER_DETECTED,

	/** The user moved during sampling */
	BIO_EVENT_USER_MOVED,

	/** Finger removed from the sensor */
	BIO_EVENT_FINGER_REMOVED,

	/** New valid sample available */
	BIO_EVENT_SAMPLE_AVAILABLE
};

/**
 * @brief Callback called when a bio sensor fires an event
 *
 * @param dev 		Pointer to the device structure for the driver
 *			instance.
 * @param type		The type of event fired by the sensor
 * @param context	An optional additional parameter
 */
typedef void (*bio_event_handler_t)(const struct device *dev,
				    enum bio_event_type type,
				    uint32_t context);

/**
 * @brief Registers an event handler for the corresponding bio sensor
 * See bio_register_event_handler() for arguments description.
 */
typedef int (*bio_register_event_callback_t)(const struct device *dev,
					     bio_event_handler_t handler);

/** @brief Sensor value type */
enum bio_sensor_value_type {
	/** Beats per minutes */
	BIO_SENSOR_VALUE_BPM,

	/** SpO2 saturation */
	BIO_SENSOR_VALUE_SpO2,

	/** Blood pressure */
	BIO_SENSOR_VALUE_BP,
};

/**
 * @brief Representation of a bio sensor readout value.
 *
 * The value is represented as having an integer and a fractional part,
 * and can be obtained using the formula val1 + val2 * 10^(-2).
 *
 *      0.5: val1 =  0, val2 =  50
 *      1.5: val1 =  1, val2 =  50
 */
struct bio_sensor_value {
	/** Integer part of the value. */
	uint8_t val1;
	/** Fractional part of the value (in one-hundredths parts). */
	uint8_t val2;
};

/**
 * @brief Gets the value from the bio sensor
 * See bio_get_value() for arguments description.
 */
typedef int (*bio_get_value_t)(const struct device *dev,
			       enum bio_sensor_value_type type,
			       struct bio_sensor_value *value);

struct bio_api {
	bio_calibrate_t calibrate;
	bio_register_event_callback_t register_event;
	bio_get_value_t get_value;
};

/**
 * @brief Sets the calibration values used by the bio sensor algorithms
 *
 * @param dev		Pointer to the device structure for the driver
 *			instance.
 * @param type		The type of calibration values.
 * @param values	Array of calibration values.
 * @param cnt		Number of items in the buffer
 *
 * @return 0 on success or a negative error code otherwise
 */
static inline int bio_calibrate(const struct device *device,
				enum bio_calibration_type type,
				const uint8_t *value, size_t cnt)
{
	const struct bio_api *api = device->api;

	return api->calibrate(device, type, value, cnt);
}

/**
 * @brief Registers an event handler for the corresponding bio sensor
 *
 * If an handler has already been registered for this device, it will
 * be replaced by the new handler.
 *
 * @param dev 		Pointer to the device structure for the driver
 *			instance.
 * @param handler	The handler to register
 *
 * @return 0 on success or -EAGAIN if the handler is NULL
 */
static inline int bio_register_event_handler(const struct device *device,
					      bio_event_handler_t handler)
{
	const struct bio_api *api = device->api;

	return api->register_event(device, handler);
}

/**
 * @brief Gets the value from the bio sensor
 *
 * @param dev		Pointer to the device structure for the driver
 *			instance.
 * @param type		The value type to get
 * @param value		The value
 */
static inline int bio_get_value(const struct device *device,
				 enum bio_sensor_value_type type,
				 struct bio_sensor_value *value)
{
	const struct bio_api *api = device->api;

	return api->get_value(device, type, value);
}

/**
 * @}
 */
#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_DRIVERS_BIO_SENSOR_H_ */