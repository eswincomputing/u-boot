// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2025, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
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
 * Authors: Dongdong Zhang <zhangdongdong@eswincomputing.com>
 */

#include <command.h>
#include <asm/gpio.h>

static int do_eswin_die(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int i;
	int ret;
	// Read die ordinary GPIO values
	int die_values[4] = { 0 };
	int die_ordinary = 0;
	unsigned int die_gpio_pins[4] = { 58, 59, 60, 61 };

	printf("Reading die ordinary GPIOs:\n");
	for (i = 0; i < 4; i++) {
		ret = gpio_request(die_gpio_pins[i], "die-ordinary");
		if (ret) {
			printf("Failed to request GPIO%d: %d\n", die_gpio_pins[i], ret);
			continue;
		}

		ret = gpio_direction_input(die_gpio_pins[i]);
		if (ret) {
			printf("Failed to set GPIO%d as input: %d\n", die_gpio_pins[i], ret);
			gpio_free(die_gpio_pins[i]);
			continue;
		}

		die_values[i] = gpio_get_value(die_gpio_pins[i]);

		gpio_free(die_gpio_pins[i]);
	}

	// Calculate die_ordinary from GPIO values
	die_ordinary = die_values[3] << 3 | die_values[2] << 2 | die_values[1] << 1 | die_values[0];

	// Ensure die_ordinary is within the valid range (0-9)
	if (die_ordinary > 9) {
		printf("die_ordinary is out of range\n");
		for (i = 0; i < 4; i++) {
			printf("GPIO%d value: %d\n", die_gpio_pins[i], die_values[i]);
		}
		return 1;
	}

	printf("Current die: %d\n", die_ordinary);

	return 0;
}

U_BOOT_CMD(eswin_die, 1, 1, do_eswin_die, "Display ESWIN VPU7702 die information",
	   "- Show die ordinary GPIOs and current die number");