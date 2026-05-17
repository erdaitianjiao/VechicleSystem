#ifndef MUSICPAGE_H
#define MUSICPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSlider>
#include <QListWidget>

class MusicPlayer;

class MusicPage : public QWidget {
    Q_OBJECT
public:
    explicit MusicPage(MusicPlayer *player, QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onPlayPause();
    void onStop();
    void onNext();
    void onPrev();
    void onModeToggle();
    void onFileClicked(QListWidgetItem *item);
    void onSeekPressed();
    void onSeekReleased();
    void onStateChanged(int state);
    void onPositionChanged(qint64 posMs);
    void onDurationChanged(qint64 durMs);
    void onCurrentFileChanged(const QString &path);

private:
    MusicPlayer *m_player;
    QListWidget *m_fileList;
    QPushButton *m_playBtn;
    QPushButton *m_stopBtn;
    QPushButton *m_nextBtn;
    QPushButton *m_prevBtn;
    QLabel *m_titleLabel;
    QLabel *m_albumArt;
    QLabel *m_timeLabel;
    QPushButton *m_modeBtn;
    QSlider *m_volumeSlider;
    QSlider *m_seekSlider;
    bool m_seeking = false;

    void refreshFileList();
};

#endif // MUSICPAGE_H
