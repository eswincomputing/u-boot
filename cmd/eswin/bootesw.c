// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN boot info CMD driver
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
 */

#include <common.h>
#include <command.h>
#include <errno.h>
#include <blk.h>
#include <update_init.h>
#include <system_update.h>
#include <boot_ab.h>
#include <display_update.h>
static struct blk_desc *mmc_dev_desc;
static struct disk_partition misc_part_info;
static struct bootloader_message *abc = NULL;
static struct boot_bank *bank = NULL;

static int get_misc_info(void)
{
    int block = 1;

    if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, MISC_DEV_PART,
                &mmc_dev_desc, &misc_part_info,true) < 0) {
        printf("UPDATE: Get information of misc partiyion failed!\n");
        return -ENOENT;
    }

    get_bootmessage_from_disk(mmc_dev_desc, &misc_part_info, &abc);
    get_bootbank_from_disk(mmc_dev_desc, &misc_part_info, block, &bank);

    return 0;
}

int do_bootesw(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
    char str_kernel[100] = {'\0'};
    char str_rootfs[100] = {'\0'};

    int ret = 0;
    ret = get_misc_info();
    if(ret < 0)
        return -ENODEV;
#ifdef CONFIG_TRIES_REMAINING
    int curr_bank = 0;
    if(0 == abc->tries_remaining){
#ifdef CONFIG_SYSTEM_UPDATE_B
        if(0 == abc->curr_bank){
            curr_bank = 1;
        }else if(1 == abc->curr_bank){
            curr_bank = 0;
        }
#endif
        abc->curr_bank = curr_bank;
    }
    else if(0 < abc->tries_remaining){
        if(0 == abc->curr_bank){
            abc->tries_remaining--;
            strcpy(str_kernel,"fatload mmc 0#boota 0x90000000 fitimage");
        } else if(1 == abc->curr_bank){
            abc->tries_remaining--;
            strcpy(str_kernel,"fatload mmc 0#bootb 0x90000000 fitimage");
        }
    }
#endif

    if (0 == abc->curr_bank){
        strcpy(str_kernel,"fatload mmc 0#boota 0x90000000 fitimage");
    } else if (1 == abc->curr_bank){
        strcpy(str_kernel,"fatload mmc 0#bootb 0x90000000 fitimage");
    }

    if (0 == abc->rtfs_bank){
        strcpy(str_rootfs,"/dev/mmcblk0p6 ro rootfstype=ext4 rootwait");
    } else {
        strcpy(str_rootfs,"/dev/mmcblk0p7 ro rootfstype=ext4 rootwait");
    }

    env_set("loadimage", str_kernel);
    env_set("mmcroot", str_rootfs);
    // env_save();

    return CMD_RET_SUCCESS;
}

U_BOOT_CMD(
    bootesw, CONFIG_SYS_MAXARGS, 1, do_bootesw,
    "Start kernel.",
    ""
);

