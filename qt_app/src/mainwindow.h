#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QTimer>
#include <QSerialPort>
#include "gpsreader.h"
#include "navigationcontroller.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QTextEdit;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QGroupBox;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onGpsDataReceived(const GPSData& data);
    void onNavigationUpdate(const NavigationData& data);
    void connectToSTM32();
    void disconnectFromSTM32();
    void startNavigation();
    void stopNavigation();
    void clearLog();

private:
    void setupUI();
    void updateGPSDisplay(const GPSData& data);
    void updateNavigationDisplay(const NavigationData& data);
    
    // UI Components
    QWidget *centralWidget;
    QGridLayout *mainLayout;
    
    // GPS Information Group
    QGroupBox *gpsGroupBox;
    QGridLayout *gpsLayout;
    QLabel *latitudeLabel;
    QLabel *longitudeLabel;
    QLabel *altitudeLabel;
    QLabel *speedLabel;
    QLabel *headingLabel;
    QLabel *satellitesLabel;
    
    // Navigation Information Group
    QGroupBox *navGroupBox;
    QGridLayout *navLayout;
    QLabel *distanceLabel;
    QLabel *bearingLabel;
    QLabel *etaLabel;
    QLabel *statusLabel;
    
    // Control Buttons Group
    QGroupBox *controlGroupBox;
    QHBoxLayout *controlLayout;
    QPushButton *connectButton;
    QPushButton *disconnectButton;
    QPushButton *startNavButton;
    QPushButton *stopNavButton;
    QPushButton *clearLogButton;
    
    // Log Display
    QGroupBox *logGroupBox;
    QVBoxLayout *logLayout;
    QTextEdit *logTextEdit;
    
    // Core Components
    GPSReader *gpsReader;
    NavigationController *navController;
    QTimer *updateTimer;
    QSerialPort *serialPort;
    
    bool isConnected;
    bool isNavigating;
};

#endif // MAINWINDOW_H