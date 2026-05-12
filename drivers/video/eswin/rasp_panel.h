// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2026, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
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

 #ifndef _RASP_PANEL_H_
 #define _RASP_PANEL_H_
 

 /**
 * struct panel_delay - Panel timing delays
 * @prepare: Time to wait after power enable before preparing panel (ms)
 * @enable: Time to wait after prepare before enabling output (ms)
 * @disable: Time to wait after disable before power off (ms)
 * @unprepare: Minimum delay between unprepare and next prepare (ms)
 */
struct panel_delay {
	unsigned int prepare;
	unsigned int enable;
	unsigned int disable;
	unsigned int unprepare;
};
struct videomode {
	unsigned int clock;	/* pixelclock in Hz */

	unsigned int hactive;
	unsigned int hfront_porch;
	unsigned int hback_porch;
	unsigned int hsync_len;

	unsigned int vactive;
	unsigned int vfront_porch;
	unsigned int vback_porch;
	unsigned int vsync_len;
};
/**
 * struct panel_desc - Panel static characteristics
 * @modes: Fixed display modes array
 * @num_modes: Number of fixed modes
 * @timings: Display timings array
 * @num_timings: Number of timings
 * @bpc: Bits per color channel
 * @size: Physical panel dimensions (mm)
 * @delay: Panel power/control delays
 * @bus_format: Media bus pixel format
 * @bus_flags: Display bus control flags
 */
struct panel_desc {
	const struct videomode *modes;
	unsigned int num_modes;
	const struct display_timing *timings;
	unsigned int num_timings;
	unsigned int bpc;
	struct {
		unsigned int width;
		unsigned int height;
	} size;
	struct panel_delay delay;
	u32 bus_format;
	u32 bus_flags;
};

/**
 * struct rasp_panel_priv - Private data for simple panel
 * @dev: U-Boot device pointer
 * @desc: Panel descriptor pointer
 * @enable_gpio: GPIO for panel enable control
 * @prepared: Panel prepare state flag
 * @enabled: Panel enable state flag
 */
struct rasp_panel_priv {
	struct udevice *dev;
	const struct panel_desc *desc;

	struct gpio_desc enable_gpio;
	bool prepared;
	bool enabled;
};
 #endif	/* _RASP_PANEL_H_ */
 