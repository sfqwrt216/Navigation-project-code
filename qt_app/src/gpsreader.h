#ifndef GPSREADER_H
#define GPSREADER_H

#include <QObject>
#include <QSerialPort>
#include <QTimer>

struct GPSData {
    double latitude;
    double longitude;
    double altitude;
    double speed;      // km/h
    double heading;    // degrees
    int satelliteCount;
    bool isValid;
    
    GPSData() : latitude(0), longitude(0), altitude(0), speed(0), 
                heading(0), satelliteCount(0), isValid(false) {}
};

class GPSReader : public QObject
{
    Q_OBJECT

public:
    explicit GPSReader(QObject *parent = nullptr);
    
    void setSerialPort(QSerialPort *port);
    void setSimulationMode(bool enabled);
    
public slots:
    void requestUpdate();

signals:
    void dataReceived(const GPSData &data);

private slots:
    void readSerialData();

private:
    void parseNMEASentence(const QString &sentence);
    void parseGPGGA(const QStringList &fields);
    void parseGPRMC(const QStringList &fields);
    void generateSimulatedData();
    double convertDMSToDecimal(const QString &dms, const QString &direction);
    
    QSerialPort *serialPort;
    GPSData currentData;
    bool simulationMode;
    QTimer *simulationTimer;
    
    // Simulation variables
    double simLatitude;
    double simLongitude;
    double simHeading;
    double simSpeed;
};

#endif // GPSREADER_H