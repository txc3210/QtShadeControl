#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>
#include <comthread.h>
#include <QtSerialPort/QSerialPort>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //ui->setupUi(this);
    mdiArea = new QMdiArea(this);
    mdiArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    mdiArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setCentralWidget(mdiArea);
    //setContentsMargins(0,0,0,0);
    child = new MdiChild;
    mdiArea->addSubWindow(child);

    view = new ViewChild;
    mdiArea->addSubWindow(view);
    view->show();
}

MainWindow::~MainWindow()
{    
    delete ui;
    if(!list.isEmpty())
    {
        foreach (auto controller, list) {
            delete controller;
        }
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
    openSerialPort();

}


void MainWindow::openSerialPort()
{
    port = new QSerialPort();
    port->setPortName(tr("COM2"));
    if(!port->open(QIODevice::ReadWrite))
    {
        QMessageBox::information(this, tr("提示"),
                                 tr("打开串口失败，请检查串口是否被占用"),
                                 QMessageBox::Ok);
        return;
    }
    qDebug() << (tr("打开串口成功"));
    port->setBaudRate(QSerialPort::Baud9600);
     //port.setBaudRate(ui->comBaud->currentText().toInt());
    port->setDataBits(QSerialPort::Data8);
    port->setParity(QSerialPort::NoParity);
    port->setStopBits(QSerialPort::OneStop);
    port->setFlowControl(QSerialPort::NoFlowControl);


    for(unsigned char addr = 1; addr <= 43; ++addr)
    {
        Controller *dev = new Controller(port, addr);
        list.push_back(dev);
    }

    ComThread *th = new ComThread(port, &list, this);
 //   connect(th, SIGNAL(updateDataSignal(int)), this, SLOT(updateDataSlot(int)));
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
    child->showMaximized();
    view->showMaximized();
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
