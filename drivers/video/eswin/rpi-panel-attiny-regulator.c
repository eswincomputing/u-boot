// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2020 Marek Vasut <marex@denx.de>
 *
 * Based on rpi_touchscreen.c by Eric Anholt <eric@anholt.net>
 */

 #include <common.h>
 #include <dm.h>
 #include <i2c.h>
 #include <regmap.h>
 #include <backlight.h>
 #include <linux/delay.h>
 #include <asm-generic/gpio.h>
 #include <dm/uclass.h>
 #include <dm/uclass-id.h>
 #include <media_bus_format.h>
 #include <power/regulator.h>
 #include <dm/device_compat.h>

 #include "eswin_vo_log.h"
/* I2C registers of the Atmel microcontroller. */
#define REG_ID		0x80
#define REG_PORTA	0x81
#define REG_PORTB	0x82
#define REG_PORTC	0x83
#define REG_POWERON	0x85
#define REG_PWM		0x86
#define REG_ADDR_L	0x8c
#define REG_ADDR_H	0x8d
#define REG_WRITE_DATA_H	0x90
#define REG_WRITE_DATA_L	0x91

#define PA_LCD_LR		BIT(2)

#define PB_LCD_VCC_N		BIT(1)
#define PB_LCD_MAIN		BIT(7)

#define PC_LED_EN		BIT(0)
#define PC_RST_TP_N		BIT(1)
#define PC_RST_LCD_N		BIT(2)
#define PC_RST_BRIDGE_N	BIT(3)

enum gpio_signals {
	RST_BRIDGE_N,
	RST_TP_N,
	NUM_GPIO
};

struct attiny_lcd_priv {
	struct udevice *dev;
	u8 port_states[3];
};

static int attiny_set_port_state(struct attiny_lcd_priv *priv, u8 reg, u8 val)
{
	int ret;

	ret = dm_i2c_reg_write(priv->dev, reg, val);
	if (!ret)
		priv->port_states[reg - REG_PORTA] = val;

	return ret;
}

static u8 attiny_get_port_state(struct attiny_lcd_priv *priv, u8 reg)
{
	return priv->port_states[reg - REG_PORTA];
}

/* enable LCD power + backlight + reset */
int attiny_lcd_power_enable(struct udevice *dev)
{
	struct attiny_lcd_priv *priv = dev_get_priv(dev);
	int ret;

	/* Ensure bridge, and tp stay in reset */
	ret = attiny_set_port_state(priv, REG_PORTC, 0);
	if (ret) return ret;
	udelay(8000);

	/* Default to the same orientation as the closed source
	 * firmware used for the panel.  Runtime rotation
	 * configuration will be supported using VC4's plane
	 * orientation bits.
	 */
	ret = attiny_set_port_state(priv, REG_PORTA, PA_LCD_LR);
	udelay(8000);

	/* Main regulator on, and power to the panel (LCD_VCC_N) */
	ret = attiny_set_port_state(priv, REG_PORTB, PB_LCD_MAIN);
	udelay(8000);

	/* Bring controllers out of reset */
	ret = attiny_set_port_state(priv, REG_PORTC, PC_LED_EN);
	mdelay(80);

	/* set PWM */
	dm_i2c_reg_write(dev, REG_PWM, 0xff);
	vo_debug("attiny_lcd_power_enable\n");
	return 0;
}

/* TC358762 reset */
void attiny_bridge_reset(struct udevice *dev)
{
	struct attiny_lcd_priv *priv = dev_get_priv(dev);
	u8 val;

	val = attiny_get_port_state(priv, REG_PORTC);
	val |= PC_RST_BRIDGE_N | PC_RST_LCD_N;
	attiny_set_port_state(priv, REG_PORTC, val);

	udelay(6000);
	dm_i2c_reg_write(dev, REG_ADDR_H, 0x04);
	udelay(6000);
	dm_i2c_reg_write(dev, REG_ADDR_L, 0x7c);
	udelay(6000);
	dm_i2c_reg_write(dev, REG_WRITE_DATA_H, 0x00);
	udelay(6000);
	dm_i2c_reg_write(dev, REG_WRITE_DATA_L, 0x00);
	mdelay(100);
}

/* Probe */
static int attiny_i2c_probe(struct udevice *dev)
{
	vo_debug("debug start attiny_i2c_probe\n");
	struct attiny_lcd_priv *priv = dev_get_priv(dev);
	unsigned int id = 0;
	int ret, i;

	priv->dev = dev;

	/* read ID */
	for (i = 0; i < 5; i++) {
		ret = dm_i2c_reg_read(dev, REG_ID);
		if (ret >= 0) break;
		mdelay(10);
	}
	if (ret < 0) {
		vo_err("read ID failed\n");
		return ret;
	}
	id = ret;
	if (id != 0xde && id != 0xc3) {
		vo_err("invalid ID 0x%02x\n", id);
		return -ENODEV;
	}
	vo_info("ATTINY probed, ID=0x%02x\n", id);
	/* init pwr on*/
	dm_i2c_reg_write(dev, REG_POWERON, 0);
	mdelay(30);
	dm_i2c_reg_write(dev, REG_PWM, 0);
	/* power on */
	attiny_lcd_power_enable(dev);

	return 0;
}

static const struct udevice_id attiny_ids[] = {
	{ .compatible = "raspberrypi,7inch-touchscreen-panel-regulator" },
	{ }
};

U_BOOT_DRIVER(rpi_touchscreen_attiny) = {
	.name		= "rpi_touchscreen",
	.id			= UCLASS_I2C_GENERIC,
	.of_match	= attiny_ids,
	.probe		= attiny_i2c_probe,
	.priv_auto	= sizeof(struct attiny_lcd_priv),
};