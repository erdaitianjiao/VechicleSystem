#include "mainwindow.h"
#include "Hardware.h"
#include "MyShell.h"

#include <QApplication>
#include <QTextStream>
#include <QCoreApplication>
#include <QFile>

int main(int argc, char *argv[]) {

    MyHardware.init();

    QApplication a(argc, argv);

    QFile file(":/style.qss");

    if (file.exists()) {

        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());

        qApp->setStyleSheet(styleSheet);

        file.close();

    }

    MainWindow w;
    w.show();
    return a.exec();

}
