#ifndef HARDWARE_H
#define HARDWARE_H

#include <QTextStream>
#include <QFile>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QDebug>
#include <QObject>
#include <QSerialPortInfo>
#include <QStackedWidget>

// 前照大灯
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

// 警报器
class beep {

public:
    beep();
    int on();               // 开灯函数
    int off();              // 关灯函数
    int GetStatus();        // 得到状态

    int status;

private:
    QFile file;             // 文件对象
    QTextStream *filein;    // 读取文件



};

//串口
class SerialManger : public QObject
{
    Q_OBJECT
public:
    SerialManger();

    void Serialsenddata(QByteArray &buf);
    QByteArray Serialgetdata();
    void SerialgetConfiguration(qint32 &baud, QSerialPort::DataBits &dataBits,
                                QSerialPort::StopBits &stopBits, QSerialPort::Parity &parity,
                                QSerialPort::FlowControl &flow);

public:
    //串口对象
    QSerialPort *serialPort;
};


// 硬件集合
class Hardware {

public:
    void init();            // 用于初始化

    light *Mlight;          // 实例化前照灯
    beep  *Mbeep;           // 实例化警报器
    SerialManger *smr;      // 实例化Serial


};


// 全局处理Hardware 在任何地方都可以调用
extern Hardware MyHardware;

#endif // HARDWARE_H
