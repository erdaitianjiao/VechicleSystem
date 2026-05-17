#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "MyShell.h"
#include "ShellCommands.h"
#include "BottomTabBar.h"
#include "Page.h"
#include "MusicPlayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CameraPage;
class MusicPage;
class VehiclePage;
class GalleryPage;
class ClockPage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // Shell
    CommandDispatcher *m_dispatcher;
    VechilcleShell *shell;

    // Player
    MusicPlayer *m_player;

    // Layout
    QWidget *m_centralWidget;
    QVBoxLayout *m_mainLayout;
    BottomTabBar *m_tabBar;
    QStackedWidget *m_pages;

    // Pages
    HomePage *m_homePage;
    VehiclePage *m_vehiclePage;
    CameraPage *m_cameraPage;
    MusicPage *m_musicPage;
    GalleryPage *m_galleryPage;
    ClockPage *m_clockPage;
    MapPage *m_mapPage;

    void setupUi();
};

#endif // MAINWINDOW_H
