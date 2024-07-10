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

#ifndef _ESWIN_BRIDGE_H_
#define _ESWIN_BRIDGE_H_

#include <config.h>
#include <common.h>
#include <dm/device.h>
#include <errno.h>

struct display_state;
struct eswin_bridge;

struct eswin_bridge_funcs {
	void (*enable)(struct eswin_bridge *bridge);
	void (*disable)(struct eswin_bridge *bridge);
	void (*pre_enable)(struct eswin_bridge *bridge);
	void (*post_disable)(struct eswin_bridge *bridge);
};

struct eswin_bridge {
	struct udevice *dev;
	const struct eswin_bridge_funcs *funcs;

	struct display_state *state;
};

void eswin_bridge_enable(struct eswin_bridge *bridge);
void eswin_bridge_disable(struct eswin_bridge *bridge);
void eswin_bridge_pre_enable(struct eswin_bridge *bridge);
void eswin_bridge_post_disable(struct eswin_bridge *bridge);

#endif
