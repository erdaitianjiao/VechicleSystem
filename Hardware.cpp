#include "Hardware.h"
#include "MyShell.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <QString>


// 硬件类 所有的硬件都封装在这里面 体现程序分层调用
Hardware MyHardware;

/*
 *  @ 车灯控制的实现
 *
 *  含有初始化
 *  开灯关灯函数
 *  获取状态
 */

// 初始化车灯函数
light::light() {

#if __arm__

    // 关闭呼吸灯程序
    system("echo none > /sys/class/leds/sys-led/trigger");

#endif

    // 设置驱动目录
    this->file.setFileName("/sys/devices/platform/leds/leds/sys-led/brightness");

    if (!file.exists()) out << "can't init light" << endl;

    // 创建文件读取对象
    filein = new QTextStream(&file);

}

// 获取车灯状态 并在shell打印状态信息 更新 light->status 成员变量
int light::GetStatus() {

    // 检查文件存在
    if (!file.exists()) return -1;

    // 检查打开文件
    if (!file.open(QIODevice::ReadWrite)) {

        // 输出文件报错
        out << file.errorString();

        // 设置为未正确读取
        this->status = -1;
        return -1;
    }


    // 读取文件内容
    QString buf = filein->readLine();

    // 检查读取内容
    if (buf == "1") {

        this->status = 1;

    } else if (buf == "0") {

        this->status = 0;

    } else {

        // 错误情况
        this->status = -1;

    }

    // 如果正确读取 打印状态
    if (this->status != -1) out << "light status: " << this->status << endl;
    else out << "light status get wroing" << endl;


    // 关闭文件
    file.close();

    return this->status;

}


// 开灯函数 开灯并更新灯当前状态 打印在shell里面
int light::on() {

    // 检查文件存在
    if (!file.exists()) return -1;

    // 打开文件
    if (!file.open(QIODevice::ReadWrite)) {

        // 输出文件报错
        out << file.errorString();

    }

    // 向驱动写入数据
    file.write("1");

    // 关闭文件
    file.close();

    // 读取当前状态
    return this->GetStatus();

}

// 关灯函数
int light::off() {

    // 检查文件存在
    if (!file.exists()) return -1;

    // 打开文件
    if (!file.open(QIODevice::ReadWrite)) {


        // 输出文件报错
        out << file.errorString();

    }

    // 写入文件
    file.write("0");

    // 关闭文件
    file.close();

    return this->GetStatus();

}

/*
 *  @ 警报器函数的实现
 *
 *  含有初始化
 *  开警报关警报函数
 *  获取状态
 */

// 警报器初始化函数
beep::beep() {

    // 设置驱动路径
    this->file.setFileName("/sys/devices/platform/leds/leds/beep/brightness");

    // 检查文件存在
    if (!file.exists()) out << "can't init beep" << endl;

    // 创建文件读取对象
    filein = new QTextStream(&file);

}

// 得到警报器状态 并且在shell里面打印状态信息 并更新beep->status
int beep::GetStatus() {

    // 检查文件存在
    if (!file.exists()) return -1;


    // 打开文件
    if (!file.open(QIODevice::ReadWrite)) {

        // 打印文件错误信息
        out << file.errorString();
        this->status = -1;
        return -1;
    }

    // 读取驱动信息
    QString buf = filein->readLine();

    if (buf == "1") {

        this->status = 1;

    } else if (buf == "0") {

        this->status = 0;

    } else {

        // 错误情况
        this->status = -1;

    }

    // 如果读取没错 就打印beep的状态
    if (this->status != -1) out << "beep status: " << this->status << endl;
    else out << "beep status get wroing" << endl;


    // 关闭文件
    file.close();

    // 返回状态
    return this->status;

}


// 打开警报器 并更新警报器的状态
int beep::on() {

    // 检查文件存在
    if (!file.exists()) return -1;

    // 打开文件
    if (!file.open(QIODevice::ReadWrite)) {


        // 输出错误信息
        out << file.errorString();

    }

    // 写入文件
    file.write("1");

    // 关闭文件
    file.close();

    // 返回状态信息
    return this->GetStatus();

}

// 关闭警报器 并更新警报器的状态
int beep::off() {

    // 检查文件存在
    if (!file.exists()) return -1;

    // 打开文件
    if (!file.open(QIODevice::ReadWrite)) {

        // 输出文件错误信息
        out << file.errorString();

    }

    // 写入文件
    file.write("0");

    // 关闭文件
    file.close();

    // 返回状态
    return this->GetStatus();

}

/*
 *  @ 串口函数的实现
 *
 *  含有初始化
 *  串口发送与接收函数
 *  获取串口波特率，数据位等信息
 */

//初始化串口
SerialManger::SerialManger()
{
    serialPort = new QSerialPort(this);

}

//串口的发送
void SerialManger::Serialsenddata(QByteArray &buf)
{
    if(!serialPort->isOpen())
    {
        qWarning() << "串口未打开";
        return;
    }
    qint64 n = serialPort->write(buf);
    if(n == -1)
    {
        qWarning() << "串口写入失败";
       return;
    }
}

//串口的接收
QByteArray SerialManger::Serialgetdata()
{
    if(!serialPort->isOpen())
    {
        qWarning() << "串口未打开";
        return {};              //返回空 QByteArray
    }

    QByteArray buf = serialPort->readAll();

    if (serialPort->error() != QSerialPort::NoError)
    {
        qWarning() << "串口读取出错:" << serialPort->errorString();
        serialPort->clearError();
        return {};
    }
    else if(buf.isEmpty())
    {

    }

    return buf;     //后续要判断是否是空数组
}

//波特率，数据位，停止位，校验位,流控的获取
void SerialManger::SerialgetConfiguration(qint32 &baud, QSerialPort::DataBits &dataBits,
                            QSerialPort::StopBits &stopBits, QSerialPort::Parity &parity,
                            QSerialPort::FlowControl &flow)
{
    if(!serialPort->isOpen())
    {
        qWarning() << "串口未打开";
        return;
    }

    baud = serialPort->baudRate();          //波特率
    dataBits = serialPort->dataBits();      //数据位
    stopBits = serialPort->stopBits();      //停止位
    parity = serialPort->parity();          //校验位
    flow = serialPort->flowControl();       //流控

}

/*
 *  @ AP3216C构造函数
 *  初始化定时器和传感器状态
 */
Ap3216c::Ap3216c(QObject *parent) : QObject(parent)
{
    // 创建数据采集定时器
    timer = new QTimer();

    // 连接定时器超时信号
    connect(timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));

    // 初始状态为停止
    status = 0;
}

/*
 *  @ AP3216C析构函数
 *  停止数据采集并清理资源
 */
Ap3216c::~Ap3216c()
{
    // 停止数据采集
    setCapture(false);

    // 释放定时器
    delete timer;
}

/*
 *  @ 获取传感器状态
 *  返回传感器当前运行状态
 */
int Ap3216c::GetStatus()
{
    return status;
}

/*
 *  @ 定时器超时处理函数
 *  读取所有传感器数据并发射变化信号
 */
void Ap3216c::timer_timeout()
{
    // 读取三种传感器数据
    alsdata = readAlsData();
    psdata = readPsData();
    irdata = readIrData();

    // 通知界面更新数据
    emit ap3216cDataChanged();
}

/*
 *  @ 读取红外传感器数据
 *  通过sysfs接口从硬件读取IR数据
 */
QString Ap3216c::readIrData()
{
    char const *filename = "/sys/class/misc/ap3216c/ir";
    int err = 0;
    int fd;
    char buf[10];

    // 打开设备文件
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        close(fd);
        return "open file error!";
    }

    // 读取传感器数据
    err = read(fd, buf, sizeof(buf));
    if (err < 0) {
        close(fd);
        return "read data error!";
    }
    close(fd);

    // 处理读取的数据（去除换行符）
    QString irValue = buf;
    QStringList list = irValue.split("\n");
    return list[0];
}

/*
 *  @ 读取接近传感器数据
 *  通过sysfs接口从硬件读取PS数据
 */
QString Ap3216c::readPsData()
{
    char const *filename = "/sys/class/misc/ap3216c/ps";
    int err = 0;
    int fd;
    char buf[10];

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        close(fd);
        return "open file error!";
    }

    err = read(fd, buf, sizeof(buf));
    if (err < 0) {
        close(fd);
        return "read data error!";
    }
    close(fd);

    QString psValue = buf;
    QStringList list = psValue.split("\n");
    return list[0];
}

/*
 *  @ 读取环境光传感器数据
 *  通过sysfs接口从硬件读取ALS数据
 */
QString Ap3216c::readAlsData()
{
    char const *filename = "/sys/class/misc/ap3216c/als";
    int err = 0;
    int fd;
    char buf[10];

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        close(fd);
        return "open file error!";
    }

    err = read(fd, buf, sizeof(buf));
    if (err < 0) {
        close(fd);
        return "read data error!";
    }
    close(fd);

    QString alsValue = buf;
    QStringList list = alsValue.split("\n");
    return list[0];
}

/*
 *  @ 获取环境光传感器数据
 *  返回当前ALS数据值
 */
QString Ap3216c::alsData() {
    return alsdata;
}

/*
 *  @ 获取红外传感器数据
 *  返回当前IR数据值
 */
QString Ap3216c::irData() {
    return irdata;
}

/*
 *  @ 获取接近传感器数据
 *  返回当前PS数据值
 */
QString Ap3216c::psData() {
    return psdata;
}

/*
 *  @ 控制数据采集
 *  启动或停止传感器数据采集
 */
void Ap3216c::setCapture(bool str)
{
    if (str) {
        // 启动定时器，500ms间隔采集数据
        timer->start(500);
        status = 1;  // 设置状态为运行
    } else {
        // 停止定时器
        timer->stop();
        status = 0;  // 设置状态为停止
    }
}

/*
 * Hardware成员
 * 用于存放所有用到的硬件类
 * 作为底层调用 所有的page界面都可以调用硬件层
 * 包括调试用的shell
 */

// HardWare 初始化函数 初始化每个硬件成员
void Hardware::init()
{

    Mlight = new light();
    Mbeep = new beep();
    smr = new SerialManger();
    MAp3216c = new Ap3216c();
}
