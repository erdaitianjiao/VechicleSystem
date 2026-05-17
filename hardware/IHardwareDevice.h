#ifndef IHARDWAREDEVICE_H
#define IHARDWAREDEVICE_H

#include <QString>

class IHardwareDevice {
public:
    virtual ~IHardwareDevice() = default;
    virtual int on() = 0;
    virtual int off() = 0;
    virtual int getStatus() = 0;
    virtual QString name() const = 0;

protected:
    int m_status = -1;
};

#endif // IHARDWAREDEVICE_H
