#ifndef HW_SPI_H
#define HW_SPI_H

#include "hardware/s5l8900.h"

// Device
#define SPI0 0x3C300000
#define SPI1 0x3CE00000
#define SPI2 0x3D200000
#ifdef CONFIG_IPOD2G
#define SPI3 0x3DA00000
#define SPI4 0x3E100000
#endif

// Registers

#define CONTROL 0x0
#define SETUP 0x4
#define STATUS 0x8
#define UNKREG1 0xC
#define TXDATA 0x10
#define RXDATA 0x20
#define CLKDIVIDER 0x30
#define SPCNT 0x34
#define SPIDD 0x38

// Values
#define MAX_TX_BUFFER 8
#define TX_BUFFER_LEFT(x) GET_BITS(status, 4, 4)
#define RX_BUFFER_LEFT(x) GET_BITS(status, 8, 4)

#define CLOCK_SHIFT 12
#define MAX_DIVIDER 0x3FF

#ifdef CONFIG_IPOD2G
#define SPI0_CLOCKGATE 0xE
#define SPI1_CLOCKGATE 0xF
#define SPI2_CLOCKGATE 0x10
#define SPI3_CLOCKGATE 0x11
#define SPI4_CLOCKGATE 0x12
#else
#define SPI0_CLOCKGATE 0x22
#define SPI1_CLOCKGATE 0x2B
#define SPI2_CLOCKGATE 0x2F
#endif

// GPIO interrupts
#define SPI0_IRQ 0x9
#define SPI1_IRQ 0xA
#define SPI2_IRQ 0xB
#ifdef CONFIG_IPOD2G
#define SPI3_IRQ 0x1C
#define SPI4_IRQ 0x37
#endif

// GPIO pins
#ifdef CONFIG_IPHONE
#define GPIO_SPI0_CS0 0x400
#define GPIO_SPI1_CS0 0x1800
#define GPIO_SPI2_CS0 0x705
#endif

#ifdef CONFIG_IPOD
#define GPIO_SPI0_CS0 0x700
#define GPIO_SPI1_CS0 0x1800
#define GPIO_SPI2_CS0 0x1804
#define GPIO_SPI2_CS1 0x705
#endif

#ifdef CONFIG_3G
#define GPIO_SPI0_CS0 0x400
#define GPIO_SPI0_CS1 0x705
#define GPIO_SPI0_CS2 0x706
#define GPIO_SPI1_CS0 0x1800
#endif

#ifdef CONFIG_IPOD2G
#define GPIO_SPI0_CS0 0x0
#define GPIO_SPI1_CS0 0x406
#define GPIO_SPI2_CS0 -1 //TODO: remove me, needed to make code compile
#define GPIO_SPI4_CS0 0xA07
#endif

#ifdef CONFIG_IPOD2G
#define NUM_SPIPORTS 3
//TODO: switch this to 5 eventually (needed for multitouch which is on SPI4!). This is left at 3 for now for debugging purposes).
#else
#define NUM_SPIPORTS 3
#endif

#endif

