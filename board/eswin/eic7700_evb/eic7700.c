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
#include <mmc.h>
#include <net.h>
#include <stdlib.h>
#include <init.h>

#define EMMC_MAC_SIZE 6
#define DTB_MAC_SIZE 20
#define BOARD_NUM_SIZE 256
#define ITEM_NUM 8
#define MMC_DEV_IFACE     "mmc"
#define VD_DEV_PART       "0#vendor"

#define BOARD_ASSEMBLY_NUMBER_ID 1
#define PCB_VERSION_ID 2
#define LAN0_MAC_ID 3
#define LAN1_MAC_ID 4
#define WIFI_MAC_ID 5
#define BT_MAC_ID 6
#define AON_INT_MUX 0x518103c0
#define AON_I2CX_INT_SEL_MASK (3<<15)

// #define DEBUG
#ifndef DEBUG
#define debug_printf(fmt, args...) do{;}while(0)
#else
#define debug_printf(fmt, args...)	printf(fmt, ##args)
#endif



struct item_t {
        uint32_t id;
        uint32_t offset;
        uint32_t size;
        uint32_t flag;
};

struct vendor_info_t {
        uint32_t magic;
        uint32_t version;
        uint32_t total_item;
        uint32_t free_offset;
        uint32_t free_size;
        struct item_t item[0];
};


static struct blk_desc *mmc_dev_desc;
static struct disk_partition vendor_info_part_info;

int emmc_dev_get(void)
{
	const char *dev_part_str;
	/* addr blk cnt */
	dev_part_str = VD_DEV_PART;
	if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
				&mmc_dev_desc, &vendor_info_part_info, true) < 0) {
		debug_printf("UPDATE: Get information of vendor_info partition failed!\n");
		return -ENOENT;
	}
	return 0;
}

static void *emmc_read_vendor(uint64_t offset, uint64_t size)
{
	uint8_t *buf = NULL;
	uint64_t cnt, emmc_offset;
	int32_t ret;
	emmc_offset = (uint64_t)(offset / vendor_info_part_info.blksz) + vendor_info_part_info.start;
	debug_printf("%s %d emmc_offset %x\n", __func__,__LINE__, emmc_offset - vendor_info_part_info.start);
	cnt = DIV_ROUND_UP(size, vendor_info_part_info.blksz);
	buf = malloc(cnt * vendor_info_part_info.blksz);
	ret = blk_dread(mmc_dev_desc, emmc_offset, cnt, buf);
	if(ret != cnt){
		free(buf);
		debug_printf("emmc vendor read fw_header failed!\n");
		return NULL;
	}
	return (buf);
}

static int get_vd_info(char* dst_addr, uint32_t vender_id)
{
	uint32_t len = 1024;
	struct vendor_info_t *vendor_info;
	vendor_info = (struct vendor_info_t *)emmc_read_vendor(0, len);
	if(vendor_info->magic != 0x44565345)
	{
		debug_printf(" vendor magic err! %s : \" ESVD\" ", (char *)vendor_info->magic);
		return -1;
	}
	debug_printf("\n+++++++++++++++++++++++++++++++++++\n");
	debug_printf("magic %x\n", vendor_info->magic);
	debug_printf("version %x\n", vendor_info->version);
	debug_printf("total_item %x\n", vendor_info->total_item);
	debug_printf("free_offset %x\n", vendor_info->free_offset);
	debug_printf("free_size %x\n", vendor_info->free_size);
	int i = 0;
	for ( i = 0; i < vendor_info->total_item; i++) {
		debug_printf("id %x\n", vendor_info->item[i].id);
		debug_printf("offset %x\n", vendor_info->item[i].offset);
		debug_printf("size %x\n", vendor_info->item[i].size);
		debug_printf("flag %x\n", vendor_info->item[i].flag);
		if (vender_id == vendor_info->item[i].id) {
			if(vender_id > 2 && vendor_info->item[i].size != EMMC_MAC_SIZE)
				return 0;

			debug_printf("%s %d\n", __func__,__LINE__);
			uint8_t *mmc_block = (uint8_t *)emmc_read_vendor(vendor_info->item[i].offset, vendor_info->item[i].size);
			if(!mmc_block)
				return 0;
			debug_printf("%s %d\n", __func__,__LINE__);
			len = vendor_info->item[i].offset - (uint64_t)(vendor_info->item[i].offset / vendor_info_part_info.blksz * vendor_info_part_info.blksz);
			debug_printf("%s %d\n", __func__,__LINE__);
			memcpy(dst_addr, mmc_block+len, vendor_info->item[i].size);
			debug_printf("%s %d\n", __func__,__LINE__);
			len = vendor_info->item[i].size;
			free(mmc_block);
			free(vendor_info);
			return len;
		}
	}
	free(vendor_info);
	return 0;
}

int misc_init_r(void)
{
	uint8_t lan0_mac_addr[DTB_MAC_SIZE];
	uint8_t lan1_mac_addr[DTB_MAC_SIZE];
	uint8_t board_num[BOARD_NUM_SIZE];
	uint32_t len;
	struct udevice *dev;
	int32_t ret;

#ifdef CONFIG_ESWIN_PMP
	eswin_pmp_init();
#endif

#if defined(CONFIG_ESWIN_SPI)
	es_bootspi_write_protection_init();
#endif

	// struct udevice *dev;
	if (!eth_env_get_enetaddr("ethaddr", lan0_mac_addr))
	{
		ret = emmc_dev_get();
		if(ret)
			return 0;
		memset(lan0_mac_addr, 0, DTB_MAC_SIZE);
		len = get_vd_info(lan0_mac_addr, LAN0_MAC_ID);
		if (len == EMMC_MAC_SIZE && is_valid_ethaddr(lan0_mac_addr))
		{
			eth_env_set_enetaddr("ethaddr", lan0_mac_addr);

		} else
			debug_printf("lan0_mac_address err\n");
		memset(lan1_mac_addr, 0, DTB_MAC_SIZE);
		len = get_vd_info(lan1_mac_addr, LAN1_MAC_ID);
		if (len == EMMC_MAC_SIZE && is_valid_ethaddr(lan1_mac_addr))
		{
			eth_env_set_enetaddr("eth1addr", lan1_mac_addr);

		} else{
			debug_printf("lan1_mac_address err\n");

		}
		memset(board_num, 0, BOARD_NUM_SIZE);
		len = get_vd_info(board_num, BOARD_ASSEMBLY_NUMBER_ID);
		if (len != 0)
		{
			env_set("board_info", board_num);

		} else{
			debug_printf("BOARD_ASSEMBLY_NUMBER err\n");
		}

	}
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
	irq_mux_route();
	return 0;
}

int board_late_init(void)
{
	return 0;
}
