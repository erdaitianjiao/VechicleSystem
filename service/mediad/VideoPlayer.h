#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>
#include <QImage>
#include <QString>
#include <memory>

struct _GstElement;
typedef struct _GstElement GstElement;

class VideoPlayer : public QObject {
    Q_OBJECT
public:
    explicit VideoPlayer(QObject *parent = nullptr);
    ~VideoPlayer();

    void play(const QString &filePath);
    void pause();
    void resume();
    void stop();

    enum State { Stopped, Playing, Paused };
    State state() const { return m_state; }
    GstElement *pipeline() const;

signals:
    void frameReady(const QImage &frame);
    void stateChanged(int state);
    void positionChanged(qint64 posMs);
    void durationChanged(qint64 durMs);
    void playbackFinished();
    void errorOccurred(const QString &msg);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
    State m_state = Stopped;

    void setLastFrame(const QImage &img);
    friend struct Impl;
};

#endif // VIDEOPLAYER_H
