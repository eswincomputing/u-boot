// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN OTP CMD driver
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdint.h>
#include <common.h>
#include <command.h>
#include "eswin/es_otp.h"

#define OTP_MAGIC				0x42575345
#define FILE_OTP				0x0050544f
#define OTP_DEFINED_SIZE 		3200
#define OTP_RESERVED_SIZE		40
#define OTP_BINARY_FILE_LEN 	(OTP_DEFINED_SIZE + OTP_RESERVED_SIZE)

typedef struct __OTP_FILE_HEAD_ST {
	uint32_t magic;                    /* Must be 0x42575345, "ESWB */
	uint32_t file_type;                /* otp file 0x50544f "OTP"*/
	uint32_t version;
	uint32_t count;                   /* otp parameter count */
	uint64_t size;                    /* file size */
} OTP_FILE_HEAD_T;

typedef struct __OPT_DATA_ST {
	uint32_t count_num;            /* count num*/
	uint32_t offset;               /* reg offset */
	uint32_t len;                  /* val len bytes */
	uint8_t val[0];                /* reg vale hex */
} OTP_DATA_T;

static int eswin_otp_program_bit(int bit_addr)
{
	int byte_addr;
	unsigned int mask;
	int ret;
	uint8_t data = 1;

	byte_addr = bit_addr / 8;
	mask = 1 << (bit_addr % 8);
	data = data << (bit_addr % 8);

	ret = otp_write_data(byte_addr, (unsigned char)data, mask);
	if (ERR_OK != ret) {
		printf("faild to write otp bit, bit_addr 0x%x, byte_addr 0x%x, mask 0x%x, data %d, ret %d\n",
			bit_addr, byte_addr, mask, data, ret);
		return ret;
	}
	return 0;
}

static int eswin_otp_read_bit(int bit_addr, uint8_t *bit_data)
{
	int byte_addr;
	unsigned int mask;
	uint8_t byte_data;
	int ret;

	byte_addr = bit_addr / 8;
	mask = 1 << (bit_addr % 8);

	ret = otp_read_data(&byte_data, byte_addr, 1, 0);
	if (ERR_OK != ret) {
		printf("faild to read data, addr %d\n", byte_addr);
		return ret;
	}
	*bit_data = (byte_data & mask) >> (bit_addr % 8);
	return 0;
}

static int eswin_otp_read_field(char *field_name)
{
	int field;
	uint8_t *field_data = NULL;
	const struct OTP_MAP_ITEM *map = NULL;
	int ret;
	int i, valid_size;

	for (field = OTP_RSA_PUBLIC_KEY; field <= OTP_DFT_INFORMATION; field++) {
		map = &otp_map[field];
		if (!strcmp(field_name, map->name)) {
			break;
		}
	}
	if (field > OTP_DFT_INFORMATION) {
		printf("invalid otp field name %s\n", field_name);
		return -EINVAL;
	}
	field_data = (uint8_t *)malloc(map->otp_size);
	if (!field_data) {
		printf("faild to alloc data memory, size 0x%x\n", map->otp_size);
		return -ENOMEM;
	}
	memset(field_data, 0, map->otp_size);
	ret = get_otp_value(field_data, field, map->otp_size, map->enxor);
	if (ret) {
		printf("faild to read otp value, ret %d\n", ret);
		free(field_data);
		return -EAGAIN;
	}
	printf("field %s data:\r\n", field_name);
	if (map->enxor && map->otp_size >= 4) {
		valid_size = map->otp_size /4;
	} else {
		valid_size = map->otp_size;
	}
	for (i = 0; i < valid_size; i++) {
		printf("  0x%02X ", field_data[i]);
		if ( (i + 1) % 16 == 0) {
			printf("\r\n");
		}
	}
	printf("\r\n");
	free(field_data);
	return 0;
}

static int do_eswin_otp_read_field(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	int i;

	if (argc < 2) {
		printf("Usage: %s <field_name>, legal field name include:\n", argv[0]);
		for (i = OTP_RSA_PUBLIC_KEY; i <= OTP_DFT_INFORMATION; i++) {
			printf("%-12s%-52s%-s","","", otp_map[i].name);
			if (otp_map[i].enxor) {
				printf("(4bit XOR)");
			}
			printf("\n");
		}
		return CMD_RET_USAGE;
	}

	ret = eswin_otp_read_field(argv[1]);
	if (ret) {
		printf("Failed to read OTP field.\n");
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_eswin_otp_program_bit(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	unsigned long addr;

	if (argc < 2) {
		printf("Usage: %s <bit_address>\n", argv[0]);
		return CMD_RET_USAGE;
	}

	addr = simple_strtoul(argv[1], NULL, 10);
	ret = eswin_otp_program_bit(addr);
	if (ret) {
		printf("Failed to program OTP bit.\n");
		return CMD_RET_FAILURE;
	} else {
		printf("OTP bit programmed successfully.\n");
	}

	return CMD_RET_SUCCESS;
}

static int do_eswin_otp_read_bit(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	int ret;
	unsigned long addr;
	uint8_t bit_data;

	if (argc < 2) {
		printf("Usage: %s <bit_address>\n", argv[0]);
		return CMD_RET_USAGE;
	}

	addr = simple_strtoul(argv[1], NULL, 10);
	ret = eswin_otp_read_bit(addr, &bit_data);
	if (ret) {
		printf("Failed to read OTP bit.\n");
		return CMD_RET_FAILURE;
	} else {
		printf("OTP bit value: %d\n", bit_data);
	}

	return CMD_RET_SUCCESS;
}

static struct cmd_tbl cmd_eswin_otp[] = {
	U_BOOT_CMD_MKENT(read_field, 2, 0, do_eswin_otp_read_field, "", ""),
	//U_BOOT_CMD_MKENT(program_bit, 2, 0, do_eswin_otp_program_bit, "", ""),
	U_BOOT_CMD_MKENT(read_bit, 2, 0, do_eswin_otp_read_bit, "", ""),
};

static int do_eswin_otp(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
	struct cmd_tbl *cmd;

	if (argc < 2)
		return CMD_RET_USAGE;

	cmd = find_cmd_tbl(argv[1], cmd_eswin_otp, ARRAY_SIZE(cmd_eswin_otp));
	if (!cmd)
		return CMD_RET_USAGE;

	argc -= 1;
	argv += 1;
	return cmd->cmd(cmdtp, flag, argc, argv);
}

U_BOOT_CMD(
	es_otp, 3, 0, do_eswin_otp,
	"ESWIN OTP sub-system",
	"\n read_field <field_name> - Read OTP field data\n"
	" read_bit <bit_address> - Read OTP bit value"
);
