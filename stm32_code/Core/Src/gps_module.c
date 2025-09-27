/**
 * @file gps_module.c
 * @brief GPS module implementation for navigation system
 */

#include "gps_module.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* Private variables */
static UART_HandleTypeDef *gps_uart;
static char nmea_buffer[256];
static uint16_t buffer_index = 0;

/**
 * @brief Initialize GPS module
 * @param huart UART handle for GPS communication
 * @return GPS_Status
 */
GPS_Status GPS_Init(UART_HandleTypeDef *huart)
{
    gps_uart = huart;
    buffer_index = 0;
    memset(nmea_buffer, 0, sizeof(nmea_buffer));
    
    /* Send GPS initialization commands if needed */
    // Example: Set update rate to 1Hz
    // HAL_UART_Transmit(gps_uart, (uint8_t*)"$PMTK220,1000*1F\r\n", 18, HAL_MAX_DELAY);
    
    return GPS_OK;
}

/**
 * @brief Read GPS data
 * @param gps_data Pointer to GPS data structure
 * @return GPS_Status
 */
GPS_Status GPS_Read(GPS_Data *gps_data)
{
    uint8_t received_byte;
    
    /* Read bytes from UART */
    while(HAL_UART_Receive(gps_uart, &received_byte, 1, 10) == HAL_OK)
    {
        /* Check for buffer overflow */
        if(buffer_index >= sizeof(nmea_buffer) - 1)
        {
            buffer_index = 0;
            continue;
        }
        
        nmea_buffer[buffer_index++] = received_byte;
        
        /* Check for end of NMEA sentence */
        if(received_byte == '\n')
        {
            nmea_buffer[buffer_index] = '\0';
            
            /* Parse the complete NMEA sentence */
            GPS_Status status = GPS_ParseNMEA(nmea_buffer, gps_data);
            
            /* Reset buffer for next sentence */
            buffer_index = 0;
            memset(nmea_buffer, 0, sizeof(nmea_buffer));
            
            if(status == GPS_OK)
            {
                return GPS_OK;
            }
        }
    }
    
    return GPS_TIMEOUT;
}

/**
 * @brief Parse NMEA sentence
 * @param nmea_sentence NMEA sentence string
 * @param gps_data Pointer to GPS data structure
 * @return GPS_Status
 */
GPS_Status GPS_ParseNMEA(char *nmea_sentence, GPS_Data *gps_data)
{
    /* Check if sentence starts with $ */
    if(nmea_sentence[0] != '$')
    {
        return GPS_ERROR;
    }
    
    /* Verify checksum */
    char *checksum_pos = strchr(nmea_sentence, '*');
    if(checksum_pos != NULL)
    {
        uint8_t calculated_checksum = GPS_CalculateChecksum(nmea_sentence);
        uint8_t received_checksum = strtol(checksum_pos + 1, NULL, 16);
        
        if(calculated_checksum != received_checksum)
        {
            return GPS_ERROR;
        }
    }
    
    /* Parse based on sentence type */
    if(strncmp(nmea_sentence, "$GPGGA", 6) == 0 || strncmp(nmea_sentence, "$GNGGA", 6) == 0)
    {
        return GPS_ParseGPGGA(nmea_sentence, gps_data);
    }
    else if(strncmp(nmea_sentence, "$GPRMC", 6) == 0 || strncmp(nmea_sentence, "$GNRMC", 6) == 0)
    {
        return GPS_ParseGPRMC(nmea_sentence, gps_data);
    }
    
    return GPS_ERROR;
}

/**
 * @brief Parse GPGGA sentence
 * @param sentence GPGGA sentence
 * @param gps_data Pointer to GPS data structure
 * @return GPS_Status
 */
static GPS_Status GPS_ParseGPGGA(char *sentence, GPS_Data *gps_data)
{
    char *token;
    int field = 0;
    char *sentence_copy = strdup(sentence);
    
    token = strtok(sentence_copy, ",");
    
    while(token != NULL && field < 15)
    {
        switch(field)
        {
            case 1: // UTC Time
                if(strlen(token) > 0)
                {
                    gps_data->utc_time = atof(token);
                }
                break;
                
            case 2: // Latitude
                if(strlen(token) > 0)
                {
                    float lat_dms = atof(token);
                    gps_data->latitude = GPS_ConvertDMSToDecimal(lat_dms, ' ');
                }
                break;
                
            case 3: // Latitude direction
                if(token[0] == 'S')
                {
                    gps_data->latitude = -gps_data->latitude;
                }
                break;
                
            case 4: // Longitude
                if(strlen(token) > 0)
                {
                    float lon_dms = atof(token);
                    gps_data->longitude = GPS_ConvertDMSToDecimal(lon_dms, ' ');
                }
                break;
                
            case 5: // Longitude direction
                if(token[0] == 'W')
                {
                    gps_data->longitude = -gps_data->longitude;
                }
                break;
                
            case 6: // Fix quality
                gps_data->fix_valid = (atoi(token) > 0);
                break;
                
            case 7: // Number of satellites
                gps_data->satellites_used = atoi(token);
                break;
                
            case 8: // HDOP
                gps_data->hdop = atof(token);
                break;
                
            case 9: // Altitude
                gps_data->altitude = atof(token);
                break;
        }
        
        token = strtok(NULL, ",");
        field++;
    }
    
    free(sentence_copy);
    return GPS_OK;
}

/**
 * @brief Parse GPRMC sentence
 * @param sentence GPRMC sentence
 * @param gps_data Pointer to GPS data structure
 * @return GPS_Status
 */
static GPS_Status GPS_ParseGPRMC(char *sentence, GPS_Data *gps_data)
{
    char *token;
    int field = 0;
    char *sentence_copy = strdup(sentence);
    
    token = strtok(sentence_copy, ",");
    
    while(token != NULL && field < 12)
    {
        switch(field)
        {
            case 1: // UTC Time
                if(strlen(token) > 0)
                {
                    gps_data->utc_time = atof(token);
                }
                break;
                
            case 2: // Status
                gps_data->fix_valid = (token[0] == 'A');
                break;
                
            case 7: // Speed in knots
                gps_data->speed_knots = atof(token);
                break;
                
            case 8: // Course
                gps_data->course = atof(token);
                break;
                
            case 9: // Date
                gps_data->date = atof(token);
                break;
        }
        
        token = strtok(NULL, ",");
        field++;
    }
    
    free(sentence_copy);
    return GPS_OK;
}

/**
 * @brief Convert degrees/minutes/seconds to decimal degrees
 * @param dms Degrees and minutes in DDDMM.MMMM format
 * @param direction N/S/E/W direction
 * @return Decimal degrees
 */
static double GPS_ConvertDMSToDecimal(float dms, char direction)
{
    int degrees = (int)(dms / 100);
    float minutes = dms - (degrees * 100);
    
    double decimal = degrees + (minutes / 60.0);
    
    if(direction == 'S' || direction == 'W')
    {
        decimal = -decimal;
    }
    
    return decimal;
}

/**
 * @brief Calculate NMEA checksum
 * @param sentence NMEA sentence
 * @return Calculated checksum
 */
static uint8_t GPS_CalculateChecksum(char *sentence)
{
    uint8_t checksum = 0;
    char *ptr = sentence + 1; // Skip '$'
    
    while(*ptr != '*' && *ptr != '\0')
    {
        checksum ^= *ptr;
        ptr++;
    }
    
    return checksum;
}

/**
 * @brief Reset GPS module
 */
void GPS_Reset(void)
{
    buffer_index = 0;
    memset(nmea_buffer, 0, sizeof(nmea_buffer));
}