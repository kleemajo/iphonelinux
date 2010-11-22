#include "openiboot.h"
#include "nor.h"
#include "hardware/nor.h"
#include "util.h"
#include "timer.h"

#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
#include "spi.h"
#include "gpio.h"
#include "hardware/spi.h"
#endif

static int NORSectorSize = 4096;

static int Prepared = 0;

#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
static int WritePrepared = FALSE;
#endif

static int NORVendor;

static void nor_prepare() {
#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
	if(Prepared == 0) {
		spi_set_baud(0, 12000000, SPIWordSize8, 1, 0, 0);
	}
#endif
	Prepared++;
}

static void nor_unprepare() {
	Prepared--;
}

static NorInfo* probeNOR() {
	nor_prepare();
#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
	uint8_t command = NOR_SPI_JEDECID;
	uint8_t deviceID[3];
	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(NOR_SPI_PORT, &command, 1, TRUE, 0);
	spi_rx(NOR_SPI_PORT, deviceID, 3, TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);
	uint16_t vendor = deviceID[0];
	uint16_t device = deviceID[2];

	// Unprotect NOR
	command = NOR_SPI_EWSR;
	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(NOR_SPI_PORT, &command, 1, TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);

	uint8_t wrsrCommand[2] = {NOR_SPI_WRSR, 0};
	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(NOR_SPI_PORT, wrsrCommand, 2, TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);
#else
	SET_REG16(NOR + COMMAND, COMMAND_UNLOCK);
	SET_REG16(NOR + LOCK, LOCK_UNLOCK);

	SET_REG16(NOR + COMMAND, COMMAND_IDENTIFY);
	GET_REG16(NOR);
	GET_REG16(NOR);
	uint16_t vendor = GET_REG16(NOR + VENDOR);
	uint16_t device = GET_REG16(NOR + DEVICE);

	SET_REG16(NOR + COMMAND, COMMAND_LOCK);
	SET_REG16(NOR, DATA_MODE);
	GET_REG16(NOR);
	GET_REG16(NOR);
#endif

	NORVendor = vendor;

	bufferPrintf("NOR vendor=%x, device=%x\r\n", (uint32_t) vendor, (uint32_t) device);

	nor_unprepare();

	return NULL;
}

#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
/*TODO remove from .h static*/ uint8_t nor_get_status() {
	nor_prepare();
	uint8_t data;

	uint8_t command[1];
	command[0] = NOR_SPI_RDSR;

	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(NOR_SPI_PORT, command, sizeof(command), TRUE, 0);
	spi_rx(NOR_SPI_PORT, &data, sizeof(data), TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);

	nor_unprepare();

	return data;
}

static int nor_wait_for_ready(int timeout) {
	if((nor_get_status() & (1 << NOR_SPI_SR_BUSY)) == 0) {
		return 0;
	}

	uint64_t startTime = timer_get_system_microtime();
	while((nor_get_status() & (1 << NOR_SPI_SR_BUSY)) != 0) {
		if(has_elapsed(startTime, timeout * 1000)) {
			bufferPrintf("nor: timed out waiting for ready\r\n");
			return -1;
		}
	}

	return 0;
}

static void nor_write_enable() {
	nor_prepare();

	if(nor_wait_for_ready(100) != 0) {
		nor_unprepare();
		return;
	}

	uint8_t command[1];
	command[0] = NOR_SPI_WREN;

	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(NOR_SPI_PORT, command, sizeof(command), TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);

	nor_unprepare();
}

static void nor_write_disable() {
	nor_prepare();
bufferPrintf("1\r\n");
	if(nor_wait_for_ready(100) != 0) {
		nor_unprepare();
		return;
	}
bufferPrintf("2\r\n");
	uint8_t command[1];
	command[0] = NOR_SPI_WRDI;
bufferPrintf("3\r\n");
	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(0, command, sizeof(command), TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);
bufferPrintf("4\r\n");
	nor_unprepare();
bufferPrintf("5\r\n");
	WritePrepared = FALSE;
}

static int nor_serial_prepare_write(uint32_t offset, uint16_t data) {
	nor_prepare();

	if(nor_wait_for_ready(100) != 0) {
		nor_unprepare();
		return -1;
	}

	nor_write_enable();

	uint8_t command[6];
	command[0] = NOR_SPI_AIPG;
	command[1] = (offset >> 16) & 0xFF;
	command[2] = (offset >> 8) & 0xFF;
	command[3] = offset & 0xFF;
	command[4] = data & 0xFF;
	command[5] = (data >> 8) & 0xFF;

	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(0, command, sizeof(command), TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);

	nor_unprepare();

	WritePrepared = TRUE;
	
	return 0;
}

#endif

#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)

static int nor_serial_write_byte(uint32_t offset, uint8_t data) {
	nor_prepare();

	if(nor_wait_for_ready(100) != 0) {
		nor_unprepare();
		return -1;
	}

	nor_write_enable();

	uint8_t command[5];
	command[0] = NOR_SPI_PRGM;
	command[1] = (offset >> 16) & 0xFF;
	command[2] = (offset >> 8) & 0xFF;
	command[3] = offset & 0xFF;
	command[4] = data & 0xFF;

	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(0, command, sizeof(command), TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);

	nor_unprepare();

	return 0;
}

#endif

int nor_write_word(uint32_t offset, uint16_t data) {
	nor_prepare();
	bufferPrintf("ka\r\n");
#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
	if(NORVendor == 0xBF)
	{
		// SST has the auto-increment program opcode that lets programming happen more quickly
		if(!WritePrepared) {
			nor_serial_prepare_write(offset, data);
		} else {
			if(nor_wait_for_ready(100) != 0) {
				nor_unprepare();
				return -1;
			}

			uint8_t command[3];
			command[0] = NOR_SPI_AIPG;
			command[1] = data & 0xFF;
			command[2] = (data >> 8) & 0xFF;

			gpio_pin_output(NOR_SPI_CS0, 0);
			spi_tx(0, command, sizeof(command), TRUE, 0);
			gpio_pin_output(NOR_SPI_CS0, 1);
		}
	} else
	{
		nor_serial_write_byte(offset, data & 0xFF);
		nor_serial_write_byte(offset + 1, (data >> 8) & 0xFF);
	}
#else
	SET_REG16(NOR + COMMAND, COMMAND_UNLOCK);
	SET_REG16(NOR + LOCK, LOCK_UNLOCK);

	SET_REG16(NOR + COMMAND, COMMAND_WRITE);
	SET_REG16(NOR + offset, data);

	while(TRUE) {
		udelay(40);
		if(GET_REG16(NOR + offset) != data) {
			bufferPrintf("failed to write to NOR\r\n");
		} else {
			break;
		}
	}
#endif

	nor_unprepare();

	return 0;
}

uint16_t nor_read_word(uint32_t offset) {
	uint16_t data;
	nor_prepare();

#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
	uint8_t command[4];
	command[0] = NOR_SPI_READ;
	command[1] = (offset >> 16) & 0xFF;
	command[2] = (offset >> 8) & 0xFF;
	command[3] = offset & 0xFF;

	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(0, command, sizeof(command), TRUE, 0);
	spi_rx(0, (uint8_t*) &data, sizeof(data), TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);

#else
	data = GET_REG16(NOR + offset);
#endif
	nor_unprepare();

	return data;
}

int nor_erase_sector(uint32_t offset) {
	nor_prepare();
bufferPrintf("a\r\n");
#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
	if(nor_wait_for_ready(100) != 0) {
		nor_unprepare();
		return -1;
	}
bufferPrintf("b\r\n");
	nor_write_enable();
bufferPrintf("c\r\n");
	uint8_t command[4];
	command[0] = NOR_SPI_ERSE_4KB;
	command[1] = (offset >> 16) & 0xFF;
	command[2] = (offset >> 8) & 0xFF;
	command[3] = offset & 0xFF;
bufferPrintf("d\r\n");
	gpio_pin_output(NOR_SPI_CS0, 0);
	spi_tx(NOR_SPI_PORT, command, sizeof(command), TRUE, 0);
	gpio_pin_output(NOR_SPI_CS0, 1);
bufferPrintf("e\r\n");
	nor_write_disable();
#else
	SET_REG16(NOR + COMMAND, COMMAND_UNLOCK);
	SET_REG16(NOR + LOCK, LOCK_UNLOCK);

	SET_REG16(NOR + COMMAND, COMMAND_ERASE);

	SET_REG16(NOR + COMMAND, COMMAND_UNLOCK);
	SET_REG16(NOR + LOCK, LOCK_UNLOCK);

	SET_REG16(NOR + offset, ERASE_DATA);

	while(TRUE) {
		udelay(50000);
		if(GET_REG16(NOR + offset) != 0xFFFF) {
			bufferPrintf("failed to erase NOR sector: %x %x\r\n", (unsigned int) GET_REG16(NOR + offset), GET_REG(0x20000000));
		} else {
			break;
		}
	}
#endif
bufferPrintf("f\r\n");
	nor_unprepare();
bufferPrintf("g\r\n");
	return 0;
}

void nor_read(void* buffer, int offset, int len) {
	nor_prepare();
#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
	if(nor_wait_for_ready(100) != 0) {
		nor_unprepare();
		return;
	}

	uint8_t command[4];
	uint8_t* data = buffer;
	while(len > 0) {
		int toRead = (len > 0x10) ? 0x10 : len;

		command[0] = NOR_SPI_READ;
		command[1] = (offset >> 16) & 0xFF;
		command[2] = (offset >> 8) & 0xFF;
		command[3] = offset & 0xFF;

		gpio_pin_output(NOR_SPI_CS0, 0);
		spi_tx(NOR_SPI_PORT, command, sizeof(command), TRUE, 0);
		if(spi_rx(NOR_SPI_PORT, data, toRead, TRUE, 0) < 0)
		{
			gpio_pin_output(NOR_SPI_CS0, 1);
			continue;
		}
		gpio_pin_output(NOR_SPI_CS0, 1);

		len -= toRead;
		data += toRead;
		offset += toRead;
	}
#else
	uint16_t* alignedBuffer = (uint16_t*) buffer;
	for(; len >= 2; len -= 2) {
		*alignedBuffer = nor_read_word(offset);
		offset += 2;
		alignedBuffer++;
	}

	if(len > 0) {
		uint16_t lastWord = nor_read_word(offset);
		uint8_t* unalignedBuffer = (uint8_t*) alignedBuffer;
		*unalignedBuffer = *((uint8_t*)(&lastWord));
	}
#endif

	nor_unprepare();
}

int nor_write(void* buffer, int offset, int len) {
bufferPrintf("1\r\n");
	nor_prepare();
bufferPrintf("2\r\n");
	int startSector = offset / NORSectorSize;
	int endSector = (offset + len) / NORSectorSize;
bufferPrintf("3\r\n");
	int numSectors = endSector - startSector + 1;
	uint8_t* sectorsToChange = (uint8_t*) malloc(NORSectorSize * numSectors);
	nor_read(sectorsToChange, startSector * NORSectorSize, NORSectorSize * numSectors);
bufferPrintf("4\r\n");
	int offsetFromStart = offset - (startSector * NORSectorSize);
bufferPrintf("5\r\n");
	memcpy(sectorsToChange + offsetFromStart, buffer, len);
bufferPrintf("6\r\n");
	int i;
	for(i = 0; i < numSectors; i++) {
		if(nor_erase_sector((i + startSector) * NORSectorSize) != 0) {
bufferPrintf("6a\r\n");
			nor_unprepare();
			return -1;
		}
bufferPrintf("7\r\n");
		int j;
		uint16_t* curSector = (uint16_t*)(sectorsToChange + (i * NORSectorSize));
		for(j = 0; j < (NORSectorSize / 2); j++) {
			bufferPrintf("zz\r\n");
			if(nor_write_word(((i + startSector) * NORSectorSize) + (j * 2), curSector[j]) != 0) {
				bufferPrintf("yy\r\n");
				nor_unprepare();
#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
				nor_write_disable();
#endif
				return -1;
			}
		}
		bufferPrintf("xx\r\n");
#if defined(CONFIG_3G) || defined(CONFIG_IPOD2G)
		nor_write_disable();
#endif
	}

	free(sectorsToChange);

	nor_unprepare();

	return 0;
}

int getNORSectorSize() {
	return NORSectorSize;
}

int nor_setup() {
	probeNOR();

	return 0;
}

