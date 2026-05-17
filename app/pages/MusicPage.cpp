#include "MusicPage.h"
#include "MusicPlayer.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QDir>
#include <QFileInfo>
#include <QShowEvent>
#include <QPixmap>

MusicPage::MusicPage(MusicPlayer *player, QWidget *parent)
    : QWidget(parent)
    , m_player(player)
{
    auto *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ═══ LEFT: playlist sidebar ═══
    auto *sidebar = new QFrame(this);
    sidebar->setObjectName("musicSidebar");
    sidebar->setFixedWidth(150);
    auto *sideLay = new QVBoxLayout(sidebar);
    sideLay->setContentsMargins(8, 12, 8, 12);

    auto *sideTitle = new QLabel("播放列表", this);
    sideTitle->setObjectName("spinLabel");
    sideTitle->setAlignment(Qt::AlignCenter);
    sideLay->addWidget(sideTitle);

    m_fileList = new QListWidget(this);
    m_fileList->setObjectName("cityList");
    sideLay->addWidget(m_fileList, 1);
    mainLayout->addWidget(sidebar);

    // ═══ RIGHT: player ═══
    auto *playerArea = new QFrame(this);
    playerArea->setObjectName("musicPlayerArea");
    auto *pLayout = new QVBoxLayout(playerArea);
    pLayout->setContentsMargins(20, 12, 20, 12);
    pLayout->setSpacing(8);

    // ---- Volume at top ----
    auto *volRow = new QHBoxLayout();
    auto *volIcon = new QLabel(this);
    volIcon->setObjectName("volIcon");
    volIcon->setPixmap(QPixmap(":/icons/volume.svg").scaled(16, 16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setObjectName("volSlider");
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setValue(m_player->volume());
    m_volumeSlider->setMaximumWidth(130);
    m_volumeSlider->setMinimumHeight(20);
    auto *volVal = new QLabel(QString::number(m_player->volume()), this);
    volVal->setObjectName("volLabel");

    volRow->addStretch();
    volRow->addWidget(volIcon);
    volRow->addWidget(m_volumeSlider);
    volRow->addWidget(volVal);
    volRow->addStretch();
    pLayout->addLayout(volRow);

    pLayout->addSpacing(6);

    // ---- Album art ----
    m_albumArt = new QLabel(this);
    m_albumArt->setObjectName("albumArt");
    m_albumArt->setAlignment(Qt::AlignCenter);
    m_albumArt->setFixedSize(90, 90);
    m_albumArt->setPixmap(QPixmap(":/icons/album.svg").scaled(72, 72, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    auto *artRow = new QHBoxLayout();
    artRow->addStretch();
    artRow->addWidget(m_albumArt);
    artRow->addStretch();
    pLayout->addLayout(artRow);

    // ---- Song title ----
    m_titleLabel = new QLabel("未播放", this);
    m_titleLabel->setObjectName("musicTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setWordWrap(true);
    pLayout->addWidget(m_titleLabel);

    // ---- Time ----
    m_timeLabel = new QLabel("--:-- / --:--", this);
    m_timeLabel->setObjectName("musicTime");
    m_timeLabel->setAlignment(Qt::AlignCenter);
    pLayout->addWidget(m_timeLabel);

    // ---- Controls: prev play stop next + mode ----
    auto *controls = new QHBoxLayout();
    controls->setSpacing(6);

    m_prevBtn = new QPushButton("⏮", this);
    m_prevBtn->setObjectName("ctrlBtn");
    m_prevBtn->setMinimumSize(42, 42);

    m_playBtn = new QPushButton("▶", this);
    m_playBtn->setObjectName("playBtn");
    m_playBtn->setMinimumSize(52, 52);

    m_stopBtn = new QPushButton("⏹", this);
    m_stopBtn->setObjectName("ctrlBtn");
    m_stopBtn->setMinimumSize(42, 42);

    m_nextBtn = new QPushButton("⏭", this);
    m_nextBtn->setObjectName("ctrlBtn");
    m_nextBtn->setMinimumSize(42, 42);

    controls->addStretch();
    controls->addWidget(m_prevBtn);
    controls->addWidget(m_playBtn);
    controls->addWidget(m_stopBtn);
    controls->addWidget(m_nextBtn);
    controls->addStretch();
    pLayout->addLayout(controls);

    // ---- Mode button ----
    m_modeBtn = new QPushButton("顺序播放", this);
    m_modeBtn->setObjectName("modeBtn");
    m_modeBtn->setMinimumHeight(30);
    m_modeBtn->setCursor(Qt::PointingHandCursor);

    auto *modeRow = new QHBoxLayout();
    modeRow->addStretch();
    modeRow->addWidget(m_modeBtn);
    modeRow->addStretch();
    pLayout->addLayout(modeRow);

    // ---- Seek bar at bottom ----
    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setObjectName("seekSlider");
    m_seekSlider->setRange(0, 0);
    m_seekSlider->setMinimumHeight(26);
    pLayout->addWidget(m_seekSlider);

    pLayout->addStretch();

    mainLayout->addWidget(playerArea, 1);

    // ---- Connections ----
    connect(m_playBtn, &QPushButton::clicked, this, &MusicPage::onPlayPause);
    connect(m_stopBtn,  &QPushButton::clicked, this, &MusicPage::onStop);
    connect(m_nextBtn,  &QPushButton::clicked, this, &MusicPage::onNext);
    connect(m_prevBtn,  &QPushButton::clicked, this, &MusicPage::onPrev);
    connect(m_modeBtn,  &QPushButton::clicked, this, &MusicPage::onModeToggle);
    connect(m_fileList, &QListWidget::itemClicked, this, &MusicPage::onFileClicked);

    connect(m_volumeSlider, &QSlider::valueChanged, this, [this, volVal](int v) {
        m_player->setVolume(v);
        volVal->setText(QString::number(v));
    });
    connect(m_seekSlider, &QSlider::sliderPressed,  this, &MusicPage::onSeekPressed);
    connect(m_seekSlider, &QSlider::sliderReleased, this, &MusicPage::onSeekReleased);

    connect(m_player, &MusicPlayer::stateChanged, this, &MusicPage::onStateChanged);
    connect(m_player, &MusicPlayer::positionChanged, this, &MusicPage::onPositionChanged);
    connect(m_player, &MusicPlayer::durationChanged, this, &MusicPage::onDurationChanged);
    connect(m_player, &MusicPlayer::currentFileChanged, this, &MusicPage::onCurrentFileChanged);
}

void MusicPage::showEvent(QShowEvent *event) { QWidget::showEvent(event); refreshFileList(); }

void MusicPage::refreshFileList()
{
    m_fileList->clear();
    QDir dir("media/music");
    if (!dir.exists()) return;
    for (const QFileInfo &fi : dir.entryInfoList(
             QStringList() << "*.mp3" << "*.wav" << "*.flac" << "*.ogg" << "*.m4a",
             QDir::Files, QDir::Name)) {
        auto *item = new QListWidgetItem(fi.fileName());
        item->setData(Qt::UserRole, fi.absoluteFilePath());
        m_fileList->addItem(item);
    }
}

void MusicPage::onFileClicked(QListWidgetItem *item)
{
    QString path = item->data(Qt::UserRole).toString();
    QStringList list;
    for (int i = 0; i < m_fileList->count(); i++)
        list.append(m_fileList->item(i)->data(Qt::UserRole).toString());
    m_player->setPlaylist(list);
    m_player->play(path);
}

void MusicPage::onPlayPause()
{
    switch (m_player->state()) {
    case MusicPlayer::Playing: m_player->pause(); break;
    case MusicPlayer::Paused:  m_player->resume(); break;
    default: break;
    }
}
void MusicPage::onStop() { m_player->stop(); }
void MusicPage::onNext() { m_player->next(); }
void MusicPage::onPrev() { m_player->prev(); }

void MusicPage::onModeToggle()
{
    switch (m_player->playMode()) {
    case MusicPlayer::Sequential: m_player->setPlayMode(MusicPlayer::SingleLoop); m_modeBtn->setText("单曲循环"); break;
    case MusicPlayer::SingleLoop:  m_player->setPlayMode(MusicPlayer::Shuffle);    m_modeBtn->setText("随机播放"); break;
    case MusicPlayer::Shuffle:     m_player->setPlayMode(MusicPlayer::Sequential); m_modeBtn->setText("顺序播放"); break;
    }
}

void MusicPage::onSeekPressed()  { m_seeking = true; }
void MusicPage::onSeekReleased() { m_seeking = false; m_player->seekTo(m_seekSlider->value()); }

void MusicPage::onStateChanged(int state) { m_playBtn->setText(state == MusicPlayer::Playing ? "⏸" : "▶"); }

void MusicPage::onPositionChanged(qint64 posMs)
{
    if (!m_seeking) m_seekSlider->setValue(static_cast<int>(posMs));
    int t = m_seekSlider->maximum() / 1000, c = static_cast<int>(posMs / 1000);
    m_timeLabel->setText(QString("%1:%2 / %3:%4")
        .arg(c/60,2,10,QChar('0')).arg(c%60,2,10,QChar('0'))
        .arg(t/60,2,10,QChar('0')).arg(t%60,2,10,QChar('0')));
}

void MusicPage::onDurationChanged(qint64 d) { m_seekSlider->setRange(0, static_cast<int>(d)); }
void MusicPage::onCurrentFileChanged(const QString &p) { m_titleLabel->setText(p.section('/', -1)); }
