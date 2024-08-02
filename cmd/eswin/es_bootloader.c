// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN BURN IMAGE CMD driver
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
#include <boot_ab.h>
#include <cpu_func.h>
#include <lmb.h>
#include <dm.h>
#include <spi.h>
#include <spi_flash.h>
#include <fs.h>
#include <dm/device-internal.h>
#include <eswin/esw_mkfs.h>

// #define DEBUG
#ifndef DEBUG
#define debug_printf(fmt, args...) do{;}while(0)
#else
#define debug_printf(fmt, args...)	printf(fmt, ##args)
#endif


static struct spi_flash *flash = NULL;

#define MAX_FW_NUM 6
#define FW_HEAD_SIZE 1024
static int flash_stg = 1;  // emmc : 1; flash : 0

enum fw_offset {
	FW_HEAD_OFFSET	    = 0x0UL,
	PUBKEY_RSA_OFFSET	= 0x1000UL,
	PUBKEY_ECC_OFFSET	= 0x2000UL,
	D2D_FW_OFFSET		= 0x10000UL,
	DDR_FW_OFFSET		= 0x100000UL,
	FIRMWARE_OFFSET	    = 0x400000UL,
	BOOTLOADER_OFFSET	= 0x480000UL
};

enum fw_id {
	PUBKEY_RSA_ID   = 0,
	PUBKEY_ECC_ID   = 1,
	D2D_FW_ID	    = 2,
	FIRMWARE_ID	    = 3,
	DDR_FW_ID	    = 4,
	BOOTLOADER_ID   = 5,
	ERASE_ALL	    =100
};

typedef struct __attribute__((packed)) entry_head_info_t{
	uint32_t fw_valid_flag;
	firmware_entry_header_t entry_header;
}entry_head_info_t;

static struct entry_head_info_t fw_head_info[MAX_FW_NUM];

static struct blk_desc *mmc_dev_desc;
static struct disk_partition bootchain_part_info;
static struct disk_partition misc_part_info;


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

/******************************emmc*********************************************/
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


	debug_printf("bootchain_part_info start %lx  blksz %lx !\n", bootchain_part_info.start, bootchain_part_info.blksz);
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
int es_spi_flash_probe(void)
{
	unsigned int bus = CONFIG_SF_DEFAULT_BUS;
	unsigned int cs = CONFIG_SF_DEFAULT_CS;
	/* In DM mode, defaults speed and mode will be taken from DT */
	unsigned int speed = CONFIG_SF_DEFAULT_SPEED;
	unsigned int mode = CONFIG_SF_DEFAULT_MODE;
	struct udevice *new, *bus_dev;
	int ret;

	if(flash == NULL) {
		/* Remove the old device, otherwise probe will just be a nop */
		ret = spi_find_bus_and_cs(bus, cs, &bus_dev, &new);
		if (!ret) {
			printf("device_remove(new, DM_REMOVE_NORMAL)(error %d)\n", ret);
			device_remove(new, DM_REMOVE_NORMAL);
		}
		flash = NULL;
		ret = spi_flash_probe_bus_cs(bus, cs, &new);
		if (ret) {
			printf("Failed to initialize SPI flash at %u:%u (error %d)\n",
				bus, cs, ret);
			return -1;
		}

		flash = dev_get_uclass_priv(new);
	}
	return 0;
}

#define BOOTCHAIN_PACKAGE_SIZE (0x100000ul)
static int es_spi_flash_erase(uint64_t offset, uint64_t size)
{
	int ret;
	uint64_t package_blk, total_size, erase_size, currentIndex = 0;
	double percent;

	debug_printf("offset : %llx, size %llx  flash->size %x\n",offset, size, flash->size);
	total_size = DIV_ROUND_UP(size, 0x1000)*0x1000;
	/* Consistency checking */
	if (offset + size > flash->size) {
		printf("ERROR: attempting past flash size (%#x)\n",
			   flash->size);
		return 1;
	}
	debug_printf("offset : %llx, size %llx\n",offset, size);

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
	uint64_t package_blk, total_size, write_size, currentIndex = 0;

	/* Consistency checking */
	if (offset + size > flash->size) {
		printf("ERROR: attempting past flash size (%#x)\r\n",
			   flash->size);
		return ret;
	}

	debug_printf("offset : %llx, size %llx\r\n",offset, size);

	es_bootspi_wp_cfg(0);
	ret = es_spi_flash_erase(offset, size);
	if(ret)
		return ret;

	package_blk = DIV_ROUND_UP(size, BOOTCHAIN_PACKAGE_SIZE); 
	total_size = size;
	printf("Write progress: %3d%%:\r", 0);
	for(int i = 0;i < package_blk; i++) {
		if(total_size > BOOTCHAIN_PACKAGE_SIZE)
		{
			write_size = BOOTCHAIN_PACKAGE_SIZE;
		} else {
			write_size = total_size;
		}
		total_size -= BOOTCHAIN_PACKAGE_SIZE;
		ret = spi_flash_write(flash, offset + i * BOOTCHAIN_PACKAGE_SIZE, write_size,
						      src_addr + i * BOOTCHAIN_PACKAGE_SIZE);
		currentIndex = (uint64_t)i * 100 / package_blk;
		printf("Write progress: %3lld%%:", currentIndex);
		for(int col = 0; col < currentIndex/2; col++) {
			printf("%s","+");
		}
		printf("\r");
		if(ret)
			break;
	}

	if(!ret) {
		printf("Write progress: %3d%%:", 100);
		for(int j = 0; j < 100/2; j ++)
			printf("%s","+");
		printf("\r\n");
	}

	printf("SF: 0x%lx bytes @ %#x Written: %s\r\n",
		(size_t)size, (uint32_t)offset, ret?"ERROR":"OK");
	es_bootspi_wp_cfg(1);

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
	if(flash_stg)
		emmc_write_bootchain(src_addr, offset, size);
	else
		norflash_write_bootchain(src_addr, offset, size);
	return 0;
}

static int get_bootchain_info(void)
{
	struct firmware_header_t *fht = NULL;
	struct firmware_entry_header_t *feht;
	uint64_t len;
	uint32_t num_entries;
	uint8_t payload_type, type_id = 0;

	/* get firmware header info from emmc*/
	len = sizeof(struct firmware_header_t) + MAX_FW_NUM * sizeof(struct firmware_entry_header_t);
	fht = malloc_cache_aligned(FW_HEAD_SIZE);
	if(fht == NULL)
		return -1;
	memset(fht, 0, len);

	es_read_bootchain((uint64_t)fht, FW_HEAD_OFFSET, len);

	if(FHT_MAGIC != fht->magic){
		goto out;
    }

	num_entries = fht->num_entries;
	debug_printf("%s: num_entries %d\r\n",__func__, num_entries);
	for(int i = 0; i < num_entries; i++)
	{
		feht = (struct firmware_entry_header_t *) &fht->entries[i];
		payload_type = feht->payload_type;
		switch(payload_type){
			case PUBKEY_RSA:
				type_id = PUBKEY_RSA_ID;
				break;
			case PUBKEY_ECC:
				type_id = PUBKEY_ECC_ID;
				break;
			case DDR:
				type_id = DDR_FW_ID;
				break;
			case D2D:
				type_id = D2D_FW_ID;
				break;
			case BOOTLOADER:
				type_id = BOOTLOADER_ID;
				break;
			case FIRMWARE:
				type_id = FIRMWARE_ID;
				break;
			default:
				free(fht);
				fht = NULL;
				printf("Invalid image file type!\r\n");
				return -ENXIO;
		}

		fw_head_info[type_id].fw_valid_flag = 1;
		memcpy(&fw_head_info[type_id].entry_header, &fht->entries[i], sizeof(struct firmware_entry_header_t));
	}
out:
	if(fht) {
		free(fht);
		fht = NULL;
	}
	return 0;
}

static int do_bootchain_write(int argc, char *const argv[])
{
	struct firmware_entry_header_t * feht;
	struct firmware_header_t *fht = NULL;
	struct firmware_header_t *fw_fht;
	uint64_t src_addr, size, fw_addr, dst_offset;
	int32_t ret = 0;
	uint32_t num_entries, len;
	uint8_t payload_type, type_id = 0, cnt = 0;

	fw_addr = simple_strtoul(argv[1], NULL, 16);
	debug_printf("fw_addr 0x%llx\r\n", fw_addr);

	if (strcmp(argv[2], "emmc") == 0) {
		flash_stg = 1;
		ret = emmc_dev_get();
		if(ret < 0)
			return -ENOENT;
	}else if (strcmp(argv[2], "flash") == 0){
		ret = es_spi_flash_probe();
		if(ret < 0)
			return -ENOENT;
		flash_stg = 0;
	}else{
		printf("arguments : flash_stg type error!\r\n");
		return -ENXIO;
	}

	fw_fht = (struct firmware_header_t *) fw_addr;
	if(FHT_MAGIC != fw_fht->magic){
		printf("FHT magic should be %x, but %x!\r\n",
				FHT_MAGIC, fw_fht->magic);
		return -ENODATA;
	}

	memset(&fw_head_info, 0, sizeof(struct entry_head_info_t)*MAX_FW_NUM);
	ret = get_bootchain_info();
	if(ret < 0)
		return -ENXIO;

	num_entries = fw_fht->num_entries;
	debug_printf("%s %d num_entries %x!\r\n", __func__, __LINE__, num_entries);
	if(num_entries < 0){
		printf("fw file error!\r\n");
		return -ENXIO;
	}

	for(int i = 0; i < num_entries; i++)
	{
		feht = (struct firmware_entry_header_t *) &fw_fht->entries[i];
		src_addr = feht->offset + fw_addr;
		size = feht->size + 0x200;
		payload_type = feht->payload_type;
		switch(payload_type){
			case PUBKEY_RSA:
				printf("PUBKEY_RSA writing...\r\n");
				dst_offset = PUBKEY_RSA_OFFSET;
				type_id = PUBKEY_RSA_ID;
				break;
			case PUBKEY_ECC:
				printf("PUBKEY_ECC writing...\r\n");
				dst_offset = PUBKEY_ECC_OFFSET;
				type_id = PUBKEY_ECC_ID;
				break;
			case DDR:
				printf("DDR writing...\r\n");
				dst_offset = DDR_FW_OFFSET;
				type_id = DDR_FW_ID;
				break;
			case D2D:
				printf("D2D writing...\r\n");
				dst_offset = D2D_FW_OFFSET;
				type_id = D2D_FW_ID;
				break;
			case BOOTLOADER:
				printf("BOOTLOADER writing...\r\n");
				dst_offset = BOOTLOADER_OFFSET;
				type_id = BOOTLOADER_ID;
				break;
			case FIRMWARE:
				printf("FIRMWARE writing...\r\n");
				dst_offset = FIRMWARE_OFFSET;
				type_id = FIRMWARE_ID;
				break;
			default:
				free(fht);
				printf("Invalid image file type!\r\n");
				return -ENXIO;
		}
		// feht->offset = dst_offset;
		fw_head_info[type_id].fw_valid_flag = 1;
		memcpy(&fw_head_info[type_id].entry_header, &feht->version, sizeof(struct firmware_entry_header_t));
		fw_head_info[type_id].entry_header.offset = dst_offset;
		debug_printf("fw_head_info type_id %d version %x offset %llx size %llx sign_type %x\n",type_id,fw_head_info[type_id].entry_header.version, fw_head_info[type_id].entry_header.offset,\
				fw_head_info[type_id].entry_header.size, fw_head_info[type_id].entry_header.sign_type);

		debug_printf("entry_header\r\n");
		debug_printf("src_addr %llx!\r\n", src_addr);
		debug_printf("size %llx!\r\n", size);
		debug_printf("payload_type %x!\r\n", payload_type);
		debug_printf("dst_offset %llx!\r\n", dst_offset);
		ret = es_write_bootchain(src_addr, dst_offset, size);
		if(ret < 0){
			return ret;
		}
	}

	len = sizeof(struct firmware_header_t) + MAX_FW_NUM * sizeof(struct firmware_entry_header_t);
	// len = DIV_ROUND_UP(len, bootchain_part_info.blksz) * bootchain_part_info.blksz;
	// len = FW_HEAD_SIZE;
	fht = malloc_cache_aligned(FW_HEAD_SIZE);
	if(fht == NULL)
		return -1;
	memset(fht, 0, len);
	debug_printf("\n******cnt %d************\r\n",cnt);
	cnt = 0;

	for(int i = 0; i < MAX_FW_NUM; i++)
	{
		if(fw_head_info[i].fw_valid_flag == 1)
		{
			memcpy(&fht->entries[cnt].version, &fw_head_info[i].entry_header, sizeof(struct firmware_entry_header_t));
			cnt++;
			debug_printf("\n\n*******************************\r\n");
			debug_printf("fht entries %d key_index %x payload_type %x reserved1 %x Reserved2 %x\r\n",cnt, fht->entries[cnt].key_index, fht->entries[cnt].payload_type,\
				fht->entries[cnt].reserved1, fht->entries[cnt].Reserved2);
			debug_printf("\n******cnt %d************\r\n",cnt);
		}
	}

	fht->magic = FHT_MAGIC;
	fht->num_entries = cnt;

	ret = es_write_bootchain((uint64_t)&fht->magic, FW_HEAD_OFFSET, len);
	free(fht);
	if(ret < 0){
		free(fht);
		return ret;
	}
	printf("bootloader write OK\r\n");
	return 0;
}

static int do_bootchain_read(int argc, char *const argv[])
{
	struct firmware_header_t *fht;
	uint64_t dump_addr, size, offset;
	int32_t ret;
	uint8_t type_id;

	if (strcmp(argv[1], "DDR") == 0) {
		offset = DDR_FW_OFFSET;
		type_id = DDR_FW_ID;
	}else if (strcmp(argv[1], "D2D") == 0){
		offset = D2D_FW_OFFSET;
		type_id = D2D_FW_ID;
	}else if (strcmp(argv[1], "FIRMWARE") == 0){
		offset = FIRMWARE_OFFSET;
		type_id = FIRMWARE_ID;
	}else if (strcmp(argv[1], "BOOTLOADER") == 0){
		offset = BOOTLOADER_OFFSET;
		type_id = BOOTLOADER_ID;
	}else if (strcmp(argv[1], "PUBKEY_RSA") == 0){
		offset = PUBKEY_RSA_OFFSET;
		type_id = PUBKEY_RSA_ID;
	}else if (strcmp(argv[1], "PUBKEY_ECC") == 0){
		offset = PUBKEY_ECC_OFFSET;
		type_id = PUBKEY_ECC_ID;
	}else{
		printf("arguments : fw payload error!\r\n");
		return -ENXIO;
	}

	if (strcmp(argv[3], "emmc") == 0) {
		flash_stg = 1;
		ret = emmc_dev_get();
		if(ret < 0)
			return -ENOENT;
	}else if (strcmp(argv[3], "flash") == 0){
		ret = es_spi_flash_probe();
		if(ret < 0)
			return -ENOENT;
		flash_stg = 0;
	}else{
		printf("arguments : flash_stg type error!\r\n");
		return -ENXIO;
	}

	dump_addr = simple_strtoul(argv[2], NULL, 16);
	fht = (struct firmware_header_t *) dump_addr;
	memset((void *)dump_addr, 0, 0x200);
	memset(&fw_head_info, 0, sizeof(struct entry_head_info_t)*MAX_FW_NUM);
	ret = get_bootchain_info();
	if(ret < 0)
		return -ENXIO;
	fht->magic = FHT_MAGIC;
	fht->num_entries = 1;
	if(fw_head_info[type_id].fw_valid_flag != 1)
	{
		printf("read bootchain type %s not exit\r\n",argv[1]);
		return -ENXIO;
	}
	memcpy(&fht->entries[0], &fw_head_info[type_id].entry_header, sizeof(struct firmware_entry_header_t));
	size = fht->entries[0].size + 0x200;
	es_read_bootchain(dump_addr+0x200, offset, size);
	size += 0x200;
	flush_cache(dump_addr, size);
	printf("bootloader read OK: %s to 0x%llx len 0x%llx\r\n", argv[1], dump_addr, size);

	return 0;
}

static int do_bootchain_erase(int argc, char *const argv[])
{
	struct firmware_header_t *fht = NULL;
	uint64_t offset, len;
	int32_t ret;
	uint8_t type_id = 0, cnt = 0;

	if (argc < 2){
		return -1;
	}

	if (strcmp(argv[1], "DDR") == 0) {
		offset = DDR_FW_OFFSET;
		type_id = DDR_FW_ID;
	}else if (strcmp(argv[1], "D2D") == 0){
		offset = D2D_FW_OFFSET;
		type_id = D2D_FW_ID;
	}else if (strcmp(argv[1], "FIRMWARE") == 0){
		offset = FIRMWARE_OFFSET;
		type_id = FIRMWARE_ID;
	}else if (strcmp(argv[1], "BOOTLOADER") == 0){
		offset = BOOTLOADER_OFFSET;
		type_id = BOOTLOADER_ID;
	}else if (strcmp(argv[1], "PUBKEY_RSA") == 0){
		offset = PUBKEY_RSA_OFFSET;
		type_id = PUBKEY_RSA_ID;
	}else if (strcmp(argv[1], "PUBKEY_ECC") == 0){
		offset = PUBKEY_ECC_OFFSET;
		type_id = PUBKEY_ECC_ID;
	}else{
		type_id = ERASE_ALL;
		printf("erase all bootchain %d\r\n",__LINE__);
	}

	if (strcmp(argv[2], "emmc") == 0) {
		flash_stg = 1;
		ret = emmc_dev_get();
		if(ret < 0)
			return -ENOENT;
	}else if (strcmp(argv[2], "flash") == 0){
		ret = es_spi_flash_probe();
		if(ret < 0)
			return -ENOENT;
		flash_stg = 0;
	}else{
		printf("arguments : flash_stg type error!\r\n");
		return -ENXIO;
	}

	len = sizeof(struct firmware_header_t) + MAX_FW_NUM * sizeof(struct firmware_entry_header_t);
	if( 1 == flash_stg)
		len = DIV_ROUND_UP(len, bootchain_part_info.blksz) * bootchain_part_info.blksz;
	fht = malloc_cache_aligned(len);
	if(fht == NULL)
		return -1;

	memset(fht, 0, len);
	fht->magic = FHT_MAGIC;
	if (type_id == ERASE_ALL){
		// fht->magic = 0;
		// fht->num_entries = 0;
		memset(fht, 0, len);
	}
	else{
		memset(&fw_head_info, 0, sizeof(struct entry_head_info_t)*MAX_FW_NUM);
		ret = get_bootchain_info();
		if(ret < 0){
			free(fht);
			return -ENXIO;
		}

		if(fw_head_info[type_id].fw_valid_flag == 1)
		{
			fw_head_info[type_id].fw_valid_flag = 0;
		}
		for(int i = 0; i < MAX_FW_NUM; i++)
		{
			if(fw_head_info[i].fw_valid_flag == 1)
			{
				memcpy(&fht->entries[cnt].version, &fw_head_info[i].entry_header, sizeof(struct firmware_entry_header_t));
				cnt++;
			}
		}
		fht->num_entries = cnt;
	}
	ret = es_write_bootchain((uint64_t)&fht->magic, FW_HEAD_OFFSET, len);
	free(fht);

	return 0;
}

static struct bootloader_message *abc = NULL;
static struct boot_bank *bank = NULL;

static int get_abc_and_bank(void)
{
	int block = 1;

	if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, MISC_DEV_PART,
				&mmc_dev_desc, &misc_part_info,true) < 0) {
		printf("UPDATE: Get information of misc partiyion failed!\r\n");
		return -ENOENT;
	}

	get_bootmessage_from_disk(mmc_dev_desc, &misc_part_info, &abc);
#ifdef CONFIG_SYSTEM_UPDATE_B
	if(0 == abc->curr_bank){
		block = 1;
	}else if(1 == abc->curr_bank){
		block = 2;
	}else{
		printf("UPDATE: Read curr_bank failed!\r\n");
		free(abc);
		return -ENOENT;
	}
#endif
	get_bootbank_from_disk(mmc_dev_desc, &misc_part_info, block, &bank);

	return 0;
}


static int do_kernel_write(int argc, char *const argv[])
{
	int curr_bank = 0;
	int block = 1;
	uint64_t addr, len, time = 0;
	struct disk_partition boot_part_info;
	const char *dev_part_str;
	const char *filename;
	int32_t ret = 0;

	if (argc < 3)
		return -ENXIO;

	addr = simple_strtoul(argv[1], NULL, 16);
	len = simple_strtoul(argv[2], NULL, 16);

	if(esburn_init_load_addr(addr, len)) {
		puts("\nes_burn error: ");
		puts("trying to overwrite reserved memory...\n");
		return -ENXIO;
	}

	if (strcmp(argv[3], "emmc") == 0) {
		flash_stg = 1;
	}else{
		printf("arguments : The kernel can only be written to emmc!\r\n");
		return -ENXIO;
	}

	ret = get_abc_and_bank();
	if(ret < 0)
		return -ENXIO;

	dev_part_str = UPDATE_BOOTA_DEV_PART;


#ifdef CONFIG_SYSTEM_UPDATE_B
	if(0 == abc->curr_bank){
		dev_part_str = UPDATE_BOOTB_DEV_PART;  /* "0#bootb" */
	}else{
		dev_part_str = UPDATE_BOOTA_DEV_PART;
	}
#endif
	if (fs_set_blk_dev(MMC_DEV_IFACE, dev_part_str, FS_TYPE_FAT)) {
		ret =  mkfs_fat(MMC_DEV_IFACE, dev_part_str);
		if( 0 != ret ) {
			printf("mkfs_fat failed\r\n");
			ret = -ENXIO;
			goto out;
		}
		if (fs_set_blk_dev(MMC_DEV_IFACE, dev_part_str, FS_TYPE_FAT)) {
			printf("part_get_info_by_dev_and_name_or_num failed\r\n");
			ret = -ENXIO;
			goto out;
		}
	}
	filename = BOOT_FILE_IMG;	/* "fitimage" */
	time = get_timer(0);
	ret = fs_write(filename, addr, 0x0, len, (void *)&len);
	fs_close();
	time = get_timer(time);
	if(0 == ret ){
		strcpy(abc->command, "boot_normal");
#ifdef CONFIG_SYSTEM_UPDATE_B
		if(0 == abc->curr_bank){
			curr_bank = 1;
			block = 2;
		}else{
			curr_bank = 0;
		}
#endif
		abc->curr_bank = curr_bank;
		bank->boot_version = 0;
		bank->successful_flag = 1;
		printf("%s:%llu bytes has been successfully writen in %s use %llu ms\r\n", filename, len, dev_part_str, time);
	}
	else{
	/*TODO*/
#ifndef CONFIG_SYSTEM_UPDATE_A
		strcpy(abc->command, "boot_normal");
#endif
		bank->successful_flag = 0;
#ifdef CONFIG_SYSTEM_UPDATE_C
		abc->curr_bank = 1;
#endif
		printf("%s has been failed writen in %s\r\n", filename, dev_part_str);
	}

	abc->magic =  BOOTMESS_MAGIC;
	bank->magic = BOOTBANK_MAGIC;
	abc->crc32_bc = bootmessage_compute_crc(abc);
	bank->crc32_bb = bootbank_compute_crc(bank);
	bootloader_message_store(mmc_dev_desc, &misc_part_info, abc);
	boot_bank_store(mmc_dev_desc, &misc_part_info, block, bank);

out:
	debug_printf("out:\nbootloader_message info\n \
			magic : 0x%08x\t command : %s\t crc32_bc : 0x%08x \n \
			kernel : boot%s \t rootfs : rootfs%s \r\n", \
			abc->magic, abc->command, abc->crc32_bc,\
			( abc->curr_bank == 0 )?"a":  ( abc->curr_bank == 1)?"b":" err", \
			( abc->rtfs_bank == 0 )?"a":  ( abc->rtfs_bank == 1)?"b":" err");
	free(abc);
	free(bank);
	return ret;
}

static int do_rootfs_write(int argc, char *const argv[])
{

	uint64_t addr, size, cnt;
	uint64_t package_blk, last_blk = 0, cycle_index, currentIndex = 0;
	int32_t ret = 0;
	const char *dev_part_str;
	struct disk_partition rootfs_part_info;
	int block = 1;

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
	ret = get_abc_and_bank();
	if(ret < 0)
		return -ENXIO;

	/* addr blk cnt */
	dev_part_str = UPDATE_ROOTFSA_DEV_PART;  /* "rootfsa" */

	if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
				&mmc_dev_desc, &rootfs_part_info,true) < 0) {
		printf("UPDATE: Get information of rootfs partition failed!\r\n");
		ret =  -ENOENT;
		goto out;
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

	for(int i = 0;i < cycle_index; i++) {
		currentIndex = i / 2;
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + i * package_blk, package_blk, (void __iomem *)(addr + i * package_blk * rootfs_part_info.blksz));
		printf("Write progress: %3d%%:", i);
		for(int col = 0; col < currentIndex; col++) {
			printf("%s","+");
		}
		printf("\r");
		if(ret != package_blk){
		/*TODO*/
		#ifdef CONFIG_SYSTEM_UPDATE_C
			strcpy(abc->command, "boot_normal");
			abc->rtfs_bank = 1;
		#endif
			printf("UPDATE: emmc rootfs partition write failed!\r\n");
			ret =  -ENOENT;
			goto out;
		}
	}
	if(last_blk)
	{
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + cycle_index * package_blk, last_blk, (void __iomem *)(addr + cycle_index * package_blk * rootfs_part_info.blksz));

		if(ret != last_blk){
		/*TODO*/
		#ifdef CONFIG_SYSTEM_UPDATE_C
			strcpy(abc->command, "boot_normal");
			abc->rtfs_bank = 1;
		#endif
			printf("UPDATE: emmc rootfs partition write failed!\r\n");
			ret =  -ENOENT;
			goto out;
		}
	}
	printf("Write progress: %3d%%:", 100);
	for(int j = 0; j < 100/2; j ++)
		printf("%s","+");
	printf("\r\n");

	bank->rootfs_version = 0;
	abc->rtfs_bank = 0;
	printf("rootfs has been successfully writen in %s\r\n", dev_part_str);

	abc->magic =  BOOTMESS_MAGIC;
	bank->magic = BOOTBANK_MAGIC;
	abc->crc32_bc = bootmessage_compute_crc(abc);
	bank->crc32_bb = bootbank_compute_crc(bank);
	boot_bank_store(mmc_dev_desc, &misc_part_info, block, bank);
	bootloader_message_store(mmc_dev_desc, &misc_part_info, abc);
out:
	debug_printf("bootloader_message info\n \
			magic : 0x%08x\t command : %s\t crc32_bc : 0x%08x \n \
			kernel : boot%s \t rootfs : rootfs%s \r\n",
			abc->magic, abc->command, abc->crc32_bc,\
			( abc->curr_bank == 0 )?"a":  ( abc->curr_bank == 1)?"b":" err", \
			( abc->rtfs_bank == 0 )?"a":  ( abc->rtfs_bank == 1)?"b":" err");
	free(abc);
	free(bank);
	return ret;

}

static int do_boot_write(int argc, char *const argv[])
{

	uint64_t addr, size, cnt;
	uint64_t package_blk, last_blk = 0, cycle_index, currentIndex = 0;
	int32_t ret = 0;
	const char *dev_part_str;
	struct disk_partition rootfs_part_info;
	int block = 1;

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
	dev_part_str = UPDATE_BOOTA_DEV_PART;  /* "rootfsa" */

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
		printf("Write progress: %3d%%:", i);
		for(int col = 0; col < currentIndex; col++) {
			printf("%s","+");
		}
		printf("\r");
	}
	if(last_blk)
	{
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + cycle_index * package_blk, last_blk, (void __iomem *)(addr + cycle_index * package_blk * rootfs_part_info.blksz));

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
	int block = 1;

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
		printf("Write progress: %3d%%:", i);
		for(int col = 0; col < currentIndex; col++) {
			printf("%s","+");
		}
		printf("\r");
	}
	if(last_blk)
	{
		ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start + cycle_index * package_blk, last_blk, (void __iomem *)(addr + cycle_index * package_blk * rootfs_part_info.blksz));
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

	uint64_t fw_addr = simple_strtoul(argv[1], NULL, 16);
	debug_printf("fw_addr 0x%llx\r\n", fw_addr);

	ret = es_spi_flash_probe();
	if(ret < 0)
		return -ENOENT;

	struct vendor_info_t *vendor_info = (struct vendor_info_t *) fw_addr;
	if(vendor_info->magic != VENDOR_MAGIC)
		return -ENOENT;

	ret = norflash_write_bootchain((uint64_t)&vendor_info->magic, VENDER_OFFSET, VENDER_SIZE);
	if(ret)
		return -1;
	printf("vendor info write OK\r\n");
	return 0;
}

static int do_esburn_bootchain(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	const char *cmd;
	int ret;

	if (argc < 3)
		goto usage;

	cmd = argv[1];
	--argc;
	++argv;
	for(int i = 0; i< MAX_FW_NUM;i++)
	{
		fw_head_info[i].fw_valid_flag = 0;
	}

	// if (strcmp(cmd, "read") == 0)
	// 	ret = do_bootchain_read(argc, argv);
	if (strcmp(cmd, "write") == 0)
		ret = do_bootchain_write(argc, argv);
	else if (strcmp(cmd, "erase") == 0)
		ret = do_bootchain_erase(argc, argv);
	// else if (strcmp(cmd, "wkernel") == 0)
	// 	ret = do_kernel_write(argc, argv);
	// else if (strcmp(cmd, "wrootfs") == 0)
	// 	ret = do_rootfs_write(argc, argv);
	else if (strcmp(cmd, "wboot") == 0)
		ret = do_boot_write(argc, argv);
	else if (strcmp(cmd, "wroot") == 0)
		ret = do_root_write(argc, argv);
	// else if (strcmp(cmd, "vendor") == 0)
		// ret = do_vendor_write(argc, argv);
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
	"\nes_burn write addr flash_stg	- write binary file from memory at `addr' to mtd\n"
	"es_burn erase fw_type flash_stg	- erase binary file who type is 'fw_type' from 'flash_stg'\n"
	"es_burn wboot addr len flash_stg	- write bootmenu mode boot filesystem binary file from memory at `addr' to mtd 'flash_stg'\n"
	"es_burn wroot addr len flash_stg	- write bootmenu mode root filesystem binary file from memory at `addr' to mtd 'flash_stg'\n"
);
