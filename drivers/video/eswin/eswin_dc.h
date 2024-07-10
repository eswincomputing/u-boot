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

#ifndef _ESWIN_DC_H_
#define _ESWIN_DC_H_

#include <drm/eswin_dc_type.h>
#include "eswin_display.h"

/*
 * major: IP major vertion, used for IP structure
 * minor: big feature change under same structure
 */
#define DC_VERSION(major, minor)	((major) << 8 | (minor))
#define DC_MAJOR(version) 	((version) >> 8)
#define DC_MINOR(version) 	((version) & 0xff)

#define YUV_STANDARD_BT709			0x1
#define YUV_STANDARD_BT2020			0x3
#define DEST_MEM_BUF_ADDR		(0x80000000U + (32 * 1024 * 1024))

struct dc8000_host_ctrl {
	gctUINT32 AQHiClkControl;
	gctUINT32 AQAxiConfig;
	gctUINT32 AQIntrEnbl;
};

struct dc8000_mem_ctrl {
	gctUINT32 mmuConfiguration;
	gctUINT32 mmuControl;
	gctUINT32 mmuConfig;
	gctUINT32 mmuAHBControl;
};

struct dc8000_dec_ctrl {
	gctUINT32 decControl;
	gctUINT32 decControlEx;
	gctUINT32 decControlEx2;
	gctUINT32 decIntrEnbl;
	gctUINT32 decIntrEnblEx;
	gctUINT32 decIntrEnblEx2;
};

//reg page 275
struct dc8000_framebuffer_ctrl {
	gctUINT32 dcFBAddr0;
	gctUINT32 dcFBStride0;
	gctUINT32 dcFBColorKey0;
	gctUINT32 dcFBColorKeyHigh0;
	gctUINT32 dcFBConfig0;
	gctUINT32 dcFBScaleConfig0;
	gctUINT32 dcFBBGColor0;
	gctUINT32 dcFBSize0;
	gctUINT32 dcFBColorTableIndex0;
	gctUINT32 dcFBColorTableData0[256];
	gctUINT32 dcFBScaleFactorX0;
	gctUINT32 dcFBScaleFactorY0;
	gctUINT32 dcFBHorKernelIndex0;
    gctUINT32 dcFBVerKernelIndex0;
    gctUINT32 dcFBHorKernel0[128];
    gctUINT32 dcFBVerKernel0[128];
	gctUINT32 dcFBClearValue0;
	gctUINT32 dcFBInitialOffset0;
	gctUINT32 dcFBROIOrigin0;
	gctUINT32 dcFBROISize0;
	gctUINT32 dcFBConfigEx0;
	gctUINT32 dcFBWaterMark0;
};

//reg page 293
struct dc8000_overlay_ctrl {
	gctUINT32 dcOverlayConfig0;
	gctUINT32 dcOverlayAlphaBlend0;
    gctUINT32 dcOverlayAddr0;
	gctUINT32 dcOverlayStride0;
	gctUINT32 dcOverlayTL0;
	gctUINT32 dcOverlayBR0;
	gctUINT32 dcOverlaySrcGlobalColor0;
	gctUINT32 dcOverlayDstGlobalColor0;
	gctUINT32 dcOverlayColorKey0;
	gctUINT32 dcOverlayColorKeyHigh0;
	gctUINT32 dcOverlaySize0;
	gctUINT32 dcOverlayColorTableIndex0;
	gctUINT32 dcOverlayColorTableData0[256];
	gctUINT32 dcOverlayClearValue0;
	gctUINT32 dcOverlayScaleFactorX0;
	gctUINT32 dcOverlayScaleFactorY0;
	gctUINT32 dcOverlayHorKernelIndex0;
    gctUINT32 dcOverlayVerKernelIndex0;
    gctUINT32 dcOverlayHorKernel0[128];
    gctUINT32 dcOverlayVerKernel0[128];
	gctUINT32 dcOverlayInitialOffset0;
	gctUINT32 dcOverlayScaleConfig0;
	gctUINT32 dcOverlayROIOrigin0;
	gctUINT32 dcOverlayROISize0;
	gctUINT32 dcOverlayWaterMark0;
};

struct dc8000_display_ctrl {
	gctUINT32 dcDitherConfig0;
	gctUINT32 dcPanelConfig0;
	gctUINT32 dcDitherTabLow0;
	gctUINT32 dcDitherTabhigh0;
	gctUINT32 dcHDisplay0;
	gctUINT32 dcHSync0;
	gctUINT32 dcVDisplay0;
	gctUINT32 dcVSync0;
	gctUINT32 dcDisplayIntrEn;
	gctUINT32 dcGeneralConfig0;
	gctUINT32 dcDpiConfig0;
	gctUINT32 dcModuleClockGatingCtl0;
	gctUINT32 dcDpConfig0;
};

struct dc8000_dest_ctrl {
	gctUINT32 dcMemDestAddr;
	gctUINT32 dcPanelDestAddr;
	gctUINT32 dcDestConfig;
	gctUINT32 dcDestStride;
};

struct dc8000_data {
	gctUINT32 version;
	const struct dc8000_host_ctrl *host_ctrl;
	const struct dc8000_mem_ctrl *mem_ctrl;
	const struct dc8000_dec_ctrl *dec_ctrl;
	const struct dc8000_framebuffer_ctrl *fb_ctrl;
	const struct dc8000_overlay_ctrl *overlay_ctrl;
	const struct dc8000_display_ctrl *display_ctrl;
	const struct dc8000_dest_ctrl *dest_ctrl;
	gctUINT32 reg_len;
	gctINT win_offset;
	gctUINT64 feature;
	struct eswin_rect max_output;
};

struct dc8000_dc {
	gctUINT32 *regsbak;
	void *regs;

	gctUINT32 version;
	struct dc8000_host_ctrl host_ctrl;
	struct dc8000_mem_ctrl mem_ctrl;
	struct dc8000_dec_ctrl dec_ctrl;
	struct dc8000_framebuffer_ctrl fb_ctrl;
	struct dc8000_overlay_ctrl overlay_ctrl;
	struct dc8000_display_ctrl display_ctrl;
	struct dc8000_dest_ctrl dest_ctrl;
	gctUINT32 reg_len;
	gctINT win_offset;
	gctUINT32 is_scale;
};

#endif
