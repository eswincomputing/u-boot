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
 */

#include "display_logo.h"
#undef ERR_TIMEOUT
#undef ERR_INVAL
#include "common.h"

#include <dm.h>
#include <dm/of_access.h>
#include "dm/ofnode.h"
#include "dm/device-internal.h"

#ifndef USE_HOSTCC
#include <fdt.h>
#include <linux/libfdt.h>
#else
#include "fdt_host.h"
#endif

static int display_logo_probe(void)
{
    int ret;
    struct uclass *uc;
    struct udevice *dev;

    printf("************ display_logo_probe ******\n");

    ret = uclass_get(UCLASS_VIDEO, &uc);
    if (ret)
        return ret;

    ret = uclass_get_device_by_name(UCLASS_DISPLAY, "display_logo",&dev);
    if(ret)
        printf("get %s failed: %d\n", dev->name, ret);

    ret = device_probe(dev);

    if (ret)
        printf("%s - probe failed: %d\n", dev->name, ret);

    return ret;
}

int display_logo_initialize(void)
{
    static int initialized = 0;
    int ret = RET_OK;
    if (initialized)
        return ret;
    initialized = 1;

    ret = display_logo_probe();

    return ret;
}

int initr_display_logo(void)
{
    display_logo_initialize();
    return 0;
}
