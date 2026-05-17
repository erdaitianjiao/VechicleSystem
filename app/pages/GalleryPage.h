#ifndef GALLERYPAGE_H
#define GALLERYPAGE_H

#include <QWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>

class VideoPlayer;

class GalleryPage : public QWidget {
    Q_OBJECT
public:
    explicit GalleryPage(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void onItemClicked(QListWidgetItem *item);
    void onBack();
    void onVideoFrame(const QImage &frame);

private:
    QStackedWidget *m_stack;

    // File list view
    QWidget *m_listPage;
    QListWidget *m_fileList;

    // Photo view
    QWidget *m_photoPage;
    QLabel *m_photoLabel;

    // Video view
    QWidget *m_videoPage;
    QLabel *m_videoLabel;
    QPushButton *m_videoPlayBtn;
    QPushButton *m_videoStopBtn;

    VideoPlayer *m_player;
    bool m_videoPlaying = false;

    void refreshFileList();
    void showPhoto(const QString &path);
    void playVideo(const QString &path);
};

#endif // GALLERYPAGE_H
