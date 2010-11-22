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
#define UNKREG4 0x3C
#define UNKREG5 0x40
#define UNKREG6 0x44
#define UNKREG7 0x48
#define TXBUFFERLEN 0x4C

// Setup register
#define SPISETUP_NO_TRANSMIT_JUNK           (1 << 0)    // 1 bit
#define SPISETUP_LAST_CLOCK_EDGE_MISSING    (1 << 1)    // 1 bit
#define SPISETUP_IS_ACTIVE_LOW              (1 << 2)    // 1 bit
#define SPISETUP_IS_MASTER                  (1 << 3)    // 2 bits
#define SPISETUP_OPTION5                    (1 << 5)    // 2 bits
#define SPISETUP_UNKN1                      (1 << 7)    // 1 bit
#define SPISETUP_UNKN2                      (1 << 8)    // 1 bit
#ifdef CONFIG_IPOD2G
#define SPISETUP_CLOCKSOURCE                (1 << 14)   // 1 bit
#define SPISETUP_OPTION13                   (1 << 15)   // 1 bit
#else
#define SPISETUP_CLOCKSOURCE                (1 << 12)   // 1 bit
#define SPISETUP_OPTION13                   (1 << 13)   // 1 bit
#endif
#define SPISETUP_UNKN3                      (1 << 21)   // 1 bit

// Values
#ifdef CONFIG_IPOD2G
#define MAX_TX_BUFFER 8		//TODO: this is 8 in some places (spi_tx), 16 in others (spi_irq_handler)... figure that out
#define TX_BUFFER_LEFT(x) GET_BITS(x, 6, 5)
#define RX_BUFFER_LEFT(x) GET_BITS(x, 11, 5)
#else
#define MAX_TX_BUFFER 8
#define TX_BUFFER_LEFT(x) GET_BITS(x, 4, 4)
#define RX_BUFFER_LEFT(x) GET_BITS(x, 8, 4)
#endif

#ifdef CONFIG_IPOD2G
#define CLOCK_SHIFT 14
#define OPTION13_SHIFT 15
#else
#define CLOCK_SHIFT 12
#define OPTION13_SHIFT 13
#endif
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
#define NUM_SPIPORTS 5
#else
#define NUM_SPIPORTS 3
#endif

#endif

