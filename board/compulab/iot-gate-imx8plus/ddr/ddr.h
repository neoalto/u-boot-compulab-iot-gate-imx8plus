/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef __IOT_GATE_IMX8PLUS_DDR_H__
#define __IOT_GATE_IMX8PLUS_DDR_H__

#include <asm/arch/ddr.h>

extern struct dram_timing_info ucm_dram_timing_01061010_2G;
extern struct dram_timing_info ucm_dram_timing_13000210_2G;

void spl_dram_init_compulab(void);

#endif
