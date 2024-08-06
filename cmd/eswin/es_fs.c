// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Xiang Xu <xuxiang@eswincomputing.com>
 */
#include <stdlib.h>
#include <common.h>
#include <command.h>
#include <errno.h>
#include <blk.h>
#include <mmc.h>
#include <memalign.h>
#include <asm/io.h>
#include <cpu_func.h>
#include <lmb.h>
#include <dm.h>
#include <fs.h>
#include <linux/math64.h>
#include <display_options.h>

static struct blk_desc *mmc_dev_desc;

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

int get_size(const char *ifname, const char *dev_part_str, const char *fname, loff_t *size)
{
	if (fs_set_blk_dev(ifname, dev_part_str, FS_TYPE_ANY))
		return 1;

	if (fs_size(fname, size) < 0)
		return CMD_RET_FAILURE;

	return 0;
}

#define BUF_SIZE (1024*1024*64)

int fs_load(const char *ifname, const char *dev_part_str, const char *fname, 
			 ulong addr, uint64_t offset, uint64_t bytes, uint64_t *len_read)
{
	int ret;
	if (fs_set_blk_dev(ifname, dev_part_str, FS_TYPE_ANY)) {
		log_err("Can't set block device\n");
		return 1;
	}
	ret = fs_read(fname, addr, offset, bytes, len_read);
	if (ret < 0) {
		log_err("Failed to load '%s'\n", fname);
		return 1;
	}

	return 0;
}
static int do_image_write(int argc, char *const argv[])
{
	const char *r_ifname = NULL;
	const char *r_dev_part_str = NULL;
	const char *w_ifname = "mmc";
	const char *filename = NULL;
	uint8_t *buf = NULL;
	uint64_t buf_size = BUF_SIZE, len_read;
	uint64_t file_size = 0, file_offset = 0;
	uint64_t cnt =0, emmc_offset = 0;
	uint64_t currentIndex = 0, n;
	struct mmc *mmc;
	int dev = -1;
	int32_t ret = 0;

	if (argc != 5)
		return CMD_RET_USAGE;

	r_ifname = argv[1];
	r_dev_part_str = argv[2];
	filename = argv[3];
	dev = (int)dectoul(argv[4], NULL);

	if(get_size(r_ifname, r_dev_part_str, filename, &file_size)) {
		printf("Error: %s not found\n", filename);
		return CMD_RET_FAILURE;
	}
	mmc = __init_mmc_device(dev, false, MMC_MODES_END);
	if (!mmc)
		return CMD_RET_FAILURE;
	if (mmc_getwp(mmc) == 1) {
		printf("Error: card is write protected!\n");
		return CMD_RET_FAILURE;
	}
	if ( file_size >  mmc->capacity_user ) {
		printf("Error: %s %d partition too small.\r\n", w_ifname, dev);
		return -ENOENT;
	}
	buf_size = roundup(buf_size, mmc->write_bl_len);
	buf = memalign(64,buf_size);
	printf("Write progress: %3d%%:\r", 0);
	do {
		if(file_size - file_offset < buf_size) {
			buf_size = file_size - file_offset;
		}

		if(fs_load(r_ifname, r_dev_part_str, filename, (ulong)buf, file_offset, buf_size, &len_read)) {
			printf("Error: %s read failed!\n",filename);
			ret = -ENXIO;
			goto out;
		}

		emmc_offset = DIV_ROUND_UP(file_offset, mmc->write_bl_len);
		cnt = DIV_ROUND_UP(buf_size, mmc->write_bl_len);  /* blkcnt */
		n = blk_dwrite(mmc_get_blk_desc(mmc), emmc_offset, cnt, buf);
		if(n != cnt) {
			printf("Error: %s write failed!\n", w_ifname);
			ret = -ENXIO;
			goto out;
		}

		file_offset += buf_size;
		currentIndex = (uint64_t)(file_offset * 100 / file_size );
		printf("Write progress: %3lld%%:", currentIndex);
		for(int col = 0; col < currentIndex/2; col++) {
			printf("%s","+");
		}
		printf("\r");
	} while (file_offset < file_size );

	printf("Write progress: %3d%%:", 100);
	for(int j = 0; j < 100/2; j ++)
		printf("%s","+");
	printf("\r\n");
	printf("%s has been successfully writen in mmc %d\r\n", filename, dev);
out:
	free(buf);
	return 0;
}

static int do_image_update(int argc, char *const argv[])
{
	const char *r_ifname = NULL;
	const char *r_dev_part_str = NULL;
	const char *w_ifname = NULL;
	const char *w_dev_part_str = NULL;
	const char *filename = NULL;
	uint8_t *buf = NULL;
	struct disk_partition mmc_part_info;
	uint64_t buf_size = BUF_SIZE, len_read;
	uint64_t file_size = 0, file_offset = 0;
	uint64_t cnt =0, emmc_offset = 0;
	uint64_t currentIndex = 0;
	int32_t ret = 0, n;

	if (argc != 5)
		return CMD_RET_USAGE;

	r_ifname = argv[1];
	r_dev_part_str = argv[2];
	filename = argv[3];
	w_ifname = "mmc";
	w_dev_part_str = argv[4];
	// w_ifname = argv[4];
	// w_dev_part_str = argv[5];

	if(get_size(r_ifname, r_dev_part_str, filename, &file_size)) {
		printf("Error: %s not found\n", filename);
		return CMD_RET_FAILURE;
	}

	if (part_get_info_by_dev_and_name_or_num(w_ifname, w_dev_part_str,
				&mmc_dev_desc, &mmc_part_info,true) < 0) {
		printf("Error: Get information of %s %s partition failed!\r\n", w_ifname, w_dev_part_str);
		return -ENOENT;
	}
	cnt = DIV_ROUND_UP(file_size, mmc_part_info.blksz);
	if ( cnt > mmc_part_info.size ) {
		printf("Error: %s %s partition too small.\r\n", w_ifname, w_dev_part_str);
		return -ENOENT;
	}

	buf_size = roundup(buf_size, mmc_part_info.blksz);
	buf = memalign(64,buf_size);
	printf("Write progress: %3d%%:\r", 0);
	do {
		if(file_size - file_offset < buf_size) {
			buf_size = file_size - file_offset;
		}
		if(fs_load(r_ifname, r_dev_part_str, filename, (ulong)buf, file_offset, buf_size, &len_read)) {
			printf("Error: %s read failed!\n",filename);
			ret = -ENXIO;
			goto out;
		}
		emmc_offset = DIV_ROUND_UP(file_offset, mmc_part_info.blksz) + mmc_part_info.start;
		cnt = DIV_ROUND_UP(buf_size, mmc_part_info.blksz);  /* blkcnt */
		n = blk_dwrite(mmc_dev_desc, emmc_offset, cnt, buf);
		if(n != cnt) {
			printf("Error: %s %s write failed!\n", w_ifname, w_dev_part_str);
			ret = -ENXIO;
			goto out;
		}
		file_offset += buf_size;
		currentIndex = (uint64_t)(file_offset * 100 / file_size );
		printf("Write progress: %3lld%%:", currentIndex);
		for(int col = 0; col < currentIndex/2; col++) {
			printf("%s","+");
		}
		printf("\r");
	} while (file_offset < file_size );

	printf("Write progress: %3d%%:", 100);
	for(int j = 0; j < 100/2; j ++)
		printf("%s","+");
	printf("\r\n");
	printf("%s has been successfully writen in %s %s\r\n", w_ifname, w_ifname, w_dev_part_str);
out:
	free(buf);
	return ret;
}

static int do_esfs(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	const char *cmd;
	int ret;
	if (argc < 6)
		goto usage;

	cmd = argv[1];
	--argc;
	++argv;
	if (strcmp(cmd, "write") == 0)
		ret = do_image_write(argc, argv);
	else if (strcmp(cmd, "update") == 0)
		ret = do_image_update(argc, argv);
	else
		ret = -1;

	if (ret != -1)
		return ret;

usage:
	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	es_fs,	7,	0,	do_esfs,
	"ESWIN write filesystem image file into MMC",
	"\nes_fs write ifname  dev_part filename dev_num \n"
	"  - Write System image file(.wic) from {ifname} {dev_part} to mmc {dev_num}\n"
	"    ifname dev_part : block device and partition\n"
	"    filename : filesystem image file (e.g. xxx.wic)\n"
	"    dev_num : set write image file mmc device,You can see the devece num for the emmc or sd card in the cmd \"mmc list\"(e.g. 0, 1)\n"
	"es_fs update ifname  dev_part filename wdev_part\n"
	"  - Write File system image file from {ifname} {dev_part} to mmc {dev_part_num}\n"
	"    ifname dev_part : block device and partition\n"
	"    filename : filesystem image file (e.g. boot.ext4, root.ext4)\n"
	"    wdev_part : image file is written to the MMC partition number or name (e.g. 0#boot, 0:1)\n"
);