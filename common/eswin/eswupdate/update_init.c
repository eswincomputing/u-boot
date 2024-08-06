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
#include <mmc.h>
#include <usb.h>
#include <fs.h>
#include <u-boot/crc.h>
#include <linux/err.h>
#include <errno.h>
#include <update_init.h>
#include <boot_ab.h>
#include <system_update.h>

static int get_misc_command(struct blk_desc *dev_desc, struct disk_partition *part_info)
{
    int ret;
    struct bootloader_message *abc = NULL;

    ret = get_bootmessage_from_disk(dev_desc, part_info, &abc);

    if(ret < 0){
        printf("UPDATE: get bootloader_message failed!\n");
        return -ENOENT;
    }
    printf("bootloader_message info\n \
            magic : 0x%08x\t command : %s\t crc32_bc : 0x%08x \n\
            kernel : boot%s \t rootfs : rootfs%s \n\
            ", abc->magic, abc->command, abc->crc32_bc,\
            ( abc->curr_bank == 0 )?"a":  ( abc->curr_bank == 1)?"b":" err", \
            ( abc->rtfs_bank == 0 )?"a":  ( abc->rtfs_bank == 1)?"b":" err");
    if (!strcmp(abc->command, "boot_normal")){
        return UPDATE_MODE_NORMAL;
    }

    if (!strcmp(abc->command, "boot_ota")){
        return UPDATE_MODE_OTA;
    }

    if (!strcmp(abc->command, "boot_sd")){
        return UPDATE_MODE_OTA_SD;
    }

    if (!strcmp(abc->command, "boot_usb")){
        return UPDATE_MODE_OTA_USB;
    }

    if (!strcmp(abc->command, "boot_recovery")){
        return UPDATE_MODE_RECOVERY;
    }

    return UPDATE_MODE_NORMAL;
}

int ota_update_init(void)
{
    int ret;
    struct blk_desc *dev_desc;
    struct disk_partition part_info;

    dev_desc = blk_get_dev(MMC_DEV_IFACE, MMC_DEV);

    if (dev_desc == NULL) {
        printf("UPDATE: Block device %s %d not supported.\n",
                MMC_DEV_IFACE, MMC_DEV);
        return -ENODEV;
    }

    if(part_get_info_by_name(dev_desc, PART_MISC, &part_info) < 0){
        printf("part_get_info_by_name failed\n");
        return -ENODEV;
    }

    ret = get_misc_command(dev_desc, &part_info);

    return ret;
}

void ota_update_exit(void)
{
}

int mmc_update_init(void)
{
    int ret;
    struct mmc *mmc;

    mmc = find_mmc_device(strtoul(SD_DEV_PART, NULL, 10));
    //mmc = find_mmc_device(SD_DEV_PART);
    if(NULL == mmc) {
        printf("No mmc driver found!\n");
        return -ENXIO;
    }

#if !CONFIG_IS_ENABLED(BLK)
    if((0 == (unsigned int)mmc->block_dev.vendor[0])
            || (0 == (unsigned int)mmc->block_dev.product[0])) {
        printf("No SD card found!\n");
        return -ENXIO;
    }
#endif

    ret = file_exists(MMC_DEV_IFACE, SD_DEV_PART, UPDATE_FILE_NAME,
            FS_TYPE_FAT);
    if(ret < 1){
        printf("File %s no found in device SD!\n", UPDATE_FILE_NAME);
        return -ENOENT;
    }

    return UPDATE_MODE_SD;
}

void mmc_update_exit(void)
{

}

int usb_update_init(void)
{
    int ret = -1;
    // extern char usb_started;
    // if (usb_started)
        // goto out;

    if(usb_init() < 0){
        printf("usb_init failed!\n");
        return -EAGAIN;
    }

    ret = usb_stor_scan(0);
    if (-1 == ret){
        printf("No USB device found. Not initialized!\n");
        usb_stop();
        return ret;
    }
out:
    ret = file_exists(USB_DEV_IFACE, USB_DEV_PART, UPDATE_FILE_NAME,
            FS_TYPE_FAT);
    if(ret < 1){
        printf("File %s no found in device USB!\n", UPDATE_FILE_NAME);
        usb_stop();
        return -ENOENT;
    }

    return UPDATE_MODE_USB;
}

void usb_update_exit(void)
{
    usb_stop();
}
