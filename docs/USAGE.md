# Usage Guide

## Getting Started

### First Time Setup
1. Build and flash the STM32 code to your microcontroller
2. Connect the GPS and IMU modules according to the wiring diagram
3. Build and run the QT application on your PC
4. Connect the STM32 to your PC via USB

### QT Application Overview

#### Main Window Layout
The application window is divided into several sections:

1. **GPS Information Panel**
   - Latitude/Longitude coordinates
   - Altitude above sea level
   - Current speed and heading
   - Number of satellites in view

2. **Navigation Information Panel**
   - Distance to target destination
   - Bearing to target
   - Estimated time of arrival (ETA)
   - Navigation status

3. **Control Panel**
   - Connect/Disconnect buttons
   - Start/Stop navigation buttons
   - Clear log button

4. **System Log**
   - Real-time status messages
   - GPS data updates
   - Error messages and diagnostics

## Basic Operations

### Connecting to STM32
1. Ensure STM32 is powered and connected via USB
2. Click "Connect to STM32" button
3. Application will automatically detect STM32 device
4. If no hardware found, simulation mode will start automatically
5. Green status indicates successful connection

### Starting Navigation
1. Ensure GPS has valid fix (check satellite count > 4)
2. Click "Start Navigation" button
3. System will navigate to default target (can be modified in code)
4. Monitor distance and bearing information
5. Navigation status shows "Navigating" when active

### Monitoring GPS Data
- GPS coordinates update every second
- Altitude shows height above sea level
- Speed displays current ground speed
- Heading shows direction of travel
- Satellite count indicates GPS signal quality

## Advanced Features

### Simulation Mode
When no STM32 hardware is detected, the application automatically switches to simulation mode:
- Generates realistic GPS movement patterns
- Simulates satellite availability
- Useful for testing and demonstration

### Data Logging
The system log displays:
- GPS position updates with timestamps
- Navigation calculations
- System status changes
- Error messages and diagnostics

### Navigation Algorithms
The system uses professional navigation algorithms:
- **Haversine formula** for distance calculations
- **Great circle bearing** for direction
- **Real-time updates** for accuracy

## STM32 Operation

### Power-On Sequence
1. System initialization
2. GPS module initialization
3. IMU calibration (keep device still)
4. Status LED indicators activate
5. Ready for PC communication

### LED Status Indicators
- **GPS LED (PC13)**:
  - Solid ON: Valid GPS fix
  - Blinking: Searching for satellites
  - OFF: GPS error
  
- **System LED (PC14)**:
  - Solid ON: System error
  - Blinking: Normal operation
  - OFF: System shutdown

### Communication Protocol
The STM32 responds to these commands from the PC:
- `GPS_REQ` - Send current GPS data
- `NAV_START` - Begin navigation mode
- `NAV_STOP` - Stop navigation mode

## Troubleshooting Common Issues

### No GPS Signal
**Symptoms**: Satellite count shows 0, no position data
**Solutions**:
- Ensure GPS antenna has clear sky view
- Wait 2-5 minutes for initial GPS fix
- Check GPS module wiring
- Verify GPS module power (3.3V)

### Connection Failed
**Symptoms**: "Connection failed" message in QT app
**Solutions**:
- Check USB cable connection
- Verify STM32 is powered on
- Check serial port permissions (Linux/Mac)
- Try different USB port
- Restart both applications

### Incorrect Navigation Data
**Symptoms**: Wrong distance/bearing calculations
**Solutions**:
- Verify GPS coordinates are reasonable
- Check target coordinates in code
- Ensure GPS has valid fix before navigation
- Restart navigation system

### IMU Data Issues
**Symptoms**: Incorrect heading information
**Solutions**:
- Calibrate IMU (keep device still during startup)
- Check I2C wiring connections
- Avoid magnetic interference sources
- Verify IMU module power supply

## Customization Options

### Changing Target Coordinates
To modify the navigation target, edit in `qt_app/src/mainwindow.cpp`:
```cpp
// In startNavigation() function
navController->setTarget(your_latitude, your_longitude);
```

### Adjusting Update Rates
GPS update interval can be changed in `qt_app/src/mainwindow.cpp`:
```cpp
// Change from 1000ms to desired interval
updateTimer->setInterval(1000);
```

### Modifying Serial Settings
Serial communication parameters in `stm32_code/Core/Src/main.c`:
```c
// GPS module baud rate
huart1.Init.BaudRate = 9600;

// PC communication baud rate  
huart2.Init.BaudRate = 115200;
```

## Best Practices

### GPS Usage
- Allow time for initial GPS fix (cold start)
- Position GPS antenna for best sky view
- Avoid operation near tall buildings or indoors
- Monitor satellite count for signal quality

### System Operation
- Connect hardware before starting QT application
- Stop navigation before disconnecting
- Monitor system log for errors
- Regular system testing recommended

### Maintenance
- Check connections periodically
- Clean GPS antenna surface
- Verify power supply stability
- Update firmware as needed

## Safety Considerations

- This system is for educational purposes only
- Do not rely on this system for safety-critical navigation
- Always use official navigation systems for important applications
- Test thoroughly before any field deployment
- Maintain backup navigation methods

## Performance Optimization

### For Better GPS Performance
- Use external active GPS antenna
- Ensure stable power supply
- Minimize electromagnetic interference
- Position away from metal objects

### For Better System Performance
- Use high-quality USB cable
- Ensure adequate STM32 power supply
- Monitor system temperature
- Regular calibration of IMU sensors

## Data Export and Analysis

The system log can be saved and analyzed:
1. Copy log content from QT application
2. Parse GPS coordinates for track analysis
3. Calculate statistics (speed, distance traveled)
4. Generate maps using coordinate data