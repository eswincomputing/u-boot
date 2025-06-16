// SPDX-License-Identifier: GPL-2.0
/*
 * eic7702 D2D related command
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 * SPDX-License-Identifier: GPL-2.0
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 2.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 * Authors: Huangyifeng <huangyifeng@eswincomputing.com>
 */

#include <common.h>
#include <command.h>
#include <spi.h>
#include <spi_flash.h>
#include <dm/uclass.h>
#include <init.h>
#include <dm/device-internal.h>
#include <stdint.h>

#define PMIX_RECORD_ADDR                0xefc000    /* 0xefc000 - 0xefffff (16KiB) */
#define PMIX_RECORD_ADDR_ALT            0x7fc000    /* 0x7fc000 - 0x7fffff (16KiB) */

int d2d_pmix_erase(const char *node_name)
{
	struct spi_flash *flash = NULL;
	struct udevice *bus, *dev;
	int ret;
	void *addr;

	ret = uclass_get_device_by_name(UCLASS_SPI, node_name, &bus);
	if(ret) {
		printf("SPI flash, failed to get node %s\n", node_name);
		return ret;
	}

	ret = spi_find_chip_select(bus, 0, &dev);
	if(ret) {
		printf("SPI flash, Invalid chip select :%d (err=%d)\n", 0, ret);
		return ret;
	}

	if (!device_active(dev)) {
		if(device_probe(dev)) {
			printf("SPI flash, device_probe fail\n");
			return -1;
		}
	}
	flash = dev_get_uclass_priv(dev);
	if(!flash) {
		printf("SPI flash, dev_get_uclass_priv failed\n");
		return -1;
	}
	addr = (void *)ALIGN_DOWN(PMIX_RECORD_ADDR, SZ_64K);
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 0);
	ret = spi_flash_erase(flash, PMIX_RECORD_ADDR, SZ_4K);
	if(ret) {
		printf("D2D PMIX Data erase failed\n");
	}
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 1);
	/*
	addr = (void *)ALIGN_DOWN(PMIX_RECORD_ADDR_ALT, SZ_64K);
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 0);
	ret = spi_flash_erase(flash, PMIX_RECORD_ADDR_ALT, SZ_4K);
	if(ret) {
		printf("D2D PMIX Data erase failed\n");
	}
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 1);
	*/

	printf("D2D PMIX Data has been invalid\n");
	return 0;
}

static int do_d2d_pmix_invalid(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	d2d_pmix_erase("spi@51800000");
	d2d_pmix_erase("spi@71800000");
	return 0;
}

/* Subcommand list */
static struct cmd_tbl d2d_sub[] = {
	U_BOOT_CMD_MKENT(pmix_invalid, 1, 0, do_d2d_pmix_invalid, "", ""),
};

/* Parent command handler function */
static int do_d2d(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	struct cmd_tbl *sub_cmd;
	const char *cmd = (argc > 1) ? argv[1] : NULL;

	if (!cmd)
		return CMD_RET_USAGE;

	sub_cmd = find_cmd_tbl(cmd, d2d_sub, ARRAY_SIZE(d2d_sub));
	if (sub_cmd)
		return sub_cmd->cmd(sub_cmd, flag, argc - 1, argv + 1);

	return CMD_RET_USAGE;
}

/* Register parent command */
U_BOOT_CMD(
	d2d,	/* Command name */
	2,		/* Maximum number of arguments (d2d + subcommand) */
	0,		/* Repeat count */
	do_d2d,	/* Handler function */
	"D2D subsystem commands",	/* Short help text */
	"pmix_invalid - Invalidate D2D PMIX Data\n"	/* Long help text */
);
