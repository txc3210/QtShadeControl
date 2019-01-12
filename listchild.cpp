#include "listchild.h"
#include <QTableWidget>
#include <QHeaderView>
#include <assert.h>
#include <QVBoxLayout>
#include <controller.h>
#include <QDebug>
#include <QPainter>

#define TABLE_COL_NUM   8
//#define TABLE_ROW_NUM   43

ListChild::ListChild(QWidget *parent)
{

}

ListChild::ListChild(QWidget *parent, QList<Controller *>* list)   
{

    this->list = list;
    tableInit();//创建表格
    QVBoxLayout *layout = new  QVBoxLayout;
    layout->addWidget(tableWidget );
    layout->setContentsMargins(2,2,2,2);
    setLayout(layout);
}

/**********************************************************************
 * Function: Create a tabel initialized from the controller list
 * Param: None
 * Return: None
 * ******************************************************************/
void ListChild::tableInit()
{
    assert(list->size() > 0);
    tableWidget = new QTableWidget(list->size(), TABLE_COL_NUM, this);
    QStringList header;
    header << tr("楼层") << tr("百叶位置") << tr("控制模式") ;
    header << tr("当前角度") << tr("通讯状态") << tr("设备地址");
    header << tr("启用状态") << tr("时间戳");
    tableWidget->setHorizontalHeaderLabels(header);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//改为只读模式
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);//选中整行
    tableWidget->verticalHeader()->setVisible(false);//隐藏垂直表头

    //Set  column width of table
    for(int col = 1; col < TABLE_COL_NUM; ++col)
    {
        tableWidget->setColumnWidth(col, 150);
    }
    //Set row height of table
    for(int row = 0; row < list->size(); ++row)
    {
        tableWidget->setRowHeight(row, 20);
    }
    tableWidget->show();

    int row = 0;
    foreach (auto controller, *list) {
        //显示楼层
        QTableWidgetItem *item = new QTableWidgetItem(QString().sprintf("%dF",controller->floor));
        item->setTextAlignment(Qt::AlignCenter);
        tableWidget->setItem(row, 0, item);
        //显示安装位置
        item = new QTableWidgetItem(controller->posNmae);
                //item->setTextAlignment(Qt::AlignLeft);
                tableWidget->setItem(row, 1, item);
        //显示控制模式
        item = new QTableWidgetItem(controller->getModeDesc());
                item->setTextAlignment(Qt::AlignCenter);
                tableWidget->setItem(row, 2, item);
        //显示角度
        item = new QTableWidgetItem(QString().sprintf("%d",controller->angle));
                item->setTextAlignment(Qt::AlignCenter);
                tableWidget->setItem(row, 3, item);
        //显示通讯状态
        item = new QTableWidgetItem(controller->state == RS485_OK? tr("正常") : tr("无响应"));
                item->setTextAlignment(Qt::AlignCenter);
                tableWidget->setItem(row, 4, item);

        //显示RS485地址
        item = new QTableWidgetItem(QString().sprintf("%d",controller->addr));
                item->setTextAlignment(Qt::AlignCenter);
                tableWidget->setItem(row, 5, item);

        //显示设备启用状态
        item = new QTableWidgetItem(controller->enable == ENABLE? tr("已启用") : tr("已禁用"));
                item->setTextAlignment(Qt::AlignCenter);
                tableWidget->setItem(row, 6, item);

        //显示时间戳
        item = new QTableWidgetItem(controller->timestamp);
                item->setTextAlignment(Qt::AlignCenter);
                tableWidget->setItem(row, 7, item);
        ++row;
    }

    QPalette pal = this->palette();
    pal.setBrush(this->backgroundRole(),QBrush(QPixmap(":/images/background.png")) );
    this->setPalette( pal );
  //  QPalette pll = tableWidget->palette();
  //  pll.setBrush(QPalette::Base,QBrush(QColor(0,255,255,0)));
  //  tableWidget->setPalette(pll);
}

/*********************************************************************
 * Function: Slot to update table item
 * Param index: Index of the controller in list which need to be update
 * Return: None
 * ******************************************************************/
void ListChild::updateInfoSlot(int index)
{
    Controller *controller = list->at(index);
    //更新控制模式
    tableWidget->item(index, 2)->setText(controller->getModeDesc());
    //更新当前角度
    tableWidget->item(index, 3)->setText(controller->getAngleDesc());
    //更新通讯状态
    tableWidget->item(index, 4)->setText(controller->state == RS485_OK? tr("正常") : tr("无响应"));
    //更新时间戳
    tableWidget->item(index, 7)->setText(controller->timestamp);
    //改变当前行的背景颜色
    tableWidget->item(index, 7)->setBackgroundColor(QColor(224, 153, 230));
    //恢复前一行的背景颜色
    tableWidget->item(index == 0 ? list->size() -1 : index -1, 7)->setBackgroundColor(QColor(255, 255, 255));

}

void ListChild::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(220, 148, 243));
    painter.setBrush(QBrush(QPixmap(":/images/background.png")) );
    painter.drawRect(rect());
}
