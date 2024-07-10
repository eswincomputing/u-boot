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
#include <asm/unaligned.h>
#include <asm/io.h>
#include <linux/delay.h>
#include <linux/list.h>

#include "eswin_dc_reg.h"

/*******************************************************************************
** Register access.
*******************************************************************************/
gctUINT eswin_dc_read_reg(void *regs, gctUINT addr)
{
    gctUINT read = 0;

    read = readl((unsigned char *)regs + addr);
    return read;
}

gctVOID eswin_dc_write_reg(void *regs, gctUINT addr, gctUINT data)
{
	// printf("DC--W: 0x%04x, 0x%08x\n", addr, data);
    writel(data, (gctUINT8 *)regs + addr);
}

static const struct dc8000_host_ctrl dc8000_host_ctrl_data = {
	.AQHiClkControl = 0x900,
	.AQAxiConfig = 0x222200,
	.AQIntrEnbl = 0x0,
};

static const struct dc8000_mem_ctrl dc8000_mem_ctrl_data = {
	.mmuConfiguration = 0x0,
	.mmuControl = 0x1F0,
	.mmuConfig = 0x0,
	.mmuAHBControl = 0x0,
};

static const struct dc8000_dec_ctrl dc8000_dec_ctrl_data = {
	.decControl = 0x21d0188,
	.decControlEx = 0x80000,
	.decControlEx2 = 0x003fc810,
	.decIntrEnbl = 0x0,
	.decIntrEnblEx = 0x0,
	.decIntrEnblEx2 = 0xffff,
};

static const struct dc8000_framebuffer_ctrl dc8000_framebuffer_ctrl_data = {
	.dcFBAddr0 = 0x0,
	.dcFBStride0 = 0x0,
	.dcFBColorKey0 = 0x0,
	.dcFBColorKeyHigh0 = 0x0,
	.dcFBConfig0 = 0x0,
	.dcFBScaleConfig0 = 0x0,
	.dcFBBGColor0 = 0x0,
	.dcFBSize0 = 0x0,
	.dcFBScaleFactorX0 = 0x0,
	.dcFBScaleFactorY0 = 0x0,
	.dcFBColorTableIndex0 = 0x0,
	.dcFBHorKernelIndex0 = 0x0,
	.dcFBVerKernelIndex0 = 0x0,
	.dcFBClearValue0 = 0x0,
	.dcFBInitialOffset0 = 0x0,
	.dcFBROIOrigin0 = 0x0,
	.dcFBROISize0 = 0x0,
	.dcFBConfigEx0 = 0x10,
	.dcFBWaterMark0 = 0x0,
};

static const struct dc8000_overlay_ctrl dc8000_overlay_ctrl_data = {
	.dcOverlayConfig0 = 0x00060020,
	.dcOverlayAlphaBlend0 = 0x40a,
	.dcOverlayAddr0 = 0x0,
	.dcOverlayStride0 = 0x0,
	.dcOverlayTL0 = 0x0,
	.dcOverlayBR0 = 0x0,
	.dcOverlaySrcGlobalColor0 = 0xff000000,
	.dcOverlayDstGlobalColor0 = 0xff000000,
	.dcOverlayColorKey0 = 0x0,
	.dcOverlayColorKeyHigh0 = 0x0,
	.dcOverlaySize0 = 0x0,
	.dcOverlayClearValue0 = 0x0,
	.dcOverlayScaleFactorX0 = 0x0,
	.dcOverlayScaleFactorY0 = 0x0,
	.dcOverlayInitialOffset0 = 0x0,
	.dcOverlayScaleConfig0 = 0x0,
	.dcOverlayColorTableIndex0 = 0x0,
	.dcOverlayHorKernelIndex0 = 0x0,
	.dcOverlayVerKernelIndex0 = 0x0,
	.dcOverlayROIOrigin0 = 0x0,
	.dcOverlayROISize0 = 0x0,
	.dcOverlayWaterMark0 = 0x0,
};

static const struct dc8000_display_ctrl dc8000_display_ctrl_data = {
	.dcDitherConfig0 = 0x0,
	.dcPanelConfig0 = 0x111,
	.dcDitherTabLow0 = 0x0,
	.dcDitherTabhigh0 = 0x0,
	.dcHDisplay0 = 0x0,
	.dcHSync0 = 0x0,
	.dcVDisplay0 = 0x0,
	.dcVSync0 = 0x0,
	.dcDisplayIntrEn = 0x1,
	.dcGeneralConfig0 = 0x0,
	.dcDpiConfig0 = 0x5,	//dpi output format: 5->D24
	.dcModuleClockGatingCtl0 = 0x0,
	.dcDpConfig0 = 0x0,
};

static const struct dc8000_dest_ctrl dc8000_dest_ctrl_data = {
	.dcMemDestAddr = 0xffffffff,
	.dcPanelDestAddr = 0xffffffff,
	.dcDestConfig = 0x30000,
	.dcDestStride = 0x0,
};

const struct dc8000_data dc8000_data_val = {
	.version = DC_VERSION(0x80, 0x00),
	.max_output = {3840, 2160},
	.feature = gcvFEATURE_DC_10BIT,
	.host_ctrl = &dc8000_host_ctrl_data,
	.mem_ctrl = &dc8000_mem_ctrl_data,
	.dec_ctrl = &dc8000_dec_ctrl_data,
	.fb_ctrl = &dc8000_framebuffer_ctrl_data,
	.overlay_ctrl = &dc8000_overlay_ctrl_data,
	.display_ctrl = &dc8000_display_ctrl_data,
	.dest_ctrl = &dc8000_dest_ctrl_data,
	.reg_len = dcregThreedLutEnlargeRegAddrs * 4,
};

/* dc clock Operation */
gctVOID eswin_dc_reset(struct dc8000_dc *dc)
{
    gctUINT idle = 0;
    gctUINT val;

    val = eswin_dc_read_reg(dc->regs, AQ_HI_CLOCK_CONTROL_Address);
    eswin_dc_write_reg(dc->regs, AQ_HI_CLOCK_CONTROL_Address, val | (1 << 12));
    eswin_dc_write_reg(dc->regs, AQ_HI_CLOCK_CONTROL_Address, val);

    do
    {
    	eswin_dc_write_reg(dc->regs, AQ_HI_IDLE_Address, 0);
        idle = eswin_dc_read_reg(dc->regs, AQ_HI_IDLE_Address);

        if (idle == 0)
        {
            udelay(100000);
        }
    } while ((idle & (1 << 16)) == 0);
}

/* dc memory operation */
gctVOID eswin_hw_mmu_enable(struct dc8000_dc *dc, gctBOOL enable)
{
	gctUINT config = 0;

	config = eswin_dc_read_reg(dc->regs, GCREG_MMUAHB_CONTROL_Address);
	if(enable) {
    	config |= GCREG_MMUAHB_CONTROL_MMU_ENABLE;
	}
	else {
		config &= ~GCREG_MMUAHB_CONTROL_MMU_ENABLE;
	}
	eswin_dc_write_reg(dc->regs, GCREG_MMUAHB_CONTROL_Address, config);
}

/* dc dec operation */
gctVOID eswin_hw_dec_disable(struct dc8000_dc *dc, gctBOOL disable)
{
	gctUINT config = 0;

	config = eswin_dc_read_reg(dc->regs, GCREG_AHBDEC_CONTROL_Address);
	if(disable) {
    	config |= GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION_DISABLE;
	}
	else {
		config &= ~GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION_DISABLE;
	}
	eswin_dc_write_reg(dc->regs, GCREG_AHBDEC_CONTROL_Address, config);
}

gctVOID eswin_hw_dec_inter_enable(struct dc8000_dc *dc, gctUINT inter)
{
	eswin_dc_write_reg(dc->regs, GCREG_AHBDEC_INTRENBL_Address, inter);
}

gctVOID eswin_hw_dec_inter_enable_ex(struct dc8000_dc *dc, gctUINT inter)
{
	eswin_dc_write_reg(dc->regs, GCREG_AHBDEC_INTRENBLEX_Address, inter);
}

gctVOID eswin_hw_dec_inter_enable_ex2(struct dc8000_dc *dc, gctUINT inter)
{
	eswin_dc_write_reg(dc->regs, GCREG_AHBDEC_INTRENBLEX2_Address, inter);
}

gctVOID eswin_hw_dec_control(struct dc8000_dc *dc, gctUINT control)
{
	eswin_dc_write_reg(dc->regs, GCREG_AHBDEC_CONTROL_Address, control);
}

gctVOID eswin_hw_dec_control_ex(struct dc8000_dc *dc, gctUINT control)
{
	eswin_dc_write_reg(dc->regs, GCREG_AHBDEC_CONTROLEX_Address, control);
}

gctVOID eswin_hw_dec_control_ex2(struct dc8000_dc *dc, gctUINT control)
{
	eswin_dc_write_reg(dc->regs, GCREG_AHBDEC_CONTROLEX2_Address, control);
}

/* dc Framebuffer Operation */
gctVOID eswin_hw_set_framebuffer_address(struct dc8000_dc *dc, gctUINT addr)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_ADDRESS_Address, addr);
}

gctVOID eswin_hw_set_framebuffer_stride(struct dc8000_dc *dc, gctUINT stride)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_STRIDE_Address, stride);
}

gctVOID eswin_hw_set_framebuffer_bgcolor(struct dc8000_dc *dc, gctUINT color)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_BG_COLOR_Address, color);
}

gctVOID eswin_hw_set_framebuffer_colorkey(struct dc8000_dc *dc, gctUINT colorKey)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_COLOR_KEY_Address, colorKey);
}

gctVOID eswin_hw_set_framebuffer_colorkeyhigh(struct dc8000_dc *dc, gctUINT colorKey)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_Address, colorKey);
}

gctVOID eswin_hw_set_framebuffer_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_CONFIG_Address, config);
}

gctUINT eswin_hw_get_framebuffer_config(struct dc8000_dc *dc)
{
    gctUINT config = 0;
    config = eswin_dc_read_reg(dc->regs, DCREG_FRAME_BUFFER_CONFIG_Address);
	return config;
}

gctVOID eswin_hw_set_framebuffer_scale_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_SCALE_CONFIG_Address, config);
}

gctVOID eswin_hw_set_framebuffer_colortable_index(struct dc8000_dc *dc, gctUINT index)
{
	eswin_dc_write_reg(dc->regs, DCREG_INDEX_COLOR_TABLE_INDEX_Address, index);
}

gctVOID eswin_hw_set_framebuffer_colortable_data(struct dc8000_dc *dc, gctUINT data)
{
	eswin_dc_write_reg(dc->regs, DCREG_INDEX_COLOR_TABLE_DATA_Address, data);
}

gctVOID eswin_hw_set_framebuffer_hor_index(struct dc8000_dc *dc, gctUINT index)
{
	eswin_dc_write_reg(dc->regs, DCREG_HORI_FILTER_KERNEL_INDEX_Address, index);
}

gctVOID eswin_hw_set_framebuffer_ver_index(struct dc8000_dc *dc, gctUINT index)
{
	eswin_dc_write_reg(dc->regs, DCREG_VERTI_FILTER_KERNEL_INDEX_Address, index);
}

gctVOID eswin_hw_set_framebuffer_hor_kernel(struct dc8000_dc *dc, gctUINT data)
{
	eswin_dc_write_reg(dc->regs, DCREG_HORI_FILTER_KERNEL_Address, data);
}

gctVOID eswin_hw_set_framebuffer_ver_kernel(struct dc8000_dc *dc, gctUINT data)
{
	eswin_dc_write_reg(dc->regs, DCREG_VERTI_FILTER_KERNEL_Address, data);
}

gctVOID eswin_hw_set_framebuffer_size(struct dc8000_dc *dc, gctUINT size)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_SIZE_Address, size);
}

gctVOID eswin_hw_set_framebuffer_scale_factorX(struct dc8000_dc *dc, gctUINT x)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_SCALE_FACTOR_X_Address, x);
}

gctVOID eswin_hw_set_framebuffer_scale_factorY(struct dc8000_dc *dc, gctUINT y)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_Address, y);
}

gctVOID eswin_hw_set_framebuffer_clear(struct dc8000_dc *dc, gctUINT value)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_CLEAR_VALUE_Address, value);
}

gctVOID eswin_hw_set_framebuffer_initial_offset(struct dc8000_dc *dc, gctUINT offset)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_INITIAL_OFFSET_Address, offset);
}

gctVOID eswin_hw_framebuffer_gamma_enable(struct dc8000_dc *dc, gctBOOL enable)
{
	gctUINT config = 0;

	config = eswin_dc_read_reg(dc->regs, DCREG_FRAME_BUFFER_CONFIG_Address);
	if(enable) {
    	config |= DCREG_FRAME_BUFFER_CONFIG_GAMMA_ENABLED;
	}
	else {
		config &= ~DCREG_FRAME_BUFFER_CONFIG_GAMMA_ENABLED;
	}
	eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_CONFIG_Address, config);
}

gctVOID eswin_hw_framebuffer_degamma_enable(struct dc8000_dc *dc, gctBOOL enable)
{
	gctUINT config = 0;

	config = eswin_dc_read_reg(dc->regs, DCREG_FRAME_BUFFER_CONFIG_EX_Address);
	if(enable) {
    	config |= DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA_ENABLED;
	}
	else {
		config &= ~DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA_ENABLED;
	}
	eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_CONFIG_EX_Address, config);
}

gctVOID eswin_hw_framebuffer_water_mark(struct dc8000_dc *dc, gctUINT mark)
{
	eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_WATER_MARK_Address, mark);
}

gctVOID eswin_hw_set_framebuffer_config_ex(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_FRAME_BUFFER_CONFIG_EX_Address, config);
}

/* dc overlay Operation */
gctVOID eswin_hw_overlay_enable(struct dc8000_dc *dc, gctBOOL enable)
{
	gctUINT config = 0;

	config = eswin_dc_read_reg(dc->regs, DCREG_OVERLAY_CONFIG_Address);
	if(enable) {
    	config |= DCREG_OVERLAY_CONFIG_ENABLE_ENABLE;
	}
	else {
		config &= ~DCREG_OVERLAY_CONFIG_ENABLE_ENABLE;
	}
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_CONFIG_Address, config);
}

gctVOID eswin_hw_set_overlay_address(struct dc8000_dc *dc, gctUINT addr)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_ADDRESS_Address, addr);
}

gctVOID eswin_hw_set_overlay_stride(struct dc8000_dc *dc, gctUINT stride)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_STRIDE_Address, stride);
}

gctVOID eswin_hw_set_overlay_size(struct dc8000_dc *dc, gctUINT size)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_SIZE_Address, size);
}

gctVOID eswin_hw_set_overlay_tl(struct dc8000_dc *dc, gctUINT tl)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_TL_Address, tl);
}

gctVOID eswin_hw_set_overlay_br(struct dc8000_dc *dc, gctUINT br)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_BR_Address, br);
}

gctVOID eswin_hw_set_overlay_scale_factorX(struct dc8000_dc *dc, gctUINT x)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_SCALE_FACTOR_X_Address, x);
}

gctVOID eswin_hw_set_overlay_scale_factorY(struct dc8000_dc *dc, gctUINT y)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_SCALE_FACTOR_Y_Address, y);
}

gctVOID eswin_hw_set_overlay_src_global_color(struct dc8000_dc *dc, gctUINT color)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_SRC_GLOBAL_COLOR_Address, color);
}

gctVOID eswin_hw_set_overlay_dst_global_color(struct dc8000_dc *dc, gctUINT color)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_DST_GLOBAL_COLOR_Address, color);
}

gctVOID eswin_hw_set_overlay_config(struct dc8000_dc *dc, gctUINT config)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_CONFIG_Address, config);
}

gctVOID eswin_hw_set_overlay_alpha_blend_config(struct dc8000_dc *dc, gctUINT config)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_ALPHA_BLEND_CONFIG_Address, config);
}

gctVOID eswin_hw_set_overlay_scale_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_SCALE_CONFIG_Address, config);
}

gctVOID eswin_hw_set_overlay_colortable_index(struct dc8000_dc *dc, gctUINT index)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_Address, index);
}

gctVOID eswin_hw_set_overlay_colortable_data(struct dc8000_dc *dc, gctUINT data)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_Address, data);
}

gctVOID eswin_hw_set_overlay_hor_index(struct dc8000_dc *dc, gctUINT index)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_Address, index);
}

gctVOID eswin_hw_set_overlay_ver_index(struct dc8000_dc *dc, gctUINT index)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_Address, index);
}

gctVOID eswin_hw_set_overlay_hor_kernel(struct dc8000_dc *dc, gctUINT data)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_HORI_FILTER_KERNEL_Address, data);
}

gctVOID eswin_hw_set_overlay_ver_kernel(struct dc8000_dc *dc, gctUINT data)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_VERTI_FILTER_KERNEL_Address, data);
}

gctVOID eswin_hw_set_overlay_initial_offset(struct dc8000_dc *dc, gctUINT offset)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_INITIAL_OFFSET_Address, offset);
}

gctVOID eswin_hw_set_overlay_colorkey(struct dc8000_dc *dc, gctUINT colorKey)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_COLOR_KEY_Address, colorKey);
}

gctVOID eswin_hw_set_overlay_colorkeyhigh(struct dc8000_dc *dc, gctUINT colorKey)
{
    eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_COLOR_KEY_HIGH_Address, colorKey);
}

gctVOID eswin_hw_overlay_water_mark(struct dc8000_dc *dc, gctUINT mark)
{
	eswin_dc_write_reg(dc->regs, DCREG_OVERLAY_WATER_MARK_Address, mark);
}

/* dc cursor Operation */
gctVOID eswin_hw_set_cursor_config(struct dc8000_dc *dc, gctUINT config)
{
	eswin_dc_write_reg(dc->regs, DCREG_CURSOR_CONFIG_Address, config);
}

gctVOID eswin_hw_set_cursor_module_clock_gating(struct dc8000_dc *dc, gctUINT control)
{
	eswin_dc_write_reg(dc->regs, DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_Address, control);
}

/* dc writeback Operation */
gctVOID eswin_hw_set_mem_dest_addr(struct dc8000_dc *dc, gctUINT addr)
{
	eswin_dc_write_reg(dc->regs, DCREG_MEM_DEST_ADDRESS_Address, addr);
}

gctVOID eswin_hw_set_panel_dest_addr(struct dc8000_dc *dc, gctUINT addr)
{
	eswin_dc_write_reg(dc->regs, DCREG_PANEL_DEST_ADDRESS_Address, addr);
}

gctVOID eswin_hw_set_dest_stride(struct dc8000_dc *dc, gctUINT stride)
{
	eswin_dc_write_reg(dc->regs, DCREG_DEST_STRIDE_Address, stride);
}

gctVOID eswin_hw_set_dest_config(struct dc8000_dc *dc, gctUINT config)
{
	eswin_dc_write_reg(dc->regs, DCREG_DEST_CONFIG_Address, config);
}

/* dc display Operation */
gctVOID eswin_hw_set_display_dither_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_CONFIG_Address, config);
}

gctVOID eswin_hw_set_panel_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_PANEL_CONFIG_Address, config);
}

gctVOID eswin_hw_set_general_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_GENERAL_CONFIG_Address, config);
}

gctVOID eswin_hw_set_dpi_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_DPI_CONFIG_Address, config);
}

gctVOID eswin_hw_set_dp_config(struct dc8000_dc *dc, gctUINT config)
{
    eswin_dc_write_reg(dc->regs, DCREG_DP_CONFIG_Address, config);
}

gctVOID eswin_hw_set_hdispay(struct dc8000_dc *dc, gctUINT hdispay)
{
    eswin_dc_write_reg(dc->regs, DCREG_HDISPLAY_Address, hdispay);
}

gctVOID eswin_hw_set_vdispay(struct dc8000_dc *dc, gctUINT vdispay)
{
    eswin_dc_write_reg(dc->regs, DCREG_VDISPLAY_Address, vdispay);
}

gctVOID eswin_hw_set_hsync(struct dc8000_dc *dc, gctUINT hsync)
{
    eswin_dc_write_reg(dc->regs, DCREG_HSYNC_Address, hsync);
}

gctVOID eswin_hw_set_vsync(struct dc8000_dc *dc, gctUINT vsync)
{
    eswin_dc_write_reg(dc->regs, DCREG_VSYNC_Address, vsync);
}

gctVOID eswin_hw_dither_enable(struct dc8000_dc *dc, gctBOOL enable, gctUINT low, gctUINT high)
{
    if (enable) {
        eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_TABLE_LOW_Address, low);
        eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_TABLE_HIGH_Address, high);
        eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_CONFIG_Address, 1);
    }
    else {
        eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_CONFIG_Address, 0);
        eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_TABLE_LOW_Address, 0);
        eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_TABLE_HIGH_Address, 0);
    }
}

gctVOID eswin_hw_set_interrupt_enable(struct dc8000_dc *dc, gctBOOL enable)
{
    gctUINT config = 0;

	config = eswin_dc_read_reg(dc->regs, DCREG_DISPLAY_INTR_ENABLE_Address);
    if (enable) {
        config |= DCREG_DISPLAY_INTR_ENABLE_DISP0_ENABLE;
    }
    else {
        config &= ~DCREG_DISPLAY_INTR_ENABLE_DISP0_ENABLE;
    }

    eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_INTR_ENABLE_Address, config);
}

gctUINT eswin_hw_get_interrupt_enable(struct dc8000_dc *dc)
{
    gctUINT enable = 0;
	
    enable = eswin_dc_read_reg(dc->regs, DCREG_DISPLAY_INTR_Address);
    return enable;
}

gctVOID eswin_hw_set_module_clock_gating(struct dc8000_dc *dc, gctUINT control)
{
	eswin_dc_write_reg(dc->regs, DCREG_MODULE_CLOCK_GATING_CONTROL_Address, control);
}

gctVOID eswin_hw_set_read_ot(struct dc8000_dc *dc, gctUINT ot)
{
	eswin_dc_write_reg(dc->regs, DCREG_READ_OT_Address, ot);
}

gctVOID eswin_hw_set_three_lut_enlarge(struct dc8000_dc *dc, gctUINT enlarge)
{
	eswin_dc_write_reg(dc->regs, DCREG_THREED_LUT_ENLARGE_Address, enlarge);
}

gctVOID eswin_hw_set_tsc_prefetch(struct dc8000_dc *dc, gctUINT prefetch)
{
	eswin_dc_write_reg(dc->regs, GCREG_TSC_PREFETCH_Address, prefetch);
}

gctVOID eswin_hw_set_display_dither_tablelow(struct dc8000_dc *dc, gctUINT table)
{
	eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_TABLE_LOW_Address, table);
}

gctVOID eswin_hw_set_display_dither_tablehigh(struct dc8000_dc *dc, gctUINT table)
{
	eswin_dc_write_reg(dc->regs, DCREG_DISPLAY_DITHER_TABLE_HIGH_Address, table);
}

/* system config */
void eswin_syscrg_config(int pclk)
{
	//volatile void *syscrg_reg_base = (volatile void *)(0x51828000);
	void *syscrg_reg_base = (void *)(0x51828000);
	// unsigned int val;

    eswin_dc_write_reg(syscrg_reg_base, 0x1b0, 0xc0000020);
    eswin_dc_write_reg(syscrg_reg_base, 0x1b4, 0x80000020);
    eswin_dc_write_reg(syscrg_reg_base, 0x1b8, 0x80000080);
    eswin_dc_write_reg(syscrg_reg_base, 0x1bc, 0x80000500);
	// eswin_dc_write_reg(syscrg_reg_base, 0x1b8, 0x80000100);
    // eswin_dc_write_reg(syscrg_reg_base, 0x1bc, 0x80000100);

    eswin_dc_write_reg(syscrg_reg_base, 0x1c0, 0x7);
    eswin_dc_write_reg(syscrg_reg_base, 0x484, 0x3f);
    eswin_dc_write_reg(syscrg_reg_base, 0x488, 0x3);
    eswin_dc_write_reg(syscrg_reg_base, 0x48c, 0xf);

	// //vo aclk
	// val = eswin_dc_read_reg(syscrg_reg_base, 0x1b0);
	// //printf("eswin_syscrg_config: val(0x1b0) = 0x%x\n", val);
	// val |= (1U << 31); 	//enable aclk
	// val |= (1 << 30);	//enable cfg_clk
	// val &= ~(1 << 20);	//vo_aclk_xtal24m_sel
	// eswin_dc_write_reg(syscrg_reg_base, 0x1b0, 0xc0000020);

	// //vo pclk
	// val = eswin_dc_read_reg(syscrg_reg_base, 0x1b8);
	// //printf("eswin_syscrg_config: val(0x1b8) = 0x%x\n", val);
	// val |= (1U << 31); 	//enable pclk
	// val &= ~(1 << 20);	//vo_aclk_xtal24m_sel
	// eswin_dc_write_reg(syscrg_reg_base, 0x1b8, 0x80000100);

	// udelay(5);

	// //vo reset
	// val = eswin_dc_read_reg(syscrg_reg_base, 0x48c);
	// //printf("eswin_syscrg_config: val(0x48c) = 0x%x\n", val);
	// val |= (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3); 	//axi cfg dc dcp
	// eswin_dc_write_reg(syscrg_reg_base, 0x48c, val);

	// //mipi reset
	// printf("mipi crg reset\n");
	// val = eswin_dc_read_reg(syscrg_reg_base, 0x484);
	// //printf("eswin_syscrg_config: val(0x484) = 0x%x\n", val);
	// val |= (1 << 0);
	// eswin_dc_write_reg(syscrg_reg_base, 0x484, val);

	udelay(5);
}