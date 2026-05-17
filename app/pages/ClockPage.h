#ifndef CLOCKPAGE_H
#define CLOCKPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>
#include <QTimer>
#include <QTimeZone>
#include <QComboBox>
#include <QStackedWidget>
#include <QTime>

class ClockPage : public QWidget {
    Q_OBJECT
public:
    explicit ClockPage(QWidget *parent = nullptr);

private:
    QStackedWidget *m_subTabs;

    // ---- World Clock ----
    QWidget *m_worldPage;
    QListWidget *m_cityList;
    QComboBox *m_tzCombo;
    QTimer *m_worldTimer;

    void addCity();
    void removeCity();
    void refreshWorldClock();

    // ---- Timer ----
    QWidget *m_timerPage;
    QSpinBox *m_timerH, *m_timerM, *m_timerS;
    QLabel *m_timerDisplay;
    QPushButton *m_timerStartBtn;
    QPushButton *m_timerResetBtn;
    QTimer *m_countdownTimer;
    int m_remainingSec = 0;

    void timerStartPause();
    void timerReset();
    void timerTick();
    void updateTimerDisplay();

    // ---- Stopwatch ----
    QWidget *m_stopwatchPage;
    QLabel *m_stopwatchDisplay;
    QPushButton *m_swStartBtn;
    QPushButton *m_swResetBtn;
    QPushButton *m_swLapBtn;
    QListWidget *m_lapList;
    QTimer *m_stopwatchTimer;
    int m_elapsedMs = 0;
    bool m_swRunning = false;

    void stopwatchStartStop();
    void stopwatchReset();
    void stopwatchLap();
    void stopwatchTick();
    void updateStopwatchDisplay();
};

#endif // CLOCKPAGE_H
