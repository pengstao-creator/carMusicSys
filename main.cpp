#include "mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char *argv[])
{
    qDebug() << "Starting application...";
    QApplication a(argc, argv);
    qDebug() << "Creating MainWindow...";
    MainWindow w;
    qDebug() << "Showing MainWindow...";
    w.show();
    qDebug() << "Entering event loop...";
    return a.exec();
}