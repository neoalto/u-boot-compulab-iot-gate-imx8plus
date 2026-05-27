// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Minimal SPL for CompuLab IOT-GATE-IMX8PLUS.
 */

#include <hang.h>
#include <init.h>
#include <log.h>
#include <power/pmic.h>
#include <power/pca9450.h>
#include <spl.h>
#include <asm/arch/clock.h>
#include <asm/arch/ddr.h>
#include <asm/arch/sys_proto.h>
#include <asm/mach-imx/boot_mode.h>

#include "ddr/ddr.h"

int spl_board_boot_device(enum boot_device boot_dev_spl)
{
	return BOOT_DEVICE_BOOTROM;
}

void spl_board_init(void)
{
	arch_misc_init();

	/*
	 * Set GIC clock to 500 MHz for OD VDD_SOC, matching the i.MX8MP EVK
	 * and CompuLab downstream SPL sequence.
	 */
	clock_enable(CCGR_GIC, 0);
	clock_set_target_val(GIC_CLK_ROOT, CLK_ROOT_ON | CLK_ROOT_SOURCE_SEL(5));
	clock_enable(CCGR_GIC, 1);

	puts("Normal Boot\n");
}

#if CONFIG_IS_ENABLED(DM_PMIC_PCA9450)
int power_init_board(void)
{
	struct udevice *dev;
	int ret;

	ret = pmic_get("pca9450@25", &dev);
	if (ret == -ENODEV) {
		puts("No pca9450@25\n");
		return 0;
	}
	if (ret < 0)
		return ret;

	/* BUCKxOUT_DVS0/1 control BUCK123 output. */
	pmic_reg_write(dev, PCA9450_BUCK123_DVS, 0x29);

	/* VDD_SOC 0.95V before first DRAM access, DVS1 0.85V for suspend. */
	pmic_reg_write(dev, PCA9450_BUCK1OUT_DVS0, 0x1c);
	pmic_reg_write(dev, PCA9450_BUCK1OUT_DVS1, 0x14);
	pmic_reg_write(dev, PCA9450_BUCK1CTRL, 0x59);

	/* VDD_ARM 0.95V for OD/OD operation. */
	pmic_reg_write(dev, PCA9450_BUCK2OUT_DVS0, 0x1c);

	/* Configure WDOG_B as cold reset, as in CompuLab downstream SPL. */
	pmic_reg_write(dev, PCA9450_RESET_CTRL, 0xa1);

	return 0;
}
#endif

#ifdef CONFIG_SPL_LOAD_FIT
int board_fit_config_name_match(const char *name)
{
	return 0;
}
#endif

/* Do not use BSS area before it is cleared by start.S. */
void board_init_f(ulong dummy)
{
	int ret;

	arch_cpu_init();

	/* UART2: ttymxc1, base 0x30890000. */
	init_uart_clk(1);

	ret = spl_early_init();
	if (ret) {
		debug("spl_early_init() failed: %d\n", ret);
		hang();
	}

	preloader_console_init();
	enable_tzc380();
	power_init_board();

	spl_dram_init_compulab();
}
