# Navigation Course Project Documentation

## Project Overview
This repository contains the complete implementation of a navigation system for a course project. The system consists of two main components:
- **QT Application**: A GUI application for visualization and control
- **STM32 Microcontroller Code**: Embedded system handling sensors and navigation calculations

## System Architecture

### Hardware Components
- STM32F4xx microcontroller (e.g., STM32F411RE)
- GPS module (UART interface, 9600 baud)
- IMU/Compass sensor (I2C interface, e.g., MPU9250)
- USB connection for PC communication (115200 baud)
- Status LEDs for system indication

### Software Components

#### QT Application (`qt_app/`)
The GUI application provides:
- Real-time GPS data display (coordinates, altitude, speed, heading)
- Navigation information (distance to target, bearing, ETA)
- Serial communication with STM32
- Simulation mode for testing without hardware
- System logging and status monitoring

**Key Features:**
- Modern QT6 interface with grouped information panels
- Automatic STM32 device detection
- Fallback to simulation mode if hardware not available
- Real-time data updates (1Hz)
- Target navigation with distance and bearing calculations

#### STM32 Code (`stm32_code/`)
The embedded system handles:
- GPS module communication (NMEA parsing)
- IMU sensor data acquisition
- Navigation algorithms (Haversine formula)
- PC communication protocol
- Real-time sensor fusion

**Key Features:**
- Modular code architecture
- NMEA sentence parsing (GPGGA, GPRMC)
- IMU data processing (accelerometer, gyroscope, magnetometer)
- Navigation calculations (distance, bearing, cross-track error)
- Status LED indication

## Directory Structure
```
Navigation-project-code/
├── README.md
├── docs/
│   ├── README.md
│   ├── BUILD.md
│   └── USAGE.md
├── qt_app/
│   ├── CMakeLists.txt
│   └── src/
│       ├── main.cpp
│       ├── mainwindow.h
│       ├── mainwindow.cpp
│       ├── mainwindow.ui
│       ├── gpsreader.h
│       ├── gpsreader.cpp
│       ├── navigationcontroller.h
│       └── navigationcontroller.cpp
└── stm32_code/
    ├── Makefile
    └── Core/
        ├── Inc/
        │   ├── main.h
        │   ├── gps_module.h
        │   ├── imu_module.h
        │   ├── navigation.h
        │   └── usart.h
        └── Src/
            ├── main.c
            ├── gps_module.c
            ├── imu_module.c
            ├── navigation.c
            └── usart.c
```

## Communication Protocol

The QT application and STM32 communicate via serial port using simple ASCII commands:

### Commands (PC → STM32)
- `GPS_REQ\r\n` - Request GPS data
- `NAV_START\r\n` - Start navigation
- `NAV_STOP\r\n` - Stop navigation

### Responses (STM32 → PC)
- NMEA sentences (GPGGA, GPRMC format)
- Status messages
- Error notifications

## Navigation Algorithms

### Distance Calculation
Uses the Haversine formula for calculating great-circle distances between two points on Earth:
```
a = sin²(Δφ/2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ/2)
c = 2 ⋅ atan2( √a, √(1−a) )
d = R ⋅ c
```

### Bearing Calculation
Calculates the initial bearing from current position to target:
```
y = sin(Δlong) ⋅ cos(lat2)
x = cos(lat1) ⋅ sin(lat2) − sin(lat1) ⋅ cos(lat2) ⋅ cos(Δlong)
bearing = atan2(y, x)
```

## Simulation Features

Both components include simulation capabilities for testing without physical hardware:
- **QT App**: Generates realistic GPS movement patterns
- **STM32 Code**: Provides simulated sensor data when hardware is unavailable

## Safety and Error Handling

- GPS fix validation before navigation
- Communication timeout handling
- Sensor data validation
- Error status indication via LEDs
- Comprehensive logging system

## Future Enhancements

Potential improvements for the navigation system:
- Waypoint-based navigation
- Map integration
- Data logging to SD card
- Kalman filter for sensor fusion
- Web interface for remote monitoring
- Multiple coordinate system support