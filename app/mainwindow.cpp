#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "MyShell.h"
#include "ShellCommands.h"
#include "BottomTabBar.h"
#include "HardwareManager.h"
#include "Devices.h"
#include "CameraPage.h"
#include "MusicPage.h"
#include "MusicPlayer.h"
#include "Page.h"
#include "GalleryPage.h"
#include "ClockPage.h"

#include <QList>
#include <QGuiApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    QList<QScreen *> list_screen = QGuiApplication::screens();
#if __arm__
    this->resize(list_screen.at(0)->geometry().width(),
                 list_screen.at(0)->geometry().height());
#else
    this->resize(800, 480);
#endif

    // Music player
    m_player = new MusicPlayer(this);

    // Command dispatcher & shell
    m_dispatcher = new CommandDispatcher();
    m_dispatcher->registerCommand(new HelpCommand());
    m_dispatcher->registerCommand(new LightCommand());
    m_dispatcher->registerCommand(new BeepCommand());
    m_dispatcher->registerCommand(new CameraCommand());
    m_dispatcher->registerCommand(new MusicCommand(m_player));

    shell = new VechilcleShell(m_dispatcher);
    shell->start();

    setupUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupUi()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // ---- Page Stack ----
    m_pages = new QStackedWidget(this);

    m_homePage = new HomePage();           // 0: Dashboard
    m_vehiclePage = new VehiclePage();      // 1: Vehicle
    m_clockPage = new ClockPage();          // 2: Clock
    m_cameraPage = new CameraPage();        // 3: Camera
    m_musicPage = new MusicPage(m_player);  // 4: Music
    m_galleryPage = new GalleryPage();       // 5: Gallery
    m_mapPage = new MapPage();              // 6: Map

    m_pages->addWidget(m_homePage);       // 0
    m_pages->addWidget(m_vehiclePage);    // 1
    m_pages->addWidget(m_clockPage);      // 2
    m_pages->addWidget(m_cameraPage);     // 3
    m_pages->addWidget(m_musicPage);      // 4
    m_pages->addWidget(m_galleryPage);    // 5
    m_pages->addWidget(m_mapPage);        // 6

    m_mainLayout->addWidget(m_pages, 1);

    // ---- Bottom Tab Bar ----
    m_tabBar = new BottomTabBar(m_pages, this);
    m_tabBar->addTab("首页",   ":/icons/home.svg");
    m_tabBar->addTab("车辆",   ":/icons/car.svg");
    m_tabBar->addTab("时钟",   ":/icons/clock.svg");
    m_tabBar->addTab("相机",   ":/icons/camera.svg");
    m_tabBar->addTab("音乐",   ":/icons/music.svg");
    m_tabBar->addTab("回放",   ":/icons/playback.svg");
    m_tabBar->addTab("地图",   ":/icons/map.svg");

    m_mainLayout->addWidget(m_tabBar);

    // Default to home
    m_pages->setCurrentIndex(0);
}
