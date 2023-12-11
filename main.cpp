#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Genavir");
    QCoreApplication::setApplicationName("MDM_Graphes");
    MainWindow w;
    w.show();
    return a.exec();
}
