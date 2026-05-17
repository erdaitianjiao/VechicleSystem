# VechicleSystem

车载嵌入式 Linux 综合信息娱乐系统，基于 Qt5 + GStreamer 开发。

## 功能

| 模块 | 功能 |
|------|------|
| 车辆控制 | 前照灯开关、警报器开关（通过 sysfs 驱动） |
| 摄像头 | 实时预览、拍照、音视频同步录像（GStreamer V4L2 + tee + 双 valve 分流） |
| 音乐播放 | MP3/WAV/FLAC/OGG 播放，进度拖动，音量调节，顺序/单曲循环/随机播放 |
| 视频回放 | 播放已录制的音视频文件，支持暂停/恢复 |
| 照片回放 | 浏览已拍摄的照片 |
| 世界时钟 | 多时区时钟、倒计时定时器、毫秒级秒表 |
| Shell 调试 | 内置命令行终端，支持上下键历史（内存 100 条）、方向键编辑 |

## 架构

```
app/           UI 应用层（页面、Shell、主窗口）
service/       服务层（相机服务、音乐播放器、视频播放器）
hardware/      硬件抽象层（设备接口、sysfs 驱动、硬件管理器）
```

```
app → service → hardware
```

## 目录结构

```
VechicleSystem/
├── VechicleSystem.pro          Qt 项目文件
├── app/                        UI 应用层
│   ├── main.cpp                   程序入口
│   ├── mainwindow.h/cpp           主窗口 & 底部导航
│   ├── BottomTabBar.h/cpp         底部标签栏
│   ├── pages/                     各功能页面
│   │   ├── Page.h/cpp             首页仪表盘 / 车辆控制 / 地图
│   │   ├── CameraPage.h/cpp       相机页面
│   │   ├── MusicPage.h/cpp        音乐播放器页面
│   │   ├── GalleryPage.h/cpp      照片视频回放页面
│   │   └── ClockPage.h/cpp        世界时钟 / 定时器 / 秒表
│   └── shell/
│       ├── MyShell.h/cpp          Shell 终端（raw 模式输入、命令历史）
│       └── ShellCommands.h/cpp    命令系统（light/beep/camera/music）
├── service/                    服务层
│   └── mediad/                    媒体服务
│       ├── CameraDevice.h/cpp     GStreamer 相机（预览/拍照/录像/tee分流）
│       ├── MusicPlayer.h/cpp      GStreamer 音乐播放器（playbin）
│       └── VideoPlayer.h/cpp      GStreamer 视频回放器（playbin+appsink）
├── hardware/                   硬件抽象层
│   ├── IHardwareDevice.h          设备统一接口
│   ├── SysfsDevice.h/cpp          sysfs 文件读写基类
│   ├── Devices.h/cpp              灯 / 警报器 设备实现
│   └── HardwareManager.h/cpp      硬件管理器（单例 + 工厂模式）
├── res/                        资源文件
│   ├── res.qrc                    Qt 资源索引
│   ├── style.qss                  Qt 样式表（iOS 风格白色主题）
│   └── icons/                     SVG 图标
├── ui/                         Qt UI 表单
│   └── mainwindow.ui
├── media/music/                音乐文件目录
├── record/                     拍摄输出
│   ├── pic/                       拍照 (photo_xxxx.jpg)
│   └── video/                     录像 (video_xxxx.mpg)
├── logs/                       调试日志
└── build/                      编译产物（自动生成）
```

## 构建

### 依赖

```bash
# Fedora
sudo dnf install qt5-qtbase-devel \
    gstreamer1-devel gstreamer1-plugins-base-devel

# Debian/Ubuntu
sudo apt install qtbase5-dev \
    libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
```

### 编译

```bash
cd VechicleSystem/
qmake-qt5      # Fedora
# 或 qmake     # Debian/Ubuntu
make
```

编译产物在 `build/` 目录下：

```
build/
├── VechicleSystem    ← 可执行文件
├── obj/              ← .o 文件
├── moc/              ← Qt MOC 生成
├── rcc/              ← 资源编译
└── ui/               ← UI 生成
```

### 运行

```bash
./build/VechicleSystem
```

## 设计模式

| 模式 | 位置 | 说明 |
|------|------|------|
| Singleton | HardwareManager | 全局硬件管理器 |
| Interface | IHardwareDevice | 设备统一接口 |
| Template Method | SysfsDevice | sysfs 读写公共逻辑 |
| Command | ICommand → CommandDispatcher | Shell 命令路由 |
| Strategy | MusicPlayer::PlayMode | 播放模式切换 |
| Pimpl | CameraDevice / MusicPlayer | 隐藏 GStreamer 头文件 |

## 平台适配

通过 `#if __arm__` 宏区分 ARM 嵌入式和 x86 开发：

| 行为 | x86 | ARM |
|------|-----|-----|
| 窗口大小 | 800x480 | 自动全屏 |
| 硬件控制 | sysfs 文件不存在，返回 -1 | 正常操作 |
| 摄像头 | /dev/video0 | /dev/video0 |

## Shell 命令

| 命令 | 用法 |
|------|------|
| help | 显示命令列表 |
| light | `light <on\|off\|status>` |
| beep | `beep <on\|off\|status>` |
| camera | `camera <preview\|photo\|record\|stop> [path]` |
| music | `music <play\|pause\|resume\|stop\|next\|prev> [path]` |

Shell 支持 `↑↓` 翻历史（最近 100 条保留在内存中）、`←→` 移动光标、Backspace 删除、Ctrl+C 清行、Ctrl+D 退出。
