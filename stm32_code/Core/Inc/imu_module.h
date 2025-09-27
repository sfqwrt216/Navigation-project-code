/**
 * @file imu_module.h
 * @brief IMU module interface for navigation system
 */

#ifndef __IMU_MODULE_H
#define __IMU_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdbool.h>

/* IMU Status definitions */
typedef enum {
    IMU_OK = 0,
    IMU_ERROR = 1,
    IMU_TIMEOUT = 2,
    IMU_NOT_READY = 3
} IMU_Status;

/* IMU data structure */
typedef struct {
    float accel_x;      // Acceleration X-axis (m/s²)
    float accel_y;      // Acceleration Y-axis (m/s²)
    float accel_z;      // Acceleration Z-axis (m/s²)
    float gyro_x;       // Gyroscope X-axis (deg/s)
    float gyro_y;       // Gyroscope Y-axis (deg/s)
    float gyro_z;       // Gyroscope Z-axis (deg/s)
    float mag_x;        // Magnetometer X-axis (µT)
    float mag_y;        // Magnetometer Y-axis (µT)
    float mag_z;        // Magnetometer Z-axis (µT)
    float heading;      // Calculated heading (degrees)
    float pitch;        // Pitch angle (degrees)
    float roll;         // Roll angle (degrees)
    uint32_t timestamp; // Timestamp in milliseconds
    bool data_ready;    // Data ready flag
} IMU_Data;

/* Function prototypes */
IMU_Status IMU_Init(void);
IMU_Status IMU_Read(IMU_Data *imu_data);
IMU_Status IMU_Calibrate(void);
void IMU_Reset(void);

/* Calculation functions */
float IMU_CalculateHeading(float mag_x, float mag_y, float mag_z);
float IMU_CalculatePitch(float accel_x, float accel_y, float accel_z);
float IMU_CalculateRoll(float accel_x, float accel_y, float accel_z);

#ifdef __cplusplus
}
#endif

#endif /* __IMU_MODULE_H */