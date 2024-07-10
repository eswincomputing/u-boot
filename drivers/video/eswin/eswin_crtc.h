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

#ifndef _ESWIN_CRTC_H_
#define _ESWIN_CRTC_H_

struct eswin_crtc {
	const struct eswin_crtc_funcs *funcs;
	const void *data;
	struct drm_display_mode active_mode;
	bool hdmi_hpd : 1;
	bool active : 1;
};

struct eswin_crtc_funcs {
	int (*preinit)(struct display_state *state);
	int (*init)(struct display_state *state);
	int (*set_plane)(struct display_state *state);
	int (*prepare)(struct display_state *state);
	int (*enable)(struct display_state *state);
	int (*disable)(struct display_state *state);
	int (*unprepare)(struct display_state *state);
	int (*fixup_dts)(struct display_state *state, void *blob);
	int (*send_mcu_cmd)(struct display_state *state, u32 type, u32 value);
};

struct dc8000_data;
extern const struct eswin_crtc_funcs eswin_dc_funcs;
extern const struct dc8000_data dc8000_data_val;
#endif
