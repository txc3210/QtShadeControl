/****************************************************************************
 * **Copyright GDJYK Ltd. All rights reserved
 * Write by Tao Xiachao in 2019
***************************************************************************/
#include <QThread>
#include "controller.h"
#include <thread>
#include <chrono>
#include <ctime>
#include <QDebug>
#include <QString>
#include <QMessageBox>

Controller::Controller(QSerialPort *port, unsigned char addr)
{
    state = 0;//默认通讯正常
    this->addr = addr;
    mode = MODE_AUTO;
    angle = 0;
    this->port = port;
    timestamp = QString("");
}

Controller::~Controller() {

};


/**********************************************************************
 * Function: Get current time string
 * Param: None
 * Return: Current time string
 * *********************************************************************/
QString get_current_time()
{
    char str[20];
    using std::chrono::system_clock;
    std::time_t tt = system_clock::to_time_t(system_clock::now());
    struct std::tm tms;
    localtime_s(&tms, &tt);
    strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S", &tms);
    return QString(str);
}

/**************************************************************************
 * Function: Calculate CRC16 for the bytes
 * Param: Buffer of the bytes
 * Param len: Count of the bytes
 * Return: The CRC16 result
 * ***********************************************************************/
unsigned short Controller::crc_16b(unsigned char *pBuf, unsigned short len)
{
    unsigned short uCRC = 0xFFFF;
    int i, j;
    for (i = 0; i<len; i++)
    {
        uCRC ^= (*(pBuf + i));
        for (j = 0; j<8; j++)
        {
            if ((uCRC & 0x0001) == 0x0001)
            {
                uCRC = (uCRC >> 1);
                uCRC ^= 0xA001;
            }
            else
                uCRC = (uCRC >> 1);
        }
    }
    return uCRC;
}


/*************************************************************
@Function: Turn the ange to motor time
@Param angel: Angle wanted for the board
@Return: Time for the motor to run to the angel
*************************************************************/
unsigned short Controller::angle_to_time(unsigned char angle)
{
    float time = static_cast<float>(angle) / 90.0f * MAX_ANGLE_TIME;
    return static_cast<unsigned short>(time);
}

/******************************************************************
@Function: Turn the motor time to board angle
@Param time: Time for the motor to make the board to current angel
@Return: Angle wanted for the board
*******************************************************************/
unsigned char Controller::time_to_angle(unsigned short time)
{
    float angle = static_cast<float>(time) / MAX_ANGLE_TIME * 90.0f ;
    return static_cast<unsigned char>(angle);
}

/*********************************************************************************************
@Function: Set target angel fo the board
@Param addr: Device address of the controller on RS485 bus
@Param angle: Target angle of board
@Return: 0 for success , <0 values for errors
*********************************************************************************************/
int Controller::set_angle(unsigned char angle)
{
    unsigned char send_buf[8];
    if (!port->isOpen())
        return -1;
    send_buf[0] = addr;
    send_buf[1] = 0x06;
    send_buf[2] = 0xA0;
    send_buf[3] = 0x00;
    unsigned short time = angle_to_time(angle); //角度转换成电机运动时间
    send_buf[4] = static_cast<unsigned char>(time >> 8);
    send_buf[5] = static_cast<unsigned char>(time);
    unsigned short crc = crc_16b(send_buf, 6);
    send_buf[6] = static_cast<unsigned char>(crc); //CRC低字节
    send_buf[7] = static_cast<unsigned char>(crc >> 8);//CRC高字节
    port->clear(); //发送之前先清空缓冲区
    qint64 num = port->write(reinterpret_cast<const char *>(send_buf), 8);
    if(port->waitForBytesWritten(RS485_WRITE_TIMEOUT))
    {
        qDebug() << __func__ << ": Serial port write successful";
    }else
    {
        qDebug() << __func__ << ": Serial port write timeout. err= " << port->errorString();
        port->clearError();
        return -2;
    }
    qDebug() << __func__ << ": Serial port send" << num <<" bytes.";
    if (addr == 0) //广播地址发送数据时没有返回
        return 0;
    //等待接收返回数据
    if(port->waitForReadyRead(RS485_READ_TIMEOUT))
    {
        //有数据到达了串口，但不代表所有数据到达
        qDebug() << __func__ << ": Serial port received some data";
    }else
    {
        qDebug() << __func__ << ": Serial port read timeout. err= " << port->errorString();
        port->clearError();
        return -3;
    }
    QThread::msleep(RS485_WAIT_TIME); //等待数据全部接收完成
    timestamp = get_current_time();
    QByteArray array = port->readAll();
    if (array.isEmpty())
    {
        qDebug() << __func__ << ": Serial port recv 0 bytes.";
        state = RS485_NO_REPLY;
        return -4;
    }
    else if (array.size() != 6)
    {
        qDebug() << __func__ << ": Serial port does not recv enough bytes.";
        return -5;
    }

    unsigned char *recv_buf = reinterpret_cast<unsigned char *>(array.data());
    if (recv_buf[0] != addr || recv_buf[1] != 0x06)
        return -6;
    state = RS485_OK;
    return 0;
}

/**************************************************************************
@Function: Get current angle of the board
@Param addr: Device address of the controller on RS485 bus
@Param angle: Point the data to store the angle;
@Return: 0 for success, <0 value for error;
**************************************************************************/
int Controller::get_angle()
{
    assert(addr <= 247);
    unsigned char send_buf[6];
    if (!port->isOpen())
    {
        qDebug() << __func__ << ": Serial port is not opened.";
        return -1;
    }
    send_buf[0] = addr;
    send_buf[1] = 0x03;
    send_buf[2] = 0xA0;
    send_buf[3] = 0x00;
    unsigned short crc = crc_16b(send_buf, 4);
    send_buf[4] = static_cast<unsigned char>(crc);//CRC低字节
    send_buf[5] = static_cast<unsigned char>(crc >> 8);//CRC高字节

    if(!port->clear())
    {
        qDebug() << __func__ << ": Serial port clear failed.";
    }
    qint64 num = port->write(reinterpret_cast<const char*>(send_buf), 6);
    if(port->waitForBytesWritten(RS485_WRITE_TIMEOUT))
    {
        qDebug() << __func__ << ": Serial port write successful";
    }else
    {
        qDebug() << __func__ << ": Serial port write timeout. err= " << port->errorString();
        port->clearError();
        return -2;
    }
    qDebug() << __func__ << ": Serial port send" << num <<" bytes.";

    if(port->waitForReadyRead(RS485_READ_TIMEOUT))
    {
        qDebug() << __func__ << ": Serial port received some data";
    }else
    {
        qDebug() << __func__ << ": Serial port read timeout. err= " << port->errorString();
        port->clearError();
        return -3;
    }
    QThread::msleep(RS485_WAIT_TIME); //等待数据接收完成
    timestamp = get_current_time();
    QByteArray array = port->readAll(); //读取缓冲区内所有数据
    if (array.isEmpty())
    {
        qDebug() << __func__ << ": Serial port recv 0 bytes.";
        state = RS485_NO_REPLY;
        return -4;
    }
    else if (array.size() != 6)
    {
        qDebug() << __func__ << ": Serial port does not recv enough bytes.";
        return -5;
    }
    unsigned char *recv_buf = reinterpret_cast<unsigned char *>(array.data());
    if (recv_buf[0] != addr || recv_buf[1] != 0x03)
    {
        qDebug() << __func__ << ": Serial port recv error.";
        return -6;
    } 
    crc = crc_16b(recv_buf, 4);
    if (crc != (recv_buf[4] + recv_buf[5] * 256))
    {
        qDebug() << __func__ << ": Serial port recv crc error.";
        return -7;
    }

    unsigned short time = recv_buf[2] * 256 + recv_buf[3];
    angle = time_to_angle(time); //时间转换为角度
    state = RS485_OK; //通讯正常
    return 0; 
}

/****************************************************************
@Function: Set the mode of controller
@Param addr: Device address of controller on RS485 bus
@Param mode: Mode of controller, can be MODE_AUTO or MODE_MANUAL
@Return: 0 for success , <0 value for errors;
****************************************************************/
int Controller::set_mode(unsigned char mode)
{
    unsigned char send_buf[8];
    if (!port->isOpen())
        return -1;
    send_buf[0] = addr;
    send_buf[1] = 0x06;
    send_buf[2] = 0xA0;
    send_buf[3] = 0x02;
    send_buf[4] = 0x00;
    send_buf[5] = mode ;

    unsigned short crc = crc_16b(send_buf, 6);
    send_buf[6] = static_cast<unsigned char>(crc); //CRC低字节
    send_buf[7] = static_cast<unsigned char>(crc >> 8);//CRC高字节

    port->clear();
    qint64 num = port->write(reinterpret_cast<const char *>(send_buf), 8);
    if(port->waitForBytesWritten(RS485_WRITE_TIMEOUT))
    {
        qDebug() << __func__ << ": Serial port write successful";
    }else
    {
        qDebug() << __func__ << ": Serial port write timeout. err= " << port->errorString();
        port->clearError();
        return -2;
    }
    qDebug() << __func__ << ": Serial port send" << num <<" bytes.";
    if (addr == 0) //向广播地址发送数据时没有返回
        return 0;

    if(port->waitForReadyRead(RS485_READ_TIMEOUT))
    {
        qDebug() << __func__ << ": Serial port received some data";
    }else
    {
        qDebug() << __func__ << ": Serial port read timeout. err= " << port->errorString();
        port->clearError();
        return -3;
    }
    QThread::msleep(RS485_WAIT_TIME);
    timestamp = get_current_time();

    QByteArray  array = port->readAll();
    if(array.isEmpty())
    {
        state = RS485_NO_REPLY;
        return -4;
    }else if(array.size() != 6)
        return -5;
    unsigned char *recv_buf = reinterpret_cast<unsigned char *>(array.data());
    if (recv_buf[0] != addr || recv_buf[1] != 0x06)
        return -6;
    state = RS485_OK;
    return 0;  
}


/**************************************************************************
@Function: Get current angle of the board
@Param addr: Device address of the controller on RS485 bus
@Param angle: Point the data to store the mode;
@Return: 0 for success, <0 value for error;
**************************************************************************/
int Controller::get_mode()
{
    assert(addr <= 247);
    unsigned char send_buf[6];
    if (!port->isOpen())
        return -1;
    send_buf[0] = addr;
    send_buf[1] = 0x03;
    send_buf[2] = 0xA0;
    send_buf[3] = 0x02;
    unsigned short crc = crc_16b(send_buf, 4);
    send_buf[4] = static_cast<unsigned char>(crc); //CRC低字节
    send_buf[5] = static_cast<unsigned char>(crc >> 8);//CRC高字节

    port->clear();
    qint64 num = port->write(reinterpret_cast<const char*>(send_buf), 6);
    if(port->waitForBytesWritten(RS485_WRITE_TIMEOUT))
    {
        qDebug() << __func__ << ": Serial port write successful.";
    }else
    {
        qDebug() << __func__ << ": Serial port write timeout. err= " << port->errorString();
        port->clearError();
        return -2;
    }
    qDebug() << __func__ << ": Serial port send" << num <<" bytes.";

    if(port->waitForReadyRead(RS485_READ_TIMEOUT))
    {
        qDebug() << __func__ << ": Serial port received some data";
    }else
    {
        qDebug() << __func__ << ": Serial port receive timeout. err= " << port->errorString();
        port->clearError();
        return -3;
    }
    QThread::msleep(RS485_WAIT_TIME);
    QByteArray array = port->readAll();
    if (array.isEmpty())
    {
        qDebug() << __func__ << ": Serial port recv 0 bytes.";
        state = RS485_NO_REPLY;
        return -4;
    }
    else if (array.size() != 6)
    {
        qDebug() << __func__ << ": Serial port does not recv enough bytes.";
        return -5;
    }

    unsigned char *recv_buf = reinterpret_cast<unsigned char*>(array.data());
    if (recv_buf[0] != addr || recv_buf[1] != 0x03)
        return -6;
    crc = crc_16b(recv_buf, 4);
    if (crc != (recv_buf[4] + recv_buf[5] * 256))
        return -7;
    mode = recv_buf[3];
    state = RS485_OK;
    return 0;
}

/***************************************************************************
@Function: Synchronize current angle and mode of controller to server
@Return: 0 for success, -1 when get_angel failed, -2 when get_mode failed
***************************************************************************/
int Controller::synchroize()
{
    int ret = 0;
    ret = get_angle();
    if (ret != 0)
        return -1;
    QThread::msleep(200);
    ret = get_mode();
    if (ret != 0)
        return -2;
    return 0;
}

/**********************************************************************
@Function: Set mode and angle of the controller
@Param mode: Mode want to set the controller
@Param angle: Angle want to set the controller
@Return: 0 for success, -1 or -2 for failed
**********************************************************************/
int Controller::flush(unsigned char mode, unsigned char angle)
{
    int ret = 0;
    ret = set_mode(mode);
    if (ret != 0)
        return -1;
    QThread::msleep(200);
    ret = set_angle(angle);
    if (ret != 0)
        return -2;
    return 0;
}
