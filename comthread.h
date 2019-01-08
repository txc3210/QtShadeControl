#ifndef COMTHREAD_H
#define COMTHREAD_H
#include <QThread>
#include <QtSerialPort/QSerialPort>
#include <controller.h>


class ComThread : public QThread
{
    Q_OBJECT
public:
//    ComThread(QSerialPort *port, QWidget * parent);
    ComThread(QSerialPort *port, QList<Controller*> *list, QWidget * parent);

protected:
    void run();
signals:
    void updateDataSignal(int addr);
private:
    QSerialPort *port;//通讯串口
    QWidget *parent;
    QList<Controller*> *list;
};

#endif // COMTHREAD_H
