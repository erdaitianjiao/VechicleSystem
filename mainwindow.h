#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QStackedWidget>
#include <QPixmap>
#include <QBuffer>

#include "camera.h"
#include "MyShell.h"
#include "Hardware.h"
#include "Page.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void myUiInit();

private:
    Ui::MainWindow *ui;

    // 创建Shell 可以通过shell进行page操作
    VechilcleShell *shell;


    // 创建ui界面 测试用
    // 创建一个页面容器 用于装页面和切换界面
    QStackedWidget *myPageTest;
    HomePage *Home;
    MapPage *Map;
    Serialpage *ser;
    CameraPage *crm;
    Ap3216cPage *Ap32;

    // 测试用

    QRadioButton *radioLight;
    QPushButton *pushButton;


private slots:
    void pushButtonClicked();

public slots:
    void  setPage(int idx);

};
#endif // MAINWINDOW_H
