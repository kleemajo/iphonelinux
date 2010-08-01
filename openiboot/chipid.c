#include "openiboot.h"
#include "chipid.h"
#include "hardware/chipid.h"

int chipid_spi_clocktype() {
	return GET_SPICLOCKTYPE(GET_REG(CHIPID + SPICLOCKTYPE));
}

#ifdef CONFIG_IPOD2G
unsigned int chipid_get_security_epoch() {
	return CHIPID_GET_SECURITY_EPOCH(GET_REG(CHIPID + CHIPINFO));
}
#endif


