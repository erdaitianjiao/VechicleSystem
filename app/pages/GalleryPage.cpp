#include "GalleryPage.h"
#include "VideoPlayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <QFileInfo>
#include <QPixmap>
#include <QShowEvent>
#include <QFrame>

GalleryPage::GalleryPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_stack = new QStackedWidget(this);
    layout->addWidget(m_stack);

    // ---- List page ----
    m_listPage = new QWidget(this);
    auto *listLayout = new QVBoxLayout(m_listPage);
    listLayout->setContentsMargins(12, 12, 12, 12);

    auto *title = new QLabel("回放", this);
    title->setObjectName("pageTitle");
    title->setAlignment(Qt::AlignCenter);
    listLayout->addWidget(title);

    m_fileList = new QListWidget(this);
    m_fileList->setObjectName("galleryList");
    m_fileList->setSpacing(4);
    listLayout->addWidget(m_fileList, 1);

    m_stack->addWidget(m_listPage);  // 0

    // ---- Photo view ----
    m_photoPage = new QWidget(this);
    auto *photoLay = new QVBoxLayout(m_photoPage);
    photoLay->setContentsMargins(0, 0, 0, 0);

    m_photoLabel = new QLabel(this);
    m_photoLabel->setObjectName("galleryViewer");
    m_photoLabel->setAlignment(Qt::AlignCenter);
    photoLay->addWidget(m_photoLabel, 1);

    auto *photoBack = new QPushButton("返回列表", this);
    photoBack->setObjectName("openFileBtn");
    photoBack->setMinimumHeight(44);
    connect(photoBack, &QPushButton::clicked, this, &GalleryPage::onBack);
    photoLay->addWidget(photoBack);

    m_stack->addWidget(m_photoPage);  // 1

    // ---- Video view ----
    m_videoPage = new QWidget(this);
    auto *videoLay = new QVBoxLayout(m_videoPage);
    videoLay->setContentsMargins(0, 0, 0, 0);

    m_videoLabel = new QLabel(this);
    m_videoLabel->setObjectName("galleryViewer");
    m_videoLabel->setAlignment(Qt::AlignCenter);
    m_videoLabel->setText("点击播放");
    videoLay->addWidget(m_videoLabel, 1);

    auto *vidCtrls = new QHBoxLayout();
    m_videoPlayBtn = new QPushButton("播放", this);
    m_videoPlayBtn->setObjectName("camBtn");
    m_videoPlayBtn->setMinimumHeight(44);
    m_videoStopBtn = new QPushButton("返回", this);
    m_videoStopBtn->setObjectName("camBtn");
    m_videoStopBtn->setMinimumHeight(44);

    vidCtrls->addStretch();
    vidCtrls->addWidget(m_videoPlayBtn);
    vidCtrls->addWidget(m_videoStopBtn);
    vidCtrls->addStretch();
    videoLay->addLayout(vidCtrls);

    m_stack->addWidget(m_videoPage);  // 2

    // Player
    m_player = new VideoPlayer(this);

    // Connections
    connect(m_fileList, &QListWidget::itemClicked, this, &GalleryPage::onItemClicked);
    connect(m_player, &VideoPlayer::frameReady, this, &GalleryPage::onVideoFrame);
    connect(m_videoPlayBtn, &QPushButton::clicked, this, [this]() {
        if (!m_videoPlaying) return;
        if (m_player->state() == VideoPlayer::Playing) {
            m_player->pause();
            m_videoPlayBtn->setText("播放");
        } else {
            m_player->resume();
            m_videoPlayBtn->setText("暂停");
        }
    });
    connect(m_videoStopBtn, &QPushButton::clicked, this, &GalleryPage::onBack);
    connect(m_player, &VideoPlayer::playbackFinished, this, [this]() {
        m_videoPlaying = false;
        m_videoPlayBtn->setText("播放");
    });
}

void GalleryPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refreshFileList();
}

void GalleryPage::refreshFileList()
{
    m_fileList->clear();

    auto addFiles = [this](const QString &dir, const QString &prefix) {
        QDir d(dir);
        if (!d.exists()) return;
        for (const QFileInfo &fi : d.entryInfoList(QDir::Files, QDir::Time)) {
            auto *item = new QListWidgetItem(
                QString("%1  %2  %3 KB")
                    .arg(prefix)
                    .arg(fi.fileName())
                    .arg(fi.size() / 1024));
            item->setData(Qt::UserRole, fi.absoluteFilePath());
            m_fileList->addItem(item);
        }
    };

    addFiles("record/pic",   "[照片]");
    addFiles("record/video", "[视频]");
}

void GalleryPage::onItemClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    if (path.endsWith(".jpg") || path.endsWith(".png") || path.endsWith(".jpeg")) {
        showPhoto(path);
    } else {
        playVideo(path);
    }
}

void GalleryPage::showPhoto(const QString &path)
{
    QPixmap pix(path);
    m_photoLabel->setPixmap(pix.scaled(m_photoLabel->size(),
        Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_stack->setCurrentIndex(1);
}

void GalleryPage::playVideo(const QString &path)
{
    m_videoLabel->setText("加载中...");
    m_stack->setCurrentIndex(2);
    m_player->play(path);
    m_videoPlaying = true;
    m_videoPlayBtn->setText("暂停");
}

void GalleryPage::onVideoFrame(const QImage &frame)
{
    m_videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(
        m_videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void GalleryPage::onBack()
{
    m_player->stop();
    m_videoPlaying = false;
    m_videoPlayBtn->setText("播放");
    m_stack->setCurrentIndex(0);
}
