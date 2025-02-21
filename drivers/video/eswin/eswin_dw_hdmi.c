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

#include <common.h>
#include <asm/cache.h>
#include <asm/io.h>
#include <dm/device.h>
#include <part.h>
#include <linux/dw_hdmi.h>
#include <linux/hdmi.h>
#include <media_bus_format.h>
#include "eswin_display.h"
#include "eswin_crtc.h"
#include "eswin_connector.h"
#include "dw_hdmi.h"
#include "eswin_dw_hdmi.h"
#include "eswin_dc_reg.h"

#define ESWIN_BLK_SIZE 512

static const struct dw_hdmi_mpll_config eswin_mpll_cfg[] = {
    {
        27000000, {
            { 0x0003, 0x0628 },
            { 0x1003, 0x0632 },
            { 0x2003, 0x023c },
        },
    },  {
        54000000, {
            { 0x0002, 0x0614 },
            { 0x1002, 0x0619 },
            { 0x2002, 0x021e },
        },
    },  {
        74250000, {
            { 0x0002, 0x0214 },
            { 0x1009, 0x0619 },
            { 0x2001, 0x060f },
        },
    },  {
        108000000, {
            { 0x0001, 0x060a },
            { 0x1009, 0x0619 },
            { 0x2001, 0x020f },
        },
    },  {
        148500000, {
            { 0x0001, 0x020a },
            { 0x1018, 0x0619 },
            { 0x2008, 0x060f },
        },
    },  {
		235690000, {
			{ 0x0000, 0x0605 },
			{ 0x1018, 0x0219 },
			{ 0x2648, 0x020f },
		},
	}, {
        297000000, {
            { 0x0000, 0x0205 },
            { 0x1658, 0x0219 },
            { 0x2648, 0x020f },
        },
    }, {
		371370000, {
			{ 0x0640, 0x0205 },
			{ 0x1658, 0x0019 },
			{ 0x2648, 0x000f },
		},
	}, {
		513820000,
		{
			{ 0x0640, 0x0005 },
			{ 0x1658, 0x0019 },
			{ 0x2648, 0x000f },
		},
	}, {
        594000000, {
            { 0x0640, 0x0005 },
            { 0x1658, 0x0019 },
            { 0x2648, 0x000f },
        },
    },  {
        ~0UL, {
            { 0x0000, 0x0000 },
            { 0x0000, 0x0000 },
            { 0x0000, 0x0000 },
        },
    }
};

static const struct dw_hdmi_curr_ctrl eswin_cur_ctr[] = {
    /*  pixelclk     bpp8    bpp10   bpp12  */
    {
        27000000,  { 0x0283, 0x0281, 0x02c2 },
    },  {
        54000000,  { 0x1183, 0x1203, 0x1202 },
    },  {
        74250000,  { 0x1142, 0x2203, 0x2141 },
    },  {
        108000000, { 0x20c0, 0x2203, 0x2100 },
    },  {
        148500000, { 0x2080, 0x3203, 0x3141 },
    },  {
		235690000, { 0x3040, 0x3182, 0x3100 },
	},  {
        297000000, { 0x3041, 0x3182, 0x3100 },
    },  {
		371370000, { 0x3041, 0x31c0, 0x3100 },
	},  {
		513820000, { 0x3080, 0x31c0, 0x3100 },
	},  {
        594000000, { 0x3080, 0x31c0, 0x3100 },
    },  {
        ~0UL,      { 0x0000, 0x0000, 0x0000 },
    }
};

static const struct dw_hdmi_phy_config eswin_phy_config[] = {
    /*pixelclk   symbol   term   vlev*/
    { 165000000, 0x8088, 0x0007, 0x0180},
    { 297000000, 0x80c8, 0x0004, 0x0180},
    { 594000000, 0x80f3, 0x0000, 0x0180},
    { ~0UL,      0x0000, 0x0000, 0x0000}
};

void drm_eswin_selete_output(struct hdmi_edid_data *edid_data,
              unsigned int *bus_format,
              struct overscan *overscan,
              enum dw_hdmi_devtype dev_type)
{
    int max_scan = 100;
    overscan->left_margin = max_scan;
    overscan->right_margin = max_scan;
    overscan->top_margin = max_scan;
    overscan->bottom_margin = max_scan;

    *bus_format = MEDIA_BUS_FMT_RGB888_1X24;
}

static const struct eswin_connector_funcs eswin_dw_hdmi_funcs = {
    .init = eswin_dw_hdmi_init,
    .deinit = eswin_dw_hdmi_deinit,
    .prepare = eswin_dw_hdmi_prepare,
    .enable = eswin_dw_hdmi_enable,
    .disable = eswin_dw_hdmi_disable,
    .get_timing = eswin_dw_hdmi_get_timing,
    .detect = eswin_dw_hdmi_detect,
    .dump_hdmi_phy_regs = eswin_dw_hdmi_dump_phy,
};

const struct dw_hdmi_plat_data eswin_hdmi_drv_data = {
    .mpll_cfg   = eswin_mpll_cfg,
    .cur_ctr    = eswin_cur_ctr,
    .phy_config = eswin_phy_config,
    .dev_type   = ESWIN_HDMI,
};

static int eswin_dw_hdmi_probe(struct udevice *dev)
{
    eswin_vo_clk_init(DEFAULT_PIXEL_CLK);
    return 0;
}

static const struct eswin_connector eswin_dw_hdmi_data = {
    .funcs = &eswin_dw_hdmi_funcs,
    .data = &eswin_hdmi_drv_data,
};

static const struct udevice_id eswin_dw_hdmi_ids[] = {
    {
     .compatible = "eswin,eswin-dw-hdmi",
     .data = (ulong)&eswin_dw_hdmi_data,
    },{}
};

U_BOOT_DRIVER(eswin_dw_hdmi) = {
    .name = "eswin_dw_hdmi",
    .id = UCLASS_DISPLAY,
    .of_match = eswin_dw_hdmi_ids,
    .probe  = eswin_dw_hdmi_probe,
};
