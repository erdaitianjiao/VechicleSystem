#include "BottomTabBar.h"
#include <QHBoxLayout>
#include <QIcon>
#include <QSize>

BottomTabBar::BottomTabBar(QStackedWidget *pages, QWidget *parent)
    : QWidget(parent)
    , m_pages(pages)
{
    setObjectName("bottomTabBar");
    setFixedHeight(60);

    auto *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 2, 0, 4);
    layout->setSpacing(0);
}

void BottomTabBar::addTab(const QString &label, const QString &iconPath)
{
    auto *btn = new QPushButton(this);
    btn->setObjectName("tabBtn");
    btn->setText(label);
    btn->setIcon(QIcon(iconPath));
    btn->setIconSize(QSize(24, 24));
    btn->setCursor(Qt::PointingHandCursor);
    btn->setCheckable(true);
    btn->setMinimumHeight(54);

    int index = m_tabs.size();

    connect(btn, &QPushButton::clicked, this, [this, index]() {
        setCurrentTab(index);
    });

    m_tabs.append(btn);

    auto *lay = qobject_cast<QHBoxLayout *>(layout());
    if (lay) lay->addWidget(btn);

    if (m_tabs.size() == 1)
        setCurrentTab(0);
}

void BottomTabBar::setCurrentTab(int index)
{
    if (index < 0 || index >= m_tabs.size())
        return;

    m_currentIndex = index;
    updateHighlight();

    if (m_pages && index < m_pages->count())
        m_pages->setCurrentIndex(index);

    emit tabSelected(index);
}

void BottomTabBar::updateHighlight()
{
    for (int i = 0; i < m_tabs.size(); i++) {
        m_tabs[i]->setChecked(i == m_currentIndex);
    }
}
