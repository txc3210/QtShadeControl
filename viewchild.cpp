#include "viewchild.h"
#include <QDebug>
#include <QPainter>
#include <QImage>
#include <QPixmap>
#include <QBitmap>
#include <QPicture>


ViewChild::ViewChild()
{

}


void ViewChild::paintEvent(QPaintEvent * event)
{
  //  Q_UNUSED(event);



    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    painter.drawRect(rect());
    QImage image;
    bool bRet = image.load("images/shade_bk.png");
    if(!bRet)
        qDebug() << (tr("加载文件失败"));
    qDebug() << image.size() << image.format() << image.depth();
   // image.scaled(200, 100);
    painter.drawImage(QPoint(0, 0), image);


}
