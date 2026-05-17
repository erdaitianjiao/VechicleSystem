#include "CameraDevice.h"
#include <QDebug>
#include <QMetaObject>
#include <cstring>

extern "C" {
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video-format.h>
#include <gst/video/video-info.h>
}

#define LOG_PREFIX "[Camera]"

static GstFlowReturn onNewSampleCallback(GstAppSink *appsink, gpointer user_data);

struct CameraDevice::GstImpl {
    GstElement *pipeline = nullptr;
    GstElement *previewValve = nullptr;
    GstElement *appsink = nullptr;
    GstElement *recValve = nullptr;
    GstElement *recBin = nullptr;
};

// ---- helpers ----

static void logState(const char *step, GstStateChangeReturn ret)
{
    const char *s = "?";
    switch (ret) {
    case GST_STATE_CHANGE_SUCCESS:  s = "SUCCESS"; break;
    case GST_STATE_CHANGE_FAILURE:  s = "FAILURE"; break;
    case GST_STATE_CHANGE_ASYNC:    s = "ASYNC"; break;
    case GST_STATE_CHANGE_NO_PREROLL: s = "NO_PREROLL"; break;
    }
    qDebug() << LOG_PREFIX << step << "state:" << s;
}

static void logBusError(GstElement *pipe)
{
    GstBus *bus = gst_element_get_bus(pipe);
    GstMessage *msg = gst_bus_pop_filtered(bus,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING));
    while (msg) {
        GError *err = nullptr; gchar *dbg = nullptr;
        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_ERROR) {
            gst_message_parse_error(msg, &err, &dbg);
            qDebug() << LOG_PREFIX << "BUS ERROR:" << err->message;
        } else {
            gst_message_parse_warning(msg, &err, &dbg);
            qDebug() << LOG_PREFIX << "BUS WARN:" << err->message;
        }
        if (dbg) qDebug() << LOG_PREFIX << "  debug:" << dbg;
        g_error_free(err); g_free(dbg);
        gst_message_unref(msg);
        msg = gst_bus_pop_filtered(bus,
            static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_WARNING));
    }
    gst_object_unref(bus);
}

// ---- lifecycle ----

CameraDevice::CameraDevice(const QString &videoDevice, QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<GstImpl>())
    , m_devicePath(videoDevice)
{
    static bool init = false;
    if (!init) { gst_init(nullptr, nullptr); init = true; }
    qDebug() << LOG_PREFIX << "created, device:" << m_devicePath;
}

CameraDevice::~CameraDevice()
{
    qDebug() << LOG_PREFIX << "destroying";
    destroyPipeline();
}

// ---- Preview control ----

int CameraDevice::on()
{
    qDebug() << LOG_PREFIX << "on(), state:" << m_state;

    if (m_state != Stopped) {
        // Pipeline already exists — just open preview valve
        if (m_impl->previewValve)
            g_object_set(G_OBJECT(m_impl->previewValve), "drop", FALSE, nullptr);
        m_state = m_recording ? Recording : Previewing;
        qDebug() << LOG_PREFIX << "preview resumed";
        return 0;
    }

    if (!buildPipeline()) {
        qDebug() << LOG_PREFIX << "on() FAILED: build";
        return -1;
    }

    // Open preview valve (already open from pipeline string drop=false)
    GstStateChangeReturn ret = gst_element_set_state(m_impl->pipeline, GST_STATE_PLAYING);
    logState("on()", ret);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        logBusError(m_impl->pipeline);
        destroyPipeline();
        return -1;
    }
    if (ret == GST_STATE_CHANGE_ASYNC) {
        GstState s;
        ret = gst_element_get_state(m_impl->pipeline, &s, nullptr, 3 * GST_SECOND);
        if (ret == GST_STATE_CHANGE_FAILURE) {
            logBusError(m_impl->pipeline);
            destroyPipeline();
            return -1;
        }
    }

    m_state = Previewing;
    qDebug() << LOG_PREFIX << "on() OK, previewing";
    return 0;
}

int CameraDevice::off()
{
    qDebug() << LOG_PREFIX << "off(), state:" << m_state << "recording:" << m_recording;

    if (m_recording) {
        // Close preview, keep recording
        if (m_impl->previewValve)
            g_object_set(G_OBJECT(m_impl->previewValve), "drop", TRUE, nullptr);
        m_state = Recording;
        qDebug() << LOG_PREFIX << "preview closed, recording continues";
        return 0;
    }

    destroyPipeline();
    m_state = Stopped;
    qDebug() << LOG_PREFIX << "off() done";
    return 0;
}

int CameraDevice::getStatus()
{
    return static_cast<int>(m_state);
}

// ---- Photo ----

int CameraDevice::capturePhoto(const QString &filePath)
{
    qDebug() << LOG_PREFIX << "capturePhoto:" << filePath;
    if (m_lastFrame.isNull()) {
        qDebug() << LOG_PREFIX << "photo FAILED: no frame";
        emit errorOccurred("拍照失败：没有预览帧");
        return -1;
    }
    if (m_lastFrame.save(filePath, "JPEG", 90)) {
        qDebug() << LOG_PREFIX << "photo OK, size:" << m_lastFrame.size();
        return 0;
    }
    qDebug() << LOG_PREFIX << "photo FAILED: save error";
    emit errorOccurred("拍照失败：无法保存");
    return -1;
}

// ---- Recording ----

int CameraDevice::startRecording(const QString &filePath)
{
    qDebug() << LOG_PREFIX << "startRecording:" << filePath;

    if (m_recording) {
        qDebug() << LOG_PREFIX << "already recording";
        return 0;
    }
    if (m_state == Stopped) {
        qDebug() << LOG_PREFIX << "startRecording FAILED: pipeline not running";
        return -1;
    }

    if (!linkRecordBin(filePath)) {
        qDebug() << LOG_PREFIX << "startRecording FAILED: linkRecordBin";
        return -1;
    }

    g_object_set(G_OBJECT(m_impl->recValve), "drop", FALSE, nullptr);
    m_recording = true;
    m_state = Recording;
    emit recordingStarted();
    qDebug() << LOG_PREFIX << "recording STARTED";
    return 0;
}

int CameraDevice::stopRecording()
{
    qDebug() << LOG_PREFIX << "stopRecording";

    if (!m_recording) { qDebug() << LOG_PREFIX << "not recording"; return 0; }

    g_object_set(G_OBJECT(m_impl->recValve), "drop", TRUE, nullptr);

    // Send EOS to finalize file
    GstPad *sinkPad = gst_element_get_static_pad(m_impl->recValve, "sink");
    gst_pad_send_event(sinkPad, gst_event_new_eos());
    gst_object_unref(sinkPad);

    qDebug() << LOG_PREFIX << "EOS sent, finalizing...";

    // Check for errors
    GstBus *bus = gst_element_get_bus(m_impl->pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(
        bus, 2 * GST_SECOND,
        static_cast<GstMessageType>(GST_MESSAGE_ERROR));
    if (msg) {
        GError *err = nullptr; gchar *dbg = nullptr;
        gst_message_parse_error(msg, &err, &dbg);
        qDebug() << LOG_PREFIX << "record finalize error:" << err->message;
        g_error_free(err); g_free(dbg);
        gst_message_unref(msg);
    }
    gst_object_unref(bus);

    removeRecordBin();
    m_recording = false;
    m_state = Previewing;
    emit recordingStopped();
    qDebug() << LOG_PREFIX << "recording stopped";
    return 0;
}

// ---- Pipeline ----

bool CameraDevice::buildPipeline()
{
    // Pipeline with double valve:
    // v4l2src → videoconvert → tee name=t
    //   t. → queue → valve name=pv drop=false → videoscale → capsfilter → appsink  (preview)
    //   t. → queue → valve name=rv drop=true                                   (recording, to be linked)
    QString desc = QString(
        "v4l2src device=%1 ! "
        "videoconvert ! "
        "tee name=t "
        "t. ! queue ! valve name=pv drop=false ! videoscale ! "
        "video/x-raw,format=BGRx,width=640,height=480 ! "
        "appsink name=sink emit-signals=true sync=false "
        "t. ! queue ! valve name=rv drop=true")
        .arg(m_devicePath);

    qDebug() << LOG_PREFIX << "building pipeline:" << desc;

    GError *error = nullptr;
    m_impl->pipeline = gst_parse_launch(desc.toUtf8().constData(), &error);
    if (error) {
        qDebug() << LOG_PREFIX << "parse error:" << error->message;
        g_error_free(error);
        return false;
    }

    m_impl->previewValve = gst_bin_get_by_name(GST_BIN(m_impl->pipeline), "pv");
    m_impl->recValve = gst_bin_get_by_name(GST_BIN(m_impl->pipeline), "rv");
    m_impl->appsink = gst_bin_get_by_name(GST_BIN(m_impl->pipeline), "sink");

    if (!m_impl->previewValve || !m_impl->recValve || !m_impl->appsink) {
        qDebug() << LOG_PREFIX << "missing elements";
        destroyPipeline();
        return false;
    }

    GstCaps *caps = gst_caps_from_string("video/x-raw,format=BGRx,width=640,height=480");
    gst_app_sink_set_caps(GST_APP_SINK(m_impl->appsink), caps);
    gst_caps_unref(caps);

    GstAppSinkCallbacks cb = {};
    cb.new_sample = onNewSampleCallback;
    gst_app_sink_set_callbacks(GST_APP_SINK(m_impl->appsink), &cb, this, nullptr);

    qDebug() << LOG_PREFIX << "pipeline built OK";
    return true;
}

bool CameraDevice::linkRecordBin(const QString &filePath)
{
    QString desc = QString(
        "videoconvert ! "
        "avenc_mpeg1video bitrate=2000000 ! "
        "mpegpsmux ! "
        "filesink location=%1 name=recsink")
        .arg(filePath);

    GError *error = nullptr;
    m_impl->recBin = gst_parse_bin_from_description(desc.toUtf8().constData(), TRUE, &error);
    if (error) {
        qDebug() << LOG_PREFIX << "rec bin parse error:" << error->message;
        g_error_free(error);
        return false;
    }

    gst_bin_add(GST_BIN(m_impl->pipeline), m_impl->recBin);

    GstPad *valveSrc = gst_element_get_static_pad(m_impl->recValve, "src");
    GstPad *binSink = gst_element_get_static_pad(m_impl->recBin, "sink");

    GstPadLinkReturn linkRet = gst_pad_link(valveSrc, binSink);
    gst_object_unref(valveSrc);
    gst_object_unref(binSink);

    if (linkRet != GST_PAD_LINK_OK) {
        qDebug() << LOG_PREFIX << "rec pad link FAILED";
        gst_bin_remove(GST_BIN(m_impl->pipeline), m_impl->recBin);
        m_impl->recBin = nullptr;
        return false;
    }

    gst_element_sync_state_with_parent(m_impl->recBin);
    qDebug() << LOG_PREFIX << "rec bin linked OK";
    return true;
}

void CameraDevice::removeRecordBin()
{
    if (!m_impl->recBin) return;

    GstPad *valveSrc = gst_element_get_static_pad(m_impl->recValve, "src");
    GstPad *peer = gst_pad_get_peer(valveSrc);
    if (peer) { gst_pad_unlink(valveSrc, peer); gst_object_unref(peer); }
    gst_object_unref(valveSrc);

    gst_element_set_state(m_impl->recBin, GST_STATE_NULL);
    gst_bin_remove(GST_BIN(m_impl->pipeline), m_impl->recBin);
    m_impl->recBin = nullptr;
    qDebug() << LOG_PREFIX << "rec bin removed";
}

void CameraDevice::destroyPipeline()
{
    if (!m_impl->pipeline) return;

    qDebug() << LOG_PREFIX << "destroying pipeline...";

    if (m_recording)
        removeRecordBin();

    gst_element_set_state(m_impl->pipeline, GST_STATE_NULL);

    if (m_impl->appsink)      { gst_object_unref(m_impl->appsink);      m_impl->appsink = nullptr; }
    if (m_impl->previewValve) { gst_object_unref(m_impl->previewValve); m_impl->previewValve = nullptr; }
    if (m_impl->recValve)     { gst_object_unref(m_impl->recValve);     m_impl->recValve = nullptr; }

    gst_object_unref(m_impl->pipeline);
    m_impl->pipeline = nullptr;
    m_recording = false;
    m_state = Stopped;
    qDebug() << LOG_PREFIX << "pipeline destroyed";
}

// ---- appsink callback ----

static GstFlowReturn onNewSampleCallback(GstAppSink *appsink, gpointer user_data)
{
    CameraDevice *self = static_cast<CameraDevice *>(user_data);

    GstSample *sample = gst_app_sink_pull_sample(appsink);
    if (!sample) return GST_FLOW_OK;

    GstCaps *caps = gst_sample_get_caps(sample);
    GstStructure *s = gst_caps_get_structure(caps, 0);
    int w = 0, h = 0;
    gst_structure_get_int(s, "width", &w);
    gst_structure_get_int(s, "height", &h);

    GstVideoInfo info;
    if (!gst_video_info_from_caps(&info, caps)) {
        gst_sample_unref(sample);
        return GST_FLOW_OK;
    }

    GstBuffer *buf = gst_sample_get_buffer(sample);
    GstMapInfo map;
    if (gst_buffer_map(buf, &map, GST_MAP_READ)) {
        QImage img;
        int strideNeed = w * 4;

        if (info.stride[0] == strideNeed) {
            img = QImage(reinterpret_cast<const uchar *>(map.data),
                         w, h, info.stride[0], QImage::Format_RGB32).copy();
        } else {
            const uchar *src = map.data;
            QImage tmp(w, h, QImage::Format_RGB32);
            for (int row = 0; row < h; row++) {
                memcpy(tmp.scanLine(row), src, strideNeed);
                src += info.stride[0];
            }
            img = tmp;
        }
        gst_buffer_unmap(buf, &map);

        self->setLastFrame(img);

        QMetaObject::invokeMethod(self, [self, img]() {
            emit self->frameReady(img);
        }, Qt::QueuedConnection);
    } else {
        gst_buffer_unmap(buf, &map);
    }

    gst_sample_unref(sample);
    return GST_FLOW_OK;
}
