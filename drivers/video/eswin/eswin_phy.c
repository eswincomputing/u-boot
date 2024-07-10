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
#include "eswin_phy.h"

int eswin_phy_init(struct eswin_phy *phy)
{
	if (!phy)
		return -ENODEV;

	if (phy->funcs && phy->funcs->init)
		return phy->funcs->init(phy);

	return 0;
}

int eswin_phy_power_on(struct eswin_phy *phy)
{
	if (!phy)
		return -ENODEV;

	if (phy->funcs && phy->funcs->power_on)
		return phy->funcs->power_on(phy);

	return 0;
}

int eswin_phy_power_off(struct eswin_phy *phy)
{
	if (!phy)
		return -ENODEV;

	if (phy->funcs && phy->funcs->power_off)
		return phy->funcs->power_off(phy);

	return 0;
}

unsigned long eswin_phy_set_pll(struct eswin_phy *phy,
				   unsigned long rate)
{
	if (!phy)
		return -ENODEV;

	if (phy->funcs && phy->funcs->set_pll)
		return phy->funcs->set_pll(phy, rate);

	return 0;
}

int eswin_phy_set_bus_width(struct eswin_phy *phy, unsigned int bus_width)
{
	if (!phy)
		return -ENODEV;

	if (phy->funcs && phy->funcs->set_bus_width)
		return phy->funcs->set_bus_width(phy, bus_width);

	return 0;
}

long eswin_phy_round_rate(struct eswin_phy *phy, unsigned long rate)
{
	if (!phy)
		return -ENODEV;

	if (phy->funcs && phy->funcs->round_rate)
		return phy->funcs->round_rate(phy, rate);

	return 0;
}

int eswin_phy_set_mode(struct eswin_phy *phy, enum phy_mode mode)
{
	if (!phy)
		return -ENODEV;

	if (phy->funcs && phy->funcs->set_mode)
		return phy->funcs->set_mode(phy, mode);

	return 0;
}
