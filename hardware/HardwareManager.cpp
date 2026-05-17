#include "HardwareManager.h"
#include "Devices.h"

HardwareManager &HardwareManager::instance()
{
    static HardwareManager manager;
    return manager;
}

HardwareManager::~HardwareManager()
{
    qDeleteAll(m_devices);
    m_devices.clear();
}

void HardwareManager::initAll()
{
    createDevice<LightDevice>();
    createDevice<BeepDevice>();
}

IHardwareDevice *HardwareManager::getDevice(const QString &name)
{
    return m_devices.value(name, nullptr);
}
