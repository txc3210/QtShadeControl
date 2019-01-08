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


    QList<QScreen*> list = QGuiApplication::screens();
    QRect rect = list[0]->availableGeometry();
    qDebug() << rect;
    //w.move(rect.x(), rect.y());
    //w.setFixedSize(rect.width() , rect.height());
   // w.setBaseSize(rect.width() , rect.height());
    //w.show();
    w.showMaximized();//窗口最大化
    w.Init();
    return a.exec();
}
