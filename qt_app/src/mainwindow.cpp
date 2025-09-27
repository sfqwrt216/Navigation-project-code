#include "mainwindow.h"
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , centralWidget(nullptr)
    , gpsReader(nullptr)
    , navController(nullptr)
    , updateTimer(nullptr)
    , serialPort(nullptr)
    , isConnected(false)
    , isNavigating(false)
{
    setupUI();
    
    // Initialize components
    gpsReader = new GPSReader(this);
    navController = new NavigationController(this);
    updateTimer = new QTimer(this);
    serialPort = new QSerialPort(this);
    
    // Connect signals
    connect(gpsReader, &GPSReader::dataReceived, this, &MainWindow::onGpsDataReceived);
    connect(navController, &NavigationController::navigationUpdate, this, &MainWindow::onNavigationUpdate);
    connect(updateTimer, &QTimer::timeout, gpsReader, &GPSReader::requestUpdate);
    
    // Set update interval to 1 second
    updateTimer->setInterval(1000);
    
    // Log startup message
    logTextEdit->append(QString("[%1] Navigation System Started")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

MainWindow::~MainWindow()
{
    if (isConnected) {
        disconnectFromSTM32();
    }
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QGridLayout(centralWidget);
    
    // GPS Information Group
    gpsGroupBox = new QGroupBox("GPS Information", this);
    gpsLayout = new QGridLayout(gpsGroupBox);
    
    gpsLayout->addWidget(new QLabel("Latitude:"), 0, 0);
    latitudeLabel = new QLabel("N/A");
    gpsLayout->addWidget(latitudeLabel, 0, 1);
    
    gpsLayout->addWidget(new QLabel("Longitude:"), 1, 0);
    longitudeLabel = new QLabel("N/A");
    gpsLayout->addWidget(longitudeLabel, 1, 1);
    
    gpsLayout->addWidget(new QLabel("Altitude:"), 2, 0);
    altitudeLabel = new QLabel("N/A");
    gpsLayout->addWidget(altitudeLabel, 2, 1);
    
    gpsLayout->addWidget(new QLabel("Speed:"), 0, 2);
    speedLabel = new QLabel("N/A");
    gpsLayout->addWidget(speedLabel, 0, 3);
    
    gpsLayout->addWidget(new QLabel("Heading:"), 1, 2);
    headingLabel = new QLabel("N/A");
    gpsLayout->addWidget(headingLabel, 1, 3);
    
    gpsLayout->addWidget(new QLabel("Satellites:"), 2, 2);
    satellitesLabel = new QLabel("N/A");
    gpsLayout->addWidget(satellitesLabel, 2, 3);
    
    mainLayout->addWidget(gpsGroupBox, 0, 0, 1, 2);
    
    // Navigation Information Group
    navGroupBox = new QGroupBox("Navigation Information", this);
    navLayout = new QGridLayout(navGroupBox);
    
    navLayout->addWidget(new QLabel("Distance to Target:"), 0, 0);
    distanceLabel = new QLabel("N/A");
    navLayout->addWidget(distanceLabel, 0, 1);
    
    navLayout->addWidget(new QLabel("Bearing:"), 1, 0);
    bearingLabel = new QLabel("N/A");
    navLayout->addWidget(bearingLabel, 1, 1);
    
    navLayout->addWidget(new QLabel("ETA:"), 2, 0);
    etaLabel = new QLabel("N/A");
    navLayout->addWidget(etaLabel, 2, 1);
    
    navLayout->addWidget(new QLabel("Status:"), 3, 0);
    statusLabel = new QLabel("Idle");
    navLayout->addWidget(statusLabel, 3, 1);
    
    mainLayout->addWidget(navGroupBox, 1, 0, 1, 2);
    
    // Control Buttons Group
    controlGroupBox = new QGroupBox("Control Panel", this);
    controlLayout = new QHBoxLayout(controlGroupBox);
    
    connectButton = new QPushButton("Connect to STM32", this);
    disconnectButton = new QPushButton("Disconnect", this);
    startNavButton = new QPushButton("Start Navigation", this);
    stopNavButton = new QPushButton("Stop Navigation", this);
    clearLogButton = new QPushButton("Clear Log", this);
    
    disconnectButton->setEnabled(false);
    startNavButton->setEnabled(false);
    stopNavButton->setEnabled(false);
    
    controlLayout->addWidget(connectButton);
    controlLayout->addWidget(disconnectButton);
    controlLayout->addWidget(startNavButton);
    controlLayout->addWidget(stopNavButton);
    controlLayout->addWidget(clearLogButton);
    
    mainLayout->addWidget(controlGroupBox, 2, 0, 1, 2);
    
    // Log Display
    logGroupBox = new QGroupBox("System Log", this);
    logLayout = new QVBoxLayout(logGroupBox);
    
    logTextEdit = new QTextEdit(this);
    logTextEdit->setMaximumHeight(150);
    logTextEdit->setReadOnly(true);
    logLayout->addWidget(logTextEdit);
    
    mainLayout->addWidget(logGroupBox, 3, 0, 1, 2);
    
    // Connect button signals
    connect(connectButton, &QPushButton::clicked, this, &MainWindow::connectToSTM32);
    connect(disconnectButton, &QPushButton::clicked, this, &MainWindow::disconnectFromSTM32);
    connect(startNavButton, &QPushButton::clicked, this, &MainWindow::startNavigation);
    connect(stopNavButton, &QPushButton::clicked, this, &MainWindow::stopNavigation);
    connect(clearLogButton, &QPushButton::clicked, this, &MainWindow::clearLog);
    
    setWindowTitle("Navigation System - Course Project");
    resize(800, 600);
}

void MainWindow::onGpsDataReceived(const GPSData& data)
{
    updateGPSDisplay(data);
    
    if (isNavigating) {
        navController->updatePosition(data);
    }
    
    logTextEdit->append(QString("[%1] GPS: %2°, %3°, Alt: %4m, Speed: %5 km/h")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                       .arg(data.latitude, 0, 'f', 6)
                       .arg(data.longitude, 0, 'f', 6)
                       .arg(data.altitude, 0, 'f', 1)
                       .arg(data.speed, 0, 'f', 1));
}

void MainWindow::onNavigationUpdate(const NavigationData& data)
{
    updateNavigationDisplay(data);
    
    logTextEdit->append(QString("[%1] Navigation: Distance: %2m, Bearing: %3°")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                       .arg(data.distanceToTarget, 0, 'f', 1)
                       .arg(data.bearingToTarget, 0, 'f', 1));
}

void MainWindow::updateGPSDisplay(const GPSData& data)
{
    latitudeLabel->setText(QString::number(data.latitude, 'f', 6) + "°");
    longitudeLabel->setText(QString::number(data.longitude, 'f', 6) + "°");
    altitudeLabel->setText(QString::number(data.altitude, 'f', 1) + " m");
    speedLabel->setText(QString::number(data.speed, 'f', 1) + " km/h");
    headingLabel->setText(QString::number(data.heading, 'f', 1) + "°");
    satellitesLabel->setText(QString::number(data.satelliteCount));
}

void MainWindow::updateNavigationDisplay(const NavigationData& data)
{
    distanceLabel->setText(QString::number(data.distanceToTarget, 'f', 1) + " m");
    bearingLabel->setText(QString::number(data.bearingToTarget, 'f', 1) + "°");
    
    if (data.eta > 0) {
        int minutes = data.eta / 60;
        int seconds = data.eta % 60;
        etaLabel->setText(QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0')));
    } else {
        etaLabel->setText("N/A");
    }
    
    statusLabel->setText(data.status);
}

void MainWindow::connectToSTM32()
{
    // Try to find STM32 device (usually appears as USB Serial device)
    const auto ports = QSerialPortInfo::availablePorts();
    
    for (const QSerialPortInfo &port : ports) {
        if (port.description().contains("STM32") || port.manufacturer().contains("STMicroelectronics")) {
            serialPort->setPortName(port.portName());
            serialPort->setBaudRate(115200);
            serialPort->setDataBits(QSerialPort::Data8);
            serialPort->setParity(QSerialPort::NoParity);
            serialPort->setStopBits(QSerialPort::OneStop);
            
            if (serialPort->open(QIODevice::ReadWrite)) {
                isConnected = true;
                connectButton->setEnabled(false);
                disconnectButton->setEnabled(true);
                startNavButton->setEnabled(true);
                
                gpsReader->setSerialPort(serialPort);
                
                logTextEdit->append(QString("[%1] Connected to STM32 on %2")
                                   .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                                   .arg(port.portName()));
                return;
            }
        }
    }
    
    // If no STM32 found, use simulation mode
    QMessageBox::information(this, "Connection", 
                           "No STM32 device found. Starting in simulation mode.");
    
    isConnected = true;
    connectButton->setEnabled(false);
    disconnectButton->setEnabled(true);
    startNavButton->setEnabled(true);
    
    gpsReader->setSimulationMode(true);
    
    logTextEdit->append(QString("[%1] Started in simulation mode")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void MainWindow::disconnectFromSTM32()
{
    if (isNavigating) {
        stopNavigation();
    }
    
    if (serialPort && serialPort->isOpen()) {
        serialPort->close();
    }
    
    isConnected = false;
    connectButton->setEnabled(true);
    disconnectButton->setEnabled(false);
    startNavButton->setEnabled(false);
    stopNavButton->setEnabled(false);
    
    gpsReader->setSimulationMode(false);
    
    logTextEdit->append(QString("[%1] Disconnected from STM32")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void MainWindow::startNavigation()
{
    if (!isConnected) return;
    
    isNavigating = true;
    startNavButton->setEnabled(false);
    stopNavButton->setEnabled(true);
    
    // Set a default target location (can be extended to allow user input)
    navController->setTarget(40.7128, -74.0060); // Example: New York City
    
    updateTimer->start();
    
    statusLabel->setText("Navigating");
    
    logTextEdit->append(QString("[%1] Navigation started")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void MainWindow::stopNavigation()
{
    isNavigating = false;
    startNavButton->setEnabled(true);
    stopNavButton->setEnabled(false);
    
    updateTimer->stop();
    
    statusLabel->setText("Idle");
    
    logTextEdit->append(QString("[%1] Navigation stopped")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}

void MainWindow::clearLog()
{
    logTextEdit->clear();
    logTextEdit->append(QString("[%1] Log cleared")
                       .arg(QDateTime::currentDateTime().toString("hh:mm:ss")));
}