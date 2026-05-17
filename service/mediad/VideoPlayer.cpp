#include "VideoPlayer.h"
#include <QDebug>
#include <QMetaObject>
#include <QTimer>
#include <cstring>

extern "C" {
#include <gst/gst.h>
#include <gst/app/gstappsink.h>
#include <gst/video/video-info.h>
}

#define LOG_VP "[VideoPlayer]"

static GstFlowReturn vpNewSample(GstAppSink *sink, gpointer data);

struct VideoPlayer::Impl {
    GstElement *pipeline = nullptr;
    QTimer *posTimer = nullptr;
};

VideoPlayer::VideoPlayer(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<Impl>())
{
    m_impl->posTimer = new QTimer(this);
    m_impl->posTimer->setInterval(300);
    connect(m_impl->posTimer, &QTimer::timeout, this, [this]() {
        if (!m_impl->pipeline || m_state != Playing) return;
        gint64 pos = 0, dur = GST_CLOCK_TIME_NONE;
        if (gst_element_query_position(m_impl->pipeline, GST_FORMAT_TIME, &pos))
            emit positionChanged(pos / GST_MSECOND);
        if (gst_element_query_duration(m_impl->pipeline, GST_FORMAT_TIME, &dur))
            emit durationChanged(dur / GST_MSECOND);
    });
}

VideoPlayer::~VideoPlayer() { stop(); }

GstElement *VideoPlayer::pipeline() const { return m_impl->pipeline; }

void VideoPlayer::play(const QString &filePath)
{
    stop();
    QString uri = filePath;
    if (!uri.startsWith("file://")) uri = "file://" + uri;

    // playbin with appsink for video output
    QString desc = QString(
        "playbin uri=%1 "
        "video-sink=\"videoconvert ! video/x-raw,format=BGRx ! appsink name=vpsink emit-signals=true sync=true\"")
        .arg(uri);

    qDebug() << LOG_VP << "play:" << desc;

    GError *error = nullptr;
    m_impl->pipeline = gst_parse_launch(desc.toUtf8().constData(), &error);
    if (error) {
        qDebug() << LOG_VP << "parse error:" << error->message;
        emit errorOccurred(QString("play: %1").arg(error->message));
        g_error_free(error);
        return;
    }

    GstElement *appsink = gst_bin_get_by_name(GST_BIN(m_impl->pipeline), "vpsink");
    if (appsink) {
        GstAppSinkCallbacks cb = {};
        cb.new_sample = vpNewSample;
        gst_app_sink_set_callbacks(GST_APP_SINK(appsink), &cb, this, nullptr);
        gst_object_unref(appsink);
    }

    gst_element_set_state(m_impl->pipeline, GST_STATE_PLAYING);
    m_state = Playing;
    m_impl->posTimer->start();
    emit stateChanged(m_state);
}

void VideoPlayer::pause()
{
    if (m_impl->pipeline && m_state == Playing) {
        gst_element_set_state(m_impl->pipeline, GST_STATE_PAUSED);
        m_state = Paused;
        m_impl->posTimer->stop();
        emit stateChanged(m_state);
    }
}

void VideoPlayer::resume()
{
    if (m_impl->pipeline && m_state == Paused) {
        gst_element_set_state(m_impl->pipeline, GST_STATE_PLAYING);
        m_state = Playing;
        m_impl->posTimer->start();
        emit stateChanged(m_state);
    }
}

void VideoPlayer::stop()
{
    m_impl->posTimer->stop();
    if (m_impl->pipeline) {
        gst_element_set_state(m_impl->pipeline, GST_STATE_NULL);
        gst_object_unref(m_impl->pipeline);
        m_impl->pipeline = nullptr;
    }
    m_state = Stopped;
    emit stateChanged(m_state);
}

static GstFlowReturn vpNewSample(GstAppSink *sink, gpointer data)
{
    VideoPlayer *self = static_cast<VideoPlayer *>(data);
    GstSample *sample = gst_app_sink_pull_sample(sink);
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

        QMetaObject::invokeMethod(self, [self, img]() { emit self->frameReady(img); },
                                  Qt::QueuedConnection);
    } else {
        gst_buffer_unmap(buf, &map);
    }

    gst_sample_unref(sample);

    // Check for EOS
    GstBus *bus = gst_element_get_bus(self->pipeline());
    GstMessage *msg = gst_bus_pop_filtered(bus, static_cast<GstMessageType>(
        GST_MESSAGE_EOS | GST_MESSAGE_ERROR));
    if (msg) {
        if (GST_MESSAGE_TYPE(msg) == GST_MESSAGE_EOS) {
            QMetaObject::invokeMethod(self, [self]() {
                self->stop();
                emit self->playbackFinished();
            }, Qt::QueuedConnection);
        }
        gst_message_unref(msg);
    }
    gst_object_unref(bus);

    return GST_FLOW_OK;
}
