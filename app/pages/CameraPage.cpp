#include "CameraPage.h"
#include "CameraDevice.h"
#include "HardwareManager.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPixmap>
#include <QDateTime>
#include <QDir>
#include <QShowEvent>
#include <QHideEvent>
#include <QStyle>
#include <QVariant>

CameraPage::CameraPage(QWidget *parent)
    : QWidget(parent)
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Preview area
    m_videoLabel = new QLabel(this);
    m_videoLabel->setObjectName("videoPreview");
    m_videoLabel->setAlignment(Qt::AlignCenter);
    m_videoLabel->setText("等待摄像头...");
    m_videoLabel->setMinimumSize(320, 240);
    layout->addWidget(m_videoLabel, 1);

    // Controls (no preview button — auto start)
    auto *controlBar = new QFrame(this);
    controlBar->setObjectName("camControlBar");
    auto *controls = new QHBoxLayout(controlBar);
    controls->setContentsMargins(12, 12, 12, 12);
    controls->setSpacing(12);

    auto makeBtn = [this](const QString &text, const QString &objName) {
        auto *btn = new QPushButton(text, this);
        btn->setObjectName(objName);
        btn->setMinimumSize(80, 52);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    m_photoBtn = makeBtn("拍照", "camBtn");
    m_recordBtn = makeBtn("录像", "camBtn");
    m_stopBtn = makeBtn("停止", "camStopBtn");

    controls->addStretch();
    controls->addWidget(m_photoBtn);
    controls->addWidget(m_recordBtn);
    controls->addWidget(m_stopBtn);
    controls->addStretch();
    layout->addWidget(controlBar);

    connect(m_photoBtn, &QPushButton::clicked, this, &CameraPage::onCapturePhoto);
    connect(m_recordBtn, &QPushButton::clicked, this, &CameraPage::onRecord);
    connect(m_stopBtn,  &QPushButton::clicked, this, &CameraPage::onStop);
}

CameraDevice *CameraPage::getCamera()
{
    auto *cam = HardwareManager::instance().getDevice<CameraDevice>("camera");
    if (!cam)
        cam = HardwareManager::instance().createDevice<CameraDevice>("/dev/video0");
    return cam;
}

void CameraPage::startPreview()
{
    auto *cam = getCamera();
    if (!cam) return;
    connect(cam, &CameraDevice::frameReady, this, &CameraPage::onFrameReady, Qt::UniqueConnection);
    cam->on();
}

void CameraPage::stopPreview()
{
    auto *cam = HardwareManager::instance().getDevice<CameraDevice>("camera");
    if (!cam) return;
    if (cam->state() != CameraDevice::Recording) {
        disconnect(cam, &CameraDevice::frameReady, this, &CameraPage::onFrameReady);
        cam->off();
    }
    // If recording, keep pipeline alive — just disconnect frame signal
    disconnect(cam, &CameraDevice::frameReady, this, &CameraPage::onFrameReady);
}

void CameraPage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    qDebug() << "[CameraPage] shown — starting preview";
    startPreview();
}

void CameraPage::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    qDebug() << "[CameraPage] hidden — stopping preview";
    stopPreview();
}

void CameraPage::onCapturePhoto()
{
    auto *cam = HardwareManager::instance().getDevice<CameraDevice>("camera");
    if (!cam) return;
    QDir().mkpath("record/pic");
    QString path = QString("record/pic/photo_%1.jpg")
        .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
    cam->capturePhoto(path);
    qDebug() << "[CameraPage] 拍照:" << path;
}

void CameraPage::onRecord()
{
    auto *cam = HardwareManager::instance().getDevice<CameraDevice>("camera");
    if (!cam) return;
    if (m_recording) {
        cam->stopRecording();
        m_recordBtn->setText("录像");
        m_recordBtn->setProperty("recording", QVariant(false));
        m_recordBtn->style()->unpolish(m_recordBtn);
        m_recordBtn->style()->polish(m_recordBtn);
        m_recording = false;
    } else {
        QDir().mkpath("record/video");
        QString path = QString("record/video/video_%1.mpg")
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"));
        cam->startRecording(path);
        m_recordBtn->setText("停止录像");
        m_recordBtn->setProperty("recording", QVariant(true));
        m_recordBtn->style()->unpolish(m_recordBtn);
        m_recordBtn->style()->polish(m_recordBtn);
        m_recording = true;
        qDebug() << "[CameraPage] 录像开始:" << path;
    }
}

void CameraPage::onStop()
{
    auto *cam = HardwareManager::instance().getDevice<CameraDevice>("camera");
    if (!cam) return;
    cam->off();
    m_recording = false;
    m_recordBtn->setText("录像");
    m_recordBtn->setProperty("recording", QVariant(false));
    m_recordBtn->style()->unpolish(m_recordBtn);
    m_recordBtn->style()->polish(m_recordBtn);
    m_videoLabel->setText("等待摄像头...");
    qDebug() << "[CameraPage] 停止";
}

void CameraPage::onFrameReady(const QImage &frame)
{
    m_videoLabel->setPixmap(QPixmap::fromImage(frame).scaled(
        m_videoLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}
