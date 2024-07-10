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

#include "eswin_bridge.h"

void eswin_bridge_pre_enable(struct eswin_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->funcs && bridge->funcs->pre_enable)
		bridge->funcs->pre_enable(bridge);
}

void eswin_bridge_post_disable(struct eswin_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->funcs && bridge->funcs->post_disable)
		bridge->funcs->post_disable(bridge);
}

void eswin_bridge_enable(struct eswin_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->funcs && bridge->funcs->enable)
		bridge->funcs->enable(bridge);
}

void eswin_bridge_disable(struct eswin_bridge *bridge)
{
	if (!bridge)
		return;

	if (bridge->funcs && bridge->funcs->disable)
		bridge->funcs->disable(bridge);
}
