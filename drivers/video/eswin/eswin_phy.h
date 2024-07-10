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

#ifndef _ESWIN_PHY_H_
#define _ESWIN_PHY_H_

enum phy_mode {
	PHY_MODE_INVALID,
	PHY_MODE_VIDEO_MIPI,
	PHY_MODE_VIDEO_LVDS,
	PHY_MODE_VIDEO_TTL,
};

struct eswin_phy;

struct eswin_phy_funcs {
	int (*init)(struct eswin_phy *phy);
	int (*power_on)(struct eswin_phy *phy);
	int (*power_off)(struct eswin_phy *phy);
	unsigned long (*set_pll)(struct eswin_phy *phy, unsigned long rate);
	int (*set_bus_width)(struct eswin_phy *phy, unsigned int bus_width);
	long (*round_rate)(struct eswin_phy *phy, unsigned long rate);
	int (*set_mode)(struct eswin_phy *phy, enum phy_mode mode);
};

struct eswin_phy {
	struct udevice *dev;
	const struct eswin_phy_funcs *funcs;
	const void *data;
	int soc_type;
};

int eswin_phy_init(struct eswin_phy *phy);
int eswin_phy_power_off(struct eswin_phy *phy);
int eswin_phy_power_on(struct eswin_phy *phy);
unsigned long eswin_phy_set_pll(struct eswin_phy *phy,
				   unsigned long rate);
int eswin_phy_set_bus_width(struct eswin_phy *phy, u32 bus_width);
long eswin_phy_round_rate(struct eswin_phy *phy, unsigned long rate);
int eswin_phy_set_mode(struct eswin_phy *phy, enum phy_mode mode);

#endif
