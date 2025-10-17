#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QPushButton>
#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTextBrowser>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QGridLayout>
#include <QMessageBox>
#include <QDebug>
#include <QObject>
#include <QSerialPortInfo>
#include <QStackedWidget>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QFont>
#include <QTimer>
#include <QDateTime>
#include "Hardware.h"

/*
 *  @ page文件
 *  所有的界面都在这里定义
 *
 */

#define AppNum     4

class Camera;

// 前置声明，避免循环引用
//class QScrollArea;
//class QLabel;
//class QHBoxLayout;
//class QVBoxLayout;
//class QComboBox;
//class QPushButton;
//class Camera;

// 测试界面
class HomePage : public QWidget
{

    Q_OBJECT

public:
    HomePage();

private:
    QPushButton *pushButton[AppNum]; // 按钮 用来选择应用
    QPushButton *rightbutton;   //右边的按钮 用来到主界面
    QPushButton *test1;         // 测试按钮 可删除

signals:
    void goSerial();
    void goCamera();
    void goAp3216C();
    void goSentryMode();
    void goMainPage();

};

// 主界面
class MapPage : public QWidget
{
    Q_OBJECT

public:
    MapPage();

private:

    QPushButton *test2;             // 测试按钮 可删除
    QPushButton *button;

signals:
    void goSentryMode();
    void goHome();          //返回信号

};

// 串口的界面
class Serialpage : public QWidget
{
    Q_OBJECT

public:
    Serialpage();

private:
    //用作接收数据
    QTextBrowser *textBrowser;
    //用作发送数据
    QTextEdit *textEdit;
    //按钮
    QPushButton *pushButton[2];
    //退出按钮
    QPushButton *btn;
    //下拉选择盒子
    QComboBox *comboBox[5];
    //标签
    QLabel *label[5];
    //垂直布局
    QVBoxLayout *vboxLayout;
    // 顶部放一个水平布局，只放按钮
    QHBoxLayout *topLayout;
    // 创建主垂直布局
    QVBoxLayout *mainLayout;
    //网格布局
    QGridLayout *gridLayout;
    //主布局
    QWidget *mainWidget;
    //设置功能区域
    QWidget *funcWidget;
    //布局初始化
    void Ser_layoutInit();
    //扫描系统可用串口
    void scanSerialport();
    //波特率项初始化
    void baudRateInit();
    //数据位项初始化
    void dataBitsItemInit();
    //检验位项初始化
    void parityItemInit();
    //停止位项初始化
    void stopBitsItemInit();

    QPushButton *test3;     // 测试按钮 可删除

signals:
    void goHome();          //返回信号

private slots:
     void sendPushButtonClicked();
     void openSerialPortPushButtonClicked();
     void serialPortReadyRead();
     //void SerialpagetoMianpage();
};

// 摄像机
class CameraPage : public QWidget
{
    Q_OBJECT

public:
    explicit CameraPage(QWidget *parent = nullptr);

private:
    /* 主容器，此类本身就是一个QWidget容器 */

    /* 滚动区域，方便开发高分辨率 */
    QScrollArea *scrollArea;

    /* 将采集到的图像使用Widget显示 */
    QLabel *displayLabel;

    /* 界面右侧区域布局 */
    QHBoxLayout *hboxLayout;

    /* 界面右侧区域布局 */
    QVBoxLayout *vboxLayout;

    /* 界面右侧区域容器 */
    QWidget *rightWidget;

    /* 界面右侧区域显示拍照的图片 */
    QLabel *photoLabel;

    /* 界面右侧区域摄像头设备下拉选择框 */
    QComboBox *comboBox;

    /* 两个按钮，一个为拍照按钮，另一个是开启摄像头按钮 */
    QPushButton *pushButton[2];

    //  关闭按钮
    QPushButton *closeButton;

    /* 拍照保存的照片 */
    QImage saveImage;

    /* 摄像头设备 */
    Camera *camera;

    /* 布局初始化 */
    void Camer_layoutInit();

    /* 扫描是否存在摄像头 */
    void scanCameraDevice();

public slots:
    void takePicture();
    void startCameraFeed();
    void stopCameraFeed();

private slots:
    /* 显示图像 */
    void showImage(const QImage&);

    /* 设置按钮文本 */
    void setButtonText(bool);

    /* 保存照片到本地 */
    void saveImageToLocal();

signals:
    void goHome();          //返回信号
};

// Ap3216c页面 - 修改：整合test_for_Ap3216c的两个页面，同时保留原有结构
class Ap3216cPage : public QWidget {
    Q_OBJECT  // 新增：需要信号槽支持

public:
    Ap3216cPage();
    ~Ap3216cPage();

private:
    QPushButton *btn;

    // 新增：内部页面管理
    QStackedWidget *internalStackedWidget;
    QWidget *ap3216cMainPage;      // 传感器系统主页面
    QWidget *ap3216cSensorPage;    // 传感器数据页面

    // 新增：主页面组件
    QLabel *titleLabel;
    QLabel *descLabel;
    QPushButton *sensorBtn;

    // 新增：传感器页面组件
    QPushButton *backBtn;
    QLabel *sensorTitle;
    QGridLayout *sensorGridLayout;
    QLabel *alsLabel, *psLabel, *irLabel;
    QLabel *alsValue, *psValue, *irValue;

    // 新增：布局
    QVBoxLayout *mainLayout;
    QVBoxLayout *sensorLayout;

    void Ap32_layoutInit();

private slots:
    // 新增：内部页面切换槽函数
    void showSensorPage();
    void showMainPage();
    void updateSensorData();  // 更新传感器数据显示

signals:
    // 新增：用于通知主窗口可能需要的外部操作
    void sensorDataRequested(bool start);  // 请求开始/停止数据采集

    void goHome();          //返回信号

};
#endif // PAGE_H
