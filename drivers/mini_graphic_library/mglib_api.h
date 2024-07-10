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

#ifndef __MGLIB_API_H__
#define __MGLIB_API_H__

#include "mglib_driver.h"

typedef struct _PROGRESSBAR_S
{
    u16 x;
    u16 y;
    u16 length;
    u16 height;
    u16 text_x;
    u16 text_y;
    u16 percentage;
    u32 percentage_x;
    u32 percentage_y;
    u32 highlightColor;
    u32 bgColor;
} PROGRESSBAR_S;

typedef struct _TEXTBAR_S
{
    u16 x;
    u16 y;
    u16 length;
    u16 height;
    u16 text_x;
    u16 text_y;
    u32 bgColor;
    u32 charColor;
    u16 maxCharNums;
    u16 lastCharNums;
} TEXTBAR_S;

err_no eswin_mgl_create_progressbar(PROGRESSBAR_S** progressBar, u32 x, u32 y, u32 w, u32 h, u32 highlightColor, u32 bgColor, u32* osdBuf);
err_no eswin_mgl_update_progressBar_percentage(PROGRESSBAR_S* progressBar, u16 percentage, u32 charColor, u32 charbgColor, u32* osdBuf);
err_no eswin_mgl_update_progressBar_text(PROGRESSBAR_S* progressBar, char* string, u32 charColor, u32 charbgColor, u32* osdBuf);
err_no eswin_mgl_create_textbar(TEXTBAR_S** textBar, u32 x, u32 y, u32 w, u32 h, u32 bgColor, u32* osdBuf);
err_no eswin_mgl_update_text(TEXTBAR_S* textBar, u32 charColor, char* string, u32* osdBuf);
void eswin_mgl_frontPanel_init(u32* osdBuf);
void eswin_mgl_distory_textbar(TEXTBAR_S** textBar);
void eswin_mgl_distory_progressBar(PROGRESSBAR_S** progressBar);

#endif
