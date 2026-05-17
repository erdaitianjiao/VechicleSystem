#ifndef CAMERADEVICE_H
#define CAMERADEVICE_H

#include "IHardwareDevice.h"
#include <QObject>
#include <QImage>
#include <QString>
#include <memory>

class CameraDevice : public QObject, public IHardwareDevice {
    Q_OBJECT

public:
    explicit CameraDevice(const QString &videoDevice = "/dev/video0",
                          QObject *parent = nullptr);
    ~CameraDevice();

    // Preview control (auto called by CameraPage show/hide)
    int on() override;
    int off() override;
    int getStatus() override;
    QString name() const override { return "camera"; }

    // Photo: grab current frame
    int capturePhoto(const QString &filePath);

    // Recording: independent of preview, survives page switch
    int startRecording(const QString &filePath);
    int stopRecording();

    enum State { Stopped, Previewing, Recording };
    State state() const { return m_state; }
    bool isRecording() const { return m_recording; }

    // Called by appsink callback to cache frame for photo
    void setLastFrame(const QImage &img) { m_lastFrame = img; }

signals:
    void frameReady(const QImage &frame);
    void recordingStarted();
    void recordingStopped();
    void errorOccurred(const QString &message);

private:
    struct GstImpl;
    std::unique_ptr<GstImpl> m_impl;
    State m_state = Stopped;
    QString m_devicePath;
    QImage m_lastFrame;
    bool m_recording = false; // true while recording branch active

    void destroyPipeline();
    bool buildPipeline();
    bool linkRecordBin(const QString &filePath);
    void removeRecordBin();
};

#endif // CAMERADEVICE_H
