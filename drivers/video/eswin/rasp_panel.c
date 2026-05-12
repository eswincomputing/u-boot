// SPDX-License-Identifier: GPL-2.0+
/*
 * U-Boot Driver for Simple Display Panels
 *
 * Based on Linux kernel panel-simple driver
 * Copyright (C) 2013 NVIDIA Corporation
 * Copyright (C) 2026 U-Boot Port
 */

 #include <common.h>
 #include <dm.h>
 #include <errno.h>
 #include <asm-generic/gpio.h>
 #include <log.h>
 #include <misc.h>
 #include <fdtdec.h>
 #include <cpu_func.h>
 #include <fdt_support.h>
 #include <dm/device_compat.h>
 #include <linux/bug.h>
 #include <linux/kernel.h>
 #include <linux/delay.h>
 
 #include "rasp_panel.h"
 #include "eswin_panel.h"
 #include "eswin_vo_log.h"


/* -------------------------- Panel Descriptors -------------------------- */
/* Example: Raspberry Pi 7-inch DSI Panel */
static const struct videomode raspberrypi_7inch_mode = {
	.clock = 30000,
	.hactive = 800,
	.hfront_porch = 131,
	.hsync_len = 2,
	.hback_porch = 45,
	.vactive = 480,
	.vfront_porch = 7,
	.vsync_len = 2,
	.vback_porch = 22,
};

static const struct panel_desc raspberrypi_7inch = {
	.modes = &raspberrypi_7inch_mode,
	.num_modes = 1,
	.bpc = 8,
	.size = {
		.width = 154,
		.height = 86,
	},
	.bus_format = 0,
};

/* Helper macro to get private data from panel device */
#define to_rasp_panel_priv(panel_dev) \
	((struct rasp_panel_priv *)dev_get_priv(panel_dev))

static void rasp_panel_init(struct eswin_panel *panel){

	}
/**
 * rasp_panel_prepare - Power up and prepare the panel
 * @dev: U-Boot panel device
 *
 * Powers on the panel, asserts enable GPIO, and waits for prepare delay.
 * Returns 0 on success, negative error code on failure.
 */

static void rasp_panel_prepare(struct eswin_panel *panel)
{
	struct udevice *dev = panel->dev;
	struct rasp_panel_priv *priv = to_rasp_panel_priv(dev);
	const struct panel_delay *delay = &priv->desc->delay;

	if (priv->prepared)
		return;

	/* Assert panel enable GPIO */
	dm_gpio_set_value(&priv->enable_gpio, 1);

	/* Wait for panel to stabilize */
	if (delay->prepare)
		mdelay(delay->prepare);

	priv->prepared = true;
	vo_debug("Rasp Panel prepared\n");

	return;
}

/**
 * rasp_panel_unprepare - Power down the panel
 * @dev: U-Boot panel device
 *
 * Deasserts enable GPIO and powers off the panel.
 * Returns 0 on success, negative error code on failure.
 */
static void rasp_panel_unprepare(struct eswin_panel *panel)
{
	struct udevice *dev = panel->dev;
	struct rasp_panel_priv *priv = to_rasp_panel_priv(dev);

	if (!priv->prepared)
		return;

	/* Deassert panel enable GPIO */
	dm_gpio_set_value(&priv->enable_gpio, 0);

	priv->prepared = false;
	priv->enabled = false;
	vo_debug("Panel unprepared\n");

	return;
}

/**
 * rasp_panel_enable - Enable panel video output
 * @dev: U-Boot panel device
 *
 * Enables the panel after prepare, waits for enable delay.
 * Returns 0 on success, negative error code on failure.
 */
static void rasp_panel_enable(struct eswin_panel *panel)
{
	struct udevice *dev = panel->dev;
	struct rasp_panel_priv *priv = to_rasp_panel_priv(dev);
	const struct panel_delay *delay = &priv->desc->delay;

	if (priv->enabled)
		return;

	if (!priv->prepared) {
		vo_err("Cannot enable: panel not prepared\n");
		return;
	}

	/* Wait for panel to accept video data */
	if (delay->enable)
		mdelay(delay->enable);

	priv->enabled = true;
	vo_info("Raspberry Panel enabled\n");

	return;
}

/**
 * rasp_panel_disable - Disable panel video output
 * @dev: U-Boot panel device
 *
 * Disables video output and waits for disable delay.
 * Returns 0 on success, negative error code on failure.
 */
static void rasp_panel_disable(struct eswin_panel *panel)
{
	struct udevice *dev = panel->dev;
	struct rasp_panel_priv *priv = to_rasp_panel_priv(dev);
	const struct panel_delay *delay = &priv->desc->delay;

	if (!priv->enabled)
		return;

	/* Wait for panel to turn off */
	if (delay->disable)
		mdelay(delay->disable);

	priv->enabled = false;
	vo_debug("Panel disabled\n");

	return;
}

/**
 * rasp_panel_get_display_mode - Get panel display mode
 * @dev: U-Boot panel device
 * @mode: Output videomode structure
 *
 * Reads fixed mode or display timing from panel descriptor.
 * Returns 0 on success, negative error code on failure.
 */
static int rasp_panel_get_display_mode(struct udevice *dev)
{
	struct rasp_panel_priv *priv = to_rasp_panel_priv(dev);
	const struct panel_desc *desc = priv->desc;
	struct videomode *mode = &raspberrypi_7inch_mode;
	if (!mode)
		return -1;

	/* Use fixed display mode if available */
	if (desc->num_modes > 0) {
		*mode = desc->modes[0];
		return 0;
	}
	return 0;
}

/* U-Boot panel operations structure */
static const struct eswin_panel_funcs rasp_panel_ops = {
	.init = rasp_panel_init,
	.prepare = rasp_panel_prepare,
	.unprepare = rasp_panel_unprepare,
	.enable = rasp_panel_enable,
	.disable = rasp_panel_disable,
	.get_modes = rasp_panel_get_display_mode,
};


/* -------------------------- DT Match Table -------------------------- */
static const struct udevice_id rasp_panel_ids[] = {
	{
		.compatible = "raspberrypi,7inch-dsi",
	},
	{ /* Sentinel */ }
};

/**
 * rasp_panel_probe - Probe simple panel device
 * @dev: U-Boot device
 *
 * Initializes GPIO, reads panel descriptor from match data,
 * initializes private data.
 * Returns 0 on success, negative error code on failure.
 */
static int rasp_panel_probe(struct udevice *dev)
{
	struct eswin_panel *panel;
	struct rasp_panel_priv *priv;
	const struct panel_desc *desc;
	int ret;

	/* Get private data (auto allocated) */
	priv = dev_get_priv(dev);
	if (!priv)
		return -ENOMEM;

	/* Get panel descriptor from device match data */
	desc = &raspberrypi_7inch;
	if (!desc) {
		vo_err("No panel descriptor found\n");
		return -ENODEV;
	}

	priv->dev = dev;
	priv->desc = desc;
	priv->prepared = false;
	priv->enabled = false;

	/* Request enable GPIO (optional) */
	ret = gpio_request_by_name(dev, "enable-gpios", 0,
				   &priv->enable_gpio, GPIOD_IS_OUT);
	if (ret && ret != -ENOENT) {
		vo_err("Failed to request enable GPIO: %d\n", ret);
		return ret;
	}
    panel = calloc(1, sizeof(*panel));
    if (!panel) return -ENOMEM;

    dev->driver_data = (ulong)panel;
    panel->dev = dev;
    panel->bus_format = desc->bus_format;
    panel->bpc = desc->bpc;
    panel->funcs = &rasp_panel_ops;
	panel->data = priv;
	vo_debug("Simple panel probed successfully\n");
	return 0;
}

/* U-Boot driver structure */
U_BOOT_DRIVER(rasp_panel) = {
	.name = "rasp_panel",
	.id = UCLASS_PANEL,
	.of_match = rasp_panel_ids,
	.probe = rasp_panel_probe,
	.ops = &rasp_panel_ops,
	.priv_auto = sizeof(struct rasp_panel_priv),
};