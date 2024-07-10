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
        297000000, {
            { 0x0000, 0x0205 },
            { 0x1658, 0x0219 },
            { 0x2648, 0x020f },
        },
    },  {
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
        297000000, { 0x3041, 0x3182, 0x3100 },
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
    { 594000000, 0x80f8, 0x0000, 0x0180},
    { ~0UL,      0x0000, 0x0000, 0x0000}
};

static unsigned int drm_eswin_select_color(struct hdmi_edid_data *edid_data,
                    struct base_screen_info *screen_info,
                    enum dw_hdmi_devtype dev_type)
{
    struct drm_display_info *info = &edid_data->display_info;
    struct drm_display_mode *mode = edid_data->preferred_mode;
    int max_tmds_clock = info->max_tmds_clock;
    bool support_dc = false;
    bool mode_420 = drm_mode_is_420(info, mode);
    unsigned int color_depth = 8;
    unsigned int base_color = DRM_HDMI_OUTPUT_YCBCR444;
    unsigned int color_format = DRM_HDMI_OUTPUT_DEFAULT_RGB;
    unsigned long tmdsclock, pixclock = mode->clock;

    if (screen_info)
        base_color = screen_info->format;

    switch (base_color) {
    case DRM_HDMI_OUTPUT_YCBCR_HQ:
        if (info->color_formats & DRM_COLOR_FORMAT_YCRCB444)
            color_format = DRM_HDMI_OUTPUT_YCBCR444;
        else if (info->color_formats & DRM_COLOR_FORMAT_YCRCB422)
            color_format = DRM_HDMI_OUTPUT_YCBCR422;
        else if (mode_420)
            color_format = DRM_HDMI_OUTPUT_YCBCR420;
        break;
    case DRM_HDMI_OUTPUT_YCBCR_LQ:
        if (mode_420)
            color_format = DRM_HDMI_OUTPUT_YCBCR420;
        else if (info->color_formats & DRM_COLOR_FORMAT_YCRCB422)
            color_format = DRM_HDMI_OUTPUT_YCBCR422;
        else if (info->color_formats & DRM_COLOR_FORMAT_YCRCB444)
            color_format = DRM_HDMI_OUTPUT_YCBCR444;
        break;
    case DRM_HDMI_OUTPUT_YCBCR420:
        if (mode_420)
            color_format = DRM_HDMI_OUTPUT_YCBCR420;
        break;
    case DRM_HDMI_OUTPUT_YCBCR422:
        if (info->color_formats & DRM_COLOR_FORMAT_YCRCB422)
            color_format = DRM_HDMI_OUTPUT_YCBCR422;
        break;
    case DRM_HDMI_OUTPUT_YCBCR444:
        if (info->color_formats & DRM_COLOR_FORMAT_YCRCB444)
            color_format = DRM_HDMI_OUTPUT_YCBCR444;
        break;
    case DRM_HDMI_OUTPUT_DEFAULT_RGB:
    default:
        break;
    }

    if (color_format == DRM_HDMI_OUTPUT_DEFAULT_RGB &&
        info->edid_hdmi_dc_modes & DRM_EDID_HDMI_DC_30)
        support_dc = true;
    if (color_format == DRM_HDMI_OUTPUT_YCBCR444 &&
        (info->edid_hdmi_dc_modes &
         (DRM_EDID_HDMI_DC_Y444 | DRM_EDID_HDMI_DC_30)))
        support_dc = true;
    if (color_format == DRM_HDMI_OUTPUT_YCBCR422)
        support_dc = true;
    if (color_format == DRM_HDMI_OUTPUT_YCBCR420 &&
        info->hdmi.y420_dc_modes & DRM_EDID_YCBCR420_DC_30)
        support_dc = true;

    if (mode->flags & DRM_MODE_FLAG_DBLCLK)
        pixclock *= 2;

    if (screen_info && screen_info->depth == 10)
        color_depth = screen_info->depth;

    if (color_format == DRM_HDMI_OUTPUT_YCBCR422 || color_depth == 8)
        tmdsclock = pixclock;
    else
        tmdsclock = pixclock * color_depth / 8;

    if (color_format == DRM_HDMI_OUTPUT_YCBCR420)
        tmdsclock /= 2;

    if (!max_tmds_clock)
        max_tmds_clock = 340000;

    max_tmds_clock = min(max_tmds_clock, 594000);

    if (tmdsclock > max_tmds_clock) {
        if (max_tmds_clock >= 594000) {
            color_depth = 8;
        } else if (max_tmds_clock > 340000) {
            if (drm_mode_is_420(info, mode))
                color_format = DRM_HDMI_OUTPUT_YCBCR420;
        } else {
            color_depth = 8;
            if (drm_mode_is_420(info, mode))
                color_format = DRM_HDMI_OUTPUT_YCBCR420;
        }
    }
    if (color_depth > 8 && support_dc) {
        switch (color_format) {
        case DRM_HDMI_OUTPUT_YCBCR444:
            return MEDIA_BUS_FMT_YUV10_1X30;
        case DRM_HDMI_OUTPUT_YCBCR422:
            return MEDIA_BUS_FMT_UYVY10_1X20;
        case DRM_HDMI_OUTPUT_YCBCR420:
            return MEDIA_BUS_FMT_UYYVYY10_0_5X30;
        default:
            return MEDIA_BUS_FMT_RGB101010_1X30;
        }
    } else {
        switch (color_format) {
        case DRM_HDMI_OUTPUT_YCBCR444:
            return MEDIA_BUS_FMT_YUV8_1X24;
        case DRM_HDMI_OUTPUT_YCBCR422:
            return MEDIA_BUS_FMT_UYVY8_1X16;
        case DRM_HDMI_OUTPUT_YCBCR420:
            return MEDIA_BUS_FMT_UYYVYY8_0_5X24;
        default:
            return MEDIA_BUS_FMT_RGB888_1X24;
        }
    }
}

void drm_eswin_selete_output(struct hdmi_edid_data *edid_data,
              unsigned int *bus_format,
              struct overscan *overscan,
              enum dw_hdmi_devtype dev_type)
{
    int ret, i, screen_size;
    struct base_disp_info base_parameter;
    const struct base_overscan *scan;
    struct base_screen_info *screen_info = NULL;
    int max_scan = 100;
    int min_scan = 51;
    struct blk_desc *dev_desc = NULL;
    struct disk_partition part_info;
    char baseparameter_buf[8 * ESWIN_BLK_SIZE] __aligned(ARCH_DMA_MINALIGN);

    overscan->left_margin = max_scan;
    overscan->right_margin = max_scan;
    overscan->top_margin = max_scan;
    overscan->bottom_margin = max_scan;

    *bus_format = MEDIA_BUS_FMT_RGB888_1X24;
#if 0
    dev_desc = eswin_get_bootdev();
    if (!dev_desc) {
        return;
    }

    if (part_get_info_by_name(dev_desc, "baseparameter", &part_info) < 0) {
        return;
    }

    ret = blk_dread(dev_desc, part_info.start, 1,
            (void *)baseparameter_buf);
    if (ret < 0) {
        return;
    }

    memcpy(&base_parameter, baseparameter_buf, sizeof(base_parameter));
    scan = &base_parameter.scan;

    if (scan->leftscale < min_scan && scan->leftscale > 0)
        overscan->left_margin = min_scan;
    else if (scan->leftscale < max_scan && scan->leftscale > 0)
        overscan->left_margin = scan->leftscale;

    if (scan->rightscale < min_scan && scan->rightscale > 0)
        overscan->right_margin = min_scan;
    else if (scan->rightscale < max_scan && scan->rightscale > 0)
        overscan->right_margin = scan->rightscale;

    if (scan->topscale < min_scan && scan->topscale > 0)
        overscan->top_margin = min_scan;
    else if (scan->topscale < max_scan && scan->topscale > 0)
        overscan->top_margin = scan->topscale;

    if (scan->bottomscale < min_scan && scan->bottomscale > 0)
        overscan->bottom_margin = min_scan;
    else if (scan->bottomscale < max_scan && scan->bottomscale > 0)
        overscan->bottom_margin = scan->bottomscale;

    screen_size = sizeof(base_parameter.screen_list) /
        sizeof(base_parameter.screen_list[0]);

    for (i = 0; i < screen_size; i++) {
        if (base_parameter.screen_list[i].type ==
            DRM_MODE_CONNECTOR_HDMIA) {
            screen_info = &base_parameter.screen_list[i];
            break;
        }
    }

    if (screen_info)
        printf("base_parameter.mode:%dx%d\n",
               screen_info->mode.hdisplay,
               screen_info->mode.vdisplay);
    drm_rk_select_mode(edid_data, screen_info);

    *bus_format = drm_eswin_select_color(edid_data, screen_info,
                      dev_type);
#endif
}

static const struct eswin_connector_funcs eswin_dw_hdmi_funcs = {
    .init = eswin_dw_hdmi_init,
    .deinit = eswin_dw_hdmi_deinit,
    .prepare = eswin_dw_hdmi_prepare,
    .enable = eswin_dw_hdmi_enable,
    .disable = eswin_dw_hdmi_disable,
    .get_timing = eswin_dw_hdmi_get_timing,
    .detect = eswin_dw_hdmi_detect,
    .get_edid = eswin_dw_hdmi_get_edid,
};

const struct dw_hdmi_plat_data eswin_hdmi_drv_data = {
    .mpll_cfg   = eswin_mpll_cfg,
    .cur_ctr    = eswin_cur_ctr,
    .phy_config = eswin_phy_config,
    .dev_type   = ESWIN_HDMI,
};

static int eswin_dw_hdmi_probe(struct udevice *dev)
{
    eswin_syscrg_config(0);
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
