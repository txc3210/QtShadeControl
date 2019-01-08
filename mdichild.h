#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>
#include <QWindow>
#include <QGraphicsView>
#include <QMdiSubWindow>
#include <QTableWidget>

class MdiChild : public QWidget
{
    Q_OBJECT
public:
    MdiChild();
    void loadFromDatabase();
    void tableInit();
    void paintEvent(QPaintEvent* event);
private:
    QTableWidget *tableWidget;
private slots:
    void sizeSlot();

signals:
    void sizeSignal();
};

#endif // MDICHILD_H
