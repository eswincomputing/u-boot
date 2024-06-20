// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2010
 * Texas Instruments, <www.ti.com>
 *
 * Aneesh V <aneesh@ti.com>
 */
#include <dm.h>
#include <log.h>
#include <part.h>
#include <spl.h>
#include <spl_load.h>
#include <linux/compiler.h>
#include <errno.h>
#include <errno.h>
#include <mmc.h>
#include <image.h>
#include <imx_container.h>

static ulong h_spl_load_read(struct spl_load_info *load, ulong off,
			     ulong size, void *buf)
{
	struct blk_desc *bd = load->priv;
	lbaint_t sector = off >> bd->log2blksz;
	lbaint_t count = size >> bd->log2blksz;

	return blk_dread(bd, sector, count, buf) << bd->log2blksz;
}

static __maybe_unused unsigned long spl_mmc_raw_uboot_offset(int part)
{
#if IS_ENABLED(CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_SECTOR)
	if (part == 0)
		return CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_DATA_PART_OFFSET;
#endif

	return 0;
}

static __maybe_unused
int mmc_load_image_raw_sector(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev,
			      struct mmc *mmc, unsigned long sector)
{
	int ret;
	struct blk_desc *bd = mmc_get_blk_desc(mmc);
	struct spl_load_info load;

	load.priv = bd;
	spl_set_bl_len(&load, bd->blksz);
	load.read = h_spl_load_read;
	ret = spl_load(spl_image, bootdev, &load, 0, sector << bd->log2blksz);
	if (ret) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		puts("mmc_load_image_raw_sector: mmc block read error\n");
#endif
		return -1;
	}

	return 0;
}

static int spl_mmc_get_device_index(u32 boot_device)
{
	switch (boot_device) {
	case BOOT_DEVICE_MMC1:
		return 0;
	case BOOT_DEVICE_MMC2:
	case BOOT_DEVICE_MMC2_2:
		return 1;
	}

#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
	printf("spl: unsupported mmc boot device.\n");
#endif

	return -ENODEV;
}

static int spl_mmc_find_device(struct mmc **mmcp, u32 boot_device)
{
	int err, mmc_dev;

	mmc_dev = spl_mmc_get_device_index(boot_device);
	if (mmc_dev < 0)
		return mmc_dev;

#if CONFIG_IS_ENABLED(DM_MMC)
	err = mmc_init_device(mmc_dev);
#else
	err = mmc_initialize(NULL);
#endif /* DM_MMC */
	if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		printf("spl: could not initialize mmc. error: %d\n", err);
#endif
		return err;
	}
	*mmcp = find_mmc_device(mmc_dev);
	err = *mmcp ? 0 : -ENODEV;
	if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		printf("spl: could not find mmc device %d. error: %d\n",
		       mmc_dev, err);
#endif
		return err;
	}

	return 0;
}

#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_PARTITION
static int mmc_load_image_raw_partition(struct spl_image_info *spl_image,
					struct spl_boot_device *bootdev,
					struct mmc *mmc, int partition,
					unsigned long sector)
{
	struct disk_partition info;
	int err;

#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_PARTITION_TYPE
	int type_part;
	/* Only support MBR so DOS_ENTRY_NUMBERS */
	for (type_part = 1; type_part <= DOS_ENTRY_NUMBERS; type_part++) {
		err = part_get_info(mmc_get_blk_desc(mmc), type_part, &info);
		if (err)
			continue;
		if (info.sys_ind ==
			CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION_TYPE) {
			partition = type_part;
			break;
		}
	}
#endif

	err = part_get_info(mmc_get_blk_desc(mmc), partition, &info);
	if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		puts("spl: partition error\n");
#endif
		return -1;
	}

#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_SECTOR
	return mmc_load_image_raw_sector(spl_image, bootdev, mmc, info.start + sector);
#else
	return mmc_load_image_raw_sector(spl_image, bootdev, mmc, info.start);
#endif
}
#endif

#if CONFIG_IS_ENABLED(FALCON_BOOT_MMCSD)
static int mmc_load_image_raw_os(struct spl_image_info *spl_image,
				 struct spl_boot_device *bootdev,
				 struct mmc *mmc)
{
	int ret;

#if defined(CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR)
	unsigned long count;

	count = blk_dread(mmc_get_blk_desc(mmc),
		CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR,
		CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS,
		(void *)CONFIG_SPL_PAYLOAD_ARGS_ADDR);
	if (count != CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTORS) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
		puts("mmc_load_image_raw_os: mmc block read error\n");
#endif
		return -1;
	}
#endif	/* CONFIG_SYS_MMCSD_RAW_MODE_ARGS_SECTOR */

	ret = mmc_load_image_raw_sector(spl_image, bootdev, mmc,
		CONFIG_SYS_MMCSD_RAW_MODE_KERNEL_SECTOR);
	if (ret)
		return ret;

	if (spl_image->os != IH_OS_LINUX && spl_image->os != IH_OS_TEE) {
		puts("Expected image is not found. Trying to start U-Boot\n");
		return -ENOENT;
	}

	return 0;
}
#else
static int mmc_load_image_raw_os(struct spl_image_info *spl_image,
				 struct spl_boot_device *bootdev,
				 struct mmc *mmc)
{
	return -ENOSYS;
}
#endif

#ifndef CONFIG_SPL_OS_BOOT
int spl_start_uboot(void)
{
	return 1;
}
#endif

#ifdef CONFIG_SYS_MMCSD_FS_BOOT
static int spl_mmc_do_fs_boot(struct spl_image_info *spl_image,
			      struct spl_boot_device *bootdev,
			      struct mmc *mmc,
			      const char *filename)
{
	int err = -ENOSYS;

	__maybe_unused int partition = CONFIG_SYS_MMCSD_FS_BOOT_PARTITION;

#if CONFIG_SYS_MMCSD_FS_BOOT_PARTITION == -1
	{
		struct disk_partition info;
		debug("Checking for the first MBR bootable partition\n");
		for (int type_part = 1; type_part <= DOS_ENTRY_NUMBERS; type_part++) {
			err = part_get_info(mmc_get_blk_desc(mmc), type_part, &info);
			if (err)
				continue;
			debug("Partition %d is of type %d and bootable=%d\n", type_part, info.sys_ind, info.bootable);
			if (info.bootable != 0) {
				debug("Partition %d is bootable, using it\n", type_part);
				partition = type_part;
				break;
			}
		}
		printf("Using first bootable partition: %d\n", partition);
		if (partition == CONFIG_SYS_MMCSD_FS_BOOT_PARTITION) {
			return -ENOSYS;
		}
	}
#endif

#ifdef CONFIG_SPL_FS_FAT
	if (!spl_start_uboot()) {
		err = spl_load_image_fat_os(spl_image, bootdev, mmc_get_blk_desc(mmc),
			partition);
		if (!err)
			return err;
	}
#ifdef CONFIG_SPL_FS_LOAD_PAYLOAD_NAME
	err = spl_load_image_fat(spl_image, bootdev, mmc_get_blk_desc(mmc),
				 partition,
				 filename);
	if (!err)
		return err;
#endif
#endif
#ifdef CONFIG_SPL_FS_EXT4
	if (!spl_start_uboot()) {
		err = spl_load_image_ext_os(spl_image, bootdev, mmc_get_blk_desc(mmc),
			partition);
		if (!err)
			return err;
	}
#ifdef CONFIG_SPL_FS_LOAD_PAYLOAD_NAME
	err = spl_load_image_ext(spl_image, bootdev, mmc_get_blk_desc(mmc),
				 partition,
				 filename);
	if (!err)
		return err;
#endif
#endif

#if defined(CONFIG_SPL_FS_FAT) || defined(CONFIG_SPL_FS_EXT4)
	err = -ENOENT;
#endif

	return err;
}
#endif

u32 __weak spl_mmc_boot_mode(struct mmc *mmc, const u32 boot_device)
{
#if defined(CONFIG_SPL_FS_FAT) || defined(CONFIG_SPL_FS_EXT4)
	return MMCSD_MODE_FS;
#elif defined(CONFIG_SUPPORT_EMMC_BOOT)
	return MMCSD_MODE_EMMCBOOT;
#else
	return MMCSD_MODE_RAW;
#endif
}

#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_PARTITION
int __weak spl_mmc_boot_partition(const u32 boot_device)
{
	return CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION;
}
#endif

unsigned long __weak arch_spl_mmc_get_uboot_raw_sector(struct mmc *mmc,
						       unsigned long raw_sect)
{
	return raw_sect;
}

unsigned long __weak board_spl_mmc_get_uboot_raw_sector(struct mmc *mmc,
						  unsigned long raw_sect)
{
	return arch_spl_mmc_get_uboot_raw_sector(mmc, raw_sect);
}

unsigned long __weak spl_mmc_get_uboot_raw_sector(struct mmc *mmc,
						  unsigned long raw_sect)
{
	return board_spl_mmc_get_uboot_raw_sector(mmc, raw_sect);
}

int default_spl_mmc_emmc_boot_partition(struct mmc *mmc)
{
	int part;
	/*
	 * This function returns the eMMC partition from which
	 * would be loaded next payload (e.g. U-Boot proper).
	 * This partition will be set into eMMC partition access
	 * register and therefore return value of this function
	 * has same meaning as EXT_CSD_EXTRACT_PARTITION_ACCESS.
	 *
	 * Meaning of EXT_CSD_EXTRACT_PARTITION_ACCESS (3-bit) is:
	 * 0 - User area
	 * 1 - Boot partition 1
	 * 2 - Boot partition 2
	 * 3 - Replay Protected Memory Block (RPMB)
	 * 4 - General Purpose partition 1
	 * 5 - General Purpose partition 2
	 * 6 - General Purpose partition 3
	 * 7 - General Purpose partition 4
	 */
#ifdef CONFIG_SYS_MMCSD_RAW_MODE_EMMC_BOOT_PARTITION
	part = CONFIG_SYS_MMCSD_RAW_MODE_EMMC_BOOT_PARTITION;
#else
	/*
	 * When explicit CONFIG_SYS_MMCSD_RAW_MODE_EMMC_BOOT_PARTITION
	 * config option was not chosen during build time we choose
	 * the boot partition based on the eMMC boot partition enable
	 * register (EXT_CSD_EXTRACT_BOOT_PART).
	 *
	 * Meaning of EXT_CSD_EXTRACT_BOOT_PART (3-bit) value is:
	 * 0 - Booting from this eMMC device is disabled
	 * 1 - Boot partition 1 is used for booting
	 * 2 - Boot partition 2 is used for booting
	 * 3 - Reserved
	 * 4 - Reserved
	 * 5 - Reserved
	 * 6 - Reserved
	 * 7 - User area is used for booting
	 *
	 * Note: See difference between EXT_CSD_EXTRACT_PARTITION_ACCESS
	 * and EXT_CSD_EXTRACT_BOOT_PART, specially about User area value.
	 */
	if (mmc->part_config == MMCPART_NOAVAILABLE)
		part = 0; /* If partitions are not supported then we have only User Area partition */
	else {
		switch(EXT_CSD_EXTRACT_BOOT_PART(mmc->part_config)) {
		case 0: /* Booting from this eMMC device is disabled */
			/*
			 * This eMMC device has disabled booting.
			 * This may happen because of misconfiguration of eMMC device or
			 * because user explicitly wanted to not boot from this eMMC device.
			 * eMMC boot configuration can be changed by "mmc partconf" command.
			 */
			part = -ENXIO; /* negative value indicates error */
			/* Note that error message is printed by caller of this function. */
			break;
		case 1: /* Boot partition 1 is used for booting */
			part = 1;
			break;
		case 2: /* Boot partition 2 is used for booting */
			part = 2;
			break;
		case 7: /* User area is used for booting */
			part = 0;
			break;
		default: /* Other values are reserved */
			/*
			 * This eMMC device has configured booting from reserved value.
			 * This may happen because of misconfiguration of eMMC device or
			 * because this is newer eMMC device than what U-Boot understand.
			 * If newer eMMC specification defines meaning for some reserved
			 * values then switch above should be updated for new cases.
			 * At this stage we do not know how to interpret this reserved
			 * value so return error.
			 * eMMC boot configuration can be changed by "mmc partconf" command.
			 */
			part = -EINVAL; /* negative value indicates error */
			/* Note that error message is printed by caller of this function. */
			break;
		}
	}
#endif
	return part;
}

int __weak spl_mmc_emmc_boot_partition(struct mmc *mmc)
{
	return default_spl_mmc_emmc_boot_partition(mmc);
}

static int spl_mmc_get_mmc_devnum(struct mmc *mmc)
{
	struct blk_desc *block_dev;
#if !CONFIG_IS_ENABLED(BLK)
	block_dev = &mmc->block_dev;
#else
	block_dev = mmc_get_blk_desc(mmc);
#endif
	return block_dev->devnum;
}

static struct mmc *mmc;

void spl_mmc_clear_cache(void)
{
	mmc = NULL;
}

int spl_mmc_load(struct spl_image_info *spl_image,
		 struct spl_boot_device *bootdev,
		 const char *filename,
		 int raw_part,
		 unsigned long raw_sect)
{
	u32 boot_mode;
	int err = 0;
	__maybe_unused int part = 0;
	int mmc_dev;

	/* Perform peripheral init only once for an mmc device */
	mmc_dev = spl_mmc_get_device_index(bootdev->boot_device);
	if (!mmc || spl_mmc_get_mmc_devnum(mmc) != mmc_dev) {
		err = spl_mmc_find_device(&mmc, bootdev->boot_device);
		if (err)
			return err;

		err = mmc_init(mmc);
		if (err) {
			mmc = NULL;
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
			printf("spl: mmc init failed with error: %d\n", err);
#endif
			return err;
		}
	}

	boot_mode = spl_mmc_boot_mode(mmc, bootdev->boot_device);
	err = -EINVAL;
	switch (boot_mode) {
	case MMCSD_MODE_EMMCBOOT:
		part = spl_mmc_emmc_boot_partition(mmc);
		if (part < 0) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
			if (part == -ENXIO)
				puts("spl: mmc booting disabled\n");
			else
				puts("spl: mmc misconfigured\n");
#endif
			return part;
		}

		if (CONFIG_IS_ENABLED(MMC_TINY))
			err = mmc_switch_part(mmc, part);
		else
			err = blk_dselect_hwpart(mmc_get_blk_desc(mmc), part);

		if (err) {
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
			puts("spl: mmc partition switch failed\n");
#endif
			return err;
		}
		/* Fall through */
	case MMCSD_MODE_RAW:
		debug("spl: mmc boot mode: raw\n");

		if (!spl_start_uboot()) {
			err = mmc_load_image_raw_os(spl_image, bootdev, mmc);
			if (!err)
				return err;
		}

		raw_sect = spl_mmc_get_uboot_raw_sector(mmc, raw_sect);

#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_PARTITION
		err = mmc_load_image_raw_partition(spl_image, bootdev,
						   mmc, raw_part,
						   raw_sect);
		if (!err)
			return err;
#endif
#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_USE_SECTOR
		err = mmc_load_image_raw_sector(spl_image, bootdev, mmc,
				raw_sect + spl_mmc_raw_uboot_offset(part));
		if (!err)
			return err;
#endif
		/* If RAW mode fails, try FS mode. */
#ifdef CONFIG_SYS_MMCSD_FS_BOOT
	case MMCSD_MODE_FS:
		debug("spl: mmc boot mode: fs\n");

		err = spl_mmc_do_fs_boot(spl_image, bootdev, mmc, filename);
		if (!err)
			return err;

		break;
#endif
#ifdef CONFIG_SPL_LIBCOMMON_SUPPORT
	default:
		puts("spl: mmc: wrong boot mode\n");
#endif
	}

	return err;
}

int spl_mmc_load_image(struct spl_image_info *spl_image,
		       struct spl_boot_device *bootdev)
{
	return spl_mmc_load(spl_image, bootdev,
#ifdef CONFIG_SPL_FS_LOAD_PAYLOAD_NAME
			    CONFIG_SPL_FS_LOAD_PAYLOAD_NAME,
#else
			    NULL,
#endif
#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_PARTITION
			    spl_mmc_boot_partition(bootdev->boot_device),
#else
			    0,
#endif
#ifdef CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR
			    CONFIG_SYS_MMCSD_RAW_MODE_U_BOOT_SECTOR);
#else
			    0);
#endif
}

SPL_LOAD_IMAGE_METHOD("MMC1", 0, BOOT_DEVICE_MMC1, spl_mmc_load_image);
SPL_LOAD_IMAGE_METHOD("MMC2", 0, BOOT_DEVICE_MMC2, spl_mmc_load_image);
SPL_LOAD_IMAGE_METHOD("MMC2_2", 0, BOOT_DEVICE_MMC2_2, spl_mmc_load_image);
