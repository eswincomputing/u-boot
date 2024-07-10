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

#include <config.h>
#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <asm/unaligned.h>
#include <linux/list.h>
#include <dm/device.h>
#include <dm.h>

#include "eswin_display.h"
#include "eswin_crtc.h"
#include "eswin_connector.h"

static const struct eswin_crtc dc8000_dc_data = {
	.funcs = &eswin_dc_funcs,
	.data = &dc8000_data_val,
};

static const struct udevice_id eswin_dc_ids[] = {
   {
		.compatible = "eswin,dc8000-osd",
		.data = (ulong)&dc8000_dc_data,
	}, { }
};

static int eswin_dc_probe(struct udevice *dev)
{
    return 0;
}

static int eswin_dc_bind(struct udevice *dev)
{
	return 0;
}

U_BOOT_DRIVER(eswin_dc) = {
	.name	= "eswin_dc",
	.id	= UCLASS_VIDEO_OSD,
	.of_match = eswin_dc_ids,
	.bind	= eswin_dc_bind,
	.probe	= eswin_dc_probe,
};

UCLASS_DRIVER(eswin_crtc) = {
	.id		= UCLASS_VIDEO_OSD,
	.name		= "CRTC",
};
