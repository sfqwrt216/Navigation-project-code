#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Navigation System");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Navigation Course Project");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}