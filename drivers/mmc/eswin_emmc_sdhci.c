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
#include <linux/bug.h>
#include <malloc.h>
#include <mapmem.h>
#include <sdhci.h>
#include <clk.h>
#include <reset.h>
#include <mmc.h>
#include <dm/device_compat.h>
#include "eswin_sdhci.h"

/* 400KHz is max freq for card ID etc. Use that as min */
#define EMMC_MIN_FREQ   400000
#define  ESWIN_SDHCI_CTRL_HS400	0x0007 /* Non-standard */

extern int mmc_switch(struct mmc *mmc, u8 set, u8 index, u8 value);

struct eswin_sdhci_plat
{
	struct mmc_config cfg;
	struct mmc mmc;
};

static void eswin_sdhci_coreclk_config(struct sdhci_host *host, unsigned short divisor, unsigned int flag_sel)
{
	u32 val = 0;
	u32 delay = 0xfffff;
	volatile u32 *addr;

	addr = (u32 *)(HOST_DIE_OFFSET(host) + ESWIN_MSHC_CORE_CLK_REG + host->index * 4);

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
	addr = (u32 *)(HOST_DIE_OFFSET(host) + ESWIN_MSHC_CORE_CLK_REG + host->index * 4);

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
			pr_err("%s: Internal clock never stabilised.\n",mmc_hostname(host->mmc));
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
	host->clock = host->mmc->clock;

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

static void eswin_sdhci_set_uhs_timing(struct sdhci_host *host)
{
	struct mmc *mmc = host->mmc;
	u32 reg;

	reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	reg &= ~SDHCI_CTRL_UHS_MASK;

	switch (mmc->selected_mode) {
	case UHS_SDR25:
	case MMC_HS:
		reg |= SDHCI_CTRL_UHS_SDR25;
		break;
	case UHS_SDR50:
	case MMC_HS_52:
		reg |= SDHCI_CTRL_UHS_SDR25;
		break;
	case UHS_DDR50:
	case MMC_DDR_52:
		reg |= SDHCI_CTRL_UHS_DDR50;
		break;
	case UHS_SDR104:
	case MMC_HS_200:
		reg |= SDHCI_CTRL_UHS_SDR104;
		break;
	case MMC_HS_400:
	case MMC_HS_400_ES:
		reg |= ESWIN_SDHCI_CTRL_HS400;
		break;
	default:
		reg |= SDHCI_CTRL_UHS_SDR12;
	}

	sdhci_writew(host, reg, SDHCI_HOST_CONTROL2);
}

void sdhci_do_enable_v4_mode(struct sdhci_host *host)
{
	u16 ctrl2;

	ctrl2 = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	if (ctrl2 & SDHCI_CTRL_V4_MODE)
		return;

	ctrl2 |= SDHCI_CTRL_V4_MODE;
	sdhci_writew(host, ctrl2, SDHCI_HOST_CONTROL2);
}

static void eswin_emmc_set_control_reg(struct sdhci_host *host)
{
	struct mmc *mmc = (struct mmc *)host->mmc;
	u32 reg;

	sdhci_do_enable_v4_mode(host);

	if (mmc->signal_voltage == MMC_SIGNAL_VOLTAGE_180) {
		reg = sdhci_readw(host, SDHCI_HOST_CONTROL2);
		reg |= SDHCI_CTRL_VDD_180;
		sdhci_writew(host, reg, SDHCI_HOST_CONTROL2);
	}

	if ((mmc->selected_mode != MMC_LEGACY))
			eswin_sdhci_set_uhs_timing(host);
}

static int eswin_sdhci_phy_poweron(struct sdhci_host *host)
{
	unsigned long io_mem = (unsigned long)host->ioaddr;
	unsigned long hsp_io_mem = HOST_DIE_OFFSET(host) + HSP_IO_MEM;
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
	addr = (unsigned int *)(HOST_DIE_OFFSET(host) + ESWIN_HSPDMA_RST_CTRL);
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

	sdhci_writew(host, 0xd, VENDOR_EMMC_CTRL_R);
	sdhci_writeb(host, 0x12, VENDOR_MSHC_CTRL_R);

	eswin_sdhci_config_phy(host);

	return 0;
}

static void sdhci_reset(struct sdhci_host *host, u8 mask)
{
	unsigned long timeout;

	/* Wait max 100 ms */
	timeout = 100;
	sdhci_writeb(host, mask, SDHCI_SOFTWARE_RESET);
	while (sdhci_readb(host, SDHCI_SOFTWARE_RESET) & mask) {
		if (timeout == 0) {
			printf("%s: Reset 0x%x never completed.\n",
				   __func__, (int)mask);
			return;
		}
		timeout--;
		udelay(1000);
	}
}

static int eswin_sdhci_phase_code_tuning(struct sdhci_host *host,
						  u32 opcode)
{
	int cmd_error = 0;
	int ret = 0;
	int i = 0;
	int phase_code = -1;
	int phase_range = -1;
	int phase_min = -1;
	int phase_max = -1;

	for (i = 0; i <= MAX_PHASE_CODE;i++) {
		eswin_disable_card_clk(host);
		sdhci_writew(host, i, VENDOR_AT_SATA_R);
		eswin_enable_card_clk(host);

		ret = mmc_send_tuning(host->mmc, opcode, &cmd_error);
		sdhci_reset(host, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
		if (ret) {
			pr_debug("%s: bad phase_code:0x%x!\n", mmc_hostname(host->mmc), i);
			if (phase_min != -1 && phase_max != -1) {
				if (phase_max - phase_min > phase_range) {
					phase_range = phase_max - phase_min;
					phase_code = (phase_min + phase_max) / 2;
					if (phase_range > TUNING_RANGE_THRESHOLD)
						break;
				}
				phase_min = -1;
				phase_max = -1;
			}
		} else {
			pr_debug("%s: ok phase_code:0x%x\n", mmc_hostname(host->mmc), i);
			if (phase_min == -1)
				phase_min = i;
			phase_max = i;
		}
	}

	if (phase_max - phase_min > phase_range)
		phase_code = (phase_min + phase_max) / 2;

	if (phase_code == -1) {
		pr_debug("%s: phase code tuning failed!\n",
			   mmc_hostname(host->mmc));
		eswin_disable_card_clk(host);
		sdhci_writew(host, 0, VENDOR_AT_SATA_R);
		eswin_enable_card_clk(host);
		return -EIO;
	}

	pr_debug("%s: set phase_code:0x%x\n", mmc_hostname(host->mmc), phase_code);

	eswin_disable_card_clk(host);
	sdhci_writew(host, phase_code, VENDOR_AT_SATA_R);
	eswin_enable_card_clk(host);

	ret = mmc_send_tuning(host->mmc, opcode, &cmd_error);
	sdhci_reset(host, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
	if (ret) {
		pr_err("%s: phase_code code(0x%x) not work, tuning failed!\n",
			   mmc_hostname(host->mmc), phase_code);
		return ret;
	}

	return 0;
}


static int eswin_sdhci_sdio_delay_tuning(struct sdhci_host *host, u32 opcode)
{
	int ret;
	int delay = -1;
	int i = 0;
	int delay_min = -1;
	int delay_max = -1;
	int delay_range = -1;
	int cmd_error = 0;
	struct udevice *dev = host->mmc->dev;
	struct eswin_sdhci_data *eswin_sdhci = dev_get_priv(dev);
	struct eswin_sdhci_phy_data *phy = &eswin_sdhci->phy;

	for (i = 0; i <= PHY_DELAY_CODE_MAX;i++) {
		eswin_disable_card_clk(host);
		eswin_sdhci_config_phy_delay(host, i);
		eswin_enable_card_clk(host);
		ret = mmc_send_tuning(host->mmc, opcode, &cmd_error);
		sdhci_reset(host, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
		if (ret) {
			pr_debug("%s: ========bad delay:0x%x!==========\n", mmc_hostname(host->mmc), i);
			if (delay_min != -1 && delay_max != -1) {
				if (delay_max - delay_min > delay_range) {
					delay = (delay_min + delay_max) / 2;
					delay_range = delay_max - delay_min;
					if (delay_range > TUNING_RANGE_THRESHOLD)
						break;
				}
			}
			delay_min = -1;
			delay_max = -1;
		} else {
			pr_debug("%s: ==========ok delay:0x%x===========\n", mmc_hostname(host->mmc), i);
			if (delay_min == -1)
				delay_min = i;

			delay_max = i;
		}
	}

	if (delay_max - delay_min > delay_range)
		delay = (delay_min + delay_max) / 2;

	if (delay == -1) {
		pr_debug("%s: delay code tuning failed!\n",
		       mmc_hostname(host->mmc));
		eswin_disable_card_clk(host);
		eswin_sdhci_config_phy_delay(host, phy->delay_code);
		eswin_enable_card_clk(host);
		return ret;
	}

	pr_debug("%s: set delay:0x%x\n", mmc_hostname(host->mmc), delay);
	eswin_disable_card_clk(host);
	eswin_sdhci_config_phy_delay(host, delay);
	eswin_enable_card_clk(host);

	ret = mmc_send_tuning(host->mmc, opcode, &cmd_error);
	sdhci_reset(host, SDHCI_RESET_CMD | SDHCI_RESET_DATA);
	if (ret) {
		pr_err("%s: delay code(0x%x) not work, tuning failed!\n",
		       mmc_hostname(host->mmc), delay);
		return ret;
	}
	phy->delay_code = delay;
	return 0;
}


int eswin_sdhci_execute_tuning(struct mmc *mmc, u8 opcode)
{
	struct eswin_sdhci_data *eswin_sdhci = dev_get_priv(mmc->dev);
	struct sdhci_host *host = &eswin_sdhci->host;
	u32 ctrl;
	u32 val;
	int ret = 0;

	eswin_disable_card_clk(host);

	ctrl = sdhci_readw(host, SDHCI_HOST_CONTROL2);
	ctrl &= ~SDHCI_CTRL_TUNED_CLK;
	sdhci_writew(host, ctrl, SDHCI_HOST_CONTROL2);

	val = sdhci_readl(host, VENDOR_AT_CTRL_R);
	val |= SW_TUNE_ENABLE;
	sdhci_writew(host, val, VENDOR_AT_CTRL_R);
	sdhci_writew(host, 0, VENDOR_AT_SATA_R);

	eswin_enable_card_clk(host);

	sdhci_writew(host, 0x0, SDHCI_CMD_DATA);
	ret = eswin_sdhci_phase_code_tuning(host, opcode);
	// if(ret)
		// return ret;
	// ret = eswin_sdhci_sdio_delay_tuning(host, opcode);
	return ret;
}

static int eswin_sdhci_config_dll(struct sdhci_host *host, unsigned int clock, bool enable)
{
	struct mmc *mmc = host->mmc;
	u32 val;
	u32 status;
	u32 timeout = 0;

	if (enable && (mmc->selected_mode == MMC_HS_400) && (clock == 200000000)) {
		eswin_disable_card_clk(host);

		val = sdhci_readl(host, VENDOR_AT_CTRL_R);
		val &= ~(LATENCY_LT_MASK << LATENCY_LT_BIT_OFFSET);
		val |= (LATENCY_LT_3 << LATENCY_LT_MASK);
		sdhci_writel(host, val, VENDOR_AT_CTRL_R);

		sdhci_writeb(host, 0x23, PHY_DLL_CNFG1_R);
		sdhci_writeb(host, 0x02, PHY_DLL_CNFG2_R);
		sdhci_writeb(host, 0x60, PHY_DLLDL_CNFG_R);
		sdhci_writeb(host, 0x00, PHY_DLL_OFFST_R);
		sdhci_writew(host, 0xffff, PHY_DLLBT_CNFG_R);

		eswin_enable_card_clk(host);
		sdhci_writeb(host, DLL_ENABEL, PHY_DLL_CTRL_R);
		udelay(100);

		while (1) {
			status = sdhci_readb(host, PHY_DLL_STATUS_R);
			if (status & DLL_LOCK_STS) {
				pr_debug("%s: locked status:0x%x\n", mmc_hostname(host->mmc), status);
				break;
			}
			timeout++;
			udelay(100);
			if (timeout > 10000) {
				pr_err("%s: DLL lock failed!status:0x%x\n",
					mmc_hostname(host->mmc), status);
				return -EIO;
			}
		}

		status = sdhci_readb(host, PHY_DLL_STATUS_R);
		if (status & DLL_ERROR_STS) {
			pr_err("%s: DLL lock failed!err_status:0x%x\n",
				mmc_hostname(host->mmc), status);
		} else {
			pr_debug("%s: DLL lock is success\n", mmc_hostname(host->mmc));
		}
	}

	return 0;
}

static struct sdhci_ops eswin_emmc_ops = {
	.platform_execute_tuning	= &eswin_sdhci_execute_tuning,
	.set_control_reg = &eswin_emmc_set_control_reg,
	.set_clock = &eswin_sdhci_set_clock,
	.set_ios_post = &eswin_sdhci_set_ios_post,
	.config_dll = &eswin_sdhci_config_dll,
};

static void eswin_sdhci_do_reset(struct eswin_sdhci_data *eswin_sdhci)
{
	int ret;

	ret = reset_assert(eswin_sdhci->txrx_rst);
	WARN_ON(0 != ret);
	ret = reset_assert(eswin_sdhci->phy_rst);
	WARN_ON(0 != ret);
	ret = reset_assert(eswin_sdhci->prstn);
	WARN_ON(0 != ret);
	ret = reset_assert(eswin_sdhci->arstn);
	WARN_ON(0 != ret);

	mdelay(2);

	ret = reset_deassert(eswin_sdhci->txrx_rst);
	WARN_ON(0 != ret);
	ret = reset_deassert(eswin_sdhci->phy_rst);
	WARN_ON(0 != ret);
	ret = reset_deassert(eswin_sdhci->prstn);
	WARN_ON(0 != ret);
	ret = reset_deassert(eswin_sdhci->arstn);
	WARN_ON(0 != ret);
}

int eswin_sdhci_reset_init(struct udevice *dev,
				 struct eswin_sdhci_data *eswin_sdhci)
{
	int ret = 0;
	eswin_sdhci->txrx_rst = devm_reset_control_get_optional(dev, "txrx_rst");
	if (IS_ERR_OR_NULL(eswin_sdhci->txrx_rst)) {
		dev_err(dev, "txrx_rst reset not found.\n");
		return -EFAULT;
	}

	eswin_sdhci->phy_rst = devm_reset_control_get_optional(dev, "phy_rst");
	if (IS_ERR_OR_NULL(eswin_sdhci->phy_rst)) {
		dev_err(dev, "phy_rst reset not found.\n");
		return -EFAULT;
	}

	eswin_sdhci->prstn = devm_reset_control_get_optional(dev, "prstn");
	if (IS_ERR_OR_NULL(eswin_sdhci->prstn)) {
		dev_err(dev, "prstn reset not found.\n");
		return -EFAULT;
	}

	eswin_sdhci->arstn = devm_reset_control_get_optional(dev, "arstn");
	if (IS_ERR_OR_NULL(eswin_sdhci->arstn)) {
		dev_err(dev, "arstn reset not found.\n");
		return -EFAULT;
	}
	eswin_sdhci_do_reset(eswin_sdhci);

	return ret;
}

static int eswin_clk_reset_init(struct udevice *dev)
{
	int ret = 0;
	struct eswin_sdhci_data *eswin_sdhci = dev_get_priv(dev);

	eswin_sdhci->aclk = devm_clk_get(dev, "aclk");
	if (IS_ERR(eswin_sdhci->aclk)) {
		dev_err(dev, "aclk clock not found.\n");
		return PTR_ERR(eswin_sdhci->aclk);
	}

	eswin_sdhci->clk_ahb = devm_clk_get(dev, "clk_ahb");
	if (IS_ERR(eswin_sdhci->clk_ahb)) {
		dev_err(dev, "clk_ahb clock not found.\n");
		return PTR_ERR(eswin_sdhci->clk_ahb);
	}

	ret = clk_prepare_enable(eswin_sdhci->aclk);
	if (ret) {
		dev_err(dev, "Unable to enable aclk clock.\n");
		return ret;
	}

	ret = clk_prepare_enable(eswin_sdhci->clk_ahb);
	if (ret) {
		dev_err(dev, "Unable to enable AHB clock.\n");
		return ret;
	}

	ret = eswin_sdhci_reset_init(dev, eswin_sdhci);
	if (ret < 0) {
		dev_err(dev, "failed to reset\n");
		return ret;
	}

    return 0;
}


static int eswin_sdhci_probe(struct udevice *dev)
{
	struct mmc_uclass_priv *upriv = dev_get_uclass_priv(dev);
	struct eswin_sdhci_plat *plat = dev_get_plat(dev);
	struct eswin_sdhci_data *eswin_sdhci = dev_get_priv(dev);
	struct sdhci_host *host = &eswin_sdhci->host;
	int max_frequency, ret;
	unsigned int val = 0;

	ret = dev_read_s32(dev, "numa-node-id", &host->node_id);
	if (ret) {
		dev_err(dev, "%s %d, failed to get node id, ret %d!\r\n", __func__, __LINE__, ret);
		return ret;
	}

	ret = dev_read_s32(dev, "index", &host->index);
	if (ret) {
		dev_err(dev, "%s %d, failed to get index id, ret %d!\r\n", __func__, __LINE__, ret);
		return ret;
	}
	debug("mmc node_id:%d index:%d\n", host->node_id, host->index);

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

	ret = eswin_clk_reset_init(dev);
	if (ret) {
		dev_err(dev, "%s %d, clk reset failed, ret %d!\r\n", __func__, __LINE__, ret);
		return ret;
	}

	host->quirks = SDHCI_QUIRK_WAIT_SEND_CMD;
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
	debug("host->ioaddr:0x%lx\n", (unsigned long)host->ioaddr);

	ret = mmc_of_parse(dev, &plat->cfg);
	if (ret) {
		printf("%s fail to parse\n", __func__);
	}

	eswin_sdhci_pre_init(host);
	ret = sdhci_setup_cfg(&plat->cfg, host, 0, EMMC_MIN_FREQ);
	if (ret)
		return ret;

	host->ops = &eswin_emmc_ops;
	sdhci_do_enable_v4_mode(host);

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
	.name       = "eswin_emmc_sdhci_5_1",
	.id         = UCLASS_MMC,
	.of_match   = eswin_sdhci_ids,
	.of_to_plat = eswin_sdhci_ofdata_to_platdata,
	.ops        = &sdhci_ops,
	.bind       = eswin_sdhci_bind,
	.probe      = eswin_sdhci_probe,
	.priv_auto  = sizeof(struct eswin_sdhci_data),
	.plat_auto  = sizeof(struct eswin_sdhci_plat),
};
