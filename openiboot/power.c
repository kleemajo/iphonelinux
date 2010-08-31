#include "openiboot.h"
#include "power.h"
#include "hardware/power.h"
#ifdef CONFIG_IPOD2G
#include "clock.h"
#endif

int power_setup() {
#ifdef CONFIG_IPOD2G
	SET_REG(POWER + POWER_CONFIG0, POWER_CONFIG0_RESET);
	SET_REG(POWER + POWER_CONFIG2, POWER_CONFIG2_RESET);
	SET_REG(POWER + POWER_CONFIG3, get_base_frequency() / POWER_CONFIG3_RESET_DIVIDER);
#else
		// Deprecated in 2.x
#if 0
	SET_REG(POWER + POWER_CONFIG0, POWER_CONFIG0_RESET);
	SET_REG(POWER + POWER_CONFIG1, POWER_CONFIG1_RESET);
	SET_REG(POWER + POWER_CONFIG2, POWER_CONFIG2_RESET);

	/* turn off everything */
	int toReset = POWER_DEFAULT_DEVICES | POWER_VROM;
	SET_REG(POWER + POWER_OFFCTRL, toReset);

	/* wait for the new state to take effect */
	while((GET_REG(POWER + POWER_SETSTATE) & toReset) != (GET_REG(POWER + POWER_STATE) & toReset));
#endif
#endif	
	return 0;
}

#ifndef CONFIG_IPOD2G
int power_ctrl(uint32_t device, OnOff on_off) {
	if(on_off == ON) {
		SET_REG(POWER + POWER_ONCTRL, device);
	} else {
		SET_REG(POWER + POWER_OFFCTRL, device);
	}

	/* wait for the new state to take effect */
	while((GET_REG(POWER + POWER_SETSTATE) & device) != (GET_REG(POWER + POWER_STATE) & device));

	return 0;
}
#endif
