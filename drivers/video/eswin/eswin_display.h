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
 */

#ifndef _ESWIN_DISPLAY_H
#define _ESWIN_DISPLAY_H

#include <bmp_layout.h>
#include <drm_modes.h>
#include <edid.h>
#include <dm/ofnode.h>
#include "eswin_edid.h"

#define ESWIN_OUTPUT_DSI_DUAL_CHANNEL	BIT(0)
#define ESWIN_OUTPUT_DSI_DUAL_LINK		BIT(1)

enum data_format {
    ESWIN_FMT_XRGB4444 = 0,
	ESWIN_FMT_ARGB4444,
	ESWIN_FMT_XRGB1555,
	ESWIN_FMT_ARGB1555,
	ESWIN_FMT_RGB565 = 4,
	ESWIN_FMT_XRGB8888,
	ESWIN_FMT_ARGB8888,
	ESWIN_FMT_YUY2,
	ESWIN_FMT_UYVY = 8,
	ESWIN_FMT_INDEX8,
	ESWIN_FMT_MONOCHROME,
	ESWIN_FMT_YV12 = 15,
	ESWIN_FMT_A8,
	ESWIN_FMT_NV12,
	ESWIN_FMT_NV16,
	ESWIN_FMT_RG16,
	ESWIN_FMT_R8,
	ESWIN_FMT_NV12_10BIT = 21,
	ESWIN_FMT_ARGB2101010,
	ESWIN_FMT_NV16_10BIT,
	ESWIN_FMT_INDEX1,
	ESWIN_FMT_INDEX2,
	ESWIN_FMT_INDEX4,
	ESWIN_FMT_P010,
};

enum eswin_layer {
	ESWIN_VIDEO_LAYER = 0,
	ESWIN_OVERLAY_LAYER,
	ESWIN_LAYER_NUM,
};

enum display_mode {
	ESWIN_DISPLAY_FULLSCREEN,
	ESWIN_DISPLAY_CENTER,
};

enum eswin_cmd_type {
	CMD_TYPE_DEFAULT,
	CMD_TYPE_SPI,
	CMD_TYPE_MCU
};

enum eswin_mcu_cmd {
	MCU_WRCMD = 0,
	MCU_WRDATA,
	MCU_SETBYPASS,
};

/*
 * display output interface supported by eswin lcdc
 */
#define ESWIN_OUT_MODE_P888	0
#define ESWIN_OUT_MODE_P666	1
#define ESWIN_OUT_MODE_P565	2
#define ESWIN_OUT_MODE_S888		8
#define ESWIN_OUT_MODE_S888_DUMMY	12
#define ESWIN_OUT_MODE_YUV420	14
/* for use special outface */
#define ESWIN_OUT_MODE_AAAA	15

struct eswin_mcu_timing {
	int mcu_pix_total;
	int mcu_cs_pst;
	int mcu_cs_pend;
	int mcu_rw_pst;
	int mcu_rw_pend;
	int mcu_hold_mode;
};

struct eswin_rect {
	int width;
	int height;
};

struct crtc_state {
	struct udevice *dev;
	struct eswin_crtc *crtc;
	void *private;
	ofnode node;
	int crtc_id;

	int format;
	u32 dma_addr;
	int ymirror;
	int rb_swap;
	int xvir;
	int src_x;
	int src_y;
	int src_w;
	int src_h;
	int crtc_x;
	int crtc_y;
	int crtc_w;
	int crtc_h;
	bool yuv_overlay;
	struct eswin_mcu_timing mcu_timing;
	u32 dual_channel_swap;
	struct eswin_rect max_output;
};

struct panel_state {
	struct eswin_panel *panel;

	ofnode dsp_lut_node;
};

struct overscan {
	int left_margin;
	int right_margin;
	int top_margin;
	int bottom_margin;
};

struct connector_state {
	struct udevice *dev;
	const struct eswin_connector *connector;
	struct eswin_bridge *bridge;
	struct eswin_phy *phy;
	ofnode node;

	void *private;

	struct drm_display_mode mode;
	struct overscan overscan;
	u8 edid[EDID_SIZE * 4];
	int bus_format;
	int output_mode;
	int type;
	int output_type;
	int color_space;

	struct {
		u32 *lut;
		int size;
	} gamma;
};

struct logo_info {
	int mode;
	char *mem;
	bool ymirror;
	u32 offset;
	u32 width;
	int height;
	u32 bpp;
};

struct eswin_logo_cache {
	struct list_head head;
	char name[20];
	struct logo_info logo;
};

struct display_state {
	struct list_head head;

	const void *blob;
	ofnode node;

	struct crtc_state crtc_state;
	struct connector_state conn_state;
	struct panel_state panel_state;

	char ulogo_name[30];
	char klogo_name[30];

	struct logo_info logo;
	int logo_mode;
	int charge_logo_mode;
	void *mem_base;
	int mem_size;
	int layer;

	int enable;
	int is_init;
	int is_enable;
};

static inline struct eswin_panel *state_get_panel(struct display_state *s)
{
	struct panel_state *panel_state = &s->panel_state;

	return panel_state->panel;
}

int drm_mode_vrefresh(const struct drm_display_mode *mode);
int display_send_mcu_cmd(struct display_state *state, u32 type, u32 val);
bool drm_mode_is_420(const struct drm_display_info *display,
		     struct drm_display_mode *mode);

void drm_mode_max_resolution_filter(struct hdmi_edid_data *edid_data,
				    struct eswin_rect *max_output);

#endif
