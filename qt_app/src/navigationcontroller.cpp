#include "navigationcontroller.h"
#include <cmath>
#include <QDebug>

NavigationController::NavigationController(QObject *parent)
    : QObject(parent)
    , targetLatitude(0)
    , targetLongitude(0)
    , hasTarget(false)
{
}

void NavigationController::setTarget(double latitude, double longitude)
{
    targetLatitude = latitude;
    targetLongitude = longitude;
    hasTarget = true;
    
    navData.status = "Target Set";
    emit navigationUpdate(navData);
}

void NavigationController::updatePosition(const GPSData &gpsData)
{
    if (!gpsData.isValid || !hasTarget) {
        navData.status = hasTarget ? "No GPS Signal" : "No Target";
        emit navigationUpdate(navData);
        return;
    }
    
    currentPosition = gpsData;
    
    // Calculate distance to target
    navData.distanceToTarget = calculateDistance(
        currentPosition.latitude, currentPosition.longitude,
        targetLatitude, targetLongitude
    ) * 1000.0; // Convert km to meters
    
    // Calculate bearing to target
    navData.bearingToTarget = calculateBearing(
        currentPosition.latitude, currentPosition.longitude,
        targetLatitude, targetLongitude
    );
    
    // Calculate ETA (estimated time of arrival)
    if (currentPosition.speed > 1.0) { // Only calculate if moving (> 1 km/h)
        double distanceKm = navData.distanceToTarget / 1000.0;
        navData.eta = static_cast<int>((distanceKm / currentPosition.speed) * 3600); // seconds
    } else {
        navData.eta = 0;
    }
    
    // Check if target is reached
    if (navData.distanceToTarget <= TARGET_REACHED_THRESHOLD) {
        navData.status = "Target Reached";
        emit targetReached();
    } else {
        navData.status = "Navigating";
    }
    
    emit navigationUpdate(navData);
}

void NavigationController::clearTarget()
{
    hasTarget = false;
    navData.distanceToTarget = 0;
    navData.bearingToTarget = 0;
    navData.eta = 0;
    navData.status = "No Target";
    
    emit navigationUpdate(navData);
}

double NavigationController::calculateDistance(double lat1, double lon1, double lat2, double lon2)
{
    // Haversine formula for calculating distance between two points on Earth
    double dLat = degreesToRadians(lat2 - lat1);
    double dLon = degreesToRadians(lon2 - lon1);
    
    double lat1Rad = degreesToRadians(lat1);
    double lat2Rad = degreesToRadians(lat2);
    
    double a = sin(dLat/2) * sin(dLat/2) +
               cos(lat1Rad) * cos(lat2Rad) *
               sin(dLon/2) * sin(dLon/2);
    
    double c = 2 * atan2(sqrt(a), sqrt(1-a));
    
    return EARTH_RADIUS_KM * c; // Distance in kilometers
}

double NavigationController::calculateBearing(double lat1, double lon1, double lat2, double lon2)
{
    // Calculate initial bearing from point 1 to point 2
    double lat1Rad = degreesToRadians(lat1);
    double lat2Rad = degreesToRadians(lat2);
    double dLon = degreesToRadians(lon2 - lon1);
    
    double y = sin(dLon) * cos(lat2Rad);
    double x = cos(lat1Rad) * sin(lat2Rad) - sin(lat1Rad) * cos(lat2Rad) * cos(dLon);
    
    double bearing = radiansToDegrees(atan2(y, x));
    
    // Normalize to 0-360 degrees
    while (bearing < 0) bearing += 360.0;
    while (bearing >= 360.0) bearing -= 360.0;
    
    return bearing;
}

double NavigationController::degreesToRadians(double degrees)
{
    return degrees * M_PI / 180.0;
}

double NavigationController::radiansToDegrees(double radians)
{
    return radians * 180.0 / M_PI;
}