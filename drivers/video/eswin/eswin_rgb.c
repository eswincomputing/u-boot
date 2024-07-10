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
#include <errno.h>
#include <syscon.h>
#include <regmap.h>
#include <dm/device.h>
#include <dm/read.h>
#include <dm/pinctrl.h>
#include <media_bus_format.h>
#include <dm/device_compat.h>

#include "eswin_display.h"
#include "eswin_crtc.h"
#include "eswin_connector.h"
#include "eswin_phy.h"

#define HIWORD_UPDATE(v, h, l)		(((v) << (l)) | (GENMASK(h, l) << 16))

#define RK3288_GRF_SOC_CON6		0x025c
#define RK3288_LVDS_LCDC_SEL(v)		HIWORD_UPDATE(v,  3,  3)
#define RK3288_GRF_SOC_CON7		0x0260
#define RK3288_LVDS_PWRDWN(v)		HIWORD_UPDATE(v, 15, 15)
#define RK3288_LVDS_CON_ENABLE_2(v)	HIWORD_UPDATE(v, 12, 12)
#define RK3288_LVDS_CON_ENABLE_1(v)	HIWORD_UPDATE(v, 11, 11)
#define RK3288_LVDS_CON_CLKINV(v)	HIWORD_UPDATE(v,  8,  8)
#define RK3288_LVDS_CON_TTL_EN(v)	HIWORD_UPDATE(v,  6,  6)

#define RK3368_GRF_SOC_CON15		0x043c
#define RK3368_FORCE_JETAG(v)		HIWORD_UPDATE(v,  13,  13)

struct eswin_rgb;

struct eswin_rgb_funcs {
	void (*prepare)(struct eswin_rgb *rgb, int pipe);
	void (*unprepare)(struct eswin_rgb *rgb);
};

struct eswin_rgb {
	struct udevice *dev;
	struct regmap *grf;
	bool data_sync_bypass;
	struct eswin_phy *phy;
	const struct eswin_rgb_funcs *funcs;
};

static inline struct eswin_rgb *state_to_rgb(struct display_state *state)
{
	struct connector_state *conn_state = &state->conn_state;

	return dev_get_priv(conn_state->dev);
}

static int eswin_rgb_connector_prepare(struct display_state *state)
{
	struct eswin_rgb *rgb = state_to_rgb(state);
	struct crtc_state *crtc_state = &state->crtc_state;
	int pipe = crtc_state->crtc_id;
	int ret;

	pinctrl_select_state(rgb->dev, "default");

	if (rgb->funcs && rgb->funcs->prepare)
		rgb->funcs->prepare(rgb, pipe);

	if (rgb->phy) {
		ret = eswin_phy_set_mode(rgb->phy, PHY_MODE_VIDEO_TTL);
		if (ret) {
			dev_err(rgb->dev, "failed to set phy mode: %d\n", ret);
			return ret;
		}

		eswin_phy_power_on(rgb->phy);
	}

	return 0;
}

static void eswin_rgb_connector_unprepare(struct display_state *state)
{
	struct eswin_rgb *rgb = state_to_rgb(state);

	if (rgb->phy)
		eswin_phy_power_off(rgb->phy);

	if (rgb->funcs && rgb->funcs->unprepare)
		rgb->funcs->unprepare(rgb);

	pinctrl_select_state(rgb->dev, "sleep");
}

static int eswin_rgb_connector_init(struct display_state *state)
{
	struct eswin_rgb *rgb = state_to_rgb(state);
	struct connector_state *conn_state = &state->conn_state;

	rgb->phy = conn_state->phy;

	conn_state->type = DRM_MODE_CONNECTOR_LVDS;
	conn_state->color_space = V4L2_COLORSPACE_DEFAULT;

	switch (conn_state->bus_format) {
	case MEDIA_BUS_FMT_RGB666_1X18:
		conn_state->output_mode = ESWIN_OUT_MODE_P666;
		break;
	case MEDIA_BUS_FMT_RGB565_1X16:
		conn_state->output_mode = ESWIN_OUT_MODE_P565;
		break;
	case MEDIA_BUS_FMT_SRGB888_3X8:
	case MEDIA_BUS_FMT_SBGR888_3X8:
	case MEDIA_BUS_FMT_SRBG888_3X8:
		conn_state->output_mode = ESWIN_OUT_MODE_S888;
		break;
	case MEDIA_BUS_FMT_SRGB888_DUMMY_4X8:
	case MEDIA_BUS_FMT_SBGR888_DUMMY_4X8:
	case MEDIA_BUS_FMT_SRBG888_DUMMY_4X8:
		conn_state->output_mode = ESWIN_OUT_MODE_S888_DUMMY;
		break;
	case MEDIA_BUS_FMT_RGB888_1X24:
	case MEDIA_BUS_FMT_RGB666_1X24_CPADHI:
	default:
		conn_state->output_mode = ESWIN_OUT_MODE_P888;
		break;
	}

	return 0;
}

static const struct eswin_connector_funcs eswin_rgb_connector_funcs = {
	.init = eswin_rgb_connector_init,
	.prepare = eswin_rgb_connector_prepare,
	.unprepare = eswin_rgb_connector_unprepare,
};

static int eswin_rgb_probe(struct udevice *dev)
{
	struct eswin_rgb *rgb = dev_get_priv(dev);
	const struct eswin_connector *connector =
		(const struct eswin_connector *)dev_get_driver_data(dev);

	rgb->dev = dev;
	rgb->funcs = connector->data;
	rgb->grf = syscon_get_regmap(dev_get_parent(dev));
	rgb->data_sync_bypass = dev_read_bool(dev, "eswin,data-sync-bypass");

	return 0;
}

static void rk3288_rgb_prepare(struct eswin_rgb *rgb, int pipe)
{
	regmap_write(rgb->grf, RK3288_GRF_SOC_CON6, RK3288_LVDS_LCDC_SEL(pipe));
	regmap_write(rgb->grf, RK3288_GRF_SOC_CON7,
		     RK3288_LVDS_PWRDWN(0) | RK3288_LVDS_CON_ENABLE_2(1) |
		     RK3288_LVDS_CON_ENABLE_1(1) | RK3288_LVDS_CON_CLKINV(0) |
		     RK3288_LVDS_CON_TTL_EN(1));
}

static void rk3288_rgb_unprepare(struct eswin_rgb *rgb)
{
	regmap_write(rgb->grf, RK3288_GRF_SOC_CON7,
		     RK3288_LVDS_PWRDWN(1) | RK3288_LVDS_CON_ENABLE_2(0) |
		     RK3288_LVDS_CON_ENABLE_1(0) | RK3288_LVDS_CON_TTL_EN(0));
}

static const struct eswin_rgb_funcs rk3288_rgb_funcs = {
	.prepare = rk3288_rgb_prepare,
	.unprepare = rk3288_rgb_unprepare,
};

static const struct eswin_connector rk3288_rgb_driver_data = {
	.funcs = &eswin_rgb_connector_funcs,
	.data = &rk3288_rgb_funcs,
};

static void rk3368_rgb_prepare(struct eswin_rgb *rgb, int pipe)
{
	regmap_write(rgb->grf, RK3368_GRF_SOC_CON15, RK3368_FORCE_JETAG(0));
}

static const struct eswin_rgb_funcs rk3368_rgb_funcs = {
	.prepare = rk3368_rgb_prepare,
};

static const struct eswin_connector rk3368_rgb_driver_data = {
	.funcs = &eswin_rgb_connector_funcs,
	.data = &rk3368_rgb_funcs,
};

static const struct eswin_connector eswin_rgb_driver_data = {
	.funcs = &eswin_rgb_connector_funcs,
};

static const struct udevice_id eswin_rgb_ids[] = {
	{
		.compatible = "eswin,rk3288-rgb",
		.data = (ulong)&rk3288_rgb_driver_data,
	},
	{
		.compatible = "eswin,rk3368-rgb",
		.data = (ulong)&rk3368_rgb_driver_data,
	},
	{}
};

U_BOOT_DRIVER(eswin_rgb) = {
	.name = "eswin_rgb",
	.id = UCLASS_DISPLAY,
	.of_match = eswin_rgb_ids,
	.probe = eswin_rgb_probe,
	.priv_auto = sizeof(struct eswin_rgb),
};
