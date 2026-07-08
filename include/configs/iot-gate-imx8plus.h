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

/*
 * With CONFIG_ENV_WRITEABLE_LIST enabled, every variable that U-Boot may
 * update at runtime must be explicitly marked writeable.  Policy commands,
 * bootlimit, device selection, image names and load addresses intentionally
 * remain read-only because they are not listed here.
 */
#define CFG_ENV_FLAGS_LIST_STATIC \
	"boot_slot:sw," \
	"upgrade_available:bw," \
	"bootcount:dw," \
	"ustate:dw," \
	"recovery_status:sw," \
	"boot_watchdog:bw," \
	"ab_fail_count:dw," \
	"ab_config_fail_count:dw," \
	"ab_last_error:sw," \
	"ab_last_slot:sw," \
	"ab_env_health:sw," \
	"slot:sw," \
	"boot_part_name:sw," \
	"root_part_name:sw," \
	"bootpart:dw," \
	"bootargs:sw," \
	"mmcroot:sw," \
	"uuid:sw," \
	"fileaddr:xw," \
	"filesize:xw"

#endif
