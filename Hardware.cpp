#include "Hardware.h"
#include "MyShell.h"

#include <QString>

Hardware MyHardware;

// 车灯控制
light::light(){

#if __arm__

    system("echo none > /sys/class/leds/sys-led/trigger");

#endif

    this->file.setFileName("/sys/devices/platform/leds/leds/sys-led/brightness");

    if (!file.exists()) out << "can't init light" << endl;

    // 创建文件读取对象
    filein = new QTextStream(&file);

}

int light::GetStatus() {

    if (!file.exists()) return -1;

    if (!file.open(QIODevice::ReadWrite)) {

        out << file.errorString();
        this->status = -1;
        return -1;
    }

    QString buf = filein->readLine();

    if (buf == "1") {

        this->status = 1;

    } else if (buf == "0") {

        this->status = 0;

    } else {

        // 错误情况
        this->status = -1;

    }
    if (this->status != -1) out << "light status: " << this->status << endl;
    else out << "light status get wroing" << endl;

    file.close();

    return this->status;

}

int light::on() {

    if (!file.exists()) return -1;

    if (!file.open(QIODevice::ReadWrite)) {

        out << file.errorString();

    }

    file.write("1");

    file.close();

    return this->GetStatus();

}

int light::off() {

    if (!file.exists()) return -1;

    if (!file.open(QIODevice::ReadWrite)) {

        out << file.errorString();

    }

    file.write("0");

    file.close();

    return this->GetStatus();

}


// HardWare
void Hardware::init() {

    Mlight = new light();

}
