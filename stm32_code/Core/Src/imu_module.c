/**
 * @file imu_module.c
 * @brief IMU module implementation for navigation system
 */

#include "imu_module.h"
#include <math.h>
#include <string.h>

/* Private variables */
static IMU_Data calibration_data;
static bool is_calibrated = false;
static I2C_HandleTypeDef *imu_i2c;

/* MPU9250/MPU6050 I2C address */
#define MPU9250_I2C_ADDR 0x68 << 1

/* MPU9250 Register addresses */
#define MPU9250_WHO_AM_I     0x75
#define MPU9250_PWR_MGMT_1   0x6B
#define MPU9250_GYRO_CONFIG  0x1B
#define MPU9250_ACCEL_CONFIG 0x1C
#define MPU9250_ACCEL_XOUT_H 0x3B
#define MPU9250_GYRO_XOUT_H  0x43

/**
 * @brief Initialize IMU module
 * @return IMU_Status
 */
IMU_Status IMU_Init(void)
{
    /* Initialize I2C if needed */
    // This would typically be done in the main init function
    // For simulation purposes, we'll assume I2C is initialized
    
    uint8_t who_am_i;
    
    /* Check if IMU is present */
    if(HAL_I2C_Mem_Read(&hi2c1, MPU9250_I2C_ADDR, MPU9250_WHO_AM_I, 1, &who_am_i, 1, 100) != HAL_OK)
    {
        /* IMU not found, continue with simulated data */
        memset(&calibration_data, 0, sizeof(calibration_data));
        is_calibrated = false;
        return IMU_OK;
    }
    
    /* Wake up the MPU */
    uint8_t data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU9250_I2C_ADDR, MPU9250_PWR_MGMT_1, 1, &data, 1, 100);
    
    /* Configure gyroscope (±250°/s) */
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU9250_I2C_ADDR, MPU9250_GYRO_CONFIG, 1, &data, 1, 100);
    
    /* Configure accelerometer (±2g) */
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU9250_I2C_ADDR, MPU9250_ACCEL_CONFIG, 1, &data, 1, 100);
    
    HAL_Delay(100); // Let IMU settle
    
    return IMU_OK;
}

/**
 * @brief Read IMU data
 * @param imu_data Pointer to IMU data structure
 * @return IMU_Status
 */
IMU_Status IMU_Read(IMU_Data *imu_data)
{
    uint8_t raw_data[14];
    int16_t accel_raw[3], gyro_raw[3];
    
    /* Try to read from actual hardware first */
    if(HAL_I2C_Mem_Read(&hi2c1, MPU9250_I2C_ADDR, MPU9250_ACCEL_XOUT_H, 1, raw_data, 14, 100) == HAL_OK)
    {
        /* Parse accelerometer data */
        accel_raw[0] = (int16_t)((raw_data[0] << 8) | raw_data[1]);
        accel_raw[1] = (int16_t)((raw_data[2] << 8) | raw_data[3]);
        accel_raw[2] = (int16_t)((raw_data[4] << 8) | raw_data[5]);
        
        /* Parse gyroscope data */
        gyro_raw[0] = (int16_t)((raw_data[8] << 8) | raw_data[9]);
        gyro_raw[1] = (int16_t)((raw_data[10] << 8) | raw_data[11]);
        gyro_raw[2] = (int16_t)((raw_data[12] << 8) | raw_data[13]);
        
        /* Convert to physical units */
        imu_data->accel_x = accel_raw[0] / 16384.0f * 9.81f; // m/s²
        imu_data->accel_y = accel_raw[1] / 16384.0f * 9.81f;
        imu_data->accel_z = accel_raw[2] / 16384.0f * 9.81f;
        
        imu_data->gyro_x = gyro_raw[0] / 131.0f; // deg/s
        imu_data->gyro_y = gyro_raw[1] / 131.0f;
        imu_data->gyro_z = gyro_raw[2] / 131.0f;
        
        /* For magnetometer, we would need to read from AK8963 */
        /* For now, simulate magnetometer data */
        imu_data->mag_x = 25.0f;  // µT
        imu_data->mag_y = -12.0f;
        imu_data->mag_z = -35.0f;
    }
    else
    {
        /* Generate simulated IMU data */
        static uint32_t sim_time = 0;
        sim_time += 100; // 100ms increment
        
        float t = sim_time / 1000.0f; // Convert to seconds
        
        /* Simulate some movement */
        imu_data->accel_x = 0.1f * sinf(t * 0.5f);
        imu_data->accel_y = 0.1f * cosf(t * 0.3f);
        imu_data->accel_z = 9.81f + 0.05f * sinf(t * 0.7f);
        
        imu_data->gyro_x = 2.0f * sinf(t * 0.2f);
        imu_data->gyro_y = 1.5f * cosf(t * 0.15f);
        imu_data->gyro_z = 0.5f * sinf(t * 0.1f);
        
        /* Simulate earth's magnetic field */
        imu_data->mag_x = 25.0f + 2.0f * sinf(t * 0.1f);
        imu_data->mag_y = -12.0f + 1.5f * cosf(t * 0.08f);
        imu_data->mag_z = -35.0f + 1.0f * sinf(t * 0.05f);
    }
    
    /* Calculate orientation */
    imu_data->heading = IMU_CalculateHeading(imu_data->mag_x, imu_data->mag_y, imu_data->mag_z);
    imu_data->pitch = IMU_CalculatePitch(imu_data->accel_x, imu_data->accel_y, imu_data->accel_z);
    imu_data->roll = IMU_CalculateRoll(imu_data->accel_x, imu_data->accel_y, imu_data->accel_z);
    
    imu_data->timestamp = HAL_GetTick();
    imu_data->data_ready = true;
    
    return IMU_OK;
}

/**
 * @brief Calibrate IMU
 * @return IMU_Status
 */
IMU_Status IMU_Calibrate(void)
{
    IMU_Data temp_data;
    const int calibration_samples = 100;
    
    /* Reset calibration data */
    memset(&calibration_data, 0, sizeof(calibration_data));
    
    /* Collect calibration samples */
    for(int i = 0; i < calibration_samples; i++)
    {
        if(IMU_Read(&temp_data) == IMU_OK)
        {
            calibration_data.gyro_x += temp_data.gyro_x;
            calibration_data.gyro_y += temp_data.gyro_y;
            calibration_data.gyro_z += temp_data.gyro_z;
        }
        HAL_Delay(10);
    }
    
    /* Calculate averages (bias) */
    calibration_data.gyro_x /= calibration_samples;
    calibration_data.gyro_y /= calibration_samples;
    calibration_data.gyro_z /= calibration_samples;
    
    is_calibrated = true;
    
    return IMU_OK;
}

/**
 * @brief Reset IMU module
 */
void IMU_Reset(void)
{
    memset(&calibration_data, 0, sizeof(calibration_data));
    is_calibrated = false;
}

/**
 * @brief Calculate heading from magnetometer data
 * @param mag_x Magnetometer X value
 * @param mag_y Magnetometer Y value
 * @param mag_z Magnetometer Z value
 * @return Heading in degrees (0-360)
 */
float IMU_CalculateHeading(float mag_x, float mag_y, float mag_z)
{
    float heading = atan2f(mag_y, mag_x) * 180.0f / M_PI;
    
    /* Normalize to 0-360 degrees */
    if(heading < 0)
    {
        heading += 360.0f;
    }
    
    return heading;
}

/**
 * @brief Calculate pitch angle from accelerometer data
 * @param accel_x Accelerometer X value
 * @param accel_y Accelerometer Y value
 * @param accel_z Accelerometer Z value
 * @return Pitch angle in degrees
 */
float IMU_CalculatePitch(float accel_x, float accel_y, float accel_z)
{
    return atan2f(-accel_x, sqrtf(accel_y * accel_y + accel_z * accel_z)) * 180.0f / M_PI;
}

/**
 * @brief Calculate roll angle from accelerometer data
 * @param accel_x Accelerometer X value
 * @param accel_y Accelerometer Y value
 * @param accel_z Accelerometer Z value
 * @return Roll angle in degrees
 */
float IMU_CalculateRoll(float accel_x, float accel_y, float accel_z)
{
    return atan2f(accel_y, accel_z) * 180.0f / M_PI;
}