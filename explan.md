# VechicleSystem 设计说明

## 一、项目概述

VechicleSystem 是一个车载嵌入式 Linux 信息娱乐系统，采用 Qt5 作为 UI 框架、GStreamer 处理多媒体管道、sysfs 驱动硬件设备。架构分三层：`app`（UI 应用层）→ `service`（服务层）→ `hardware`（硬件抽象层）。

---

## 二、硬件抽象层（hardware/）

### 2.1 设计理念

硬件层提供统一设备接口，上层代码不直接操作 sysfs 文件，而是通过接口调用。添加新硬件只需实现接口即可。

### 2.2 核心接口

```cpp
class IHardwareDevice {
    virtual int on() = 0;        // 开启设备
    virtual int off() = 0;       // 关闭设备
    virtual int getStatus() = 0; // 查询状态
    virtual QString name() = 0;  // 设备名称（用作注册 key）
};
```

### 2.3 SysfsDevice — 模板方法模式

SysfsDevice 提取了 sysfs 设备的公共逻辑：打开文件、写入 "0"/"1"、读取行、解析状态。子类只需提供设备文件路径和名称：

```
SysfsDevice 基类：
  on()        → file.write("1") → getStatus()
  off()       → file.write("0") → getStatus()
  getStatus() → file.readLine() → 解析 "0"/"1"

LightDevice : public SysfsDevice
  → 构造函数传入路径 "/sys/.../sys-led/brightness"，名称 "light"

BeepDevice : public SysfsDevice
  → 构造函数传入路径 "/sys/.../beep/brightness"，名称 "beep"
```

消除 120 行重复代码，每个设备实现仅约 10 行。

### 2.4 HardwareManager — 单例 + 工厂

```cpp
class HardwareManager {
    static HardwareManager& instance();  // Meyer's Singleton
    template<typename T> T* getDevice(name); // 类型安全查询
    template<typename T> T* createDevice(args...); // 模板工厂
};
```

`initAll()` 在启动时创建所有默认设备，全局通过 `HardwareManager::instance().getDevice(name)` 访问。

---

## 三、UI 应用层（app/）

### 3.1 页面架构

```
BottomTabBar（底部导航，7 个标签）
  ├─ HomePage      首页仪表盘（时钟、状态卡片、2 秒自动刷新）
  ├─ VehiclePage   车辆控制（灯/警报器大卡片、56px 触控按钮）
  ├─ ClockPage     时钟（世界时钟、定时器、秒表，子标签切换）
  ├─ CameraPage    相机（进入自动预览、离开停止、拍照/录像/停止按钮）
  ├─ MusicPage     音乐（左侧播放列表 + 右侧封面/控制器/进度条）
  ├─ GalleryPage   回放（照片/视频文件列表 → 图片查看/视频播放）
  └─ MapPage       地图占位页
```

### 3.2 设计模式

| 模式 | 位置 | 说明 |
|------|------|------|
| Command | ShellCommands（ICommand → CommandDispatcher） | Shell 命令路由，添加命令零改动调度器 |
| Observer | Qt 信号槽 | 页面与设备解耦（CameraDevice::frameReady → 页面 onFrameReady） |
| Strategy | MusicPlayer::PlayMode（Sequential/SingleLoop/Shuffle） | 播放结束时的行为切换 |

### 3.3 Shell 终端

通过 termios 将终端设为 raw 模式（`~ECHO | ~ICANON`），逐字符读取实现完整行编辑功能：

| 按键 | 功能 |
|------|------|
| `↑` / `↓` | 翻历史命令（内存保留最近 100 条，无文件 I/O） |
| `←` / `→` | 移动光标 |
| Backspace | 删除前一个字符 |
| Ctrl+C | 清空当前行 |
| Ctrl+D | 退出 Shell |
| Tab | 忽略（防止插入制表符） |

按 Enter 执行命令后，通过 `CommandDispatcher::dispatch()` 路由到对应的 `ICommand` 子类：

```cpp
// 命令路由 — 零改动添加新命令
m_dispatcher->registerCommand(new LightCommand());  // 注册
m_dispatcher->dispatch(argc, argv);                  // 路由
```

转义序列解析核心：

```cpp
read(STDIN_FILENO, &c, 1);
if (c == '\033') { inEsc = true; continue; }
// \033[A = ↑, \033[B = ↓, \033[C = →, \033[D = ←

// 非转义字符直接插入行缓冲区
currentLine.insert(cursorPos, QChar(c));
```

### 3.4 QSS 样式

使用 iOS 风格白色主题，通过 objectName 选择器精确控制每个控件的样式。SVG 图标使用 `currentColor` 继承文字颜色，选中/未选中自动变色。

---

## 四、服务层（service/mediad/）

### 4.1 MusicPlayer — 音频播放

基于 GStreamer playbin，自动解复用和解码。播放模式通过 EOS 回调实现：

```
Sequential  → 自动切下一首（循环）
SingleLoop  → 重新播放当前文件
Shuffle     → 随机选取播放
```

### 4.2 VideoPlayer — 视频回放

使用 playbin 播放音视频文件。video-sink 指定为 `videoconvert ! appsink`，将视频帧转为 QImage 发射 `frameReady` 信号；audio-sink 使用系统默认（pulsesink / autoaudiosink），无需额外配置。在 Wayland 环境下避免了 `xvimagesink` 不兼容的问题。

### 4.3 CameraDevice — GStreamer 三条管道详解

CameraDevice 是服务层最复杂的组件，实现了双阀门（valve）+ tee 分流的管道架构，预览和录像共享同一个 v4l2src 实例。

#### 主管道结构

```
v4l2src device=/dev/video0
  → videoconvert                    // 色彩空间转换
  → tee name=t                      // 分流节点
    ├─ t. → queue → valve pv → videoscale → capsfilter(BGRx) → appsink    (预览分支)
    └─ t. → queue → valve rv → [录像 bin 动态链接]                         (录像分支)
```

两个 valve 分别控制预览和录像的启停，互不干扰。

#### 管道 1：预览流

```
v4l2src → videoconvert → tee
  → queue → valve(pv) drop=false
  → videoscale
  → video/x-raw,format=BGRx,width=640,height=480   // capsfilter 强制 BGRx
  → appsink emit-signals=true sync=false
```

**设计要点：**

1. **format=BGRx 强制格式**：必须显式指定输出格式。不指定的话 videoconvert 可能输出 I420/YUV，导致 QImage 解析出黑白重影画面（实际踩坑记录在 logs/vechicle-debug.log）。

2. **appsink + 回调**：每个帧到达时触发 `onNewSampleCallback`，将 GstBuffer 转为 QImage。BGRx（4 字节/像素）映射为 `QImage::Format_RGB32`（小端平台）。

3. **sync=false**：预览帧不需要同步时钟，立即推送到 appsink，降低延迟。

4. **GstVideoInfo stride 处理**：缓冲区可能存在行对齐填充。当 `videoInfo.stride[0] != width*4` 时，逐行 memcpy 到 QImage：

```cpp
if (videoInfo.stride[0] == expectedStride) {
    img = QImage(data, w, h, stride, QImage::Format_RGB32).copy();
} else {
    for (int row = 0; row < h; row++)
        memcpy(temp.scanLine(row), src + row*stride, w*4);
}
```

#### 管道 2：拍照流

拍照不使用单独的 GStreamer 管道，而是**直接抓取预览流的最后一帧**，用 QImage::save() 写入 JPEG：

```cpp
int CameraDevice::capturePhoto(const QString &filePath) {
    // m_lastFrame 由预览 appsink 回调持续更新
    m_lastFrame.save(filePath, "JPEG", 90);
}
```

这样做的好处：拍照不中断预览，零延迟，无需额外管道。

#### 管道 3：录像流（音视频同步）

录像分支包含视频编码链和音频采集链，两条链汇入同一个 muxer，最终写入 MPEG PS 文件：

```
recValve (来自 tee)
  → videoconvert → avenc_mpeg1video (2Mbps) ─┐
                                               ├→ mpegpsmux → filesink
autoaudiosrc → audioconvert → audioresample   │
            → avenc_mp2 ───────────────────────┘
```

**动态构建流程：**

```cpp
// 1. 手动创建每个元素（不再使用 gst_parse_bin_from_description）
m_impl->recVidConv = gst_element_factory_make("videoconvert", "...");
m_impl->recVidEnc  = gst_element_factory_make("avenc_mpeg1video", "...");
m_impl->recAudSrc  = gst_element_factory_make("autoaudiosrc", "...");
m_impl->recAudEnc  = gst_element_factory_make("avenc_mp2", "...");
m_impl->recMux     = gst_element_factory_make("mpegpsmux", "...");
m_impl->recSink    = gst_element_factory_make("filesink", "...");

// 2. 一次性加入管道
gst_bin_add_many(GST_BIN(pipeline),
    recVidConv, recVidEnc, recAudSrc, recAudConv,
    recAudResample, recAudEnc, recMux, recSink, nullptr);

// 3. 链接视频链：recValve → recVidConv → recVidEnc → muxer
gst_element_link_many(recVidConv, recVidEnc, nullptr);
gst_element_link(recVidEnc, recMux);

// 4. 链接音频链：audiosrc → audioconv → audioresample → audioenc → muxer
gst_element_link_many(recAudSrc, recAudConv, recAudResample, recAudEnc, nullptr);
gst_element_link(recAudEnc, recMux);

// 5. 链接 muxer → filesink
gst_element_link(recMux, recSink);

// 6. 链接阀门到视频链
GstPad *valveSrc = gst_element_get_static_pad(recValve, "src");
GstPad *vidSink  = gst_element_get_static_pad(recVidConv, "sink");
gst_pad_link(valveSrc, vidSink);

// 7. 同步所有元素状态
gst_element_sync_state_with_parent(recVidConv);
gst_element_sync_state_with_parent(recVidEnc);
// ... 每个元素都同步

// 8. 打开阀门 → 录像开始
g_object_set(recValve, "drop", FALSE, nullptr);
```

**停止录像：**

```cpp
// 1. 关闭阀门
g_object_set(recValve, "drop", TRUE, nullptr);

// 2. 向录像阀门发送 EOS，让编码器和 muxer 完成文件写入
GstPad *sinkPad = gst_element_get_static_pad(recValve, "sink");
gst_pad_send_event(sinkPad, gst_event_new_eos());

// 3. 反链接、设 NULL 状态、逐个从管道移除
gst_pad_unlink(valveSrc, peer);
gst_element_set_state(el, GST_STATE_NULL);
gst_bin_remove(GST_BIN(pipeline), el);
```

**设计要点：**

1. **手动构建代替 bin**：因为需要两条输入链（视频+音频），`gst_parse_bin_from_description` 只能创建单个 sink ghost pad，无法同时接受视频和音频。手动构建每个元素并链接，灵活控制 muxer 的多个输入 pad。

2. **mpegpsmux 的多 pad 特性**：mpegpsmux 通过 request pad 机制接受多路输入。视频编码器链接到第一个 pad，音频编码器链接到第二个 pad，muxer 自动交错写入。

3. **为什么用 avenc_mp2 编码音频？** MP2 是 MPEG PS 容器的标准音频格式，与 `mpegpsmux` 原生兼容。`avenc_mp2` 由 libav 提供，始终可用。

4. **为什么用 autoaudiosrc？** 自动选择系统默认音频源（PulseAudio / ALSA），无需硬编码设备名。

#### CameraPage 生命周期

```
进入页面 (showEvent)    → CameraDevice::on()   → 打开预览阀门
拍照                    → capturePhoto()       → m_lastFrame.save()
录像开始                → startRecording()     → 动态链接录像 bin，打开录像阀门
录像停止                → stopRecording()      → 关闭录像阀门，EOS，移除 bin
离开页面 (hideEvent)    → CameraDevice::off()  → if 录像中: 只关预览阀门
                                               → if 非录像: 销毁管道
```

---

## 五、设计决策记录

| 决策 | 选择 | 原因 |
|------|------|------|
| 相机视频编码 | avenc_mpeg1video | 系统无 x264enc，libav 编码器始终可用 |
| 视频封装 | mpegpsmux | 配合 MPEG-1 编码器 |
| Wayland 视频渲染 | appsink + QImage | xvimagesink 不兼容 Wayland |
| Shell 输入 | termios raw 模式 | 实现方向键支持，无需额外依赖 readline |
| 拍照方案 | 抓取预览帧 save() | 零延迟，不中断预览 |
| 录像预览并存 | tee + 双 valve | 共享 v4l2src，独立控制 |
