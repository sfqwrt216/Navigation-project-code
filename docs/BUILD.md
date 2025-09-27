# Build Instructions

## Prerequisites

### For QT Application
- Qt6 development environment (Qt Creator recommended)
- CMake 3.16 or later
- C++17 compatible compiler
- Serial port permissions (Linux/Mac)

### For STM32 Code
- ARM GCC toolchain (`arm-none-eabi-gcc`)
- STM32 HAL drivers
- ST-Link programmer
- Make utility

## Building the QT Application

### Using Qt Creator (Recommended)
1. Open Qt Creator
2. Open the CMakeLists.txt file in `qt_app/`
3. Configure the project with Qt6
4. Build and run

### Using Command Line
```bash
cd qt_app/
mkdir build
cd build
cmake ..
make
./NavigationApp
```

### Dependencies
The application requires Qt6 with the following modules:
- Qt6::Core
- Qt6::Widgets
- Qt6::SerialPort

## Building the STM32 Code

### Setup Development Environment
1. Install ARM GCC toolchain:
   ```bash
   # Ubuntu/Debian
   sudo apt install gcc-arm-none-eabi
   
   # macOS (with Homebrew)
   brew install arm-none-eabi-gcc
   ```

2. Install ST-Link tools:
   ```bash
   # Ubuntu/Debian
   sudo apt install stlink-tools
   
   # macOS (with Homebrew)
   brew install stlink
   ```

### Building
```bash
cd stm32_code/
make clean
make
```

### Programming the STM32
```bash
make flash
```

## Hardware Setup

### Required Connections

#### GPS Module (to STM32)
- VCC → 3.3V
- GND → GND
- TX → PA10 (USART1_RX)
- RX → PA9 (USART1_TX)

#### IMU Module (to STM32)
- VCC → 3.3V
- GND → GND
- SDA → PB7 (I2C1_SDA)
- SCL → PB6 (I2C1_SCL)

#### PC Communication (to STM32)
- USB cable (ST-Link/V2 or similar)
- USART2: PA2 (TX), PA3 (RX)

#### Status LEDs (optional)
- GPS Status LED → PC13
- System Status LED → PC14

### STM32CubeMX Configuration (if regenerating)
1. Enable USART1 (GPS) - 9600 baud
2. Enable USART2 (PC) - 115200 baud
3. Enable I2C1 (IMU)
4. Configure GPIO pins for LEDs
5. Enable systick timer

## Troubleshooting

### QT Application Issues
- **Serial port access denied**: Add user to dialout group (Linux)
- **Qt6 not found**: Install qt6-base-dev package
- **Build fails**: Check CMake version and Qt6 installation

### STM32 Code Issues
- **Compilation fails**: Verify ARM GCC toolchain installation
- **Programming fails**: Check ST-Link connection and permissions
- **No GPS data**: Verify GPS module wiring and antenna

### Communication Issues
- **No data from STM32**: Check baud rate settings (115200)
- **Garbled data**: Verify serial port configuration
- **Timeout errors**: Check USB cable and STM32 power

## Testing

### QT Application Testing
1. Run in simulation mode first
2. Verify GUI updates and controls
3. Test with actual STM32 hardware

### STM32 Code Testing
1. Use serial terminal (115200 baud) to verify output
2. Check LED indicators
3. Verify GPS NMEA sentences
4. Test IMU data output

### System Integration Testing
1. Connect STM32 to PC via USB
2. Start QT application
3. Verify automatic connection
4. Test navigation functionality
5. Check data logging

## Performance Notes

- GPS update rate: 1Hz (standard)
- IMU update rate: 10Hz
- PC communication: Real-time
- Navigation calculations: 2Hz (to conserve CPU)
- QT GUI updates: 1Hz

## Code Style and Standards

- Follow embedded C best practices
- Use HAL library for STM32 peripherals
- Qt code follows Qt coding conventions
- Comprehensive error handling
- Modular architecture for maintainability