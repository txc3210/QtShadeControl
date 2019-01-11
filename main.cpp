#include "mainwindow.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include <QList>
#include <QScreen>
#include <QGuiApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.showMaximized();//窗口最大化
    w.start();
    return a.exec();
}
