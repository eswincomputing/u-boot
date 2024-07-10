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

#include "../mglib_driver.h"
#include "../mglib_api.h"
#include "malloc.h"
#include "stdio.h"
#include "linux/delay.h"

extern FONT_S* gFont;

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

#if 0
static RESOLUTION_S resolution_layer2 =
{
    .w = BACKGROUND_WIDTH,
    .h = BACKGROUND_HEIGHT
};
#endif

/* ponit test */
err_no eswin_mgl_draw_point_test(u32* osdBuf)
{
    err_no ret = RET_OK;

    ret = eswin_mgl_get_resolution(OSDLAYER1, &resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: invalid argument\n", __func__);
        printf("mgl test : %s : test failed!\n", __func__);
        return ERR_INVAL;
    }

    /* vaild */
    POINT_S point1 = {600,600};
    /* invalid x */
    POINT_S point2 = {resolution_layer1.w + 100,600};
    /* invalid y */
    POINT_S point3 = {600,resolution_layer1.w + 100};

    /* point test 1 */
    ret = eswin_mgl_draw_point(point1.x, point2.y, SYSTEM_YELLOW_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 success!\n", __func__);
    else
        printf("mgl test : %s : test 1 fail!\n", __func__);

    /* point test 2*/
    ret = eswin_mgl_draw_point(point2.x, point2.y, SYSTEM_YELLOW_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 2 success!\n", __func__);
    else
        printf("mgl test : %s : test 2 fail!\n", __func__);

    /* point test 3*/
    eswin_mgl_draw_point(point3.x, point3.y, SYSTEM_YELLOW_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 3 success!\n", __func__);
    else
        printf("mgl test : %s : test 3 fail!\n", __func__);

    return ret;
}

/* line test */
err_no eswin_mgl_draw_line_test(u32* osdBuf)
{
    err_no ret = RET_OK;

    ret = eswin_mgl_get_resolution(OSDLAYER1, &resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: invalid argument\n", __func__);
        printf("mgl test : %s : test failed!\n", __func__);
        return ERR_INVAL;
    }

    /* draw line */
    POINT_S point1 = {720,320};
    POINT_S point2 = {1200,320};
    POINT_S point3 = {720,680};
    /* x invalid */
    POINT_S point4 = {resolution_layer1.w + 100, 680};
    /* y invalid */
    POINT_S point5 = {720,resolution_layer1.h + 100};

    /* test 1 */
    ret = eswin_mgl_draw_line(point1, point2, SYSTEM_CYAN_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 success!\n", __func__);
    else
        printf("mgl test : %s : test 1 fail!\n", __func__);

    /* test 2 */
    ret = eswin_mgl_draw_line(point1, point3, SYSTEM_YELLOW_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 2 success!\n", __func__);
    else
        printf("mgl test : %s : test 2 fail!\n", __func__);

    /* test 3 */
    ret = eswin_mgl_draw_line(point2, point3, SYSTEM_RED_COLOR, osdBuf);
    if(ERR_PERM == ret)
        printf("mgl test : %s : test 3 success!\n", __func__);
    else
        printf("mgl test : %s : test 3 fail!\n", __func__);

    /* test 4 */
    ret = eswin_mgl_draw_line(point1, point4, SYSTEM_BLUE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 4 success!\n", __func__);
    else
        printf("mgl test : %s : test 4 fail!\n", __func__);

    /* test 5 */
    ret = eswin_mgl_draw_line(point1, point5, SYSTEM_GREEN_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 5 success!\n", __func__);
    else
        printf("mgl test : %s : test 5 fail!\n", __func__);

    /* test 6 */
    ret = eswin_mgl_draw_line(point4, point1, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 6 success!\n", __func__);
    else
        printf("mgl test : %s : test 6 fail!\n", __func__);

    /* test 7 */
    ret = eswin_mgl_draw_line(point5, point1, SYSTEM_PURPLE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 7 success!\n", __func__);
    else
        printf("mgl test : %s : test 7 fail!\n", __func__);

    return ret;
}

/* draw string test */
err_no eswin_mgl_draw_string_test(u32* osdBuf)
{
    err_no ret = RET_OK;

    ret = eswin_mgl_get_resolution(OSDLAYER1, &resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: invalid argument\n", __func__);
        printf("mgl test : %s : test failed!\n", __func__);
        return ERR_INVAL;
    }

    u16 x = 100, y = 200;
    char* str = "This is a test string";

    /* test 1 */
    ret = eswin_mgl_draw_string(x, y, str, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 success!\n", __func__);
    else
        printf("mgl test : %s : test 1 fail!\n", __func__);

    /* test 2 */
    str = "This is a too long string................................................a too long";
    ret = eswin_mgl_draw_string(x, y, str, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 2 success!\n", __func__);
    else
        printf("mgl test : %s : test 2 fail!\n", __func__);

    /* test 3 */
    str = "This is a test string";
    x = resolution_layer1.w + 100;
    ret = eswin_mgl_draw_string(x, y, str, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 3 success!\n", __func__);
    else
        printf("mgl test : %s : test 3 fail!\n", __func__);

    /* test 4 */
    y = resolution_layer1.h + 100;
    ret = eswin_mgl_draw_string(x, y, str, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 4 success!\n", __func__);
    else
        printf("mgl test : %s : test 4 fail!\n", __func__);

    /* test 5 : draw a cahr that is not in font lib*/
    str = "This is a test 牛 string";
    x = 300;
    /* will have the printf info that a char is not support */
    ret = eswin_mgl_draw_string(x, y, str, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 5 success!\n", __func__);
    else
        printf("mgl test : %s : test 5 fail!\n", __func__);

    return ret;
}

/* textBar test */
err_no eswin_mgl_draw_textbar_test(u32* osdBuf)
{
    err_no ret = RET_OK;

    ret = eswin_mgl_get_resolution(OSDLAYER1, &resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: invalid argument\n", __func__);
        printf("mgl test : %s : test failed!\n", __func__);
        return ERR_INVAL;
    }

    TEXTBAR_S* textBar1 = NULL;
    /* test 1  : draw one line */
    u32 textBar1_x, textBar1_y, textBar1_w, textBar1_h;
    char* versionInfo_text = "Version : 0-0-3";
    textBar1_x = 100;
    textBar1_y = 100;
    textBar1_w = 800;
    textBar1_h = 50;
    ret = eswin_mgl_create_textbar(&textBar1, textBar1_x, textBar1_y, textBar1_w, textBar1_h, SYSTEM_WHITE_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 success!\n", __func__);
    else
        printf("mgl test : %s : test 1 fail!\n", __func__);
    ret = eswin_mgl_update_text(textBar1, SYSTEM_BLACK_COLOR, versionInfo_text, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 success!\n", __func__);
    else
        printf("mgl test : %s : test 1 fail!\n", __func__);

    /* test 2 : draw two line */
    versionInfo_text = "1234567890abcdef1234567890abcdef";
    ret = eswin_mgl_update_text(textBar1, SYSTEM_BLACK_COLOR, versionInfo_text, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 2 success!\n", __func__);
    else
        printf("mgl test : %s : test 2 fail!\n", __func__);

    /* test 3 : draw three line */
    versionInfo_text = "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcedf1234567890abcdef123";
    ret = eswin_mgl_update_text(textBar1, SYSTEM_BLACK_COLOR, versionInfo_text, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 3 success!\n", __func__);
    else
        printf("mgl test : %s : test 3 fail!\n", __func__);

    /* test 4 : string too long */
    versionInfo_text = "1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcedf1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef1234567890abcdef";
    ret = eswin_mgl_update_text(textBar1, SYSTEM_BLACK_COLOR, versionInfo_text, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 4 success!\n", __func__);
    else
        printf("mgl test : %s : test 4 fail!\n", __func__);
    eswin_mgl_distory_textbar(&textBar1);

    /* test 5 : invalid x */
    TEXTBAR_S* textBar4 = NULL;
    u32 textBar4_x, textBar4_y, textBar4_w, textBar4_h;
    versionInfo_text = "1234567890abcdef";
    textBar4_x = resolution_layer1.w + 100;
    textBar4_y = 200;
    textBar4_w = 480;
    textBar4_h = 50;
    ret = eswin_mgl_create_textbar(&textBar4, textBar4_x, textBar4_y, textBar4_w, textBar4_h, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 5 success!\n", __func__);
    else
        printf("mgl test : %s : test 5 fail!\n", __func__);
    eswin_mgl_distory_textbar(&textBar4);

    /* test 6 : invalid y */
    textBar4_x = 200;
    textBar4_y = resolution_layer1.h + 100;
    textBar4_w = 480;
    textBar4_h = 50;
    ret = eswin_mgl_create_textbar(&textBar4, textBar4_x, textBar4_y, textBar4_w, textBar4_h, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 6 success!\n", __func__);
    else
        printf("mgl test : %s : test 6 fail!\n", __func__);
    eswin_mgl_distory_textbar(&textBar4);

    /* test 7 invalid w */
    textBar4_x = 200;
    textBar4_y = 200;
    textBar4_w = resolution_layer1.w + 100;
    textBar4_h = 50;
    ret = eswin_mgl_create_textbar(&textBar4, textBar4_x, textBar4_y, textBar4_w, textBar4_h, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 7 success!\n", __func__);
    else
        printf("mgl test : %s : test 7 fail!\n", __func__);
    eswin_mgl_distory_textbar(&textBar4);

    /* test 8 invalid h */
    textBar4_x = 200;
    textBar4_y = 200;
    textBar4_w = 480;
    textBar4_h = resolution_layer1.h;
    ret = eswin_mgl_create_textbar(&textBar4, textBar4_x, textBar4_y, textBar4_w, textBar4_h, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 8 success!\n", __func__);
    else
        printf("mgl test : %s : test 8 fail!\n", __func__);
    eswin_mgl_distory_textbar(&textBar4);

    return ret;
}

/* draw rect test */
err_no eswin_mgl_draw_rect_test(u32* osdBuf)
{
    err_no ret = RET_OK;

    u16 x = 100;
    u16 y = 100;
    u16 w = 800;
    u16 h = 100;

    ret = eswin_mgl_get_resolution(OSDLAYER1, &resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: invalid argument\n", __func__);
        printf("mgl test : %s : test failed!\n", __func__);
        return ERR_INVAL;
    }

    /* test 1 */
    ret  = eswin_mgl_draw_rect(x, y, w, h, SYSTEM_YELLOW_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 success!\n", __func__);
    else
        printf("mgl test : %s : test 1 fail!\n", __func__);

    x = resolution_layer1.w + 100;
    y = 100;
    w = 800;
    h = 100;
    /* test 2 */
    ret  = eswin_mgl_draw_rect(x, y, w, h, SYSTEM_YELLOW_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 2 success!\n", __func__);
    else
        printf("mgl test : %s : test 2 fail!\n", __func__);

    x = 200;
    y = resolution_layer1.h + 100;
    w = 800;
    h = 100;
    /* test 3 */
    ret  = eswin_mgl_draw_rect(x, y, w, h, SYSTEM_YELLOW_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 3 success!\n", __func__);
    else
        printf("mgl test : %s : test 3 fail!\n", __func__);

    x = 200;
    y = 100;
    w = resolution_layer1.w;
    h = 100;
    /* test 4 */
    ret  = eswin_mgl_draw_rect(x, y, w, h, SYSTEM_YELLOW_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 4 success!\n", __func__);
    else
        printf("mgl test : %s : test 4 fail!\n", __func__);

    x = 200;
    y = 100;
    w = 800;
    h = resolution_layer1.h;
    /* test 5 */
    ret  = eswin_mgl_draw_rect(x, y, w, h, SYSTEM_YELLOW_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 5 success!\n", __func__);
    else
        printf("mgl test : %s : test 5 fail!\n", __func__);

    return ret;
}

err_no eswin_mgl_draw_progressBar_test(u32* osdBuf)
{
    err_no ret = RET_OK;

    u32 progressBar_x = 300;
    u32 progressBar_y = 100;
    u32 progressBar_w = 500;
    u32 progressBar_h = 50;

    PROGRESSBAR_S* progressBar = NULL;

    ret = eswin_mgl_get_resolution(OSDLAYER1, &resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: invalid argument\n", __func__);
        printf("mgl test : %s : test failed!\n", __func__);
        return ERR_INVAL;
    }

    /* test1  */
    static u16 percentage = 10;
    ret = eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_BLUE_COLOR, SYSTEM_WHITE_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 create progressBar success!\n", __func__);
    else
        printf("mgl test : %s : test 1 create progressBar fail!\n", __func__);

    char* progressBar_text = "Loading :";
    ret = eswin_mgl_update_progressBar_text(progressBar, progressBar_text, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1  update text success!\n", __func__);
    else
        printf("mgl test : %s : test 1 update text fail!\n", __func__);

    ret = eswin_mgl_update_progressBar_percentage(progressBar, percentage, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 update percentage success!\n", __func__);
    else
        printf("mgl test : %s : test 1 update percentage fail!\n", __func__);
    eswin_mgl_distory_progressBar(&progressBar);

    /* test2 : invalid x */
    progressBar_x = resolution_layer1.w + 100;
    progressBar_y = 100;
    progressBar_w = 500;
    progressBar_h = 50;

    ret = eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_BLUE_COLOR, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 2 success!\n", __func__);
    else
        printf("mgl test : %s : test 2 fail!\n", __func__);
    eswin_mgl_distory_progressBar(&progressBar);

    /* test3 : invalid y */
    progressBar_x = 100;
    progressBar_y = resolution_layer1.h + 100;
    progressBar_w = 500;
    progressBar_h = 50;

    ret = eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_BLUE_COLOR, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 3 success!\n", __func__);
    else
        printf("mgl test : %s : test 3 fail!\n", __func__);
    eswin_mgl_distory_progressBar(&progressBar);

    /* test4 : invalid w */
    progressBar_x = 100;
    progressBar_y = 100;
    progressBar_w = resolution_layer1.w + 100;
    progressBar_h = 50;

    ret = eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_BLUE_COLOR, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 4 success!\n", __func__);
    else
        printf("mgl test : %s : test 4 fail!\n", __func__);
    eswin_mgl_distory_progressBar(&progressBar);

    /* test5 : invalid h */
    progressBar_x = 100;
    progressBar_y = 100;
    progressBar_w = 500;
    progressBar_h = resolution_layer1.h + 100;

    ret = eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_BLUE_COLOR, SYSTEM_WHITE_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 5 success!\n", __func__);
    else
        printf("mgl test : %s : test 5 fail!\n", __func__);
    eswin_mgl_distory_progressBar(&progressBar);

    /* test6 : invalid percentage  */
    progressBar_x = 100;
    progressBar_y = 100;
    progressBar_w = 500;
    progressBar_h = 100;
    percentage = 101;

    ret = eswin_mgl_update_progressBar_percentage(progressBar, percentage, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 6 success!\n", __func__);
    else
        printf("mgl test : %s : test 6 fail!\n", __func__);
    eswin_mgl_distory_progressBar(&progressBar);

    return ret;
}

/* draw barckground test */
err_no eswin_mgl_draw_background_test(u32* osdBuf)
{
    err_no ret = RET_OK;
    BACKGROUNDTYPE_E type = PURECOLOR_BG;
    /* test 1 */
    ret = eswin_mgl_draw_background(type, ARGB8888, SYSTEM_CYAN_COLOR, osdBuf);
    if(RET_OK == ret)
        printf("mgl test : %s : test 1 success!\n", __func__);
    else
        printf("mgl test : %s : test 1 fail!\n", __func__);

    /* test 2*/
    ret = eswin_mgl_draw_background(2, ARGB8888, SYSTEM_CYAN_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 2 success!\n", __func__);
    else
        printf("mgl test : %s : test 2 fail!\n", __func__);

    ret = eswin_mgl_draw_background(type, RGB888, SYSTEM_CYAN_COLOR, osdBuf);
    if(ERR_INVAL == ret)
        printf("mgl test : %s : test 3 success!\n", __func__);
    else
        printf("mgl test : %s : test 3 fail!\n", __func__);

    return ret;
}

err_no eswin_mgl_draw_progressBar_dyn_test(u32* osdBuf)
{
    err_no ret = RET_OK;

    u32 progressBar_x = 300;
    u32 progressBar_y = 100;
    u32 progressBar_w = 500;
    u32 progressBar_h = 50;

    PROGRESSBAR_S* progressBar = NULL;

    ret = eswin_mgl_get_resolution(OSDLAYER1, &resolution_layer1);
    if(ret)
    {
        printf("mgl_demo: %s: invalid argument\n", __func__);
        printf("mgl test : %s : test failed!\n", __func__);
        return ERR_INVAL;
    }

    /* test1  */
    u16 percentage = 0;
    ret = eswin_mgl_create_progressbar(&progressBar, progressBar_x, progressBar_y, progressBar_w, progressBar_h, SYSTEM_BLUE_COLOR, SYSTEM_WHITE_COLOR, osdBuf);

    char* progressBar_text = "Loading :";
    ret = eswin_mgl_update_progressBar_text(progressBar, progressBar_text, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);

    for(; percentage <= 100; percentage++)
    {
        ret = eswin_mgl_update_progressBar_percentage(progressBar, percentage, SYSTEM_GREEN_COLOR, SYSTEM_BLACK_COLOR, osdBuf);
        ret = eswin_mgl_display_update(OSDLAYER1);
        if(ret)
        {
            printf("mgl_demo: %s: update display failed!\n", __func__);
            return ret;
        }
    }

    return ret;
}

err_no mgl_test_init(u32* osdBuf)
{
    err_no ret = RET_OK;
    ret = eswin_mgl_set_fontSize(FONTSZIE);
    if(ret)
    {
        printf("mgl_demo: %s: unsupport fontsize\n", __func__);
        return ERR_INVAL;
    }

    eswin_mgl_frontPanel_init(osdBuf);
    return ret;
}

void mglib_test_all(u32* osdBuf)
{
    err_no ret = RET_OK;
    eswin_mgl_draw_point_test(osdBuf);
    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return;
    }
    mdelay(10000);

    eswin_mgl_draw_line_test(osdBuf);
    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return;
    }
    mdelay(10000);

    eswin_mgl_draw_string_test(osdBuf);
    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return;
    }
    mdelay(10000);

    eswin_mgl_draw_textbar_test(osdBuf);
    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return;
    }
    mdelay(10000);

    eswin_mgl_draw_rect_test(osdBuf);
    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return;
    }
    mdelay(10000);

    /*
     * clear the background
     */
    eswin_mgl_draw_background_test(osdBuf);
    eswin_mgl_draw_progressBar_dyn_test(osdBuf);
    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return;
    }
    mdelay(10000);

    eswin_mgl_draw_background_test(osdBuf);
}

err_no mgl_test_demo(void)
{
    err_no ret = RET_OK;
    OSD_INFO_S osd_layer1_info = eswin_mgl_get_osdBuf(OSDLAYER1);
    OSD_INFO_S osd_layer2_info = eswin_mgl_get_osdBuf(OSDLAYER2);

    ret = eswin_mgl_set_resolution(OSDLAYER1, osd_layer1_info.resolution);
    if(ret)
    {
        printf("mgl_demo: %s: set osd layer 1 resolution failed!\n", __func__);
        return ret;
    }

    ret = eswin_mgl_set_resolution(OSDLAYER2, osd_layer2_info.resolution);
    if(ret)
    {
        printf("mgl_demo: %s: set osd layer 2 resolution failed!\n", __func__);
        return ret;
    }

    mgl_test_init(osd_layer1_info.osdBuf);

    /*
     * [> add all test here, for example : draw line test  <]
     * eswin_mgl_draw_point_test(osd_layer1_info.osdBuf);
     */

    mglib_test_all(osd_layer1_info.osdBuf);
    ret = eswin_mgl_display_update(OSDLAYER1);
    if(ret)
    {
        printf("mgl_demo: %s: update display failed!\n", __func__);
        return ret;
    }

    return ret;
}


