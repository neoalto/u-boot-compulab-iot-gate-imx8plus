/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef __IOT_GATE_IMX8PLUS_CONFIG_H
#define __IOT_GATE_IMX8PLUS_CONFIG_H

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>

#define CFG_SYS_INIT_RAM_ADDR	0x40000000
#define CFG_SYS_INIT_RAM_SIZE	SZ_512K

#define CFG_SYS_SDRAM_BASE	0x40000000
#define PHYS_SDRAM		0x40000000
#define PHYS_SDRAM_SIZE		SZ_2G

#define CFG_SYS_UBOOT_BASE \
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#endif
