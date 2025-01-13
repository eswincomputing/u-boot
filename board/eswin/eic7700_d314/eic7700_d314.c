// SPDX-License-Identifier: GPL-2.0
/*
 * eic7700 d314 board init
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
#include <spi.h>
#include <spi_flash.h>
#include <init.h>
#include <asm/gpio.h>
#include <dm/device-internal.h>
#include <dm/pinctrl.h>
#include <eswin/cpu.h>
#ifdef CONFIG_ESWIN_UMBOX
#include <eswin/eswin-umbox-srvc.h>
#endif
#include "eic7700_common.h"

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

typedef struct som_info{
	uint32_t magic;
	uint8_t version;
	uint16_t id;
	uint8_t pcb;
	uint8_t bom_revision;
	uint8_t bom_variant;
	uint8_t sn[18];		// 18 bytes of serial number, excluding string terminator
	uint8_t status;
	uint32_t crc;
} __attribute__((packed)) som_info_t;

#define AT24C_ADDR (0x50)


#define CBINFO_MAX_SIZE		64
#define GAP_SIZE		16
#define USER_MAX_SIZE		96

/* A, B cbinfo */
#define CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET	0
#define CARRIER_BOARD_INFO_EEPROM_BACKUP_OFFSET	(CARRIER_BOARD_INFO_EEPROM_MAIN_OFFSET + CBINFO_MAX_SIZE + GAP_SIZE)

int update_som_info(struct spi_flash *flash, u64 offset, u32 size, const void *buf)
{
	int ret = 0;
	es_flash_region_wp_cfg(flash, (void *)offset, HARDWARE_BOARD_INFO_SIZE, 0);
	ret = spi_flash_erase(flash, offset, HARDWARE_BOARD_INFO_SIZE);
	if(ret) {
		goto out;
	}
	ret = spi_flash_write(flash, offset, size, buf);
	if(ret) {
		goto out;
	}
out:
	es_flash_region_wp_cfg(flash, (void *)offset, HARDWARE_BOARD_INFO_SIZE, 1);
	return ret;
}

static int get_som_info(const char *node_name)
{
	HardwareBoardInfo_t gSom_Board_InfoA;
	HardwareBoardInfo_t gSom_Board_InfoB;
	struct spi_flash *flash = NULL;
	struct udevice *bus, *dev;
	uint64_t size = 0;
	uint32_t crc32ChecksumA, crc32ChecksumB;
	int ret;
	bool valid_flaga, valid_flagb;
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
	memset((uint8_t *)&gSom_Board_InfoA, 0, size);
	memset((uint8_t *)&gSom_Board_InfoB, 0, size);
	printf("Get som info from flash\n");
	ret = spi_flash_read(flash, HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET, size, (void *)&gSom_Board_InfoA);
	if(ret) {
		return ret;
	}
	ret = spi_flash_read(flash, HARDWARE_BOARD_INFO_FLASH_BACKUP_OFFSET, size, (void *)&gSom_Board_InfoB);
	if(ret) {
		return ret;
	}
	crc32ChecksumA = crc32(0xffffffff,  (uint8_t *)&gSom_Board_InfoA, sizeof(HardwareBoardInfo_t)-4);
	crc32ChecksumB = crc32(0xffffffff,  (uint8_t *)&gSom_Board_InfoB, sizeof(HardwareBoardInfo_t)-4);
	valid_flaga = (gSom_Board_InfoA.magicNumber == HARDWARE_BOARD_INFO_MAGIC_NUMBER) && (gSom_Board_InfoA.crc32Checksum == crc32ChecksumA);
	valid_flagb = (gSom_Board_InfoB.magicNumber == HARDWARE_BOARD_INFO_MAGIC_NUMBER) && (gSom_Board_InfoB.crc32Checksum == crc32ChecksumB);
	if (valid_flaga) {
		printf("SOM SerialNumber : %s\r\n", gSom_Board_InfoA.boardSerialNumber);
		update_som_info(flash, HARDWARE_BOARD_INFO_FLASH_BACKUP_OFFSET, size, &gSom_Board_InfoA);
	} else if (!valid_flaga && valid_flagb) {
		printf("SOM SerialNumber : %s\r\n", gSom_Board_InfoB.boardSerialNumber);
		update_som_info(flash, HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET, size, &gSom_Board_InfoB);
	} else if (!valid_flaga && !valid_flagb) {
		printf("ERROR: No valid SOM Board info\r\n");
	}

	return 0;
}

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

	if((gCarrier_Board_Info.magicNumber != HARDWARE_BOARD_INFO_MAGIC_NUMBER) || (gCarrier_Board_Info.crc32Checksum != crc32Checksum)) {
		memset((uint8_t *)&gCarrier_Board_Info, 0, sizeof(CarrierBoardInfo));
		ret = dm_i2c_read(dev, CARRIER_BOARD_INFO_EEPROM_BACKUP_OFFSET, (uint8_t *)&gCarrier_Board_Info, sizeof(CarrierBoardInfo));
		crc32Checksum = crc32(0xffffffff, (uint8_t *)&gCarrier_Board_Info, sizeof(CarrierBoardInfo)-4);
	}
	if((gCarrier_Board_Info.magicNumber == HARDWARE_BOARD_INFO_MAGIC_NUMBER) && (gCarrier_Board_Info.crc32Checksum == crc32Checksum)) {
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
	} else {
		printf("ERROR: No valid Carrier Board info\r\n");
	}

	return 0;
}


int set_voltage_default(void)
{
	ofnode node;
	struct udevice *pinctrl;
	struct gpio_desc desc;

	node = ofnode_path("/config");
	if (!ofnode_valid(node)) {
		pr_err("Can't find /config node!\n");
		return -EINVAL;
	}
	if(uclass_get_device(UCLASS_PINCTRL, 0, &pinctrl)) {
		debug("%s: Cannot find pinctrl device\n", __func__);
		return -EINVAL;
	}
	if(pinctrl_select_state(pinctrl, "default")) {
		printf("Failed to set pinctrl state: %d\n", pinctrl_select_state(pinctrl, "default"));
		return -EINVAL;
	}
	if(gpio_request_by_name_nodev(node, "power-gpios", 0, &desc,
				   GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE)) {
		pr_err("Can't request  \"power-gpios\" !\n");
		return -EINVAL;
	}
	dm_gpio_set_value(&desc, 0);
	return 0;
}

int misc_init_r(void)
{
	struct udevice *dev;

	set_voltage_default();

#ifdef CONFIG_ESWIN_PMP
	eswin_pmp_init();
#endif

#if defined(CONFIG_ESWIN_SPI)
	es_bootspi_write_protection_init();
#endif

	get_carrier_board_info();
	const char *node_name_d0 = "spi@51800000";
	get_som_info(node_name_d0);
	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);

	env_set_ulong("ram_size", (gd->ram_size / 1024 / 1024 / 1024));
	eswin_update_bootargs();
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
	writel(val,(void *)(0x51810000+0x3c0));
}

int board_init(void)
{
	/* For now nothing to do here. */
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
