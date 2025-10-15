#include "Page.h"
#include "Hardware.h"  // 新增：访问硬件全局变量
#include <QDebug>      // 新增：调试输出
#include <QSpacerItem> // 新增：弹性空间
#include <QSizePolicy> // 新增：尺寸策略

// 主页
HomePage::HomePage() {

    test1 = new QPushButton(this);
    // test1->setMinimumSize(100, 100);
    test1->setText("界面1");
}

// 地图
MapPage::MapPage() {

    test2 = new QPushButton(this);
    // test2->setMinimumSize(100, 100);
    test2->setText("界面2");

}

// Ap3216cPage - 修改：实现完整的传感器系统界面，同时保留原有结构
Ap3216cPage::Ap3216cPage() {
    test3 = new QPushButton(this);
    // test2->setMinimumSize(100, 100);
    test3->setText("界面3");

    // 首先设置主widget的布局
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);  // 新增：移除边距
    outerLayout->setSpacing(0);                    // 新增：移除间距

    // 创建内部页面管理器
    internalStackedWidget = new QStackedWidget(this);

    // 新增：设置大小策略确保扩展
    internalStackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 创建主页面
    ap3216cMainPage = new QWidget();
    mainLayout = new QVBoxLayout(ap3216cMainPage);
    mainLayout->setSpacing(30);
    mainLayout->setContentsMargins(50, 80, 50, 50);

    // 主页面标题
    titleLabel = new QLabel("AP3216C 传感器系统", ap3216cMainPage);
    QFont titleFont("Arial", 28, QFont::Bold);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    // 主页面说明文字
    descLabel = new QLabel(
        "集成环境光传感器(ALS)、接近传感器(PS)和红外传感器(IR)\n\n"
        "点击下方按钮查看实时传感器数据",
        ap3216cMainPage
    );
    QFont descFont("Arial", 16);
    descLabel->setFont(descFont);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);

    // 查看传感器数据按钮
    sensorBtn = new QPushButton("查看传感器数据", ap3216cMainPage);
    sensorBtn->setMinimumHeight(80);
    QFont btnFont("Arial", 18);
    sensorBtn->setFont(btnFont);

    // 弹性空间
    QSpacerItem *spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    // 主页面布局
    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(descLabel);
    mainLayout->addItem(spacer);
    mainLayout->addWidget(sensorBtn);

    // 创建传感器数据页面
    ap3216cSensorPage = new QWidget();
    sensorLayout = new QVBoxLayout(ap3216cSensorPage);
    sensorLayout->setSpacing(10);
    sensorLayout->setContentsMargins(20, 50, 20, 20);

    // 返回按钮
    backBtn = new QPushButton("返回", ap3216cSensorPage);
    backBtn->setMinimumHeight(50);
    QFont backFont("Arial", 16);
    backBtn->setFont(backFont);

    // 传感器标题
    sensorTitle = new QLabel("传感器实时数据", ap3216cSensorPage);
    QFont sensorTitleFont("Arial", 24, QFont::Bold);
    sensorTitle->setFont(sensorTitleFont);
    sensorTitle->setAlignment(Qt::AlignCenter);

    // 传感器数据显示区域
    QGridLayout *sensorGridLayout = new QGridLayout();
    sensorGridLayout->setSpacing(15);

    // 传感器标签和数值显示
    alsLabel = new QLabel("环境光(ALS):", ap3216cSensorPage);
    psLabel = new QLabel("接近传感器(PS):", ap3216cSensorPage);
    irLabel = new QLabel("红外(IR):", ap3216cSensorPage);

    alsValue = new QLabel("--", ap3216cSensorPage);
    psValue = new QLabel("--", ap3216cSensorPage);
    irValue = new QLabel("--", ap3216cSensorPage);

    // 设置字体
    QFont labelFont("Arial", 16);
    QFont valueFont("Arial", 18, QFont::Bold);

    alsLabel->setFont(labelFont);
    psLabel->setFont(labelFont);
    irLabel->setFont(labelFont);

    alsValue->setFont(valueFont);
    psValue->setFont(valueFont);
    irValue->setFont(valueFont);

    // 添加到网格布局
    sensorGridLayout->addWidget(alsLabel, 0, 0);
    sensorGridLayout->addWidget(alsValue, 0, 1);
    sensorGridLayout->addWidget(psLabel, 1, 0);
    sensorGridLayout->addWidget(psValue, 1, 1);
    sensorGridLayout->addWidget(irLabel, 2, 0);
    sensorGridLayout->addWidget(irValue, 2, 1);

    // 传感器页面布局
    sensorLayout->addWidget(backBtn);
    sensorLayout->addWidget(sensorTitle);
    sensorLayout->addLayout(sensorGridLayout);
    sensorLayout->addStretch(1);

    // 将两个页面添加到内部堆叠窗口
    internalStackedWidget->addWidget(ap3216cMainPage);
    internalStackedWidget->addWidget(ap3216cSensorPage);

    // 设置主布局
    outerLayout->addWidget(internalStackedWidget);

    // 连接信号槽
    connect(sensorBtn, SIGNAL(clicked()), this, SLOT(showSensorPage()));
    connect(backBtn, SIGNAL(clicked()), this, SLOT(showMainPage()));

    // 连接传感器数据更新信号
    if (MyHardware.MAp3216c) {
        connect(MyHardware.MAp3216c, SIGNAL(ap3216cDataChanged()),
                this, SLOT(updateSensorData()));
    }

    // 默认显示主页面
    internalStackedWidget->setCurrentIndex(0);

    // 调试：验证布局
    qDebug() << "Ap3216cPage初始化完成";
    qDebug() << "internalStackedWidget大小策略:" << internalStackedWidget->sizePolicy();

    // 新增：强制更新布局
    this->updateGeometry();
    internalStackedWidget->updateGeometry();
}

Ap3216cPage::~Ap3216cPage() {
    // 新增：确保停止数据采集
    if (MyHardware.MAp3216c) {
        MyHardware.MAp3216c->setCapture(false);
    }
}

// 新增：显示传感器数据页面
void Ap3216cPage::showSensorPage() {
    internalStackedWidget->setCurrentIndex(1);

    // 启动数据采集
    if (MyHardware.MAp3216c) {
        MyHardware.MAp3216c->setCapture(true);
    }

    qDebug() << "切换到传感器数据页面";
}

// 新增：显示主页面
void Ap3216cPage::showMainPage() {
    internalStackedWidget->setCurrentIndex(0);

    // 停止数据采集
    if (MyHardware.MAp3216c) {
        MyHardware.MAp3216c->setCapture(false);
    }

    qDebug() << "返回传感器主页面";
}

// 新增：更新传感器数据显示
void Ap3216cPage::updateSensorData() {
    if (MyHardware.MAp3216c) {
        alsValue->setText(MyHardware.MAp3216c->alsData());
        psValue->setText(MyHardware.MAp3216c->psData());
        irValue->setText(MyHardware.MAp3216c->irData());
    }
}
