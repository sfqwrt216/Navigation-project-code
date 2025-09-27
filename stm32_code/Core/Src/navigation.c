/**
 * @file navigation.c
 * @brief Navigation system implementation
 */

#include "navigation.h"
#include <math.h>
#include <string.h>

/* Global variables */
Navigation_State navigation_state;
Waypoint waypoints[MAX_WAYPOINTS];

/* Private variables */
static bool navigation_active = false;
static uint32_t last_calculation_time = 0;

/**
 * @brief Initialize navigation system
 */
void Navigation_Init(void)
{
    /* Initialize navigation state */
    memset(&navigation_state, 0, sizeof(navigation_state));
    navigation_state.status = NAV_IDLE;
    navigation_state.has_target = false;
    navigation_state.gps_valid = false;
    
    /* Initialize waypoints */
    memset(waypoints, 0, sizeof(waypoints));
    
    navigation_active = false;
    last_calculation_time = 0;
}

/**
 * @brief Start navigation
 */
void Navigation_Start(void)
{
    navigation_active = true;
    navigation_state.status = NAV_ACTIVE;
    last_calculation_time = HAL_GetTick();
}

/**
 * @brief Stop navigation
 */
void Navigation_Stop(void)
{
    navigation_active = false;
    navigation_state.status = NAV_IDLE;
}

/**
 * @brief Update current position from GPS data
 * @param gps_data Pointer to GPS data
 */
void Navigation_UpdatePosition(GPS_Data *gps_data)
{
    if(gps_data == NULL || !gps_data->fix_valid)
    {
        navigation_state.gps_valid = false;
        return;
    }
    
    navigation_state.current_latitude = gps_data->latitude;
    navigation_state.current_longitude = gps_data->longitude;
    navigation_state.current_altitude = gps_data->altitude;
    navigation_state.gps_valid = true;
    navigation_state.last_update = HAL_GetTick();
}

/**
 * @brief Update current heading from IMU data
 * @param heading Heading in degrees
 */
void Navigation_UpdateHeading(float heading)
{
    navigation_state.current_heading = heading;
}

/**
 * @brief Perform navigation calculations
 * @param nav_state Pointer to navigation state (for output)
 */
void Navigation_Calculate(Navigation_State *nav_state)
{
    if(!navigation_active || !navigation_state.gps_valid || !navigation_state.has_target)
    {
        return;
    }
    
    uint32_t current_time = HAL_GetTick();
    
    /* Limit calculation frequency to save CPU */
    if(current_time - last_calculation_time < 500) // 500ms minimum interval
    {
        return;
    }
    
    last_calculation_time = current_time;
    
    /* Calculate distance to target */
    navigation_state.distance_to_target = Navigation_CalculateDistance(
        navigation_state.current_latitude,
        navigation_state.current_longitude,
        navigation_state.target_latitude,
        navigation_state.target_longitude
    );
    
    /* Calculate bearing to target */
    navigation_state.bearing_to_target = Navigation_CalculateBearing(
        navigation_state.current_latitude,
        navigation_state.current_longitude,
        navigation_state.target_latitude,
        navigation_state.target_longitude
    );
    
    /* Check if target is reached */
    if(navigation_state.distance_to_target <= TARGET_REACHED_THRESHOLD)
    {
        navigation_state.status = NAV_TARGET_REACHED;
    }
    else
    {
        navigation_state.status = NAV_ACTIVE;
    }
    
    /* Copy state to output parameter if provided */
    if(nav_state != NULL)
    {
        *nav_state = navigation_state;
    }
}

/**
 * @brief Set navigation target
 * @param latitude Target latitude
 * @param longitude Target longitude
 */
void Navigation_SetTarget(double latitude, double longitude)
{
    navigation_state.target_latitude = latitude;
    navigation_state.target_longitude = longitude;
    navigation_state.has_target = true;
    
    if(navigation_active)
    {
        navigation_state.status = NAV_ACTIVE;
    }
}

/**
 * @brief Clear navigation target
 */
void Navigation_ClearTarget(void)
{
    navigation_state.has_target = false;
    navigation_state.distance_to_target = 0.0f;
    navigation_state.bearing_to_target = 0.0f;
    navigation_state.cross_track_error = 0.0f;
    navigation_state.status = NAV_IDLE;
}

/**
 * @brief Calculate distance between two points using Haversine formula
 * @param lat1 Latitude of point 1 (degrees)
 * @param lon1 Longitude of point 1 (degrees)
 * @param lat2 Latitude of point 2 (degrees)
 * @param lon2 Longitude of point 2 (degrees)
 * @return Distance in meters
 */
float Navigation_CalculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    double lat1_rad = Navigation_DegreesToRadians(lat1);
    double lon1_rad = Navigation_DegreesToRadians(lon1);
    double lat2_rad = Navigation_DegreesToRadians(lat2);
    double lon2_rad = Navigation_DegreesToRadians(lon2);
    
    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;
    
    double a = sin(dlat/2) * sin(dlat/2) + 
               cos(lat1_rad) * cos(lat2_rad) * 
               sin(dlon/2) * sin(dlon/2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    
    return (float)(EARTH_RADIUS_M * c);
}

/**
 * @brief Calculate bearing from point 1 to point 2
 * @param lat1 Latitude of point 1 (degrees)
 * @param lon1 Longitude of point 1 (degrees)
 * @param lat2 Latitude of point 2 (degrees)
 * @param lon2 Longitude of point 2 (degrees)
 * @return Bearing in degrees (0-360)
 */
float Navigation_CalculateBearing(double lat1, double lon1, double lat2, double lon2)
{
    double lat1_rad = Navigation_DegreesToRadians(lat1);
    double lat2_rad = Navigation_DegreesToRadians(lat2);
    double dlon_rad = Navigation_DegreesToRadians(lon2 - lon1);
    
    double y = sin(dlon_rad) * cos(lat2_rad);
    double x = cos(lat1_rad) * sin(lat2_rad) - 
               sin(lat1_rad) * cos(lat2_rad) * cos(dlon_rad);
    
    double bearing_rad = atan2(y, x);
    double bearing_deg = Navigation_RadiansToDegrees(bearing_rad);
    
    /* Normalize to 0-360 degrees */
    while(bearing_deg < 0) bearing_deg += 360.0;
    while(bearing_deg >= 360.0) bearing_deg -= 360.0;
    
    return (float)bearing_deg;
}

/**
 * @brief Calculate cross track error
 * @param lat1 Latitude of start point (degrees)
 * @param lon1 Longitude of start point (degrees)
 * @param lat2 Latitude of end point (degrees)
 * @param lon2 Longitude of end point (degrees)
 * @param lat3 Latitude of current point (degrees)
 * @param lon3 Longitude of current point (degrees)
 * @return Cross track error in meters (positive = right of track)
 */
float Navigation_CalculateCrossTrackError(double lat1, double lon1, double lat2, double lon2, double lat3, double lon3)
{
    double dist_start_current = Navigation_CalculateDistance(lat1, lon1, lat3, lon3) / EARTH_RADIUS_M;
    double bearing_start_end = Navigation_DegreesToRadians(Navigation_CalculateBearing(lat1, lon1, lat2, lon2));
    double bearing_start_current = Navigation_DegreesToRadians(Navigation_CalculateBearing(lat1, lon1, lat3, lon3));
    
    double cross_track_distance = asin(sin(dist_start_current) * sin(bearing_start_current - bearing_start_end));
    
    return (float)(cross_track_distance * EARTH_RADIUS_M);
}

/**
 * @brief Convert degrees to radians
 * @param degrees Angle in degrees
 * @return Angle in radians
 */
double Navigation_DegreesToRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

/**
 * @brief Convert radians to degrees
 * @param radians Angle in radians
 * @return Angle in degrees
 */
double Navigation_RadiansToDegrees(double radians)
{
    return radians * 180.0 / M_PI;
}