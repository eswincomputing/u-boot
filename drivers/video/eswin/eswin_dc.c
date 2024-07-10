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

#include <config.h>
#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <fdtdec.h>
#include <fdt_support.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <linux/list.h>
#include <media_bus_format.h>
#include <clk.h>
#include <asm/arch-fu540/clk.h>
#include <linux/err.h>
#include <dm/device.h>
#include <dm/read.h>
#include <syscon.h>
#include "eswin_display.h"
#include "eswin_crtc.h"
#include "eswin_connector.h"
#include "eswin_dc.h"
#include "eswin_dc_reg.h"

static gctUINT32 lut[] = {
0x00000000, 0x00800000, 0x00008000, 0x00808000, 0x00000080, 0x00800080, 0x00008080, 0x00c0c0c0,
0x00c0dcc0, 0x00a6caf0, 0x00402000, 0x00602000, 0x00802000, 0x00a02000, 0x00c02000, 0x00e02000,
0x00004000, 0x00204000, 0x00404000, 0x00604000, 0x00804000, 0x00a04000, 0x00c04000, 0x00e04000,
0x00006000, 0x00206000, 0x00406000, 0x00606000, 0x00806000, 0x00a06000, 0x00c06000, 0x00e06000,
0x00008000, 0x00208000, 0x00408000, 0x00608000, 0x00808000, 0x00a08000, 0x00c08000, 0x00e08000,
0x0000a000, 0x0020a000, 0x0040a000, 0x0060a000, 0x0080a000, 0x00a0a000, 0x00c0a000, 0x00e0a000,
0x0000c000, 0x0020c000, 0x0040c000, 0x0060c000, 0x0080c000, 0x00a0c000, 0x00c0c000, 0x00e0c000,
0x0000e000, 0x0020e000, 0x0040e000, 0x0060e000, 0x0080e000, 0x00a0e000, 0x00c0e000, 0x00e0e000,
0x00000040, 0x00200040, 0x00400040, 0x00600040, 0x00800040, 0x00a00040, 0x00c00040, 0x00e00040,
0x00002040, 0x00202040, 0x00402040, 0x00602040, 0x00802040, 0x00a02040, 0x00c02040, 0x00e02040,
0x00004040, 0x00204040, 0x00404040, 0x00604040, 0x00804040, 0x00a04040, 0x00c04040, 0x00e04040,
0x00006040, 0x00206040, 0x00406040, 0x00606040, 0x00806040, 0x00a06040, 0x00c06040, 0x00e06040,
0x00008040, 0x00208040, 0x00408040, 0x00608040, 0x00808040, 0x00a08040, 0x00c08040, 0x00e08040,
0x0000a040, 0x0020a040, 0x0040a040, 0x0060a040, 0x0080a040, 0x00a0a040, 0x00c0a040, 0x00e0a040,
0x0000c040, 0x0020c040, 0x0040c040, 0x0060c040, 0x0080c040, 0x00a0c040, 0x00c0c040, 0x00e0c040,
0x0000e040, 0x0020e040, 0x0040e040, 0x0060e040, 0x0080e040, 0x00a0e040, 0x00c0e040, 0x00e0e040,
0x00000080, 0x00200080, 0x00400080, 0x00600080, 0x00800080, 0x00a00080, 0x00c00080, 0x00e00080,
0x00002080, 0x00202080, 0x00402080, 0x00602080, 0x00802080, 0x00a02080, 0x00c02080, 0x00e02080,
0x00004080, 0x00204080, 0x00404080, 0x00604080, 0x00804080, 0x00a04080, 0x00c04080, 0x00e04080,
0x00006080, 0x00206080, 0x00406080, 0x00606080, 0x00806080, 0x00a06080, 0x00c06080, 0x00e06080,
0x00008080, 0x00208080, 0x00408080, 0x00608080, 0x00808080, 0x00a08080, 0x00c08080, 0x00e08080,
0x0000a080, 0x0020a080, 0x0040a080, 0x0060a080, 0x0080a080, 0x00a0a080, 0x00c0a080, 0x00e0a080,
0x0000c080, 0x0020c080, 0x0040c080, 0x0060c080, 0x0080c080, 0x00a0c080, 0x00c0c080, 0x00e0c080,
0x0000e080, 0x0020e080, 0x0040e080, 0x0060e080, 0x0080e080, 0x00a0e080, 0x00c0e080, 0x00e0e080,
0x000000c0, 0x002000c0, 0x004000c0, 0x006000c0, 0x008000c0, 0x00a000c0, 0x00c000c0, 0x00e000c0,
0x000020c0, 0x002020c0, 0x004020c0, 0x006020c0, 0x008020c0, 0x00a020c0, 0x00c020c0, 0x00e020c0,
0x000040c0, 0x002040c0, 0x004040c0, 0x006040c0, 0x008040c0, 0x00a040c0, 0x00c040c0, 0x00e040c0,
0x000060c0, 0x002060c0, 0x004060c0, 0x006060c0, 0x008060c0, 0x00a060c0, 0x00c060c0, 0x00e060c0,
0x000080c0, 0x002080c0, 0x004080c0, 0x006080c0, 0x008080c0, 0x00a080c0, 0x00c080c0, 0x00e080c0,
0x0000a0c0, 0x0020a0c0, 0x0040a0c0, 0x0060a0c0, 0x0080a0c0, 0x00a0a0c0, 0x00c0a0c0, 0x00e0a0c0,
0x0000c0c0, 0x0020c0c0, 0x0040c0c0, 0x0060c0c0, 0x0080c0c0, 0x00a0c0c0, 0x00fffbf0, 0x00a0a0a4,
0x00808080, 0x00ff0000, 0x0000ff00, 0x00ffff00, 0x000000ff, 0x00ff00ff, 0x0000ffff, 0x00ffffff,
};

static gctUINT32 horKernel[] =
{
    0x00000000, 0x20000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x23fd1c03, 0x00000000,
    0x00000000, 0x00000000, 0x181f0000, 0x000027e1, 0x00000000, 0x00000000, 0x00000000, 0x2b981468,
    0x00000000, 0x00000000, 0x00000000, 0x10f00000, 0x00002f10, 0x00000000, 0x00000000, 0x00000000,
    0x32390dc7, 0x00000000, 0x00000000, 0x00000000, 0x0af50000, 0x0000350b, 0x00000000, 0x00000000,
    0x00000000, 0x3781087f, 0x00000000, 0x00000000, 0x00000000, 0x06660000, 0x0000399a, 0x00000000,
    0x00000000, 0x00000000, 0x3b5904a7, 0x00000000, 0x00000000, 0x00000000, 0x033c0000, 0x00003cc4,
    0x00000000, 0x00000000, 0x00000000, 0x3de1021f, 0x00000000, 0x00000000, 0x00000000, 0x01470000,
    0x00003eb9, 0x00000000, 0x00000000, 0x00000000, 0x3f5300ad, 0x00000000, 0x00000000, 0x00000000,
    0x00480000, 0x00003fb8, 0x00000000, 0x00000000, 0x00000000, 0x3fef0011, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00004000, 0x00000000, 0x00000000, 0x00000000, 0x20002000, 0x00000000,
    0x00000000, 0x00000000, 0x1c030000, 0x000023fd, 0x00000000, 0x00000000, 0x00000000, 0x27e1181f,
    0x00000000, 0x00000000, 0x00000000, 0x14680000, 0x00002b98, 0x00000000, 0x00000000, 0x00000000,
    0x2f1010f0, 0x00000000, 0x00000000, 0x00000000, 0x0dc70000, 0x00003239, 0x00000000, 0x00000000,
    0x00000000, 0x350b0af5, 0x00000000, 0x00000000, 0x00000000, 0x087f0000, 0x00003781, 0x00000000,
    0x00000000, 0x00000000, 0x399a0666, 0x00000000, 0x00000000, 0x00000000, 0x04a70000, 0x00003b59,
    0x00000000, 0x00000000, 0x00000000, 0x3cc4033c, 0x00000000, 0x00000000, 0x00000000, 0x021f0000,
};

static gctUINT32 verKernel[] =
{
    0x00000000, 0x20000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x23fd1c03, 0x00000000,
    0x00000000, 0x00000000, 0x181f0000, 0x000027e1, 0x00000000, 0x00000000, 0x00000000, 0x2b981468,
    0x00000000, 0x00000000, 0x00000000, 0x10f00000, 0x00002f10, 0x00000000, 0x00000000, 0x00000000,
    0x32390dc7, 0x00000000, 0x00000000, 0x00000000, 0x0af50000, 0x0000350b, 0x00000000, 0x00000000,
    0x00000000, 0x3781087f, 0x00000000, 0x00000000, 0x00000000, 0x06660000, 0x0000399a, 0x00000000,
    0x00000000, 0x00000000, 0x3b5904a7, 0x00000000, 0x00000000, 0x00000000, 0x033c0000, 0x00003cc4,
    0x00000000, 0x00000000, 0x00000000, 0x3de1021f, 0x00000000, 0x00000000, 0x00000000, 0x01470000,
    0x00003eb9, 0x00000000, 0x00000000, 0x00000000, 0x3f5300ad, 0x00000000, 0x00000000, 0x00000000,
    0x00480000, 0x00003fb8, 0x00000000, 0x00000000, 0x00000000, 0x3fef0011, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00004000, 0x00000000, 0xcdcd0000, 0xfdfdfdfd, 0xabababab, 0xabababab,
    0x00000000, 0x00000000, 0x5ff5f456, 0x000f5f58, 0x02cc6c78, 0x02cc0c28, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
};

static gctUINT get_stretch_factor(gctUINT src_size, gctUINT dest_size)
{
    gctUINT stretch_factor = 0;

    if ((src_size > 1) && (dest_size > 1)){
        stretch_factor = ((src_size - 1) << 16) / (dest_size - 1);
    }
    return stretch_factor;
}

static gctINT eswin_dc_init(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	const struct eswin_crtc *crtc = crtc_state->crtc;
	const struct dc8000_data *dc_data = crtc->data;
	struct dc8000_dc *dc;
	//struct clk dclk;
	gctINT ret;
	gctINT layer = state->layer;
	gctUINT scale_factorX = 0;
	gctUINT scale_factorY = 0;
	gctUINT16 src_width = state->logo.width;
	gctUINT16 src_height = state->logo.height;
	gctUINT16 hdisplay = mode->crtc_hdisplay;
    gctUINT16 htotal = mode->crtc_htotal;
    gctUINT16 hsync_st = mode->crtc_hsync_start;
    gctUINT16 hsync_end = mode->crtc_hsync_end;
    gctUINT16 vdisplay = mode->crtc_vdisplay;
    gctUINT16 vtotal = mode->crtc_vtotal;
    gctUINT16 vsync_st = mode->crtc_vsync_start;
    gctUINT16 vsync_end = mode->crtc_vsync_end;
	
	dc = malloc(sizeof(*dc));
	if (!dc) {
		printf("[%s] malloc dc mem failed.\n", __FUNCTION__);
		return -ENOMEM;
	}
	memset(dc, 0, sizeof(*dc));

	crtc_state->private = dc;
	dc->regs = dev_read_addr_ptr(crtc_state->dev);
	dc->reg_len = dc_data->reg_len;
	dc->regsbak = malloc(dc->reg_len);
	dc->win_offset = dc_data->win_offset;
	dc->host_ctrl = *dc_data->host_ctrl;
	dc->mem_ctrl = *dc_data->mem_ctrl;
	dc->dec_ctrl = *dc_data->dec_ctrl;
	dc->fb_ctrl = *dc_data->fb_ctrl;
	dc->overlay_ctrl = *dc_data->overlay_ctrl;
	dc->display_ctrl = *dc_data->display_ctrl;
	dc->dest_ctrl = *dc_data->dest_ctrl;
	dc->version = dc_data->version;

	printf("[%s]regs=0x%px, regsbak=0x%px\n", __FUNCTION__, dc->regs, dc->regsbak);

	/* Process 'assigned-{clocks/clock-parents/clock-rates}' properties */
	ret = clk_set_defaults(crtc_state->dev, 0);
	if (ret)
		printf("[%s]clk_set_defaults failed %d\n", __FUNCTION__, ret);

	//ret = clk_get_by_name(crtc_state->dev, "dclk_osd", &dclk);
	//if (!ret)
	//	ret = clk_set_rate(&dclk, mode->clock * 1000);
	//if (IS_ERR_VALUE(ret)) {
	//	printf("[%s]Failed to set dclk: ret=%d\n", __FUNCTION__, ret);
	//	return ret;
	//}

	printf("[%s]:layer:%d hdisplay = %d, htotal = %d.hsync_st:%d.hsync_end:%d\n", __FUNCTION__, layer, hdisplay, htotal,hsync_st,hsync_end);
	printf("[%s]: vdisplay = %d, vtotal = %d.vsync_st:%d.vsync_end:%d\n", __FUNCTION__, vdisplay, vtotal,vsync_st,vsync_end);

	//need scale
	if((src_width != hdisplay) || (src_height != vdisplay)) {
		printf("[%s]: need scaler.\n", __FUNCTION__);
		dc->is_scale = 1;
		scale_factorX = get_stretch_factor(src_width, hdisplay);
		scale_factorY = get_stretch_factor(src_height, vdisplay);
		if (scale_factorY > (3 << 16))
        {
            scale_factorY = (3 << 16);
        }

		if(layer == ESWIN_OVERLAY_LAYER) {
			dc->overlay_ctrl.dcOverlayScaleFactorX0 = scale_factorX;
			dc->overlay_ctrl.dcOverlayScaleFactorY0 = scale_factorY;
			memcpy(dc->overlay_ctrl.dcOverlayHorKernel0, horKernel, 128 * 4);
			memcpy(dc->overlay_ctrl.dcOverlayVerKernel0, verKernel, 128 * 4);
		} else {
			dc->fb_ctrl.dcFBScaleFactorX0 = scale_factorX;
			// dc->fb_ctrl.dcFBScaleFactorX0 = 0x00010000;
			dc->fb_ctrl.dcFBScaleFactorY0 = scale_factorY;
			memcpy(dc->fb_ctrl.dcFBHorKernel0, horKernel, 128 * 4);
			memcpy(dc->fb_ctrl.dcFBVerKernel0, verKernel, 128 * 4);
		}
	} else {
		dc->is_scale = 0;
		scale_factorX = 0;
		scale_factorY = 0;
	}

	if(layer == ESWIN_OVERLAY_LAYER) {
		dc->overlay_ctrl.dcOverlaySize0 = (src_height << 15) | src_width;
		dc->overlay_ctrl.dcOverlayStride0 = src_width * (state->logo.bpp / 8);
		dc->overlay_ctrl.dcOverlayTL0 = 0x0;
		dc->overlay_ctrl.dcOverlayBR0 = (vdisplay << 15) | hdisplay;
		memcpy(dc->overlay_ctrl.dcOverlayColorTableData0, lut, 256 * 4);
	}

	dc->fb_ctrl.dcFBSize0 = (src_height << 15) | src_width;
	dc->fb_ctrl.dcFBStride0 = src_width * (state->logo.bpp / 8);
	memcpy(dc->fb_ctrl.dcFBColorTableData0, lut, 256 * 4);
	printf("[%s]: src_width = %d, src_height = %d.\n", __FUNCTION__, src_width, src_height);

	dc->display_ctrl.dcHDisplay0 = ((htotal & 0x7fff) << 16) | (hdisplay & 0x7fff);
	dc->display_ctrl.dcVDisplay0 = ((vtotal & 0x7fff) << 16) | (vdisplay & 0x7fff);
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
	dc->display_ctrl.dcHSync0 = (0x1 << 30) | (hsync_end << 15) | hsync_st;
	dc->display_ctrl.dcVSync0 = (0x1 << 30) | (vsync_end << 15) | vsync_st;
#else
	dc->display_ctrl.dcHSync0 = (0x3 << 30) | (hsync_end << 15) | hsync_st;
    dc->display_ctrl.dcVSync0 = (0x3 << 30) | (vsync_end << 15) | vsync_st;
#endif
#ifdef CONFIG_DRM_ESWIN_WRITEBACK
	/* writeback init */
	dc->dest_ctrl.dcMemDestAddr = (gctUINT32 *)DEST_MEM_BUF_ADDR;
	dc->dest_ctrl.dcPanelDestAddr = (gctUINT32 *)DEST_MEM_BUF_ADDR;
	memset(dc->dest_ctrl.dcMemDestAddr, 0xff, hdisplay * vdisplay * 4);
	printf("[%s]: bpp = %d, *dst_buf = 0x%x.\n", __FUNCTION__, state->logo.bpp, *((gctUINT32 *)dc->dest_ctrl.dcMemDestAddr));
#endif
	return 0;
}

static gctINT eswin_dc_preinit(struct display_state *state)
{
	const struct dc8000_data *dc_data = state->crtc_state.crtc->data;

	state->crtc_state.max_output = dc_data->max_output;

	return 0;
}

static gctINT eswin_dc_set_plane(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct connector_state *conn_state = &state->conn_state;
	struct drm_display_mode *mode = &conn_state->mode;
	struct dc8000_dc *dc = crtc_state->private;
	gctUINT i = 0;
	gctINT layer = state->layer;

	if ((mode->crtc_hdisplay > crtc_state->max_output.width) ||
	    (mode->crtc_vdisplay > crtc_state->max_output.height)) {
		printf("[%s]Maximum destination %dx%d exceeded\n", __FUNCTION__,
		       crtc_state->max_output.width, crtc_state->max_output.height);
		return -EINVAL;
	}

	if(layer == ESWIN_OVERLAY_LAYER) {
		dc->overlay_ctrl.dcOverlayAddr0 = crtc_state->dma_addr;
		dc->overlay_ctrl.dcOverlayConfig0 = (crtc_state->format << DCREG_OVERLAY_CONFIG_FORMAT_Start) | (YUV_STANDARD_BT709 << DCREG_OVERLAY_CONFIG_YUV_Start);
	} else {
		dc->fb_ctrl.dcFBAddr0 = crtc_state->dma_addr;
	}
	dc->fb_ctrl.dcFBConfig0 = (crtc_state->format << DCREG_FRAME_BUFFER_CONFIG_FORMAT_Start) | (YUV_STANDARD_BT709 << DCREG_FRAME_BUFFER_CONFIG_YUV_Start);

	if(dc->is_scale) {
		if(layer == ESWIN_OVERLAY_LAYER) {
			dc->overlay_ctrl.dcOverlayScaleConfig0 |= (1 << DCREG_OVERLAY_SCALE_CONFIG_SCALE_Start);
			dc->overlay_ctrl.dcOverlayInitialOffset0 = 0x80008000;
		} else {
			dc->fb_ctrl.dcFBConfig0 |= (1 << DCREG_FRAME_BUFFER_CONFIG_SCALE_Start);
			dc->fb_ctrl.dcFBInitialOffset0 = 0x80008000;
		}
	}

	eswin_syscrg_config(mode->clock);
	//host interface
	eswin_dc_reset(dc);

	eswin_hw_set_framebuffer_config(dc, 0x10);		//dc8000 display control reset
	eswin_hw_set_framebuffer_config(dc, 0x0);

	//memory control
	eswin_hw_mmu_enable(dc, 0);

	//dec control
	eswin_hw_dec_disable(dc, 1);

	if(layer == ESWIN_OVERLAY_LAYER) {
		//overlay control
	    eswin_hw_set_overlay_address(dc, dc->overlay_ctrl.dcOverlayAddr0);
		eswin_hw_set_overlay_stride(dc, dc->overlay_ctrl.dcOverlayStride0);
		eswin_hw_set_overlay_tl(dc, dc->overlay_ctrl.dcOverlayTL0);
		eswin_hw_set_overlay_br(dc, dc->overlay_ctrl.dcOverlayBR0);
		eswin_hw_set_overlay_src_global_color(dc, dc->overlay_ctrl.dcOverlaySrcGlobalColor0);
		eswin_hw_set_overlay_dst_global_color(dc, dc->overlay_ctrl.dcOverlayDstGlobalColor0);
		eswin_hw_set_overlay_alpha_blend_config(dc, dc->overlay_ctrl.dcOverlayAlphaBlend0);
		eswin_hw_set_overlay_scale_config(dc, dc->overlay_ctrl.dcOverlayScaleConfig0);
		eswin_hw_set_overlay_size(dc, dc->overlay_ctrl.dcOverlaySize0);
		eswin_hw_set_overlay_colorkey(dc, dc->overlay_ctrl.dcOverlayColorKey0);
		eswin_hw_set_overlay_colorkeyhigh(dc, dc->overlay_ctrl.dcOverlayColorKeyHigh0);
		eswin_hw_overlay_water_mark(dc, dc->overlay_ctrl.dcOverlayWaterMark0);
		eswin_hw_set_overlay_colortable_index(dc, dc->overlay_ctrl.dcOverlayColorTableIndex0);
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
		for(i = 0; i < 256; i++)
			eswin_hw_set_overlay_colortable_data(dc, dc->overlay_ctrl.dcOverlayColorTableData0[i]);
#endif
	}
	//framebuffer control
	eswin_hw_set_framebuffer_address(dc, dc->fb_ctrl.dcFBAddr0);
	eswin_hw_set_framebuffer_stride(dc, dc->fb_ctrl.dcFBStride0);
	eswin_hw_set_framebuffer_scale_config(dc, dc->fb_ctrl.dcFBScaleConfig0);
	eswin_hw_set_framebuffer_bgcolor(dc, dc->fb_ctrl.dcFBBGColor0);
	eswin_hw_set_framebuffer_size(dc, dc->fb_ctrl.dcFBSize0);
	eswin_hw_set_framebuffer_colorkey(dc, dc->fb_ctrl.dcFBColorKey0);
	eswin_hw_set_framebuffer_colorkeyhigh(dc, dc->fb_ctrl.dcFBColorKeyHigh0);
	eswin_hw_framebuffer_water_mark(dc, dc->fb_ctrl.dcFBWaterMark0);
	eswin_hw_set_framebuffer_config_ex(dc, dc->fb_ctrl.dcFBConfigEx0);
	eswin_hw_set_framebuffer_colortable_index(dc, dc->fb_ctrl.dcFBColorTableIndex0);
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
	for(i = 0; i < 256; i++)
		eswin_hw_set_framebuffer_colortable_data(dc, dc->fb_ctrl.dcFBColorTableData0[i]);
#endif

	if(dc->is_scale) {
		if(layer == ESWIN_OVERLAY_LAYER) {
			eswin_hw_set_overlay_scale_factorX(dc, dc->overlay_ctrl.dcOverlayScaleFactorX0);
			eswin_hw_set_overlay_scale_factorY(dc, dc->overlay_ctrl.dcOverlayScaleFactorY0);

			eswin_hw_set_overlay_hor_index(dc, dc->overlay_ctrl.dcOverlayHorKernelIndex0);
			eswin_hw_set_overlay_ver_index(dc, dc->overlay_ctrl.dcOverlayVerKernelIndex0);
			for(i = 0; i < 128; i++)
				eswin_hw_set_overlay_hor_kernel(dc, dc->overlay_ctrl.dcOverlayHorKernel0[i]);
			for(i = 0; i < 128; i++)
				eswin_hw_set_overlay_ver_kernel(dc, dc->overlay_ctrl.dcOverlayVerKernel0[i]);
			eswin_hw_set_overlay_initial_offset(dc, dc->overlay_ctrl.dcOverlayInitialOffset0);
		} else {
			eswin_hw_set_framebuffer_scale_factorX(dc, dc->fb_ctrl.dcFBScaleFactorX0);
			eswin_hw_set_framebuffer_scale_factorY(dc, dc->fb_ctrl.dcFBScaleFactorY0);

			eswin_hw_set_framebuffer_hor_index(dc, dc->fb_ctrl.dcFBHorKernelIndex0);
			eswin_hw_set_framebuffer_ver_index(dc, dc->fb_ctrl.dcFBVerKernelIndex0);
			for(i = 0; i < 128; i++)
				eswin_hw_set_framebuffer_hor_kernel(dc, dc->fb_ctrl.dcFBHorKernel0[i]);
			for(i = 0; i < 128; i++)
				eswin_hw_set_framebuffer_ver_kernel(dc, dc->fb_ctrl.dcFBVerKernel0[i]);
			eswin_hw_set_framebuffer_initial_offset(dc, dc->fb_ctrl.dcFBInitialOffset0);
		}
	}

	//cursor control
	eswin_hw_set_cursor_config(dc, 0x0);
	eswin_hw_set_cursor_module_clock_gating(dc, 0x1);
#ifdef CONFIG_DRM_ESWIN_WRITEBACK
	//writeback control
	eswin_hw_set_mem_dest_addr(dc, dc->dest_ctrl.dcMemDestAddr);			//dc8000 writeback memory address	
	eswin_hw_set_panel_dest_addr(dc, dc->dest_ctrl.dcPanelDestAddr);		//dc8000 writeback panel mem address
	eswin_hw_set_dest_stride(dc, dc->dest_ctrl.dcDestStride);				//dc8000 writeback mem stride
	eswin_hw_set_dest_config(dc, dc->dest_ctrl.dcDestConfig);				//dc8000 writeback enable
#endif
	//display control
	eswin_hw_set_interrupt_enable(dc, dc->display_ctrl.dcDisplayIntrEn);     //Display interrupt enable register
	eswin_hw_set_display_dither_config(dc, dc->display_ctrl.dcDitherConfig0);
    eswin_hw_set_hdispay(dc, dc->display_ctrl.dcHDisplay0);
    eswin_hw_set_vdispay(dc, dc->display_ctrl.dcVDisplay0);
    eswin_hw_set_hsync(dc, dc->display_ctrl.dcHSync0);
    eswin_hw_set_vsync(dc, dc->display_ctrl.dcVSync0);
	eswin_hw_set_panel_config(dc, dc->display_ctrl.dcPanelConfig0);
	eswin_hw_set_dpi_config(dc, dc->display_ctrl.dcDpiConfig0);
	eswin_hw_set_module_clock_gating(dc, dc->display_ctrl.dcModuleClockGatingCtl0);
	eswin_hw_set_read_ot(dc, 0x0);
	eswin_hw_set_dp_config(dc, dc->display_ctrl.dcDpConfig0);
	eswin_hw_set_three_lut_enlarge(dc, 0x0);
	eswin_hw_set_tsc_prefetch(dc, 0x1);
	eswin_hw_set_display_dither_tablelow(dc, dc->display_ctrl.dcDitherTabLow0);
	eswin_hw_set_display_dither_tablehigh(dc, dc->display_ctrl.dcDitherTabhigh0);

	//memcpy(dc->regsbak, dc->regs, dc->reg_len);
	return 0;
}

static gctINT eswin_dc_prepare(struct display_state *state)
{
	return 0;
}

static gctINT eswin_dc_unprepare(struct display_state *state)
{
    return 0;
}

static gctINT eswin_dc_enable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct dc8000_dc *dc = crtc_state->private;
	gctINT layer = state->layer;

	if(layer == ESWIN_OVERLAY_LAYER) {
		eswin_hw_set_overlay_config(dc, dc->overlay_ctrl.dcOverlayConfig0 | 0x1000000); //overlay0 configuration register
	}
	eswin_hw_set_framebuffer_config(dc, dc->fb_ctrl.dcFBConfig0 | 0x11);    		//Framebuffer0 configuration register
#ifdef CONFIG_DRM_ESWIN_WRITEBACK
	if(layer == ESWIN_OVERLAY_LAYER) {
		if(state->logo.bpp == 32) {
	    	printf("[%s]: after writeback => *src_buf = 0x%x, *dst_buf = 0x%x.\n", __FUNCTION__, *((gctUINT32 *)dc->overlay_ctrl.dcOverlayAddr0), *((gctUINT32 *)dc->dest_ctrl.dcMemDestAddr));
		} else if(state->logo.bpp == 16) {
	    	printf("[%s]: after writeback => *src_buf = 0x%x, *dst_buf = 0x%x.\n", __FUNCTION__, *((gctUINT16 *)dc->overlay_ctrl.dcOverlayAddr0), *((gctUINT32 *)dc->dest_ctrl.dcMemDestAddr));
		}
	} else {
		if(state->logo.bpp == 32) {
	    	printf("[%s]: after writeback => *src_buf = 0x%x, *dst_buf = 0x%x.\n", __FUNCTION__, *((gctUINT32 *)dc->fb_ctrl.dcFBAddr0), *((gctUINT32 *)dc->dest_ctrl.dcMemDestAddr));
		} else if(state->logo.bpp == 16) {
	    	printf("[%s]: after writeback => *src_buf = 0x%x, *dst_buf = 0x%x.\n", __FUNCTION__, *((gctUINT16 *)dc->fb_ctrl.dcFBAddr0), *((gctUINT32 *)dc->dest_ctrl.dcMemDestAddr));
		}
	}
#endif
	return 0;
}

static gctINT eswin_dc_disable(struct display_state *state)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct dc8000_dc *dc = crtc_state->private;

	eswin_hw_set_framebuffer_config(dc, dc->fb_ctrl.dcFBConfig0 & 0xfffffffe);
	return 0;
}

static gctINT eswin_dc_fixup_dts(struct display_state *state, void *blob)
{
	return 0;
}

static gctINT eswin_dc_send_mcu_cmd(struct display_state *state,
				     gctUINT32 type, gctUINT32 value)
{
	struct crtc_state *crtc_state = &state->crtc_state;
	struct dc8000_dc *dc = crtc_state->private;

	if (dc) {
		switch (type) {
		case MCU_WRCMD:
			
			break;
		case MCU_WRDATA:

			break;
		case MCU_SETBYPASS:
			
			break;
		default:
			break;
		}
	}

	return 0;
}

const struct eswin_crtc_funcs eswin_dc_funcs = {
	.preinit = eswin_dc_preinit,
	.init = eswin_dc_init,
	.set_plane = eswin_dc_set_plane,
	.prepare = eswin_dc_prepare,
	.unprepare = eswin_dc_unprepare,
	.enable = eswin_dc_enable,
	.disable = eswin_dc_disable,
	.fixup_dts = eswin_dc_fixup_dts,
	.send_mcu_cmd = eswin_dc_send_mcu_cmd,
};
