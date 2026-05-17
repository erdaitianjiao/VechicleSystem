#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QTimer>

// Dashboard home page
class HomePage : public QWidget {
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = nullptr);

private:
    QLabel *m_timeLabel;
    QLabel *m_dateLabel;
    QLabel *m_lightStatusCard;
    QLabel *m_beepStatusCard;
    QTimer *m_clockTimer;

    void refreshStatusCards();
};

// Vehicle controls page
class VehiclePage : public QWidget {
    Q_OBJECT
public:
    explicit VehiclePage(QWidget *parent = nullptr);

private slots:
    void onLightToggle();
    void onBeepToggle();

private:
    QPushButton *m_lightToggle;
    QPushButton *m_beepToggle;
    QLabel *m_lightStatus;
    QLabel *m_beepStatus;

    void updateLightDisplay();
    void updateBeepDisplay();
};

// Map placeholder page
class MapPage : public QWidget {
    Q_OBJECT
public:
    explicit MapPage(QWidget *parent = nullptr);
};

#endif // PAGE_H
