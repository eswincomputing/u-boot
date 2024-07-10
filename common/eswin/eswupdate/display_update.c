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
#include <fs.h>
#include <fat.h>
#include <image.h>
#include <mmc.h>
#include "../../../drivers/mini_graphic_library/mglib_driver.h"
#include "../../../drivers/mini_graphic_library/mglib_api.h"

#ifdef CONFIG_DRM_ESWIN_DW_HDMI
#define X_AXIS       400
#define Y_AXIS       500
#define BAR_WIDTH    500
#define BAR_HEIGHT   50
#else
#define X_AXIS       100
#define Y_AXIS       380
#define BAR_WIDTH    150
#define BAR_HEIGHT   20
#endif

int draw_progressBar(u16 percentage)
{
    OSD_INFO_S osdInfo;
    err_no ret = RET_OK;
    osdInfo = eswin_mgl_get_osdBuf(OSDLAYER1);

    u32 progressBar_x = X_AXIS;
    u32 progressBar_y = Y_AXIS;
    u32 progressBar_w = BAR_WIDTH;
    u32 progressBar_h = BAR_HEIGHT;

    PROGRESSBAR_S* progressBar = NULL;

    ret = eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_PURPLE_COLOR, SYSTEM_WHITE_COLOR, osdInfo.osdBuf);
    if(ret){
        printf("update progress : %s : create progressBar fail!\n", __func__);
        goto fail;
    }

    char* progressBar_text = "Loading :";
    ret = eswin_mgl_update_progressBar_text(progressBar, progressBar_text, SYSTEM_BLACK_COLOR, SYSTEM_DARK_GRAY_COLOR, osdInfo.osdBuf);
    if(ret){
        printf("update progress : %s : update text fail!\n", __func__);
        goto fail;
    }

    ret = eswin_mgl_update_progressBar_percentage(progressBar, percentage, SYSTEM_BLACK_COLOR, SYSTEM_DARK_GRAY_COLOR, osdInfo.osdBuf);
    if(ret){
        printf("update progress : %s : update percentage fail!\n", __func__);
        goto fail;
    }

    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret < 0){
        printf("eswin_mgl_display_update fail!\n");
        goto fail;
    }

    return 0;

fail:
    eswin_mgl_distory_progressBar(&progressBar);
    return ret;
}

int draw_background(void)
{
    err_no ret = RET_OK;
    OSD_INFO_S osdInfo;
    osdInfo = eswin_mgl_get_osdBuf(OSDLAYER1);
    /* draw background */
    ret = eswin_mgl_draw_background(PURECOLOR_BG, ARGB8888, SYSTEM_DARK_GRAY_COLOR, osdInfo.osdBuf);
    if(ret){
        printf("update progress: %s : draw background fail!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_display_update(OSDLAYER1);

    return ret;

}

int draw_updateinfo(u32 textBar_x, u32 textBar_y, char* versionInfo_text)
{
    OSD_INFO_S osdInfo;
    err_no ret = RET_OK;
    osdInfo = eswin_mgl_get_osdBuf(OSDLAYER1);
    printf("%s \n", versionInfo_text);
    /* show information of upgrade package */
    TEXTBAR_S* textBar = NULL;
    u32 textBar_w = BAR_WIDTH;
    u32 textBar_h = BAR_HEIGHT;

    ret = eswin_mgl_create_textbar(&textBar, textBar_x, textBar_y, textBar_w, textBar_h, SYSTEM_WHITE_COLOR, osdInfo.osdBuf);
    if(ret){
        printf("update progress : %s : draw textbar of version number fail!\n", __func__);
        goto fail;
    }

    ret = eswin_mgl_update_text(textBar, SYSTEM_BLACK_COLOR, versionInfo_text, osdInfo.osdBuf);
    if(ret){
        printf("update progress : %s : draw information of version number fail!\n", __func__);
        goto fail;
    }

    ret = eswin_mgl_display_update(OSDLAYER1);
    return ret;

fail:
    eswin_mgl_distory_textbar(&textBar);

    return ret;
}