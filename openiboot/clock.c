#include "openiboot.h"
#include "clock.h"
#include "util.h"
#ifdef CONFIG_IPOD2G
#include "chipid.h"
#include "hardware/clockS5L8720.h"
#else
#include "hardware/clock0.h"
#include "hardware/clock1.h"
#endif

uint32_t ClockPLL;
uint32_t PLLFrequencies[NUM_PLL];

uint32_t ClockFrequency;
uint32_t MemoryFrequency;
uint32_t BusFrequency;
uint32_t PeripheralFrequency;
uint32_t UnknownFrequency;   
uint32_t DisplayFrequency;  
uint32_t FixedFrequency;
uint32_t TimebaseFrequency;
#ifdef CONFIG_IPOD2G
uint32_t UsbPhyFrequency;
#endif

#ifndef CONFIG_IPOD2G
uint32_t ClockSDiv;
#endif

uint32_t TicksPerSec;

#ifdef CONFIG_IPOD2G
int clock_setup() {			// S5L8720
	uint32_t config;

	config = GET_REG(CLOCK1 + CLOCK1_CONFIG0);
	uint32_t clockPLL = CLOCK1_CLOCKPLL(config);

	// Config1 divisors
	config = GET_REG(CLOCK1 + CLOCK1_CONFIG1);

	// Clock divisor
	uint32_t clockDivisor;
	if(CLOCK1_CLOCKHASDIVIDER(config)) {
		clockDivisor = CLOCK1_CLOCKDIVIDER(config);
	} else {
		clockDivisor = 1;
		bufferPrintf("PLL %d: off.\n", clockPLL);
	}
	
	// Bus divisor
	uint32_t busPLL = clockPLL;
	uint32_t busDivisor;
	if(CLOCK1_BUSHASDIVIDER(config)) {
		busDivisor = CLOCK1_BUSDIVIDER(config);
	} else {
		busDivisor = 1;
	}
	
	// Peripheral divisor
	uint32_t peripheralPLL = clockPLL;
	uint32_t peripheralDivisor;
	if(CLOCK1_PERIPHERALHASDIVIDER(config)) {
		peripheralDivisor = CLOCK1_PERIPHERALDIVIDER(config);
	} else {
		peripheralDivisor = 1;
	}
	
	// Memory divisor
	uint32_t memoryPLL = clockPLL;
	uint32_t memoryDivisor;
	if(CLOCK1_MEMORYHASDIVIDER(config)) {
		memoryDivisor = CLOCK1_MEMORYDIVIDER(config);
	} else {
		memoryDivisor = 1;
	}
	
	// Config2 divisors
	config = GET_REG(CLOCK1 + CLOCK1_CONFIG2);
	
	// Unknown divisor
	uint32_t unknownPLL = CLOCK1_UNKNOWNPLL(config);
	uint32_t unknownDivisor = CLOCK1_UNKNOWNDIVIDER(config);

	// Display divisor
	uint32_t displayPLL = CLOCK1_DISPLAYPLL(config);
	uint32_t displayDivisor = CLOCK1_DISPLAYDIVIDER(config);
	
	// Populate the PLLFrequencies table
	int pll = 0;
	for (pll = 0; pll < NUM_PLL; pll++) {
		uint32_t pllStates = GET_REG(CLOCK1 + CLOCK1_PLLMODE);
		if(CLOCK1_PLLMODE_ONOFF(pllStates, pll)) {
			uint32_t inputFrequency;
			uint32_t pllConReg;
			uint32_t pllCon;
			
			if (CLOCK1_PLLMODE_USE_SECURITY_EPOCH_INFREQ(pllStates, pll)) {
				inputFrequency = get_base_frequency();
			} else {
				inputFrequency = CLOCK1_STATIC_INFREQ;
			}
			
			if (pll == 0) {
				pllConReg = CLOCK1 + CLOCK1_PLL0CON;
			} else if (pll == 1) {
				pllConReg = CLOCK1 + CLOCK1_PLL1CON;
			} else if (pll == 2) {
				pllConReg = CLOCK1 + CLOCK1_PLL2CON;
			} else {
				PLLFrequencies[pll] = 0;
				continue;
			}

			pllCon = GET_REG(pllConReg);

			uint64_t afterMDiv = inputFrequency * CLOCK1_MDIV(pllCon);
			uint64_t afterPDiv = afterMDiv / CLOCK1_PDIV(pllCon);
			uint64_t afterSDiv;
			if(pll != clockPLL) {
				afterSDiv = afterPDiv / (((uint64_t)0x1) << (CLOCK1_SDIV(pllCon) + 1));
			} else {
				afterSDiv = afterPDiv;
			}

			PLLFrequencies[pll] = (uint32_t)afterSDiv;

			bufferPrintf("PLL %d: %d\n", pll, PLLFrequencies[pll]);
		} else {
			PLLFrequencies[pll] = 0;
			bufferPrintf("PLL %d: off.\n", pll);
			continue;
		}
	}

	ClockFrequency = PLLFrequencies[clockPLL] / clockDivisor;
	MemoryFrequency = PLLFrequencies[memoryPLL] / memoryDivisor;
	BusFrequency = PLLFrequencies[busPLL] / busDivisor;
	PeripheralFrequency = PLLFrequencies[peripheralPLL] / peripheralDivisor;
	UnknownFrequency = PLLFrequencies[unknownPLL] / unknownDivisor;
	DisplayFrequency = PLLFrequencies[displayPLL] / displayDivisor;
	if (chipid_get_security_epoch() == 1) {
		FixedFrequency = get_base_frequency();
	} else {
		FixedFrequency = 0;
	}
	TimebaseFrequency = FixedFrequency / 4;
	UsbPhyFrequency = get_base_frequency();

	TicksPerSec = FixedFrequency / 2;

	return 0;
}

#else
int clock_setup() {		// S5L8900
	uint32_t config;

	config = GET_REG(CLOCK1 + CLOCK1_CONFIG0);
	uint32_t clockPLL = CLOCK1_CLOCKPLL(config);
	uint32_t clockDivisor;

	if(CLOCK1_CLOCKHASDIVIDER(config)) {
		clockDivisor = CLOCK1_CLOCKDIVIDER(config);
	} else {
		clockDivisor = 1;
	}

	bufferPrintf("PLL %d: off.\n", clockPLL);

	uint32_t memoryPLL = CLOCK1_MEMORYPLL(config);
	uint32_t memoryDivisor;

	if(CLOCK1_MEMORYHASDIVIDER(config)) {
		memoryDivisor = CLOCK1_MEMORYDIVIDER(config);
	} else {
		memoryDivisor = 1;
	}


	config = GET_REG(CLOCK1 + CLOCK1_CONFIG1);
	uint32_t busPLL = CLOCK1_BUSPLL(config);
	uint32_t busDivisor;

	if(CLOCK1_BUSHASDIVIDER(config)) {
		busDivisor = CLOCK1_BUSDIVIDER(config);
	} else {
		busDivisor = 1;
	}


	uint32_t unknownPLL = CLOCK1_UNKNOWNPLL(config);
	uint32_t unknownDivisor;

	if(CLOCK1_UNKNOWNHASDIVIDER(config)) {
		unknownDivisor = CLOCK1_UNKNOWNDIVIDER(config);
	} else {
		unknownDivisor = 1;
	}

	uint32_t peripheralFactor = CLOCK1_PERIPHERALDIVIDER(config);

	config = GET_REG(CLOCK1 + CLOCK1_CONFIG2);
	uint32_t displayPLL = CLOCK1_DISPLAYPLL(config);
	uint32_t displayDivisor;

	if(CLOCK1_DISPLAYDIVIDER(config)) {
		displayDivisor = CLOCK1_DISPLAYHASDIVIDER(config) + 1;
	} else {
		displayDivisor = 1;
	}

	ClockPLL = clockPLL;

	/* Now we get to populate the PLLFrequencies table so we can calculate the various clock frequencies */
	int pll = 0;
	for(pll = 0; pll < NUM_PLL; pll++) {
		uint32_t pllStates = GET_REG(CLOCK1 + CLOCK1_PLLMODE);
		if(CLOCK1_PLLMODE_ONOFF(pllStates, pll)) {
			int dividerMode = CLOCK1_PLLMODE_DIVIDERMODE(pllStates, pll);
			int dividerModeInputFrequency = dividerMode;
			uint32_t inputFrequency;
			uint32_t inputFrequencyMult;
			uint32_t pllConReg;
			uint32_t pllCon;

			if(pll == 0) {
				inputFrequency = PLL0_INFREQ_DIV * 2;
				pllConReg = CLOCK1 + CLOCK1_PLL0CON;
				inputFrequencyMult = PLL0_INFREQ_MULT * 2;
			} else if(pll == 1) {
				inputFrequency = PLL1_INFREQ_DIV * 2;
				pllConReg = CLOCK1 + CLOCK1_PLL1CON;
				inputFrequencyMult = PLL1_INFREQ_MULT * 2;
			} else if(pll == 2) {
				inputFrequency = PLL2_INFREQ_DIV * 2;
				pllConReg = CLOCK1 + CLOCK1_PLL2CON;
				inputFrequencyMult = PLL2_INFREQ_MULT * 2;
			} else if(pll == 3) {
				inputFrequency = PLL3_INFREQ_DIV * 2;
				pllConReg = CLOCK1 + CLOCK1_PLL3CON;
				inputFrequencyMult = PLL3_INFREQ_MULT * 2;
				dividerMode = CLOCK1_PLLMODE_DIVIDE;
			} else {
				PLLFrequencies[pll] = 0;
				continue;
			}

			if(dividerModeInputFrequency == CLOCK1_PLLMODE_MULTIPLY) {
				inputFrequency = inputFrequencyMult;
			}

			pllCon = GET_REG(pllConReg);

			uint64_t afterMDiv = inputFrequency * CLOCK1_MDIV(pllCon);
			uint64_t afterPDiv;
			if(dividerMode == CLOCK1_PLLMODE_DIVIDE) {
				afterPDiv = afterMDiv / CLOCK1_PDIV(pllCon);
			} else {
				afterPDiv = afterMDiv * CLOCK1_PDIV(pllCon);
			}

			uint64_t afterSDiv;
			if(pll != clockPLL) {
				afterSDiv = afterPDiv / (((uint64_t)0x1) << CLOCK1_SDIV(pllCon));
			} else {
				afterSDiv = afterPDiv;
				ClockSDiv = CLOCK1_SDIV(pllCon);
			}

			PLLFrequencies[pll] = (uint32_t)afterSDiv;

			bufferPrintf("PLL %d: %d\n", pll, PLLFrequencies[pll]);
		} else {
			PLLFrequencies[pll] = 0;
			bufferPrintf("PLL %d: off.\n", pll);
			continue;
		}
	}

	ClockFrequency = PLLFrequencies[clockPLL] / clockDivisor;
	MemoryFrequency = PLLFrequencies[memoryPLL] / memoryDivisor;
	BusFrequency = PLLFrequencies[busPLL] / busDivisor;
	UnknownFrequency = PLLFrequencies[unknownPLL] / unknownDivisor;
	PeripheralFrequency = BusFrequency / (1 << peripheralFactor);
	DisplayFrequency = PLLFrequencies[displayPLL] / displayDivisor;
	FixedFrequency = FREQUENCY_BASE * 2;
	TimebaseFrequency = FREQUENCY_BASE / 2;

	TicksPerSec = FREQUENCY_BASE;

	return 0;
}
#endif

#ifdef CONFIG_IPOD2G
// Data table for the various register settings needed for each clock gate switch
// TODO: move this to the hardware/clock.h file
ClockGate ClockGateTable[29] = {
	// id	reg_0_bits	reg_1_bits	reg_2_bits	reg_3_bits	reg_4_bits
	{0x0,	0x00000080,	0x0,		0x0,		0x0,		0x0		},
	{0x1,	0x0,		0x00004000,	0x0,		0x0,		0x0		},
	{0x2,	0x00004000,	0x0,		0x0,		0x00004E00,	0x0		},
	{0x3,	0x00000800,	0x0,		0x0,		0x0,		0x0		},
	{0x4,	0x00001000,	0x0,		0x0,		0x0,		0x0		},
	{0x5,	0x00000220,	0x0,		0x0,		0x0,		0x0		},
	{0x6,	0x0,		0x00001000,	0x0,		0x0,		0x0		},
	{0x7,	0x0,		0x00000010,	0x0,		0x0,		0x00000800	},
	{0x8,	0x0,		0x00000040,	0x0,		0x0,		0x00001000	},
	{0x9,	0x00000002,	0x0,		0x0,		0x0,		0x00010000	},
	{0xA,	0x0,		0x00080000,	0x0,		0x0,		0x0		},
	{0xB,	0x0,		0x00002000,	0x0,		0x0,		0x0		},
	{0xC,	0x00000400,	0x0,		0x0,		0x0,		0x0		},
	{0xD,	0x00000001,	0x0,		0x0,		0x0,		0x0		},
	{0xE,	0x0,		0x00000004,	0x0,		0x0,		0x00002000	},
	{0xF,	0x0,		0x00000800,	0x0,		0x0,		0x00004000	},
	{0x10,	0x0,		0x00008000,	0x0,		0x0,		0x00008000	},
	{0x11,	0x0,		0x0,		0x00000002,	0x0,		0x00080000	},
	{0x12,	0x0,		0x0,		0x00000010,	0x0,		0x00100000	},
	{0x13,	0x0,		0x1F800020,	0x00000060,	0x0,		0x00C0007F	},
	{0x14,	0x0,		0x00000200,	0x0,		0x0,		0x00000080	},
	{0x15,	0x0,		0x20000000,	0x0,		0x0,		0x00000100	},
	{0x16,	0x0,		0x40000000,	0x0,		0x0,		0x00000200	},
	{0x17,	0x0,		0x80000000,	0x0,		0x0,		0x00000400	},
	{0x18,	0x00000004,	0x0,		0x0,		0x0,		0x0		},
	{0x19,	0x0,		0x00000008,	0x0,		0x0,		0x0		},
	{0x1A,	0x0,		0x0,		0x00000001,	0x0,		0x0		},
	{0x1B,	0x00000220,	0x0,		0x0,		0x0,		0x0		},
	{0x1C,	0x00000220,	0x0,		0x0,		0x0,		0x0		}
};

void clock_gate_switch(uint32_t gate, OnOff on_off) {		// S5L8720
	uint32_t gate_register;
	uint32_t gate_register_bits;
	uint32_t reg_num;
	for (reg_num=0; reg_num<NUM_CLOCK_GATE_REGISTERS; reg_num++) {
		switch (reg_num) {
			case 0:
				gate_register = CLOCK1 + CLOCK1_GATES_0;
				gate_register_bits = ClockGateTable[gate].reg_0_bits;
				break;
			case 1:
				gate_register = CLOCK1 + CLOCK1_GATES_1;
				gate_register_bits = ClockGateTable[gate].reg_1_bits;
				break;
			case 2:
				gate_register = CLOCK1 + CLOCK1_GATES_2;
				gate_register_bits = ClockGateTable[gate].reg_2_bits;
				break;
			case 3:
				gate_register = CLOCK1 + CLOCK1_GATES_3;
				gate_register_bits = ClockGateTable[gate].reg_3_bits;
				break;
			case 4:
				gate_register = CLOCK1 + CLOCK1_GATES_4;
				gate_register_bits = ClockGateTable[gate].reg_4_bits;
				break;
			default:
				continue;
		}
		
		uint32_t gate_register_data = GET_REG(gate_register);
		if (on_off == ON) {
			gate_register_data &= ~gate_register_bits;
		} else {
			gate_register_data |= gate_register_bits;
		}
		SET_REG(gate_register, gate_register_data);
	}
}

#else
void clock_gate_switch(uint32_t gate, OnOff on_off) {		// S5L8900
	uint32_t gate_register;
	uint32_t gate_flag;

	if(gate < CLOCK1_Separator) {
		gate_register = CLOCK1 + CLOCK1_CL2_GATES;
		gate_flag = gate;
	} else {
		gate_register = CLOCK1 + CLOCK1_CL3_GATES;
		gate_flag = gate - CLOCK1_Separator;
	}

	uint32_t gates = GET_REG(gate_register);

	if(on_off == ON) {
		gates &= ~(1 << gate_flag);
	} else {
		gates |= 1 << gate_flag;
	}

	SET_REG(gate_register, gates);

}
#endif

#ifdef CONFIG_IPOD2G
int clock_set_base_clock_divisor(ClockDivisorCode code) {		// S5L8720
	int baseDivisor;
	
	switch(code) {
		case ClockDivisorCode0:
			baseDivisor = CLOCK_BASE_DIVISOR_0;
			break;
		case ClockDivisorCode1:
			baseDivisor = CLOCK_BASE_DIVISOR_1;
			break;
		case ClockDivisorCode2:
			baseDivisor = CLOCK_BASE_DIVISOR_2;
			break;
		case ClockDivisorCode3:
			baseDivisor = CLOCK_BASE_DIVISOR_3;
			break;
		default:
			return -1;
	}
	
	SET_REG(CLOCK1 + CLOCK1_CONFIG0, (GET_REG(CLOCK1 + CLOCK1_CONFIG0) & (~CLOCK_BASE_DIVISOR_MASK)) | baseDivisor);

	return 0;
}
#else
int clock_set_bottom_bits_38100000(ClockDivisorCode code) {		// S5L8900
	int bottomValue;

	switch(code) {
		case ClockDivisorCode0:
			bottomValue = CLOCK0_CONFIG_C0VALUE;
			break;
		case ClockDivisorCode1:
			bottomValue = CLOCK0_CONFIG_C1VALUE;
			break;
		case ClockDivisorCode2:
			bottomValue = CLOCK0_CONFIG_C2VALUE;
			break;
		case ClockDivisorCode3:
			bottomValue = CLOCK0_CONFIG_C3VALUE;
			break;
		default:
			return -1;
	}

	SET_REG(CLOCK0 + CLOCK0_CONFIG, (GET_REG(CLOCK0 + CLOCK0_CONFIG) & (~CLOCK0_CONFIG_BOTTOMMASK)) | bottomValue);

	return 0;
}
#endif

uint32_t clock_get_frequency(FrequencyBase freqBase) {
	switch(freqBase) {
		case FrequencyBaseClock:
			return ClockFrequency;
		case FrequencyBaseMemory:
			return MemoryFrequency;
		case FrequencyBaseBus:
			return BusFrequency;
		case FrequencyBasePeripheral:
			return PeripheralFrequency;
		case FrequencyBaseUnknown:
			return UnknownFrequency;
		case FrequencyBaseDisplay:
			return DisplayFrequency;
		case FrequencyBaseFixed:
			return FixedFrequency;
		case FrequencyBaseTimebase:
			return TimebaseFrequency;
		#ifdef CONFIG_IPOD2G
		case FrequencyBaseUsbPhy:
			return UsbPhyFrequency;
		#endif
		default:
			return 0;
	}
}

#ifndef CONFIG_IPOD2G
uint32_t clock_calculate_frequency(uint32_t pdiv, uint32_t mdiv, FrequencyBase freqBase) {
	unsigned int y = clock_get_frequency(freqBase) / (0x1 << ClockSDiv);
	uint64_t z = (((uint64_t) pdiv) * ((uint64_t) 1000000000)) / ((uint64_t) y);
	uint64_t divResult = ((uint64_t)(1000000 * mdiv)) / z;
	return divResult - 1;
}
#endif

#ifndef CONFIG_IPOD2G
static void clock0_reset_frequency() {
	SET_REG(CLOCK0 + CLOCK0_ADJ1, (GET_REG(CLOCK0 + CLOCK0_ADJ1) & CLOCK0_ADJ_MASK) | clock_calculate_frequency(0x200C, 0x40, FrequencyBaseMemory));
	SET_REG(CLOCK0 + CLOCK0_ADJ2, (GET_REG(CLOCK0 + CLOCK0_ADJ2) & CLOCK0_ADJ_MASK) | clock_calculate_frequency(0x1018, 0x4, FrequencyBaseBus));
}
#endif

#ifndef CONFIG_IPOD2G
void clock_set_sdiv(int sdiv) {
	int oldClockSDiv = ClockSDiv;

	if(sdiv < 1) {
		sdiv = 0;
	}

	if(sdiv <= 2) {
		ClockSDiv = sdiv;
	}

	if(oldClockSDiv < ClockSDiv) {
		clock0_reset_frequency();
	}

	uint32_t clockCon = 0;
	switch(ClockPLL) {
		case 0:
			clockCon = CLOCK1 + CLOCK1_PLL0CON;
			break;
		case 1:
			clockCon = CLOCK1 + CLOCK1_PLL1CON;
			break;
		case 2:
			clockCon = CLOCK1 + CLOCK1_PLL2CON;
			break;
		case 3:
			clockCon = CLOCK1 + CLOCK1_PLL3CON;
			break;
	}

	if(clockCon != 0) {
		SET_REG(clockCon, (GET_REG(clockCon) & ~0x7) | ClockSDiv);
	}

	if(oldClockSDiv >= ClockSDiv) {
		clock0_reset_frequency();
	}
}
#endif

#ifdef CONFIG_IPOD2G
unsigned int get_base_frequency(void) {
	if (chipid_get_security_epoch() == 1) {
		return CLOCK1_SECURITY_EPOCH_1_FREQUENCY;
	} else {
		return CLOCK1_SECURITY_EPOCH_0_FREQUENCY;
	}
}
#endif


