// SPDX-License-Identifier: GPL-2.0
/*
 * eic7700 sbc board init
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
#include <eic7700_common.h>
#include <fdt_support.h>

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
	printf("BoardSerialNumber %s\n",boardSerialNumber);
	if(strstr(boardSerialNumber, "EIMSB00")) {
		env_set("fdtfile","eswin/eic7700-sbc-a1.dtb");
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

bool i2c_check_mipi_dsi_panel(void){
	struct udevice *bus;
	struct udevice *dev;
	int ret;
	unsigned int data;

	ret = uclass_get_device_by_seq(UCLASS_I2C, 2, &bus);
	if (ret) {
		printf("%s: No bus %d\n", __func__, 2);
		return false;
	}
	/* panel addr 0x45*/
	ret = i2c_get_chip(bus, 0x45, 1, &dev);
	if(!ret)
	{ 
		ret = i2c_set_chip_offset_len(dev, 1);
		if(!ret)
		{
			/*read raspberry panel id, version 0xde or 0xc3 is right*/
			ret = dm_i2c_read(dev, 0x80, (uchar *)&data, 1);
			if(!ret && ((data & 0xff) == 0xc3 || (data & 0xff) == 0xde))
			{
				printf("mipi dsi panel get success! ret = %d, data= 0x%x\n",ret, data);
				return true;
			}
		}
	}
	return false;
}

int misc_init_r(void)
{
	struct udevice *dev;
	bool mipi_dsi_panel_connected;
	set_voltage_default();

	mipi_dsi_panel_connected = false;
#ifdef CONFIG_ESWIN_PMP
	eswin_pmp_init();
#endif

#if defined(CONFIG_ESWIN_SPI)
	es_bootspi_write_protection_init();
#endif

	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);
	hardware_info_env_set();
	mipi_dsi_panel_connected = i2c_check_mipi_dsi_panel();
	if (NULL == env_get("fdtfile")) {
		env_set("fdtfile","eswin/eic7700-sbc-a1.dtb");
	}
	if(mipi_dsi_panel_connected)
	{
		env_set("fdtfile","eswin/eic7700-sbc-a1-mipi.dtb");
	}else
	{
		env_set("fdtfile","eswin/eic7700-sbc-a1.dtb");
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

int board_init(void)
{
	/* For now nothing to do here. */
	irq_mux_route();
	return 0;
}
#define PD_STATUS0 0X0
#define PD_STATUS1 0X1
#define SRC_PDO_5V 0X2
#define SRC_PDO_9V 0X3
#define SRC_PDO_12V 0X4
#define SRC_PDO_15V 0X5
#define SRC_PDO_18V 0X6
#define SRC_PDO_20V 0X7
#define SRC_PDO 0X8
#define PD_COMMAND 0X9
#define SRC_PDO_DETECTED_FLAG(x) ((x >> 7) & 0x1)
static int current_capacitys[] = {
	500,
	700,
	1000,
	1250,
	1500,
	1750,
	2000,
	2250,
	2500,
	2750,
	3000,
	3250,
	3500,
	4000,
	4500,
	5000};
static int pd_volt_list[] = {
	0,
	5,
	9,
	12,
	15,
	18,
	20};

int request_power_pd(uint busnum, uint chip)
{
	struct udevice *bus;
	int ret, is_qc = 0;
	struct udevice *dev;
	uint cache = 0, max_volt = 0, max_curr = 0, now_volt_index = 0;

	ret = uclass_get_device_by_seq(UCLASS_I2C, busnum, &bus);
	if (ret)
	{
		printf("%s: No bus %d\n", __func__, busnum);
		return ret;
	}

	i2c_get_chip(bus, chip, 1, &dev);
	if (!ret)
		ret = i2c_set_chip_offset_len(dev, 1);
	if (ret)
	{
		printf("%s.%d busnum:%d no chip:%d\n", __func__, __LINE__, busnum, chip);
		return -1;
	}

	ret = dm_i2c_read(dev, PD_STATUS0, (uchar *)&cache, 1);
	now_volt_index = (cache >> 4) & 0xf;
	if (0x7 == now_volt_index)
	{
		max_volt = 2;
		max_curr = 6;
		is_qc = 1;
		printf("boot PD is QC capacity volt:9V,current:2000mA\n");
	}
	else if (0x8 == now_volt_index)
	{
		max_volt = 3;
		max_curr = 4;
		is_qc = 1;
		printf("boot PD is QC capacity volt:12V,current:1500mA\n");
	}
	else
	{
		printf("boot PD capacity volt:%dV,current:%dmA\n",
			   pd_volt_list[(cache >> 4) & 0xf], current_capacitys[cache & 0xf]);
	}
	/* printf("%s.%d PD_STATUS0:0x%x\n", __func__, __LINE__, cache); */
	/* ret = dm_i2c_read(dev, PD_STATUS1, (uchar *)&cache, 1);
	printf("%s.%d PD_STATUS1:0x%x\n", __func__, __LINE__, cache); */
	ret = dm_i2c_read(dev, SRC_PDO_5V, (uchar *)&cache, 1);
	/* printf("SRC_PDO_5V:0x%x,is_enable:%d,current:%dmA\n",
		   cache, SRC_PDO_DETECTED_FLAG(cache),
		   current_capacitys[cache & 0xf]); */
	if (1 == SRC_PDO_DETECTED_FLAG(cache))
	{
		max_volt = 1;
		max_curr = cache & 0xf;
	}
	ret = dm_i2c_read(dev, SRC_PDO_9V, (uchar *)&cache, 1);
	/* printf("SRC_PDO_9V:0x%x,is_enable:%d,current:%dmA\n",
		   cache, SRC_PDO_DETECTED_FLAG(cache),
		   current_capacitys[cache & 0xf]); */
	if (1 == SRC_PDO_DETECTED_FLAG(cache))
	{
		max_volt = 2;
		max_curr = cache & 0xf;
	}
	ret = dm_i2c_read(dev, SRC_PDO_12V, (uchar *)&cache, 1);
	/* printf("SRC_PDO_12V:0x%x,is_enable:%d,current:%dmA\n",
		   cache, SRC_PDO_DETECTED_FLAG(cache),
		   current_capacitys[cache & 0xf]); */
	if (1 == SRC_PDO_DETECTED_FLAG(cache))
	{
		max_volt = 3;
		max_curr = cache & 0xf;
	}
	ret = dm_i2c_read(dev, SRC_PDO_15V, (uchar *)&cache, 1);
	/* printf("SRC_PDO_15V:0x%x,is_enable:%d,current:%dmA\n",
		   cache, SRC_PDO_DETECTED_FLAG(cache),
		   current_capacitys[cache & 0xf]); */
	if (1 == SRC_PDO_DETECTED_FLAG(cache))
	{
		max_volt = 8;
		max_curr = cache & 0xf;
	}
	/* ret = dm_i2c_read(dev, SRC_PDO_18V, (uchar *)&cache, 1);
	printf("SRC_PDO_18V:0x%x,is_enable:%d,current:%dmA\n",
		   cache, SRC_PDO_DETECTED_FLAG(cache),
		   current_capacitys[cache & 0xf]); */
	/* if (1 == SRC_PDO_DETECTED_FLAG(cache))
	{
		max_volt = 9;
		max_curr = cache & 0xf;
	} */
	/* ret = dm_i2c_read(dev, SRC_PDO_20V, (uchar *)&cache, 1);
	printf("SRC_PDO_20V:0x%x,is_enable:%d,current:%dmA\n",
		   cache, SRC_PDO_DETECTED_FLAG(cache),
		   current_capacitys[cache & 0xf]); */
	/* if (1 == SRC_PDO_DETECTED_FLAG(cache))
	{
		max_volt = 10;
		max_curr = cache & 0xf;
	} */

	/* printf("%s.%d set volt:%dV,current:%dmA\n",
		   __func__, __LINE__, max_volt, current_capacitys[max_curr]); */
	/* if (max_volt < 3)
	{
		printf("PD power can't support NPU!!!!!!\n");
	}
	else */
	{
		if (0 == is_qc)
		{
			cache = (max_volt << 4);
			ret = dm_i2c_write(dev, SRC_PDO, (uchar *)&cache, 1);
			cache = 1;
			ret = dm_i2c_write(dev, PD_COMMAND, (uchar *)&cache, 1);
			mdelay(10);
		}
	}

	/* ret = dm_i2c_read(dev, SRC_PDO, (uchar *)&cache, 1);
	printf("%s.%d SRC_PDO:0x%x\n", __func__, __LINE__, cache); */

	ret = dm_i2c_read(dev, PD_STATUS0, (uchar *)&cache, 1);
	now_volt_index = (cache >> 4) & 0xf;
	if (0x7 == now_volt_index)
	{
		printf("now PD is QC capacity volt:9V,current:2000mA,power:1800mW\n");
	}
	else if (0x8 == now_volt_index)
	{
		printf("now PD is QC capacity volt:12V,current:1500mA,power:1800mW\n");
	}
	else
	{
		printf("now PD capacity volt:%dV;current:%dmA;power:%dmW.\n",
			   pd_volt_list[(cache >> 4) & 0xf], current_capacitys[cache & 0xf],
			pd_volt_list[(cache >> 4) & 0xf] * current_capacitys[cache & 0xf]);
	}
	return ret;
}

int board_late_init(void)
{
	request_power_pd(1, 0x8);
	return 0;
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	return fdt_fixup_memory(blob, gd->ram_base, gd->ram_size);
}
