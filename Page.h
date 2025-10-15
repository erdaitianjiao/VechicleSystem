#ifndef PAGE_H
#define PAGE_H

#include <QWidget>
#include <QPushButton>
#include <QStackedWidget>  // 新增：用于内部页面管理
#include <QLabel>          // 新增：用于显示文本
#include <QVBoxLayout>     // 新增：布局管理
#include <QGridLayout>     // 新增：网格布局
#include <QFont>           // 新增：字体设置

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

// Ap3216c页面 - 修改：整合test_for_Ap3216c的两个页面，同时保留原有结构
class Ap3216cPage : public QWidget {
    Q_OBJECT  // 新增：需要信号槽支持

public:
    Ap3216cPage();
    ~Ap3216cPage();

private:
    // 保留：原有的test3按钮
    QPushButton *test3;             // 原有测试按钮保留

    // 新增：内部页面管理
    QStackedWidget *internalStackedWidget;
    QWidget *ap3216cMainPage;      // 传感器系统主页面
    QWidget *ap3216cSensorPage;    // 传感器数据页面

    // 新增：主页面组件
    QLabel *titleLabel;
    QLabel *descLabel;
    QPushButton *sensorBtn;

    // 新增：传感器页面组件
    QPushButton *backBtn;
    QLabel *sensorTitle;
    QGridLayout *sensorGridLayout;
    QLabel *alsLabel, *psLabel, *irLabel;
    QLabel *alsValue, *psValue, *irValue;

    // 新增：布局
    QVBoxLayout *mainLayout;
    QVBoxLayout *sensorLayout;

private slots:
    // 新增：内部页面切换槽函数
    void showSensorPage();
    void showMainPage();
    void updateSensorData();  // 更新传感器数据显示

signals:
    // 新增：用于通知主窗口可能需要的外部操作
    void sensorDataRequested(bool start);  // 请求开始/停止数据采集
};

#endif // PAGE_H
