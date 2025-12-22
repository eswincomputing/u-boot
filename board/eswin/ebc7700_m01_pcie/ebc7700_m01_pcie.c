// SPDX-License-Identifier: GPL-2.0
/*
 * ebc7700 m.2 init
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
#include <spi.h>
#include <spi_flash.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>
#include <dm/pinctrl.h>
#include <u-boot/crc.h>
#include <eic7700_common.h>
#include <eswin/cpu.h>
#include <asm/gpio.h>
#include <fdt_support.h>

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
	printf("Get board info from flash\n");
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

int hardware_info_env_set(void)
{
	uint8_t mac_addr[6];
	const char *node_name = "spi@51800000";
	HardwareBoardInfo_t gHardware_Board_Info;
	uint64_t size = sizeof(HardwareBoardInfo_t);
	memset((uint8_t *)&gHardware_Board_Info, 0, size);
	if(get_hardware_board_info(node_name, &gHardware_Board_Info)) {
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
	else if(strstr(boardSerialNumber, "EIMSE00")) {
		env_set("fdtfile","eswin/eic7700-evb-a3.dtb");
		set_voltage_default();
	}
	return 0;
}

int misc_init_r(void)
{
	//struct udevice *dev;

#ifdef CONFIG_ESWIN_PMP
	eswin_pmp_init();
#endif

#if defined(CONFIG_ESWIN_SPI)
	es_bootspi_write_protection_init();
#endif

	//uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);
	//hardware_info_env_set();

	if (NULL == env_get("fdtfile")) {
		env_set("fdtfile","eswin/ebc7700-m01-pcie.dtb");
	}
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
	 * SPI  bit11~10
	 * DMA  bit9
	 * MPMP bit8
	 * TIMER1 bit5~4
	 * TIMER2 bit3~2
	 * TIMER3 bit1~0
	 *
	 * Route GPIO, TIMER0 to LPCPU
	*/

	val = 0x1040;
	writel(val,(void *)(0x51810000+0x3c0));
}

static void write_reg(uint32_t base, uint32_t mask, uint32_t val)
{
	uint32_t old_val = readl(base);
	uint32_t new_val = (old_val & ~mask) | (val & mask);
	writel(new_val, base);
}

void clk_disable_and_reset_assert(uint32_t syscrg_base)
{
	/* clk_disable_unprepare */
	write_reg(syscrg_base + 0x12c, 0x1u << 31, 0); // gpu aclken
	write_reg(syscrg_base + 0x130, 0x1u << 31, 0); // gpu cfgclken
	write_reg(syscrg_base + 0x134, 0x1u << 31, 0); // gpu grayclken
	write_reg(syscrg_base + 0x138, 0x1u << 31, 0); //dsp aclken
	write_reg(syscrg_base + 0x13c, 0x1u << 31, 0); //dsp cfgclken
	write_reg(syscrg_base + 0x140, 0x1u << 31, 0); //d2d aclken
	write_reg(syscrg_base + 0x144, 0x1u << 0, 0); //d2d clken
	write_reg(syscrg_base + 0x150, 0x1, 0); // sata rbc clken
	write_reg(syscrg_base + 0x154, 0x1u << 31, 0); // sata oob clken
	write_reg(syscrg_base + 0x158, 0x80000001, 0); // eth0 rmii0 ref [31], clk_en [0]
	write_reg(syscrg_base + 0x15c, 0x80000001, 0); // eth1 rmii0 ref [31], clk_en [0]
	write_reg(syscrg_base + 0x160, 0x1u << 16, 0); // emmc coreclk_en
	write_reg(syscrg_base + 0x164, 0x1u << 16, 0); // sdio0 coreclk_en
	write_reg(syscrg_base + 0x168, 0x1u << 16, 0); // sdio1 coreclk_en
	write_reg(syscrg_base + 0x184, 0x1u << 31, 0); // vi dw_clk_en
	write_reg(syscrg_base + 0x188, 0x3 << 30, 0); // vi aclken/cfgclken
	write_reg(syscrg_base + 0x18c, 0x1u << 31, 0); // vi dig isp clken
	write_reg(syscrg_base + 0x190, 0x1u << 31, 0); // vi dvp clken
	write_reg(syscrg_base + 0x194, 0x1u << 31, 0); // vi shutter clken0
	write_reg(syscrg_base + 0x198, 0x1u << 31, 0); // vi shutter clken1
	write_reg(syscrg_base + 0x19c, 0x1u << 31, 0); // vi shutter clken2
	write_reg(syscrg_base + 0x1a0, 0x1u << 31, 0); // vi shutter clken3
	write_reg(syscrg_base + 0x1a4, 0x1u << 31, 0); // vi shutter clken4
	write_reg(syscrg_base + 0x1a8, 0x1u << 31, 0); // vi shutter clken5
	write_reg(syscrg_base + 0x1ac, 0x3, 0); // vi phycfg/txesc clken
	write_reg(syscrg_base + 0x1b0, 0x3u << 30, 0); // vo aclken/cfglcken
	write_reg(syscrg_base + 0x1b4, 0x1u << 31, 0); // vo iesmclk_en
	write_reg(syscrg_base + 0x1b8, 0x1u << 31, 0); // vo pixel_clk_en
	write_reg(syscrg_base + 0x1bc, 0x1u << 31, 0); // vo mclk_clk_en
	write_reg(syscrg_base + 0x1c0, 0x3, 0); // vo phy clk en
	write_reg(syscrg_base + 0x1c4, 0x1u << 31, 0); // vc aclken
	write_reg(syscrg_base + 0x1cc, 0x3u << 30 | 0x1u << 28, 0); // g2d aclken/clken/pclken
	write_reg(syscrg_base + 0x1d0, 0x3f, 0); // ve_pclken/vd_pclken/mon_pclken/je_pclken/jd_pclken/cfg_clken
	write_reg(syscrg_base + 0x1d4, 0x1u << 31, 0); // je clken
	write_reg(syscrg_base + 0x1d8, 0x1u << 31, 0); // jd clken
	write_reg(syscrg_base + 0x1dc, 0x1u << 31, 0); // vd clken
	write_reg(syscrg_base + 0x1e0, 0x1u << 31, 0); // ve clken
	
	/* reset_control_deassert */
	write_reg(syscrg_base + 0x404, 0x1f, 0); // gpu rst
	write_reg(syscrg_base + 0x408, 0xf7, 0); // dsp rst
	write_reg(syscrg_base + 0x428, 0x1, 0); // fan rst
	write_reg(syscrg_base + 0x434, 0x1e, 0); // uart rst
	write_reg(syscrg_base + 0x43c, 0x1, 0); // pwm rst
	write_reg(syscrg_base + 0x440, 0x3, 0); // ssi rst
	write_reg(syscrg_base + 0x444, 0xf, 0); // wdt rst
	write_reg(syscrg_base + 0x458, 0x7, 0); // vc rst
	write_reg(syscrg_base + 0x45c, 0x3, 0); // jd rst
	write_reg(syscrg_base + 0x460, 0x3, 0); // je rst
	write_reg(syscrg_base + 0x464, 0x3, 0); // vd rst
	write_reg(syscrg_base + 0x468, 0x3, 0); // ve rst
	write_reg(syscrg_base + 0x46c, 0x7, 0); // g2d rst
	write_reg(syscrg_base + 0x470, 0x7, 0); // vi rst
	write_reg(syscrg_base + 0x474, 0x1, 0); // dvp rst
	write_reg(syscrg_base + 0x478, 0x1, 0); // isp0 rst
	write_reg(syscrg_base + 0x47c, 0x1, 0); // isp1 rst
	write_reg(syscrg_base + 0x480, 0x1, 0); // shutter rst
	write_reg(syscrg_base + 0x484, 0x3f, 0); // vo_phy rst
	write_reg(syscrg_base + 0x488, 0x3, 0); // i2s rst
	write_reg(syscrg_base + 0x48c, 0xf, 0); // vo rst
	write_reg(syscrg_base + 0x4d0, 0x1, 0); // rtc rst
}

void npu_lowpower(uint32_t syscrg_base)
{
	write_reg(syscrg_base + 0x178, 0xf0, 0x40);
	write_reg(syscrg_base + 0x17c, 0xfff, 0x440);
	write_reg(syscrg_base + 0x180, 0xff, 0x42);
}

int board_init(void)
{
	clk_disable_and_reset_assert(0x51828000);
	npu_lowpower(0x51828000);
	irq_mux_route();
	return 0;
}

int board_late_init(void)
{
	return 0;
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	return eic770x_fdt_fixup_matched_memory_banks(blob, bd);
}
