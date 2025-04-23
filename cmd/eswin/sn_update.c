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

#include <common.h>
#include <command.h>
#include <dm.h>
#include <i2c.h>
#include <malloc.h>
#include <asm/io.h>
#include <stdint.h>

#ifndef CMD_RET_SUCCESS
#define CMD_RET_SUCCESS 0
#define CMD_RET_FAILURE 1
#define CMD_RET_USAGE	-1
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

#define HARDWARE_BOARD_INFO_MAGIC_NUMBER      0x05454943
#define AT24C_ADDR			      (0x50)
#define CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET 0
#define I2C_BUS_NUM			      5

#define AT24C02C_PAGE_SIZE	    8
#define AT24C02C_PAGE_WRITE_TIME_MS 5

typedef struct {
	uint32_t magicNumber;
	uint8_t formatVersionNumber;
	uint16_t productIdentifier;
	uint8_t pcbRevision;
	uint8_t bomRevision;
	uint8_t bomVariant;
	uint8_t SomSerialNumber[18];
	uint8_t manufacturingTestStatus;

	uint8_t boardSerialNumber[32];
	uint8_t ethernetMAC1[6];
	uint8_t ethernetMAC2[6];
	uint8_t boardName[32];
	uint8_t productModel[32];
	uint8_t hardwareVersion[16];
	uint8_t softwareVersion[16];
	uint8_t stationIdentifier[32];
	uint8_t factoryInit[2];

	uint32_t crc32Checksum;
} BoardInfo;

/**
 * Calculate CRC32 checksum
 */
static uint32_t calculate_crc32(const uint8_t *data, int length)
{
	uint32_t crc = 0xFFFFFFFF;
	int i, j;

	for (i = 0; i < length; i++) {
		crc ^= data[i];
		for (j = 0; j < 8; j++) {
			if (crc & 1)
				crc = (crc >> 1) ^ 0xEDB88320;
			else
				crc = crc >> 1;
		}
	}

	return ~crc;
}

static int at24c_write_bytes(struct udevice *dev, uint32_t addr, uint8_t *buffer, int len)
{
	int ret = 0;
	int write_len, current_addr, remaining;

	remaining = len;
	current_addr = addr;

	while (remaining > 0) {
		write_len =
			MIN(remaining, AT24C02C_PAGE_SIZE - (current_addr % AT24C02C_PAGE_SIZE));

		ret = dm_i2c_write(dev, current_addr, buffer, write_len);
		if (ret != 0) {
			printf("EEPROM write failed at address 0x%02x\n", current_addr);
			return ret;
		}

		udelay(AT24C02C_PAGE_WRITE_TIME_MS * 1000);

		remaining -= write_len;
		current_addr += write_len;
		buffer += write_len;
	}

	return 0;
}

/**
 * do_sn_update - Update SomSerialNumber in EEPROM
 *
 * Command format: sn_update "new_serial_number"
 * Example: sn_update "TELEVPU225150099"
 */
static int do_sn_update(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	BoardInfo board_info;
	struct udevice *bus;
	struct udevice *dev;
	int ret;
	const char *new_serial;

	if (argc != 2) {
		return CMD_RET_USAGE;
	}

	new_serial = argv[1];

	if (strlen(new_serial) >= sizeof(board_info.SomSerialNumber)) {
		printf("Error: Serial number too long (maximum %d characters)\n",
		       (int)sizeof(board_info.SomSerialNumber) - 1);
		return CMD_RET_FAILURE;
	}

	ret = uclass_get_device_by_seq(UCLASS_I2C, I2C_BUS_NUM, &bus);
	if (ret) {
		printf("Error: Unable to get I2C bus %d, error code=%d\n", I2C_BUS_NUM, ret);
		return CMD_RET_FAILURE;
	}

	ret = dm_i2c_probe(bus, AT24C_ADDR, 0, &dev);
	if (ret) {
		printf("Error: Unable to probe I2C device at address 0x%x, error code=%d\n",
		       AT24C_ADDR, ret);
		return CMD_RET_FAILURE;
	}

	ret = i2c_set_chip_offset_len(dev, 1);
	if (ret) {
		printf("Error: Unable to set chip offset length, error code=%d\n", ret);
		return CMD_RET_FAILURE;
	}

	memset((uint8_t *)&board_info, 0, sizeof(BoardInfo));
	ret = dm_i2c_read(dev, CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET, (uint8_t *)&board_info,
			  sizeof(BoardInfo));
	if (ret != 0) {
		printf("Error: Unable to read board information from EEPROM!\n");
		return CMD_RET_FAILURE;
	}

	if (board_info.magicNumber != HARDWARE_BOARD_INFO_MAGIC_NUMBER) {
		printf("Error: Invalid EEPROM data (magic number mismatch: 0x%08X)\n",
		       board_info.magicNumber);
		return CMD_RET_FAILURE;
	}

	printf("Current SomSerialNumber: %s\n", board_info.SomSerialNumber);

	memset(board_info.SomSerialNumber, 0, sizeof(board_info.SomSerialNumber));
	strncpy((char *)board_info.SomSerialNumber, new_serial,
		sizeof(board_info.SomSerialNumber) - 1);

	board_info.crc32Checksum =
		calculate_crc32((uint8_t *)&board_info, sizeof(BoardInfo) - sizeof(uint32_t));

	const int SN_OFFSET = offsetof(BoardInfo, SomSerialNumber);
	const int CRC_OFFSET = offsetof(BoardInfo, crc32Checksum);

	ret = at24c_write_bytes(dev, CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET + SN_OFFSET,
				board_info.SomSerialNumber, sizeof(board_info.SomSerialNumber));
	if (ret != 0) {
		printf("Error: Failed to write SomSerialNumber!\n");
		return CMD_RET_FAILURE;
	}

	ret = at24c_write_bytes(dev, CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET + CRC_OFFSET,
				(uint8_t *)&board_info.crc32Checksum,
				sizeof(board_info.crc32Checksum));
	if (ret != 0) {
		printf("Error: Failed to write CRC32 checksum!\n");
		return CMD_RET_FAILURE;
	}

	udelay(10000);

	memset((uint8_t *)&board_info, 0, sizeof(BoardInfo));
	ret = dm_i2c_read(dev, CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET, (uint8_t *)&board_info,
			  sizeof(BoardInfo));
	if (ret != 0) {
		printf("Error: Unable to read back board information from EEPROM!\n");
		return CMD_RET_FAILURE;
	}

	if (board_info.magicNumber != HARDWARE_BOARD_INFO_MAGIC_NUMBER) {
		printf("Error: Magic number corrupted after write (0x%08X)!\n",
		       board_info.magicNumber);
		return CMD_RET_FAILURE;
	}

	if (strcmp((char *)board_info.SomSerialNumber, new_serial) != 0) {
		printf("Error: Failed to update SomSerialNumber!\n");
		return CMD_RET_FAILURE;
	}

	printf("Successfully updated SomSerialNumber to: %s\n", board_info.SomSerialNumber);
	return CMD_RET_SUCCESS;
}

U_BOOT_CMD(sn_update, 2, 0, do_sn_update, "Update SomSerialNumber in EEPROM",
	   "\"new_serial_number\" - Set new SomSerialNumber\n"
	   "Example: sn_update \"TELEVPU225150099\"");