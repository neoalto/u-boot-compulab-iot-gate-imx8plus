// SPDX-License-Identifier: GPL-2.0+
/*
 * Minimal CompuLab IOT-GATE-IMX8PLUS board support.
 */

#include <init.h>
#include <env.h>
#include <asm/global_data.h>
#include <linux/sizes.h>
#include <linux/types.h>
#include <stdio.h>

DECLARE_GLOBAL_DATA_PTR;

int board_phys_sdram_size(phys_size_t *size)
{
	/* D2 SOM: 2 GiB LPDDR4 at 0x40000000. */
	*size = SZ_2G;
	return 0;
}

static int iot_gate_note_env_health(void)
{
#if CONFIG_IS_ENABLED(ENV_SUPPORT)
	const char *health = "primary";
	const char *error = NULL;
	ulong failures;

	/*
	 * The MMC redundant environment loader has already attempted the primary
	 * and backup copies before board_late_init() runs.  Record non-primary
	 * cases so Linux/libubootenv can report that U-Boot had to recover from
	 * a damaged or missing persisted configuration.
	 */
	if (gd->flags & GD_FLG_ENV_DEFAULT) {
		health = "default";
		error = "env_default_used";
	} else if (gd->env_valid == ENV_REDUND) {
		health = "redundant";
		error = "env_redundant_used";
	}

	env_set("ab_env_health", health);
	if (!error)
		return 0;

	printf("WARN: U-Boot environment recovery path used: %s\n", error);
	env_set("ab_last_error", error);
	env_set("ab_last_slot", "none");
	failures = env_get_ulong("ab_config_fail_count", 10, 0);
	env_set_ulong("ab_config_fail_count", failures + 1);

	/* Best effort only: never block booting because diagnostics could not save. */
	return env_save();
#else
	/* SPL and configurations without environment support have no gd->env_valid. */
	return 0;
#endif
}

int board_late_init(void)
{
	iot_gate_note_env_health();

	return 0;
}
