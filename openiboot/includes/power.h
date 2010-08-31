#ifndef POWER_H
#define POWER_H

#include "openiboot.h"

int power_setup();
#ifndef CONFIG_IPOD2G
int power_ctrl(uint32_t device, OnOff on_off);
#endif

#endif
