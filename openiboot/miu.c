#include "openiboot.h"
#include "miu.h"
#include "clock.h"
#include "hardware/power.h"
#include "util.h"
#ifdef CONFIG_IPOD2G
#include "chipid.h"
#endif

int miu_setup() {
	#ifdef CONFIG_IPOD2G
	if(POWER_ID_EPOCH(GET_REG(POWER + POWER_ID)) != chipid_get_power_epoch()) {
	#else
	if(POWER_ID_EPOCH(*((uint8_t*)(POWER + POWER_ID))) != 3) {
	#endif	
		// Epoch mismatch
		bufferPrintf("miu: epoch mismatch\r\n");
		return -1;
	}
#ifndef CONFIG_IPOD2G
	clock_set_bottom_bits_38100000(1);
#endif
	return 0;
}

