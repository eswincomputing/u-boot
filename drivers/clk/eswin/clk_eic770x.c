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
 * Authors: Huangyifeng <huangifeng@eswincomputing.com>
 */

#include <common.h>
#include <clk-uclass.h>
#include <dm.h>
#include <dt-structs.h>
#include <errno.h>
#include <malloc.h>
#include <mapmem.h>
#include <syscon.h>
#include <asm/io.h>
#include <dt-bindings/clock/eic7x-clock.h>
#include <dt-bindings/reset/eswin,eic7x-syscrg.h>
#include <dm/device-internal.h>
#include <dm/uclass-internal.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/log2.h>
#include <linux/bitfield.h>
#include <linux/io.h>
#include <linux/bitops.h>

#define EIC7X_CLK_SYS_CFG_DEFAULT 		200000000
#define EIC7X_SPLL0_FOUT3_RATE			400000000
#define EIC7X_SPLL0_FOUT2_RATE			800000000
#define EIC7X_VPLL_FOUT1_RATE				1188000000
#define EIC7X_VPLL_FOUT3_RATE				49500000
#define EIC7X_SPLL2_FOUT2_RATE			1040000000
#define EIC7X_SPLL2_FOUT3_RATE			416000000
#define EIC7X_XTAL_24M_RATE				24000000

#define GET_BITFIELD(reg, start, end) \
		(FIELD_GET(GENMASK(start, end), readl(reg)))

#define SET_BITFIELD(reg, start, end, value) do { \
    unsigned int temp = readl(reg); \
    temp &= ~GENMASK(start, end); \
    temp |= FIELD_PREP(GENMASK(start, end), value); \
    writel(temp, reg); \
} while (0)

#define IS_DIVISIBLE(a, b) ((b) == 0 ? 0 : (!((a) % (b))))
#define EIC770X_DIV_TO_RATE(input_rate, div)  ((input_rate) / (((div) == 0 || (div) == 1) ? 1 : (div)))
#define EIC770X_DIV_TO_RATE_2(input_rate, div)  ((input_rate) / (((div) == 0 || (div) == 1) ? 2 : (div)))

/* Private data for the clock driver*/
struct eic770x_clk_priv {
	ulong rate;
	void __iomem *base;
};

static ulong eswin_mmc_get_clk(struct clk *clk)
{
	uint div;
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	void __iomem *reg = 0, *base = priv->base;
	int id = clk->id;
	ulong parent_rate;
	int sel;

	switch (id) {
		case EIC7X_CLK_HSP_MSHC1_CORE_CLK:
			reg = base + EIC7X_REG_OFFSET_MSHC1_CORECLK_CTRL;
			sel = GET_BITFIELD(reg, 0, 0);
			div = GET_BITFIELD(reg, 15, 4);
			if (0 == sel) {
				parent_rate = EIC7X_SPLL0_FOUT3_RATE;
			} else {
				parent_rate = EIC7X_SPLL2_FOUT3_RATE;
			}
			break;
		default:
			printf("%s %d:unsupport clk id %d\n",__func__,__LINE__, id);
			return -EINVAL;
	}
	return EIC770X_DIV_TO_RATE_2(parent_rate, div);
}

static ulong eswin_mmc_set_clk(struct clk *clk, uint freq)
{
	int src_clk_div;
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	void __iomem *reg = 0, *base = priv->base;
	int id = clk->id;
	ulong parent_rate;

	switch (id) {
		case EIC7X_CLK_HSP_MSHC1_CORE_CLK:
			if (IS_DIVISIBLE(EIC7X_SPLL0_FOUT3_RATE, freq)) {
				parent_rate = EIC7X_SPLL0_FOUT3_RATE;
			} else if (IS_DIVISIBLE(EIC7X_SPLL2_FOUT3_RATE, freq)) {
				parent_rate = EIC7X_SPLL2_FOUT3_RATE;
			} else {
				printf("%s %d:unsupport clk frequency %d\n",__func__,__LINE__, freq);
				return -EINVAL;
			}
			reg = base + EIC7X_REG_OFFSET_MSHC1_CORECLK_CTRL;
			if (EIC7X_SPLL0_FOUT3_RATE == parent_rate) {
				SET_BITFIELD(reg, 0, 0, 0);
			} else if (EIC7X_SPLL2_FOUT3_RATE == parent_rate) {
				SET_BITFIELD(reg, 0, 0, 1);
			}
			src_clk_div = DIV_ROUND_UP(parent_rate , freq);
			SET_BITFIELD(reg, 15, 4, src_clk_div);
			break;
		default:
			printf("%s %d:unsupport clk id %d\n",__func__,__LINE__, id);
			return -EINVAL;
	}
	return 0;
}

static ulong eswin_mipi_get_clk(struct clk *clk)
{
	uint div;
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	void __iomem *reg = 0, *base = priv->base;
	int id = clk->id;
	ulong parent_rate;
	int sel;

	switch (id) {
		case EIC7X_CLK_VO_PIXEL_CLK:
			reg = base + EIC7X_REG_OFFSET_VO_PIXEL_CTRL;
			sel = GET_BITFIELD(reg, 0, 0);
			div = GET_BITFIELD(reg, 9, 4);
			if (0 == sel) {
				parent_rate = EIC7X_VPLL_FOUT1_RATE;
			} else {
				parent_rate = EIC7X_SPLL2_FOUT2_RATE;
			}
			break;
		default:
			printf("%s %d:unsupport clk id %d\n",__func__,__LINE__, id);
			return -EINVAL;
	}
	return EIC770X_DIV_TO_RATE_2(parent_rate, div);
}

static ulong eswin_mipi_set_clk(struct clk *clk, uint freq)
{
	int src_clk_div;
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	void __iomem *reg = 0, *base = priv->base;
	int id = clk->id;
	ulong parent_rate;

	switch (id) {
		case EIC7X_CLK_VO_PIXEL_CLK:
			if (IS_DIVISIBLE (EIC7X_VPLL_FOUT1_RATE, freq)) {
				parent_rate = EIC7X_VPLL_FOUT1_RATE;
			} else if (IS_DIVISIBLE(EIC7X_SPLL2_FOUT2_RATE, freq)) {
				parent_rate = EIC7X_SPLL2_FOUT2_RATE;
			} else {
				printf("%s %d:unsupport clk frequency %d\n",__func__,__LINE__, freq);
				return -EINVAL;
			}
			reg = base + EIC7X_REG_OFFSET_VO_PIXEL_CTRL;
			if (EIC7X_VPLL_FOUT1_RATE == parent_rate) {
				SET_BITFIELD(reg, 0, 0, 0);
			} else if (EIC7X_SPLL2_FOUT2_RATE == parent_rate) {
				SET_BITFIELD(reg, 0, 0, 1);
			}
			src_clk_div = DIV_ROUND_UP(parent_rate , freq);
			SET_BITFIELD(reg, 9, 4, src_clk_div);
			break;
		default:
			printf("%s %d:unsupport clk id %d\n",__func__,__LINE__, id);
			return -EINVAL;
	}
	return 0;
}

static ulong eswin_boospi_set_clk(struct clk *clk, uint freq)
{
	int src_clk_div;
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	void __iomem *reg = 0, *base = priv->base;
	int id = clk->id;
	ulong parent_rate;

	switch (id) {
		case EIC7X_CLK_CLK_BOOTSPI:
			if (IS_DIVISIBLE(EIC7X_SPLL0_FOUT2_RATE, freq)) {
				parent_rate = EIC7X_SPLL0_FOUT2_RATE;
			} else if (freq == EIC7X_XTAL_24M_RATE) {
				parent_rate = EIC7X_XTAL_24M_RATE;
			} else {
				printf("%s %d:unsupport clk frequency %d\n",__func__,__LINE__, freq);
				return -EINVAL;
			}
			reg = base + EIC7X_REG_OFFSET_BOOTSPI_CLK_CTRL;
			if (EIC7X_SPLL0_FOUT2_RATE == parent_rate) {
				SET_BITFIELD(reg, 0, 0, 0);
				src_clk_div = DIV_ROUND_UP(parent_rate , freq);
				SET_BITFIELD(reg, 9, 4, src_clk_div);
			} else if (EIC7X_XTAL_24M_RATE == parent_rate) {
				SET_BITFIELD(reg, 0, 0, 1);
			}
			break;
		default:
			printf("%s %d:unsupport clk id %d\n",__func__,__LINE__, id);
			return -EINVAL;
	}
	return 0;
}

static ulong eic770x_clk_get_sys_cfg_rate(struct clk *clk)
{
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	void __iomem *reg = 0, *base = priv->base;
	int mux;
	ulong rate;
	uint div;

	reg = base + EIC7X_REG_OFFSET_SYSCFG_CLK_CTRL;
	mux = GET_BITFIELD(reg, 0, 0);
	switch(mux) {
		case 0:
			reg = base + EIC7X_REG_OFFSET_SYSCFG_CLK_CTRL;
			div = GET_BITFIELD(reg, 6, 4);
			rate = EIC770X_DIV_TO_RATE_2(EIC7X_SPLL0_FOUT3_RATE, div);
			break;
		case 1:
			rate = EIC7X_XTAL_24M_RATE;
			break;
		default:
			return -EINVAL;
	}
	return rate;
}

static ulong eic770x_clk_get_bootspi_rate(struct clk *clk)
{
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	void __iomem *reg = 0, *base = priv->base;
	int mux;
	ulong rate;
	uint div;

	reg = base + EIC7X_REG_OFFSET_BOOTSPI_CLK_CTRL;
	mux = GET_BITFIELD(reg, 0, 0);
	switch(mux) {
		case 0:
			reg = base + EIC7X_REG_OFFSET_BOOTSPI_CLK_CTRL;
			div = GET_BITFIELD(reg, 9, 4);
			rate = EIC770X_DIV_TO_RATE_2(EIC7X_SPLL0_FOUT2_RATE, div);
			break;
		case 1:
			rate = EIC7X_XTAL_24M_RATE;
			break;
		default:
			return -EINVAL;
	}
	return rate;
}

static ulong eic770x_clk_get_rate(struct clk *clk)
{
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	int id = clk->id;
	ulong rate = -1;

	switch (id) {
		case EIC7X_CLK_LSP_I2C0_PCLK:
		case EIC7X_CLK_LSP_I2C1_PCLK:
		case EIC7X_CLK_LSP_I2C2_PCLK:
		case EIC7X_CLK_LSP_I2C3_PCLK:
		case EIC7X_CLK_LSP_I2C4_PCLK:
		case EIC7X_CLK_LSP_I2C5_PCLK:
		case EIC7X_CLK_LSP_I2C6_PCLK:
		case EIC7X_CLK_LSP_I2C7_PCLK:
		case EIC7X_CLK_LSP_I2C8_PCLK:
		case EIC7X_CLK_LSP_I2C9_PCLK:
		case EIC7X_CLK_AON_I2C0_PCLK:
		case EIC7X_CLK_AON_I2C1_PCLK:
		case EIC7X_CLK_HSP_CFG_CLK:
			rate = eic770x_clk_get_sys_cfg_rate(clk);
			break;
		case EIC7X_CLK_HSP_MSHC1_CORE_CLK:
			rate = eswin_mmc_get_clk(clk);
			break;
		case EIC7X_CLK_VO_PIXEL_CLK:
			rate = eswin_mipi_get_clk(clk);
			break;
		//timer
		case EIC7X_CLK_TIMER_CLK_0:
		case EIC7X_CLK_TIMER_CLK_1:
		case EIC7X_CLK_TIMER_CLK_2:
		case EIC7X_CLK_TIMER_CLK_3:
			rate = EIC7X_XTAL_24M_RATE;
			break;
		case EIC7X_CLK_TIMER_PCLK_0:
		case EIC7X_CLK_TIMER_PCLK_1:
		case EIC7X_CLK_TIMER_PCLK_2:
		case EIC7X_CLK_TIMER_PCLK_3:
			rate = eic770x_clk_get_sys_cfg_rate(clk);
			break;
		case EIC7X_GATE_TIMER3_CLK8:
			rate = EIC7X_VPLL_FOUT3_RATE;
			break;
		//uart
		case EIC7X_CLK_LSP_UART0_PCLK:
		case EIC7X_CLK_LSP_UART1_PCLK:
		case EIC7X_CLK_LSP_UART2_PCLK:
		case EIC7X_CLK_LSP_UART3_PCLK:
		case EIC7X_CLK_LSP_UART4_PCLK:
			rate = eic770x_clk_get_sys_cfg_rate(clk);
			break;
		case EIC7X_CLK_CLK_BOOTSPI:
			rate = eic770x_clk_get_bootspi_rate(clk);
			break;
		default:
			printf("%s %d:unsupport clk id %d\n",__func__,__LINE__, id);
			return -EINVAL;
	}
	return rate;
}

static ulong eic770x_clk_set_rate(struct clk *clk, ulong rate)
{
	ulong new_rate;
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);

	switch (clk->id) {
		case EIC7X_CLK_HSP_MSHC1_CORE_CLK:
			new_rate = eswin_mmc_set_clk(clk, rate);
			break;
		case EIC7X_CLK_VO_PIXEL_CLK:
			new_rate = eswin_mipi_set_clk(clk, rate);
			break;
		case EIC7X_CLK_CLK_BOOTSPI:
			new_rate = eswin_boospi_set_clk(clk, rate);
			break;
		default:
			return -ENOENT;
	}
	return new_rate;
}

static int eic770x_clk_enable(struct clk *clk)
{
	struct eic770x_clk_priv *priv = dev_get_priv(clk->dev);
	int id = clk->id;
	void __iomem *base = priv->base;
	int ret = 0;

	switch(id) {
		case EIC7X_CLK_LSP_I2C0_PCLK:
			__set_bit(7, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C1_PCLK:
			__set_bit(8, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C2_PCLK:
			__set_bit(9, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C3_PCLK:
			__set_bit(10, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C4_PCLK:
			__set_bit(11, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C5_PCLK:
			__set_bit(12, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C6_PCLK:
			__set_bit(13, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C7_PCLK:
			__set_bit(14, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C8_PCLK:
			__set_bit(15, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_I2C9_PCLK:
			__set_bit(16, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_AON_I2C0_PCLK:
			__set_bit(31, base + EIC7X_REG_OFFSET_I2C0_CLK_CTRL);
			break;
		case EIC7X_CLK_AON_I2C1_PCLK:
			__set_bit(31, base + EIC7X_REG_OFFSET_I2C1_CLK_CTRL);
			break;

		//mmc/sdio/sata
		case EIC7X_CLK_HSP_CFG_CLK:
			__set_bit(31, base + EIC7X_REG_OFFSET_HSP_CFG_CTRL);
			__set_bit(30, base + EIC7X_REG_OFFSET_HSP_CFG_CTRL);
			break;
		case EIC7X_CLK_HSP_MSHC1_CORE_CLK:
			__set_bit(16, base + EIC7X_REG_OFFSET_MSHC1_CORECLK_CTRL);
			break;
		//dsi
		case EIC7X_CLK_CLK_MIPI_TXESC:
			break;
		//vo
		case EIC7X_CLK_VO_CFG_CLK:
			__set_bit(30, base + EIC7X_REG_OFFSET_VO_ACLK_CTRL);
			break;
		case EIC7X_CLK_VO_PIXEL_CLK:
			__set_bit(31, base + EIC7X_REG_OFFSET_VO_PIXEL_CTRL);
			break;
		case EIC7X_CLK_VO_ACLK:
			__set_bit(31, base + EIC7X_REG_OFFSET_VO_ACLK_CTRL);
			break;
		//eth
		case EIC7X_CLK_HSP_ETH0_CORE_CLK:
			__set_bit(0, base + EIC7X_REG_OFFSET_ETH0_CTRL);
			break;
		case EIC7X_CLK_HSP_ETH_APP_CLK:
			break;
		case EIC7X_CLK_HSP_ETH_CSR_CLK:
			break;
		//hdmi
		case EIC7X_CLK_VO_CEC_CLK:
			break;
		case EIC7X_CLK_VO_CR_CLK:
			__set_bit(1, base + EIC7X_REG_OFFSET_VO_PHY_CLKCTRL);
			break;
		//timer
		case EIC7X_CLK_TIMER_CLK_0:
			__set_bit(0, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_CLK_TIMER_CLK_1:
			__set_bit(1, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_CLK_TIMER_CLK_2:
			__set_bit(2, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_CLK_TIMER_CLK_3:
			__set_bit(3, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_CLK_TIMER_PCLK_0:
			__set_bit(4, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_CLK_TIMER_PCLK_1:
			__set_bit(5, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_CLK_TIMER_PCLK_2:
			__set_bit(6, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_CLK_TIMER_PCLK_3:
			__set_bit(7, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		case EIC7X_GATE_TIMER3_CLK8:
			__set_bit(8, base + EIC7X_REG_OFFSET_TIMER_CLK_CTRL);
			break;
		//uart
		case EIC7X_CLK_LSP_UART0_PCLK:
			__set_bit(17, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_UART1_PCLK:
			__set_bit(18, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_UART2_PCLK:
			__set_bit(19, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_UART3_PCLK:
			__set_bit(20, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		case EIC7X_CLK_LSP_UART4_PCLK:
			__set_bit(21, base + EIC7X_REG_OFFSET_LSP_CLK_EN0);
			break;
		//pcie
		case EIC7X_CLK_PCIET_ACLK:
			__set_bit(31, base + EIC7X_REG_OFFSET_PCIE_ACLK_CTRL);
			break;
		case EIC7X_CLK_PCIET_CR_CLK:
			__set_bit(0, base + EIC7X_REG_OFFSET_PCIE_CFG_CTRL);
			break;
		case EIC7X_CLK_PCIET_AUX_CLK:
			__set_bit(1, base + EIC7X_REG_OFFSET_PCIE_CFG_CTRL);
			break;
		case EIC7X_CLK_PCIET_CFG_CLK:
			__set_bit(31, base + EIC7X_REG_OFFSET_PCIE_CFG_CTRL);
		case EIC7X_CLK_CLK_BOOTSPI:
			__set_bit(31, base + EIC7X_REG_OFFSET_BOOTSPI_CLK_CTRL);
			break;
		default:
			printf("%s %d:unsupport clk id %d\n",__func__,__LINE__, id);
			return -EINVAL;
	}
	return ret;
}

static struct clk_ops eic770x_clk_ops = {
	.get_rate	= eic770x_clk_get_rate,
	.set_rate	= eic770x_clk_set_rate,
	.enable 	= eic770x_clk_enable,
};

static int eic770x_clk_probe(struct udevice *dev)
{
	struct eic770x_clk_priv *priv = dev_get_priv(dev);
	fdt_addr_t addr;
	fdt_size_t size;

	addr = dev_read_addr_size(dev, &size);
	if (addr == FDT_ADDR_T_NONE)
		return -EINVAL;

	priv->base = ioremap(addr, size);
	return 0;
}

static int eic770x_clk_bind(struct udevice *dev)
{
	return 0;
}

static const struct udevice_id eic770x_clk_ids[] = {
	{ .compatible = "eswin,eic770x-cru", },
	{ }
};

U_BOOT_DRIVER(eswin_eic770x_cru) = {
	.name			= "eswin_eic770x_cru",
	.id			= UCLASS_CLK,
	.of_match		= eic770x_clk_ids,
	.priv_auto	= sizeof(struct eic770x_clk_priv),
	.ops			= &eic770x_clk_ops,
	.bind			= eic770x_clk_bind,
	.probe			= eic770x_clk_probe,
};
