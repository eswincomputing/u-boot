// SPDX-License-Identifier: GPL-2.0
/*
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
 */

#include <common.h>
#include <blk.h>
#include <u-boot/crc.h>
#include <linux/err.h>
#include <part.h>
#include <env.h>
#include <memalign.h>
#include <errno.h>
#include <update_init.h>
#include <boot_ab.h>
#include <system_update.h>
#define BOOT_BANK_START_NUM    8
/**
 * Compute the CRC-32 of the bootloader control struct.
 *
 * Only the bytes up to the crc32_le field are considered for the CRC-32
 * calculation.
 *
 * @param[in] abc bootloader control block
 *
 * @return crc32 sum
 */
uint32_t bootmessage_compute_crc(struct bootloader_message *abc)
{
    return crc32(0, (void *)abc, offsetof(typeof(*abc), crc32_bc));
}

uint32_t bootbank_compute_crc(struct boot_bank *bank)
{
    return crc32(0, (void *)bank, offsetof(typeof(*bank), crc32_bb));
}

/**
 * Store the loaded boot_control block.
 *
 * Store back to the same location it was read from with
 * ab_control_create_from_misc().
 *
 * @param[in] dev_desc Device where we should write the boot_control struct
 * @param[in] part_info Partition on the 'dev_desc' where to write
 * @param[in] abc Pointer to the boot control struct and the extra bytes after
 *                it up to the nearest block boundary
 * @return 0 on success and a negative on error
 */
int bootloader_message_store(struct blk_desc *dev_desc,
                struct disk_partition *part_info,
                struct bootloader_message *abc)
{
    ulong abc_blocks, ret;

    abc_blocks = DIV_ROUND_UP(sizeof(struct bootloader_message),
                  part_info->blksz);
    ret = blk_dwrite(dev_desc, part_info->start, abc_blocks,
             abc);
    if (IS_ERR_VALUE(ret)) {
        printf("UPDATE: Could not write back the misc partition\n");
        return -ENOENT;
    }

    return 0;
}

int boot_bank_store(struct blk_desc *dev_desc,
                struct disk_partition *part_info,
                int block, struct boot_bank *bank)
{
    ulong bank_blocks, ret;

    bank_blocks = DIV_ROUND_UP(sizeof(struct boot_bank),
                  part_info->blksz);
    ret = blk_dwrite(dev_desc, part_info->start + BOOT_BANK_START_NUM*block, bank_blocks, bank);
    if (IS_ERR_VALUE(ret)) {
        printf("UPDATE: Could not write back the misc partition\n");
        return -ENOENT;
    }

    return 0;
}

/**
 * Load the bootloader_message struct from disk into newly allocated memory.
 *
 * This function allocates and returns an integer number of disk blocks,
 * based on the block size of the passed device to help performing a
 * read-modify-write operation on the boot_control struct.
 * The boot_control struct offset (2 KiB) must be a multiple of the device
 * block size, for simplicity.
 *
 * @param[in] dev_desc Device where to read the boot_control struct from
 * @param[in] part_info Partition in 'dev_desc' where to read from, normally
 *            the "misc" partition should be used
 * @param[out] pointer to pointer to bootloader_message data
 * @return 0 on success and a negative on error
 */

int get_bootmessage_from_disk(struct blk_desc *dev_desc,
                       const struct disk_partition *part_info,
                       struct bootloader_message **abc)
{
    ulong abc_blocks, ret;
    abc_blocks = DIV_ROUND_UP(sizeof(struct bootloader_message),
                  part_info->blksz);
    if (abc_blocks > part_info->size) {
        printf("UPDATE: misc partition too small. Need at least %lu blocks but have " LBAFU " blocks.\n",
                abc_blocks, part_info->size);
        return -EIO;
    }

    *abc = malloc_cache_aligned(abc_blocks * part_info->blksz);
    if (!(*abc))
        return -EIO;

    ret = blk_dread(dev_desc, part_info->start, abc_blocks, *abc);
    if (IS_ERR_VALUE(ret)) {
        printf("UPDATE: Could not read bootloader_message from misc partition.\n");
        free(*abc);
        return -ENOENT;
    }

    // printf("UPDATE: Loaded ABC, %lu blocks\n", abc_blocks);

    return 0;
}

int get_bootbank_from_disk(struct blk_desc *dev_desc,
                       const struct disk_partition *part_info,
                       int block, struct boot_bank **bank)
{
    ulong bank_blocks, ret;

    bank_blocks = DIV_ROUND_UP(sizeof(struct boot_bank),
            part_info->blksz);
    if (bank_blocks > part_info->size) {
        printf("UPDATE: misc partition too small. Need at least %lu blocks, but have " LBAFU " blocks.\n",
                bank_blocks, part_info->size);
        return -EIO;
    }

    *bank = malloc_cache_aligned(bank_blocks * part_info->blksz);
    if (!(*bank))
        return -EIO;

    ret = blk_dread(dev_desc, part_info->start + BOOT_BANK_START_NUM*block, bank_blocks, *bank);
    if (IS_ERR_VALUE(ret)) {
        printf("UPDATE: Could not read boot_bank from misc partition.\n");
        free(*bank);
        return -ENOENT;
    }

    // printf("UPDATE: Loaded bank %lu blocks offset %d\n", bank_blocks, block);

    return 0;
}

int set_bootable_devplist(void)
{
    struct blk_desc *dev_desc;
    struct disk_partition part_info;
    struct bootloader_message *abc = NULL;
    const char *dev_part_str;
    const char *other_dev_part_str;

#if (defined CONFIG_SYSTEM_UPDATE_B) || (defined CONFIG_SYSTEM_UPDATE_C)
    if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, MISC_DEV_PART,
                &dev_desc, &part_info, true) < 0) {
        printf("part_get_info_by_dev_and_name_or_num failed!\n");
        return -ENODEV;
    }

    get_bootmessage_from_disk(dev_desc, &part_info, &abc);

    if(0 == abc->curr_bank){
        dev_part_str = UPDATE_BOOT_DEV_PART;
        other_dev_part_str = UPDATE_BOOTB_DEV_PART;
    }
    if(1 == abc->curr_bank){
        dev_part_str = UPDATE_BOOTB_DEV_PART;
        other_dev_part_str = UPDATE_BOOT_DEV_PART;
    }
    free(abc);

    if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
                &dev_desc, &part_info, true) < 0) {
        return -ENXIO;
    }

    part_info.bootable = 1;

    if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, other_dev_part_str,
                &dev_desc, &part_info, true) < 0) {
        return -ENXIO;
    }

    part_info.bootable = 0;
#endif

    return 0;
}
