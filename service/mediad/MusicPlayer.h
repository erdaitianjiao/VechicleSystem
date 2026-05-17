#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <memory>

class MusicPlayer : public QObject {
    Q_OBJECT
public:
    explicit MusicPlayer(QObject *parent = nullptr);
    ~MusicPlayer();

    enum State { Stopped, Playing, Paused };
    State state() const { return m_state; }

    void play(const QString &filePath);
    void resume();
    void pause();
    void stop();
    void setVolume(int percent);
    int volume() const { return m_volume; }

    void seekTo(qint64 positionMs);

    enum PlayMode { Sequential, SingleLoop, Shuffle };
    void setPlayMode(PlayMode mode);
    PlayMode playMode() const { return m_playMode; }

    void setPlaylist(const QStringList &files);
    QStringList playlist() const { return m_playlist; }
    void next();
    void prev();

    QString currentFile() const { return m_currentFile; }

signals:
    void stateChanged(int state);
    void positionChanged(qint64 positionMs);
    void durationChanged(qint64 durationMs);
    void currentFileChanged(const QString &filePath);
    void errorOccurred(const QString &message);

private:
    struct GstImpl;
    std::unique_ptr<GstImpl> m_impl;
    State m_state = Stopped;
    int m_volume = 80;
    QString m_currentFile;
    QStringList m_playlist;
    int m_playlistIndex = -1;
    PlayMode m_playMode = Sequential;

    void setPipelineState(int gstState);
    void updatePosition();
};

#endif // MUSICPLAYER_H
