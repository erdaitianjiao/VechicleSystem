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
