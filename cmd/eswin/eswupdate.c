// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN UPDATE CMD driver
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
 */

#include <common.h>
#include <command.h>
#include <update_init.h>
#include <system_update.h>
#include <display_update.h>
#include <boot_ab.h>
#include <fs.h>
#include <ext4fs.h>
#include <errno.h>
#define FILETYPE_DIRECTORY      2

static char eswversion[STR_LENGTH];
static char eswaddress[STR_LENGTH];
static char eswstatus[STR_LENGTH];

static int do_update_boot(struct cmd_tbl *cmdtp, int flag, int argc, char * const argv[])
{
    int ret;
    int update_mode = 0;
    update_mode = ota_update_init();
    if(update_mode < 0){
        printf("UPDATE: misc/command is invalid.\n");
        strcpy(eswstatus,"status:misc/command is invalid!");
        draw_progressBar(PROGRESS_2);
        draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
        return -ENOENT;
    }
    if(UPDATE_MODE_NORMAL == update_mode)
        return 0;
    strcpy(eswversion,"version: ");
    strcpy(eswaddress,"address: ");
    strcpy(eswstatus,"status: update is beginning!");
    draw_background();
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_VER, eswversion);
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_ADD, eswaddress);
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
    draw_progressBar(PROGRESS_1);

    if(UPDATE_MODE_NORMAL != update_mode && UPDATE_MODE_RECOVERY != update_mode){
        ret = esw_update(update_mode);
        if(ret){
#ifdef CONFIG_SYSTEM_UPDATE_B
            if(-1 == ret)
                goto bringup;
#endif
#ifdef CONFIG_SYSTEM_UPDATE_C
            goto bringup;
#endif
            printf("UPDATE: esw_update failed.\n");
            return -EAGAIN;
        }
    }

    strcpy(eswstatus,"status: done!");
    draw_progressBar(PROGRESS_100);
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);

    return 0;

#if defined(CONFIG_SYSTEM_UPDATE_B) || defined(CONFIG_SYSTEM_UPDATE_C)
bringup:
    if(set_bootable_devplist()){
        strcpy(eswstatus,"status:set bootable failed");
        draw_progressBar(PROGRESS_98);
        draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
        printf("UPDATE: set_bootable_devplist failed.\n");
        return -ENOENT;
    }
#endif

}

U_BOOT_CMD(
    eswupdate, CONFIG_SYS_MAXARGS, 1, do_update_boot,
    "Exceute the update bootloader flow.",
    ""
);
