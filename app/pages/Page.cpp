#include "Page.h"
#include "HardwareManager.h"
#include "Devices.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QScrollArea>
#include <QDateTime>
#include <QFrame>

// ═══════════════════════════════════════════
//  HomePage — Dashboard
// ═══════════════════════════════════════════

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
{
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *content = new QWidget();
    scroll->setWidget(content);

    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(20, 24, 20, 24);
    layout->setSpacing(20);

    // Clock
    m_timeLabel = new QLabel(this);
    m_timeLabel->setObjectName("timeDisplay");
    m_timeLabel->setAlignment(Qt::AlignCenter);

    m_dateLabel = new QLabel(this);
    m_dateLabel->setObjectName("dateDisplay");
    m_dateLabel->setAlignment(Qt::AlignCenter);

    layout->addWidget(m_timeLabel);
    layout->addWidget(m_dateLabel);
    layout->addSpacing(8);

    // Status cards
    auto *cardGrid = new QGridLayout();
    cardGrid->setSpacing(12);

    // Light card
    auto *lightCard = new QFrame(this);
    lightCard->setObjectName("statusCard");
    auto *lightLay = new QVBoxLayout(lightCard);
    lightLay->setContentsMargins(16, 16, 16, 16);
    auto *lightTitle = new QLabel("前照灯", this);
    lightTitle->setObjectName("cardTitle");
    lightTitle->setAlignment(Qt::AlignCenter);
    m_lightStatusCard = new QLabel("未知", this);
    m_lightStatusCard->setObjectName("cardValue");
    m_lightStatusCard->setAlignment(Qt::AlignCenter);
    lightLay->addWidget(lightTitle);
    lightLay->addWidget(m_lightStatusCard);

    // Beep card
    auto *beepCard = new QFrame(this);
    beepCard->setObjectName("statusCard");
    auto *beepLay = new QVBoxLayout(beepCard);
    beepLay->setContentsMargins(16, 16, 16, 16);
    auto *beepTitle = new QLabel("警报器", this);
    beepTitle->setObjectName("cardTitle");
    beepTitle->setAlignment(Qt::AlignCenter);
    m_beepStatusCard = new QLabel("未知", this);
    m_beepStatusCard->setObjectName("cardValue");
    m_beepStatusCard->setAlignment(Qt::AlignCenter);
    beepLay->addWidget(beepTitle);
    beepLay->addWidget(m_beepStatusCard);

    cardGrid->addWidget(lightCard, 0, 0);
    cardGrid->addWidget(beepCard, 0, 1);
    layout->addLayout(cardGrid);

    // Camera card
    auto *camCard = new QFrame(this);
    camCard->setObjectName("statusCard");
    auto *camLay = new QVBoxLayout(camCard);
    camLay->setContentsMargins(16, 16, 16, 16);
    auto *camTitle = new QLabel("摄像头", this);
    camTitle->setObjectName("cardTitle");
    camTitle->setAlignment(Qt::AlignCenter);
    auto *camLabel = new QLabel("就绪", this);
    camLabel->setObjectName("cardValue");
    camLabel->setAlignment(Qt::AlignCenter);
    camLay->addWidget(camTitle);
    camLay->addWidget(camLabel);
    layout->addWidget(camCard);

    layout->addStretch();

    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);

    // Clock timer
    m_clockTimer = new QTimer(this);
    m_clockTimer->setInterval(1000);
    connect(m_clockTimer, &QTimer::timeout, this, [this]() {
        auto now = QDateTime::currentDateTime();
        m_timeLabel->setText(now.toString("HH:mm"));
        m_dateLabel->setText(now.toString("yyyy年M月d日  ddd"));
    });
    m_clockTimer->start();

    refreshStatusCards();

    auto *refreshTimer = new QTimer(this);
    refreshTimer->setInterval(2000);
    connect(refreshTimer, &QTimer::timeout, this, &HomePage::refreshStatusCards);
    refreshTimer->start();
}

void HomePage::refreshStatusCards()
{
    auto *light = HardwareManager::instance().getDevice<LightDevice>("light");
    if (light) {
        int s = light->getStatus();
        if (s == 1)      m_lightStatusCard->setText("已开启");
        else if (s == 0) m_lightStatusCard->setText("已关闭");
        else             m_lightStatusCard->setText("异常");
    }

    auto *beep = HardwareManager::instance().getDevice<BeepDevice>("beep");
    if (beep) {
        int s = beep->getStatus();
        if (s == 1)      m_beepStatusCard->setText("已开启");
        else if (s == 0) m_beepStatusCard->setText("已关闭");
        else             m_beepStatusCard->setText("异常");
    }
}

// ═══════════════════════════════════════════
//  VehiclePage — Controls
// ═══════════════════════════════════════════

VehiclePage::VehiclePage(QWidget *parent)
    : QWidget(parent)
{
    auto *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    auto *content = new QWidget();
    scroll->setWidget(content);

    auto *layout = new QVBoxLayout(content);
    layout->setContentsMargins(24, 24, 24, 24);
    layout->setSpacing(20);

    auto *title = new QLabel("车辆控制", this);
    title->setObjectName("pageTitle");
    title->setAlignment(Qt::AlignCenter);
    layout->addWidget(title);
    layout->addSpacing(8);

    // Light control
    auto *lightCard = new QFrame(this);
    lightCard->setObjectName("controlCard");
    auto *lightLay = new QVBoxLayout(lightCard);
    lightLay->setContentsMargins(20, 20, 20, 20);
    lightLay->setSpacing(12);

    m_lightStatus = new QLabel("前照灯 · 已关闭", this);
    m_lightStatus->setObjectName("controlTitle");
    lightLay->addWidget(m_lightStatus);

    m_lightToggle = new QPushButton("点击开启", this);
    m_lightToggle->setObjectName("toggleBtn");
    m_lightToggle->setMinimumHeight(56);
    m_lightToggle->setCursor(Qt::PointingHandCursor);
    lightLay->addWidget(m_lightToggle);

    // Beep control
    auto *beepCard = new QFrame(this);
    beepCard->setObjectName("controlCard");
    auto *beepLay = new QVBoxLayout(beepCard);
    beepLay->setContentsMargins(20, 20, 20, 20);
    beepLay->setSpacing(12);

    m_beepStatus = new QLabel("警报器 · 已关闭", this);
    m_beepStatus->setObjectName("controlTitle");
    beepLay->addWidget(m_beepStatus);

    m_beepToggle = new QPushButton("点击开启", this);
    m_beepToggle->setObjectName("toggleBtn");
    m_beepToggle->setMinimumHeight(56);
    m_beepToggle->setCursor(Qt::PointingHandCursor);
    beepLay->addWidget(m_beepToggle);

    layout->addWidget(lightCard);
    layout->addWidget(beepCard);
    layout->addStretch();

    auto *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);
    outerLayout->addWidget(scroll);

    connect(m_lightToggle, &QPushButton::clicked, this, &VehiclePage::onLightToggle);
    connect(m_beepToggle, &QPushButton::clicked, this, &VehiclePage::onBeepToggle);

    updateLightDisplay();
    updateBeepDisplay();
}

void VehiclePage::onLightToggle()
{
    auto *light = HardwareManager::instance().getDevice<LightDevice>("light");
    if (!light) return;
    if (light->getStatus() == 1) light->off();
    else light->on();
    updateLightDisplay();
}

void VehiclePage::onBeepToggle()
{
    auto *beep = HardwareManager::instance().getDevice<BeepDevice>("beep");
    if (!beep) return;
    if (beep->getStatus() == 1) beep->off();
    else beep->on();
    updateBeepDisplay();
}

void VehiclePage::updateLightDisplay()
{
    auto *light = HardwareManager::instance().getDevice<LightDevice>("light");
    if (!light) return;
    int s = light->getStatus();
    if (s == 1) {
        m_lightToggle->setText("点击关闭");
        m_lightStatus->setText("前照灯 · 已开启");
    } else {
        m_lightToggle->setText("点击开启");
        m_lightStatus->setText("前照灯 · 已关闭");
    }
}

void VehiclePage::updateBeepDisplay()
{
    auto *beep = HardwareManager::instance().getDevice<BeepDevice>("beep");
    if (!beep) return;
    int s = beep->getStatus();
    if (s == 1) {
        m_beepToggle->setText("点击关闭");
        m_beepStatus->setText("警报器 · 已开启");
    } else {
        m_beepToggle->setText("点击开启");
        m_beepStatus->setText("警报器 · 已关闭");
    }
}

// ═══════════════════════════════════════════
//  MapPage — Placeholder
// ═══════════════════════════════════════════

MapPage::MapPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(24, 24, 24, 24);

    auto *icon = new QLabel("地图", this);
    icon->setObjectName("placeholderIcon");
    icon->setAlignment(Qt::AlignCenter);
    layout->addStretch();
    layout->addWidget(icon);

    auto *text = new QLabel("功能开发中...", this);
    text->setObjectName("pageTitle");
    text->setAlignment(Qt::AlignCenter);
    layout->addWidget(text);
    layout->addStretch();
}
