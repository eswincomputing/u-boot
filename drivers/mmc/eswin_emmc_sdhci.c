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
 * Authors: liangshuang <liangshuang@eswincomputing.com>
 */

#include <common.h>
#include <dm.h>
#include <dt-structs.h>
#include <linux/err.h>
#include <linux/libfdt.h>
#include <linux/delay.h>
#include <malloc.h>
#include <mapmem.h>
#include <sdhci.h>
#include <clk.h>
#include <mmc.h>
#include <dm/device_compat.h>
#include "eswin_sdhci.h"

/* 400KHz is max freq for card ID etc. Use that as min */
#define EMMC_MIN_FREQ   400000

#define mmc_hostname(x)   ((x)->dev->name)
extern int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value);

struct eswin_sdhci_plat
{
    struct mmc_config cfg;
    struct mmc mmc;
};

struct eswin_sdhci_phy_data {
    unsigned int drive_impedance;
    unsigned int enable_strobe_pulldown;
    unsigned int enable_data_pullup;
    unsigned int enable_cmd_pullup;
    unsigned int delay_code;
};

struct eswin_sdhci_data {
    struct sdhci_host host;
    struct eswin_sdhci_phy_data phy;
};

static void eswin_sdhci_coreclk_config(struct sdhci_host *host, unsigned short divisor, unsigned int flag_sel)
{
    u32 val = 0;
    u32 delay = 0xfffff;
    volatile u32 *addr;

    addr = (u32 *)(ESWIN_MSHC_CORE_CLK_REG + host->index * 4);

    val = *addr;
    val &= ~MSHC_CORE_CLK_ENABLE;
    *addr = val;
    while (delay--)
        ;
    val &= ~(MSHC_CORE_CLK_FREQ_BIT_MASK << MSHC_CORE_CLK_FREQ_BIT_SHIFT);
    val |= (divisor & MSHC_CORE_CLK_FREQ_BIT_MASK) << MSHC_CORE_CLK_FREQ_BIT_SHIFT;
    val &= ~(MSHC_CORE_CLK_SEL_BIT);
    val |= flag_sel;
    *addr = val;

    val |= MSHC_CORE_CLK_ENABLE;
    *addr = val;
}

static void eswin_sdhci_coreclk_disable(struct sdhci_host *host)
{
    u32 val = 0;
    volatile u32 *addr;
    addr = (u32 *)(ESWIN_MSHC_CORE_CLK_REG + host->index * 4);

    val = *addr;
    val &= ~MSHC_CORE_CLK_ENABLE;
    *addr = val;
}

void eswin_disable_card_clk(struct sdhci_host *host)
{
    unsigned int clk;

    /* Reset SD Clock Enable */
    clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
    clk &= ~SDHCI_CLOCK_CARD_EN;
    sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);
}

void eswin_enable_card_clk(struct sdhci_host *host)
{
    unsigned int i = 0;
    unsigned int clk;

    clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL);

    clk |= SDHCI_CLOCK_INT_EN;
    sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);

    /* Wait max 150 ms */
    while (1) {
        i++;
        clk = sdhci_readw(host, SDHCI_CLOCK_CONTROL);
        if (clk & SDHCI_CLOCK_INT_STABLE) break;
        if (i > 15000) {
            pr_err("sd %d: Internal clock never stabilised.\n", host->index);
            return;
        }
        udelay(10);
    }

    clk |= SDHCI_CLOCK_CARD_EN;
    sdhci_writew(host, clk, SDHCI_CLOCK_CONTROL);
    udelay(100);
}

void eswin_sdhci_set_clock(struct sdhci_host *host, unsigned int div_in)
{
    unsigned int clock, div, divide;
    unsigned int flag_sel, max_clk;

    clock = host->mmc->clock;
    if (clock == 0) {
        eswin_sdhci_coreclk_disable(host);
        return;
    }

    if (SDHCI_CLK_208M % clock == 0) {
        flag_sel = 1;
        max_clk = SDHCI_CLK_208M;
    } else {
        flag_sel = 0;
        max_clk = SDHCI_CLK_200M;
    }

    for (div = 1; div <= MAX_CORE_CLK_DIV; div++) {
        if ((max_clk / div) <= clock) break;
    }
    div--;

    if (div == 0 || div == 1) {
        divide = 2;
    } else {
        divide = (div + 1) * 2;
    }

    debug("mmc:index:%d clock:%d flag_sel:%d div:%d divide:%d\n",
                host->index, clock, flag_sel, div, divide);

    eswin_disable_card_clk(host);
    eswin_sdhci_coreclk_config(host, divide, flag_sel);
    eswin_enable_card_clk(host);
}

unsigned int eswin_convert_drive_impedance_ohm(struct udevice *dev, unsigned int dr_ohm)
{
    switch (dr_ohm) {
        case 100:
            return PHYCTRL_DR_100OHM;
        case 66:
            return PHYCTRL_DR_66OHM;
        case 50:
            return PHYCTRL_DR_50OHM;
        case 40:
            return PHYCTRL_DR_40OHM;
        case 33:
            return PHYCTRL_DR_33OHM;
    }

    dev_warn(dev, "Invalid value %u for drive-impedance-ohm.\n", dr_ohm);
    return PHYCTRL_DR_50OHM;
}

static void eswin_emmc_set_control_reg(struct sdhci_host *host)
{
    struct mmc *mmc = (struct mmc *)host->mmc;
    u32 reg;

    if (mmc->signal_voltage == MMC_SIGNAL_VOLTAGE_180) {
        reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
        reg |= SDHCI_CTRL_VDD_180;
        sdhci_writew(host, reg, SDHCI_HOST_CONTROL2);
    }

    if ((mmc->selected_mode != MMC_LEGACY) &&
            (mmc->selected_mode != MMC_HS))
            sdhci_set_uhs_timing(host);
}

void sdhci_set_signal_1_8v(struct sdhci_host *host)
{
    u32 reg;

    reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
    reg |= SDHCI_CTRL_VDD_180;
    sdhci_writew(host, reg, SDHCI_HOST_CONTROL2);
}

static int eswin_sdhci_phy_poweron(struct sdhci_host *host)
{
    unsigned long io_mem = (unsigned long)host->ioaddr;
    unsigned long hsp_io_mem = HSP_IO_MEM;
    u32 ret32 = 0;


    /*CONFIGURE IN HSPTOP, VOLTAGE*/
    writel(0x10010101, (void *)(hsp_io_mem + 0x508));
    writel(0x101, (void *)(hsp_io_mem + 0x50c));

    /*reset emmc*/
    writeb(0x1, (void *)(io_mem + SW_RST_R));
    /*negedge data sent out enable MSHC_CTRL_R -- negedge data sent out enable*/
    writew(0x2, (void *)(io_mem + 0x508));
    /*PWR_CTRL_R.SD_BUS_VOL_VDD1*/

    /*TOUT_CTRL_R.TOUR_CNT*/
    writeb(0x5, (void *)(io_mem + TOUT_CTRL_R));
    /*HOST_CTRL2_R.UHS2_IF_ENABLE=0*/
    writew(0x0, (void *)(io_mem + HOST_CTRL2_R));
    /*EMMC_CTRL_R/CARD_ISEMMC=1 NOT SPEC*/
    writew(0xd, (void *)(io_mem + 0x52C));//Reset to eMMC device is deasserted

    /*set blockcount*/
    writel(0xffffffff, (void *)(io_mem + SDMASA_R));

    while (1)
    {
        ret32 = readl((void *)(io_mem + PHY_CNFG_R)); // DWC_MSHC_PTR_PHY_R 0x300
        if (ret32 == 0x2)
        {
            debug("Phy's Power on!\r\n");
            break;
        }
    }

    return ret32;
}

static int eswin_sdhci_pre_init(struct sdhci_host *host)
{
    unsigned int reg_val;
    volatile unsigned int *addr;

    /*sw rst*/
    addr = (unsigned int *)(ESWIN_HSPDMA_RST_CTRL);
    reg_val = *addr;
    reg_val |= ESWIN_HSPDMA_EMMC_RST;
    *addr = reg_val;

    eswin_sdhci_phy_poweron(host);

    return 0;
}

static void eswin_sdhci_config_phy_delay(struct sdhci_host *host, int delay)
{
    delay &= PHY_CLK_MAX_DELAY_MASK;

    /*phy clk delay line config*/
    sdhci_writeb(host, PHY_UPDATE_DELAY_CODE, PHY_SDCLKDL_CNFG_R);
    sdhci_writeb(host, delay, PHY_SDCLKDL_DC_R);
    sdhci_writeb(host, 0x0, PHY_SDCLKDL_CNFG_R);
}

static void eswin_sdhci_config_phy(struct sdhci_host *host)
{
    unsigned int val = 0;
    unsigned int drv = 0;
    struct udevice *dev = host->mmc->dev;
    struct eswin_sdhci_data *eswin_sdhci = dev_get_priv(dev);
    struct eswin_sdhci_phy_data *phy = &eswin_sdhci->phy;

    drv = phy->drive_impedance << PHY_PAD_SP_DRIVE_SHIF;
    debug("%s: phy drv=0x%x\n", mmc_hostname(host->mmc), drv);

    eswin_disable_card_clk(host);

    /* reset phy,config phy's pad */
    sdhci_writel(host, drv | (~PHY_RSTN), PHY_CNFG_R);
    /*CMDPAD_CNFS*/
    val = (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_P_BIT_SHIFT) |
        (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_N_BIT_SHIFT) |
        (phy->enable_cmd_pullup << PHY_PULL_BIT_SHIF) | PHY_PAD_RXSEL_1;
    sdhci_writew(host, val, PHY_CMDPAD_CNFG_R);
    debug("%s: phy cmd=0x%x\n", mmc_hostname(host->mmc), val);

    /*DATA PAD CNFG*/
    val = (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_P_BIT_SHIFT) |
        (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_N_BIT_SHIFT) |
        (phy->enable_data_pullup << PHY_PULL_BIT_SHIF) | PHY_PAD_RXSEL_1;
    sdhci_writew(host, val, PHY_DATAPAD_CNFG_R);
    debug("%s: phy data=0x%x\n", mmc_hostname(host->mmc), val);

    /*Clock PAD Setting*/
    val = (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_P_BIT_SHIFT) |
        (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_N_BIT_SHIFT) | PHY_PAD_RXSEL_0;
    sdhci_writew(host, val, PHY_CLKPAD_CNFG_R);
    debug("%s: phy clk=0x%x\n", mmc_hostname(host->mmc), val);

    /*PHY strobe PAD setting*/
    val = (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_P_BIT_SHIFT) |
        (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_N_BIT_SHIFT) |
        ((phy->enable_strobe_pulldown * PHY_PULL_DOWN) << PHY_PULL_BIT_SHIF) |
        PHY_PAD_RXSEL_1;
    sdhci_writew(host, val, PHY_STBPAD_CNFG_R);
    debug("%s: phy strobe=0x%x\n", mmc_hostname(host->mmc), val);
    mdelay(2);

    /*PHY RSTN PAD setting*/
    val = (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_P_BIT_SHIFT) |
        (PHY_SLEW_2 << PHY_TX_SLEW_CTRL_N_BIT_SHIFT) |
        (PHY_PULL_UP << PHY_PULL_BIT_SHIF) | PHY_PAD_RXSEL_1;
    sdhci_writew(host, val, PHY_RSTNPAD_CNFG_R);
    debug("%s: phy rstn=0x%x\n", mmc_hostname(host->mmc), val);

    sdhci_writel(host, drv | PHY_RSTN, PHY_CNFG_R);

    eswin_sdhci_config_phy_delay(host, phy->delay_code);

    eswin_enable_card_clk(host);
}

static int eswin_sdhci_set_ios_post(struct sdhci_host *host)
{
    int ret = 0;

    if (host->mmc) {
        if (host->mmc->ext_csd) {
           if (host->mmc->ext_csd[EXT_CSD_RST_N_FUNCTION] == 0) {
                ret = mmc_switch(host->mmc, EXT_CSD_CMD_SET_NORMAL, EXT_CSD_RST_N_FUNCTION, 1);
                if (ret) {
                    printf("emmc: set reset n function failed!\n");
                } else {
                    host->mmc->ext_csd[EXT_CSD_RST_N_FUNCTION] = 1;
                }
            }
        }
    }

    sdhci_set_signal_1_8v(host);

    sdhci_writew(host, 0xd, VENDOR_EMMC_CTRL_R);
    sdhci_writeb(host, 0x12, VENDOR_MSHC_CTRL_R);

    eswin_sdhci_config_phy(host);

    return 0;
}

static struct sdhci_ops eswin_emmc_ops = {
    .set_control_reg = &eswin_emmc_set_control_reg,
    .set_clock	= &eswin_sdhci_set_clock,
    .set_ios_post = &eswin_sdhci_set_ios_post,
};

static int eswin_sdhci_probe(struct udevice *dev)
{
    struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
    struct eswin_sdhci_plat *plat = dev_get_plat(dev);
    struct eswin_sdhci_data *eswin_sdhci = dev_get_priv(dev);
    struct sdhci_host *host = &eswin_sdhci->host;
    int max_frequency, ret;
    unsigned int val = 0;

    ret = dev_read_s32(dev, "index", &host->index);
    if (ret) {
        dev_err(dev, "%s %d, failed to get index id, ret %d!\r\n", __func__, __LINE__, ret);
        return ret;
    }

    if (!dev_read_u32(dev, "delay_code", &val)) {
        eswin_sdhci->phy.delay_code = val;
        debug("delay_code:%d\n", val);
    }

    if (!dev_read_u32(dev, "drive-impedance-ohm", &val))
        eswin_sdhci->phy.drive_impedance = eswin_convert_drive_impedance_ohm(dev, val);

    if (dev_read_bool(dev, "enable-cmd-pullup"))
        eswin_sdhci->phy.enable_cmd_pullup = ENABLE;
    else
        eswin_sdhci->phy.enable_cmd_pullup = DISABLE;

    if (dev_read_bool(dev, "enable-data-pullup"))
        eswin_sdhci->phy.enable_data_pullup = ENABLE;
    else
        eswin_sdhci->phy.enable_data_pullup = DISABLE;

    if (dev_read_bool(dev, "enable-strobe-pulldown"))
        eswin_sdhci->phy.enable_strobe_pulldown = ENABLE;
    else
        eswin_sdhci->phy.enable_strobe_pulldown = DISABLE;

    max_frequency = dev_read_u32_default(dev, "max-frequency", 0);
    switch (dev_read_u32_default(dev, "bus-width", 4))
    {
    case 8:
        host->host_caps |= MMC_MODE_8BIT;
        break;
    case 4:
        host->host_caps |= MMC_MODE_4BIT;
        break;
    case 1:
        break;
    default:
        printf("Invalid \"bus-width\" value\n");
        return -EINVAL;
    }

    host->quirks = SDHCI_QUIRK_WAIT_SEND_CMD;

    debug("emmc max_frequency=%d\n",max_frequency);
    host->max_clk = max_frequency;
    /*
     * The sdhci-driver only supports 4bit and 8bit, as sdhci_setup_cfg
     * doesn't allow us to clear MMC_MODE_4BIT.  Consequently, we don't
     * check for other bus-width values.
     */
    if (host->bus_width == 8)
        host->host_caps |= MMC_MODE_8BIT;

    host->mmc = &plat->mmc;
    host->mmc->priv = &eswin_sdhci->host;
    host->mmc->dev = dev;
    upriv->mmc = host->mmc;

    eswin_sdhci_pre_init(host);
    ret = sdhci_setup_cfg(&plat->cfg, host, 0, EMMC_MIN_FREQ);
    if (ret)
        return ret;

    host->ops = &eswin_emmc_ops;
    ret = sdhci_probe(dev);

    return ret;
}

static int eswin_sdhci_ofdata_to_platdata(struct udevice *dev)
{
#if !CONFIG_IS_ENABLED(OF_PLATDATA)
    struct sdhci_host *host = dev_get_priv(dev);

    host->name = dev->name;
    host->ioaddr = dev_read_addr_ptr(dev);
    host->bus_width = dev_read_u32_default(dev, "bus-width", 4);
#endif

    return 0;
}

static int eswin_sdhci_bind(struct udevice *dev)
{
    struct eswin_sdhci_plat *plat = dev_get_plat(dev);

    return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id eswin_sdhci_ids[] =
{
    { .compatible = "eswin,emmc-sdhci-5.1" },
    { }
};

U_BOOT_DRIVER(eswin_sdhci_drv) =
{
    .name		= "eswin_emmc_sdhci_5_1",
    .id		= UCLASS_MMC,
    .of_match	= eswin_sdhci_ids,
    .of_to_plat = eswin_sdhci_ofdata_to_platdata,
    .ops		= &sdhci_ops,
    .bind		= eswin_sdhci_bind,
    .probe		= eswin_sdhci_probe,
    .priv_auto = sizeof(struct eswin_sdhci_data),
    .plat_auto = sizeof(struct eswin_sdhci_plat),
};
