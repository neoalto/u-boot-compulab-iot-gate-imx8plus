/* SPDX-License-Identifier: GPL-2.0+ */
#ifndef __IOT_GATE_IMX8PLUS_CONFIG_H
#define __IOT_GATE_IMX8PLUS_CONFIG_H

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>

#define CFG_SYS_UBOOT_BASE \
	(QSPI0_AMBA_BASE + CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR * 512)

#endif
