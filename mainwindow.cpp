#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyShell.h"

#include <QList>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    // 初始化shell
    shell = new VechilcleShell();
    shell->start();

    myUiInit();

    // ui->setupUi(this);

}

MainWindow::~MainWindow() {

    delete ui;
}


void MainWindow::myUiInit() {

    QList <QScreen *> list_screen = QGuiApplication::screens();

#if __arm__
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());

#else
    this->resize(800, 480);

#endif

    // 居中显示
    radioLight = new QRadioButton(this);
    pushButton = new QPushButton(this);

    pushButton->setText("开关灯");
    radioLight->setText("灯");
    radioLight->setGeometry(0, 0, radioLight->width(), radioLight->height());

    pushButton->setMinimumSize(200, 50);
    pushButton->setGeometry((this->width()  - pushButton->width())  / 2,
                            (this->height() - pushButton->height()) / 2,
                            pushButton->width(),
                            pushButton->height());


    connect(pushButton, SIGNAL(clicked()), this, SLOT(pushButtonClicked()));

}

void MainWindow::pushButtonClicked() {

    if (MyHardware.Mlight->status == 1) MyHardware.Mlight->off();
    else MyHardware.Mlight->on();

}


