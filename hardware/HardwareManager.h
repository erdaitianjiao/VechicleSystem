#ifndef HARDWAREMANAGER_H
#define HARDWAREMANAGER_H

#include "IHardwareDevice.h"
#include <QMap>
#include <QString>
#include <utility>

class HardwareManager {
public:
    static HardwareManager &instance();

    void initAll();

    template<typename T, typename... Args>
    T *createDevice(Args &&...args) {
        T *device = new T(std::forward<Args>(args)...);
        m_devices.insert(device->name(), device);
        return device;
    }

    IHardwareDevice *getDevice(const QString &name);

    template<typename T>
    T *getDevice(const QString &name) {
        return dynamic_cast<T *>(getDevice(name));
    }

    HardwareManager(const HardwareManager &) = delete;
    HardwareManager &operator=(const HardwareManager &) = delete;

private:
    HardwareManager() = default;
    ~HardwareManager();
    QMap<QString, IHardwareDevice *> m_devices;
};

#endif // HARDWAREMANAGER_H
