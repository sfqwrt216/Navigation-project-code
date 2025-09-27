# Navigation Course Project Code
导航课设的完整代码实现 (QT GUI应用程序和STM32嵌入式代码)

## 项目概述 | Project Overview

这是一个完整的导航系统课程项目，包含两个主要组件：
- **QT应用程序**: 用于可视化和控制的GUI应用程序
- **STM32微控制器代码**: 处理传感器和导航计算的嵌入式系统

This is a complete navigation system course project consisting of two main components:
- **QT Application**: GUI application for visualization and control
- **STM32 Microcontroller Code**: Embedded system handling sensors and navigation calculations

## 主要特性 | Key Features

### QT GUI应用程序
- 实时GPS数据显示（坐标、高度、速度、航向）
- 导航信息显示（到目标距离、方位角、预计到达时间）
- 与STM32的串口通信
- 硬件不可用时的仿真模式
- 系统日志记录和状态监控

### STM32嵌入式系统
- GPS模块通信（NMEA解析）
- IMU传感器数据采集
- 导航算法（Haversine公式）
- PC通信协议
- 实时传感器融合

## 快速开始 | Quick Start

### 1. 硬件要求 | Hardware Requirements
- STM32F4xx微控制器（如STM32F411RE）
- GPS模块（UART接口，9600波特率）
- IMU/指南针传感器（I2C接口，如MPU9250）
- USB连接线用于PC通信
- 状态指示LED（可选）

### 2. 软件要求 | Software Requirements
- Qt6开发环境
- ARM GCC工具链
- ST-Link编程器
- CMake 3.16+

### 3. 构建项目 | Building the Project

#### QT应用程序
```bash
cd qt_app/
mkdir build && cd build
cmake ..
make
./NavigationApp
```

#### STM32代码
```bash
cd stm32_code/
make
make flash
```

## 系统架构 | System Architecture

```
┌─────────────────┐    USB串口     ┌──────────────────┐
│   QT GUI应用    │ ◄─────────────► │  STM32F4微控制器  │
│   - 数据显示    │  115200 baud   │  - GPS处理       │
│   - 导航控制    │                │  - IMU数据       │
│   - 日志记录    │                │  - 导航算法      │
└─────────────────┘                └──────────────────┘
                                            │
                                            │ I2C/UART
                                            ▼
                                   ┌─────────────────┐
                                   │   传感器模块     │
                                   │   - GPS模块     │
                                   │   - IMU传感器   │
                                   │   - 状态LED     │
                                   └─────────────────┘
```

## 目录结构 | Directory Structure

```
Navigation-project-code/
├── README.md                   # 项目说明
├── docs/                       # 详细文档
│   ├── README.md              # 系统概述
│   ├── BUILD.md               # 构建说明
│   └── USAGE.md               # 使用指南
├── qt_app/                     # QT应用程序
│   ├── CMakeLists.txt         # CMake构建配置
│   └── src/                   # 源代码
│       ├── main.cpp           # 主程序入口
│       ├── mainwindow.*       # 主窗口类
│       ├── gpsreader.*        # GPS读取器
│       └── navigationcontroller.* # 导航控制器
└── stm32_code/                 # STM32嵌入式代码
    ├── Makefile               # 构建配置
    └── Core/                  # 核心代码
        ├── Inc/               # 头文件
        └── Src/               # 源文件
```

## 通信协议 | Communication Protocol

### PC → STM32命令
- `GPS_REQ\r\n` - 请求GPS数据
- `NAV_START\r\n` - 开始导航
- `NAV_STOP\r\n` - 停止导航

### STM32 → PC响应
- NMEA格式的GPS句子（GPGGA、GPRMC）
- 状态消息
- 错误通知

## 导航算法 | Navigation Algorithms

### 距离计算 | Distance Calculation
使用Haversine公式计算地球表面两点间的大圆距离：
```
a = sin²(Δφ/2) + cos φ1 ⋅ cos φ2 ⋅ sin²(Δλ/2)
c = 2 ⋅ atan2( √a, √(1−a) )
d = R ⋅ c
```

### 方位角计算 | Bearing Calculation
计算从当前位置到目标的初始方位角：
```
y = sin(Δlong) ⋅ cos(lat2)
x = cos(lat1) ⋅ sin(lat2) − sin(lat1) ⋅ cos(lat2) ⋅ cos(Δlong)
bearing = atan2(y, x)
```

## 仿真功能 | Simulation Features

两个组件都包含仿真功能，用于在没有物理硬件时进行测试：
- **QT应用**: 生成真实的GPS移动模式
- **STM32代码**: 在硬件不可用时提供模拟传感器数据

Both components include simulation capabilities for testing without physical hardware:
- **QT App**: Generates realistic GPS movement patterns  
- **STM32 Code**: Provides simulated sensor data when hardware is unavailable

## 截图 | Screenshots

### QT应用程序界面 | QT Application Interface
(应用程序运行时会显示GPS信息、导航数据和系统日志)

## 文档 | Documentation

- [详细系统文档 | Detailed Documentation](docs/README.md)
- [构建说明 | Build Instructions](docs/BUILD.md)  
- [使用指南 | Usage Guide](docs/USAGE.md)

## 贡献 | Contributing

欢迎提交问题报告和改进建议！
Welcome to submit issues and improvement suggestions!

## 许可证 | License

本项目仅用于教育目的。
This project is for educational purposes only.

## 作者 | Author

导航课程项目 | Navigation Course Project

---
**注意**: 此系统仅用于教育目的，不应用于安全关键的导航应用。
**Note**: This system is for educational purposes only and should not be used for safety-critical navigation applications.
