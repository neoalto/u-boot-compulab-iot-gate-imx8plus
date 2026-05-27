// SPDX-License-Identifier: GPL-2.0+
/*
 * D2 LPDDR4 init for CompuLab IOT-GATE-IMX8PLUS.
 */

#include <hang.h>
#include <linux/bitops.h>
#include <spl.h>
#include <asm/arch/ddr.h>

#include "ddr.h"

void spl_dram_init_compulab(void)
{
	struct dram_timing_info *timing;
	const char *name;

	if (IS_ENABLED(CONFIG_IOT_GATE_IMX8PLUS_D2_ISSI)) {
		timing = &ucm_dram_timing_13000210_2G;
		name = "D2 ISSI 0x13000210";
	} else {
		timing = &ucm_dram_timing_01061010_2G;
		name = "D2 Samsung/Nanya/Kingston-compatible";
	}

	printf("DDRINFO: using %s LPDDR4 timing\n", name);

	if (ddr_init(timing)) {
		puts("DDR init failed\n");
		hang();
	}
}
