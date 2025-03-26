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

#ifndef _ESWIN_CONNECTOR_H_
#define _ESWIN_CONNECTOR_H_

struct eswin_connector {
	const struct eswin_connector_funcs *funcs;

	const void *data;
};

struct eswin_connector_funcs {
	int (*init)(struct display_state *state);
	void (*deinit)(struct display_state *state);
	int (*detect)(struct display_state *state);
	int (*get_timing)(struct display_state *state);
	int (*prepare)(struct display_state *state);
	int (*enable)(struct display_state *state);
	int (*disable)(struct display_state *state);
	void (*unprepare)(struct display_state *state);
	int (*dump_hdmi_phy_regs)(struct display_state *state);
	int (*dump_hdmi_regs)(struct display_state *state);
};

const struct eswin_connector *
eswin_get_connector(const void *blob, int connector_node);

#endif
