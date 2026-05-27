// SPDX-License-Identifier: GPL-2.0+
/*
 * Minimal CompuLab IOT-GATE-IMX8PLUS board support.
 */

#include <init.h>
#include <env.h>
#include <linux/sizes.h>
#include <linux/types.h>

int board_phys_sdram_size(phys_size_t *size)
{
	/* D2 SOM: 2 GiB LPDDR4 at 0x40000000. */
	*size = SZ_2G;
	return 0;
}

int board_late_init(void)
{
	env_set("board_name", "IOT-GATE-IMX8PLUS");
	env_set("board_rev", "D2");

	return 0;
}
