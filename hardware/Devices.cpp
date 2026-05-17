#include "Devices.h"
#include <cstdlib>

LightDevice::LightDevice()
    : SysfsDevice("/sys/devices/platform/leds/leds/sys-led/brightness", "light")
{
#if __arm__
    system("echo none > /sys/class/leds/sys-led/trigger");
#endif
}

BeepDevice::BeepDevice()
    : SysfsDevice("/sys/devices/platform/leds/leds/beep/brightness", "beep")
{
}
