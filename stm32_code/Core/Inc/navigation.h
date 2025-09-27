/**
 * @file navigation.h
 * @brief Navigation system interface
 */

#ifndef __NAVIGATION_H
#define __NAVIGATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "gps_module.h"
#include "imu_module.h"
#include <stdbool.h>

/* Navigation Status definitions */
typedef enum {
    NAV_IDLE = 0,
    NAV_ACTIVE = 1,
    NAV_TARGET_REACHED = 2,
    NAV_ERROR = 3
} Navigation_Status;

/* Navigation state structure */
typedef struct {
    double current_latitude;
    double current_longitude;
    float current_altitude;
    float current_heading;
    double target_latitude;
    double target_longitude;
    float distance_to_target;    // meters
    float bearing_to_target;     // degrees
    float cross_track_error;     // meters
    Navigation_Status status;
    uint32_t last_update;        // timestamp
    bool has_target;
    bool gps_valid;
} Navigation_State;

/* Waypoint structure */
typedef struct {
    double latitude;
    double longitude;
    char name[32];
    bool is_active;
} Waypoint;

/* Function prototypes */
void Navigation_Init(void);
void Navigation_Start(void);
void Navigation_Stop(void);
void Navigation_UpdatePosition(GPS_Data *gps_data);
void Navigation_UpdateHeading(float heading);
void Navigation_Calculate(Navigation_State *nav_state);
void Navigation_SetTarget(double latitude, double longitude);
void Navigation_ClearTarget(void);

/* Calculation functions */
float Navigation_CalculateDistance(double lat1, double lon1, double lat2, double lon2);
float Navigation_CalculateBearing(double lat1, double lon1, double lat2, double lon2);
float Navigation_CalculateCrossTrackError(double lat1, double lon1, double lat2, double lon2, double lat3, double lon3);

/* Utility functions */
double Navigation_DegreesToRadians(double degrees);
double Navigation_RadiansToDegrees(double radians);

/* Constants */
#define EARTH_RADIUS_M 6371000.0    // Earth radius in meters
#define TARGET_REACHED_THRESHOLD 10.0  // Target reached threshold in meters
#define MAX_WAYPOINTS 10            // Maximum number of waypoints

/* External variables */
extern Navigation_State navigation_state;
extern Waypoint waypoints[MAX_WAYPOINTS];

#ifdef __cplusplus
}
#endif

#endif /* __NAVIGATION_H */