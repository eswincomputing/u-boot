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
#include <spi.h>
#include <spi_flash.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>
#include <u-boot/crc.h>
#include <fml13v03_common.h>
#include <eswin/cpu.h>
#include <fdt_support.h>
#include <command.h>

static int get_hardware_board_info(const char *node_name, FMLHardwareBoardInfo_t *gHardware_Board_Info)
{
	FMLHardwareBoardInfo_t gHardware_Board_InfoA;
	FMLHardwareBoardInfo_t gHardware_Board_InfoB;
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

	size = sizeof(FMLHardwareBoardInfo_t);
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
	crc32ChecksumA = crc32(0xffffffff,  (uint8_t *)&gHardware_Board_InfoA, sizeof(FMLHardwareBoardInfo_t)-4);
	crc32ChecksumB = crc32(0xffffffff,  (uint8_t *)&gHardware_Board_InfoB, sizeof(FMLHardwareBoardInfo_t)-4);
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
	const char *node_name_d0 = "spi@51800000";
	FMLHardwareBoardInfo_t gHardware_Board_Info;
	uint64_t size = sizeof(FMLHardwareBoardInfo_t);
	memset((uint8_t *)&gHardware_Board_Info, 0, size);
	if(get_hardware_board_info(node_name_d0, &gHardware_Board_Info)) {
		return 0;
	}
	char *boardSerialNumber = gHardware_Board_Info.boardSerialNumber;
	printf("boardSerialNumber %s\n",boardSerialNumber);

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

	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);
	uclass_get_device_by_name(UCLASS_VIDEO, "d1_display-subsystem", &dev);
	hardware_info_env_set();

	if (NULL == env_get("fdtfile")) {
		env_set("fdtfile","eswin/eic7702-deepcomputing-fml13v03.dtb");
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
	 * GPIO bit12
	 * SPI  bit11~10
	 * DMA  bit9
	 * MPMP bit8
	 * TIMER0 bit7~6
	 * TIMER1 bit5~4
	 * TIMER2 bit3~2
	 * TIMER3 bit1~0
	*/

	val = readl((void *)(0x51810000+0x3c0));
	val &= ~(1u << 16);
	writel(val,(void *)(0x51810000+0x3c0));

	val = readl((void *)(0x71810000+0x3c0));
	val &= ~(1u << 12);
	writel(val,(void *)(0x71810000+0x3c0));
}

static int init_pwm_fan(void)
{
	struct udevice* pwm_dev;
	int ret;

	ret = uclass_get_device_by_name(UCLASS_PWM, "pwm@50818000", &pwm_dev);
	if (ret) {
		printf("Failed to probe PWM@50818000: %d\n", ret);
		return ret;
	}

	ret = pwm_set_config(pwm_dev, 0, 100000, 50000);
	if (ret) {
		printf("Failed to configure PWM channel 0: %d\n", ret);
		return ret;
	}

	ret = pwm_set_enable(pwm_dev, 0, true);
	if (ret) {
		printf("Failed to enable PWM channel 0: %d\n", ret);
		return ret;
	}

	return 0;
}

int board_init(void)
{
	irq_mux_route();
	return 0;
}

int board_late_init(void)
{
	int ret;

	irq_mux_route();

	ret = init_pwm_fan();
	if (ret)
		printf("PWM fan initialization failed: %d\n", ret);

	return 0;
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	return eic770x_fdt_fixup_matched_memory_banks(blob, bd);
}

int is_valid_sn(const char* sn)
{
	if (strlen(sn) != 31) return 0;

	if (strncmp(sn, "FML13V03", 8) != 0) return 0;

	if (sn[8] != '-' || sn[13] != '-' || sn[22] != '-') return 0;

	for (int i = 0; i < 31; i++) {
		if (i == 8 || i == 13 || i == 22) continue;
		if (!isalnum((unsigned char)sn[i])) return 0;
	}

	return 1;
}

static int do_factory_write(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t crc32Checksum;
	FMLHardwareBoardInfo_t gHardware_Board_Info;

	if (argc < 2) {
		return CMD_RET_USAGE;
	}

	gHardware_Board_Info.magicNumber = HARDWARE_BOARD_INFO_MAGIC_NUMBER;
	strcpy(gHardware_Board_Info.boardSerialNumber, argv[1]);
	if (0 == is_valid_sn(gHardware_Board_Info.boardSerialNumber)) {
		printf("Invalid SN\n");
		return CMD_RET_USAGE;
	}

	gHardware_Board_Info.crc32Checksum = crc32(0xffffffff,  (uint8_t *)&gHardware_Board_Info, sizeof(FMLHardwareBoardInfo_t)-4);

	memcpy((void *)0x90000000, &gHardware_Board_Info, sizeof(FMLHardwareBoardInfo_t));
	run_command("es_burn vendor 0x90000000", 0);

	return 0;
}

static int do_factory_read(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int i;
	FMLHardwareBoardInfo_t gHardware_Board_Info;
	const char *node_name_d0 = "spi@51800000";

	uint64_t size = sizeof(FMLHardwareBoardInfo_t);
	memset((uint8_t *)&gHardware_Board_Info, 0, size);
	if(get_hardware_board_info(node_name_d0, &gHardware_Board_Info)) {
		return 0;
	}

	printf("SN:");
	for (i = 0; i < MAX_BIN_LEN; i++) {
		if (gHardware_Board_Info.boardSerialNumber[i] == '\0') break;
		putc(gHardware_Board_Info.boardSerialNumber[i]);
	}
	putc('\n');

	return 0;
}


#define HAVE_EMMC	0x1
#define HAVE_SD		0x2
#define HAVE_SSD	0x4

static void check_usb_update(int bootdelay)
{
	int ret;
	unsigned long ts;
	int aupgrade_dev_mask = 0;
	int emmc_option = 0, sd_option = 0, ssd_option = 0, bl_only_option = 0;
	int option_index = 1;
	int upgrade_target = -1;
	char s[512];
	const char *board_name;

	printf("Checking for usbupdate.scr on USB...\n");

	ret = run_command("ext4load usb 0 0x90000000 usbupdate.scr", 0);
	if (ret == 0) {
		env_set("stdout", "vidconsole,serial");
		run_command("cls", 0);
		printf("Found usbupdate.scr, preparing to execute...\n");

		printf("Select upgrade target within %d seconds:\n", bootdelay);
		board_name = env_get("board_name");
		if (board_name) {
			snprintf(s, 512, "test -e usb 0 %s.wic", board_name);
			ret = run_command(s, 0);
			if (0 == ret) {
				ret = run_command("mmc dev 0", 0);
				if (0 == ret)
					aupgrade_dev_mask |= HAVE_EMMC;
				ret = run_command("mmc dev 1", 0);
				if (0 == ret)
					aupgrade_dev_mask |= HAVE_SD;
				ret = run_command("nvme device 0", 0);
				if (0 == ret)
					aupgrade_dev_mask |= HAVE_SSD;

				printf("\n\033[1;33m");
				printf("\nWARNING: Upgrading will FORMAT the selected device and ERASE ALL DATA!\n");
				printf("         Please make sure to BACK UP your important files before proceeding.\n\n");
				printf("\033[0m\n");
				
				if (aupgrade_dev_mask & HAVE_EMMC) {
					emmc_option = option_index++;
					printf("  [%d] eMMC\n", emmc_option);
				}
				if (aupgrade_dev_mask & HAVE_SD) {
					sd_option = option_index++;
					printf("  [%d] SD Card\n", sd_option);
				}
				if (aupgrade_dev_mask & HAVE_SSD) {
					ssd_option = option_index++;
					printf("  [%d] NVMe SSD\n", ssd_option);
				}
			}
		}
		bl_only_option = option_index++;
		printf("  [%d] only upgrade BL\n", bl_only_option);
		printf("  [0] Cancel\n");

		printf("Auto-selecting [0] in     ");

		/*
		 * Check if key already pressed
		 */
		if (tstc()) {	/* we got a key press	*/
			char key = getchar();  /* consume input	*/
			if (key >= '0' && key <= '9') {
				upgrade_target = key - '0';

				printf("\b\b\b\b%2ds ", bootdelay);
				printf("\nYour choice: %c\n", key);
			}
			else
				upgrade_target = -1;
		}

		while ((bootdelay > 0) && (upgrade_target < 0)) {
			printf("\b\b\b\b%2ds ", bootdelay);
			--bootdelay;

			/* delay 1000 ms */
			ts = get_timer(0);
			do {
				if (tstc()) {	/* we got a key press	*/
					char key = getchar(); /* consume input*/
					if (key >= '0' && key <= '9') {
						upgrade_target = key - '0';
						printf("\nYour choice: %c\n", key);
					}
					else {
						upgrade_target = -1;
						printf("\nYour choice: Invalid selection\n");
					}
					bootdelay = 0;
					break;
				}
				udelay(10000);
			} while (upgrade_target < 0 && get_timer(ts) < 1000);
		}

		putc('\n');

		if (upgrade_target == 0) {
			printf("Upgrade canceled.\n");
			return;
		} else if (upgrade_target == emmc_option) {
			printf("Selected eMMC upgrade...\n");
			env_set("upgrade_dev", "mmc 0");
		} else if (upgrade_target == sd_option) {
			printf("Selected SD upgrade...\n");
			env_set("upgrade_dev", "mmc 1");
		} else if (upgrade_target == ssd_option) {
			printf("Selected SSD upgrade...\n");
			env_set("upgrade_dev", "nvme 0");
		} else if (upgrade_target == bl_only_option) {
			printf("Only upgrading bootloader...\n");
			env_set("upgrade_dev", "");
		} else {
			printf("Invalid selection. Upgrade canceled.\n");
			return;
		}

		env_set("updated", "yes");
		run_command("source 0x90000000", 0);
	} else {
		printf("No usbupdate.scr found on USB\n");
	}
}

static int do_check_usb_update(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	char *s;
	int bootdelay;
	s = env_get("bootdelay");
	bootdelay = s ? (int)simple_strtol(s, NULL, 10) : CONFIG_BOOTDELAY;

	check_usb_update(bootdelay);
	return 0;
}

U_BOOT_CMD(check_usb_update, 2, 0, do_check_usb_update,
	   "Check usb update",
	   "");

U_BOOT_CMD(factory_write, 2, 0, do_factory_write,
	   "Factory production command",
	   "");


U_BOOT_CMD(factory_read, 2, 0, do_factory_read,
	   "Factory production command",
	   "");