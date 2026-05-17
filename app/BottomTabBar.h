#ifndef BOTTOMTABBAR_H
#define BOTTOMTABBAR_H

#include <QWidget>
#include <QPushButton>
#include <QList>
#include <QStackedWidget>

class BottomTabBar : public QWidget {
    Q_OBJECT
public:
    explicit BottomTabBar(QStackedWidget *pages, QWidget *parent = nullptr);

    void addTab(const QString &label, const QString &iconPath);
    void setCurrentTab(int index);

signals:
    void tabSelected(int index);

private:
    QStackedWidget *m_pages;
    QList<QPushButton *> m_tabs;
    int m_currentIndex = 0;

    void updateHighlight();
};

#endif // BOTTOMTABBAR_H
