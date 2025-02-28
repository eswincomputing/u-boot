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
#include <autoboot.h>

static int do_eswin_die(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int die_ordinary = get_die_ordinary();
	if (die_ordinary == -1) {
		printf("Failed to get die ordinary value, ret=%d\n", die_ordinary);
		return 1;
	}
	printf("Current die: %d\n", die_ordinary);
	return 0;
}
U_BOOT_CMD(eswin_die, 1, 1, do_eswin_die, "Display ESWIN VPU7702 die information",
	   "- Show die ordinary GPIOs and current die number");
