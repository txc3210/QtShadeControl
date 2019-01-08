#ifndef VIEWCHILD_H
#define VIEWCHILD_H

#include <QWidget>
#include <QGraphicsView>

class ViewChild : public QWidget
{
    Q_OBJECT
public:
    ViewChild();
private:
    void paintEvent(QPaintEvent * event);

signals:

public slots:
};

#endif // VIEWCHILD_H
