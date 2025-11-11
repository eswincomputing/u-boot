// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN BURN IMAGE CMD driver
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
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

#include <init.h>
#include <stdlib.h>
#include <common.h>
#include <command.h>
#include <errno.h>
#include <blk.h>
#include <mmc.h>
#include <memalign.h>
#include <update_init.h>
#include <asm/io.h>
#include <system_update.h>
#include <cpu_func.h>
#include <lmb.h>
#include <dm.h>
#include <spi.h>
#include <spi_flash.h>
#include <fs.h>
#include <dm/device-internal.h>
#include <u-boot/crc.h>
#include <eswin/esw_mkfs.h>
#include <eic7700_common.h>
#include "eswin/es_mempool.h"
#include "eswin/es_list.h"
#include <sort.h>

// #define DEBUG
#ifndef DEBUG
#define debug_printf(fmt, args...) do{;}while(0)
#else
#define debug_printf(fmt, args...)	printf(fmt, ##args)
#endif

#define BOOTLOADER_INFO_SIZE SZ_4K
#define FW_HEAD_SIZE SZ_4K

#define LOGO_MAX_LEN_8M 0x80000 // 512k
#define LOGO_MAX_LEN_16M 0x200000 // 2M
#define GZIP_HEAD 0x08088b1f
#define LOGO_WRITE_BLOCK 0x20000

enum fw_offset {
	/* Head */
	FW_HEAD_OFFSET      = 0x0000UL,     /* HEAD 4K */
	/* Used to store infomation such as the space offset and size of each part of the fw */
	BOOTLOAD_INFO_OFFSET      = 0x1000UL,     /* 4K */
	/* Data */
	DDR_OFFSET          = 0x2000UL,     /* 248K */
	D2D_OFFSET          = 0x40000UL,    /* 64K */
	/* FW */
	BOOTCHAIN_OFFSET    = 0x50000UL,    /* 6336K */
	/* 8M FLASH */
	IMG_OFFSET_8M          = 0x680000UL,   /* 512K */ 
	/* 16M FLASH	*/
	IMG_OFFSET_16M          = 0xd00000UL,   /* 2M */
	/* The bootinfo offset and env offset are configured through defconfig. 
	 *	The following are the recommended values.
	 *	ENV_OFFSET FLASH size 8M/16M offset 0x700000UL/0xf00000UL total: 512K
	 *	BOARDINFO_OFFSET FLASH size 8M/16M offset 0x780000UL/0xf80000UL total: 512K
	*/
	ENV_OFFSET          = CONFIG_ENV_OFFSET,   /* 512K: 8M FLASH 0x700000UL; 16M FLASH 0xf00000UL*/
	BOARDINFO_OFFSET    = CONFIG_BOARD_INFO_OFFSET,  /* 512K: 8M FLASH 0x780000UL; 16M FLASH 0xf80000UL*/
};

enum flash_info_type {
	FW_HEAD_ID = 0,
	BOOTLOAD_INFO_ID,
	DDR_ID,
	D2D_ID,
	BOOTCHAIN_ID,
	IMG_ID,
	ENV_ID,
	BOARDINFO_ID
};

typedef struct __attribute__((packed)) flash_info_entry{
    uint32_t typeid;
    uintptr_t addr;
    size_t size;
} flash_info_entry_t;

typedef struct __attribute__((packed)) bootloader_info{
    uint32_t magic;
    uint32_t crc;	/* crc calculation range does not include magic and crc*/
    uint32_t num_entries;  /* Number of entries following this structures */
    flash_info_entry_t entries[0];
}flash_info_t;

flash_info_entry_t flash_entry[] = {
	{.typeid = FW_HEAD_ID, .addr = FW_HEAD_OFFSET},
	{.typeid = BOOTLOAD_INFO_ID, .addr = BOOTLOAD_INFO_OFFSET},
	{.typeid = DDR_ID, .addr = DDR_OFFSET},
	{.typeid = D2D_ID, .addr = D2D_OFFSET},
	{.typeid = BOOTCHAIN_ID, .addr = BOOTCHAIN_OFFSET},
	{.typeid = IMG_ID, .addr = IMG_OFFSET_16M},
	{.typeid = ENV_ID, .addr = ENV_OFFSET},
	{.typeid = BOARDINFO_ID, .addr = BOARDINFO_OFFSET}
};

static int flash_stg = 1;  // emmc : 1; flash : 0
static u32 bootchain_memory_top;
es_list_t bootloader_list;
static es_mempool_t es_pool;
static struct spi_flash *flash = NULL;
static struct blk_desc *mmc_dev_desc;
static struct disk_partition bootchain_part_info;

void print_node(es_list_node_t *node)
{
	firmware_entry_header_t *data =  (firmware_entry_header_t *)node->data;
	printf("Node: type=%x, id=%d, offset=%llx, size=%llx\r\n", node->type, node->id, data->offset, data->size);
}

void firmware_entry_header_print(firmware_entry_header_t *header)
{
	debug_printf("firmware_entry_header:\r\n");
	debug_printf("\t version %x\r\n", header->version);
	debug_printf("\t offset %llx\r\n", header->offset);
	debug_printf("\t size %llx\r\n", header->size);
	debug_printf("\t sign_type %x\r\n", header->sign_type);
	debug_printf("\t key_index %x\r\n", header->key_index);
	debug_printf("\t payload_type %x\r\n", header->payload_type);
}

/******************************emmc*********************************************/

static struct mmc *__init_mmc_device(int dev, bool force_init,
				     enum bus_mode speed_mode)
{
	struct mmc *mmc;
	mmc = find_mmc_device(dev);
	if (!mmc) {
		printf("no mmc device at slot %x\n", dev);
		return NULL;
	}

	if (!mmc_getcd(mmc))
		force_init = true;

	if (force_init)
		mmc->has_init = 0;

	if (IS_ENABLED(CONFIG_MMC_SPEED_MODE_SET))
		mmc->user_speed_mode = speed_mode;

	if (mmc_init(mmc))
		return NULL;

#ifdef CONFIG_BLOCK_CACHE
	struct blk_desc *bd = mmc_get_blk_desc(mmc);
	blkcache_invalidate(bd->uclass_id, bd->devnum);
#endif

	return mmc;
}

static int emmc_dev_get(void)
{
	const char *dev_part_str;
	/* addr blk cnt */
	dev_part_str = BOOTCHAIN_DEV_PART;  /* "bootchain" */

	if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
				&mmc_dev_desc, &bootchain_part_info,true) < 0) {
		printf("UPDATE: Get information of bootchain partition failed!\n");
		return -ENOENT;
	}
	return 0;
}

static int emmc_read_bootchain(uint64_t dst_addr, uint64_t offset, uint64_t size)
{
	void *addr;
	uint64_t cnt, emmc_offset;
	int32_t ret;


	debug_printf("bootchain_part_info start %llx  blksz %lx !\n", bootchain_part_info.start, bootchain_part_info.blksz);
	addr = (void *)(dst_addr);

	emmc_offset = DIV_ROUND_UP(offset, bootchain_part_info.blksz) + bootchain_part_info.start;
	cnt = DIV_ROUND_UP(size, bootchain_part_info.blksz);
	debug_printf("emmc emmc_offset %llx read cnt %llx!\n", emmc_offset, cnt);
	ret = blk_dread(mmc_dev_desc, emmc_offset, cnt, addr);
	if(ret != cnt){
		printf("emmc bootchain read fw_header failed!\n");
		return -ENXIO;
	}
	return 0;
}

static int emmc_write_bootchain(uint64_t src_addr, uint64_t offset, uint64_t size)
{
	void *addr;
	ulong start, write_time;
	uint64_t cnt, emmc_offset;
	int32_t ret;
	const char *dev_part_str;
	// struct disk_partition bootchain_part_info;

	/* addr blk cnt */
	dev_part_str = BOOTCHAIN_DEV_PART;  /* "bootchain" */

	debug_printf(" %s %s \n", MMC_DEV_IFACE, dev_part_str);
	if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
				&mmc_dev_desc, &bootchain_part_info,true) < 0) {
		printf("UPDATE: Get information of bootchain partition failed!\n");
		return -ENOENT;
	}
	addr = (void *)(src_addr);

	// debug_printf(" %s %d,blksz %lx  start %lx\n", __func__, __LINE__, bootchain_part_info.blksz, bootchain_part_info.start);
	emmc_offset = DIV_ROUND_UP(offset, bootchain_part_info.blksz) + bootchain_part_info.start;

	cnt = DIV_ROUND_UP(size, bootchain_part_info.blksz);  /* blkcnt */

	debug_printf("src_addr %llx emmc_offset %llx\n",src_addr, emmc_offset);
	start = get_timer(0);
	ret = blk_dwrite(mmc_dev_desc, emmc_offset, cnt, addr);
	write_time = get_timer(start);
	debug_printf("src_addr %llx emmc_offset %llx, %d blocks written: %s\n",src_addr, emmc_offset, ret, (ret == cnt) ? "OK" : "ERROR");
	debug_printf(" %x %x %x\n", __raw_readl(src_addr), __raw_readl(src_addr+4), __raw_readl(src_addr+8));
	if(ret != cnt){
		printf("UPDATE: emmc bootchain partition write failed!\n");
		return -ENXIO;
	}
	printf("MMC: Write time %lu ms, 0x%lx bytes @ %#x Written: %s\n",
		write_time, (size_t)size, (uint32_t)offset, ret != cnt?"ERROR":"OK");
	return 0;
}

/******************************flash*********************************************/
int es_spi_flash_probe(u32 num)
{
	struct udevice *bus, *dev;
	char *node_name_d0 = "spi@51800000";
	char *node_name_d1 = "spi@71800000";
	char *node_name = NULL;
	int ret;

	if(num == 0)
		node_name = node_name_d0;
	else
		node_name = node_name_d1;
	if (flash)
		spi_flash_free(flash);

	flash = NULL;
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
	return 0;
}

#define BOOTCHAIN_PACKAGE_SIZE (0x100000ul)
static int es_spi_flash_erase(uint64_t offset, uint64_t size)
{
	int ret;
	uint64_t package_blk, total_size, erase_size, currentIndex = 0;

	debug_printf("offset : %llx, size %llx\n",offset, size);
	total_size = DIV_ROUND_UP(size, SZ_4K) * SZ_4K;

	package_blk = DIV_ROUND_UP(total_size, BOOTCHAIN_PACKAGE_SIZE);  /* blkcnt */
	printf("Erase progress: %3d%%:\r", 0);
	for(int i = 0;i < package_blk; i++) {
		if(total_size > BOOTCHAIN_PACKAGE_SIZE)
		{
			erase_size = BOOTCHAIN_PACKAGE_SIZE;
		} else {
			erase_size = total_size;
		}
		total_size -= BOOTCHAIN_PACKAGE_SIZE;
		ret = spi_flash_erase(flash, offset + i*BOOTCHAIN_PACKAGE_SIZE, erase_size);
		currentIndex = (uint64_t)i * 100 / package_blk;
		printf("Erase progress: %3lld%%:", currentIndex);
		for(int col = 0; col < currentIndex/2; col++) {
			printf("%s","+");
		}
		printf("\r");
		if(ret)
			break;
	}
	if(!ret) {
		printf("Erase progress: %3d%%:", 100);
		for(int j = 0; j < 100/2; j ++)
			printf("%s","+");
		printf("\r\n");
	}
	printf("SF: %zu bytes @ %#x Erased: %s\r\n", (size_t)size, (uint32_t)offset,
		   ret ? "ERROR" : "OK");

	return ret == 0 ? 0 : 1;
}

int norflash_read_bootchain(uint64_t dst_addr, uint64_t offset, uint64_t size)
{
	int ret = 1;
	/* Consistency checking */
	if (offset + size > flash->size) {
		printf("ERROR: attempting past flash size (%#x)\r\n",
			   flash->size);
		return 1;
	}

	ret = spi_flash_read(flash, offset, size, (void *)dst_addr);

	printf("SF: %zu bytes @ %#x Read: %s\r\n", (size_t)size, (uint32_t)offset,
		   ret ? "ERROR" : "OK");

	return ret == 0 ? 0 : 1;
}

static int norflash_write_bootchain(uint64_t src_addr, uint64_t offset, uint64_t size)
{
	int ret = 1;
	uint32_t retry_count = 3;
	uint64_t package_blk, total_size, write_size, currentIndex = 0;
	uint32_t crc_raw = crc32(0, (void *)src_addr, size);

	debug_printf("src_addr %llx offset : %llx, size %llx\r\n",src_addr, offset, size);
retry:
	ret = es_spi_flash_erase(offset, size);
	if(ret) {
		return ret;
	}
	package_blk = DIV_ROUND_UP(size, BOOTCHAIN_PACKAGE_SIZE);
	total_size = size;
	printf("\rWrite progress: %3d%%:\r", 0);
	for(int i = 0;i < package_blk; i++) {
		if(total_size > BOOTCHAIN_PACKAGE_SIZE)
		{
			write_size = BOOTCHAIN_PACKAGE_SIZE;
		} else {
			write_size = total_size;
		}
		total_size -= BOOTCHAIN_PACKAGE_SIZE;
		ret = spi_flash_write(flash, offset + i * BOOTCHAIN_PACKAGE_SIZE, write_size,
				      (void *)(src_addr + i * BOOTCHAIN_PACKAGE_SIZE));
		currentIndex = (uint64_t)i * 100 / package_blk;
		printf("Write progress: %3lld%%:", currentIndex);
		for(int col = 0; col < currentIndex/2; col++) {
			printf("%s","+");
		}
		printf("\r");
		if(ret)
			goto out;
	}

	char *cmp_buf = memalign(ARCH_DMA_MINALIGN, size);
	if (cmp_buf) {
		ret = spi_flash_read(flash, offset, size, (void *)cmp_buf);
		if(ret)
			goto out;
		uint32_t crc_flash = crc32(0, cmp_buf, size);
		if(crc_flash != crc_raw) {
			retry_count--;
			ret = -1;
			printf("cmdbuf %x %x %x %x %x\r\n", cmp_buf[0],cmp_buf[1],cmp_buf[2],cmp_buf[3],cmp_buf[4]);
			if(retry_count)
				goto retry;
		}
	} else {
		printf("check crc32 error!!!\r\n");
		ret = -1;
		goto out;
	}

	if(!ret) {
		printf("Write progress: %3d%%:", 100);
		for(int j = 0; j < 100/2; j ++)
			printf("%s","+");
		printf("\r\n");
	}
out:
	printf("SF: 0x%lx bytes @ %#x Written: %s\r\n",
		(size_t)size, (uint32_t)offset, ret?"ERROR":"OK");
	free(cmp_buf);

	return ret == 0 ? 0 : 1;
}

static int es_read_bootchain(uint64_t dst_addr, uint64_t offset, uint64_t size)
{
	if(flash_stg)
		emmc_read_bootchain(dst_addr, offset, size);
	else
		norflash_read_bootchain(dst_addr, offset, size);
	return 0;
}

static int es_write_bootchain(uint64_t src_addr, uint64_t offset, uint64_t size)
{
	/* Consistency checking */
	if (offset + size > bootchain_memory_top) {
		printf("ERROR: attempting past flash size (%#x)\n",
			bootchain_memory_top);
		return -1;
	}
	if(flash_stg)
		return emmc_write_bootchain(src_addr, offset, size);
	else {
		return norflash_write_bootchain(src_addr, offset, size);
	}
	return 0;
}



static int esburn_init_load_addr(uint64_t addr, uint64_t size)
{
#ifdef CONFIG_LMB
	struct lmb lmb;
	phys_size_t max_size;

	lmb_init_and_reserve(&lmb, gd->bd, (void *)gd->fdt_blob);

	max_size = lmb_get_free_size(&lmb, addr);
	if (!max_size || max_size < size)
		return -1;
#endif
	return 0;
}

void defrag_move_cb(void *user_data, const defrag_info_t *info)
{
	uint8_t *fw_data = NULL;
	printf("Defrag move: 0x%08lx => 0x%08lx, size=%zu\n",
		info->src_addr,info->dst_addr,info->size);

	/* Move the fw from the source address to dstination address*/
	fw_data = malloc_cache_aligned(info->size);
	es_read_bootchain((uint64_t)fw_data, info->src_addr, info->size);
	es_write_bootchain((uint64_t)fw_data, info->dst_addr, info->size);
	
	/* update the bootloader info list*/
	es_list_foreach(&bootloader_list, node) {
		firmware_entry_header_t *entry = (firmware_entry_header_t *)node->data;
		entry->offset = info->dst_addr;
	}

	free(fw_data);
}

int compare_by_addr(const void *a, const void *b)
{
	const flash_info_entry_t *entry_a = (const flash_info_entry_t *)a;
	const flash_info_entry_t *entry_b = (const flash_info_entry_t *)b;

	if (entry_a->addr < entry_b->addr) return -1;
	if (entry_a->addr > entry_b->addr) return 1;
	return 0;
}

static int check_boardloader_info(void)
{
	int32_t size;
	uint64_t len = BOOTLOADER_INFO_SIZE;

	flash_info_t *src_flash_info = malloc_cache_aligned(len);
	uint32_t count = sizeof(flash_entry) / sizeof(flash_info_entry_t);
	src_flash_info->magic = FHT_MAGIC;
	src_flash_info->num_entries = count;

	memcpy(src_flash_info->entries, flash_entry, sizeof(flash_entry));
	size = sizeof(src_flash_info->num_entries) + sizeof(flash_info_entry_t) * src_flash_info->num_entries;
	uint32_t crc_src = crc32(0, (void*)&src_flash_info->num_entries, size);
	src_flash_info->crc = crc_src;

	/* 
	printf("SOURCE FLASH INFO : magic %x, num %d crc %x\n", src_flash_info->magic, src_flash_info->num_entries, crc_src);	printf("SOURCE FLASH size : num_entries %d, magic %d\n", sizeof(src_flash_info->num_entries), sizeof(src_flash_info->magic));
	for (int i = 0; i < count; i++)
		printf("Entry %d: typeid=%u addr=0x%lx siz=0x%zx\n", 
	 		i, src_flash_info->entries[i].typeid, src_flash_info->entries[i].addr, src_flash_info->entries[i].size);
	*/

	flash_info_t *dst_flash_info = malloc_cache_aligned(len);
	if(dst_flash_info == NULL)
		return -1;
	es_read_bootchain((uint64_t)dst_flash_info, BOOTLOAD_INFO_OFFSET, len);
	if(FHT_MAGIC != dst_flash_info->magic || dst_flash_info->num_entries == 0){
		goto update;
	}

	size = sizeof(dst_flash_info->num_entries) + sizeof(flash_info_entry_t) * dst_flash_info->num_entries;
	uint32_t crc_dst = crc32(0, (void*)&dst_flash_info->num_entries, size);

	/*
	printf("DST FLASH INFO : magic %x, num %d crc %x\n", dst_flash_info->magic, dst_flash_info->num_entries, crc_src);
	printf("DST FLASH size : num_entries %d, magic %d\n", sizeof(dst_flash_info->num_entries), sizeof(dst_flash_info->magic));
	for (int i = 0; i < count; i++)
		printf("Entry %d: typeid=%u addr=0x%lx siz=0x%zx\n", \
	 		i, dst_flash_info->entries[i].typeid, dst_flash_info->entries[i].addr, dst_flash_info->entries[i].size);
	*/

	if(crc_dst != dst_flash_info->crc || src_flash_info->crc != dst_flash_info->crc)
		goto update;

	goto out;

update:
	es_write_bootchain((uint64_t)src_flash_info, BOOTLOAD_INFO_OFFSET, len);
out:
	free(src_flash_info);
	free(dst_flash_info);
	return 0;
}


void update_flash_entry(uint64_t flash_size, uint64_t img_offset)
{
	uint32_t count = sizeof(flash_entry) / sizeof(flash_info_entry_t);
	for (int i = 0; i < count; i++) {
		if (flash_entry[i].typeid == IMG_ID)
			flash_entry[i].addr = img_offset;
	}

	qsort(flash_entry, count, sizeof(flash_info_entry_t), compare_by_addr);

	for (int i = 0; i < count; i++) {
		if ( i != count -1 )
			flash_entry[i].size = flash_entry[i + 1].addr - flash_entry[i].addr;
		else
			flash_entry[i].size = flash_size - flash_entry[i].addr;
	}
	check_boardloader_info();
}

void flash_memory_init(void)
{
	uint64_t size, img_offset;
	if(flash_stg)
		size = SZ_16M;
	else
		size = flash->size;

	if (es_mem_pool_init(&es_pool, 0, flash->size, SZ_4K, SZ_4K / sizeof(firmware_entry_header_t) -1) != 0) {
		printf("Failed to init mem pool\n");
		return;
	}

	if( size > SZ_8M)
		img_offset = IMG_OFFSET_16M;
	else
		img_offset = IMG_OFFSET_8M;
	bootchain_memory_top = img_offset;

	update_flash_entry(size, img_offset);
	
	uint32_t count;
	count = sizeof(flash_entry) / sizeof(flash_info_entry_t);
	for (int i = 0; i < count; i++) {
		if (flash_entry[i].typeid != BOOTCHAIN_ID)
    		es_mem_pool_reserve(&es_pool, flash_entry[i].addr, flash_entry[i].size);
	}

	// es_mem_pool_dump(&es_pool);
}

void flash_memory_free(void)
{
    es_mem_pool_destroy(&es_pool);
}

static int get_bootchain_info(void)
{
	firmware_header_t *fht = NULL;
	firmware_entry_header_t *feht;
	uint32_t ret = 0;
	uint32_t num_entries;
	uint64_t len;

	/* get firmware header info from emmc*/
	len = FW_HEAD_SIZE;
	fht = malloc_cache_aligned(len);
	if(fht == NULL)
		return -1;
	memset(fht, 0, len);

	es_read_bootchain((uint64_t)fht, FW_HEAD_OFFSET, len);
	if(FHT_MAGIC != fht->magic || fht->num_entries == 0){
		ret = -1;
		goto out;
    }

	num_entries = fht->num_entries;
	debug_printf("%s: num_entries %d\r\n",__func__, num_entries);
 	
	for(int i = 0; i < num_entries; i++)
	{
		feht = (firmware_entry_header_t *) &fht->entries[i];
		firmware_entry_header_t *entry_head = malloc_cache_aligned(sizeof(firmware_entry_header_t));
		memcpy(entry_head, &fht->entries[i], sizeof(firmware_entry_header_t));
		es_list_add(&bootloader_list, feht->payload_type, 0, entry_head);
    	es_mem_pool_alloc_at(&es_pool, entry_head->offset, entry_head->size);
	}
	// es_list_traverse(&bootloader_list, print_node);
	// es_mem_pool_dump(&es_pool);
	
out:
	free(fht);
	fht = NULL;
	return ret;
}

static int do_bootchain_write(int argc, char *const argv[])
{

	uint8_t payload_type, cnt = 0;
	int32_t ret = 0;
	int32_t update_flag = 0;
	uint32_t num_entries;
	uint64_t fw_addr, dst_addr, src_addr, size;
	void *ptr = NULL;
	firmware_entry_header_t *feht;
	firmware_header_t *fht = NULL;
	firmware_header_t *fw_fht;
	es_list_node_t *firmware_info = NULL;

	fw_addr = simple_strtoul(argv[1], NULL, 16);
	debug_printf("fw_addr 0x%llx\r\n", fw_addr);
	if(argc < 3) {
		printf("arguments : flash_stg type error!\r\n");
		return -ENOENT;
	} else if (strcmp(argv[2], "emmc") == 0) {
		flash_stg = 1;
		ret = emmc_dev_get();
		if(ret < 0)
			return -ENOENT;
	}else if (strcmp(argv[2], "flash") == 0){
		if (argc < 4)
			ret = es_spi_flash_probe(0);
		else if (strcmp(argv[3], "1") == 0) {
			ret = es_spi_flash_probe(1);
		} else
			ret = es_spi_flash_probe(0);
		if(ret < 0)
			return -ENOENT;
		flash_stg = 0;
	}else{
		printf("arguments : flash_stg type error!\r\n");
		return -ENXIO;
	}

	fw_fht = (firmware_header_t *) fw_addr;
	if(FHT_MAGIC != fw_fht->magic){
		printf("FHT magic should be %x, but %x!\r\n",
				FHT_MAGIC, fw_fht->magic);
		return -ENODATA;
	}

	num_entries = fw_fht->num_entries;
	if(num_entries == 0){
		printf("fw file error(entries is 0)!\r\n");
		ret = -ENXIO;
		goto out;
	}

	if (!flash_stg)
		es_bootspi_wp_cfg(flash, 0);


	/* bootchain firmware list and  flash memory pool init */
	es_list_init(&bootloader_list);
	flash_memory_init();

	/* get bootchain info from memory device */
	update_flag = get_bootchain_info();

	/* If there is an available bootloader in the flash, only the existing type can be updated, 
		and it is impossible to add a new firmware type.*/
	if(!update_flag) {
		size = 0;
		for(int i = 0; i < num_entries; i++) {
			feht = (firmware_entry_header_t *) &fw_fht->entries[i];
			payload_type = feht->payload_type;
			size += feht->size;
			firmware_info = (es_list_node_t *)es_list_find(&bootloader_list, payload_type, 0);
			if(!firmware_info) {
				printf("UPDATE BOOTLOADER FAILED (Invalid image file type)\r\n");
				ret = -ENXIO;
				goto out;
			}
			firmware_entry_header_t *data = firmware_info->data;
			es_mem_pool_free(&es_pool, (void *)data->offset);
		}
		if (size > es_mem_pool_count_free_pages(&es_pool) * es_pool.page_size) {
			printf("Not enough memory to update bootchain\n");
			ret = -ENXIO;
			goto out;
		}
	}

	// es_list_traverse(&bootloader_list, print_node);
	// es_mem_pool_dump(&es_pool);
	for(int i = 0; i < num_entries; i++) {

		feht = (firmware_entry_header_t *) &fw_fht->entries[i];
		src_addr = feht->offset + fw_addr;
		size = feht->size + SIGNATURE_SIZE;
		payload_type = feht->payload_type;


		ret = es_mem_pool_try_alloc(&es_pool, size, &ptr);
		if  (ret == ES_MEMPOOL_ERR_FRAGMENTED) {
			printf("Wait a moment. The malloc memory operation failed. Fragmentation cleanup is underway.\n");
			es_mem_pool_defrag(&es_pool, defrag_move_cb, NULL);
			ret = es_mem_pool_try_alloc(&es_pool, size, &ptr);
			if (ret != ES_MEMPOOL_SUCCESS) {
				printf("Allocation failed permanently (not enough memory)\n");
				ret = -ENXIO;
				goto out;
			}
		} else if (ret != ES_MEMPOOL_SUCCESS){
			printf("Allocation failed permanently (not enough memory)\n");
			ret = -ENXIO;
			goto out;
		}
	
		firmware_entry_header_t *entry_head = malloc_cache_aligned(sizeof(firmware_entry_header_t));
		memcpy(entry_head, &feht->version, sizeof(firmware_entry_header_t));
		dst_addr = (uint64_t)ptr;
		entry_head->offset = dst_addr;
		entry_head->size = feht->size;
		es_list_update(&bootloader_list, payload_type, 0, entry_head);

		switch(payload_type){
			case PUBKEY_RSA:
				printf("PUBKEY_RSA writing...\r\n");
				break;
			case PUBKEY_ECC:
				printf("PUBKEY_ECC writing...\r\n");
				break;
			case DDR:
				printf("DDR writing...\r\n");
				break;
			case D2D:
				printf("D2D writing...\r\n");
				break;
			case BOOTLOADER:
				printf("BOOTLOADER writing...\r\n");
				break;
			case FIRMWARE:
				printf("FIRMWARE writing...\r\n");
				break;
			default:
				printf("Invalid image file type!\r\n");
				ret = -ENXIO;
				goto out;
		}
		ret = es_write_bootchain(src_addr, dst_addr, size);
		if(ret)
			goto out;
		// es_list_traverse(&bootloader_list, print_node);
		// es_mem_pool_dump(&es_pool);
	}

	size = FW_HEAD_SIZE;
	fht = malloc_cache_aligned(size);
	if(!fht) {
		ret = -1;
		goto out;
	}
	memset(fht, 0, size);
	fht->magic = FHT_MAGIC;
	fht->num_entries = es_pool.alloc_cnt;
	es_list_foreach(&bootloader_list, node) {
		firmware_entry_header_t *entry = (firmware_entry_header_t *)node->data;
		memcpy(&fht->entries[cnt++].version, entry, sizeof(firmware_entry_header_t)); 
	}

	printf("BOOTCHAIN HEAD writing...\r\n");
	ret = es_write_bootchain((uint64_t)&fht->magic, FW_HEAD_OFFSET, size);
	if(ret)
		goto out;
	printf("bootloader write OK\r\n");
out:
	if (!flash_stg)
		es_bootspi_wp_cfg(flash, 1);
	if(fht)
		free(fht);
	es_list_clear(&bootloader_list);
	flash_memory_free();
	return ret;
}


static int do_bootchain_erase(int argc, char *const argv[])
{
	int32_t ret;

	uint64_t offset = FW_HEAD_OFFSET;
	uint64_t len = SZ_4K;

	if (argc < 2){
		return -1;
	}
	if (strcmp(argv[1], "emmc") == 0) {
		flash_stg = 1;
		ret = emmc_dev_get();
		if(ret < 0)
			return -ENOENT;
		
		printf("Erase the BOOTCHAIN from the eMMC...\r\n");
		uint8_t *fw_head = malloc_cache_aligned(len);
		memset(fw_head, 0, len);
		emmc_write_bootchain((uint64_t)fw_head, offset, len);
	}else if (strcmp(argv[1], "flash") == 0){
		if (argc < 3)
			ret = es_spi_flash_probe(0);
		else if (strcmp(argv[2], "1") == 0)
			ret = es_spi_flash_probe(1);
		else
			ret = es_spi_flash_probe(0);
		if(ret < 0)
			return -ENOENT;
		printf("Erase the BOOTCHAIN from the flash...\r\n");
		es_bootspi_wp_cfg(flash, 0);
		es_spi_flash_erase(offset, len);
		es_bootspi_wp_cfg(flash, 1);
	}else{
		printf("arguments : flash_stg type error!\r\n");
		return -ENXIO;
	}

	return 0;
}

static int do_boot_write(int argc, char *const argv[])
{

	uint64_t addr, size, cnt;
	uint64_t package_blk, last_blk = 0, cycle_index, currentIndex = 0;
	int32_t ret = 0;
	const char *dev_part_str;
	struct disk_partition rootfs_part_info;

	if (argc < 3)
		return -ENXIO;

	addr = simple_strtoul(argv[1], NULL, 16);
	size = simple_strtoul(argv[2], NULL, 16);

	if(esburn_init_load_addr(addr, size)) {
		puts("\nes_burn error: ");
		puts("trying to overwrite reserved memory...\n");
		return -ENXIO;
	}

	if (strcmp(argv[3], "emmc") == 0) {
		flash_stg = 1;
	}else{
		printf("arguments : The rootfs can only be written to emmc!\r\n");
		return -ENXIO;
	}

	/* addr blk cnt */
	dev_part_str = UPDATE_BOOT_DEV_PART;  /* "rootfsa" */

	if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
				&mmc_dev_desc, &rootfs_part_info,true) < 0) {
		printf("UPDATE: Get information of rootfs partition failed!\r\n");
		return -ENOENT;
	}

	cnt = DIV_ROUND_UP(size, rootfs_part_info.blksz);  /* blkcnt */
	if(cnt % 100) {
		package_blk = cnt/100;
		last_blk = package_blk + cnt % 100;
		cycle_index = 99;
	}
	else {
		package_blk = cnt/100;
		cycle_index = 100;
	}
	printf("Write progress: %3d%%:\r", 0);
	for(int i = 0;i < cycle_index; i++) {
		currentIndex = i / 2;
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + i * package_blk, package_blk, (void __iomem *)(addr + i * package_blk * rootfs_part_info.blksz));
		if(ret != package_blk) {
			printf("Error: rootfs write failed!\n");
			return -ENXIO;
		}
		printf("Write progress: %3d%%:", i);
		for(int col = 0; col < currentIndex; col++) {
			printf("%s","+");
		}
		printf("\r");
	}
	if(last_blk)
	{
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + cycle_index * package_blk, last_blk, (void __iomem *)(addr + cycle_index * package_blk * rootfs_part_info.blksz));
		if(ret != last_blk) {
			printf("Error: rootfs write failed!\n");
			return -ENXIO;
		}

	}
	printf("Write progress: %3d%%:", 100);
	for(int j = 0; j < 100/2; j ++)
		printf("%s","+");
	printf("\r\n");
	printf("boot has been successfully writen in %s\r\n", dev_part_str);
	return 0;

}

static int do_root_write(int argc, char *const argv[])
{

	uint64_t addr, size, cnt;
	uint64_t package_blk, last_blk = 0, cycle_index, currentIndex = 0;
	int32_t ret = 0;
	const char *dev_part_str;
	struct disk_partition rootfs_part_info;

	if (argc < 3)
		return -ENXIO;

	addr = simple_strtoul(argv[1], NULL, 16);
	size = simple_strtoul(argv[2], NULL, 16);

	if(esburn_init_load_addr(addr, size)) {
		puts("\nes_burn error: ");
		puts("trying to overwrite reserved memory...\n");
		return -ENXIO;
	}

	if (strcmp(argv[3], "emmc") == 0) {
		flash_stg = 1;
	}else{
		printf("arguments : The rootfs can only be written to emmc!\r\n");
		return -ENXIO;
	}

	/* addr blk cnt */
	dev_part_str = UPDATE_ROOT_DEV_PART;  /* "root" */

	if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
				&mmc_dev_desc, &rootfs_part_info,true) < 0) {
		printf("UPDATE: Get information of rootfs partition failed!\r\n");
		return -ENOENT;
	}

	cnt = DIV_ROUND_UP(size, rootfs_part_info.blksz);  /* blkcnt */
	if(cnt % 100) {
		package_blk = cnt/100;
		last_blk = package_blk + cnt % 100;
		cycle_index = 99;
	}
	else {
		package_blk = cnt/100;
		cycle_index = 100;
	}
	printf("Write progress: %3d%%:\r", 0);

	for(int i = 0;i < cycle_index; i++) {
		currentIndex = i / 2;
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + i * package_blk, package_blk, (void __iomem *)(addr + i * package_blk * rootfs_part_info.blksz));
		if(ret != package_blk) {
			printf("Error: rootfs write failed!\n");
			return -ENXIO;
		}

		printf("Write progress: %3d%%:", i);
		for(int col = 0; col < currentIndex; col++) {
			printf("%s","+");
		}
		printf("\r");
	}
	if(last_blk)
	{
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + cycle_index * package_blk, last_blk, (void __iomem *)(addr + cycle_index * package_blk * rootfs_part_info.blksz));
		if(ret != last_blk) {
			printf("Error: rootfs write failed!\n");
			return -ENXIO;
		}
	}
	printf("Write progress: %3d%%:", 100);
	for(int j = 0; j < 100/2; j ++)
		printf("%s","+");
	printf("\r\n");

	printf("root has been successfully writen in %s\r\n", dev_part_str);
	return 0;
}
static int do_vendor_write(int argc, char *const argv[])
{
	int32_t ret = 0;
	uint64_t size = 5*1024;
	uint64_t fw_addr = simple_strtoul(argv[1], NULL, 16);
	debug_printf("fw_addr 0x%llx\r\n", fw_addr);

	if (argc < 3)
		ret = es_spi_flash_probe(0);
	else if (strcmp(argv[2], "1") == 0)
		ret = es_spi_flash_probe(1);
	else
		ret = es_spi_flash_probe(0);
	if(ret < 0)
		return -ENOENT;

	HardwareBoardInfo_t *gHardware_Board_Info = (HardwareBoardInfo_t *) fw_addr;
	printf("gHardware_Board_Info->magicNumber %x\n",gHardware_Board_Info->magicNumber);
	if(gHardware_Board_Info->magicNumber != HARDWARE_BOARD_INFO_MAGIC_NUMBER)
		return -ENOENT;

	es_bootspi_wp_cfg(flash, 0);
	ret = norflash_write_bootchain((uint64_t)&gHardware_Board_Info->magicNumber, HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET, size);
	if(ret)
		goto out;
	ret = norflash_write_bootchain((uint64_t)&gHardware_Board_Info->magicNumber, HARDWARE_BOARD_INFO_FLASH_BACKUP_OFFSET, size);
	if(ret)
		goto out;
	es_bootspi_wp_cfg(flash, 1);
	printf("vendor info write OK\r\n");
out:
	return ret;
}

static int do_mmc_write(int argc, char *const argv[])
{
	struct mmc *mmc;
	u64 blk = 0, cnt, n;
	int curr_device;
	void *addr;
	uint64_t package_blk, last_blk = 0, cycle_index = 100, currentIndex = 0;

	if (argc != 3)
		return CMD_RET_USAGE;
	curr_device = 0;
	addr = (void *)hextoul(argv[1], NULL);
	cnt = hextoul(argv[2], NULL);
	mmc = __init_mmc_device(curr_device, false, MMC_MODES_END);
	if (!mmc)
		return CMD_RET_FAILURE;

	if (esburn_init_load_addr((uintptr_t)addr, cnt)) {
		puts("\nes_burn error: ");
		puts("trying to overwrite reserved memory...\n");
		return -ENXIO;
	}

	cnt = DIV_ROUND_UP(cnt, mmc->write_bl_len);  /* blkcnt */
	if (mmc_getwp(mmc) == 1) {
		printf("Error: card is write protected!\n");
		return CMD_RET_FAILURE;
	}

	if(cnt % 100) {
		package_blk = cnt/100;
		last_blk = cnt % 100;
	}
	else {
		package_blk = cnt/100;
	}
	printf("Write progress: %3d%%:\r", 0);
	for(int i = 0;i < cycle_index; i++) {
		n = blk_dwrite(mmc_get_blk_desc(mmc), blk + i * package_blk, package_blk,
				(void __iomem *)(addr + i * package_blk * mmc->write_bl_len));
		if(n != package_blk){
			return CMD_RET_FAILURE;
		}
		if ((i == cycle_index -1) && last_blk) {
			n = blk_dwrite(mmc_get_blk_desc(mmc), blk + (i + 1) * package_blk, last_blk,
							(void __iomem *)(addr + (i + 1) * package_blk * mmc->write_bl_len));
			if(n != last_blk){
				return CMD_RET_FAILURE;
			}
		}
		printf("Write progress: %3d%%:", i);
		for(int col = 0; col < i/2; col++) {
			printf("%s","+");
		}
		printf("\r");
	}
	printf("Write progress: %3lld%%:", currentIndex);
	for(int col = 0; col < currentIndex/2; col++) {
		printf("%s","+");
	}
	printf("\r");
	printf("\nMMC dev # %d, %lld blocks written: OK\n", curr_device, n);
	return CMD_RET_SUCCESS;
}

int es_bootchain_logo_write(int argc, char *const argv[], bool uboot_flag)
{
    u32 offset, len, ret, currentIndex = 0;
	u32 start_x = GZIP_HEAD, start_y = GZIP_HEAD;
    u64 src_addr = simple_strtoul(argv[1], NULL, 16);
	if (*(u32 *)src_addr != GZIP_HEAD) {
		printf("ERROR: logo data format is not gzip!\n");
		return CMD_RET_FAILURE;
	}
	ret = es_spi_flash_probe(0);
    if (ret) {
        printf("ERROR: Failed to probe SPI flash\n");
        return CMD_RET_FAILURE;
    }

	offset = (flash->size == 0x800000) ? IMG_OFFSET_8M : IMG_OFFSET_16M;
	len = (flash->size == 0x800000) ? LOGO_MAX_LEN_8M : LOGO_MAX_LEN_16M;

	if (!uboot_flag) {
		len /= 4;
		offset += len * 3;
		if (argc == 4) {
			start_x = simple_strtoul(argv[2], NULL, 10);
			start_y = simple_strtoul(argv[3], NULL, 10);
		}
	} else {
		len = len / 4 * 3;
	}

	ret = es_bootspi_wp_cfg(flash, 0);
	if (ret) {
        printf("ERROR: Failed to disEnable bootspi WP\n");
		return CMD_RET_FAILURE;
    }

	ret = es_spi_flash_erase(offset, len);
    if (ret) {
        printf("ERROR: SPI flash erase failed\n");
		return CMD_RET_FAILURE;
    }

	u32 total_size = uboot_flag ? len : len - 8;
	u32 write_cnt = DIV_ROUND_UP(total_size, LOGO_WRITE_BLOCK);
	u32 write_len = LOGO_WRITE_BLOCK;
	printf("\rWrite progress: %3d%%:\r", 0);
	for (int i = 0; i < write_cnt; i++) {
		if (i == write_cnt - 1 && total_size % LOGO_WRITE_BLOCK != 0) {
			write_len = total_size % LOGO_WRITE_BLOCK;
		}
		ret = spi_flash_write(flash, offset + i * LOGO_WRITE_BLOCK,
							  write_len, ((void *)src_addr + i * LOGO_WRITE_BLOCK));
		currentIndex = (uint64_t)i * 100 / write_cnt;
		printf("Write progress: %3d%%:", currentIndex);
		for(int col = 0; col < currentIndex / 2; col++) {
			printf("%s", "+");
		}
		printf("\r");
		if (ret) {
			printf("ERROR: SPI flash write failed\n");
			break;
		}
	}
	if(!ret) {
		printf("Write progress: %3d%%:", 100);
		for(int j = 0; j < 100 / 2; j ++)
			printf("%s", "+");
		printf("\r\n");
	}
	if (!uboot_flag) {
		ret = spi_flash_write(flash, offset + len - 8, sizeof(u32), (void *)&start_x);
		if (ret) {
			printf("ERROR: SPI flash write start_x failed\n");
			return CMD_RET_FAILURE;
		}
		ret = spi_flash_write(flash, offset + len - 4, sizeof(u32), (void *)&start_y);
		if (ret) {
			printf("ERROR: SPI flash write start_y failed\n");
			return CMD_RET_FAILURE;
		}
	}
	printf("SF: %zu bytes @ %#x Written: %s\r\n", (size_t)len,
		   (uint32_t)offset, ret ? "ERROR" : "OK");

	char *cmp_buf = memalign(ARCH_DMA_MINALIGN, len);
	if (cmp_buf) {
		ret = spi_flash_read(flash, offset, len, (void *)cmp_buf);
		u32 cmp_len = uboot_flag ? len / 4 : len / 4 - 2; //start_x, start_y
		for (int i = 0; i < cmp_len; i++) {
			u32 *src_val = (u32 *)src_addr + i;
			u32 *dst_val = (u32 *)cmp_buf + i;
			if (*src_val != *dst_val) {
				break;
				printf("ERROR: Flash Data at 0x%08x offset compares failed!\n", offset + i * 4);
				return CMD_RET_FAILURE;
			}
		}
	}
	ret = es_bootspi_wp_cfg(flash, 1);
	if (ret) {
        printf("ERROR: Failed to enable bootspi WP\n");
		return CMD_RET_FAILURE;
    }
	spi_flash_free(flash);

	return CMD_RET_SUCCESS;
}

static int do_esburn_bootchain(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	const char *cmd;
	int ret;

	if (argc < 3)
		goto usage;

	cmd = argv[1];
	--argc; // write logo x,y depend this;
	++argv;
	// for(int i = 0; i< MAX_FW_NUM;i++)
	// {
	// 	fw_head_info[i].fw_valid_flag = 0;
	// }

	if (strcmp(cmd, "write") == 0)
		ret = do_bootchain_write(argc, argv);
	else if (strcmp(cmd, "uboot_logo") == 0 || strcmp(cmd, "preboot_logo") == 0) {
		bool uboot_flag = (strcmp(cmd, "uboot_logo") == 0) ? true : false;
		ret = es_bootchain_logo_write(argc, argv, uboot_flag);
	} else if (strcmp(cmd, "erase") == 0)
		ret = do_bootchain_erase(argc, argv);
	else if (strcmp(cmd, "wboot") == 0)
		ret = do_boot_write(argc, argv);
	else if (strcmp(cmd, "wroot") == 0)
		ret = do_root_write(argc, argv);
	else if (strcmp(cmd, "wmmc") == 0)
		ret = do_mmc_write(argc, argv);
	else if (strcmp(cmd, "vendor") == 0)
		ret = do_vendor_write(argc, argv);
	else
		ret = -1;

	if (ret != -1)
		return ret;

usage:
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	es_burn,	5,	0,	do_esburn_bootchain,
	"ESWIN burn tool",
#if defined(CONFIG_TARGET_ESWIN_EVB_EIC7702) || defined(CONFIG_TARGET_FML13V03)
	"\nes_burn write addr flash_stg die_num	- write binary file from memory at `addr' to die0/die1 mtd(die_num 0:default/1)\n"
#else
	"\nes_burn write addr flash_stg	- write binary file from memory at `addr' to mtd\n"
#endif
	"es_burn erase flash_stg die_num	- erase die0/die1 mtd bootloader (die_num 0:default/1)\n"
	"\t eg. es_burn erase flash 0	-erase the die0 bootloader in flash\n"
	"es_burn wboot addr len flash_stg	- write bootmenu mode boot filesystem binary file from memory at `addr' to mtd 'flash_stg'\n"
	"es_burn wroot addr len flash_stg	- write bootmenu mode root filesystem binary file from memory at `addr' to mtd 'flash_stg'\n"
	"es_burn wmmc addr len	- write .wic image binary file from memory at `addr' to eMMC\n"
	"es_burn uboot_logo addr             - uboot write logo gzip data from addr\n"
	"es_burn preboot_logo addr           - preboot write logo gzip data from addr to flash_addr, or specify the start position of the picture by setting the (start_x, start_y)\n"
);
