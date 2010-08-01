#include "openiboot.h"
#include "miu.h"
#include "clock.h"
#include "hardware/power.h"
#include "util.h"

int miu_setup() {
#ifndef CONFIG_IPOD2G
	if(POWER_ID_EPOCH(*((uint8_t*)(POWER + POWER_ID))) != 3) {
		// Epoch mismatch
		bufferPrintf("miu: epoch mismatch\r\n");
		return -1;
	}
	clock_set_bottom_bits_38100000(1);
#endif
	return 0;
}

