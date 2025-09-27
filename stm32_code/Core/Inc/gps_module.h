/**
 * @file gps_module.h
 * @brief GPS module interface for navigation system
 */

#ifndef __GPS_MODULE_H
#define __GPS_MODULE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include <stdbool.h>

/* GPS Status definitions */
typedef enum {
    GPS_OK = 0,
    GPS_ERROR = 1,
    GPS_TIMEOUT = 2,
    GPS_NO_FIX = 3
} GPS_Status;

/* GPS data structure */
typedef struct {
    double latitude;        // Decimal degrees
    double longitude;       // Decimal degrees
    float altitude;         // Meters above sea level
    float speed_knots;      // Speed in knots
    float course;           // Course over ground in degrees
    float hdop;            // Horizontal dilution of precision
    uint8_t satellites_used; // Number of satellites used in fix
    bool fix_valid;        // GPS fix validity
    float utc_time;        // UTC time (HHMMSS.SS)
    float date;            // Date (DDMMYY)
} GPS_Data;

/* Function prototypes */
GPS_Status GPS_Init(UART_HandleTypeDef *huart);
GPS_Status GPS_Read(GPS_Data *gps_data);
GPS_Status GPS_ParseNMEA(char *nmea_sentence, GPS_Data *gps_data);
void GPS_Reset(void);

/* Private functions */
static GPS_Status GPS_ParseGPGGA(char *sentence, GPS_Data *gps_data);
static GPS_Status GPS_ParseGPRMC(char *sentence, GPS_Data *gps_data);
static double GPS_ConvertDMSToDecimal(float dms, char direction);
static uint8_t GPS_CalculateChecksum(char *sentence);

#ifdef __cplusplus
}
#endif

#endif /* __GPS_MODULE_H */