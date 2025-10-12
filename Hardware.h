#ifndef HARDWARE_H
#define HARDWARE_H

#include <QTextStream>
#include <QFile>

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




};

// 硬件集合
class Hardware {

public:
    void init();

    light *Mlight;


};

extern Hardware MyHardware;

#endif // HARDWARE_H
