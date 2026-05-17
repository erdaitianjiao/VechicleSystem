QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14 link_pkgconfig
PKGCONFIG += gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += app app/pages app/shell hardware service/mediad

SOURCES += \
    app/main.cpp \
    app/mainwindow.cpp \
    app/BottomTabBar.cpp \
    app/pages/Page.cpp \
    app/pages/CameraPage.cpp \
    app/pages/MusicPage.cpp \
    app/pages/GalleryPage.cpp \
    app/pages/ClockPage.cpp \
    app/shell/MyShell.cpp \
    app/shell/ShellCommands.cpp \
    hardware/SysfsDevice.cpp \
    hardware/Devices.cpp \
    hardware/HardwareManager.cpp \
    service/mediad/CameraDevice.cpp \
    service/mediad/MusicPlayer.cpp \
    service/mediad/VideoPlayer.cpp

HEADERS += \
    app/mainwindow.h \
    app/BottomTabBar.h \
    app/pages/Page.h \
    app/pages/CameraPage.h \
    app/pages/MusicPage.h \
    app/pages/GalleryPage.h \
    app/pages/ClockPage.h \
    app/shell/MyShell.h \
    app/shell/ShellCommands.h \
    hardware/IHardwareDevice.h \
    hardware/SysfsDevice.h \
    hardware/Devices.h \
    hardware/HardwareManager.h \
    service/mediad/CameraDevice.h \
    service/mediad/MusicPlayer.h \
    service/mediad/VideoPlayer.h

FORMS += \
    ui/mainwindow.ui

RESOURCES += \
    res/res.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DESTDIR = build
OBJECTS_DIR = build/obj
MOC_DIR = build/moc
RCC_DIR = build/rcc
UI_DIR = build/ui

DISTFILES +=
