#ifndef CAMERAPAGE_H
#define CAMERAPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>

class CameraDevice;

class CameraPage : public QWidget {
    Q_OBJECT
public:
    explicit CameraPage(QWidget *parent = nullptr);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;

private slots:
    void onCapturePhoto();
    void onRecord();
    void onStop();
    void onFrameReady(const QImage &frame);

private:
    QLabel *m_videoLabel;
    QPushButton *m_photoBtn;
    QPushButton *m_recordBtn;
    QPushButton *m_stopBtn;
    bool m_recording = false;

    CameraDevice *getCamera();
    void startPreview();
    void stopPreview();
};

#endif // CAMERAPAGE_H
