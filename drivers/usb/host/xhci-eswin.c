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
 * Authors: yangwei <yangwei1@eswincomputing.com>
 */

#include <common.h>
#include <dm.h>
#include <malloc.h>
#include <usb.h>
#include <watchdog.h>
#include <linux/errno.h>
#include <linux/io.h>
#include <linux/compat.h>
#include <linux/usb/dwc3.h>
#include <power/regulator.h>

#include "xhci.h"

DECLARE_GLOBAL_DATA_PTR;

struct eswin_xhci_platdata {
    fdt_addr_t hcd_base;
    fdt_addr_t phy_base;
    struct udevice *vbus_supply;
};

/*
 * Contains pointers to register base addresses
 * for the usb controller.
 */
struct eswin_xhci {
    struct usb_plat usb_plat;
    struct xhci_ctrl ctrl;
    struct xhci_hccr *hcd;
    struct dwc3 *dwc3_reg;
};

static int xhci_usb_of_to_plat(struct udevice *dev)
{
    struct eswin_xhci_platdata *plat = dev_get_plat(dev);
    struct udevice *child;
    int ret = 0;

    /*
     * Get the base address for XHCI controller from the device node
     */
    plat->hcd_base = dev_read_addr(dev);
    if (plat->hcd_base == FDT_ADDR_T_NONE) {
        pr_err("Can't get the XHCI register base address\n");
        return -ENXIO;
    }

    /* Get the base address for usbphy from the device node */
    for (device_find_first_child(dev, &child); child;
         device_find_next_child(&child)) {
        if (!device_is_compatible(child, "eswin,rk3399-usb3-phy"))
            continue;
        plat->phy_base = devfdt_get_addr(child);
        break;
    }

   // if (plat->phy_base == FDT_ADDR_T_NONE) {
   //     pr_err("Can't get the usbphy register address\n");
   //     return -ENXIO;
   // }

    /* Vbus regulator */
    ret = device_get_supply_regulator(dev, "vbus-supply",
                      &plat->vbus_supply);
    if (ret)
        debug("Can't get VBus regulator!\n");

    return 0;
}

/*
 * eswin_dwc3_phy_setup() - Configure USB PHY Interface of DWC3 Core
 * @dwc: Pointer to our controller context structure
 * @dev: Pointer to ulcass device
 */
static void eswin_dwc3_phy_setup(struct dwc3 *dwc3_reg,
                    struct udevice *dev)
{
    u32 reg;
    u32 utmi_bits;

    /* Set dwc3 usb2 phy config */
    reg = readl(&dwc3_reg->g_usb2phycfg[0]);

    if (dev_read_bool(dev, "snps,dis-enblslpm-quirk"))
        reg &= ~DWC3_GUSB2PHYCFG_ENBLSLPM;

    utmi_bits = dev_read_u32_default(dev, "snps,phyif-utmi-bits", -1);
    if (utmi_bits == 16) {
        reg |= DWC3_GUSB2PHYCFG_PHYIF;
        reg &= ~DWC3_GUSB2PHYCFG_USBTRDTIM_MASK;
        reg |= DWC3_GUSB2PHYCFG_USBTRDTIM_16BIT;
    } else if (utmi_bits == 8) {
        reg &= ~DWC3_GUSB2PHYCFG_PHYIF;
        reg &= ~DWC3_GUSB2PHYCFG_USBTRDTIM_MASK;
        reg |= DWC3_GUSB2PHYCFG_USBTRDTIM_8BIT;
    }

    if (dev_read_bool(dev, "snps,dis-u2-freeclk-exists-quirk"))
        reg &= ~DWC3_GUSB2PHYCFG_U2_FREECLK_EXISTS;

    if (dev_read_bool(dev, "snps,dis-u2-susphy-quirk"))
        reg &= ~DWC3_GUSB2PHYCFG_SUSPHY;

    writel(reg, &dwc3_reg->g_usb2phycfg[0]);
}

static int eswin_xhci_core_init(struct eswin_xhci *eswxhci,
                   struct udevice *dev)
{
    int ret;

    ret = dwc3_core_init(eswxhci->dwc3_reg);
    if (ret) {
        pr_err("failed to initialize core\n");
        return ret;
    }

    eswin_dwc3_phy_setup(eswxhci->dwc3_reg, dev);

    /* We are hard-coding DWC3 core to Host Mode */
    dwc3_set_mode(eswxhci->dwc3_reg, DWC3_GCTL_PRTCAP_HOST);
    /* We are hard-coding DWC3 core to Host Device */
    //dwc3_set_mode(eswxhci->dwc3_reg, DWC3_GCTL_PRTCAP_DEVICE);

    return 0;
}

static int eswin_xhci_core_exit(struct eswin_xhci *eswxhci)
{
    return 0;
}
static int dwc_usb_clk_init(void)
{
    void __iomem *crg_regs;
    void __iomem *hsp_regs;
    u32 val = 0;
    /*Use clk framework instead*/
    hsp_regs = ioremap(0x50440000, 0x200);
    crg_regs = ioremap(0x51828000, 0x1000);

    //reset hsp_por
    val = readl(crg_regs + 0x41c);
    val |= 0x00018007;
    writel(val, crg_regs + 0x41c);

    //enable scu_hsp_pclk
    writel(0x80000023, crg_regs + 0x148);
    writel(0xc0000000, crg_regs + 0x14c);

    //usb0 clk init
    //ref clk is 24M, below need to be set to satisfy usb phy requirement(125M)
    writel(0x0000002a, hsp_regs + 0x83c);
    writel(0x00000000, hsp_regs + 0x840);

    //reset usb core and usb phy
    writel(0x11010201, hsp_regs + 0x800);
    writel(0x00010001, hsp_regs + 0x808);

    //usb1 clk init
    //ref clk is 24M, below need to be set to satisfy usb phy requirement(125M)
    writel(0x0000002a, hsp_regs + 0x93c);
    writel(0x00000000, hsp_regs + 0x940);

    //reset usb core and usb phy
    writel(0x11010201, hsp_regs + 0x900);
    writel(0x00010001, hsp_regs + 0x908);

    return 0;
}
static int xhci_usb_probe(struct udevice *dev)
{
    struct eswin_xhci_platdata *plat = dev_get_plat(dev);
    struct eswin_xhci *ctx = dev_get_priv(dev);
    struct xhci_hcor *hcor;
    int ret;

    dwc_usb_clk_init();
    ctx->hcd = (struct xhci_hccr *)plat->hcd_base;
    ctx->dwc3_reg = (struct dwc3 *)((char *)(ctx->hcd) + DWC3_REG_OFFSET);
    xhci_readl(&ctx->hcd->cr_capbase);
    hcor = (struct xhci_hcor *)((uint64_t)ctx->hcd +
            HC_LENGTH(xhci_readl(&ctx->hcd->cr_capbase)));
    if (plat->vbus_supply) {
        ret = regulator_set_enable(plat->vbus_supply, true);
        if (ret) {
            pr_err("XHCI: failed to set VBus supply\n");
            return ret;
        }
    }

    ret = eswin_xhci_core_init(ctx, dev);
    if (ret) {
        pr_err("XHCI: failed to initialize controller\n");
        return ret;
    }

    return xhci_register(dev, ctx->hcd, hcor);
}

static int xhci_usb_remove(struct udevice *dev)
{
    struct eswin_xhci_platdata *plat = dev_get_plat(dev);
    struct eswin_xhci *ctx = dev_get_priv(dev);
    int ret;

    ret = xhci_deregister(dev);
    if (ret)
        return ret;
    ret = eswin_xhci_core_exit(ctx);
    if (ret)
        return ret;

    if (plat->vbus_supply) {
        ret = regulator_set_enable(plat->vbus_supply, false);
        if (ret)
            pr_err("XHCI: failed to set VBus supply\n");
    }

    return ret;
}

static const struct udevice_id xhci_usb_ids[] = {
    { .compatible = "eswin,eic7700-dwc3-host" },
    { }
};

U_BOOT_DRIVER(usb_xhci) = {
    .name   = "xhci_eswin",
    .id = UCLASS_USB,
    .of_match = xhci_usb_ids,
    .of_to_plat = xhci_usb_of_to_plat,
    .probe = xhci_usb_probe,
    .remove = xhci_usb_remove,
    .ops    = &xhci_usb_ops,
    .bind   = dm_scan_fdt_dev,
    .plat_auto = sizeof(struct eswin_xhci_platdata),
    .priv_auto = sizeof(struct eswin_xhci),
    .flags  = DM_FLAG_ALLOC_PRIV_DMA,
};

static const struct udevice_id usb_phy_ids[] = {
    { .compatible = "eswin,rk3399-usb3-phy" },
    { .compatible = "eswin,rk3328-usb3-phy" },
    { }
};

U_BOOT_DRIVER(usb_phy) = {
    .name = "usb_phy_eswin",
    .of_match = usb_phy_ids,
};
