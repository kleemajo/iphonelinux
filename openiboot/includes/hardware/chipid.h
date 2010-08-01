#ifndef HW_CHIPID_H
#define HW_CHIPID_H

#include "hardware/s5l8900.h"

// Device
#ifdef CONFIG_IPOD2G
#define CHIPID 0x3D100000
#else
#define CHIPID 0x3E500000
#endif


// Registers
#define SPICLOCKTYPE 0x4
#ifdef CONFIG_IPOD2G
#define CHIPINFO 0x8
#endif

// Values
#define GET_SPICLOCKTYPE(x) GET_BITS(x, 24, 4)

#ifdef CONFIG_IPOD2G
#define CHIPID_GET_SECURITY_EPOCH(x) (GET_BITS((x), 0, 1))
#define CHIPID_GET_POWER_EPOCH(x) (GET_BITS((x), 4, 7) > 3 ? 3 : 4)
#endif


#endif
