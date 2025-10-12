#include "MyShell.h"
#include "Hardware.h"

#include <QDebug>
#include <QTextStream>
#include <QThread>

QTextStream in(stdin);
QTextStream out(stdout);

// Shell的实现
VechilcleShell::VechilcleShell() {

    // 初始化Shell
    out << "Start VechicleSell" << endl;
    out << "please use help to get how to use" << endl;

}

int VechilcleShell::DivCommand(QString Command, QString argv[]) {

    int ComCount = 0;       // 命令个数
    int ComInter = 0;       // 命令内的指针

    // 字符串清0
    for (int i = 0; i < 10; i++) {

        argv[i] = "\0";

    }

    // 提取命令行的每个参数 用空格分开的
    for (int i = 0; Command[i] != '\0'; i ++) {

        if (Command[i] == ' ') continue;

        while (Command[i] != ' ' && Command[i] != 0) {

            argv[ComCount][ComInter ++] = Command[i ++];

        }

        ComCount ++;
        ComInter = 0;

    }

    return ComCount;

}

void VechilcleShell::Shell() {

    QString Command;
    QString argv[MaxCommand];
    int  argc;

    while (1) {

        out << "Shell@Vechicle $ " << flush;
        Command = in.readLine();

        if (!Command.isEmpty()) {

            // debug
            // this->out << Command << endl;

            argc = this->DivCommand(Command, argv);

            if (argc > 0) {

                if (argv[0] == "help") {

                    this->Mycom.help(argc, argv);

                }
                if (argv[0] == "light") {

                    this->Mycom.light(argc, argv);

                }

            }

        }

    }

}

// 内建命令的实现

QString CommandList[MaxCommand] = {"help", "light"};

InerCommand::InerCommand() {



}

void InerCommand::help(int argc, QString argv[]) {

    if (argc == 1) {

        out << "command list: " << endl;

        for (int i = 0; i < MaxCommand; i ++) {

            if (CommandList[i] != "\0") {

                out << "    "<< CommandList[i] << endl;

            }

        }


    }

}

void InerCommand::light(int argc, QString *argv) {

    if (argc != 2) {

        out << "woring usage!" << endl;
        return;

    }

    if (argv[1] == "on") {

        MyHardware.Mlight->on();

    } else if (argv[1] == "off") {

        MyHardware.Mlight->off();

    } else if (argv[1] == "status") {

        MyHardware.Mlight->GetStatus();

    } else out << "did'n has usage: " << argv[1] << endl;

}


