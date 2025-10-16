#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyShell.h"
#include "camera.h"

#include <QList>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QDebug>
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
    ser = new Serialpage();
    crm = new CameraPage();      // 实例化UI类
    Ap32 = new Ap3216cPage();

    //setCentralWidget(ser);

    // 将界面添加到页面容器里面
    myPageTest->addWidget(Home);
    myPageTest->addWidget(Map);
    myPageTest->addWidget(ser);
    myPageTest->addWidget(crm);
    myPageTest->addWidget(Ap32);

    // 切换界面 可将和按钮绑定
    myPageTest->setCurrentIndex(0);
    setCentralWidget(myPageTest);   // 告诉 Qt 用 QStackedWidget 做中央窗体

    /* 将UI设置为中心部件 */
    //this->setCentralWidget(cameraUi);

    connect(Home, &HomePage::goSerial, this, [this]{setPage(2); });  //从主界面到串口界面
    connect(ser, &Serialpage::goHome, this, [this]{setPage(0); });  //从串口界面到主界面

    connect(Home, &HomePage::goCamera, this, [this]{setPage(3); }); //从主界面到camera界面
    connect(crm, &CameraPage::goHome, this, [this]{setPage(0); }); //从camera界面到主界面

    connect(Home, &HomePage::goAp3216C, this, [this]{setPage(4); }); //从主界面到Ap32C16的界面
    connect(Ap32, &Ap3216cPage::goHome, this, [this]{setPage(0); }); //从Ap32C16到主界面

    // ui->setupUi(this);

}

MainWindow::~MainWindow() {

    delete ui;

}

//test
//void MainWindow::MAIN_home()
//{

//    myPageTest->setCurrentIndex(0);

//}

// 初始化界面 处理arm平台和x86平台
void MainWindow::myUiInit() {

    QList <QScreen *> list_screen = QGuiApplication::screens();

#if __arm__
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());

#else
    this->resize(800, 480);

#endif

//    // 居中显示
//    radioLight = new QRadioButton(this);
//    pushButton = new QPushButton(this);

//    pushButton->setText("开关灯");
//    radioLight->setText("灯");
//    radioLight->setGeometry(0, 0, radioLight->width(), radioLight->height());

//    pushButton->setMinimumSize(200, 50);
//    pushButton->setGeometry((this->width()  - pushButton->width())  / 2,
//                            (this->height() - pushButton->height()) / 2,
//                            pushButton->width(),
//                            pushButton->height());


//    connect(pushButton, SIGNAL(clicked()), this, SLOT(pushButtonClicked()));

}

// 开关灯按钮
void MainWindow::pushButtonClicked() {

//    if (MyHardware.Mlight->status == 1) MyHardware.Mlight->off();
//    else MyHardware.Mlight->on();

}

//页面切换
void MainWindow::setPage(int idx)
{
    myPageTest->setCurrentIndex(idx);
}

////串口界面返回主界面
//void Serialpage::SerialpagetoMianpage()
//{
//    emit goHome();
//}
