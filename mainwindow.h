#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QResizeEvent>
#include "mdichild.h"
#include "viewchild.h"
#include "listchild.h"
#include <QtSerialPort/QSerialPort>
#include <controller.h>

namespace Ui {
class MainWindow;
}
class QMdiArea;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void Init();
    bool openSerialPort(QString portName);
    int loadControllerInfo(QList<Controller*> &list);
    void start();

private:
    Ui::MainWindow *ui;
private:
    void createActions();
    QMdiArea *mdiArea;
    QAction *newAct;
    void resizeEvent(QResizeEvent* size);
    MdiChild *child;
    ViewChild *view;
    ListChild *listChild;

private:
  //  Ui::MainWindow *ui;
    QSerialPort *port;
    QList<Controller*> listController;
    void createChild(QList<Controller *> &list);
private slots:
   // void sizeSlot();
   // void sizeSlot2();
    void showTable();
    void showImage();
public slots:
    void updateDataSlot(int addr);
};

#endif // MAINWINDOW_H
