#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyShell.h"

#include <QList>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include "Page.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    // 初始化shell
    shell = new VechilcleShell();

    // 开启shell线程
    shell->start();

    // 初始化主界面 可删除
    myUiInit();

    // 初始化一个页面容器
    myPageTest = new QStackedWidget(this);
    Home = new HomePage();
    Map = new MapPage();

    // 将界面添加到页面容器里面
    myPageTest->addWidget(Home);
    myPageTest->addWidget(Map);

    // 切换界面 可将和按钮绑定
    myPageTest->setCurrentIndex(1);

    // ui->setupUi(this);

}

MainWindow::~MainWindow() {

    delete ui;

}

// 初始化界面 处理arm平台和x86平台
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

// 开关灯按钮
void MainWindow::pushButtonClicked() {

    if (MyHardware.Mlight->status == 1) MyHardware.Mlight->off();
    else MyHardware.Mlight->on();

}


