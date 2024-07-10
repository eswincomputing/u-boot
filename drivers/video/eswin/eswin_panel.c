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

#include <drm/dw_mipi_dsi.h>

#include <config.h>
#include <common.h>
#include <errno.h>
#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <video.h>
#include <backlight.h>
#include <asm/gpio.h>
#include <dm/device.h>
#include <dm/read.h>
#include <dm/uclass.h>
#include <dm/uclass-id.h>
#include <media_bus_format.h>
#include <power/regulator.h>
#include <linux/delay.h>
#include "eswin_display.h"
#include "eswin_crtc.h"
#include "eswin_connector.h"
#include "eswin_panel.h"

#define __HAPS
#define __ST7796

struct eswin_cmd_header {
    u8 data_type;
    u8 delay_ms;
    u8 payload_length;
} __packed;

struct eswin_cmd_desc {
    struct eswin_cmd_header header;
    const u8 *payload;
};

struct eswin_panel_cmds {
    struct eswin_cmd_desc *cmds;
    int cmd_cnt;
};

struct eswin_panel_plat {
    bool power_invert;
    u32 bus_format;
    unsigned int bpc;

    struct {
        unsigned int prepare;
        unsigned int unprepare;
        unsigned int enable;
        unsigned int disable;
        unsigned int reset;
        unsigned int init;
    } delay;

    struct eswin_panel_cmds *on_cmds;
    struct eswin_panel_cmds *off_cmds;
};

struct eswin_panel_priv {
    bool prepared;
    bool enabled;
    struct udevice *power_supply;
    struct udevice *backlight;
    struct gpio_desc enable_gpio;
    struct gpio_desc reset_gpio;

    int cmd_type;
    struct gpio_desc spi_sdi_gpio;
    struct gpio_desc spi_scl_gpio;
    struct gpio_desc spi_cs_gpio;
};

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static int lcm_dcs_write(struct mipi_dsi_device *dsi, const void *data, size_t len) {
    int ret;
    char *addr;
    addr = (char *)data;
    if ((int)*addr < 0xB0)
        ret = mipi_dsi_dcs_write_buffer(dsi, data, len);
    else
        ret = mipi_dsi_generic_write(dsi, data, len);
    if (ret < 0) printf("%s %d:MIPI DSI write error\n", __func__, __LINE__);
    return ret;
}

#define lcm_dcs_write_seq_static(dsi, seq...) \
    ({                                        \
        static const u8 d[] = {seq};          \
        lcm_dcs_write(dsi, d, ARRAY_SIZE(d)); \
    })

static void init_1080p_4lanes(struct mipi_dsi_device *dsi) {
    dsi->mode_flags |= MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_LPM | MIPI_DSI_MODE_VIDEO_BURST;
    lcm_dcs_write_seq_static(dsi, 0xB0, 0x98, 0x85, 0x0A);
    lcm_dcs_write_seq_static(dsi, 0xC1, 0x00, 0x00, 0x00);  // 01反扫  00正扫
    lcm_dcs_write_seq_static(dsi, 0xC4, 0x70, 0x19, 0x23, 0x00, 0x0F, 0x0F, 0x00);
    lcm_dcs_write_seq_static(dsi, 0xD0, 0x33, 0x05, 0x21, 0xE7, 0x62, 0x00, 0x88, 0xA0, 0xA0, 0x03, 0x02, 0x80, 0xD0,
                             0x1B,
                             0x10);  // VGH dual mode,      VGL single mode,      VGH=12V,      VGL=-12V
    lcm_dcs_write_seq_static(dsi, 0xD2, 0x13, 0x13, 0xEA, 0x22);
    lcm_dcs_write_seq_static(dsi, 0xD1, 0x09, 0x09, 0xc2);  ////4003 & 4003B EN
    lcm_dcs_write_seq_static(dsi, 0xD3, 0x44, 0x33, 0x05, 0x03, 0x4A, 0x4A, 0x33, 0x17, 0x22, 0x43,
                             0x6E);  // set GVDDP=4.1V,      GVDDN=-4.1V,      VGHO=12V,      VGLO=-11V
    lcm_dcs_write_seq_static(dsi, 0xD5, 0x8B, 0x00, 0x00, 0x00, 0x01, 0x7D, 0x01, 0x7D, 0x01, 0x7D, 0x00, 0x00, 0x00,
                             0x00);  // set VCOM
    lcm_dcs_write_seq_static(dsi, 0xD6, 0x00, 0x00, 0x08, 0x17, 0x23, 0x65, 0x77, 0x44, 0x87, 0x00, 0x00,
                             0x09);  // P12_D[3] for sleep in current reduce setting
    lcm_dcs_write_seq_static(dsi, 0xEC, 0x76, 0x1E, 0x32, 0x00, 0x46, 0x00, 0x00);
    lcm_dcs_write_seq_static(
        dsi, 0xC7, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x2D, 0x00, 0x43, 0x00, 0x58, 0x00, 0x6C, 0x00, 0x75, 0x00, 0x8C, 0x00,
        0x9A, 0x00, 0xCA, 0x00, 0xF1, 0x01, 0x2F, 0x01, 0x5F, 0x01, 0xAE, 0x01, 0xEC, 0x01, 0xEE, 0x02, 0x25, 0x02,
        0x62, 0x02, 0x8A, 0x02, 0xC4, 0x02, 0xEA, 0x03, 0x1F, 0x03, 0x33, 0x03, 0x3E, 0x03, 0x59, 0x03, 0x70, 0x03,
        0x88, 0x03, 0xB4, 0x03, 0xC8, 0x03, 0xE8, 0x00, 0x00, 0x00, 0x0D, 0x00, 0x2D, 0x00, 0x43, 0x00, 0x58, 0x00,
        0x6C, 0x00, 0x75, 0x00, 0x8C, 0x00, 0x9A, 0x00, 0xCA, 0x00, 0xF1, 0x01, 0x2F, 0x01, 0x5F, 0x01, 0xAE, 0x01,
        0xEC, 0x01, 0xEE, 0x02, 0x25, 0x02, 0x62, 0x02, 0x8A, 0x02, 0xC4, 0x02, 0xEA, 0x03, 0x1F, 0x03, 0x33, 0x03,
        0x3E, 0x03, 0x59, 0x03, 0x70, 0x03, 0x88, 0x03, 0xB4, 0x03, 0xC8, 0x03, 0xE8, 0x01, 0x01);
    lcm_dcs_write_seq_static(dsi, 0xE5, 0x6D, 0x92, 0x80, 0x34, 0x76, 0x12, 0x12, 0x00, 0x00, 0x36, 0x36, 0x24, 0x24,
                             0x26, 0xF6, 0xF6, 0xF6, 0xF6, 0xF6, 0xF6, 0x6D, 0x9B, 0x89, 0x34, 0x76, 0x1B, 0x1B, 0x09,
                             0x09, 0x3F, 0x3F, 0x2D, 0x2D, 0x26, 0xF6, 0xF6, 0xF6, 0xF6, 0xF6, 0xF6, 0x04, 0x40, 0x90,
                             0x00, 0xD6, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x44, 0x00, 0xD6, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x07);
    lcm_dcs_write_seq_static(dsi, 0xEA, 0x70, 0x00, 0x03, 0x03, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0x0b, 0x00, 0x00, 0x00, 0x89, 0x8a, 0x05, 0x05, 0x22, 0x0a, 0x0a, 0x0b, 0x00, 0x08,
                             0x00, 0x30, 0x01, 0x09, 0x00, 0x40, 0x80, 0xc0, 0x00, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89,
                             0xab, 0xcd, 0xef, 0xDD, 0x22, 0x22, 0xCC, 0xDD, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                             0x00, 0xCC, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33);
    lcm_dcs_write_seq_static(dsi, 0xEE, 0x22, 0x10, 0x02, 0x02, 0x0F, 0x40, 0x00, 0x07, 0x00, 0x04, 0x00, 0x00, 0x00,
                             0xB9, 0x77, 0x00, 0x55, 0x05, 0x00);  // 4003
    lcm_dcs_write_seq_static(dsi, 0xEB, 0xa3, 0xcf, 0x73, 0x18, 0x54, 0x55, 0x55, 0x55, 0x55, 0x00, 0x55, 0x55, 0x55,
                             0x55, 0x55, 0x25, 0x0D, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0x55, 0x55, 0x55, 0x32,
                             0x3E, 0x55, 0x43, 0x55);
    printf("cabc write\n");
    lcm_dcs_write_seq_static(dsi, 0x51, 0x0F, 0xFF);  // brightness [0x00-> 0xFF]
    lcm_dcs_write_seq_static(dsi, 0x53, (1 << 2) | (1 << 5));
    udelay(200000);

    lcm_dcs_write_seq_static(dsi, 0x11, 0x00);
    // Delay(200);
    udelay(200000);
    lcm_dcs_write_seq_static(dsi, 0x29, 0x00);
}

static inline int get_panel_cmd_type(const char *s) {
    if (!s) return -EINVAL;

    if (strncmp(s, "spi", 3) == 0)
        return CMD_TYPE_SPI;
    else if (strncmp(s, "mcu", 3) == 0)
        return CMD_TYPE_MCU;

    return CMD_TYPE_DEFAULT;
}

static int eswin_panel_parse_cmds(const u8 *data, int length, struct eswin_panel_cmds *pcmds) {
    int len;
    const u8 *buf;
    const struct eswin_cmd_header *header;
    int i, cnt = 0;

    /* scan commands */
    cnt = 0;
    buf = data;
    len = length;
    while (len > sizeof(*header)) {
        header = (const struct eswin_cmd_header *)buf;
        buf += sizeof(*header) + header->payload_length;
        len -= sizeof(*header) + header->payload_length;
        cnt++;
    }

    pcmds->cmds = calloc(cnt, sizeof(struct eswin_cmd_desc));
    if (!pcmds->cmds) return -ENOMEM;

    pcmds->cmd_cnt = cnt;

    buf = data;
    len = length;
    for (i = 0; i < cnt; i++) {
        struct eswin_cmd_desc *desc = &pcmds->cmds[i];

        header = (const struct eswin_cmd_header *)buf;
        length -= sizeof(*header);
        buf += sizeof(*header);
        desc->header.data_type = header->data_type;
        desc->header.delay_ms = header->delay_ms;
        desc->header.payload_length = header->payload_length;
        desc->payload = buf;
        buf += header->payload_length;
        length -= header->payload_length;
    }

    return 0;
}

static void eswin_panel_write_spi_cmds(struct eswin_panel_priv *priv, u8 type, int value) {
    int i;

    dm_gpio_set_value(&priv->spi_cs_gpio, 0);

    if (type == 0)
        value &= (~(1 << 8));
    else
        value |= (1 << 8);

    for (i = 0; i < 9; i++) {
        if (value & 0x100)
            dm_gpio_set_value(&priv->spi_sdi_gpio, 1);
        else
            dm_gpio_set_value(&priv->spi_sdi_gpio, 0);

        dm_gpio_set_value(&priv->spi_scl_gpio, 0);
        udelay(10);
        dm_gpio_set_value(&priv->spi_scl_gpio, 1);
        value <<= 1;
        udelay(10);
    }

    dm_gpio_set_value(&priv->spi_cs_gpio, 1);
}

static int eswin_panel_send_mcu_cmds(struct display_state *state, struct eswin_panel_cmds *cmds) {
    int i;

    if (!cmds) return -EINVAL;

    display_send_mcu_cmd(state, MCU_SETBYPASS, 1);
    for (i = 0; i < cmds->cmd_cnt; i++) {
        struct eswin_cmd_desc *desc = &cmds->cmds[i];
        int value = 0;

        value = desc->payload[0];
        display_send_mcu_cmd(state, desc->header.data_type, value);

        if (desc->header.delay_ms) mdelay(desc->header.delay_ms);
    }
    display_send_mcu_cmd(state, MCU_SETBYPASS, 0);

    return 0;
}

static int eswin_panel_send_spi_cmds(struct display_state *state, struct eswin_panel_cmds *cmds) {
    struct eswin_panel *panel = state_get_panel(state);
    struct eswin_panel_priv *priv = dev_get_priv(panel->dev);
    int i;

    if (!cmds) return -EINVAL;

    for (i = 0; i < cmds->cmd_cnt; i++) {
        struct eswin_cmd_desc *desc = &cmds->cmds[i];
        int value = 0;

        if (desc->header.payload_length == 2)
            value = (desc->payload[0] << 8) | desc->payload[1];
        else
            value = desc->payload[0];
        eswin_panel_write_spi_cmds(priv, desc->header.data_type, value);

        if (desc->header.delay_ms) mdelay(desc->header.delay_ms);
    }

    return 0;
}

static int eswin_panel_send_dsi_cmds(struct mipi_dsi_device *dsi, struct eswin_panel_cmds *cmds) {
    int i, ret;

    if (!cmds) return -EINVAL;

    for (i = 0; i < cmds->cmd_cnt; i++) {
        struct eswin_cmd_desc *desc = &cmds->cmds[i];
        const struct eswin_cmd_header *header = &desc->header;

        switch (header->data_type) {
            case MIPI_DSI_GENERIC_SHORT_WRITE_0_PARAM:
            case MIPI_DSI_GENERIC_SHORT_WRITE_1_PARAM:
            case MIPI_DSI_GENERIC_SHORT_WRITE_2_PARAM:
            case MIPI_DSI_GENERIC_LONG_WRITE:
                ret = mipi_dsi_generic_write(dsi, desc->payload, header->payload_length);
                break;
            case MIPI_DSI_DCS_SHORT_WRITE:
            case MIPI_DSI_DCS_SHORT_WRITE_PARAM:
            case MIPI_DSI_DCS_LONG_WRITE:
                ret = mipi_dsi_dcs_write_buffer(dsi, desc->payload, header->payload_length);
                break;
            default:
                printf("unsupport command data type: %d\n", header->data_type);
                return -EINVAL;
        }

        if (ret < 0) {
            printf("failed to write cmd%d: %d\n", i, ret);
            return ret;
        }

        if (header->delay_ms) mdelay(header->delay_ms);
    }

    return 0;
}

static void panel_simple_prepare(struct eswin_panel *panel) {
    printf("********[%s] enter*********\n", __func__);
    struct eswin_panel_priv *priv = dev_get_priv(panel->dev);
    struct mipi_dsi_device *dsi = dev_get_parent_plat(panel->dev);
#if !defined(__HAPS)
    struct eswin_panel_plat *plat = dev_get_plat(panel->dev);
    int ret;
#endif
    if (priv->prepared) return;
    init_1080p_4lanes(dsi);

#if !defined(__HAPS)
    if (priv->power_supply) regulator_set_enable(priv->power_supply, !plat->power_invert);

    if (dm_gpio_is_valid(&priv->enable_gpio)) dm_gpio_set_value(&priv->enable_gpio, 1);

    if (plat->delay.prepare) mdelay(plat->delay.prepare);

    if (dm_gpio_is_valid(&priv->reset_gpio)) dm_gpio_set_value(&priv->reset_gpio, 1);

    if (plat->delay.reset) mdelay(plat->delay.reset);

    if (dm_gpio_is_valid(&priv->reset_gpio)) dm_gpio_set_value(&priv->reset_gpio, 0);

    if (plat->delay.init) mdelay(plat->delay.init);

    if (plat->on_cmds) {
        if (priv->cmd_type == CMD_TYPE_SPI)
            ret = eswin_panel_send_spi_cmds(panel->state, plat->on_cmds);
        else if (priv->cmd_type == CMD_TYPE_MCU)
            ret = eswin_panel_send_mcu_cmds(panel->state, plat->on_cmds);
        else
            ret = eswin_panel_send_dsi_cmds(dsi, plat->on_cmds);
        if (ret) printf("failed to send on cmds: %d\n", ret);
    }
#endif
    priv->prepared = true;

    printf("********[%s] leave*********\n", __func__);
}

static void panel_simple_unprepare(struct eswin_panel *panel) {
    struct eswin_panel_plat *plat = dev_get_plat(panel->dev);
    struct eswin_panel_priv *priv = dev_get_priv(panel->dev);
    struct mipi_dsi_device *dsi = dev_get_parent_plat(panel->dev);
    int ret;

    if (!priv->prepared) return;

    if (plat->off_cmds) {
        if (priv->cmd_type == CMD_TYPE_SPI)
            ret = eswin_panel_send_spi_cmds(panel->state, plat->off_cmds);
        else if (priv->cmd_type == CMD_TYPE_MCU)
            ret = eswin_panel_send_mcu_cmds(panel->state, plat->off_cmds);
        else
            ret = eswin_panel_send_dsi_cmds(dsi, plat->off_cmds);
        if (ret) printf("failed to send off cmds: %d\n", ret);
    }

    if (dm_gpio_is_valid(&priv->reset_gpio)) dm_gpio_set_value(&priv->reset_gpio, 1);

    if (dm_gpio_is_valid(&priv->enable_gpio)) dm_gpio_set_value(&priv->enable_gpio, 0);

    if (priv->power_supply) regulator_set_enable(priv->power_supply, plat->power_invert);

    if (plat->delay.unprepare) mdelay(plat->delay.unprepare);

    priv->prepared = false;
}

static void panel_simple_enable(struct eswin_panel *panel) {
    struct eswin_panel_plat *plat = dev_get_plat(panel->dev);
    struct eswin_panel_priv *priv = dev_get_priv(panel->dev);

    if (priv->enabled) return;

    if (plat->delay.enable) mdelay(plat->delay.enable);

    if (priv->backlight) backlight_enable(priv->backlight);

    priv->enabled = true;
}

static void panel_simple_disable(struct eswin_panel *panel) {
    struct eswin_panel_plat *plat = dev_get_plat(panel->dev);
    struct eswin_panel_priv *priv = dev_get_priv(panel->dev);

    if (!priv->enabled) return;

    if (priv->backlight) backlight_set_brightness(priv->backlight, BACKLIGHT_OFF);
    if (plat->delay.disable) mdelay(plat->delay.disable);

    priv->enabled = false;
}

static void panel_simple_init(struct eswin_panel *panel) {
    printf("********[%s] enter*********\n", __func__);

    printf("********[%s] leave*********\n", __func__);
}

static const struct eswin_panel_funcs eswin_panel_funcs = {
    .init = panel_simple_init,
    .prepare = panel_simple_prepare,
    .unprepare = panel_simple_unprepare,
    .enable = panel_simple_enable,
    .disable = panel_simple_disable,
};

static int eswin_panel_ofdata_to_platdata(struct udevice *dev) {
    struct eswin_panel_plat *plat = dev_get_plat(dev);
    const void *data;
    int len = 0;
    int ret;

    plat->power_invert = dev_read_bool(dev, "power-invert");

    plat->delay.prepare = dev_read_u32_default(dev, "prepare-delay-ms", 0);
    plat->delay.unprepare = dev_read_u32_default(dev, "unprepare-delay-ms", 0);
    plat->delay.enable = dev_read_u32_default(dev, "enable-delay-ms", 0);
    plat->delay.disable = dev_read_u32_default(dev, "disable-delay-ms", 0);
    plat->delay.init = dev_read_u32_default(dev, "init-delay-ms", 0);
    plat->delay.reset = dev_read_u32_default(dev, "reset-delay-ms", 0);

    plat->bus_format = dev_read_u32_default(dev, "bus-format", MEDIA_BUS_FMT_RBG888_1X24);
    plat->bpc = dev_read_u32_default(dev, "bpc", 8);

    data = dev_read_prop(dev, "panel-init-sequence", &len);
    if (data) {
        plat->on_cmds = calloc(1, sizeof(*plat->on_cmds));
        if (!plat->on_cmds) return -ENOMEM;

        ret = eswin_panel_parse_cmds(data, len, plat->on_cmds);
        if (ret) {
            printf("failed to parse panel init sequence\n");
            goto free_on_cmds;
        }
    }

    data = dev_read_prop(dev, "panel-exit-sequence", &len);
    if (data) {
        plat->off_cmds = calloc(1, sizeof(*plat->off_cmds));
        if (!plat->off_cmds) {
            ret = -ENOMEM;
            goto free_on_cmds;
        }
        ret = eswin_panel_parse_cmds(data, len, plat->off_cmds);
        if (ret) {
            printf("failed to parse panel exit sequence\n");
            goto free_cmds;
        }
    }

    return 0;

free_cmds:
    free(plat->off_cmds);
free_on_cmds:
    free(plat->on_cmds);
    return ret;
}

static int eswin_panel_probe(struct udevice *dev) {
    struct eswin_panel_plat *plat = dev_get_plat(dev);
    struct eswin_panel *panel;
#if !defined(__HAPS)
    struct eswin_panel_priv *priv = dev_get_priv(dev);
    int ret;

    const char *cmd_type;
    ret = gpio_request_by_name(dev, "enable-gpios", 0, &priv->enable_gpio, GPIOD_IS_OUT);
    if (ret && ret != -ENOENT) {
        printf("%s: Cannot get enable GPIO: %d\n", __func__, ret);
        return ret;
    }

    ret = gpio_request_by_name(dev, "reset-gpios", 0, &priv->reset_gpio, GPIOD_IS_OUT);
    if (ret && ret != -ENOENT) {
        printf("%s: Cannot get reset GPIO: %d\n", __func__, ret);
        return ret;
    }

    ret = uclass_get_device_by_phandle(UCLASS_PANEL_BACKLIGHT, dev, "backlight", &priv->backlight);
    if (ret && ret != -ENOENT) {
        printf("%s: Cannot get backlight: %d\n", __func__, ret);
        return ret;
    }

    ret = uclass_get_device_by_phandle(UCLASS_REGULATOR, dev, "power-supply", &priv->power_supply);
    if (ret && ret != -ENOENT) {
        printf("%s: Cannot get power supply: %d\n", __func__, ret);
        return ret;
    }

    ret = dev_read_string_index(dev, "eswin,cmd-type", 0, &cmd_type);
    if (ret)
        priv->cmd_type = CMD_TYPE_DEFAULT;
    else
        priv->cmd_type = get_panel_cmd_type(cmd_type);

    if (priv->cmd_type == CMD_TYPE_SPI) {
        ret = gpio_request_by_name(dev, "spi-sdi-gpios", 0, &priv->spi_sdi_gpio, GPIOD_IS_OUT);
        if (ret && ret != -ENOENT) {
            printf("%s: Cannot get spi sdi GPIO: %d\n", __func__, ret);
            return ret;
        }
        ret = gpio_request_by_name(dev, "spi-scl-gpios", 0, &priv->spi_scl_gpio, GPIOD_IS_OUT);
        if (ret && ret != -ENOENT) {
            printf("%s: Cannot get spi scl GPIO: %d\n", __func__, ret);
            return ret;
        }
        ret = gpio_request_by_name(dev, "spi-cs-gpios", 0, &priv->spi_cs_gpio, GPIOD_IS_OUT);
        if (ret && ret != -ENOENT) {
            printf("%s: Cannot get spi cs GPIO: %d\n", __func__, ret);
            return ret;
        }
        dm_gpio_set_value(&priv->spi_sdi_gpio, 1);
        dm_gpio_set_value(&priv->spi_scl_gpio, 1);
        dm_gpio_set_value(&priv->spi_cs_gpio, 1);
        dm_gpio_set_value(&priv->reset_gpio, 0);
    }
#endif
    panel = calloc(1, sizeof(*panel));
    if (!panel) return -ENOMEM;

    dev->driver_data = (ulong)panel;
    panel->dev = dev;
    panel->bus_format = plat->bus_format;
    panel->bpc = plat->bpc;
    panel->funcs = &eswin_panel_funcs;

    return 0;
}

static const struct udevice_id eswin_panel_ids[] = {{
                                                        .compatible = "simple-panel",
                                                    },
                                                    {
                                                        .compatible = "simple-panel-dsi",
                                                    },
                                                    {}};

U_BOOT_DRIVER(eswin_panel) = {
    .name = "eswin_panel",
    .id = UCLASS_PANEL,
    .of_match = eswin_panel_ids,
    .of_to_plat = eswin_panel_ofdata_to_platdata,
    .probe = eswin_panel_probe,
    .priv_auto = sizeof(struct eswin_panel_priv),
    .plat_auto = sizeof(struct eswin_panel_plat),
};
