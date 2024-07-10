// SPDX-License-Identifier: GPL-2.0
/*
 * hifive premier 550 board init
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

#include <common.h>
#include <dm.h>
#include <linux/delay.h>
#include <linux/io.h>
#include "env.h"
#include <errno.h>
#include <blk.h>
#include <mmc.h>
#include <net.h>
#include <dm/uclass.h>
#include <u-boot/crc.h>
#include <i2c.h>
#include <init.h>

typedef struct {
	uint32_t magicNumber;
	uint8_t formatVersionNumber;
	uint16_t productIdentifier;
	uint8_t pcbRevision;
	uint8_t bomRevision;
	uint8_t bomVariant;
	char boardSerialNumber[18]; // 18 bytes of serial number, excluding string terminator
	uint8_t manufacturingTestStatus;
	uint8_t ethernetMAC1[6];	// The MAC of the SOM
	uint8_t ethernetMAC2[6];	// The MAC of the SOM
	uint8_t ethernetMAC3[6];	// The MAC of the MCU
	uint32_t crc32Checksum;
	// uint8_t padding[4];
} __attribute__((packed)) CarrierBoardInfo;
#define AT24C_ADDR (0x50)


#define CBINFO_MAX_SIZE		64
#define GAP_SIZE		16
#define USER_MAX_SIZE		96
/* A, B cbinfo */
#define CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET	0
#define CARRIER_BOARD_INFO_EEPROM_BACKUP_OFFSET	(CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET + CBINFO_MAX_SIZE + GAP_SIZE)

#define MAGIC_NUMBER 0x45505EF1


static int get_carrier_board_info(void)
{
	static CarrierBoardInfo gCarrier_Board_Info;
	uint8_t mac1_addr[6];
	uint8_t mac2_addr[6];
	char boardSerialNumber[19] = {0};
	uint32_t crc32Checksum;

	struct udevice *bus;
	struct udevice *dev;
	int ret;

	ret = uclass_get_device_by_seq(UCLASS_I2C, 11, &bus);
	if (ret) {
		printf("%s: No bus %d\n", __func__, 11);
		return ret;
	}
	ret = i2c_get_chip(bus, AT24C_ADDR, 1, &dev);
	if (!ret)
		ret = i2c_set_chip_offset_len(dev, 1);
	if (ret)
		return -1;

	memset((uint8_t *)&gCarrier_Board_Info, 0, sizeof(CarrierBoardInfo));
	ret = dm_i2c_read(dev, CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET, (uint8_t *)&gCarrier_Board_Info, sizeof(CarrierBoardInfo));
	if (ret != 0) {
		printf("WARN: cannot read ENHSTATUS from resetcontroller!\n");
		return -1;
	}
	crc32Checksum = crc32(0xffffffff, (uint8_t *)&gCarrier_Board_Info, sizeof(CarrierBoardInfo)-4);

	if(gCarrier_Board_Info.magicNumber != MAGIC_NUMBER) {
		memset((uint8_t *)&gCarrier_Board_Info, 0, sizeof(CarrierBoardInfo));
		ret = dm_i2c_read(dev, CARRIER_BOARD_INFO_EEPROM_BACKUP_OFFSET, (uint8_t *)&gCarrier_Board_Info, sizeof(CarrierBoardInfo));
		crc32Checksum = crc32(0xffffffff, (uint8_t *)&gCarrier_Board_Info, sizeof(CarrierBoardInfo)-4);
	}
	if(gCarrier_Board_Info.magicNumber == MAGIC_NUMBER) {
		if (!eth_env_get_enetaddr("ethaddr", mac1_addr) && is_valid_ethaddr(gCarrier_Board_Info.ethernetMAC1)) {
			eth_env_set_enetaddr("ethaddr", gCarrier_Board_Info.ethernetMAC1);
		}
		if (!eth_env_get_enetaddr("eth1addr", mac2_addr) && is_valid_ethaddr(gCarrier_Board_Info.ethernetMAC2)) {
			eth_env_set_enetaddr("eth1addr", gCarrier_Board_Info.ethernetMAC2);
		}
		if (!env_get("board_info")) {
			memcpy(boardSerialNumber, gCarrier_Board_Info.boardSerialNumber, sizeof(gCarrier_Board_Info.boardSerialNumber));
			env_set("board_info", boardSerialNumber);
		}
	}

	return 0;
}

int misc_init_r(void)
{
	struct udevice *dev;

#ifdef CONFIG_ESWIN_PMP
	eswin_pmp_init();
#endif

#if defined(CONFIG_ESWIN_SPI)
	es_bootspi_write_protection_init();
#endif

	get_carrier_board_info();

	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);

	return 0;
}

void irq_mux_route(void)
{
	unsigned int val;

	/* Route all interrupts from default LPCPU/SCPU to MCPU
	 * I2C0 bit16
	 * I2C1 bit15
	 * RTC  bit14~13
	 * GPIO bit12
	 * SPI  bit11~10
	 * DMA  bit9
	 * MPMP bit8
	 * TIMER0 bit7~6
	 * TIMER1 bit5~4
	 * TIMER2 bit3~2
	 * TIMER3 bit1~0
	*/

	val = 0;
	writel(val,0x51810000+0x3c0);
}

int board_init(void)
{
	/* For now nothing to do here. */
	irq_mux_route();
	return 0;
}

int board_late_init(void)
{
	return 0;
}
