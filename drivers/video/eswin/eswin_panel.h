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

#ifndef _ESWIN_PANEL_H_
#define _ESWIN_PANEL_H_

struct display_state;
struct eswin_panel;

struct eswin_panel_funcs {
	void (*init)(struct eswin_panel *panel);
	void (*prepare)(struct eswin_panel *panel);
	void (*unprepare)(struct eswin_panel *panel);
	void (*enable)(struct eswin_panel *panel);
	void (*disable)(struct eswin_panel *panel);
};

struct eswin_panel {
	struct udevice *dev;
	unsigned int bus_format;
	unsigned int bpc;
	const struct eswin_panel_funcs *funcs;
	const void *data;

	struct display_state *state;
};

static inline void eswin_panel_init(struct eswin_panel *panel)
{
	if (!panel)
		return;

	if (panel->funcs && panel->funcs->init)
		panel->funcs->init(panel);
}

static inline void eswin_panel_prepare(struct eswin_panel *panel)
{
	if (!panel)
		return;

	if (panel->funcs && panel->funcs->prepare)
		panel->funcs->prepare(panel);
}

static inline void eswin_panel_enable(struct eswin_panel *panel)
{
	if (!panel)
		return;

	if (panel->funcs && panel->funcs->enable)
		panel->funcs->enable(panel);
}

static inline void eswin_panel_unprepare(struct eswin_panel *panel)
{
	if (!panel)
		return;

	if (panel->funcs && panel->funcs->unprepare)
		panel->funcs->unprepare(panel);
}

static inline void eswin_panel_disable(struct eswin_panel *panel)
{
	if (!panel)
		return;

	if (panel->funcs && panel->funcs->disable)
		panel->funcs->disable(panel);
}

#endif	/* _ESWIN_PANEL_H_ */
