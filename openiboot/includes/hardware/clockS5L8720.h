#ifndef HW_CLOCKS5L8720_H
#define HW_CLOCKS5L8720_H

// Clock constants for the S5L8720. This device does not have a distinction
// between clock0 and clock1 like the S5L8900 does.

#include "hardware/s5l8900.h"

// Constants
#define NUM_PLL 3
#define CLOCK1_SECURITY_EPOCH_1_FREQUENCY 24000000
#define CLOCK1_SECURITY_EPOCH_0_FREQUENCY 12000000
#define CLOCK1_STATIC_INFREQ 27000000

// Devices
#define CLOCK1 0x3C500000

// Registers
#define CLOCK1_CONFIG0 0x0
#define CLOCK1_CONFIG1 0x4
#define CLOCK1_CONFIG2 0x8
#define CLOCK1_PLL0CON 0x20
#define CLOCK1_PLL1CON 0x24
#define CLOCK1_PLL2CON 0x28
#define CLOCK1_PLL0LCNT 0x30
#define CLOCK1_PLL1LCNT 0x34
#define CLOCK1_PLL2LCNT 0x38
#define CLOCK1_PLLLOCK 0x40
#define CLOCK1_PLLMODE 0x44
#define CLOCK1_GATES_0 0x48
#define CLOCK1_GATES_1 0x4C
#define CLOCK1_GATES_2 0x58
#define CLOCK1_GATES_3 0x68
#define CLOCK1_GATES_4 0x6C

// Values
// Config 1
#define CLOCK1_CLOCKPLL(x) (GET_BITS((x), 12, 2) - 1)
#define CLOCK1_CLOCKHASDIVIDER(x) GET_BITS((x), 30, 1)
#define CLOCK1_CLOCKDIVIDER(x) ((GET_BITS((x), 25, 5) + 1) << 1)

#define CLOCK1_BUSPLL(x) (GET_BITS((x), 12, 2) - 1)
#define CLOCK1_BUSHASDIVIDER(x) GET_BITS((x), 22, 1)
#define CLOCK1_BUSDIVIDER(x) ((GET_BITS((x), 17, 5) + 1) << 1)

#define CLOCK1_PERIPHERALPLL(x) (GET_BITS((x), 12, 2) - 1)
#define CLOCK1_PERIPHERALHASDIVIDER(x) GET_BITS((x), 14, 1)
#define CLOCK1_PERIPHERALDIVIDER(x) ((GET_BITS((x), 9, 5) + 1) << 1)

#define CLOCK1_MEMORYPLL(x) (GET_BITS((x), 12, 2) - 1)
#define CLOCK1_MEMORYHASDIVIDER(x) GET_BITS((x), 6, 1)
#define CLOCK1_MEMORYDIVIDER(x) ((GET_BITS((x), 1, 5) + 1) << 1)

// Config 2
#define CLOCK1_UNKNOWNPLL(x) (GET_BITS((x), 28, 2) - 1) 
#define CLOCK1_UNKNOWNDIVIDER(x) (GET_BITS((x), 16, 4) + 1)

#define CLOCK1_DISPLAYPLL(x) (GET_BITS((x), 12, 2) - 1)
#define CLOCK1_DISPLAYDIVIDER(x) ((GET_BITS((x), 4, 4) + 1) * (GET_BITS((x), 0, 4) + 1))

// PLL mode
#define CLOCK1_PLLMODE_ONOFF(x, y) (((x) >> (y)) & 0x1)
#define CLOCK1_PLLMODE_USE_SECURITY_EPOCH_INFREQ(x, y) (!(((x) >> (y + 4)) & 0x1))

#define CLOCK1_MDIV(x) (((x) >> 8) & 0xFF)
#define CLOCK1_PDIV(x) (((x) >> 24) & 0x3F)
#define CLOCK1_SDIV(x) ((x) & 0x7)

// Clock gates
#define NUM_CLOCK_GATE_REGISTERS 5
typedef struct {
	uint32_t id;
	uint32_t reg_0_bits;
	uint32_t reg_1_bits;
	uint32_t reg_2_bits;
	uint32_t reg_3_bits;
	uint32_t reg_4_bits;
} ClockGate;

// Base clock divisor
#define CLOCK_BASE_DIVISOR_MASK 0xf
#define CLOCK_BASE_DIVISOR_0 0x1
#define CLOCK_BASE_DIVISOR_1 0x5
#define CLOCK_BASE_DIVISOR_2 0xB
#define CLOCK_BASE_DIVISOR_3 0x4

#endif
