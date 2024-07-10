// SPDX-License-Identifier: GPL-2.0
/*
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
 * Authors: JackYaun <yuanxiaofeng@eswin.com>
 */

#include "mglib_driver.h"
#include "mglib_api.h"
#include "malloc.h"
#include "stdio.h"

PROGRESSBAR_S* progressBar = NULL;
TEXTBAR_S *textBar1 = NULL;
TEXTBAR_S *textBar2 = NULL;
extern FONT_S* gFont;

#define PROGRESSBAR_START_X_SCALER   8
#define PROGRESSBAR_START_Y_SCALER   6
#define PROGRESSBAR_HEIGHT_Y_SCALER  2
#define FONTSZIE   ASCII_12x24

#define FRONTPANEL_WIDTH  1280
#define FRONTPANEL_HEIGHT  720
#define BACKGROUND_WIDTH  1280
#define BACKGROUND_HEIGHT  720

static RESOLUTION_S resolution_layer1 =
{
    .w = FRONTPANEL_WIDTH,
    .h = FRONTPANEL_HEIGHT
};

static RESOLUTION_S resolution_layer2 =
{
    .w = BACKGROUND_WIDTH,
    .h = BACKGROUND_HEIGHT
};

err_no read_background_pixelData(u8* osdBuf)
{
    u32 ret = RET_OK;

    /* get background data form emmc */
    /* parse pixel data to osdBuf */
    printf("mgl_demo: %s: please finish the func\n", __func__);
    return ret;
}

void mglib_draw_progressBar_percentage(u32* osdBuf, u32 percentage)
{
    eswin_mgl_update_progressBar_percentage(progressBar, percentage, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
}

void mglib_draw_progressBar_text(u32* osdBuf, char* progressBar_text)
{
    eswin_mgl_update_progressBar_text(progressBar, progressBar_text, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
}


int mglib_updateImg_demo(void)
{
    OSD_INFO_S osd_layer1_info;
    OSD_INFO_S osd_layer2_info;
    COLOR_FMT_E colorFmt;
    u32 progressBar_x, progressBar_y, progressBar_w, progressBar_h;
    err_no ret = RET_OK;

    osd_layer1_info = eswin_mgl_get_osdBuf(OSDLAYER1);

    ret = eswin_mgl_set_resolution(OSDLAYER1, resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: set osd layer 1 resolution failed!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_set_resolution(OSDLAYER2, resolution_layer2);
    if(ret)
    {
        printf("mgl_demo: %s: set osd layer 2 resolution failed!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_set_fontSize(FONTSZIE);

    if(ret)
    {
        printf("mgl_demo: %s: unsupport fontsize\n", __func__);
        return ret;
    }

    progressBar_x = resolution_layer1.w / PROGRESSBAR_START_X_SCALER;
    progressBar_y = 4 * resolution_layer1.h / PROGRESSBAR_START_Y_SCALER;
    progressBar_w = resolution_layer1.w - progressBar_x * 2;
    progressBar_h = gFont->fontSize.h * PROGRESSBAR_HEIGHT_Y_SCALER;

    eswin_mgl_frontPanel_init(osd_layer1_info.osdBuf);

    /* progressBar  */
    static u16 percentage = 10;
    eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_BLUE_COLOR, SYSTEM_WHITE_COLOR, osd_layer1_info.osdBuf);

    char* progressBar_text = "Loading :";
    eswin_mgl_update_progressBar_text(progressBar, progressBar_text, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osd_layer1_info.osdBuf);

    eswin_mgl_update_progressBar_percentage(progressBar, percentage, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osd_layer1_info.osdBuf);

    /* version info  */
    u32 textBar1_x, textBar1_y, textBar1_w, textBar1_h;
    char* versionInfo_text = "Version : 0-0-3";
    textBar1_x = progressBar_x;
    textBar1_y = 100;
    textBar1_w = progressBar_w;
    textBar1_h = progressBar_h;
    eswin_mgl_create_textbar(&textBar1, textBar1_x, textBar1_y, textBar1_w, textBar1_h, SYSTEM_WHITE_COLOR, osd_layer1_info.osdBuf);
    eswin_mgl_update_text(textBar1, SYSTEM_BLACK_COLOR, versionInfo_text, osd_layer1_info.osdBuf);

    /* firmware path info  */
    u32 textBar2_x, textBar2_y, textBar2_w, textBar2_h;
    char* firmwarePath_text = "FirmwarePath : https://www.eswin.com/firmwarePath/update.img";
    textBar2_x = progressBar_x;
    textBar2_y = 200;
    textBar2_w = progressBar_w;
    textBar2_h = progressBar_h;
    eswin_mgl_create_textbar(&textBar2, textBar2_x, textBar2_y, textBar2_w, textBar2_h, SYSTEM_WHITE_COLOR, osd_layer1_info.osdBuf);
    eswin_mgl_update_text(textBar2, SYSTEM_BLACK_COLOR, firmwarePath_text, osd_layer1_info.osdBuf);

    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return ret;
    }

    /* draw background */
    osd_layer2_info = eswin_mgl_get_osdBuf(OSDLAYER2);
    if(ret)
    {
        printf("mgl_demo: %s: get osdBuf2 failed!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_set_resolution(OSDLAYER1, resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: set osd layer 1 resolution failed!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_set_colorFmt(OSDLAYER2, colorFmt);
    if(ret)
    {
        printf("mgl_demo: %s: set osd layer 2 colorFmt failed!\n", __func__);
        return ret;
    }

    BACKGROUNDTYPE_E bgtype = PICTURE_BG;
    u32 bgColor  = SYSTEM_BLACK_COLOR;
    ret = eswin_mgl_draw_background(bgtype, ARGB8888, bgColor, osd_layer2_info.osdBuf);
    if(ret)
    {
        printf("mgl_demo: %s: eswin_mgl_draw_background failed!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_display_update(OSDLAYER2);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return ret;
    }

    bgtype = PURECOLOR_BG;
    bgColor = SYSTEM_CYAN_COLOR;
    ret = eswin_mgl_draw_background(bgtype, ARGB8888, SYSTEM_BLACK_COLOR, osd_layer2_info.osdBuf);
    if(ret)
    {
        printf("mgl_demo: %s: eswin_mgl_draw_background failed!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_display_update(OSDLAYER2);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return ret;
    }

    return ret;
}
