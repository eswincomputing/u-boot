// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Marek Vasut <marex@denx.de>
 *
 * Based on tc358764.c by
 *  Andrzej Hajda <a.hajda@samsung.com>
 *  Maciej Purski <m.purski@samsung.com>
 *
 * Based on rpi_touchscreen.c by
 *  Eric Anholt <eric@anholt.net>
 */

#include <common.h>
#include <dm.h>
#include <errno.h>
#include <linux/delay.h>
#include <asm-generic/gpio.h>
#include <dm/uclass.h>
#include <dm/uclass-id.h>
#include <media_bus_format.h>
#include <power/regulator.h>
#include <mipi_dsi.h>

#include "eswin_vo_log.h"
/* PPI layer registers */
#define PPI_STARTPPI		0x0104 /* START control bit */
#define PPI_LPTXTIMECNT		0x0114 /* LPTX timing signal */
#define PPI_D0S_ATMR		0x0144
#define PPI_D1S_ATMR		0x0148
#define PPI_D0S_CLRSIPOCOUNT	0x0164 /* Assertion timer for Lane 0 */
#define PPI_D1S_CLRSIPOCOUNT	0x0168 /* Assertion timer for Lane 1 */
#define PPI_START_FUNCTION	1

/* DSI layer registers */
#define DSI_STARTDSI		0x0204 /* START control bit of DSI-TX */
#define DSI_LANEENABLE		0x0210 /* Enables each lane */
#define DSI_RX_START		1

/* LCDC/DPI Host Registers */
#define LCDCTRL			0x0420 /* Video Path Control */
#define LCDCTRL_MSF		BIT(0) /* Magic square in RGB666 */
#define LCDCTRL_VTGEN		BIT(4)/* Use chip clock for timing */
#define LCDCTRL_UNK6		BIT(6) /* Unknown */
#define LCDCTRL_EVTMODE		BIT(5) /* Event mode */
#define LCDCTRL_RGB888		BIT(8) /* RGB888 mode */
#define LCDCTRL_HSPOL		BIT(17) /* Polarity of HSYNC signal */
#define LCDCTRL_DEPOL		BIT(18) /* Polarity of DE signal */
#define LCDCTRL_VSPOL		BIT(19) /* Polarity of VSYNC signal */
#define LCDCTRL_VSDELAY(v)	(((v) & 0xfff) << 20) /* VSYNC delay */

/* SPI Master Registers */
#define SPICMR			0x0450
#define SPITCR			0x0454

/* System Controller Registers */
#define SYSCTRL			0x0464

/* System registers */
#define LPX_PERIOD		3

/* Lane enable PPI and DSI register bits */
#define LANEENABLE_CLEN		BIT(0)
#define LANEENABLE_L0EN		BIT(1)
#define LANEENABLE_L1EN		BIT(2)

/* private */
struct tc358762_priv {
	struct udevice *dev;
	struct mipi_dsi_device *dsi;
	struct udevice *regulator;
	struct gpio_desc reset_gpio;
	bool pre_enabled;
	int error;
	/* display parameters */
	unsigned int hsync_polarity;
	unsigned int vsync_polarity;
};

static void tc358762_write(struct tc358762_priv *ctx, u16 addr, u32 val)
{
	struct mipi_dsi_device *dsi = ctx->dsi;
	int ret;
	u8 data[6];

	if (ctx->error)
		return;

	data[0] = addr & 0xff;
	data[1] = (addr >> 8) & 0xff;
	data[2] = val & 0xff;
	data[3] = (val >> 8) & 0xff;
	data[4] = (val >> 16) & 0xff;
	data[5] = (val >> 24) & 0xff;

	ret = mipi_dsi_generic_write(dsi, data, sizeof(data));
	if (ret < 0)
		ctx->error = ret;
}

/* clear error*/
static int tc358762_clear_error(struct tc358762_priv *ctx)
{
	int ret = ctx->error;
	ctx->error = 0;
	return ret;
}

/* TC358762 hardware init */
static int tc358762_init(struct tc358762_priv *ctx)
{
	unsigned int lcdctrl;

	/* 1. config Single lane */
	tc358762_write(ctx, DSI_LANEENABLE, LANEENABLE_L0EN | LANEENABLE_CLEN);

	/* 2. config PPI */
	tc358762_write(ctx, PPI_D0S_CLRSIPOCOUNT, 5);
	tc358762_write(ctx, PPI_D1S_CLRSIPOCOUNT, 5);
	tc358762_write(ctx, PPI_D0S_ATMR, 0);
	tc358762_write(ctx, PPI_D1S_ATMR, 0);
	tc358762_write(ctx, PPI_LPTXTIMECNT, LPX_PERIOD);

	/* 3. config SPI controller */
	tc358762_write(ctx, SPICMR, 0x00);

	/* 4. config LCD controller */
	lcdctrl = LCDCTRL_VSDELAY(1) | LCDCTRL_RGB888 | LCDCTRL_UNK6 | LCDCTRL_VTGEN;

	/* set sync polarity*/
	if (ctx->hsync_polarity)
		lcdctrl |= LCDCTRL_HSPOL;
	if (ctx->vsync_polarity)
		lcdctrl |= LCDCTRL_VSPOL;

	tc358762_write(ctx, LCDCTRL, lcdctrl);

	/* 5. config system conctl */
	tc358762_write(ctx, SYSCTRL, 0x040f);
	mdelay(100);

	/* 6. start PPI & DSI */
	tc358762_write(ctx, PPI_STARTPPI, PPI_START_FUNCTION);
	tc358762_write(ctx, DSI_STARTDSI, DSI_RX_START);

	mdelay(100);

	return tc358762_clear_error(ctx);
}

/* enable device（pwr+reset+initilize） */
static int tc358762_enable(struct udevice *dev)
{
	struct tc358762_priv *ctx = dev_get_priv(dev);
	int ret;

	/* avoid repeat enable */
	if (ctx->pre_enabled)
		return 0;

	/* 1. enable regulator*/

	/* 2. reset*/
	if (dm_gpio_is_valid(&ctx->reset_gpio)) {
		dm_gpio_set_value(&ctx->reset_gpio, 1);
		udelay(5000); /* delay 5ms */
	}

	/* 3. mark enabled*/
	ctx->pre_enabled = true;

	/* 4. hardware init */
	ret = tc358762_init(ctx);
	if (ret < 0) {
		vo_info("Failed to init TC358762: %d\n", ret);
		ctx->pre_enabled = false;
		return ret;
	}

	return 0;
}

/* disable regulator（reset+power off） */
static int tc358762_disable(struct udevice *dev)
{
	struct tc358762_priv *ctx = dev_get_priv(dev);
	int ret;

	/* avoid repeat disabled */
	if (!ctx->pre_enabled)
		return 0;

	/* 1. set 0 GPIO */
	if (dm_gpio_is_valid(&ctx->reset_gpio))
		dm_gpio_set_value(&ctx->reset_gpio, 0);

	/* 2. disable regulator */
	if (ctx->regulator) {
		ret = regulator_set_enable(ctx->regulator, false);
		if (ret < 0)
			vo_info("Failed to disable regulator: %d\n", ret);
	}

	/* 3. clear enable */
	ctx->pre_enabled = false;

	return 0;
}

static int tc358762_parse_dt(struct udevice *dev, struct tc358762_priv *ctx)
{
	int ret;

	/* 1. get reset gpio */
	ret = gpio_request_by_name(dev, "reset-gpios", 0, &ctx->reset_gpio, GPIOD_ACTIVE_LOW);
	if (ret && ret != -ENOENT) {
		vo_info("Failed to get reset GPIO: %d\n", ret);
		return ret;
	}

	/* 2. get display polarity */
	ofnode_read_u32(dev_ofnode(dev), "hsync-polarity", &ctx->hsync_polarity);
	ofnode_read_u32(dev_ofnode(dev), "vsync-polarity", &ctx->vsync_polarity);

	return 0;
}
/* config Regulator */
static int tc358762_configure_regulators(struct udevice *dev, struct tc358762_priv *ctx)
{
	int ret = 0;
	ret = uclass_get_device_by_name(UCLASS_I2C_GENERIC, "regulator@45", &ctx->regulator);
	vo_info("tc358762_configure_regulators ret = %d\n",ret);
	return ret;
}

static int tc358762_probe(struct udevice *dev)
{
	vo_debug("debug start tc358762_probe\n");
	struct tc358762_priv *ctx = dev_get_priv(dev);
	struct mipi_dsi_device *dsi;
	struct udevice *dsi_host = dev_get_parent(dev);
	int ret;
	/* init private data*/
	ctx->dev = dev;

	ctx->pre_enabled = false;
	ctx->error = 0;
	ctx->hsync_polarity = 0;
	ctx->vsync_polarity = 0;
	dsi = eswin_dsi_host_get_dsi_dev(dev);
	if(!dsi)
	{
		vo_err("NO DSI DEVICE for TC358762!\n");
		return -ENODEV;
	}
	dsi->lanes = 1;					/* Single Lane*/
	dsi->format = MIPI_DSI_FMT_RGB888;		/* RGB888 format */
	dsi->mode_flags = MIPI_DSI_MODE_VIDEO |
			  MIPI_DSI_MODE_VIDEO_SYNC_PULSE |
			  MIPI_DSI_MODE_LPM |
			  MIPI_DSI_MODE_VIDEO_HSE;

	ctx->dsi = dsi;

	/* 2. parse device tree */
	ret = tc358762_parse_dt(dev, ctx);

	if (ret < 0)
		return ret;

	/* 3. config regulator */
	ret = tc358762_configure_regulators(dev, ctx);
	if (ret < 0 && ret != -ENOENT) {
		vo_err("Failed to configure regulator: %d\n", ret);
		return ret;
	}

	/* 4. attach to dsi */
	ret = eswin_dw_mipi_dsi_child_pre_probe(dev);

	tc358762_enable(dev);
	vo_debug("TC358762 DSI/DPI Bridge probed successfully\n");
	return 0;
}

/* Remove func*/
static int tc358762_remove(struct udevice *dev)
{
	struct udevice *dsi_dev = dev_get_parent(dev);
	struct mipi_dsi_device *dsi = dev_get_priv(dsi_dev);

	tc358762_disable(dev);
	mipi_dsi_detach(dsi);
	return 0;
}

static const struct udevice_id tc358762_of_match[] = {
	{ .compatible = "toshiba,tc358762" },
	{ }
};

U_BOOT_DRIVER(tc358762) = {
	.name		= "tc358762",
	.id		= UCLASS_VIDEO_BRIDGE,
	.of_match	= tc358762_of_match,
	.priv_auto	= sizeof(struct tc358762_priv),
	.probe = tc358762_probe,
	.remove = tc358762_remove,
};
