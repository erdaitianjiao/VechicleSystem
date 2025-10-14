#ifndef MYSHELL_H
#define MYSHELL_H

#include <QTextStream>
#include <QDebug>
#include <QThread>

#include "Hardware.h"

// 定义最大命令个数
#define MaxCommand 10

// 标出化出入输出流
extern QTextStream in;
extern QTextStream out;

/*
 * 内置命令实现
 * 仿shell设计 通过 argc argv进行传参
 * 实现help 和 其他调试功能
 */
class InerCommand {

public:
    InerCommand();

    void help(int argc, QString argv[MaxCommand]);          // 帮助命令
    void light(int argc, QString argv[MaxCommand]);         // 前照灯控制命令
    void beep(int argc, QString argv[MaxCommand]);          // 警报器控制命令

private:

};

/*
 *  实例化shell
 *  用于读取用户shell输入和调用命令
 */

// shell类 用于调试各个外设或者功能
class VechilcleShell : public QThread {

public:
    VechilcleShell();       // 构造函数

private:

    InerCommand Mycom;      // 实例化一个命令

    void Shell();           // 实现用户读取 调用命令并传参

    // 读取用户输入的命令 返回参数个数和参数列表
    int DivCommand(QString Command, QString argv[]);

    // 继承的QThread的函数 目的是开一个新的线程 运行shell
    void run() override {

        this->Shell();

    }

};

#endif // MYSHELL_H
