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
 */

#ifndef __DISPLAY_LOGO_H__
#define __DISPLAY_LOGO_H__

#include "../mini_graphic_library/mglib_driver.h"

typedef struct _PICTURESRC_S
{
    u32 blkStartAddr;
    u32 blkCnt;
    u32 blkSize;
    COLOR_FMT_E colorFmt;
}PICTURESRC_S;

err_no eswin_display_logo_get_logo_data(u8* osdBuf, RESOLUTION_S resolution_osd, COLOR_FMT_E colorFmt_osd);

#endif
