#include "ClockPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QDateTime>
#include <QScrollArea>
#include <QApplication>

// ═══════════════════════════════════════════
//  ClockPage — World Clock / Timer / Stopwatch
// ═══════════════════════════════════════════

ClockPage::ClockPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Sub-tab bar
    auto *tabBar = new QHBoxLayout();
    tabBar->setContentsMargins(8, 8, 8, 0);
    tabBar->setSpacing(0);

    auto makeTab = [this, &tabBar](const QString &text, int index) {
        auto *btn = new QPushButton(text, this);
        btn->setObjectName("clockTabBtn");
        btn->setCheckable(true);
        btn->setMinimumHeight(40);
        btn->setCursor(Qt::PointingHandCursor);
        connect(btn, &QPushButton::clicked, this, [this, index]() {
            m_subTabs->setCurrentIndex(index);
        });
        tabBar->addWidget(btn);
        if (index == 0) btn->setChecked(true);
        return btn;
    };

    auto *tab1 = makeTab("世界时钟", 0);
    auto *tab2 = makeTab("定时器", 1);
    auto *tab3 = makeTab("秒表", 2);

    // Link sub-tabs to highlight
    auto updateTabs = [tab1, tab2, tab3](int idx) {
        tab1->setChecked(idx == 0);
        tab2->setChecked(idx == 1);
        tab3->setChecked(idx == 2);
    };

    mainLayout->addLayout(tabBar);

    m_subTabs = new QStackedWidget(this);
    connect(m_subTabs, &QStackedWidget::currentChanged, this, updateTabs);
    mainLayout->addWidget(m_subTabs, 1);

    // ═══════════════════════════════════════
    //  Page 0: World Clock
    // ═══════════════════════════════════════
    m_worldPage = new QWidget(this);
    auto *wLayout = new QVBoxLayout(m_worldPage);
    wLayout->setContentsMargins(12, 8, 12, 8);

    // Add city row
    auto *addRow = new QHBoxLayout();
    m_tzCombo = new QComboBox(this);
    m_tzCombo->setObjectName("tzCombo");
    m_tzCombo->setMinimumHeight(42);

    // Populate timezones
    for (const QByteArray &id : QTimeZone::availableTimeZoneIds()) {
        QTimeZone tz(id);
        int offset = tz.offsetFromUtc(QDateTime::currentDateTime()) / 3600;
        m_tzCombo->addItem(
            QString("%1 (UTC%2%3)").arg(QString::fromUtf8(id))
                .arg(offset >= 0 ? "+" : "")
                .arg(offset),
            QString::fromUtf8(id));
    }

    auto *addBtn = new QPushButton("+", this);
    addBtn->setObjectName("addCityBtn");
    addBtn->setMinimumSize(44, 44);
    addBtn->setCursor(Qt::PointingHandCursor);
    connect(addBtn, &QPushButton::clicked, this, &ClockPage::addCity);

    auto *delBtn = new QPushButton("−", this);
    delBtn->setObjectName("delCityBtn");
    delBtn->setMinimumSize(44, 44);
    delBtn->setCursor(Qt::PointingHandCursor);
    connect(delBtn, &QPushButton::clicked, this, &ClockPage::removeCity);

    addRow->addWidget(m_tzCombo, 1);
    addRow->addWidget(addBtn);
    addRow->addWidget(delBtn);
    wLayout->addLayout(addRow);

    m_cityList = new QListWidget(this);
    m_cityList->setObjectName("cityList");
    m_cityList->setSpacing(2);
    wLayout->addWidget(m_cityList, 1);

    // Default cities
    m_tzCombo->setCurrentText("Asia/Shanghai (UTC+8)");
    addCity();
    m_tzCombo->setCurrentText("America/New_York (UTC-5)");
    addCity();
    m_tzCombo->setCurrentText("Europe/London (UTC+0)");
    addCity();

    // Refresh every second
    m_worldTimer = new QTimer(this);
    m_worldTimer->setInterval(1000);
    connect(m_worldTimer, &QTimer::timeout, this, &ClockPage::refreshWorldClock);
    m_worldTimer->start();

    // ═══════════════════════════════════════
    //  Page 1: Timer (Countdown)
    // ═══════════════════════════════════════
    m_timerPage = new QWidget(this);
    auto *tLayout = new QVBoxLayout(m_timerPage);
    tLayout->setContentsMargins(24, 16, 24, 16);
    tLayout->setSpacing(16);

    // Display
    m_timerDisplay = new QLabel("00:00:00", this);
    m_timerDisplay->setObjectName("timerDisplay");
    m_timerDisplay->setAlignment(Qt::AlignCenter);
    tLayout->addWidget(m_timerDisplay);

    // Time set spinners
    auto *spinRow = new QHBoxLayout();
    spinRow->setSpacing(8);

    auto makeSpin = [this](const QString &label, int max) {
        auto *w = new QWidget(this);
        auto *l = new QVBoxLayout(w);
        l->setContentsMargins(4, 0, 4, 0);
        auto *lb = new QLabel(label, w);
        lb->setAlignment(Qt::AlignCenter);
        lb->setObjectName("spinLabel");
        auto *sp = new QSpinBox(w);
        sp->setObjectName("timeSpin");
        sp->setRange(0, max);
        sp->setMinimumHeight(48);
        sp->setAlignment(Qt::AlignCenter);
        sp->setCursor(Qt::PointingHandCursor);
        l->addWidget(lb);
        l->addWidget(sp);
        return sp;
    };

    m_timerH = makeSpin("时", 99);
    m_timerM = makeSpin("分", 59);
    m_timerS = makeSpin("秒", 59);

    spinRow->addStretch();
    spinRow->addWidget(m_timerH);
    spinRow->addWidget(m_timerM);
    spinRow->addWidget(m_timerS);
    spinRow->addStretch();
    tLayout->addLayout(spinRow);

    // Buttons
    auto *tBtnRow = new QHBoxLayout();
    tBtnRow->setSpacing(12);
    m_timerStartBtn = new QPushButton("开始", this);
    m_timerStartBtn->setObjectName("actionBtn");
    m_timerStartBtn->setMinimumHeight(52);
    m_timerStartBtn->setCursor(Qt::PointingHandCursor);
    m_timerResetBtn = new QPushButton("重置", this);
    m_timerResetBtn->setObjectName("statusBtn");
    m_timerResetBtn->setMinimumHeight(52);
    m_timerResetBtn->setCursor(Qt::PointingHandCursor);

    connect(m_timerStartBtn, &QPushButton::clicked, this, &ClockPage::timerStartPause);
    connect(m_timerResetBtn, &QPushButton::clicked, this, &ClockPage::timerReset);

    tBtnRow->addStretch();
    tBtnRow->addWidget(m_timerStartBtn);
    tBtnRow->addWidget(m_timerResetBtn);
    tBtnRow->addStretch();
    tLayout->addLayout(tBtnRow);
    tLayout->addStretch();

    m_countdownTimer = new QTimer(this);
    m_countdownTimer->setInterval(1000);
    connect(m_countdownTimer, &QTimer::timeout, this, &ClockPage::timerTick);

    // ═══════════════════════════════════════
    //  Page 2: Stopwatch
    // ═══════════════════════════════════════
    m_stopwatchPage = new QWidget(this);
    auto *sLayout = new QVBoxLayout(m_stopwatchPage);
    sLayout->setContentsMargins(24, 16, 24, 16);
    sLayout->setSpacing(16);

    m_stopwatchDisplay = new QLabel("00:00.000", this);
    m_stopwatchDisplay->setObjectName("timerDisplay");
    m_stopwatchDisplay->setAlignment(Qt::AlignCenter);
    sLayout->addWidget(m_stopwatchDisplay);

    auto *sBtnRow = new QHBoxLayout();
    sBtnRow->setSpacing(12);
    m_swStartBtn = new QPushButton("开始", this);
    m_swStartBtn->setObjectName("actionBtn");
    m_swStartBtn->setMinimumHeight(52);
    m_swStartBtn->setCursor(Qt::PointingHandCursor);
    m_swResetBtn = new QPushButton("重置", this);
    m_swResetBtn->setObjectName("statusBtn");
    m_swResetBtn->setMinimumHeight(52);
    m_swResetBtn->setCursor(Qt::PointingHandCursor);
    m_swLapBtn = new QPushButton("计次", this);
    m_swLapBtn->setObjectName("statusBtn");
    m_swLapBtn->setMinimumHeight(52);
    m_swLapBtn->setCursor(Qt::PointingHandCursor);

    connect(m_swStartBtn, &QPushButton::clicked, this, &ClockPage::stopwatchStartStop);
    connect(m_swResetBtn, &QPushButton::clicked, this, &ClockPage::stopwatchReset);
    connect(m_swLapBtn, &QPushButton::clicked, this, &ClockPage::stopwatchLap);

    sBtnRow->addStretch();
    sBtnRow->addWidget(m_swStartBtn);
    sBtnRow->addWidget(m_swResetBtn);
    sBtnRow->addWidget(m_swLapBtn);
    sBtnRow->addStretch();
    sLayout->addLayout(sBtnRow);

    m_lapList = new QListWidget(this);
    m_lapList->setObjectName("lapList");
    m_lapList->setMaximumHeight(150);
    sLayout->addWidget(m_lapList);

    sLayout->addStretch();

    m_stopwatchTimer = new QTimer(this);
    m_stopwatchTimer->setInterval(50);
    connect(m_stopwatchTimer, &QTimer::timeout, this, &ClockPage::stopwatchTick);

    // Add pages
    m_subTabs->addWidget(m_worldPage);     // 0
    m_subTabs->addWidget(m_timerPage);     // 1
    m_subTabs->addWidget(m_stopwatchPage); // 2
}

// ---- World Clock ----

void ClockPage::addCity()
{
    QString name = m_tzCombo->currentData().toString();
    if (name.isEmpty()) return;

    // Check duplicate
    for (int i = 0; i < m_cityList->count(); i++) {
        if (m_cityList->item(i)->data(Qt::UserRole).toString() == name)
            return;
    }

    auto *item = new QListWidgetItem(name);
    item->setData(Qt::UserRole, name);
    m_cityList->addItem(item);
    refreshWorldClock();
}

void ClockPage::removeCity()
{
    auto *item = m_cityList->currentItem();
    if (item) {
        delete m_cityList->takeItem(m_cityList->row(item));
    }
}

void ClockPage::refreshWorldClock()
{
    for (int i = 0; i < m_cityList->count(); i++) {
        auto *item = m_cityList->item(i);
        QString tzId = item->data(Qt::UserRole).toString();
        QTimeZone tz(tzId.toUtf8());
        QDateTime now = QDateTime::currentDateTime().toTimeZone(tz);

        QString cityName = tzId;
        if (tzId.contains('/')) {
            cityName = tzId.section('/', -1).replace('_', ' ');
        }

        int offset = tz.offsetFromUtc(QDateTime::currentDateTime()) / 3600;
        QString offStr = QString("UTC%1%2").arg(offset >= 0 ? "+" : "").arg(offset);

        item->setText(QString("%1  %2  %3")
            .arg(now.toString("HH:mm:ss"))
            .arg(cityName, -20)
            .arg(offStr));

        item->setForeground(
            now.time().hour() >= 6 && now.time().hour() < 18
                ? QColor("#1d1d1f")
                : QColor("#007aff"));
    }
}

// ---- Timer ----

void ClockPage::timerStartPause()
{
    if (m_countdownTimer->isActive()) {
        // Pause
        m_countdownTimer->stop();
        m_timerStartBtn->setText("继续");
        return;
    }

    if (m_remainingSec == 0) {
        // Start fresh
        m_remainingSec = m_timerH->value() * 3600
                       + m_timerM->value() * 60
                       + m_timerS->value();
        m_timerH->setEnabled(false);
        m_timerM->setEnabled(false);
        m_timerS->setEnabled(false);
    }

    if (m_remainingSec <= 0) return;

    m_countdownTimer->start();
    m_timerStartBtn->setText("暂停");
}

void ClockPage::timerReset()
{
    m_countdownTimer->stop();
    m_remainingSec = 0;
    m_timerH->setEnabled(true);
    m_timerM->setEnabled(true);
    m_timerS->setEnabled(true);
    m_timerStartBtn->setText("开始");
    updateTimerDisplay();
}

void ClockPage::timerTick()
{
    if (m_remainingSec > 0) {
        m_remainingSec--;
        updateTimerDisplay();
    }
    if (m_remainingSec <= 0) {
        m_countdownTimer->stop();
        m_timerStartBtn->setText("开始");
        m_timerH->setEnabled(true);
        m_timerM->setEnabled(true);
        m_timerS->setEnabled(true);
        m_timerDisplay->setStyleSheet("color: #ff3b30;");
        QTimer::singleShot(2000, this, [this]() {
            m_timerDisplay->setStyleSheet("");
        });
    }
}

void ClockPage::updateTimerDisplay()
{
    int h = m_remainingSec / 3600;
    int m = (m_remainingSec % 3600) / 60;
    int s = m_remainingSec % 60;
    m_timerDisplay->setText(QString("%1:%2:%3")
        .arg(h, 2, 10, QChar('0'))
        .arg(m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0')));
}

// ---- Stopwatch ----

void ClockPage::stopwatchStartStop()
{
    if (m_swRunning) {
        m_stopwatchTimer->stop();
        m_swRunning = false;
        m_swStartBtn->setText("继续");
    } else {
        m_stopwatchTimer->start();
        m_swRunning = true;
        m_swStartBtn->setText("暂停");
    }
}

void ClockPage::stopwatchReset()
{
    m_stopwatchTimer->stop();
    m_swRunning = false;
    m_elapsedMs = 0;
    m_swStartBtn->setText("开始");
    m_lapList->clear();
    updateStopwatchDisplay();
}

void ClockPage::stopwatchLap()
{
    if (!m_swRunning) return;
    auto *item = new QListWidgetItem(
        QString("计次 %1  %2")
            .arg(m_lapList->count() + 1, 2)
            .arg(m_stopwatchDisplay->text()));
    item->setForeground(QColor("#007aff"));
    m_lapList->insertItem(0, item);
}

void ClockPage::stopwatchTick()
{
    m_elapsedMs += 50;
    updateStopwatchDisplay();
}

void ClockPage::updateStopwatchDisplay()
{
    int ms = m_elapsedMs;
    int h = ms / 3600000; ms %= 3600000;
    int m = ms / 60000;   ms %= 60000;
    int s = ms / 1000;    ms %= 1000;
    m_stopwatchDisplay->setText(QString("%1:%2.%3")
        .arg(h * 60 + m, 2, 10, QChar('0'))
        .arg(s, 2, 10, QChar('0'))
        .arg(ms / 10, 2, 10, QChar('0')));
}
