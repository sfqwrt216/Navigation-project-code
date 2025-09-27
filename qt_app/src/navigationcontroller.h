#ifndef NAVIGATIONCONTROLLER_H
#define NAVIGATIONCONTROLLER_H

#include <QObject>
#include "gpsreader.h"

struct NavigationData {
    double distanceToTarget;  // meters
    double bearingToTarget;   // degrees
    int eta;                  // seconds
    QString status;
    
    NavigationData() : distanceToTarget(0), bearingToTarget(0), eta(0), status("Idle") {}
};

class NavigationController : public QObject
{
    Q_OBJECT

public:
    explicit NavigationController(QObject *parent = nullptr);
    
    void setTarget(double latitude, double longitude);
    void updatePosition(const GPSData &gpsData);
    void clearTarget();

signals:
    void navigationUpdate(const NavigationData &data);
    void targetReached();

private:
    double calculateDistance(double lat1, double lon1, double lat2, double lon2);
    double calculateBearing(double lat1, double lon1, double lat2, double lon2);
    double degreesToRadians(double degrees);
    double radiansToDegrees(double radians);
    
    double targetLatitude;
    double targetLongitude;
    bool hasTarget;
    GPSData currentPosition;
    NavigationData navData;
    
    static constexpr double EARTH_RADIUS_KM = 6371.0;
    static constexpr double TARGET_REACHED_THRESHOLD = 10.0; // meters
};

#endif // NAVIGATIONCONTROLLER_H