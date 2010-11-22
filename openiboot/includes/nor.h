#ifndef NOR_H
#define NOR_H

#include "openiboot.h"

int nor_setup();

#if defined(CONFIG_3G) || defined (CONFIG_IPOD2G)
typedef struct NorChipInfo {
	uint8_t vendor;
	uint8_t device;
	Boolean aaiEnabled;
	uint32_t eraseBlockSize;
	uint32_t unkn5;
	uint32_t blockProtectBits;
	uint32_t bytesPerWrite;
	uint32_t unkn2;
	uint32_t writeTimeout;
	uint32_t unkn4;
} NorChipInfo;
#endif

int nor_write_word(uint32_t offset, uint16_t data);
uint16_t nor_read_word(uint32_t offset);
int nor_erase_sector(uint32_t offset);

void nor_read(void* buffer, int offset, int len);
int nor_write(void* buffer, int offset, int len);

int getNORSectorSize();

#endif
