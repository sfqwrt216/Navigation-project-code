cmake_minimum_required(VERSION 3.16)
project(NavigationApp VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/gpsreader.cpp
    src/navigationcontroller.cpp
)

set(HEADERS
    src/mainwindow.h
    src/gpsreader.h
    src/navigationcontroller.h
)

set(UI_FILES
    src/mainwindow.ui
)

qt_add_executable(NavigationApp ${SOURCES} ${HEADERS} ${UI_FILES})

target_link_libraries(NavigationApp Qt6::Core Qt6::Widgets Qt6::SerialPort)

# Install target
install(TARGETS NavigationApp
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)