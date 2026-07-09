// SPDX-License-Identifier: GPL-2.0
/*
 * Minimal CompuLab IOT-GATE-IMX8PLUS board support.
 */

#include <init.h>
#include <env.h>
#include <asm/global_data.h>
#include <linux/sizes.h>
#include <linux/types.h>
#include <stdio.h>
#include <dm.h>
#include <i2c.h>
#include <net.h>
#include <linux/errno.h>
#include <linux/libfdt.h>

#define IOT_GATE_EEPROM_BUS 1
#define IOT_GATE_EEPROM_ADDR 0x50
#define IOT_GATE_EEPROM_ADDR_LEN 1
#define IOT_GATE_MAC_LEN 6
#define IOT_GATE_EEPROM_ETHADDR_OFF 0x04
#define IOT_GATE_EEPROM_ETH1ADDR_OFF 0x0a

struct iot_gate_mac_info {
	const char *env_name;
	const char *alias_name;
	unsigned int offset;
	unsigned char mac[IOT_GATE_MAC_LEN];
	bool valid;
};

DECLARE_GLOBAL_DATA_PTR;

static struct iot_gate_mac_info iot_gate_macs[] = {
	{
		.env_name = "ethaddr",
		.alias_name = "ethernet0",
		.offset = IOT_GATE_EEPROM_ETHADDR_OFF,
	},
	{
		.env_name = "eth1addr",
		.alias_name = "ethernet1",
		.offset = IOT_GATE_EEPROM_ETH1ADDR_OFF,
	},
};

#if !defined(CONFIG_SPL_BUILD) && CONFIG_IS_ENABLED(DM_I2C)
static bool iot_gate_mac_is_valid(const unsigned char mac[IOT_GATE_MAC_LEN])
{
	return is_valid_ethaddr(mac);
}
#endif


#if !defined(CONFIG_SPL_BUILD) && CONFIG_IS_ENABLED(ENV_SUPPORT) && \
	CONFIG_IS_ENABLED(DM_I2C)
static void iot_gate_mac_to_string(const unsigned char mac[IOT_GATE_MAC_LEN],
				   char *buf, size_t len)
{
	snprintf(buf, len, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1],
		 mac[2], mac[3], mac[4], mac[5]);
}
#endif

#if !defined(CONFIG_SPL_BUILD) && CONFIG_IS_ENABLED(DM_I2C)
static int iot_gate_read_eeprom_mac(unsigned int offset,
				    unsigned char mac[IOT_GATE_MAC_LEN])
{
	struct udevice *eeprom;
	int ret;

	ret = i2c_get_chip_for_busnum(IOT_GATE_EEPROM_BUS, IOT_GATE_EEPROM_ADDR,
				      IOT_GATE_EEPROM_ADDR_LEN, &eeprom);
	if (ret) {
		printf("WARN: cannot get EEPROM 0x%02x on I2C bus %d: %d\n",
		       IOT_GATE_EEPROM_ADDR, IOT_GATE_EEPROM_BUS, ret);
		return ret;
	}

	ret = dm_i2c_read(eeprom, offset, mac, IOT_GATE_MAC_LEN);
	if (ret) {
		printf("WARN: cannot read EEPROM MAC at offset 0x%x: %d\n",
		       offset, ret);
		return ret;
	}

	if (!iot_gate_mac_is_valid(mac)) {
		printf("WARN: invalid EEPROM MAC at offset 0x%x: %02x:%02x:%02x:%02x:%02x:%02x\n",
		       offset, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		return -EINVAL;
	}

	return 0;
}

static void iot_gate_load_mac_addresses(void)
{
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(iot_gate_macs); i++) {
		struct iot_gate_mac_info *info = &iot_gate_macs[i];

		if (info->valid)
			continue;

		if (iot_gate_read_eeprom_mac(info->offset, info->mac))
			continue;

		info->valid = true;
	}
}
#else
static void iot_gate_load_mac_addresses(void)
{
}
#endif

#if !defined(CONFIG_SPL_BUILD) && CONFIG_IS_ENABLED(ENV_SUPPORT)
static void iot_gate_set_mac_env(void)
{
	char value[18];
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(iot_gate_macs); i++) {
		struct iot_gate_mac_info *info = &iot_gate_macs[i];

		if (!info->valid || env_get(info->env_name))
			continue;

		iot_gate_mac_to_string(info->mac, value, sizeof(value));
		if (env_set(info->env_name, value))
			printf("WARN: failed to set %s from EEPROM\n",
			       info->env_name);
		else
			printf("Using EEPROM %s=%s\n", info->env_name, value);
	}
}
#else
static void iot_gate_set_mac_env(void)
{
}
#endif

static void iot_gate_setup_mac_addresses(void)
{
	iot_gate_load_mac_addresses();
	iot_gate_set_mac_env();
}

static int
iot_gate_fdt_set_mac_by_alias(void *blob, const char *alias_name,
			      const unsigned char mac[IOT_GATE_MAC_LEN])
{
	const char *path;
	int aliases;
	int node;
	int ret;

	aliases = fdt_path_offset(blob, "/aliases");
	if (aliases < 0)
		return aliases;

	path = fdt_getprop(blob, aliases, alias_name, NULL);
	if (!path)
		return -ENOENT;

	node = fdt_path_offset(blob, path);
	if (node < 0)
		return node;

	ret = fdt_setprop(blob, node, "mac-address", mac, IOT_GATE_MAC_LEN);
	if (ret)
		return ret;

	ret = fdt_setprop(blob, node, "local-mac-address", mac,
			  IOT_GATE_MAC_LEN);
	if (ret)
		return ret;

	return 0;
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	unsigned int i;

	iot_gate_load_mac_addresses();

	for (i = 0; i < ARRAY_SIZE(iot_gate_macs); i++) {
		struct iot_gate_mac_info *info = &iot_gate_macs[i];
		int ret;

		if (!info->valid)
			continue;

		ret = iot_gate_fdt_set_mac_by_alias(blob, info->alias_name,
						    info->mac);
		if (ret)
			printf("WARN: failed to inject %s MAC into FDT alias %s: %d\n",
			       info->env_name, info->alias_name, ret);
	}

	return 0;
}

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
	iot_gate_setup_mac_addresses();
	iot_gate_note_env_health();

	return 0;
}
