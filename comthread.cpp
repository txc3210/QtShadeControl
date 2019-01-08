/**************************************
// Programmed by taoxiaochao
 * ********************************/


#include "comthread.h"
#include "controller.h"
#include <QDebug>
#include <QObject>

/*
ComThread::ComThread(QSerialPort *port, QWidget * parent)
{
    this->port = port;
    this->parent = parent;
}
*/

ComThread::ComThread(QSerialPort *port, QList<Controller*> *list, QWidget * parent)
{
    this->port = port;
    this->parent = parent;
    this->list = list;


}

void ComThread::run()
{
    if(!port->isOpen())
    {
        qDebug() << __func__ << ":Serial port is not open, thread can not run";
        return;
    }
    if(list->isEmpty())
    {
        qDebug() << __func__ << ":QList<Controller *> is empty, thread can not run";
        return;
    }    
    //QObject::connect(this, SIGNAL(updateDataSignal(int)), parent, SLOT(updateDataSlot(int)));
    while(true)
    {
        foreach (auto dev, *list) {
            dev->get_angle();
            msleep(1800);
           // emit updateDataSignal(dev->addr);
            emit updateDataSignal(1);
        }
    }
}
