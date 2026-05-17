#ifndef SYSFSDEVICE_H
#define SYSFSDEVICE_H

#include "IHardwareDevice.h"
#include <QFile>
#include <QTextStream>

class SysfsDevice : public IHardwareDevice {
public:
    explicit SysfsDevice(const QString &sysfsPath, const QString &deviceName);
    ~SysfsDevice();

    int on() override final;
    int off() override final;
    int getStatus() override final;
    QString name() const override final { return m_name; }

private:
    QFile m_file;
    QTextStream *m_filein = nullptr;
    QString m_name;
};

#endif // SYSFSDEVICE_H
