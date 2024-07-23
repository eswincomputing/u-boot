// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef VIDEO_ESWIN_H
#define VIDEO_ESWIN_H

#ifndef CONFIG_ESWIN_LOGO_DISPLAY
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
 #define DRM_ESWIN_FB_WIDTH             960
 #define DRM_ESWIN_FB_HEIGHT            540
 #define DRM_ESWIN_FB_BPP               VIDEO_BPP16
#else
 #define DRM_ESWIN_FB_WIDTH             1080
 #define DRM_ESWIN_FB_HEIGHT            1920
 #define DRM_ESWIN_FB_BPP               VIDEO_BPP16
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

#define MEMORY_POOL_SIZE        32 * 1024 * 1024
#define DRM_ESWIN_FB_BUF        (0xe0000000)
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
#endif
