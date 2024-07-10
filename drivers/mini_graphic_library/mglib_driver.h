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

#ifndef __MGLIB_DRVIER_H__
#define __MGLIB_DRVIER_H__

#include "error_code.h"

typedef unsigned char    u8;
typedef unsigned short   u16;
typedef unsigned int     u32;

typedef signed char      s8;
typedef signed short     s16;
typedef signed int       s32;

//ARGB
#define SYSTEM_WHITE_COLOR                  0xffffffff   //white
#define SYSTEM_CYAN_COLOR                   0xff00ffff   //cyan
#define SYSTEM_PURPLE_COLOR                 0xffa020f0   //purple
#define SYSTEM_BLUE_COLOR                   0xff0000ff   //blue
#define SYSTEM_YELLOW_COLOR                 0xffffff00   //yellow
#define SYSTEM_GREEN_COLOR                  0xff00ff00   //green
#define SYSTEM_RED_COLOR                    0xffff0000   //red
#define SYSTEM_DARK_GRAY_COLOR              0xffA9A9A9   //dark gray
#define SYSTEM_LIGHT_GRAY_COLOR             0xffd3d3d3   //light gray
#define SYSTEM_BLANK_COLOR                  0x00000000   //blank
#define SYSTEM_BLACK_COLOR                  0xff000000   //black
#define SYSTEM_DARK_RED_COLOR               0xff8b0000  //dark red
#define SYSTEM_DARK_GREEN_COLOR             0xff006400  //dark green
#define SYSTEM_DARK_BLUE_COLOR              0xff00008b  //dark blue
#define SYSTEM_DARK_CYAN_COLOR              0xff008b8b  //dark cyan

typedef u32 err_no;

typedef struct _RESOLUTION_S
{
    u16 w;
    u16 h;
} RESOLUTION_S;

typedef enum _OSDLAYER_E
{
    OSDLAYER1 = 0,
    OSDLAYER2
} OSDLAYER_E;

typedef enum _BACKGROUNDTYPE_E
{
    PICTURE_BG   = 0,
    PURECOLOR_BG
} BACKGROUNDTYPE_E;

typedef struct _POINT_S
{
    u32 x;
    u32 y;
} POINT_S;

typedef struct _CHAR_SIZE_S
{
    u16 w;
    u16 h;
} CHAR_SIZE_S;

typedef struct _DRAW_CHAR_S
{
    POINT_S       position;
    CHAR_SIZE_S   charSize;
    u32           charColor;
    u32           charbgColor;
    u8            *pData;
} DRAW_CHAR_S;

typedef struct _FONT_S
{
    char fontName[16];
    u8* fontData;
    CHAR_SIZE_S fontSize;
} FONT_S;

typedef enum _COLOR_FMT_E
{
    ARGB8888 = 32,
    RGB888 = 24,
    RGB565 = 16
} COLOR_FMT_E;

/*
 * Font size enum
 * ASCII_24 : width =16, height = 24;
 * If supprot other size , please add here
 */
typedef enum _FONT_SIZE_E
{
    ASCII_12x24 = 0,
} FONT_SIZE_E;

typedef struct _OSD_INFO
{
    u32* osdBuf;
    RESOLUTION_S resolution;
    COLOR_FMT_E colorFmt;
} OSD_INFO_S;


err_no eswin_mgl_display_update(OSDLAYER_E);
err_no eswin_mgl_draw_rect(u16 x, u16 y, u16 length, u16 height, u32 fillColor, u32* osdBuf);
err_no eswin_mgl_draw_point(u16 x, u16 y, u32 color, u32* osdBuf);
err_no eswin_mgl_draw_line(POINT_S point1, POINT_S point2, u32 color, u32* osdBuf);
err_no eswin_mgl_draw_background(BACKGROUNDTYPE_E type, COLOR_FMT_E colorFmt, u32 bgColor, void* osdBuf);
void eswin_mgl_exit(void);
u32 _strlen(const void *vstr);
err_no eswin_mgl_draw_string(u16 x, u16 y, char* string, u32 charColor, u32 charbgColor, u32* osdBuf);
err_no eswin_mgl_set_resolution(OSDLAYER_E layer, RESOLUTION_S resolution);
err_no eswin_mgl_set_colorFmt(OSDLAYER_E layer, COLOR_FMT_E colorFmt);
err_no eswin_mgl_set_osdBuf (OSDLAYER_E layer, RESOLUTION_S resolution, COLOR_FMT_E colorFmt);
OSD_INFO_S eswin_mgl_get_osdBuf(OSDLAYER_E layer);
void eswin_mgl_set_default_color(RESOLUTION_S resolution, u32 color, u32* osdBuf);
err_no eswin_mgl_set_fontSize(FONT_SIZE_E size);
err_no eswin_mgl_get_resolution(OSDLAYER_E layer, RESOLUTION_S* resolution);

#endif
