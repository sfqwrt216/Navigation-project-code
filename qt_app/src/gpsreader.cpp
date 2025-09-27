#include "gpsreader.h"
#include <QDebug>
#include <QStringList>
#include <cmath>
#include <QRandomGenerator>

GPSReader::GPSReader(QObject *parent)
    : QObject(parent)
    , serialPort(nullptr)
    , simulationMode(false)
    , simulationTimer(new QTimer(this))
    , simLatitude(39.9042) // Washington DC coordinates for simulation
    , simLongitude(-77.0367)
    , simHeading(45.0)
    , simSpeed(30.0)
{
    connect(simulationTimer, &QTimer::timeout, this, &GPSReader::generateSimulatedData);
}

void GPSReader::setSerialPort(QSerialPort *port)
{
    serialPort = port;
    if (serialPort) {
        connect(serialPort, &QSerialPort::readyRead, this, &GPSReader::readSerialData);
    }
}

void GPSReader::setSimulationMode(bool enabled)
{
    simulationMode = enabled;
    if (enabled) {
        simulationTimer->start(1000); // Generate data every second
    } else {
        simulationTimer->stop();
    }
}

void GPSReader::requestUpdate()
{
    if (simulationMode) {
        // Data is generated automatically by timer
        return;
    }
    
    if (serialPort && serialPort->isOpen()) {
        // Request GPS data from STM32
        serialPort->write("GPS_REQ\r\n");
    }
}

void GPSReader::readSerialData()
{
    if (!serialPort) return;
    
    QByteArray data = serialPort->readAll();
    QString dataString = QString::fromLatin1(data);
    
    QStringList lines = dataString.split('\n', Qt::SkipEmptyParts);
    
    for (const QString &line : lines) {
        QString trimmedLine = line.trimmed();
        if (trimmedLine.startsWith("$")) {
            parseNMEASentence(trimmedLine);
        }
    }
}

void GPSReader::parseNMEASentence(const QString &sentence)
{
    QStringList fields = sentence.split(',');
    
    if (fields.isEmpty()) return;
    
    QString sentenceType = fields[0];
    
    if (sentenceType == "$GPGGA") {
        parseGPGGA(fields);
    } else if (sentenceType == "$GPRMC") {
        parseGPRMC(fields);
    }
}

void GPSReader::parseGPGGA(const QStringList &fields)
{
    // $GPGGA,time,lat,N/S,lon,E/W,quality,numSV,HDOP,alt,M,sep,M,checksum
    if (fields.length() < 15) return;
    
    // Parse latitude
    if (!fields[2].isEmpty() && !fields[3].isEmpty()) {
        currentData.latitude = convertDMSToDecimal(fields[2], fields[3]);
    }
    
    // Parse longitude
    if (!fields[4].isEmpty() && !fields[5].isEmpty()) {
        currentData.longitude = convertDMSToDecimal(fields[4], fields[5]);
    }
    
    // Parse altitude
    if (!fields[9].isEmpty()) {
        currentData.altitude = fields[9].toDouble();
    }
    
    // Parse satellite count
    if (!fields[7].isEmpty()) {
        currentData.satelliteCount = fields[7].toInt();
    }
    
    // Check GPS fix quality
    if (!fields[6].isEmpty()) {
        int quality = fields[6].toInt();
        currentData.isValid = (quality > 0);
        
        if (currentData.isValid) {
            emit dataReceived(currentData);
        }
    }
}

void GPSReader::parseGPRMC(const QStringList &fields)
{
    // $GPRMC,time,status,lat,N/S,lon,E/W,speed,course,date,checksum
    if (fields.length() < 12) return;
    
    // Parse speed (convert from knots to km/h)
    if (!fields[7].isEmpty()) {
        currentData.speed = fields[7].toDouble() * 1.852; // knots to km/h
    }
    
    // Parse heading/course
    if (!fields[8].isEmpty()) {
        currentData.heading = fields[8].toDouble();
    }
    
    // Check status
    if (!fields[2].isEmpty() && fields[2] == "A") {
        currentData.isValid = true;
    }
}

double GPSReader::convertDMSToDecimal(const QString &dms, const QString &direction)
{
    if (dms.isEmpty()) return 0.0;
    
    double degrees, minutes;
    
    // For latitude: DDMM.MMMM format
    // For longitude: DDDMM.MMMM format
    if (dms.length() > 7) { // longitude
        degrees = dms.mid(0, 3).toDouble();
        minutes = dms.mid(3).toDouble();
    } else { // latitude
        degrees = dms.mid(0, 2).toDouble();
        minutes = dms.mid(2).toDouble();
    }
    
    double decimal = degrees + (minutes / 60.0);
    
    if (direction == "S" || direction == "W") {
        decimal = -decimal;
    }
    
    return decimal;
}

void GPSReader::generateSimulatedData()
{
    // Generate realistic GPS data for simulation
    QRandomGenerator *generator = QRandomGenerator::global();
    
    // Add some random movement to simulate real GPS data
    double latVariation = (generator->generateDouble() - 0.5) * 0.0001; // ~10m variation
    double lonVariation = (generator->generateDouble() - 0.5) * 0.0001;
    
    currentData.latitude = simLatitude + latVariation;
    currentData.longitude = simLongitude + lonVariation;
    currentData.altitude = 50.0 + (generator->generateDouble() - 0.5) * 10.0; // 45-55m altitude
    currentData.speed = simSpeed + (generator->generateDouble() - 0.5) * 5.0; // ±2.5 km/h variation
    currentData.heading = simHeading + (generator->generateDouble() - 0.5) * 10.0; // ±5° variation
    currentData.satelliteCount = 8 + generator->bounded(5); // 8-12 satellites
    currentData.isValid = true;
    
    // Normalize heading to 0-360 degrees
    if (currentData.heading < 0) currentData.heading += 360.0;
    if (currentData.heading >= 360.0) currentData.heading -= 360.0;
    
    // Update simulation position (simulate slow movement)
    simLatitude += (generator->generateDouble() - 0.5) * 0.00001;
    simLongitude += (generator->generateDouble() - 0.5) * 0.00001;
    
    emit dataReceived(currentData);
}