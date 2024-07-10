// SPDX-License-Identifier: GPL-2.0
/*
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
 * Authors: Huangyifeng <huangifeng@eswincomputing.com>
 */

#include <common.h>
#include <command.h>
#include <clk.h>
#include <dm.h>
#include <dm/device.h>
#include <dm/uclass.h>
/*
cmd example:
	clk_test clock-controller@51828000 0 enable


	clk_test clock-controller@51828000 1 get
	clk_test clock-controller@51828000 1 set 148500000

	clk_test clock-controller@51828000 2 get
	clk_test clock-controller@51828000 2 set 50000000

*/
static int do_clk_test(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	struct udevice *dev;
	struct clk clk;
	ulong rate;
	int ret;
	int clk_id;

	if (argc < 4 || argc > 5) {
		printf("Usage: clk_test <device_name> <id> <get|set|enable|disable> [<new_rate>]\n");
		return CMD_RET_USAGE;
	}

	const char *dev_name = argv[1];
	clk_id = simple_strtoul(argv[2], NULL, 10);
	const char *action = argv[3];

	// Find the device by name
	ret = uclass_get_device_by_name(UCLASS_CLK, dev_name, &dev);
	if (ret) {
		printf("Error: Clock device not found: %s\n", dev_name);
		return CMD_RET_FAILURE;
	}

	// Get the clock by id
	ret = clk_get_by_index(dev, clk_id, &clk);
	if (ret) {
		printf("Error: Failed to get clock with id %d for device %s\n", clk_id, dev_name);
		return CMD_RET_FAILURE;
	}

	if (strcmp(action, "get") == 0) {
		// Get the clock rate
		rate = clk_get_rate(&clk);
		if (rate == -ENOSYS) {
			printf("Error: Unsupported operation for clock with id %d\n", clk_id);
			return CMD_RET_FAILURE;
		}
		printf("Clock with id %d rate: %lu Hz\n", clk_id, rate);

	} else if (strcmp(action, "set") == 0 && argc == 5) {
		// Set a new clock rate
		ulong new_rate = simple_strtoul(argv[4], NULL, 10);
		ret = clk_set_rate(&clk, new_rate);
		if (ret) {
			printf("Error: Failed to set clock rate\n");
			return CMD_RET_FAILURE;
		}

		// Verify the new clock rate
		rate = clk_get_rate(&clk);
		printf("Clock with id %d new rate: %lu Hz\n", clk_id, rate);

	} else if (strcmp(action, "enable") == 0) {
		// Enable the clock
		ret = clk_enable(&clk);
		if (ret) {
			printf("Error: Failed to enable clock with id %d\n", clk_id);
			return CMD_RET_FAILURE;
		}
		printf("Clock with id %d enabled\n", clk_id);

	} else if (strcmp(action, "disable") == 0) {
		// Disable the clock
		ret = clk_disable(&clk);
		if (ret) {
			printf("Error: Failed to disable clock with id %d\n", clk_id);
			return CMD_RET_FAILURE;
		}
		printf("Clock with id %d disabled\n", clk_id);

	} else {
		printf("Usage: clk_test <device_name> <id> <get|set|enable|disable> [<new_rate>]\n");
		return CMD_RET_USAGE;
	}

	// Free the clock resource
	clk_free(&clk);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	clk_test, 5, 0, do_clk_test,
	"test clock functionality",
	"clk_test <device_name> <id> <get|set|enable|disable> [<new_rate>] - get, set, enable or disable clock"
);

