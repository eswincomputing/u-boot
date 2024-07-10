// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN GPIO Device Driver
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
 * Authors: Jiamiao Hu <hujiamiao@eswincomputing.com>
 */


#include <common.h>
#include <log.h>
#include <malloc.h>
#include <asm/gpio.h>
#include <asm/io.h>
#include <dm.h>
#include <dm/device-internal.h>
#include <dm/device_compat.h>
#include <dm/devres.h>
#include <dm/lists.h>
#include <dm/root.h>
#include <errno.h>
#include <reset.h>
#include <linux/bitops.h>

#define GPIO_SWPORT_DR(p)	(0x00 + (p) * 0xc)
#define GPIO_SWPORT_DDR(p)	(0x04 + (p) * 0xc)
#define GPIO_INTEN		0x30
#define GPIO_INTMASK		0x34
#define GPIO_INTTYPE_LEVEL	0x38
#define GPIO_INT_POLARITY	0x3c
#define GPIO_INTSTATUS		0x40
#define GPIO_PORTA_DEBOUNCE	0x48
#define GPIO_PORTA_EOI		0x4c
#define GPIO_EXT_PORT(p)	(0x50 + (p) * 4)

struct gpio_eswin_priv {
	struct reset_ctl_bulk	resets;
};

struct gpio_eswin_plat {
	const char	*name;
	int		bank;
	int		pins;
	void __iomem	*base;
};

static int eswin_gpio_direction_input(struct udevice *dev, unsigned pin)
{
	struct gpio_eswin_plat *plat = dev_get_plat(dev);

	clrbits_le32(plat->base + GPIO_SWPORT_DDR(plat->bank), 1 << pin);
	return 0;
}

static int eswin_gpio_direction_output(struct udevice *dev, unsigned pin,
				     int val)
{
	struct gpio_eswin_plat *plat = dev_get_plat(dev);

	setbits_le32(plat->base + GPIO_SWPORT_DDR(plat->bank), 1 << pin);

	if (val)
		setbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);
	else
		clrbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);

	return 0;
}

static int eswin_gpio_set_value(struct udevice *dev, unsigned pin, int val)
{
	struct gpio_eswin_plat *plat = dev_get_plat(dev);
	if (val)
		setbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);
	else
		clrbits_le32(plat->base + GPIO_SWPORT_DR(plat->bank), 1 << pin);

	return 0;
}

static int eswin_gpio_get_function(struct udevice *dev, unsigned offset)
{
	struct gpio_eswin_plat *plat = dev_get_plat(dev);
	u32 gpio;

	gpio = readl(plat->base + GPIO_SWPORT_DDR(plat->bank));

	if (gpio & BIT(offset))
		return GPIOF_OUTPUT;
	else
		return GPIOF_INPUT;
}

static int eswin_gpio_get_value(struct udevice *dev, unsigned pin)
{
	struct gpio_eswin_plat *plat = dev_get_plat(dev);
	u32 value;

	if (eswin_gpio_get_function(dev, pin) == GPIOF_OUTPUT)
    {
		value = readl(plat->base + GPIO_SWPORT_DR(plat->bank));
    }
	else
    {
		value = readl(plat->base + GPIO_EXT_PORT(plat->bank));
    }
	return !!(value & BIT(pin));
}

static const struct dm_gpio_ops gpio_eswin_ops = {
	.direction_input	= eswin_gpio_direction_input,
	.direction_output	= eswin_gpio_direction_output,
	.get_value		= eswin_gpio_get_value,
	.set_value		= eswin_gpio_set_value,
	.get_function		= eswin_gpio_get_function,
};

static int gpio_eswin_reset(struct udevice *dev)
{
	int ret;
	struct gpio_eswin_priv *priv = dev_get_priv(dev);

	ret = reset_get_bulk(dev, &priv->resets);
	if (ret) {
		/* Return 0 if error due to !CONFIG_DM_RESET and reset
		 * DT property is not present.
		 */
		if (ret == -ENOENT || ret == -ENOTSUPP)
			return 0;

		dev_warn(dev, "Can't get reset: %d\n", ret);
		return ret;
	}

	ret = reset_deassert_bulk(&priv->resets);
	if (ret) {
		reset_release_bulk(&priv->resets);
		dev_err(dev, "Failed to reset: %d\n", ret);
		return ret;
	}

	return 0;
}

static int gpio_eswin_probe(struct udevice *dev)
{
	struct gpio_dev_priv *priv = dev_get_uclass_priv(dev);
	struct gpio_eswin_plat *plat = dev_get_plat(dev);

	if (!plat) {
		/* Reset on parent device only */
		return gpio_eswin_reset(dev);
	}

	priv->gpio_count = plat->pins;
	priv->bank_name = plat->name;

	return 0;
}

static int gpio_eswin_bind(struct udevice *dev)
{
	struct gpio_eswin_plat *plat = dev_get_plat(dev);
	struct udevice *subdev;
	fdt_addr_t base;
	int ret, bank = 0;
	ofnode node;
	/* If this is a child device, there is nothing to do here */
	if (plat)
		return 0;

	base = dev_read_addr(dev);
	if (base == FDT_ADDR_T_NONE) {
		debug("Can't get the GPIO register base address\n");
		return -ENXIO;
	}

	for (node = dev_read_first_subnode(dev); ofnode_valid(node);
	     node = dev_read_next_subnode(node)) {
		if (!ofnode_read_bool(node, "gpio-controller"))
			continue;

		plat = devm_kcalloc(dev, 1, sizeof(*plat), GFP_KERNEL);
		if (!plat)
			return -ENOMEM;

		plat->base = (void *)base;
		plat->bank = bank;
		plat->pins = ofnode_read_u32_default(node, "ngpios", 0);

		if (ofnode_read_string_index(node, "bank-name", 0,
					     &plat->name)) {
			/*
			 * Fall back to node name. This means accessing pins
			 * via bank name won't work.
			 */
			char name[32]={0};

			name[0] = 'A' + plat->bank;
			plat->name = strdup(name);
			if (!plat->name) {
				kfree(plat);
				return -ENOMEM;
			}
		}

		ret = device_bind(dev, dev->driver, plat->name, plat, node,
				  &subdev);
		if (ret)
			return ret;

		bank++;
	}

	return 0;
}

static int gpio_eswin_remove(struct udevice *dev)
{
	struct gpio_eswin_plat *plat = dev_get_plat(dev);
	struct gpio_eswin_priv *priv = dev_get_priv(dev);

	if (!plat && priv)
		return reset_release_bulk(&priv->resets);

	return 0;
}

static const struct udevice_id gpio_eswin_ids[] = {
	{ .compatible = "eswin,gpio-bank" },
	{ }
};

U_BOOT_DRIVER(gpio_eswin) = {
	.name		= "gpio_eswin",
	.id		= UCLASS_GPIO,
	.of_match	= gpio_eswin_ids,
	.ops		= &gpio_eswin_ops,
	.bind		= gpio_eswin_bind,
	.probe		= gpio_eswin_probe,
	.remove		= gpio_eswin_remove,
	.priv_auto	  = sizeof(struct gpio_eswin_priv),
};