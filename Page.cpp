#include <QList>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QDebug>
#include <QStyle>
#include <QSpacerItem>
#include <QSizePolicy>
#include "Page.h"
#include "camera.h"

// 主页
HomePage::HomePage() {

    test1 = new QPushButton(this);
    // test1->setMinimumSize(100, 100);

    //实例化按钮
    for (int i = 0; i < AppNum; i++)
    {
        pushButton[i] = new QPushButton(this);
    }
    //pushButton = new QPushButton(this);
    pushButton[0]->setGeometry(50, 50, 70, 70);
    pushButton[0]->setText("串口");

    pushButton[1]->setGeometry(230, 50, 70, 70);
    pushButton[1]->setText("camera");

    pushButton[2]->setGeometry(410, 50, 70, 70);
    pushButton[2]->setText("Ap32");

    connect(pushButton[0], &QPushButton::clicked, this, &HomePage::goSerial);
    connect(pushButton[1], &QPushButton::clicked, this, &HomePage::goCamera);
    connect(pushButton[2], &QPushButton::clicked, this, &HomePage::goAp3216C);

    test1->setText("界面1");

}

/*****************************************************************/
// 地图
MapPage::MapPage() {

    test2 = new QPushButton(this);
    // test2->setMinimumSize(100, 100);
    test2->setText("界面2");

}

/*****************************************************************/
// 串口界面
Serialpage::Serialpage()
{
    //test3 = new QPushButton(this);

    //布局初始化
    Ser_layoutInit();

    //扫描系统的串口
    scanSerialport();

    //波特率项初始化
    baudRateInit();

    //数据位项初始化
    dataBitsItemInit();

    //检验位项初始化
    parityItemInit();

    //停止位项初始化
    stopBitsItemInit();

    //test3->setText("界面3");
}

//串口初始化界面
void Serialpage::Ser_layoutInit()
{
    QList<QScreen *> list_screen = QGuiApplication::screens();

#if __arm__
    //把当前窗口的大小设置为 主屏幕（索引为 0 的屏幕） 的宽高
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());
#else
    this->resize(800, 480);
#endif
    //初始化
    //serialPort = new QSerialPort(this);
    //smr->serialPort = new QSerialPort(this);
    textBrowser = new QTextBrowser();
    textEdit = new QTextEdit();
    vboxLayout = new QVBoxLayout();         // 顶部放一个水平布局，只放按钮
    topLayout = new QHBoxLayout();
    mainLayout = new QVBoxLayout(this);     // 创建主垂直布局
    funcWidget = new QWidget();
    mainWidget = new QWidget();
    gridLayout = new QGridLayout();
    btn = new QPushButton("退出", this);

    // QList 链表，字符串类型
    QList <QString> list1;
    list1<< "串口号：" << "波特率：" << "数据位：" << "检验位：" << "停止位：" ;

    //第一行的标签
    for (int i = 0; i < 5; i++)
    {
        label[i] = new QLabel(list1[i]);
        //设置最小宽度与高度
        label[i]->setMinimumSize(80, 30);
        //自动调整 label 的大小,水平、垂直方向都可以随窗口变大而拉伸
        label[i]->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Expanding);
        //把这个标签放进 第 0 行、第 i 列 的网格单元里
        gridLayout->addWidget(label[i], 0, i);
    }

    //第二行的下拉框
    for (int i = 0; i < 5; i++)
    {
        comboBox[i] = new QComboBox();
        comboBox[i]->setMinimumSize(80, 30);
        comboBox[i]->setSizePolicy(QSizePolicy::Expanding,
                                   QSizePolicy::Expanding);
        gridLayout->addWidget(comboBox[i], 1, i);
    }

    //QList 链表，字符串类型
    QList<QString> list2;
    list2 << "发送" << "打开串口";

    for (int i = 0; i < 2; i++)
    {
        pushButton[i] = new QPushButton(list2[i]);
        pushButton[i]->setMinimumSize(80, 30);
        pushButton[i]->setSizePolicy(QSizePolicy::Expanding,
                                     QSizePolicy::Expanding);
        gridLayout->addWidget(pushButton[i], i, 5);
    }
    //串口没打开时，“发送”按钮禁用
    pushButton[0]->setEnabled(false);

    topLayout->addWidget(btn);// 添加退出按钮
    topLayout->addStretch();  // 把按钮挤到左边

    //把只读和只写两个控件 按上下顺序 塞进 垂直布局管理器
    vboxLayout->addWidget(textBrowser);
    vboxLayout->addWidget(textEdit);

    //把 gridLayout 安装到 funcWidget 上，让 funcWidget 成为 网格布局的容器。
    funcWidget->setLayout(gridLayout);
    /*再把 funcWidget 这个“已经装好的小面板”当成一个整体，放进 垂直布局 vboxLayout。
      它在垂直方向上跟 textBrowser、textEdit 排成一列。*/
    vboxLayout->addWidget(funcWidget);

    // 把两部分拼起来
    mainLayout->addLayout(topLayout);       // 按钮部分
    mainLayout->addLayout(vboxLayout);      // 文本控件部分
    // 让 Serialpage 自己拥有顶层布局
    setLayout(mainLayout);

    //setLayout(vboxLayout);

    // 在最后一行加一个“弹簧”，所有剩余空间都给它
    // vboxLayout->addStretch();

    // 占位文本
    textBrowser->setPlaceholderText("接收到的信息");
    textEdit->setText("我是猫娘，喵～");

    //信号与槽
    connect(pushButton[0], SIGNAL(clicked()), this, SLOT(sendPushButtonClicked()));
    connect(pushButton[1], SIGNAL(clicked()), this, SLOT(openSerialPortPushButtonClicked()));
    connect(MyHardware.smr->serialPort, SIGNAL(readyRead()), this, SLOT(serialPortReadyRead()));
    connect(btn, &QPushButton::clicked, this, &Serialpage::goHome);    //点击退出按钮回到主页面
}

void Serialpage::scanSerialport()
{
    //查找可用串口
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        comboBox[0]->addItem(info.portName());      //把串口名字作为一条选项塞进下拉框comboBox[0]
    }
}

void Serialpage::baudRateInit()
{
    QList <QString> list;
    list << "1200" << "2400" << "4800" << "9600" << "19200"
         << "38400" << "57600" << "115200" << "230400" << "460800"
         << "921600";
    for (int i = 0; i < 11; i++)
    {
        comboBox[1]->addItem(list[i]);
    }
    //让波特率下拉框一打开就默认停在115200喵这一项上
    comboBox[1]->setCurrentIndex(7);
}

void Serialpage::dataBitsItemInit()
{
    QList <QString> list;
    list << "5" << "6" << "7" << "8";
    for (int i = 0; i < 4; i++)
    {
        comboBox[2]->addItem(list[i]);
    }
    comboBox[2]->setCurrentIndex(3);
}

void Serialpage::parityItemInit()
{
    QList <QString> list;
    list << "None" << "Even" << "Odd" << "Space" << "Mark";
    for (int i = 0; i < 5; i++)
    {
        comboBox[3]->addItem(list[i]);
    }
    comboBox[3]->setCurrentIndex(0);
}

void Serialpage::stopBitsItemInit()
{
    QList <QString> list;
    list << "1" << "2";
    for (int i = 0; i < 2; i++)
    {
        comboBox[4]->addItem(list[i]);
    }
    comboBox[4]->setCurrentIndex(0);
}

void Serialpage::sendPushButtonClicked()
{
    // 获取 textEdit 数据,转换成 utf8 格式的字节流
    QByteArray data = textEdit->toPlainText().toUtf8();
    MyHardware.smr->serialPort->write(data);
}

void Serialpage::openSerialPortPushButtonClicked()
{
    if(pushButton[1]->text() == "打开串口")
    {
        //设置串口名
        MyHardware.smr->serialPort->setPortName(comboBox[0]->currentText());
        //设置波特率
        MyHardware.smr->serialPort->setBaudRate(comboBox[1]->currentText().toInt());//toInt把文字转成整数
        //设置数据位数
        switch (comboBox[2]->currentText().toInt())
        {
        case 5:
            MyHardware.smr->serialPort->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            MyHardware.smr->serialPort->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            MyHardware.smr->serialPort->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            MyHardware.smr->serialPort->setDataBits(QSerialPort::Data8);
            break;
        default:
            break;
        }
        //设置奇偶校验
        switch (comboBox[3]->currentIndex())
        {
        case 0:
            MyHardware.smr->serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1:
            MyHardware.smr->serialPort->setParity(QSerialPort::EvenParity);
            break;
        case 2:
            MyHardware.smr->serialPort->setParity(QSerialPort::OddParity);
            break;
        case 3:
            MyHardware.smr->serialPort->setParity(QSerialPort::SpaceParity);
            break;
        case 4:
            MyHardware.smr->serialPort->setParity(QSerialPort::MarkParity);
            break;
        default:
            break;
        }
        //设置停止位
        switch (comboBox[4]->currentText().toInt())
        {
        case 1:
            MyHardware.smr->serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            MyHardware.smr->serialPort->setStopBits(QSerialPort::TwoStop);
            break;
        default:
            break;
        }
        //设置流控制禁用流控
        MyHardware.smr->serialPort->setFlowControl(QSerialPort::NoFlowControl);
        if(!MyHardware.smr->serialPort->open(QIODevice::ReadWrite))
        {
            QMessageBox::about(NULL, "错误", "串口无法打开，串口可能被占用！");
        }
        else
        {
            for (int i = 0; i < 5; i++)
            {
                comboBox[i]->setEnabled(false);
            }
            pushButton[1]->setText("关闭串口");
            pushButton[0]->setEnabled(true);        //使能发送按钮
        }
    }
    else
    {
        MyHardware.smr->serialPort->close();
        for (int i = 0; i < 5; i++)
        {
            comboBox[i]->setEnabled(true);
        }
        pushButton[1]->setText("打开串口");
        pushButton[0]->setEnabled(false);
    }
}

void Serialpage::serialPortReadyRead()
{
    //接收缓冲区中读取数据
    QByteArray buf = MyHardware.smr->serialPort->readAll();
    textBrowser->insertPlainText(QString(buf));
}

/*****************************************************************/
//摄像机
CameraPage::CameraPage(QWidget *parent) : QWidget(parent)
{
    /* 布局初始化 */
    Camer_layoutInit();

    /* 扫描摄像头 */
    scanCameraDevice();
}

void CameraPage::Camer_layoutInit()
{
    /* 实例化与布局 */
    photoLabel = new QLabel();
    rightWidget = new QWidget();
    comboBox = new QComboBox();
    pushButton[0] = new QPushButton();
    pushButton[1] = new QPushButton();
    scrollArea = new QScrollArea();
    displayLabel = new QLabel(scrollArea);
    vboxLayout = new QVBoxLayout();
    hboxLayout = new QHBoxLayout();
    closeButton = new QPushButton();

    // NEW: Create a horizontal layout to manage the top-right corner
    QHBoxLayout *topRightLayout = new QHBoxLayout();
    topRightLayout->addStretch(); // Spacer to push the button to the right
    topRightLayout->addWidget(closeButton);

    // Add widgets to the main vertical layout for the right panel
    vboxLayout->addLayout(topRightLayout); // NEW: Add the top-right layout first
    vboxLayout->addWidget(photoLabel);
    vboxLayout->addWidget(comboBox);
    vboxLayout->addWidget(pushButton[0]);
    vboxLayout->addWidget(pushButton[1]);
    vboxLayout->addStretch(); // 添加伸缩项，使控件靠上

    rightWidget->setLayout(vboxLayout);

    hboxLayout->addWidget(scrollArea);
    hboxLayout->addWidget(rightWidget);
    this->setLayout(hboxLayout); // 为CameraUI设置主布局

    // 获取父窗口的大小来进行一些最小宽度的设置
    QWidget* parentWin = this->parentWidget();
    int parentWidth = parentWin ? parentWin->width() : 800;
    int parentHeight = parentWin ? parentWin->height() : 480;


    pushButton[0]->setMaximumHeight(40);
    pushButton[0]->setMaximumWidth(200);

    pushButton[1]->setMaximumHeight(40);
    pushButton[1]->setMaximumWidth(200);

    comboBox->setMaximumHeight(40);
    comboBox->setMaximumWidth(200);
    photoLabel->setFixedSize(160, 120); // 使用固定大小可能效果更好
    scrollArea->setMinimumWidth(parentWidth - comboBox->width());

    /* 显示图像最大画面为xx */
    displayLabel->setMinimumWidth(scrollArea->width() * 0.75);
    displayLabel->setMinimumHeight(scrollArea->height() * 0.75);
    scrollArea->setWidget(displayLabel);

    /* 居中显示 */
    scrollArea->setAlignment(Qt::AlignCenter);

    /* 自动拉伸 */
    photoLabel->setScaledContents(true);
    displayLabel->setScaledContents(true);

    /* 设置一些属性 */
    pushButton[0]->setText("拍照");
    pushButton[0]->setEnabled(false);
    pushButton[1]->setText("开始");
    pushButton[1]->setCheckable(true);

    // NEW: Set properties for the close button
    //closeButton->setFixedSize(32, 32); // Give it a fixed square size
    //closeButton->setIcon(style()->standardIcon(QStyle::SP_TitleBarCloseButton)); // Use a standard system icon
    //closeButton->setFlat(true); // Remove the button border for a cleaner look
    closeButton->setText("关闭"); // Add a helpful tooltip

    /* 摄像头 */
    camera = new Camera(this);

    /* 信号连接槽 */
    connect(camera, &Camera::readyImage, this, &CameraPage::showImage);
    connect(pushButton[1], &QPushButton::clicked, camera, &Camera::cameraProcess);
    connect(pushButton[1], &QPushButton::clicked, this, &CameraPage::setButtonText);
    connect(pushButton[0], &QPushButton::clicked, this, &CameraPage::saveImageToLocal);

    connect(closeButton, &QPushButton::clicked, this, &CameraPage::goHome);
}

void CameraPage::scanCameraDevice()
{
    /* 如果是Windows系统，一般是摄像头0 */
#if defined(Q_OS_WIN)
    comboBox->addItem("windows摄像头0");
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            camera, &Camera::selectCameraDevice);
#else
    /* QFile文件指向/dev/video0 */
    QFile file("/dev/video0");

    /* 如果文件存在 */
    if (file.exists())
        comboBox->addItem("video0");
    else {
        displayLabel->setText("无摄像头设备");
        return;
    }

    file.setFileName("/dev/video1");

    if (file.exists()) {
        comboBox->addItem("video1");
        /* 开发板ov5640等设备是1 */
        comboBox->setCurrentIndex(1);
    }

    file.setFileName("/dev/video2");

    if (file.exists())
        /* 开发板USB摄像头设备是2 */
        comboBox->addItem("video2");

#if !__arm__
    /* ubuntu的USB摄像头一般是0 */
    comboBox->setCurrentIndex(0);
#endif

    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            camera, &Camera::selectCameraDevice);
#endif
}

void CameraPage::showImage(const QImage &image)
{
    /* 显示图像 */
    displayLabel->setPixmap(QPixmap::fromImage(image));
    saveImage = image;

    /* 判断图像是否为空，空则设置拍照按钮不可用 */
    if (!saveImage.isNull())
        pushButton[0]->setEnabled(true);
    else
        pushButton[0]->setEnabled(false);
}

void CameraPage::setButtonText(bool bl)
{
    if (bl) {
        /* 设置摄像头设备 */
        camera->selectCameraDevice(comboBox->currentIndex());
        pushButton[1]->setText("关闭");
    } else {
        /* 若关闭了摄像头则禁用拍照按钮 */
        pushButton[0]->setEnabled(false);
        pushButton[1]->setText("开始");
    }
}

void CameraPage::saveImageToLocal()
{
    /* 判断图像是否为空 */
    if (!saveImage.isNull()) {
        QString fileName =
                QCoreApplication::applicationDirPath() + "/test.png";
        qDebug()<<"正在保存"<<fileName<<"图片,请稍候..."<<endl;

        /* save(arg1，arg2，arg3)重载函数，arg1代表路径文件名，
         * arg2保存的类型，arg3代表保存的质量等级 */
        saveImage.save(fileName, "PNG", -1);

        /* 设置拍照的图像为显示在photoLabel上 */
        photoLabel->setPixmap(QPixmap::fromImage(QImage(fileName)));

        qDebug()<<"保存完成！"<<endl;
    }
}

/*****************************************************************/

// Ap3216cPage - 修改：实现完整的传感器系统界面，同时保留原有结构
Ap3216cPage::Ap3216cPage()
{
//    btn = new QPushButton(this);
//    // test2->setMinimumSize(100, 100);
//    btn->setText("返回");
//    //connect(btn, &QPushButton::clicked, this, &Ap3216cPage::goHome);

    Ap32_layoutInit();
}

Ap3216cPage::~Ap3216cPage() {
    // 新增：确保停止数据采集
    if (MyHardware.MAp3216c) {
        MyHardware.MAp3216c->setCapture(false);
    }
}

void Ap3216cPage::Ap32_layoutInit()
{
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

    //返回主界面的按钮
    btn = new QPushButton(this);
    btn->setText("返回");
    connect(btn, &QPushButton::clicked, this, &Ap3216cPage::goHome);

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
    mainLayout->addWidget(btn);
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
