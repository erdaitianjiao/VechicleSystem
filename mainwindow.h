#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QCheckBox>
#include <QRadioButton>
#include <QStackedWidget>

#include "MyShell.h"
#include "Page.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void myUiInit();

private:
    Ui::MainWindow *ui;

    // 创建Shell
    VechilcleShell *shell;

    // 创建ui界面
    QStackedWidget *myPageTest;
    HomePage *Home;
    MapPage *Map;

    QRadioButton *radioLight;
    QPushButton *pushButton;


private slots:
    void pushButtonClicked();

};
#endif // MAINWINDOW_H
