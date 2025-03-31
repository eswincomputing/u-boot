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
 * Authors: DengLei <denglei@eswincomputing.com>
 */

#ifndef VIDEO_ESWIN_H
#define VIDEO_ESWIN_H

#include <dm/device.h>

#ifndef CONFIG_ESWIN_LOGO_DISPLAY
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
 #define DRM_ESWIN_FB_WIDTH             1920
 #define DRM_ESWIN_FB_HEIGHT            1080
 #define DRM_ESWIN_FB_BPP               VIDEO_BPP32
#else
 #define DRM_ESWIN_FB_WIDTH             1080
 #define DRM_ESWIN_FB_HEIGHT            1920
 #define DRM_ESWIN_FB_BPP               VIDEO_BPP32
#endif
#else
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
 #define DRM_ESWIN_FB_WIDTH             1280
 #define DRM_ESWIN_FB_HEIGHT            720
 #define DRM_ESWIN_FB_BPP               VIDEO_BPP16
#else
 #define DRM_ESWIN_FB_WIDTH             320
 #define DRM_ESWIN_FB_HEIGHT            480
 #define DRM_ESWIN_FB_BPP               VIDEO_BPP16
#endif
#endif

#define MEMORY_POOL_SIZE        16 * 1024 * 1024
#define DRM_ESWIN_FB_BUF        (0xe0000000)
#define DRM_ESWIN_FB_BUF_DIE1   (0x20e0000000)
#define DRM_ESWIN_FB_SIZE \
        VNBYTES(DRM_ESWIN_FB_BPP) * DRM_ESWIN_FB_WIDTH * DRM_ESWIN_FB_HEIGHT

int eswin_show_bmp(const char *bmp);
int eswin_show_logo(int layer);
void eswin_display_fixup(void *blob);
unsigned int eswin_get_fbbuf(int layer);
unsigned int eswin_get_fbwidth(int layer);
unsigned int eswin_get_fbheight(int layer);
unsigned int eswin_get_fbbpp(int layer);
int eswin_show_fbbase(int layer);
int eswin_display_disable(void);

int eswin_bmp_display(struct udevice *dev, ulong addr, int x, int y);

#endif
