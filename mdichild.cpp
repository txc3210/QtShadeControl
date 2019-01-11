#include "mdichild.h"
#include <QDebug>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QtSql/QSqlDatabase>
#include <QPluginLoader>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>

#define TABLE_COL_NUM   7
#define TABLE_ROW_NUM   43

MdiChild::MdiChild()
{
    tableInit();
    QVBoxLayout *layout = new  QVBoxLayout;
    layout->addWidget(tableWidget );
    layout->setContentsMargins(2,2,2,2);
    setLayout(layout);
   //connect(this, SIGNAL(sizeSignal()), this, SLOT(sizeSlot()));
//  emit sizeSignal();
   //emit showMaximized();
   loadFromDatabase();
}

void MdiChild::tableInit()
{
    tableWidget = new QTableWidget(TABLE_ROW_NUM, TABLE_COL_NUM, this);
    QStringList header;
    header << tr("楼层") << tr("位置") << tr("控制模式") ;
    header << tr("当前角度") << tr("通讯状态") << tr("RS485地址");
    header << tr("启用");
    tableWidget->setHorizontalHeaderLabels(header);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//改为只模式
//    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选中整行
    tableWidget->verticalHeader()->setVisible(false);//隐藏垂直表头

    tableWidget->setColumnWidth(0,60);
    for(int i = 1; i < TABLE_COL_NUM; ++i)
    {
        tableWidget->setColumnWidth(i, 150);
    }

    for(int i = 0; i < TABLE_ROW_NUM; ++i)
    {
        tableWidget->setRowHeight(i,20);
    }
    //tableWidget->sett
    //tableWidget->move(this->geometry().x() + 2, this->geometry().y() + 2);
    tableWidget->show();
    //tableWidget->showMaximized();
   // tableWidget->move(this->geometry().x(), this->geometry().y());
   // tableWidget->setFixedHeight(this->geometry().height());
   // tableWidget->setFixedWidth(this->geometry().width());
   // tableWidget->setFixedSize(1200,700);
    //setContentsMargins(2,2,2,2);
}

void MdiChild::sizeSlot()
{
    setWindowFlags(Qt::Window);
    //showMaximized();
    setWindowState(Qt::WindowMaximized);
    qDebug() << tr("showMaximized");
}


void MdiChild::loadFromDatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(tr("192.168.108.2"));
    db.setDatabaseName(tr("shade_control"));
    db.setUserName(tr("root"));
    db.setPassword(tr("jns12345"));
    db.setPort(3306);
    bool bRet = db.open();

    if(!bRet)
    {
        qDebug() << tr("Open mysql database failed");
        QMessageBox::information(this, tr("提示"), tr("打开MySQL数据库失败"), QMessageBox::Ok);
        return;
    }
    qDebug() << tr("Open mysql database success");
   // QMessageBox::information(this, tr("提示"), tr("打开MySQL数据库成功"), QMessageBox::Ok);
    QSqlQuery query(db);
    bRet = query.exec(tr("select * from board order by id asc"));
    if(!bRet)
    {
        QMessageBox::information(this, tr("提示"), tr("查询MySQL数据库失败"), QMessageBox::Ok);
        return;
    }
   // QMessageBox::information(this, tr("提示"), tr("查询MySQL数据库成功"), QMessageBox::Ok);
    int i = 0;
    while(query.next())
    {
 //       qDebug() << query.value(1).toString();
        QTableWidgetItem *item = new QTableWidgetItem(query.value(1).toString() + tr("F"));
        item->setTextAlignment(Qt::AlignCenter);
     //   item->setBackgroundColor(QColor(255, 255, 255));
        tableWidget->setItem(i, 0, item);
        //tableWidget->setItem(i, 1, new QTableWidgetItem(query.value(3).toString()));
        item = new QTableWidgetItem(query.value(3).toString());
        //item->setTextAlignment(Qt::AlignHLeft);
    //    item->setBackgroundColor(QColor(255, 255, 255));
        tableWidget->setItem(i, 1, item);
        int mode = query.value(5).toInt();
        QString strMode;
        if(mode == 0)
            strMode = tr("自动");
        else
            strMode = tr("手动");
        item = new QTableWidgetItem(strMode);
        item->setTextAlignment(Qt::AlignCenter);
    //    item->setBackgroundColor(QColor(255, 255, 255));
        tableWidget->setItem(i, 2, item);
        //当前角度
        item = new QTableWidgetItem(query.value(6).toString());
        item->setTextAlignment(Qt::AlignCenter);
    //    item->setBackgroundColor(QColor(255, 255, 255));
        tableWidget->setItem(i, 3, item);
        ++i;
    }
    query.clear();
    db.close();
//    tableWidget->setSpan(0, 4, 2, 2);
    QTableWidgetItem *item = new QTableWidgetItem(tr("室内光照度"));
    item->setTextAlignment(Qt::AlignCenter);
    item->setBackgroundColor(QColor(255, 255, 255));
    tableWidget->setItem(0, 4, item);
 //   tableWidget->setStyleSheet("QTableView::Item{background-color:#FFFFFF}");
    tableWidget->setStyleSheet("QTableView::Item{border: 2px solid #FF0000}");
  //  tableWidget->setStyleSheet("border: 2px solid #FF0000");
    tableWidget->setStyleSheet("background-color:#FFFFFF");
}

void MdiChild::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(rect());
}
