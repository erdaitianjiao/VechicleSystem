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
#include "Hardware.h"

/*
 *  @ page文件
 *  所有的界面都在这里定义
 *
 */


// 主页 展示基本信息等
class HomePage : public QWidget{

public:
    HomePage();

private:

    QPushButton *test1;     // 测试按钮 可删除

};

// 地图文件 原本想调用百度地图api 有点太难了 先搁置
class MapPage : public QWidget {

public:
    MapPage();

private:

    QPushButton *test2;             // 测试按钮 可删除

};

// 串口的界面
class Serialpage : public QWidget
{
    Q_OBJECT

public:
    Serialpage();

    //退出按钮
    QPushButton *btn;

private:
    //用作接收数据
    QTextBrowser *textBrowser;
    //用作发送数据
    QTextEdit *textEdit;
    //按钮
    QPushButton *pushButton[2];

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
    void layoutInit();
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
     void SerialpagetoMianpage();
};
#endif // PAGE_H
