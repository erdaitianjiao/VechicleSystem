#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QPushButton>

/*
 *  @ page文件
 *  所有的界面都在这里定义
 *
 */


// 主页 展示基本信息等
class HomePage : public QWidget{

public:
    HomePage();

private:

    QPushButton *test1;     // 测试按钮 可删除

};

// 地图文件 原本想调用百度地图api 有点太难了 先搁置
class MapPage : public QWidget {

public:
    MapPage();

private:
    QPushButton *test2;             // 测试按钮 可删除

};

#endif // PAGE_H
