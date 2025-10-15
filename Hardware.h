#ifndef HARDWARE_H
#define HARDWARE_H

#include <QTextStream>
#include <QFile>
#include <QTimer>

/*
 *  @ 前照大灯控制类
 *  通过sysfs控制LED灯光
 */
class light {
public:
    light();
    int on();               // 开灯函数
    int off();              // 关灯函数
    int GetStatus();        // 得到状态
    int status;             // 状态变量

private:
    QFile file;             // 文件对象
    QTextStream *filein;    // 读取文件
};

/*
 *  @ 警报器控制类
 *  通过sysfs控制蜂鸣器
 */
class beep {
public:
    beep();
    int on();               // 打开警报器
    int off();              // 关闭警报器
    int GetStatus();        // 得到状态
    int status;             // 状态变量

private:
    QFile file;             // 文件对象
    QTextStream *filein;    // 读取文件
};

/*
 *  @ AP3216C三合一传感器类
 *  集成环境光传感器(ALS)、接近传感器(PS)和红外传感器(IR)
 *  通过sysfs接口读取传感器数据
 */
class Ap3216c : public QObject
{
    Q_OBJECT

public:
    explicit Ap3216c(QObject *parent = 0);
    ~Ap3216c();

    // 控制数据采集
    void setCapture(bool str);

    // 读取传感器数据
    QString alsData();
    QString psData();
    QString irData();

    // 获取传感器状态
    int GetStatus();

private:
    QTimer *timer;          // 数据采集定时器
    QString alsdata;        // 环境光传感器数据
    QString psdata;         // 接近传感器数据
    QString irdata;         // 红外传感器数据
    int status;             // 传感器状态：0-停止，1-运行

    // 从硬件读取原始数据
    QString readAlsData();
    QString readPsData();
    QString readIrData();

private slots:
    // 定时器超时槽函数 - 读取并更新传感器数据
    void timer_timeout();

signals:
    // 传感器数据变化信号
    void ap3216cDataChanged();
};

/*
 *  @ 硬件集合类
 *  统一管理所有硬件设备
 *  作为底层调用，所有界面都可以调用硬件层
 */
class Hardware {
public:
    void init();            // 用于初始化所有硬件

    light *Mlight;          // 实例化前照灯
    beep  *Mbeep;           // 实例化警报器
    Ap3216c *MAp3216c;      // 实例化AP3216C传感器
};

// 全局处理Hardware 在任何地方都可以调用
extern Hardware MyHardware;

#endif // HARDWARE_H
