#ifndef DEVICES_H
#define DEVICES_H

#include "SysfsDevice.h"

class LightDevice : public SysfsDevice {
public:
    LightDevice();
};

class BeepDevice : public SysfsDevice {
public:
    BeepDevice();
};

#endif // DEVICES_H
