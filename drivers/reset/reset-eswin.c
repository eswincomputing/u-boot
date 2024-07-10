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
 * Authors: Huangyifeng <huangifeng@eswincomputing.com>
 */

#include <log.h>
#include <malloc.h>
#include <asm/io.h>
#include <common.h>
#include <dm.h>
#include <dt-bindings/reset/ti-syscon.h>
#include <reset-uclass.h>
#include <linux/bitops.h>

#define SYSCRG_RESET_OFFSET			(0x400)

struct eswin_reset_priv {
	void __iomem *base;
};

static int eswin_reset_deassert(struct reset_ctl *rst)
{
	struct eswin_reset_priv *priv = dev_get_priv(rst->dev);
	u32 val;

	val = readl(priv->base + rst->data);
	val |= rst->id;
	writel(val, priv->base + rst->data);
	return 0;
}

static int eswin_reset_assert(struct reset_ctl *rst)
{
	struct eswin_reset_priv *priv = dev_get_priv(rst->dev);
	u32 val;

	val = readl(priv->base + rst->data);
	val &= ~rst->id;
	writel(val, priv->base + rst->data);

	return 0;
}

static int eswin_reset_free(struct reset_ctl *rst)
{
	return 0;
}

static int eswin_reset_request(struct reset_ctl *rst)
{
	return 0;
}

static int eswin_reset_of_xlate(struct reset_ctl *rst,
			       struct ofnode_phandle_args *args)
{
	if (args->args_count != 2) {
		printf("Invalid args_count: %d\n", args->args_count);
		return -EINVAL;
	}

	/* Use .data field as register offset and .id field as bit shift */
	rst->data = args->args[0] * 4;
	rst->id = args->args[1];
	return 0;
}

static const struct reset_ops eswin_reset_reset_ops = {
	.of_xlate = eswin_reset_of_xlate,
	.request = eswin_reset_request,
	.rfree = eswin_reset_free,
	.rst_assert = eswin_reset_assert,
	.rst_deassert = eswin_reset_deassert,
};

static const struct udevice_id eswin_reset_ids[] = {
	{ .compatible = "eswin,eic770x-reset" },
	{ }
};

static int eswin_reset_probe(struct udevice *dev)
{
	struct eswin_reset_priv *priv = dev_get_priv(dev);

	priv->base = dev_remap_addr(dev) + SYSCRG_RESET_OFFSET;
	if (!priv->base)
		return -ENOMEM;

	return 0;
}

U_BOOT_DRIVER(eswin_reset) = {
	.name = "eswin_reset",
	.id = UCLASS_RESET,
	.of_match = eswin_reset_ids,
	.ops = &eswin_reset_reset_ops,
	.probe = eswin_reset_probe,
	.priv_auto = sizeof(struct eswin_reset_priv),
};
