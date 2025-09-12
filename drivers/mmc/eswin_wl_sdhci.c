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
 * Authors: wangjinlong <wangjinlong@eswincomputing.com>
 */

#include <common.h>
#include <dm.h>
#include <dt-structs.h>
#include <linux/err.h>
#include <linux/libfdt.h>
#include <malloc.h>
#include <mapmem.h>
#include <sdhci.h>
#include <clk.h>
#include <reset.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <dm/device_compat.h>
#include "eswin_sdhci.h"

/* 400KHz is max freq for card ID etc. Use that as min */
#define SD_MIN_FREQ 400000

extern unsigned int eswin_convert_drive_impedance_ohm(struct udevice *dev, unsigned int dr_ohm);

struct eswin_sd_plat {
	struct mmc_config cfg;
	struct mmc mmc;
};

static int sdhci_wl_phy_poweron(struct udevice *dev)
{
    int ret = -1;
    struct gpio_desc phy_reset_gpio;

    ret = gpio_request_by_name(dev, "rst-gpios", 0, &phy_reset_gpio, GPIOD_IS_OUT);
    if (ret) {
        pr_err("%s: failed to request gpio, ret=%d\n", __func__, ret);
        return ret;
    }

    if (dm_gpio_is_valid(&phy_reset_gpio)) {
        mdelay(20);
        ret = dm_gpio_set_value(&phy_reset_gpio, 1);
        if (ret)
            pr_err("Eswin %s gpio set failed\n", __func__);

        dm_gpio_free(dev, &phy_reset_gpio);
    }
    return ret;
}

static int eswin_sd_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct eswin_sd_plat *plat = dev_get_plat(dev);
	struct eswin_sdhci_data *eswin_sdhci_sdio = dev_get_priv(dev);
	struct sdhci_host *host = &eswin_sdhci_sdio->host;
	int ret;

	host->ioaddr = (void *)dev_read_addr(dev);
	host->mmc = &plat->mmc;
	host->mmc->priv = &eswin_sdhci_sdio->host;
	host->mmc->dev = dev;
	upriv->mmc = host->mmc;

	ret = mmc_of_parse(dev, &plat->cfg);

	if (ret) {
		printf("%s fail to parse\n", __func__);
	}

	//sdhci_wl_pre_init(host);
	ret = sdhci_setup_cfg(&plat->cfg, host, 0, SD_MIN_FREQ);
	//if (ret) return ret;

	//host->ops = &eswin_sd_ops;
	sdhci_wl_phy_poweron(dev);
	return sdhci_probe(dev);
}

static int eswin_sd_ofdata_to_platdata(struct udevice *dev)
{
#if !CONFIG_IS_ENABLED(OF_PLATDATA)
	struct sdhci_host *host = dev_get_priv(dev);

	host->name = dev->name;
	host->ioaddr = dev_read_addr_ptr(dev);
	host->bus_width = dev_read_u32_default(dev, "bus-width", 4);
#endif

	return 0;
}

static int eswin_sd_bind(struct udevice *dev)
{
	struct eswin_sd_plat *plat = dev_get_plat(dev);

	return sdhci_bind(dev, &plat->mmc, &plat->cfg);
}

static const struct udevice_id eswin_wl_ids[] = {{.compatible = "eswin,wl-sdhci"}, {}};

U_BOOT_DRIVER(eswin_wl_drv) = {
	.name       = "eswin_wl_sdhci",
	.id         = UCLASS_MMC,
	.of_match   = eswin_wl_ids,
	.of_to_plat = eswin_sd_ofdata_to_platdata,
	.ops        = &sdhci_ops,
	.bind       = eswin_sd_bind,
	.probe      = eswin_sd_probe,
	.priv_auto  = sizeof(struct eswin_sdhci_data),
	.plat_auto  = sizeof(struct eswin_sd_plat),
};
