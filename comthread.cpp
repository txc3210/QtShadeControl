/**************************************
// Programmed by taoxiaochao
 * ********************************/


#include "comthread.h"
#include "controller.h"
#include <QDebug>
#include <QObject>

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

    int index = 0;
    while(true)
    {
        index = 0;
        foreach (auto dev, *list) {            
            dev->synchroize(); //读取控制器数据
            msleep(1800);
            //发射信号，告诉列表视图当前读取的是哪一行设备的数据，以便对方更新表格数据
            emit updateInfoSignal(index++);
        }
    }
}
