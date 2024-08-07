// SPDX-License-Identifier: GPL-2.0
/*
 * eic7700 evb board init
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
#include <net.h>
#include <stdlib.h>
#include <init.h>
#include <spi_flash.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>
#include <u-boot/crc.h>
#include <eic7700_common.h>

#ifdef CONFIG_ESWIN_UMBOX
#include <eswin/eswin-umbox-srvc.h>
#endif

static int get_hardware_board_info(const char *node_name, HardwareBoardInfo_t *gHardware_Board_Info)
{
	HardwareBoardInfo_t gHardware_Board_InfoA;
	HardwareBoardInfo_t gHardware_Board_InfoB;
	struct spi_flash *flash = NULL;
	struct udevice *bus, *dev;
	uint64_t size = 0;
	uint32_t crc32ChecksumA, crc32ChecksumB;
	int ret;
	bool flaga, flagb;
	ret = uclass_get_device_by_name(UCLASS_SPI, node_name, &bus);
	if(ret) {
		return ret;
	}
	ret = spi_find_chip_select(bus, 0, &dev);
	if(ret) {
		printf("Invalid chip select :%d (err=%d)\n", 0, ret);
		return ret;
	}

	if (!device_active(dev)) {
		if(device_probe(dev))
			return -1;
	}
	flash = dev_get_uclass_priv(dev);
	if(!flash) {
		printf("SPI dev_get_uclass_priv failed\n");
		return -1;
	}

	size = sizeof(HardwareBoardInfo_t);
	memset((uint8_t *)&gHardware_Board_InfoA, 0, size);
	memset((uint8_t *)&gHardware_Board_InfoB, 0, size);
	ret = spi_flash_read(flash, HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET, size, (void *)&gHardware_Board_InfoA);
	if(ret) {
		return ret;
	}
	ret = spi_flash_read(flash, HARDWARE_BOARD_INFO_FLASH_BACKUP_OFFSET, size, (void *)&gHardware_Board_InfoB);
	if(ret) {
		return ret;
	}
	crc32ChecksumA = crc32(0xffffffff,  (uint8_t *)&gHardware_Board_InfoA, sizeof(HardwareBoardInfo_t)-4);
	crc32ChecksumB = crc32(0xffffffff,  (uint8_t *)&gHardware_Board_InfoB, sizeof(HardwareBoardInfo_t)-4);
	flaga = crc32ChecksumA != gHardware_Board_InfoA.crc32Checksum || HARDWARE_BOARD_INFO_MAGIC_NUMBER != gHardware_Board_InfoA.magicNumber;
	flagb = crc32ChecksumB != gHardware_Board_InfoB.crc32Checksum || HARDWARE_BOARD_INFO_MAGIC_NUMBER != gHardware_Board_InfoB.magicNumber;
	if(flaga && flagb) {
		printf("ERROR: There is no valid hardware board information!!!\r\n");
		return -1;
	} else if(flaga) {
		memcpy(gHardware_Board_Info, &gHardware_Board_InfoB, size);
	} else if(flagb) {
		memcpy(gHardware_Board_Info, &gHardware_Board_InfoA, size);
	} else {
		if(gHardware_Board_InfoA.updateCount < gHardware_Board_InfoB.updateCount) {
			memcpy(gHardware_Board_Info, &gHardware_Board_InfoB, size);
		} else {
			memcpy(gHardware_Board_Info, &gHardware_Board_InfoA, size);
		}
	}
	return 0;
}
void hardware_info_env_set(void)
{
	uint8_t mac_addr[6];
	const char *node_name_d0 = "spi@51800000";
	const char *node_name_d1 = "spi@71800000";
	HardwareBoardInfo_t gHardware_Board_Info;
	uint64_t size = sizeof(HardwareBoardInfo_t);
	memset((uint8_t *)&gHardware_Board_Info, 0, size);
	if(get_hardware_board_info(node_name_d0, &gHardware_Board_Info)) {
		return 0;
	}
	memset(mac_addr, 0, 6);
	if (!eth_env_get_enetaddr("ethaddr", mac_addr) && is_valid_ethaddr(gHardware_Board_Info.ethernetMAC1)) {
		eth_env_set_enetaddr("ethaddr", gHardware_Board_Info.ethernetMAC1);
	}
	memset(mac_addr, 0, 6);
	if (!eth_env_get_enetaddr("eth1addr", mac_addr) && is_valid_ethaddr(gHardware_Board_Info.ethernetMAC2)) {
		eth_env_set_enetaddr("eth1addr", gHardware_Board_Info.ethernetMAC2);
	}

	char *boardSerialNumber = gHardware_Board_Info.boardSerialNumber;
	printf("boardSerialNumber %s\n",boardSerialNumber);
	if(strstr(boardSerialNumber, "EIDS100AX16")) {
		env_set("fdtfile","eswin/eic7700-evb.dtb");
	}
	else if(strstr(boardSerialNumber, "EIDS200A516")) {
		env_set("fdtfile","eswin/eic7700-evb.dtb");
	}
	else if(strstr(boardSerialNumber, "EIDS200B516")) {
		env_set("fdtfile","eswin/eic7700-evb-a2.dtb");
	}

	memset((uint8_t *)&gHardware_Board_Info, 0, size);
	if(get_hardware_board_info(node_name_d1, &gHardware_Board_Info)) {
		return 0;
	}
	memset(mac_addr, 0, 6);
	if (!eth_env_get_enetaddr("eth2addr", mac_addr) && is_valid_ethaddr(gHardware_Board_Info.ethernetMAC1)) {
		eth_env_set_enetaddr("eth2addr", gHardware_Board_Info.ethernetMAC1);
	}
	memset(mac_addr, 0, 6);
	if (!eth_env_get_enetaddr("eth3addr", mac_addr) && is_valid_ethaddr(gHardware_Board_Info.ethernetMAC2)) {
		eth_env_set_enetaddr("eth3addr", gHardware_Board_Info.ethernetMAC2);
	}
}




int misc_init_r(void)
{
	struct udevice *dev;
	int32_t ret;

#ifdef CONFIG_ESWIN_PMP
	eswin_pmp_init();
#endif

#if defined(CONFIG_ESWIN_SPI)
	es_bootspi_write_protection_init();
#endif

	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);
	hardware_info_env_set();

	if (NULL == env_get("fdtfile")) {
		env_set("fdtfile","eswin/eic7700-evb-a2.dtb");
	}
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
	irq_mux_route();
	return 0;
}

int board_late_init(void)
{
#ifdef CONFIG_ESWIN_UMBOX
	lpcpu_misc_func();
#endif
	return 0;
}
