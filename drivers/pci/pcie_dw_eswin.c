// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN PCIe root complex driver
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
 * Authors: Shuai Qin <qinshuai@eswincomputing.com>
 */

#include <common.h>
#include <clk.h>
#include <dm.h>
#include <generic-phy.h>
#include <pci.h>
#include <power-domain.h>
#include <reset.h>
#include <syscon.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <dm/device_compat.h>
#include <linux/iopoll.h>
#include <linux/delay.h>
#include <linux/log2.h>
#include <linux/bitfield.h>

#include "pcie_dw_common.h"

DECLARE_GLOBAL_DATA_PTR;

/**
 * struct eswin_pcie - Eswin EIC7700 DW PCIe controller state
 *
 * @pci: The common PCIe DW structure
 * @eswin_cfg_base: The base address of vendor regs
 * @phy
 * @aclk
 * @cfg_clk
 * @auxclk
 * @cr_clk
 * @rsts
 * @rst_gpio: The #PERST signal for slot
 */
struct eswin_pcie {
    /* Must be first member of the struct */
    struct pcie_dw dw;
    void *eswin_cfg_base;
    struct phy phy;
    struct clk *pcie_aux;
    struct clk *pcie_cfg;
    struct clk *pcie_cr;
    struct clk *pcie_aclk;
    struct reset_ctl *powerup_rst;
    struct reset_ctl *cfg_rst;
    struct reset_ctl *perst;
    struct gpio_desc *rst_gpio;
};

/* PCIe top csr */
#define PCIE_CTRL_CFG0           0x0
#define PCIE_STATUS0            0x100
#define APP_LTSSM_ENABLE        BIT(5)

/* sys crg base */
#define SYSCRG_CSR    0x01828000

#define PCI_EXP_DEVCTL_PAYLOAD  (0x00e0)  /* Max_Payload_Size */
#define PCIE_CAP_MAX_PAYLOAD_SIZE(x)    ((x) << 5)
#define PCIE_CAP_MAX_READ_REQ_SIZE(x)   ((x) << 12)

#define WAIT_LINKUP_TIMEOUT     8000
#define PORT_CLK_RATE           100000000UL
#define MAX_PAYLOAD_SIZE        512
#define MAX_READ_REQ_SIZE       512
#define PCIE_RESET_DELAY        500

/* Parameters for the waiting for #perst signal */
#define PERST_WAIT_US           1000000

static inline u32 eswin_cfg_readl(struct eswin_pcie *priv, u32 reg)
{
    return readl(priv->eswin_cfg_base + reg);
}

static inline void eswin_cfg_writel(struct eswin_pcie *priv, u32 val, u32 reg)
{
    writel(val, priv->eswin_cfg_base + reg);
}

static int eswin_size_to_payload(int size)
{
    /*
     * dwc supports 2^(val+7) payload size, which val is 0~5 default to 1.
     * So if input size is not 2^order alignment or less than 2^7 or bigger
     * than 2^12, just set to default size 2^(1+7).
     */
    if (!is_power_of_2(size) || size < 128 || size > 4096) {
        debug("%s: payload size %d, set to default 256\n", __func__, size);
        return 1;
    }

    return fls(size) - 8;
}

static void eswin_set_max_payload(struct eswin_pcie *priv, int size)
{
    u32 val;
    u32 offset = 0x70 + PCI_EXP_DEVCTL;
    int max_payload_size = eswin_size_to_payload(size);

    dw_pcie_dbi_write_enable(&priv->dw, true);

    val = readl(priv->dw.dbi_base + offset);
    val &= ~PCI_EXP_DEVCTL_PAYLOAD;
    val |= PCIE_CAP_MAX_PAYLOAD_SIZE(max_payload_size);
    writel(val, priv->dw.dbi_base + offset);

    dw_pcie_dbi_write_enable(&priv->dw, false);
}

static void eswin_set_max_rd_req_size(struct eswin_pcie *priv, int size)
{
    u32 val;
    u32 offset = 0x70 + PCI_EXP_DEVCTL;
    int max_rd_req_size = eswin_size_to_payload(size);

    dw_pcie_dbi_write_enable(&priv->dw, true);

    val = readl(priv->dw.dbi_base + offset);
    val &= ~PCI_EXP_DEVCTL_PAYLOAD;
    val |= PCIE_CAP_MAX_READ_REQ_SIZE(max_rd_req_size);
    writel(val, priv->dw.dbi_base + offset);

    dw_pcie_dbi_write_enable(&priv->dw, false);
}

static inline void eswin_pcie_enable_ltssm(struct eswin_pcie *priv)
{
    u32 val;

    val = eswin_cfg_readl(priv, PCIE_CTRL_CFG0);
    val |= APP_LTSSM_ENABLE;
    eswin_cfg_writel(priv, val, PCIE_CTRL_CFG0);
}

static int eswin_pcie_wait_link_up(struct eswin_pcie *priv)
{
    u32 cnt = 0;
    u32 val;
    /* ltssm in L0, smlh up & rdlh up */
    u32 linkup = ((0x11<<2)|0x3);

    val = eswin_cfg_readl(priv, PCIE_STATUS0);
    do {
        if((val & 0xff) == linkup)
            return 0;
        val = eswin_cfg_readl(priv, PCIE_STATUS0);
        cnt++;
        udelay(100);

    } while (cnt < WAIT_LINKUP_TIMEOUT);

    printf("%s: error: wait linkup timeout\n", __func__);
    return -EIO;
}

/**
 * eswin_pcie_link_up() - Wait for the link to come up
 *
 * @eswin_pcie: Pointer to the PCI controller state
 * @cap_speed: Desired link speed
 *
 * Return: 1 (true) for active line and negative (false) for no link (timeout)
 */
static int eswin_pcie_link_up(struct eswin_pcie *priv)
{
    /* Enable LTSSM */
    eswin_pcie_enable_ltssm(priv);

    return eswin_pcie_wait_link_up(priv);
}

static int eswin_pcie_clk_enable(struct eswin_pcie *priv)
{
    int ret = 0;

    ret = clk_prepare_enable(priv->pcie_cr);
    if (ret) {
        pr_err("PCIe: failed to enable cr clk: %d\n", ret);
        return ret;
    }

    ret = clk_prepare_enable(priv->pcie_aclk);
    if (ret) {
        pr_err("PCIe: failed to enable aclk: %d\n", ret);
        return ret;
    }

    ret= clk_prepare_enable(priv->pcie_cfg);
    if (ret) {
        pr_err("PCIe: failed to enable cfg_clk: %d\n", ret);
        return ret;
    }

    ret= clk_prepare_enable(priv->pcie_aux);
    if (ret) {
        pr_err("PCIe: failed to enable aux_clk: %d\n", ret);
        return ret;
    }

    return 0;
}

static int eswin_pcie_clk_disable(struct eswin_pcie *priv)
{
    clk_disable_unprepare(priv->pcie_aux);
    clk_disable_unprepare(priv->pcie_cfg);
    clk_disable_unprepare(priv->pcie_cr);
    clk_disable_unprepare(priv->pcie_aclk);

    return 0;
}

static int eswin_pcie_power_on(struct eswin_pcie *priv)
{
    int ret = 0;

    /* pciet_cfg_rstn */
    ret = reset_deassert(priv->cfg_rst);
    WARN_ON(0 != ret);

    /* pciet_powerup_rstn */
    ret = reset_deassert(priv->powerup_rst);
    WARN_ON(0 != ret);

    return ret;
}

static int eswin_pcie_power_off(struct eswin_pcie *priv)
{
    reset_assert(priv->perst);

    reset_assert(priv->powerup_rst);

    reset_assert(priv->cfg_rst);

    return 0;
}

static int eswin_pcie_init_port(struct udevice *dev)
{
    u32 val;
    int ret = 0;
    struct eswin_pcie *priv = dev_get_priv(dev);

    /* pinmx_cfg(); */
    val = eswin_cfg_readl(priv, 0x01600000 + 0xf0);
    val = val | 0x01;
    eswin_cfg_writel(priv, val, 0x01600000 + 0xf0);

    /* pciet_aux_clken, pcie_cfg_clken */
    ret = eswin_pcie_clk_enable(priv);
    if (ret)
        return ret;

    ret = eswin_pcie_power_on(priv);
    if (ret)
        return ret;

    /* set device type : rc */
    val = eswin_cfg_readl(priv, PCIE_CTRL_CFG0);
    val = val & (~0xf);
    val = val | 0x4;
    eswin_cfg_writel(priv, val, PCIE_CTRL_CFG0);

    ret = reset_assert(priv->perst);
    WARN_ON(0 != ret);
    udelay(100);
    ret = reset_deassert(priv->perst);
    WARN_ON(0 != ret);

    /* app_hold_phy_rst */
    val = eswin_cfg_readl(priv, PCIE_CTRL_CFG0);
    val = val & ~(0x40);
    eswin_cfg_writel(priv, val, PCIE_CTRL_CFG0);

    /* wait pm_sel_aux_clk to 0 */
    val = eswin_cfg_readl(priv, PCIE_STATUS0);
    while((val & 0x10000) != 0)
    {
        udelay(5000);
        val = eswin_cfg_readl(priv, PCIE_STATUS0);
    }

    /* DW link configurations */
    dw_pcie_dbi_write_enable(&priv->dw, true);

    /* config eswin vendor id and eic7x device id */
    writel(0x20301fe1, priv->dw.dbi_base + 0x0);

    /* lane fix config, default x4 */
    val = readl(priv->dw.dbi_base + 0x8c0);
    val &= 0xffffff80;
    val |= 0x44;
    writel(val, priv->dw.dbi_base + 0x8c0);

    eswin_set_max_payload(priv, MAX_PAYLOAD_SIZE);
    eswin_set_max_rd_req_size(priv, MAX_READ_REQ_SIZE);

   /*  config support 32 msi vectors */
    val = readl(priv->dw.dbi_base + 0x50);
    val &= ~(0x7<<17);
    val |= (0x7<<17);
    writel(val, priv->dw.dbi_base + 0x50);

    /* disable msix cap */
    val = readl(priv->dw.dbi_base + 0x70);
    val &= 0xffff00ff;
    writel(val, priv->dw.dbi_base + 0x70);

    dw_pcie_dbi_write_enable(&priv->dw, false);

    pcie_dw_setup_host(&priv->dw);

    eswin_pcie_link_up(priv);

    return 0;
}

static int eswin_pcie_parse_dt(struct udevice *dev)
{
    struct eswin_pcie *priv = dev_get_priv(dev);

    priv->dw.dbi_base = (void *)dev_read_addr_index(dev, 0);
    if (IS_ERR(priv->dw.dbi_base))
        return PTR_ERR(priv->pcie_aux);

    dev_dbg(dev, "DBI address is 0x%p\n", priv->dw.dbi_base);

    priv->eswin_cfg_base = (void *)dev_read_addr_index(dev, 1);
    if (IS_ERR(priv->eswin_cfg_base))
        return PTR_ERR(priv->pcie_aux);

    dev_dbg(dev, "CFG address is 0x%p\n", priv->eswin_cfg_base);

    /* Fetch clocks */
    priv->pcie_aux = devm_clk_get(dev, "pcie_aux_clk");
    if (IS_ERR(priv->pcie_aux)) {
        dev_err(dev, "pcie_aux clock source missing or invalid\n");
        return PTR_ERR(priv->pcie_aux);
    }

    priv->pcie_cfg = devm_clk_get(dev, "pcie_cfg_clk");
    if (IS_ERR(priv->pcie_cfg)) {
        dev_err(dev, "pcie_cfg_clk clock source missing or invalid\n");
        return PTR_ERR(priv->pcie_cfg);
    }

    priv->pcie_cr = devm_clk_get(dev, "pcie_cr_clk");
    if (IS_ERR(priv->pcie_cr)) {
        dev_err(dev, "pcie_cr_clk clock source missing or invalid\n");
        return PTR_ERR(priv->pcie_cr);
    }

    priv->pcie_aclk = devm_clk_get(dev, "pcie_aclk");

    if (IS_ERR(priv->pcie_aclk)) {
        dev_err(dev, "pcie_aclk clock source missing or invalid\n");
        return PTR_ERR(priv->pcie_aclk);
    }

    /* Fetch reset */
    priv->powerup_rst = devm_reset_control_get(dev, "pcie_powerup");
    if (IS_ERR_OR_NULL(priv->powerup_rst)) {
        dev_err(dev, "unable to get powerup reset\n");
        return PTR_ERR(priv->powerup_rst);
    }

    priv->cfg_rst = devm_reset_control_get(dev, "pcie_cfg");
    if (IS_ERR_OR_NULL(priv->cfg_rst)) {
        dev_err(dev, "unable to get cfg reset\n");
        return PTR_ERR(priv->cfg_rst);
    }

    priv->perst = devm_reset_control_get(dev, "pcie_pwren");
    if (IS_ERR_OR_NULL(priv->perst)) {
        dev_err(dev, "unable to get perst\n");
        return PTR_ERR(priv->perst);
    }

    return 0;
}

/**
 * eswin_pcie_probe() - Probe the PCIe bus for active link
 *
 * @dev: A pointer to the device being operated on
 *
 * Probe for an active link on the PCIe bus and configure the controller
 * to enable this port.
 *
 * Return: 0 on success, else -ENODEV
 */
static int eswin_pcie_probe(struct udevice *dev)
{
    struct eswin_pcie *priv = dev_get_priv(dev);
    struct udevice *ctlr = pci_get_controller(dev);
    struct pci_controller *hose = dev_get_uclass_priv(ctlr);
    int ret = 0;

    priv->dw.first_busno = dev->seq_;
    priv->dw.dev = dev;

    ret = eswin_pcie_parse_dt(dev);
    if (ret)
        return ret;

    ret = eswin_pcie_init_port(dev);
    if (ret) {
        eswin_pcie_power_off(priv);
        eswin_pcie_clk_disable(priv);
        dm_gpio_free(dev, priv->rst_gpio);
        return ret;
    }

    printf("PCIE-%d: Link up (Gen%d-x%d, Bus%d)\n",
           dev->seq_, pcie_dw_get_link_speed(&priv->dw),
           pcie_dw_get_link_width(&priv->dw),
           hose->first_busno);

    return pcie_dw_prog_outbound_atu_unroll(&priv->dw,
                        PCIE_ATU_REGION_INDEX0,
                        PCIE_ATU_TYPE_MEM,
                        priv->dw.mem.phys_start,
                        priv->dw.mem.bus_start,
                        priv->dw.mem.size);

}

static const struct dm_pci_ops eswin_pcie_ops = {
    .read_config    = pcie_dw_read_config,
    .write_config   = pcie_dw_write_config,
};

static const struct udevice_id eswin_pcie_ids[] = {
    { .compatible = "eswin,eic770x-pcie" },
    { }
};

U_BOOT_DRIVER(eswin_dw_pcie) = {
    .name           = "pcie_dw_eswin",
    .id             = UCLASS_PCI,
    .of_match       = eswin_pcie_ids,
    .ops            = &eswin_pcie_ops,
    .probe          = eswin_pcie_probe,
    .priv_auto      = sizeof(struct eswin_pcie),
};
