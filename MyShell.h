#ifndef MYSHELL_H
#define MYSHELL_H

#include <QTextStream>
#include <QDebug>
#include <QThread>

#include "Hardware.h"

// 定义最大命令个数
#define MaxCommand 10

extern QTextStream in;
extern QTextStream out;

class InerCommand {

public:
    InerCommand();

    void help(int argc, QString argv[MaxCommand]);
    void light(int argc, QString argv[MaxCommand]);

private:

};

// shell类 用于调试各个外设或者功能
class VechilcleShell : public QThread {

public:
    VechilcleShell();

private:

    InerCommand Mycom;

    void Shell();
    int DivCommand(QString Command, QString argv[]);

    void run() override {

        this->Shell();

    }

};

#endif // MYSHELL_H
