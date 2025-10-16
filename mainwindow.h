#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QStackedWidget>
#include <QPixmap>
#include <QBuffer>
#include <QTimer>

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

//    QRadioButton *radioLight;
//    QPushButton *pushButton;


    //哨兵模式
    QTimer *sentryTimer;            // 用于轮询传感器的定时器
    bool isSentryModeActive;        // 用于启用/禁用此模式的标志
    int baselinePsvalue;            // 存储没有物体靠近时的正常PS值
    int psAlertThreshold;           // 触发警报的PS值变化阈值
    int psTriggerThreshold;         // 触发拍照的更高变化阈值

private slots:
    //void pushButtonClicked();

    void checkSentrySensors();      // 将由定时器调用的槽函数
    void toggleSentryMode();        // 用于开启/关闭模式的槽函数

public slots:
    void  setPage(int idx);

};
#endif // MAINWINDOW_H
