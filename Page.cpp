#include <QList>
#include <QGuiApplication>
#include <QScreen>
#include <QRect>
#include <QDebug>
#include "Page.h"

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

// 串口界面
Serialpage::Serialpage()
{
    //test3 = new QPushButton(this);

    //布局初始化
    layoutInit();

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
void Serialpage::layoutInit()
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

