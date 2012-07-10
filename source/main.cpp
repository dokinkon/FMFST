#include <QtGui/QApplication>
#include <QDebug>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("Lynx");
    QCoreApplication::setOrganizationDomain("Lynx.com");
    QCoreApplication::setApplicationName("PathFinder");

    qDebug() << "-------------------------------------------";
    qDebug() << "PathFinder VERSION:0.1";
    qDebug() << "-------------------------------------------";

    MainWindow w;
    w.show();
    
    return a.exec();
}
