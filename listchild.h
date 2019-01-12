#ifndef LISTCHILD_H
#define LISTCHILD_H

#include <QWidget>
#include <QTableWidget>
#include "controller.h"

class ListChild : public QWidget
{
    Q_OBJECT
public:
    explicit ListChild(QWidget *parent = nullptr);
    ListChild(QWidget *parent, QList<Controller *>* list);
    void paintEvent(QPaintEvent* event);

signals:

public slots:
    void updateInfoSlot(int index);
private:
    QList<Controller *>* list;

    void tableInit();
public:
    QTableWidget *tableWidget;
};

#endif // LISTCHILD_H
