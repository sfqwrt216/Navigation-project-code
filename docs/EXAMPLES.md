# Example GPS Data for Navigation System

## Sample NMEA Sentences

These are examples of NMEA sentences that the GPS module would send to the STM32:

```
$GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
$GPRMC,123519,A,4807.038,N,01131.000,E,022.4,084.4,230394,003.1,W*6A
$GPGGA,124520,4807.042,N,01131.005,E,1,09,0.8,546.1,M,46.9,M,,*4B
$GPRMC,124520,A,4807.042,N,01131.005,E,023.1,085.2,230394,003.1,W*61
```

## Decoded GPS Information

### Location Examples
- **Beijing Coordinates**: 39.9042°N, 116.4074°E
- **New York Coordinates**: 40.7128°N, 74.0060°W
- **Washington DC**: 38.9072°N, 77.0369°W

### Navigation Calculations
For navigation from Beijing to Shanghai:
- Start: 39.9042°N, 116.4074°E
- End: 31.2304°N, 121.4737°E
- Distance: ~1067 km
- Initial Bearing: ~137°

## QT Application Screenshot Simulation

```
┌─────────────────────────────────────────────────────────────────┐
│                    Navigation System v1.0                      │
├─────────────────────────────────────────────────────────────────┤
│ GPS Information                                                 │
│ ┌─────────────────┬─────────────────┬─────────────────────────┐   │
│ │ Latitude:       │ Longitude:      │ Altitude:               │   │
│ │ 39.904200°      │ 116.407400°     │ 45.2 m                  │   │
│ │                 │                 │                         │   │
│ │ Speed:          │ Heading:        │ Satellites:             │   │
│ │ 28.5 km/h       │ 137.2°          │ 9                       │   │
│ └─────────────────┴─────────────────┴─────────────────────────┘   │
│                                                                 │
│ Navigation Information                                          │
│ ┌─────────────────────────────────────────────────────────────┐   │
│ │ Distance to Target: 1067.2 m                               │   │
│ │ Bearing:           137.5°                                   │   │
│ │ ETA:               00:37                                    │   │
│ │ Status:            Navigating                               │   │
│ └─────────────────────────────────────────────────────────────┘   │
│                                                                 │
│ Control Panel                                                   │
│ [Disconnect] [Stop Navigation] [Clear Log]                     │
│                                                                 │
│ System Log                                                      │
│ ┌─────────────────────────────────────────────────────────────┐   │
│ │ [10:25:30] Navigation System Started                       │   │
│ │ [10:25:31] Connected to STM32 on /dev/ttyACM0              │   │
│ │ [10:25:45] GPS: 39.904200°, 116.407400°, Alt: 45.2m       │   │
│ │ [10:25:46] Navigation: Distance: 1067.2m, Bearing: 137.5°  │   │
│ │ [10:25:47] GPS: 39.904205°, 116.407405°, Alt: 45.3m       │   │
│ └─────────────────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────────────┘
```

## STM32 Debug Output Example

```
STM32 Navigation System Started
GPS Module Initialized
IMU Module Initialized
Navigation System Ready

[GPS] Fix acquired: 9 satellites
[GPS] Position: 39.904200, 116.407400
[GPS] Altitude: 45.2m, Speed: 28.5 km/h
[IMU] Heading: 137.2°, Pitch: 2.1°, Roll: -0.8°
[NAV] Distance to target: 1067.2m
[NAV] Bearing to target: 137.5°
[PC] Command received: GPS_REQ
[PC] GPS data sent to PC
```

## Test Scenarios

### Scenario 1: Urban Navigation
- Start: Beijing city center
- Target: Beijing Airport
- Expected challenges: Urban canyon effects, GPS interference
- Test features: Signal recovery, accuracy validation

### Scenario 2: Highway Navigation  
- Start: Highway entrance
- Target: Next exit
- Expected behavior: Smooth tracking, consistent heading
- Test features: Speed calculation, bearing updates

### Scenario 3: Simulation Mode
- No hardware connected
- Generates realistic movement patterns
- Tests GUI functionality without physical setup
- Validates all software components

## Performance Metrics

### Expected Performance
- GPS Update Rate: 1 Hz
- Position Accuracy: ±3-5 meters (typical GPS)
- Heading Accuracy: ±5° (with IMU fusion)
- Communication Latency: <100ms
- GUI Response Time: <50ms

### Benchmark Results (Simulation)
- Memory Usage (QT): ~15MB
- CPU Usage (QT): <5% (idle), ~10% (active navigation)
- STM32 Flash Usage: ~32KB
- STM32 RAM Usage: ~4KB