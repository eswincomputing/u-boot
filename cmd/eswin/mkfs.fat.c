// SPDX-License-Identifier: GPL-2.0
/*
 * FAT file system formatting CMD driver
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
 * Authors: Xiang Xu <xuxiang@eswincomputing.com>
 */

#include "common.h"
#include <command.h>
#include <eswin/esw_mkfs.h>

static int do_mkfsfat(struct cmd_tbl *cmdtp, int flag, int argc,
			    char *const argv[])
{
	int ret;

	if (argc < 2) {
		printf("usage: fatcreat <interface> [<dev[:part]>]\n");
		return CMD_RET_USAGE;
	}

	ret =  mkfs_fat(argv[1], argv[2]);
	if( 0 != ret )
		return CMD_RET_FAILURE;
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
	mkfsfat,	3,	0,	do_mkfsfat,
	"Put the core in spin loop (Secure Boot Only)",
	"usage: mkfsfat <interface> [<dev[:part]>]\n"
);