#include "MusicPlayer.h"
#include <QDebug>
#include <QTimer>
#include <cstring>
#include <cstdlib>

extern "C" {
#include <gst/gst.h>
#include <gst/gstbus.h>
}

struct MusicPlayer::GstImpl {
    GstElement *pipeline = nullptr;
    QTimer *positionTimer = nullptr;
    gulong busWatchId = 0;
};

static int musicBusCallback(GstBus *, GstMessage *msg, gpointer data);

MusicPlayer::MusicPlayer(QObject *parent)
    : QObject(parent)
    , m_impl(std::make_unique<GstImpl>())
{
    m_impl->positionTimer = new QTimer(this);
    m_impl->positionTimer->setInterval(500);
    connect(m_impl->positionTimer, &QTimer::timeout, this, &MusicPlayer::updatePosition);
}

MusicPlayer::~MusicPlayer()
{
    stop();
}

void MusicPlayer::play(const QString &filePath)
{
    stop();

    QString uri = filePath;
    if (!uri.startsWith("file://"))
        uri = "file://" + uri;

    GError *error = nullptr;
    m_impl->pipeline = gst_parse_launch(
        ("playbin uri=" + uri).toUtf8().constData(), &error);

    if (error) {
        emit errorOccurred(QString("playbin error: %1").arg(error->message));
        g_error_free(error);
        return;
    }

    GstBus *bus = gst_element_get_bus(m_impl->pipeline);
    m_impl->busWatchId = gst_bus_add_watch(bus, musicBusCallback, this);
    gst_object_unref(bus);

    setPipelineState(GST_STATE_PLAYING);

    m_currentFile = filePath;
    m_state = Playing;
    m_impl->positionTimer->start();
    emit currentFileChanged(filePath);
    emit stateChanged(m_state);
}

void MusicPlayer::resume()
{
    if (m_impl->pipeline && m_state == Paused) {
        setPipelineState(GST_STATE_PLAYING);
        m_state = Playing;
        m_impl->positionTimer->start();
        emit stateChanged(m_state);
    }
}

void MusicPlayer::pause()
{
    if (m_impl->pipeline && m_state == Playing) {
        setPipelineState(GST_STATE_PAUSED);
        m_state = Paused;
        m_impl->positionTimer->stop();
        emit stateChanged(m_state);
    }
}

void MusicPlayer::stop()
{
    m_impl->positionTimer->stop();
    if (m_impl->pipeline) {
        setPipelineState(GST_STATE_NULL);
        gst_object_unref(m_impl->pipeline);
        m_impl->pipeline = nullptr;
    }
    m_state = Stopped;
    emit stateChanged(m_state);
}

void MusicPlayer::setPlayMode(PlayMode mode) { m_playMode = mode; }

void MusicPlayer::seekTo(qint64 positionMs)
{
    if (!m_impl->pipeline || m_state == Stopped) return;
    gst_element_seek_simple(m_impl->pipeline, GST_FORMAT_TIME,
        static_cast<GstSeekFlags>(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
        positionMs * GST_MSECOND);
}

void MusicPlayer::setVolume(int percent)
{
    m_volume = qBound(0, percent, 100);
    if (m_impl->pipeline) {
        g_object_set(G_OBJECT(m_impl->pipeline), "volume",
                     m_volume / 100.0, nullptr);
    }
}

void MusicPlayer::setPlaylist(const QStringList &files)
{
    m_playlist = files;
    m_playlistIndex = files.isEmpty() ? -1 : 0;
}

void MusicPlayer::next()
{
    if (m_playlist.isEmpty())
        return;
    m_playlistIndex = (m_playlistIndex + 1) % m_playlist.size();
    play(m_playlist.at(m_playlistIndex));
}

void MusicPlayer::prev()
{
    if (m_playlist.isEmpty())
        return;
    m_playlistIndex = (m_playlistIndex - 1 + m_playlist.size()) % m_playlist.size();
    play(m_playlist.at(m_playlistIndex));
}

void MusicPlayer::setPipelineState(int gstState)
{
    gst_element_set_state(m_impl->pipeline,
                          static_cast<GstState>(gstState));
}

static int musicBusCallback(GstBus *, GstMessage *msg, gpointer data)
{
    MusicPlayer *self = static_cast<MusicPlayer *>(data);
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        QMetaObject::invokeMethod(self, [self]() {
            switch (self->playMode()) {
            case MusicPlayer::SingleLoop:
                if (!self->currentFile().isEmpty())
                    self->play(self->currentFile());
                break;
            case MusicPlayer::Shuffle: {
                auto list = self->playlist();
                if (!list.isEmpty())
                    self->play(list.at(rand() % list.size()));
                break;
            }
            case MusicPlayer::Sequential:
            default:
                self->next();
                break;
            }
        }, Qt::QueuedConnection);
        break;
    case GST_MESSAGE_ERROR: {
        GError *err = nullptr;
        gchar *debug = nullptr;
        gst_message_parse_error(msg, &err, &debug);
        QString errMsg = QString::fromUtf8(err->message);
        g_error_free(err);
        g_free(debug);
        QMetaObject::invokeMethod(self, [self, errMsg]() {
            emit self->errorOccurred(errMsg);
            self->stop();
        }, Qt::QueuedConnection);
        break;
    }
    default:
        break;
    }
    return TRUE;
}

void MusicPlayer::updatePosition()
{
    if (!m_impl->pipeline || m_state != Playing)
        return;

    gint64 pos = 0, dur = GST_CLOCK_TIME_NONE;
    if (gst_element_query_position(m_impl->pipeline, GST_FORMAT_TIME, &pos)) {
        emit positionChanged(pos / GST_MSECOND);
    }
    if (gst_element_query_duration(m_impl->pipeline, GST_FORMAT_TIME, &dur)) {
        emit durationChanged(dur / GST_MSECOND);
    }
}
