#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <comthread.h>
#include <QtSerialPort/QSerialPort>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    listChild = nullptr;
    //ui->setupUi(this);
    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    //setContentsMargins(0,0,0,0);
//    child = new MdiChild;
//    mdiArea->addSubWindow(child);

 //   view = new ViewChild;
//    mdiArea->addSubWindow(view);
 //   view->show();
}

MainWindow::~MainWindow()
{    
    delete ui;
    if(!listController.isEmpty())
    {
        //将动态创建的Controller对象全部释放
        foreach (auto controller, listController) {
            delete controller;
        }
        listController.clear(); //清空列表
    }
}


void MainWindow::Init()
{


    //MdiChild *child = createMdiChild();
    //child->newFile();
    child->show();
//child->showMaximized();
//child->showMaximized();
    //child->setSi
    //child->resize(geometry().width(), geometry().height());
    //child->setGeometry(geometry());
    //showMaximized();
    //showFullScreen();
    //child->setWindowFlag(Qt::Window);
     //
    //setFixedSize(width(), height());
    setWindowTitle(tr("智能遮阳百叶控制系统"));
    createActions();
    //child->move(mdiArea->x(), mdiArea->y());

 //   connect(this, SIGNAL(sizeSignal()), this, SLOT(sizeSlot()));
  //  connect(this, SIGNAL(sizeSignal2()), this, SLOT(sizeSlot2()));
   // emit sizeSignal();
    //openSerialPort();

}

/**************************************************
 * Function: Get controller information form database
 * Param list: Reference to the controller list
 * Return: number of controller in dabatase if success, <0 if error;
 * *************************************************/
int MainWindow::loadControllerInfo(QList<Controller*> &list)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(tr("192.168.90.200"));
    db.setDatabaseName(tr("shade_control"));
    db.setUserName(tr("root"));
    db.setPassword(tr("jns12345"));
    db.setPort(3306);
    bool bRet = db.open();

    if(!bRet)
    {
        qDebug() << __func__ << tr("Open mysql database failed");
        QSqlError err = db.lastError();

        QMessageBox::information(this, tr("提示"), tr("打开MySQL数据库失败:")+err.driverText() +tr(",") + err.databaseText(), QMessageBox::Ok);
        return -1;
    }
    qDebug() << __func__  << tr("Open mysql database success");
    QSqlQuery query(db);
    bRet = query.exec(tr("select * from board order by id asc"));
    if(!bRet)
    {
        QMessageBox::information(this, tr("提示"), tr("查询MySQL数据库失败"), QMessageBox::Ok);
        return -2;
    }
    list.clear();//Clear Controller list
    unsigned char addr = 0;

    //从数据库中获取信息添加到列表中
    while(query.next())
    {
        addr = static_cast<unsigned char>(query.value(4).toUInt());
        Controller *dev = new Controller(port, addr);
        dev->floor = static_cast<unsigned char>(query.value(1).toUInt());
        dev->pos = static_cast<unsigned char>(query.value(2).toUInt());
        dev->posNmae = query.value(3).toString();
        dev->mode = static_cast<unsigned char>(query.value(5).toUInt());
        dev->angle = static_cast<unsigned char>(query.value(6).toUInt());
        dev->enable = static_cast<unsigned char>(query.value(7).toUInt());
        list.push_back(dev);        
    }
    query.clear();
    db.close();
    qDebug() << __func__ << tr("Close database");
    return list.size(); // Return the number of controller
}

/****************************************************
 * Function: Open serial port
 * Param portName: serial port name like "COM2"
 * Return: true for successful, or false for failed;
 * **************************************************/
bool MainWindow::openSerialPort(QString portName)
{
    port = new QSerialPort();
    port->setPortName(portName);
    if(!port->open(QIODevice::ReadWrite))
    {        
        QMessageBox::information(this, tr("提示"),
                                 tr("打开串口失败，请检查串口是否被占用"),
                                 QMessageBox::Ok);
        return false;
    }
    qDebug() << (tr("打开串口成功"));
    port->setBaudRate(QSerialPort::Baud9600);    
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);
    return true;
}

void MainWindow::createChild(QList<Controller *> & list)
{
    listChild = new ListChild(this, &list);
    mdiArea->addSubWindow(listChild);
    listChild->show();//显示表格视图
}

void MainWindow::start()
{
    if(!openSerialPort(tr("COM2")))
        return;
    int num = loadControllerInfo(listController);
    if(num < 0)
        return;
    else if(num == 0)
    {
        QMessageBox::information(this, tr("提示"),
                                 tr("数据库中没有控制器信息，请先添加控制器"),
                                 QMessageBox::Ok);
        exit(0);
    }

    createActions();
    createChild(listController);



    //启动多线程与控制器通讯
    ComThread *th = new ComThread(port, &listController, this);
    port->moveToThread(th);
    connect(th, SIGNAL(updateInfoSignal(int)), listChild, SLOT(updateInfoSlot(int)));
    th->start();
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    fileToolBar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);//taoxiaochao added
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    newAct->setText(tr("列表显示"));//taoxiaochao added
    connect(newAct, &QAction::triggered, this, &MainWindow::showTable);


    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);
    //fileToolBar->setBackgroundRole(QPalette::Background)


    const QIcon openIcon = QIcon::fromTheme("open-file", QIcon(":/images/open.png"));
    newAct = new QAction(openIcon, tr("&Open"), this);
    newAct->setShortcuts(QKeySequence::Open);
    newAct->setStatusTip(tr("Open a file"));
    newAct->setText(tr("图形显示"));//taoxiaochao added
    fileToolBar->addAction(newAct);
    connect(newAct, &QAction::triggered, this, &MainWindow::showImage);
    fileToolBar->setStyleSheet("background-color:#FFFFFF");

}


void MainWindow::resizeEvent(QResizeEvent* size)
{
 //   qDebug() << QString("resizeEvent, size=") << size;
    //mdiArea->resize(frameGeometry().size());
    mdiArea->showMaximized();
    if(listChild != nullptr)
        listChild->showMaximized();

       // child->showMaximized();

       // view->showMaximized();
   // mdiArea->resize(size->size());
    //child->resize(frameGeometry().size());
}

void MainWindow::showTable()
{
    view->hide();
    child->showMaximized();

}

void MainWindow::showImage()
{
    child->hide();
    view->showMaximized();
}

void MainWindow::updateDataSlot(int addr)
{
    qDebug() << "recv singal updateData, addr=" << addr;
}
