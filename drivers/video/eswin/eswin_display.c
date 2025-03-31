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

#include <asm/unaligned.h>
#include <config.h>
#include <common.h>
#include <command.h>
#include <gzip.h>
#include <errno.h>
#include <linux/libfdt.h>
#include <fdtdec.h>
#include <cpu_func.h>
#include <fdt_support.h>
#include <linux/hdmi.h>
#include <linux/list.h>
#include <linux/compat.h>
#include <media_bus_format.h>
#include <malloc.h>
#include <asm/cache.h>
#include <video.h>
#include <video_eswin.h>
#include <video_bridge.h>
#include <dm/device.h>
#include <dm/uclass-internal.h>
#include <linux/hdmi.h>

#include "bmp_helper.h"
#include "eswin_display.h"
#include "eswin_crtc.h"
#include "eswin_connector.h"
#include "eswin_panel.h"
#include "eswin_dc_test.h"
#include <dm.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <fs.h>
#include "eswin_dc_reg.h"
#ifdef CONFIG_ESWIN_LOGO_DISPLAY
#include "logo.h"
#endif
#include "eswin_vo_log.h"

#define DRIVER_VERSION	"v1.0.1"

#undef CONFIG_I2C_EDID

//#define _DEBUG  1
/***********************************************************************
 *  Eswin UBOOT DRM driver version
 *
 *  v1.0.0	: add basic version for eswin drm driver(hjc)
 *  v1.0.1	: add much dsi update(hjc)
 *
 **********************************************************************/

#define RK_BLK_SIZE 512
#define BMP_PROCESSED_FLAG 8399

DECLARE_GLOBAL_DATA_PTR;
static LIST_HEAD(eswin_display_list);
static LIST_HEAD(logo_cache_list);

static unsigned long memory_start;
static unsigned long memory_end;
static unsigned char *logo_buf = NULL;
static unsigned char *upgrade_buf[ESWIN_LAYER_NUM] = {NULL, NULL};
/*
 * the phy types are used by different connectors in public.
 * The current version only has inno hdmi phy for hdmi and tve.
 */
enum public_use_phy {
	NONE,
	INNO_HDMI_PHY
};

extern void sifive_l3_flush64_range(unsigned long start, unsigned long len);

static int argb8888_to_rgb565(unsigned char *argb8888, unsigned short *rgb565, unsigned int size)
{
    unsigned int i,j;

    if((argb8888 == NULL) || (rgb565 == NULL) || (size == 0))
    {
        vo_err("valide pointer or size.\n");
        return -1;
    }

    for(i = 0, j = 0; i < size; i += 4)
    {
        rgb565[j++] = ((argb8888[i+2] >> 3) << 11) | ((argb8888[i+1] >> 2) << 5) | (argb8888[i] >> 3);
    }
    return 0;
}

static void init_display_buffer(ulong base)
{
	memory_start = base + DRM_ESWIN_FB_SIZE;
	memory_end = memory_start;
}

void *get_display_buffer(int size)
{
	unsigned long roundup_memory = roundup(memory_end, PAGE_SIZE);
	void *buf;

	if (roundup_memory + size > memory_start + MEMORY_POOL_SIZE) {
		vo_err("failed to alloc %dbyte memory to display\n", size);
		return NULL;
	}
	buf = (void *)roundup_memory;

	memory_end = roundup_memory + size;

	return buf;
}

bool can_direct_logo(int bpp)
{
	return bpp == 24 || bpp == 32;
}

static int connector_panel_init(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct panel_state *panel_state = &state->panel_state;
	const struct eswin_panel *panel = panel_state->panel;
	ofnode dsp_lut_node;
	int ret, len;

	if (!panel) {
		return 0;
	}

	dsp_lut_node = dev_read_subnode(panel->dev, "dsp-lut");
	if (!ofnode_valid(dsp_lut_node)) {
		vo_err("can not find dsp-lut node\n");
		return 0;
	}

	ofnode_get_property(dsp_lut_node, "gamma-lut", &len);
	if (len > 0) {
		conn_state->gamma.size = len / sizeof(u32);
		conn_state->gamma.lut = malloc(len);
		if (!conn_state->gamma.lut) {
			vo_err("malloc gamma lut failed\n");
			return -ENOMEM;
		}
		ret = ofnode_read_u32_array(dsp_lut_node, "gamma-lut",
					    conn_state->gamma.lut,
					    conn_state->gamma.size);
		if (ret) {
			vo_err("Cannot decode gamma_lut\n");
			conn_state->gamma.lut = NULL;
			return -EINVAL;
		}
		panel_state->dsp_lut_node = dsp_lut_node;
	}

	return 0;
}

int drm_mode_vrefresh(const struct drm_display_mode *mode)
{
	int refresh = 0;
	unsigned int calc_val;

	if (mode->vrefresh > 0) {
		refresh = mode->vrefresh;
	} else if (mode->htotal > 0 && mode->vtotal > 0) {
		int vtotal;

		vtotal = mode->vtotal;
		/* work out vrefresh the value will be x1000 */
		calc_val = (mode->clock * 1000);
		calc_val /= mode->htotal;
		refresh = (calc_val + vtotal / 2) / vtotal;

		if (mode->flags & DRM_MODE_FLAG_INTERLACE)
			refresh *= 2;
		if (mode->flags & DRM_MODE_FLAG_DBLSCAN)
			refresh /= 2;
		if (mode->vscan > 1)
			refresh /= mode->vscan;
	}
	return refresh;
}

static int display_get_timing_from_dts(struct panel_state *panel_state,
				       struct drm_display_mode *mode)
{
	struct eswin_panel *panel = panel_state->panel;
	int phandle;
	int hactive, vactive, pixelclock;
	int hfront_porch, hback_porch, hsync_len;
	int vfront_porch, vback_porch, vsync_len;
	int val, flags = 0;
	ofnode timing, native_mode;

	timing = dev_read_subnode(panel->dev, "display-timings");
	if (!ofnode_valid(timing))
		return -ENODEV;

	native_mode = ofnode_find_subnode(timing, "timing");
	if (!ofnode_valid(native_mode)) {
		phandle = ofnode_read_u32_default(timing, "native-mode", -1);
		native_mode = np_to_ofnode(of_find_node_by_phandle(NULL, phandle));
		if (!ofnode_valid(native_mode)) {
			vo_err("failed to get display timings from DT\n");
			return -ENXIO;
		}
	}

#define FDT_GET_INT(val, name) \
	val = ofnode_read_s32_default(native_mode, name, -1); \
	if (val < 0) { \
		vo_err("Can't get %s\n", name); \
		return -ENXIO; \
	}

#define FDT_GET_INT_DEFAULT(val, name, default) \
	val = ofnode_read_s32_default(native_mode, name, default);

	FDT_GET_INT(hactive, "hactive");
	FDT_GET_INT(vactive, "vactive");
	FDT_GET_INT(pixelclock, "clock-frequency");
	FDT_GET_INT(hsync_len, "hsync-len");
	FDT_GET_INT(hfront_porch, "hfront-porch");
	FDT_GET_INT(hback_porch, "hback-porch");
	FDT_GET_INT(vsync_len, "vsync-len");
	FDT_GET_INT(vfront_porch, "vfront-porch");
	FDT_GET_INT(vback_porch, "vback-porch");
	FDT_GET_INT(val, "hsync-active");
	flags |= val ? DRM_MODE_FLAG_PHSYNC : DRM_MODE_FLAG_NHSYNC;
	FDT_GET_INT(val, "vsync-active");
	flags |= val ? DRM_MODE_FLAG_PVSYNC : DRM_MODE_FLAG_NVSYNC;
	FDT_GET_INT(val, "pixelclk-active");
	flags |= val ? DRM_MODE_FLAG_PPIXDATA : 0;

	FDT_GET_INT_DEFAULT(val, "screen-rotate", 0);
	if (val == DRM_MODE_FLAG_XMIRROR) {
		flags |= DRM_MODE_FLAG_XMIRROR;
	} else if (val == DRM_MODE_FLAG_YMIRROR) {
		flags |= DRM_MODE_FLAG_YMIRROR;
	} else if (val == DRM_MODE_FLAG_XYMIRROR) {
		flags |= DRM_MODE_FLAG_XMIRROR;
		flags |= DRM_MODE_FLAG_YMIRROR;
	}
	mode->hdisplay = hactive;
	mode->hsync_start = mode->hdisplay + hfront_porch;
	mode->hsync_end = mode->hsync_start + hsync_len;
	mode->htotal = mode->hsync_end + hback_porch;

	mode->vdisplay = vactive;
	mode->vsync_start = mode->vdisplay + vfront_porch;
	mode->vsync_end = mode->vsync_start + vsync_len;
	mode->vtotal = mode->vsync_end + vback_porch;

	mode->clock = pixelclock / 1000;
	mode->flags = flags;

	return 0;
}


#ifdef CONFIG_DRM_ESWIN_DW_HDMI
/**
 * drm_mode_max_resolution_filter - mark modes out of vop max resolution
 * @edid_data: structure store mode list
 * @max_output: vop max output resolution
 */
void drm_mode_max_resolution_filter(struct hdmi_edid_data *edid_data,
				    struct eswin_rect *max_output)
{
	int i;

	for (i = 0; i < edid_data->modes; i++) {
		if (edid_data->mode_buf[i].hdisplay > max_output->width ||
		    edid_data->mode_buf[i].vdisplay > max_output->height)
			edid_data->mode_buf[i].invalid = true;
	}
}
#endif

/**
 * drm_mode_set_crtcinfo - set CRTC modesetting timing parameters
 * @p: mode
 * @adjust_flags: a combination of adjustment flags
 *
 * Setup the CRTC modesetting timing parameters for @p, adjusting if necessary.
 *
 * - The CRTC_INTERLACE_HALVE_V flag can be used to halve vertical timings of
 *   interlaced modes.
 * - The CRTC_STEREO_DOUBLE flag can be used to compute the timings for
 *   buffers containing two eyes (only adjust the timings when needed, eg. for
 *   "frame packing" or "side by side full").
 * - The CRTC_NO_DBLSCAN and CRTC_NO_VSCAN flags request that adjustment *not*
 *   be performed for doublescan and vscan > 1 modes respectively.
 */
void drm_mode_set_crtcinfo(struct drm_display_mode *p, int adjust_flags)
{
	if ((p == NULL) || ((p->type & DRM_MODE_TYPE_CRTC_C) == DRM_MODE_TYPE_BUILTIN))
		return;

	if (p->flags & DRM_MODE_FLAG_DBLCLK)
		p->crtc_clock = 2 * p->clock;
	else
		p->crtc_clock = p->clock;
	p->crtc_hdisplay = p->hdisplay;
	p->crtc_hsync_start = p->hsync_start;
	p->crtc_hsync_end = p->hsync_end;
	p->crtc_htotal = p->htotal;
	p->crtc_hskew = p->hskew;
	p->crtc_vdisplay = p->vdisplay;
	p->crtc_vsync_start = p->vsync_start;
	p->crtc_vsync_end = p->vsync_end;
	p->crtc_vtotal = p->vtotal;

	if (p->flags & DRM_MODE_FLAG_INTERLACE) {
		if (adjust_flags & CRTC_INTERLACE_HALVE_V) {
			p->crtc_vdisplay /= 2;
			p->crtc_vsync_start /= 2;
			p->crtc_vsync_end /= 2;
			p->crtc_vtotal /= 2;
		}
	}

	if (!(adjust_flags & CRTC_NO_DBLSCAN)) {
		if (p->flags & DRM_MODE_FLAG_DBLSCAN) {
			p->crtc_vdisplay *= 2;
			p->crtc_vsync_start *= 2;
			p->crtc_vsync_end *= 2;
			p->crtc_vtotal *= 2;
		}
	}

	if (!(adjust_flags & CRTC_NO_VSCAN)) {
		if (p->vscan > 1) {
			p->crtc_vdisplay *= p->vscan;
			p->crtc_vsync_start *= p->vscan;
			p->crtc_vsync_end *= p->vscan;
			p->crtc_vtotal *= p->vscan;
		}
	}

	if (adjust_flags & CRTC_STEREO_DOUBLE) {
		// unsigned int layout = p->flags & DRM_MODE_FLAG_3D_MASK;
		unsigned int layout = p->flags & (0x1f << 14);

		switch (layout) {
		//case DRM_MODE_FLAG_3D_FRAME_PACKING:
		case BIT(14):
			p->crtc_clock *= 2;
			p->crtc_vdisplay += p->crtc_vtotal;
			p->crtc_vsync_start += p->crtc_vtotal;
			p->crtc_vsync_end += p->crtc_vtotal;
			p->crtc_vtotal += p->crtc_vtotal;
			break;
		}
	}

	p->crtc_vblank_start = min(p->crtc_vsync_start, p->crtc_vdisplay);
	p->crtc_vblank_end = max(p->crtc_vsync_end, p->crtc_vtotal);
	p->crtc_hblank_start = min(p->crtc_hsync_start, p->crtc_hdisplay);
	p->crtc_hblank_end = max(p->crtc_hsync_end, p->crtc_htotal);
}


#ifdef CONFIG_DRM_ESWIN_DW_HDMI
/**
 * drm_mode_is_420_only - if a given videomode can be only supported in YCBCR420
 * output format
 *
 * @connector: drm connector under action.
 * @mode: video mode to be tested.
 *
 * Returns:
 * true if the mode can be supported in YCBCR420 format
 * false if not.
 */
bool drm_mode_is_420_only(const struct drm_display_info *display,
			  struct drm_display_mode *mode)
{
	u8 vic = drm_match_cea_mode(mode);

	return test_bit(vic, display->hdmi.y420_vdb_modes);
}

/**
 * drm_mode_is_420_also - if a given videomode can be supported in YCBCR420
 * output format also (along with RGB/YCBCR444/422)
 *
 * @display: display under action.
 * @mode: video mode to be tested.
 *
 * Returns:
 * true if the mode can be support YCBCR420 format
 * false if not.
 */
bool drm_mode_is_420_also(const struct drm_display_info *display,
			  struct drm_display_mode *mode)
{
	u8 vic = drm_match_cea_mode(mode);

	return test_bit(vic, display->hdmi.y420_cmdb_modes);
}

/**
 * drm_mode_is_420 - if a given videomode can be supported in YCBCR420
 * output format
 *
 * @display: display under action.
 * @mode: video mode to be tested.
 *
 * Returns:
 * true if the mode can be supported in YCBCR420 format
 * false if not.
 */
bool drm_mode_is_420(const struct drm_display_info *display,
		     struct drm_display_mode *mode)
{
	return drm_mode_is_420_only(display, mode) ||
		drm_mode_is_420_also(display, mode);
}
#endif

static int display_get_timing(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	const struct drm_display_mode *m;
	struct panel_state *panel_state = &state->panel_state;
	const struct eswin_panel *panel = panel_state->panel;

	if (dev_has_ofnode(panel->dev) &&
	    !display_get_timing_from_dts(panel_state, mode)) {
		debug("[%s]Using display timing dts\n", __FUNCTION__);
		return 0;
	}

	if (panel->data) {
		m = (const struct drm_display_mode *)panel->data;
		memcpy(mode, m, sizeof(*m));
		debug("[%s]Using display timing from compatible panel driver\n", __FUNCTION__);
		return 0;
	}

	return -ENODEV;
}

static int display_init(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	struct panel_state *panel_state = &state->panel_state;
	const struct eswin_connector *conn = conn_state->connector;
	const struct eswin_connector_funcs *conn_funcs = conn->funcs;
	struct crtc_state *crtc_state = &state->crtc_state;
	struct eswin_crtc *crtc = crtc_state->crtc;
	const struct eswin_crtc_funcs *crtc_funcs = crtc->funcs;
	struct drm_display_mode *mode = &conn_state->mode;
	int ret = 0;

	vo_info("Die[%d] Eswin UBOOT DRM driver version: %s\n", state->numa_id, DRIVER_VERSION);

	if (state->is_init) {
		vo_warn("display init already.\n");
		return 0;
	}
	if (!conn_funcs || !crtc_funcs) {
		vo_err("failed to find connector or crtc functions\n");
		return -ENXIO;
	}

	if (crtc_funcs->preinit) {
		ret = crtc_funcs->preinit(state);
		if (ret) {
			vo_err("crtc_funcs->preinit failed.\n");
			return ret;
		}
	}

	if (conn_funcs->init) {
		ret = conn_funcs->init(state);
		if (ret) {
			vo_err("conn_funcs->init failed.\n");
			goto deinit;
		}
	}

	if (panel_state->panel) {
		vo_info("start panel init.\n");
		eswin_panel_init(panel_state->panel);
	}

	/*
	 * support hotplug, but not connect;
	 */
	if (conn_funcs->detect) {
		ret = conn_funcs->detect(state);
		if (!ret) {
			vo_info("no display connected!\n");
			goto deinit;
		}
	}

	if (panel_state->panel) {
		vo_debug("get panle timing.\n");
		ret = display_get_timing(state);
	} else if (conn_funcs->get_timing) {
		vo_debug("get connctor timing.\n");
		ret = conn_funcs->get_timing(state);
	} else {
		vo_err("can't get any timing.\n");
		goto deinit;
	}

	if (ret) {
		vo_err("get timing failed.\n");
		goto deinit;
	}

	vo_info("Detailed mode clock %u kHz, flags[%x]\n"
	       "    H: %04d %04d %04d %04d\n"
	       "    V: %04d %04d %04d %04d\n"
	       "bus_format: %x\n",
	       mode->clock, mode->flags,
	       mode->hdisplay, mode->hsync_start,
	       mode->hsync_end, mode->htotal,
	       mode->vdisplay, mode->vsync_start,
	       mode->vsync_end, mode->vtotal,
	       conn_state->bus_format);

	drm_mode_set_crtcinfo(mode, CRTC_INTERLACE_HALVE_V);
	if (crtc_funcs->init) {
		ret = crtc_funcs->init(state);
		if (ret) {
			vo_err("crtc_funcs->init failed.\n");
			goto deinit;
		}
	}
	state->is_init = 1;

	return 0;

deinit:
	if (conn_funcs->deinit)
		conn_funcs->deinit(state);
	return ret;
}

int display_send_mcu_cmd(struct display_state *state, u32 type, u32 val)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct eswin_crtc *crtc = crtc_state->crtc;
	const struct eswin_crtc_funcs *crtc_funcs = crtc->funcs;
	int ret;

	if (!state->is_init)
		return -EINVAL;

	if (crtc_funcs->send_mcu_cmd) {
		ret = crtc_funcs->send_mcu_cmd(state, type, val);
		if (ret)
			return ret;
	}

	return 0;
}

static int display_set_plane(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct eswin_crtc *crtc = crtc_state->crtc;
	const struct eswin_crtc_funcs *crtc_funcs = crtc->funcs;
	int ret;

	if (!state->is_init)
		return -EINVAL;

	if (crtc_funcs->set_plane) {
		ret = crtc_funcs->set_plane(state);
		if (ret)
			return ret;
	}

	return 0;
}

static int display_enable(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	const struct eswin_connector *conn = conn_state->connector;
	const struct eswin_connector_funcs *conn_funcs = conn->funcs;
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct eswin_crtc *crtc = crtc_state->crtc;
	const struct eswin_crtc_funcs *crtc_funcs = crtc->funcs;
	struct panel_state *panel_state = &state->panel_state;

	vo_debug("enter.\n");
	if (!state->is_init)
		return -EINVAL;

	if (state->is_enable)
		return 0;

	if (crtc_funcs->prepare)
		crtc_funcs->prepare(state);

	if (crtc_funcs->enable)
        crtc_funcs->enable(state);
#ifndef CONFIG_DRM_ESWIN_WRITEBACK
	if (conn_funcs->prepare)
		conn_funcs->prepare(state);

	if (panel_state->panel)
		eswin_panel_prepare(panel_state->panel);

	if (conn_funcs->enable)
		conn_funcs->enable(state);

	if (panel_state->panel)
		eswin_panel_enable(panel_state->panel);
#endif
	state->is_enable = true;
	vo_debug("leave.\n");

	return 0;
}

static int display_disable(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;
	const struct eswin_connector *conn = conn_state->connector;
	const struct eswin_connector_funcs *conn_funcs = conn->funcs;
	struct crtc_state *crtc_state = &state->crtc_state;
	const struct eswin_crtc *crtc = crtc_state->crtc;
	const struct eswin_crtc_funcs *crtc_funcs = crtc->funcs;
	struct panel_state *panel_state = &state->panel_state;

	if (!state->is_init)
		return 0;

	if (!state->is_enable)
		return 0;

	if (crtc_funcs->disable)
        crtc_funcs->disable(state);

	if (crtc_funcs->unprepare)
        crtc_funcs->unprepare(state);
#ifndef CONFIG_DRM_ESWIN_WRITEBACK
	if (panel_state->panel)
		eswin_panel_disable(panel_state->panel);

	if (conn_funcs->disable)
		conn_funcs->disable(state);

	if (panel_state->panel)
		eswin_panel_unprepare(panel_state->panel);

	if (conn_funcs->unprepare)
		conn_funcs->unprepare(state);
#endif
	state->is_enable = 0;
	state->is_init = 0;

	return 0;
}

static int display_logo(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct connector_state *conn_state = &state->conn_state;
	struct logo_info *logo = &state->logo;
	int hdisplay, vdisplay, ret;

	if (state->is_init && crtc_state->dma_addr != 0)
	{
		sifive_l3_flush64_range(crtc_state->dma_addr, DRM_ESWIN_FB_SIZE);
		return 0;
	}

	ret = display_init(state);
	if (!state->is_init || ret)
		return -ENODEV;

	switch (logo->bpp) {
	case 16:
		crtc_state->format = ESWIN_FMT_RGB565;
		break;
	case 32:
		crtc_state->format = ESWIN_FMT_ARGB8888;
		break;
	default:
		vo_err("can't support bmp bits[%d]\n", logo->bpp);
		return -EINVAL;
	}

	hdisplay = conn_state->mode.hdisplay;
	vdisplay = conn_state->mode.vdisplay;
	crtc_state->src_w = logo->width;
	crtc_state->src_h = logo->height;
	crtc_state->src_x = 0;
	crtc_state->src_y = 0;
	crtc_state->ymirror = logo->ymirror;

	crtc_state->xvir = ALIGN(crtc_state->src_w * logo->bpp, 32) >> 5;
#ifdef CONFIG_ESWIN_LOGO_DISPLAY
	memcpy((char *)crtc_state->dma_addr, logo->mem, DRM_ESWIN_FB_SIZE);
#endif
	sifive_l3_flush64_range(crtc_state->dma_addr, DRM_ESWIN_FB_SIZE);
	vo_debug("crtc_state->dma_addr=0x%llx,logo->mem=0x%llx, logo size:%d\n",
			crtc_state->dma_addr, (u64)logo->mem, DRM_ESWIN_FB_SIZE);

	if (logo->mode == ESWIN_DISPLAY_FULLSCREEN) {
		crtc_state->crtc_x = 0;
		crtc_state->crtc_y = 0;
		crtc_state->crtc_w = hdisplay;
		crtc_state->crtc_h = vdisplay;
	} else {
		if (crtc_state->src_w >= hdisplay) {
			crtc_state->crtc_x = 0;
			crtc_state->crtc_w = hdisplay;
		} else {
			crtc_state->crtc_x = (hdisplay - crtc_state->src_w) / 2;
			crtc_state->crtc_w = crtc_state->src_w;
		}

		if (crtc_state->src_h >= vdisplay) {
			crtc_state->crtc_y = 0;
			crtc_state->crtc_h = vdisplay;
		} else {
			crtc_state->crtc_y = (vdisplay - crtc_state->src_h) / 2;
			crtc_state->crtc_h = crtc_state->src_h;
		}
	}

	display_set_plane(state);
	display_enable(state);

	return 0;
}

static int get_crtc_id(ofnode connect)
{
	int phandle;
	struct device_node *remote;
	int val;

	phandle = ofnode_read_u32_default(connect, "remote-endpoint", -1);
	if (phandle < 0)
		goto err;
	remote = of_find_node_by_phandle(NULL, phandle);
	val = ofnode_read_u32_default(np_to_ofnode(remote), "reg", -1);
	if (val < 0)
		goto err;

	return val;
err:
	vo_err("Can't get crtc id, default set to id = 0\n");
	return 0;
}

struct eswin_logo_cache *find_or_alloc_logo_cache(const char *bmp)
{
	struct eswin_logo_cache *tmp = NULL;
	struct eswin_logo_cache *logo_cache = NULL;

	list_for_each_entry(tmp, &logo_cache_list, head) {
		if (!strcmp(tmp->name, bmp)) {
			logo_cache = tmp;
			break;
		}
	}

	if (!logo_cache) {
		logo_cache = malloc(sizeof(*logo_cache));
		if (!logo_cache) {
			vo_err("failed to alloc memory for logo cache\n");
			return NULL;
		}
		memset(logo_cache, 0, sizeof(*logo_cache));
		strcpy(logo_cache->name, bmp);
		INIT_LIST_HEAD(&logo_cache->head);
		list_add_tail(&logo_cache->head, &logo_cache_list);
	}

	return logo_cache;
}

enum LOGO_SOURCE {
    FROM_RESOURCE,
    FROM_INTERNEL
};

#ifdef CONFIG_ESWIN_LOGO_DISPLAY
static int load_bmp_logo(struct logo_info *logo, const char *bmp_name)
{
	struct eswin_logo_cache *logo_cache;
	struct bmp_header *header;
	struct bmp_header *logo_bmp = NULL;
	unsigned long lenp = ~0UL;
	void *dst = NULL, *pdst;
	int size;
#ifdef CONFIG_ESWIN_RESOURCE_IMAGE
	int len;
#endif
	int ret = 0;
	int reserved = 0;
    enum LOGO_SOURCE logo_source;

	if (!logo || !bmp_name)
		return -EINVAL;
	logo_cache = find_or_alloc_logo_cache(bmp_name);
	if (!logo_cache)
		return -ENOMEM;

	if (logo_cache->logo.mem) {
		memcpy(logo, &logo_cache->logo, sizeof(*logo));
		return 0;
	}

	header = malloc(RK_BLK_SIZE);
	if (!header)
		return -ENOMEM;

#ifdef CONFIG_ESWIN_RESOURCE_IMAGE
	len = eswin_read_resource_file(header, bmp_name, 0, RK_BLK_SIZE);
	if (len == RK_BLK_SIZE) {
        logo_source = FROM_RESOURCE;
    }
    else
#endif
    {
        free(header);
		logo_bmp = malloc(LOGO_MAX_SIZE);
		if (!logo_bmp)
			return -ENOMEM;

		if (gunzip(logo_bmp, LOGO_MAX_SIZE,
			(uchar *) logo_bmp_gzip,
			   &lenp) != 0) {
			vo_err("Error: error uncompressed logo_bmp !\n");
			free(logo_bmp);
			return 1;
		}
		else {
			vo_err("logo_bmp uncompressed size=%ld\n", lenp);
			//logo_bmp = (struct bmp_header *)(logo_bmp);
		}

        header = (struct bmp_header *)logo_bmp;
        logo_source = FROM_INTERNEL;
    }

	logo->bpp = get_unaligned_le16(&header->bit_count);
	logo->width = get_unaligned_le32(&header->width);
	logo->height = get_unaligned_le32(&header->height);
	reserved = get_unaligned_le32(&header->reserved);
	if (logo->height < 0)
	    logo->height = -logo->height;
	size = get_unaligned_le32(&header->file_size);
	if (!can_direct_logo(logo->bpp)) {
		if (size > MEMORY_POOL_SIZE) {
			vo_err("failed to use boot buf as temp bmp buffer\n");
			ret = -ENOMEM;
			goto free_header;
		}
		pdst = get_display_buffer(size);
	} else {
		pdst = get_display_buffer(size);
		dst = pdst;
	}

#ifdef CONFIG_ESWIN_RESOURCE_IMAGE
    if (logo_source == FROM_RESOURCE) {
        len = eswin_read_resource_file(pdst, bmp_name, 0, size);
        if (len != size) {
            vo_err("failed to load bmp %s\n", bmp_name);
            ret = -ENOENT;
            goto free_header;
        }
    } else
#endif
    {
        pdst = (void*)logo_bmp;
    }

	if (!can_direct_logo(logo->bpp)) {
		int dst_size;
		/*
		 * TODO: force use 16bpp if bpp less than 16;
		 */
		logo->bpp = (logo->bpp <= 16) ? 16 : logo->bpp;
		dst_size = logo->width * logo->height * logo->bpp >> 3;

		dst = get_display_buffer(dst_size);
		if (!dst) {
			ret = -ENOMEM;
			goto free_header;
		}
		if (bmpdecoder(pdst, dst, logo->bpp)) {
			vo_err("failed to decode bmp %s\n", bmp_name);
			ret = -EINVAL;
			goto free_header;
		}
		flush_dcache_range((ulong)dst,
				   ALIGN((ulong)dst + dst_size,
					 ARCH_DMA_MINALIGN));

		logo->offset = 0;
		logo->ymirror = 0;
	} else {
		logo->offset = get_unaligned_le32(&header->data_offset);
		if (reserved == BMP_PROCESSED_FLAG)
			logo->ymirror = 0;
		else
			logo->ymirror = 1;
	}
	logo->mem = dst;

	memcpy(&logo_cache->logo, logo, sizeof(*logo));

free_header:
    if (logo_source != FROM_INTERNEL) {
        free(header);
    }

	return ret;
}

int eswin_show_bmp(const char *bmp)
{
	struct display_state *s = NULL;
	int ret = 0;

	if (!bmp) {
		list_for_each_entry(s, &eswin_display_list, head)
			display_disable(s);
		return -ENOENT;
	}

	list_for_each_entry(s, &eswin_display_list, head) {
		s->logo.mode = s->charge_logo_mode;
		if (load_bmp_logo(&s->logo, bmp))
			continue;
		ret = display_logo(s);
	}

	return ret;
}

int eswin_show_logo(int layer)
{
	struct display_state *s = NULL;
	int ret = 0;

	list_for_each_entry(s, &eswin_display_list, head) {
		s->logo.mode = s->logo_mode;
		s->layer = layer;
		if (load_bmp_logo(&s->logo, s->ulogo_name)) {
		    vo_err("failed to display uboot bmp logo\n");
		}
		else {
		    vo_debug("display uboot logo\n");
		    ret = display_logo(s);
		}
		/* Load kernel bmp in eswin_display_fixup() later */
	}

	return ret;
}
#endif

enum {
	PORT_DIR_IN,
	PORT_DIR_OUT,
};

static struct eswin_panel *eswin_of_find_panel(struct udevice *dev)
{
	ofnode panel_node, ports, ep;
	ofnode port = {0};
	struct udevice *panel_dev;
	int ret;

	panel_node = dev_read_subnode(dev, "panel");
	if (ofnode_valid(panel_node) && ofnode_is_enabled(panel_node)) {
		ret = uclass_get_device_by_ofnode(UCLASS_PANEL, panel_node,
						  &panel_dev);
		if (!ret)
			goto found;
	}

	ports = dev_read_subnode(dev, "ports");
	if (!ofnode_valid(ports))
		return NULL;

	ofnode_for_each_subnode(port, ports) {
		u32 reg;

		if (ofnode_read_u32(port, "reg", &reg))
			continue;

		if (reg != PORT_DIR_OUT)
			continue;

		ofnode_for_each_subnode(ep, port) {
			ofnode _ep, _port;
			uint phandle;

			if (ofnode_read_u32(ep, "remote-endpoint", &phandle))
				continue;

			_ep = ofnode_get_by_phandle(phandle);
			if (!ofnode_valid(_ep))
				continue;

			_port = ofnode_get_parent(_ep);
			if (!ofnode_valid(_port))
				continue;

			panel_node = ofnode_get_parent(_port);
			if (!ofnode_valid(panel_node))
				continue;

			ret = uclass_get_device_by_ofnode(UCLASS_PANEL,
							  panel_node,
							  &panel_dev);
			if (!ret)
				goto found;
		}
	}

	return NULL;

found:
	return (struct eswin_panel *)dev_get_driver_data(panel_dev);
}

static struct udevice *eswin_of_find_connector(ofnode endpoint)
{
	ofnode ep, port, ports, conn;
	uint phandle;
	struct udevice *dev;
	int ret;

	if (ofnode_read_u32(endpoint, "remote-endpoint", &phandle))
		return NULL;

	ep = ofnode_get_by_phandle(phandle);
	if (!ofnode_valid(ep) || !ofnode_is_enabled(ep))
		return NULL;

	port = ofnode_get_parent(ep);
	if (!ofnode_valid(port))
		return NULL;

	ports = ofnode_get_parent(port);
	if (!ofnode_valid(ports))
		return NULL;

	conn = ofnode_get_parent(ports);
	if (!ofnode_valid(conn) || !ofnode_is_enabled(conn))
		return NULL;

	ret = uclass_get_device_by_ofnode(UCLASS_DISPLAY, conn, &dev);
	if (ret)
		return NULL;

	return dev;
}

static int eswin_display_probe(struct udevice *dev)
{
	struct video_priv *uc_priv = dev_get_uclass_priv(dev);
	struct video_uc_plat *plat = dev_get_uclass_plat(dev);
	const void *blob = gd->fdt_blob;
	int phandle;
	struct udevice *crtc_dev, *conn_dev;
	struct eswin_crtc *crtc;
	const struct eswin_connector *conn;
	struct eswin_panel *panel = NULL;
	struct display_state *s = NULL;
	const char *name;
	int ret;
	ofnode route_node;
	ofnode node = {0};
	struct device_node *port_node, *dc_node, *ep_node;
	u32 node_id;

	ret = dev_read_u32(dev, "numa-node-id", &node_id);
	if (ret) {
		vo_err("failed to get node id, ret %d\n", ret);
		return ret;
	}

	vo_debug("node id:%d\n", node_id);
	/* Before relocation we don't need to do anything */
	if (!(gd->flags & GD_FLG_RELOC))
		return 0;

	init_display_buffer(plat->base);
#ifdef CONFIG_ESWIN_LOGO_DISPLAY
	logo_buf = (unsigned char *)get_display_buffer(DRM_ESWIN_FB_SIZE);
	if(logo_buf != NULL) {
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
		memcpy(logo_buf, buf_1280_720, DRM_ESWIN_FB_SIZE);  //hdmi test
#else
		memcpy(logo_buf, buf_320_480, DRM_ESWIN_FB_SIZE);   //mipi test
#endif
	}
#endif

	route_node = dev_read_subnode(dev, "route");
	if (!ofnode_valid(route_node))
		return -ENODEV;

	ofnode_for_each_subnode(node, route_node) {
		if (!ofnode_is_enabled(node))
			continue;
		phandle = ofnode_read_u32_default(node, "connect", -1);
		if (phandle < 0) {
			vo_warn("can't find connect node's handle\n");
			continue;
		}
		ep_node = of_find_node_by_phandle(NULL, phandle);
		if (!ofnode_valid(np_to_ofnode(ep_node))) {
			vo_warn("can't find endpoint node from phandle\n");
			continue;
		}
		port_node = of_get_parent(ep_node);
		if (!ofnode_valid(np_to_ofnode(port_node))) {
			vo_warn("can't find port node from phandle\n");
			continue;
		}
		dc_node = of_get_parent(port_node);
		if (!ofnode_valid(np_to_ofnode(dc_node))) {
			vo_warn("can't find crtc node from phandle\n");
			continue;
		}
		ret = uclass_get_device_by_ofnode(UCLASS_VIDEO_OSD,
						  np_to_ofnode(dc_node),
						  &crtc_dev);
		if (ret) {
			vo_warn("can't find crtc driver %d\n", ret);
			continue;
		}
		crtc = (struct eswin_crtc *)dev_get_driver_data(crtc_dev);

		conn_dev = eswin_of_find_connector(np_to_ofnode(ep_node));
		if (!conn_dev) {
			vo_warn("can't find connect driver\n");
			continue;
		}

		conn = (const struct eswin_connector *)dev_get_driver_data(conn_dev);

		panel = eswin_of_find_panel(conn_dev);

		s = malloc(sizeof(*s));
		if (!s)
			continue;
    
		memset(s, 0, sizeof(*s));

		INIT_LIST_HEAD(&s->head);
		ret = ofnode_read_string_index(node, "logo,uboot", 0, &name);
		if (!ret)
		{
			memcpy(s->ulogo_name, name, strlen(name));
		}

		ret = ofnode_read_string_index(node, "logo,kernel", 0, &name);
		if (!ret)
		{
			memcpy(s->klogo_name, name, strlen(name));
		}

		ret = ofnode_read_string_index(node, "logo,mode", 0, &name);
		if (!strcmp(name, "fullscreen"))
			s->logo_mode = ESWIN_DISPLAY_FULLSCREEN;
		else
			s->logo_mode = ESWIN_DISPLAY_CENTER;

		ret = ofnode_read_string_index(node, "charge_logo,mode", 0, &name);
		if (!strcmp(name, "fullscreen"))
			s->charge_logo_mode = ESWIN_DISPLAY_FULLSCREEN;
		else
			s->charge_logo_mode = ESWIN_DISPLAY_CENTER;

		s->blob = blob;
		s->panel_state.panel = panel;
		s->conn_state.node = conn_dev->node_;
		s->conn_state.dev = conn_dev;
		s->conn_state.connector = conn;
		s->conn_state.overscan.left_margin = 100;
		s->conn_state.overscan.right_margin = 100;
		s->conn_state.overscan.top_margin = 100;
		s->conn_state.overscan.bottom_margin = 100;
		s->crtc_state.node = np_to_ofnode(dc_node);
#ifdef CONFIG_ESWIN_LOGO_DISPLAY
		s->crtc_state.dma_addr = DRM_ESWIN_FB_BUF;
#else
		s->crtc_state.dma_addr = plat->base;
#endif
		s->crtc_state.buf_size = DRM_ESWIN_FB_SIZE;
		if (node_id == 1) {
			s->crtc_state.dma_addr = DRM_ESWIN_FB_BUF_DIE1;
		}
		s->numa_id = node_id;
		s->crtc_state.dev = crtc_dev;
		s->crtc_state.crtc = crtc;
		s->crtc_state.crtc_id = get_crtc_id(np_to_ofnode(ep_node));
		s->node = node;

		if (panel)
			panel->state = s;

		ret = ofnode_read_u32_default(s->crtc_state.node, "eswin,dual-channel-swap", 0);
		s->crtc_state.dual_channel_swap = ret;
		if (connector_panel_init(s)) {
			vo_warn("Failed to init panel drivers\n");
			free(s);
			continue;
		}

		list_add_tail(&s->head, &eswin_display_list);
	}

	if (list_empty(&eswin_display_list)) {
		vo_err("Failed to found available display route\n");
		return -ENODEV;
	}

	uc_priv->xsize = DRM_ESWIN_FB_WIDTH;
	uc_priv->ysize = DRM_ESWIN_FB_HEIGHT;
	uc_priv->bpix = DRM_ESWIN_FB_BPP;
#ifndef CONFIG_ESWIN_LOGO_DISPLAY
	if (node_id == 1) {
		uc_priv->fb  = (void *)DRM_ESWIN_FB_BUF_DIE1;
		plat->base = DRM_ESWIN_FB_BUF_DIE1;
	}
#endif

	s->logo.mode = ESWIN_DISPLAY_FULLSCREEN;
	s->layer = ESWIN_VIDEO_LAYER;
	s->logo.mem = (char *)logo_buf;
	s->logo.width = DRM_ESWIN_FB_WIDTH;
	s->logo.height = DRM_ESWIN_FB_HEIGHT;
	s->logo.bpp = (1 << DRM_ESWIN_FB_BPP);
	s->logo.ymirror = 0;
	display_logo(s);
	video_set_flush_dcache(dev, true);

	return 0;
}

int eswin_display_bind(struct udevice *dev)
{
	struct video_uc_plat *plat = dev_get_uclass_plat(dev);

	plat->size = MEMORY_POOL_SIZE;

	return 0;
}

#ifndef CONFIG_ESWIN_LOGO_DISPLAY
static int eswin_display_sync(struct udevice *dev)
{
	struct video_uc_plat *plat = dev_get_uclass_plat(dev);

	sifive_l3_flush64_range(plat->base, DRM_ESWIN_FB_SIZE);

	return 0;
}

static const struct video_ops eswin_display_ops = {
	.video_sync = eswin_display_sync,
};
#endif


static const struct udevice_id eswin_display_ids[] = {
	{ .compatible = "eswin,display-subsystem" },
	{ }
};

U_BOOT_DRIVER(eswin_display) = {
	.name	= "eswin_display",
	.id	= UCLASS_VIDEO,
	.of_match = eswin_display_ids,
#ifndef CONFIG_ESWIN_LOGO_DISPLAY
	.ops = &eswin_display_ops,
#endif
	.bind	= eswin_display_bind,
	.probe	= eswin_display_probe,
};

unsigned int eswin_get_fbbuf(int layer)
{
    if(upgrade_buf[layer] == NULL)
        upgrade_buf[layer] = (unsigned char *)get_display_buffer(DRM_ESWIN_FB_WIDTH * DRM_ESWIN_FB_HEIGHT * 4);	//upgrade using argb8888 format
    debug("eswin_get_fbbuf:upgrade_buf[%d]=0x%hhn\n", layer, upgrade_buf[layer]);

    return (unsigned int)(u64)upgrade_buf[layer];
}

unsigned int eswin_get_fbwidth(int layer)
{
	return DRM_ESWIN_FB_WIDTH;
}

unsigned int eswin_get_fbheight(int layer)
{
    return DRM_ESWIN_FB_HEIGHT;
}

unsigned int eswin_get_fbbpp(int layer)
{
    return (1 << DRM_ESWIN_FB_BPP);
}

int eswin_show_fbbase(int layer)
{
    struct display_state *s = NULL;
	struct crtc_state *crtc_state = NULL;

    list_for_each_entry(s, &eswin_display_list, head) {
		crtc_state = &s->crtc_state;
		s->crtc_state.dma_addr = (u32)DRM_ESWIN_FB_BUF;
		struct dc8000_dc *dc = crtc_state->private;
		eswin_hw_set_framebuffer_address(dc, s->crtc_state.dma_addr);
		s->logo.mode = ESWIN_DISPLAY_FULLSCREEN;
		s->layer = layer;
		if(crtc_state->format == ESWIN_FMT_RGB565) {
			argb8888_to_rgb565((unsigned char *)upgrade_buf[layer], (unsigned short *)(u64)crtc_state->dma_addr, DRM_ESWIN_FB_WIDTH * DRM_ESWIN_FB_HEIGHT * 4); //upgrade test
		}else {
			memcpy((char *)(u64)crtc_state->dma_addr, (char *)upgrade_buf[layer], DRM_ESWIN_FB_SIZE);
		}
		display_logo(s);
        return 0;
    }
    return -1;
}

int eswin_display_disable(void)
{
    struct display_state *s = NULL;;

    list_for_each_entry(s, &eswin_display_list, head) {
        display_disable(s);
        return 0;
    }
    return -1;
}

#ifdef CONFIG_ESWIN_LOGO_DISPLAY
static int do_eswin_logo_show(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	struct udevice *dev;
	if (argc != 2)
		return CMD_RET_USAGE;
	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);
	if(*argv[1] == '0')
		eswin_show_logo(ESWIN_VIDEO_LAYER);
	else
		eswin_show_logo(ESWIN_OVERLAY_LAYER);

	return 0;
}

static int do_eswin_show_bmp(struct cmd_tbl *cmdtp, int flag, int argc,
				char *const argv[])
{
	if (argc != 2)
		return CMD_RET_USAGE;

	eswin_show_bmp(argv[1]);

	return 0;
}
#endif

static int do_read_dc8k_regs(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	struct udevice *dev;
	struct display_state *s = NULL;;
	struct crtc_state *crtc_state;
	void *regs;

	printf("do_read_dc8k_regs: enter.\n");
	if (argc != 1)
		return CMD_RET_USAGE;
	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);

	list_for_each_entry(s, &eswin_display_list, head) {
		crtc_state = &s->crtc_state;
		regs = dev_read_addr_ptr(crtc_state->dev);
		printf("do_read_dc8k_regs: regs=0x%px\n", regs);
		dc_reg_test_read(regs);
	}

	return 0;
}

static int do_write_dc8k_regs(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	struct udevice *dev;
	struct display_state *s = NULL;
	struct crtc_state *crtc_state;
	void *regs;

	printf("do_write_dc8k_regs: enter.\n");
	if (argc != 2)
		return CMD_RET_USAGE;
	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);

	list_for_each_entry(s, &eswin_display_list, head) {
		crtc_state = &s->crtc_state;
		regs = dev_read_addr_ptr(crtc_state->dev);
		printf("do_write_dc8k_regs: regs=0x%px\n", regs);
		if(*argv[1] == '0')
			dc_reg_test_write(regs, 0x0);
		else
			dc_reg_test_write(regs, 0xffffffff);
	}

	return 0;
}

static int do_writeback_to_mem(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	struct udevice *dev;
	struct display_state *s = NULL;
	struct crtc_state *crtc_state;
	void *regs;

	printf("do_writeback_to_mem: enter.\n");
	if (argc != 1)
		return CMD_RET_USAGE;
	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);

	list_for_each_entry(s, &eswin_display_list, head) {
		crtc_state = &s->crtc_state;
		regs = dev_read_addr_ptr(crtc_state->dev);
		printf("do_writeback_to_mem: regs=0x%px\n", regs);
		dc_test_writeback(regs, 0);
	}

	return 0;
}

static int do_read_hdmi_regs(struct cmd_tbl *cmdtp, int flag, int argc,
	char *const argv[])
{
	struct udevice *dev;
	struct display_state *s = NULL;
	struct connector_state *conn_state;

	printf("do read hdmi regs: enter.\n");

	uclass_get_device_by_name(UCLASS_VIDEO, "display-subsystem", &dev);

	list_for_each_entry(s, &eswin_display_list, head) {
		conn_state = &s->conn_state;
		conn_state->connector->funcs->dump_hdmi_regs(s);
	}

	return 0;
}

#ifdef CONFIG_ESWIN_LOGO_DISPLAY
U_BOOT_CMD(show_logo, 2, 1, do_eswin_logo_show,
	"load and display log from resource partition",
	"<value>"
);

U_BOOT_CMD(show_bmp, 2, 1, do_eswin_show_bmp,
	"load and display bmp from resource partition",
	"<bmp_name>"
);
#endif

U_BOOT_CMD(read_regs, 1, 1, do_read_dc8k_regs,
	"read and print value from dc8k regs",
	NULL
);

U_BOOT_CMD(write_regs, 2, 1, do_write_dc8k_regs,
	"write and print value from dc8k regs",
	"<value>"
);

U_BOOT_CMD(write_back, 1, 1, do_writeback_to_mem,
	"write and print value from dc8k regs",
	NULL
);

U_BOOT_CMD(read_hdmi_regs, 1, 1, do_read_hdmi_regs,
	"read and print the hdmi regs value",
	NULL
);

