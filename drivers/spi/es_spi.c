// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN Boot SPI Flash driver
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

#define LOG_CATEGORY UCLASS_SPI
#include <common.h>
#include <clk.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <dm/device-internal.h>
#include <errno.h>
#include <fdtdec.h>
#include <log.h>
#include <malloc.h>
#include <reset.h>
#include <spi.h>
#include <spi_flash.h>
#include <spi-mem.h>
#include <asm/io.h>
#include <asm-generic/gpio.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/compat.h>
#include <linux/iopoll.h>
#include <linux/sizes.h>
#include <linux/mtd/spi-nor.h>
#include <command.h>
#include <asm/cache.h>

/* dma init */
#define CHAN_OFFSET 		0x100

/* Common registers offset */
#define DMAC_ID			0x000 /* R DMAC ID */
#define DMAC_COMPVER		0x008 /* R DMAC Component Version */
#define DMAC_CFG		0x010 /* R/W DMAC Configuration */
#define DMAC_CHEN		0x018 /* R/W DMAC Channel Enable */
#define DMAC_CHEN_L		0x018 /* R/W DMAC Channel Enable 00-31 */
#define DMAC_CHEN_H		0x01C /* R/W DMAC Channel Enable 32-63 */
#define DMAC_INTSTATUS		0x030 /* R DMAC Interrupt Status */
#define DMAC_COMMON_INTCLEAR	0x038 /* W DMAC Interrupt Clear */
#define DMAC_COMMON_INTSTATUS_ENA 0x040 /* R DMAC Interrupt Status Enable */
#define DMAC_COMMON_INTSIGNAL_ENA 0x048 /* R/W DMAC Interrupt Signal Enable */
#define DMAC_COMMON_INTSTATUS	0x050 /* R DMAC Interrupt Status */
#define DMAC_RESET		0x058 /* R DMAC Reset Register1 */

/* DMA channel registers offset */
#define CH_SAR			0x100 /* R/W Chan Source Address */
#define CH_DAR			0x108 /* R/W Chan Destination Address */
#define CH_BLOCK_TS		0x110 /* R/W Chan Block Transfer Size */
#define CH_CTL			0x118 /* R/W Chan Control */
#define CH_CTL_L		0x118 /* R/W Chan Control 00-31 */
#define CH_CTL_H		0x11C /* R/W Chan Control 32-63 */
#define CH_CFG			0x120 /* R/W Chan Configuration */
#define CH_CFG_L		0x120 /* R/W Chan Configuration 00-31 */
#define CH_CFG_H		0x124 /* R/W Chan Configuration 32-63 */
#define CH_LLP			0x128 /* R/W Chan Linked List Pointer */
#define CH_STATUS		0x130 /* R Chan Status */
#define CH_SWHSSRC		0x138 /* R/W Chan SW Handshake Source */
#define CH_SWHSDST		0x140 /* R/W Chan SW Handshake Destination */
#define CH_BLK_TFR_RESUMEREQ	0x148 /* W Chan Block Transfer Resume Req */
#define CH_AXI_ID		0x150 /* R/W Chan AXI ID */
#define CH_AXI_QOS		0x158 /* R/W Chan AXI QOS */
#define CH_SSTAT		0x160 /* R Chan Source Status */
#define CH_DSTAT		0x168 /* R Chan Destination Status */
#define CH_SSTATAR		0x170 /* R/W Chan Source Status Fetch Addr */
#define CH_DSTATAR		0x178 /* R/W Chan Destination Status Fetch Addr */
#define CH_INTSTATUS_ENA	0x180 /* R/W Chan Interrupt Status Enable */
#define CH_INTSTATUS		0x188 /* R/W Chan Interrupt Status */
#define CH_INTSIGNAL_ENA	0x190 /* R/W Chan Interrupt Signal Enable */
#define CH_INTCLEAR		0x198 /* W Chan Interrupt Clear */

/* DMAC_CFG */
#define DMAC_EN_POS			0
#define DMAC_EN_MASK			(0x1UL<<DMAC_EN_POS)

#define INT_EN_POS			1
#define INT_EN_MASK			(0x1UL<<INT_EN_POS)

#define DMAC_CHAN_EN_SHIFT		0
#define DMAC_CHAN_EN_WE_SHIFT		8
#define	DMAC_CHAN_EN2_WE_SHIFT		16

#define DMAC_CHAN_SUSP_SHIFT		16
#define DMAC_CHAN_SUSP_WE_SHIFT		24

/* CH_CTL_H */
#define CH_CTL_H_ARLEN_EN		(0x1UL<<6)
#define CH_CTL_H_ARLEN_POS		7
#define CH_CTL_H_AWLEN_EN		(0x1UL<<15)
#define CH_CTL_H_AWLEN_POS		16

enum {
	DWAXIDMAC_ARWLEN_1			= 0,
	DWAXIDMAC_ARWLEN_2			= 1,
	DWAXIDMAC_ARWLEN_4			= 3,
	DWAXIDMAC_ARWLEN_8			= 7,
	DWAXIDMAC_ARWLEN_16			= 15,
	DWAXIDMAC_ARWLEN_32			= 31,
	DWAXIDMAC_ARWLEN_64			= 63,
	DWAXIDMAC_ARWLEN_128		= 127,
	DWAXIDMAC_ARWLEN_256		= 255,
	DWAXIDMAC_ARWLEN_MIN		= DWAXIDMAC_ARWLEN_1,
	DWAXIDMAC_ARWLEN_MAX		= DWAXIDMAC_ARWLEN_256
};

#define CH_CTL_H_LLI_LAST		(0x1UL<<30)
#define CH_CTL_H_LLI_VALID		(0x1UL<<31)

/* CH_CTL_L */
#define CH_CTL_L_LAST_WRITE_EN		(0x1UL<<30)

#define CH_CTL_L_DST_MSIZE_POS		18
#define CH_CTL_L_SRC_MSIZE_POS		14

enum dma_burst_length{
	DWAXIDMAC_BURST_TRANS_LEN_1	= 0,
	DWAXIDMAC_BURST_TRANS_LEN_4,
	DWAXIDMAC_BURST_TRANS_LEN_8,
	DWAXIDMAC_BURST_TRANS_LEN_16,
	DWAXIDMAC_BURST_TRANS_LEN_32,
	DWAXIDMAC_BURST_TRANS_LEN_64,
	DWAXIDMAC_BURST_TRANS_LEN_128,
	DWAXIDMAC_BURST_TRANS_LEN_256,
	DWAXIDMAC_BURST_TRANS_LEN_512,
	DWAXIDMAC_BURST_TRANS_LEN_1024
};

#define CH_CTL_L_DST_WIDTH_POS		11
#define CH_CTL_L_SRC_WIDTH_POS		8

enum dma_transfer_width{
	DWAXIDMAC_TRANS_WIDTH_8		= 0,
	DWAXIDMAC_TRANS_WIDTH_16,
	DWAXIDMAC_TRANS_WIDTH_32,
	DWAXIDMAC_TRANS_WIDTH_64,
	DWAXIDMAC_TRANS_WIDTH_128,
	DWAXIDMAC_TRANS_WIDTH_256,
	DWAXIDMAC_TRANS_WIDTH_512,
	DWAXIDMAC_TRANS_WIDTH_MAX	= DWAXIDMAC_TRANS_WIDTH_512
};

#define CH_CTL_L_DST_INC_POS		6
#define CH_CTL_L_SRC_INC_POS		4
enum dma_increment {
	DWAXIDMAC_CH_CTL_L_INC		= 0,
	DWAXIDMAC_CH_CTL_L_NOINC
};

#define CH_CTL_L_DST_MASTET		2
#define CH_CTL_L_SRC_MASTET		0

enum dma_mast {
	DWAXIDMAC_CH_CTL_L_MASTER0		= 0,
	DWAXIDMAC_CH_CTL_L_MASTER1
};

/* CH_CFG_H */
#define CH_CFG_H_PRIORITY_POS		17

#define CH_CFG_H_DST_PER			12
#define CH_CFG_H_SRC_PER			7

enum dma_hw_hs_port{
	DWAXIDMAC_HS_PORT_UART1RX	= 32,
	DWAXIDMAC_HS_PORT_UART1TX	= 33,
	DWAXIDMAC_HS_PORT_UART0RX	= 34,
	DWAXIDMAC_HS_PORT_UART0TX	= 35,
	DWAXIDMAC_HS_PORT_I2C0TX,
	DWAXIDMAC_HS_PORT_I2C0RX,
	DWAXIDMAC_HS_PORT_I2C1TX,
	DWAXIDMAC_HS_PORT_I2C1RX,
	DWAXIDMAC_HS_PORT_BOOTSPI_REQ = 40
};


#define CH_CFG_H_HS_SEL_DST_POS		4
#define CH_CFG_H_HS_SEL_SRC_POS		3
enum dma_handshake_select{
	DWAXIDMAC_HS_SEL_HW		= 0,
	DWAXIDMAC_HS_SEL_SW
};

#define CH_CFG_H_TT_FC_POS		0
enum dma_transfer_direction{
	DWAXIDMAC_TT_FC_MEM_TO_MEM_DMAC	= 0,
	DWAXIDMAC_TT_FC_MEM_TO_PER_DMAC,
	DWAXIDMAC_TT_FC_PER_TO_MEM_DMAC,
	DWAXIDMAC_TT_FC_PER_TO_PER_DMAC,
	DWAXIDMAC_TT_FC_PER_TO_MEM_SRC,
	DWAXIDMAC_TT_FC_PER_TO_PER_SRC,
	DWAXIDMAC_TT_FC_MEM_TO_PER_DST,
	DWAXIDMAC_TT_FC_PER_TO_PER_DST
};


/* CH_CFG2 */
#define CH_CFG2_L_SRC_PER_POS		4
#define CH_CFG2_L_DST_PER_POS		11

#define CH_CFG2_H_TT_FC_POS		0
#define CH_CFG2_H_HS_SEL_SRC_POS	3
#define CH_CFG2_H_HS_SEL_DST_POS	4
#define CH_CFG2_H_PRIORITY_POS		15
#define CH_CFG2_H_SRC_OSR_LMT_POS	23
#define CH_CFG2_H_DST_OSR_LMT_POS	27


#define CH_CFG_L_DST_MULTBLK_TYPE_POS	2
#define CH_CFG_L_SRC_MULTBLK_TYPE_POS	0


enum dma_mblk_type{
	DWAXIDMAC_MBLK_TYPE_CONTIGUOUS	= 0,
	DWAXIDMAC_MBLK_TYPE_RELOAD,
	DWAXIDMAC_MBLK_TYPE_SHADOW_REG,
	DWAXIDMAC_MBLK_TYPE_LL
};

/**
 * DW AXI DMA channel interrupts
 *
 * @DWAXIDMAC_IRQ_NONE: Bitmask of no one interrupt
 * @DWAXIDMAC_IRQ_BLOCK_TRF: Block transfer complete
 * @DWAXIDMAC_IRQ_DMA_TRF: Dma transfer complete
 * @DWAXIDMAC_IRQ_SRC_TRAN: Source transaction complete
 * @DWAXIDMAC_IRQ_DST_TRAN: Destination transaction complete
 * @DWAXIDMAC_IRQ_SRC_DEC_ERR: Source decode error
 * @DWAXIDMAC_IRQ_DST_DEC_ERR: Destination decode error
 * @DWAXIDMAC_IRQ_SRC_SLV_ERR: Source slave error
 * @DWAXIDMAC_IRQ_DST_SLV_ERR: Destination slave error
 * @DWAXIDMAC_IRQ_LLI_RD_DEC_ERR: LLI read decode error
 * @DWAXIDMAC_IRQ_LLI_WR_DEC_ERR: LLI write decode error
 * @DWAXIDMAC_IRQ_LLI_RD_SLV_ERR: LLI read slave error
 * @DWAXIDMAC_IRQ_LLI_WR_SLV_ERR: LLI write slave error
 * @DWAXIDMAC_IRQ_INVALID_ERR: LLI invalid error or Shadow register error
 * @DWAXIDMAC_IRQ_MULTIBLKTYPE_ERR: Slave Interface Multiblock type error
 * @DWAXIDMAC_IRQ_DEC_ERR: Slave Interface decode error
 * @DWAXIDMAC_IRQ_WR2RO_ERR: Slave Interface write to read only error
 * @DWAXIDMAC_IRQ_RD2RWO_ERR: Slave Interface read to write only error
 * @DWAXIDMAC_IRQ_WRONCHEN_ERR: Slave Interface write to channel error
 * @DWAXIDMAC_IRQ_SHADOWREG_ERR: Slave Interface shadow reg error
 * @DWAXIDMAC_IRQ_WRONHOLD_ERR: Slave Interface hold error
 * @DWAXIDMAC_IRQ_LOCK_CLEARED: Lock Cleared Status
 * @DWAXIDMAC_IRQ_SRC_SUSPENDED: Source Suspended Status
 * @DWAXIDMAC_IRQ_SUSPENDED: Channel Suspended Status
 * @DWAXIDMAC_IRQ_DISABLED: Channel Disabled Status
 * @DWAXIDMAC_IRQ_ABORTED: Channel Aborted Status
 * @DWAXIDMAC_IRQ_ALL_ERR: Bitmask of all error interrupts
 * @DWAXIDMAC_IRQ_ALL: Bitmask of all interrupts
 */
enum {
	DWAXIDMAC_IRQ_NONE		= 0,
	DWAXIDMAC_IRQ_BLOCK_TRF		= (0x1UL<<0),
	DWAXIDMAC_IRQ_DMA_TRF		= (0x1UL<<1),
	DWAXIDMAC_IRQ_SRC_TRAN		= (0x1UL<<3),
	DWAXIDMAC_IRQ_DST_TRAN		= (0x1UL<<4),
	DWAXIDMAC_IRQ_SRC_DEC_ERR	= (0x1UL<<5),
	DWAXIDMAC_IRQ_DST_DEC_ERR	= (0x1UL<<6),
	DWAXIDMAC_IRQ_SRC_SLV_ERR	= (0x1UL<<7),
	DWAXIDMAC_IRQ_DST_SLV_ERR	= (0x1UL<<8),
	DWAXIDMAC_IRQ_LLI_RD_DEC_ERR	= (0x1UL<<9),
	DWAXIDMAC_IRQ_LLI_WR_DEC_ERR	= (0x1UL<<10),
	DWAXIDMAC_IRQ_LLI_RD_SLV_ERR	= (0x1UL<<11),
	DWAXIDMAC_IRQ_LLI_WR_SLV_ERR	= (0x1UL<<12),
	DWAXIDMAC_IRQ_INVALID_ERR	= (0x1UL<<13),
	DWAXIDMAC_IRQ_MULTIBLKTYPE_ERR	= (0x1UL<<14),
	DWAXIDMAC_IRQ_DEC_ERR		= (0x1UL<<16),
	DWAXIDMAC_IRQ_WR2RO_ERR		= (0x1UL<<17),
	DWAXIDMAC_IRQ_RD2RWO_ERR	= (0x1UL<<18),
	DWAXIDMAC_IRQ_WRONCHEN_ERR	= (0x1UL<<19),
	DWAXIDMAC_IRQ_SHADOWREG_ERR	= (0x1UL<<20),
	DWAXIDMAC_IRQ_WRONHOLD_ERR	= (0x1UL<<21),
	DWAXIDMAC_IRQ_LOCK_CLEARED	= (0x1UL<<27),
	DWAXIDMAC_IRQ_SRC_SUSPENDED	= (0x1UL<<28),
	DWAXIDMAC_IRQ_SUSPENDED		= (0x1UL<<29),
	DWAXIDMAC_IRQ_DISABLED		= (0x1UL<<30),
	DWAXIDMAC_IRQ_ABORTED		= (0x1UL<<31),
	DWAXIDMAC_IRQ_ALL_ERR		= ((0x3ffUL << 5) | ( 0x3fUL<< 16)),//(GENMASK(21, 16) | GENMASK(14, 5)),
	DWAXIDMAC_IRQ_ALL		= (0xffffffffUL << 0)//GENMASK(31, 0)
};



struct dma_slave_config {
	enum dma_transfer_direction direction;
    unsigned long src_addr;
    unsigned long dst_addr;
    unsigned int chan_priority;
    unsigned int chan_block_ts;
    enum dma_increment src_inc;
    enum dma_increment dst_inc;
    enum dma_mblk_type src_mblock_type;
    enum dma_mblk_type dst_mblock_type;
    enum dma_transfer_width src_ts_width;
    enum dma_transfer_width dst_ts_width;
    enum dma_burst_length src_burst;
    enum dma_burst_length dst_burst;
	enum dma_handshake_select src_hs_sel;
	enum dma_handshake_select dst_hs_sel;
	enum dma_hw_hs_port src_hs_port;
	enum dma_hw_hs_port dst_hs_port;
};


struct dw_axi_dma {
	void __iomem	*dma_base;
	unsigned int	chan_id;
	struct dma_slave_config	config;
};

void axi_dma_iowrite32(struct dw_axi_dma *dw, unsigned int reg, unsigned int val)
{
	writel(val, (void *)(dw->dma_base + reg));
}

unsigned int axi_dma_ioread32(struct dw_axi_dma *dw, unsigned int reg)
{
	return readl((void *)(dw->dma_base + reg));
}

void axi_chan_iowrite32(struct dw_axi_dma *dw, unsigned int reg, unsigned int val)
{
	writel(val, (void *)(dw->dma_base + reg + CHAN_OFFSET * dw->chan_id));
}

unsigned int axi_chan_ioread32(struct dw_axi_dma *dw, unsigned int reg)
{
	return readl((void *)(dw->dma_base + reg + CHAN_OFFSET*dw->chan_id));
}

void axi_chan_iowrite64(struct dw_axi_dma *dw, unsigned int reg, unsigned long val)
{
	/*
	 * We split one 64 bit write for two 32 bit write as some HW doesn't
	 * support 64 bit access.
	 */
	writel(lower_32_bits(val), (void *)(dw->dma_base + reg + CHAN_OFFSET*dw->chan_id));
	writel(upper_32_bits(val), (void *)(dw->dma_base + reg + CHAN_OFFSET*dw->chan_id + 4));
}

unsigned long axi_chan_ioread64(struct dw_axi_dma *dw, unsigned int reg, unsigned long val)
{
	/*
	 * We split one 64 bit write for two 32 bit write as some HW doesn't
	 * support 64 bit access.
	 */
	unsigned long reg_data;
	reg_data = readl((void *)(dw->dma_base + reg + CHAN_OFFSET * dw->chan_id));
	reg_data |= ((unsigned long)readl((void *)(dw->dma_base + reg + CHAN_OFFSET * dw->chan_id + 4)) << 32);
	return reg_data;
}


unsigned int axi_dma_get_irq_state(struct dw_axi_dma *dw)
{
	return readl((void *)(dw->dma_base + DMAC_INTSTATUS));
}

unsigned long axi_chan_get_irq_state(struct dw_axi_dma *dw)
{
	unsigned long reg_data;
	reg_data = readl((void *)(dw->dma_base + CH_INTSTATUS + CHAN_OFFSET * dw->chan_id));
	reg_data |= ((unsigned long)readl((void *)(dw->dma_base + CH_INTSTATUS + CHAN_OFFSET * dw->chan_id + 4)) << 32);
	return reg_data;
}

void axi_chan_irq_clear(struct dw_axi_dma *dw, unsigned int irq_mask)
{
	writel(irq_mask, (void *)(dw->dma_base + CH_INTCLEAR + CHAN_OFFSET * dw->chan_id));
}

void axi_dma_disable(struct dw_axi_dma *dw)
{
	unsigned int val;

	val = axi_dma_ioread32(dw, DMAC_CFG);
	val &= ~DMAC_EN_MASK;
	axi_dma_iowrite32(dw, DMAC_CFG, val);
}

void axi_dma_enable(struct dw_axi_dma *dw)
{
	unsigned int val;

	val = axi_dma_ioread32(dw, DMAC_CFG);

	val |= DMAC_EN_MASK;
	axi_dma_iowrite32(dw, DMAC_CFG, val);
}

void axi_dma_irq_disable(struct dw_axi_dma *dw)
{
	unsigned int val;

	val = axi_dma_ioread32(dw, DMAC_CFG);
	val &= ~INT_EN_MASK;
	axi_dma_iowrite32(dw, DMAC_CFG, val);
}

void axi_dma_irq_enable(struct dw_axi_dma *dw)
{
	unsigned int val;

	val = axi_dma_ioread32(dw, DMAC_CFG);
	val |= INT_EN_MASK;
	axi_dma_iowrite32(dw, DMAC_CFG, val);
	
}

void axi_chan_irq_disable(struct dw_axi_dma *dw, unsigned int irq_mask)
{
	if (likely(irq_mask == DWAXIDMAC_IRQ_ALL)) {
		axi_chan_iowrite32(dw, CH_INTSTATUS_ENA, DWAXIDMAC_IRQ_NONE);
	} else {
		unsigned int val;
		val = axi_chan_ioread32(dw, CH_INTSTATUS_ENA);
		val &= ~irq_mask;
		axi_chan_iowrite32(dw, CH_INTSTATUS_ENA, val);
	}
}

void axi_chan_irq_set(struct dw_axi_dma *dw, unsigned int irq_mask)
{
	axi_chan_iowrite32(dw, CH_INTSTATUS_ENA, irq_mask);
}

void axi_chan_irq_sig_set(struct dw_axi_dma *dw, unsigned int irq_mask)
{
	axi_chan_iowrite32(dw, CH_INTSIGNAL_ENA, irq_mask);
}

unsigned int axi_chan_irq_read(struct dw_axi_dma *dw)
{
	return axi_chan_ioread32(dw, CH_INTSTATUS);
}

void axi_chan_disable(struct dw_axi_dma *dw)
{
	unsigned int val;
	val = axi_dma_ioread32(dw, DMAC_CHEN);
	val &= ~((0x1UL << dw->chan_id) << DMAC_CHAN_EN_SHIFT);
	val &= ~((0x1UL << dw->chan_id) << DMAC_CHAN_EN2_WE_SHIFT);
	axi_dma_iowrite32(dw, DMAC_CHEN, val);
}

void axi_chan_enable(struct dw_axi_dma *dw)
{
	unsigned int val;
	val = axi_dma_ioread32(dw, DMAC_CHEN);
	val |= (0x1UL << dw->chan_id) << DMAC_CHAN_EN_SHIFT |
		(0x1UL << dw->chan_id) << DMAC_CHAN_EN2_WE_SHIFT;
	axi_dma_iowrite32(dw, DMAC_CHEN, val);
}

bool axi_chan_is_hw_enable(struct dw_axi_dma *dw)
{
	unsigned int val;

	val = axi_dma_ioread32(dw, DMAC_CHEN);
	return !!(val & ((0x1UL<<dw->chan_id) << DMAC_CHAN_EN_SHIFT));
}

int dma_enable(struct dw_axi_dma *dw)
{
	unsigned int cfg_lo, cfg_hi, reg, irq_mask;
	axi_dma_enable(dw);
	axi_dma_irq_enable(dw);
	axi_chan_disable(dw);
    if( axi_chan_is_hw_enable(dw)) {
        printf("chan enable file!\n");
	    return -1;
    }
	cfg_lo = (dw->config.dst_mblock_type << CH_CFG_L_DST_MULTBLK_TYPE_POS | \
		dw->config.src_mblock_type << CH_CFG_L_SRC_MULTBLK_TYPE_POS);


	cfg_lo |= dw->config.src_hs_port << CH_CFG2_L_SRC_PER_POS |
			dw->config.dst_hs_port << CH_CFG2_L_DST_PER_POS ;

	cfg_hi = (dw->config.direction << CH_CFG2_H_TT_FC_POS |
		dw->config.chan_priority << CH_CFG2_H_PRIORITY_POS | 
		dw->config.src_hs_sel << CH_CFG2_H_HS_SEL_SRC_POS |
		dw->config.dst_hs_sel << CH_CFG2_H_HS_SEL_DST_POS |
		0xF <<CH_CFG2_H_SRC_OSR_LMT_POS |
		0xF <<CH_CFG2_H_DST_OSR_LMT_POS );
			
	axi_chan_iowrite32(dw, CH_CFG_L, cfg_lo);
	axi_chan_iowrite32(dw, CH_CFG_H, cfg_hi);

	axi_chan_iowrite64(dw, CH_SAR, dw->config.src_addr);
	axi_chan_iowrite64(dw, CH_DAR, dw->config.dst_addr);
	axi_chan_iowrite32(dw, CH_BLOCK_TS, (dw->config.chan_block_ts - 1));

	reg = dw->config.src_inc << CH_CTL_L_SRC_INC_POS | dw->config.dst_inc << CH_CTL_L_DST_INC_POS | 				\
		  dw->config.src_ts_width << CH_CTL_L_SRC_WIDTH_POS | dw->config.dst_ts_width << CH_CTL_L_DST_WIDTH_POS | 	\
		  dw->config.src_burst << CH_CTL_L_SRC_MSIZE_POS | dw->config.dst_burst << CH_CTL_L_DST_MSIZE_POS ;
	// if( dw->config.src_hs_sel == DWAXIDMAC_HS_SEL_HW)
	{
		reg |=	DWAXIDMAC_CH_CTL_L_MASTER1 << CH_CTL_L_SRC_MASTET;
	}
	// if( dw->config.dst_hs_sel == DWAXIDMAC_HS_SEL_HW)
	{

		reg |=	DWAXIDMAC_CH_CTL_L_MASTER1 << CH_CTL_L_DST_MASTET;
	}
	axi_chan_iowrite32(dw, CH_CTL_L, reg);

	axi_chan_iowrite32(dw, CH_CTL_H, 0x1 << 26);

	irq_mask = DWAXIDMAC_IRQ_NONE;
	axi_chan_irq_sig_set(dw, irq_mask);

	/* Generate 'suspend' status but don't generate interrupt */
	irq_mask = DWAXIDMAC_IRQ_DMA_TRF | DWAXIDMAC_IRQ_ALL_ERR | DWAXIDMAC_IRQ_BLOCK_TRF; // | DWAXIDMAC_IRQ_SRC_TRAN | DWAXIDMAC_IRQ_DST_TRAN;
	axi_chan_irq_set(dw, irq_mask);

	axi_chan_enable(dw);

	return 0;
}

/* Register offsets */
#define ES_SPI_CSR_00			0x00	/*WRITE_STATUS_REG_TIME*/
#define ES_SPI_CSR_01			0x04	/*SPI_BUS_MODE*/
#define ES_SPI_CSR_02			0x08	/*ERASE_COUNTER_TAP*/
#define ES_SPI_CSR_03			0x0c	/*DMA_EN_HCLK_STATUS*/
#define ES_SPI_CSR_04			0x10	/*FAST_READ_CONTROL*/
#define ES_SPI_CSR_05			0x14	/*SPI_FLASH_WR_NUM*/
#define ES_SPI_CSR_06			0x18	/*SPI_FLASH_COMMAND*/
#define ES_SPI_CSR_07			0x1c	/*INTERRUPT_CONTROL*/
#define ES_SPI_CSR_08			0x20	/*DMA_REQUEST_TAP*/
#define ES_SPI_CSR_09			0x24	/*SPI_FLASH_WR_ADDRESS*/
#define ES_SPI_CSR_10			0x28	/*PAGE_PROGRAM_TIME*/
#define ES_SPI_CSR_11			0x2c	/*SECTOR_ERASE_TIME*/
#define ES_SPI_CSR_12			0x30	/*SMALL_BLOCK_ERASE_TIME*/
#define ES_SPI_CSR_13			0x34	/*LARGE_BLOCK_ERASE_TIME*/
#define ES_SPI_CSR_14			0x38	/*CHIP_ERASE_TIME*/
#define ES_SPI_CSR_15			0x3c	/*CHIP_DESELECT_TIME*/
#define ES_SPI_CSR_16			0x40	/*POWER_DOWN_TIME*/
#define ES_SPI_DR				0xa800000ul

#define ES_SYSCSR_SPICLK_CTR			0x104
#define ES_SYSCSR_SPICFGCLK_CTR			0x108
#define ES_SYSCSR_SPIMODECFG			0x340
#define ES_SYSCSR_RSTCTR				0x490

#define ES_CONCSR_SPI_INTSEL			0x3c0

#define SPI_COMMAND_VALID				0x01
#define SPI_COMMAND_MOVE_VALUE			0x00
#define SPI_COMMAND_MOVE_DMA			0x01
#define SPI_COMMAND_CODE_FIELD_POSITION 0X06
#define SPI_COMMAND_MOVE_FIELD_POSITION 0X05
#define SPI_COMMAND_TYPE_FIELD_POSITION 0X01

#define SR_BUSY				BIT(0)

/* Bit fields in CTRLR0 */
/*
 * Only present when SSI_MAX_XFER_SIZE=16. This is the default, and the only
 * option before version 3.23a.
 */
#define SPI_INTSEL_MASK			GENMASK(11, 10)
#define INT_ROUTED_U84			0x0
#define INT_ROUTED_LPCPU		0x1
#define INT_ROUTED_SCPU			0x3u

#define RX_TIMEOUT			5000		/* timeout in ms */

#define SPI_COMMAND_INIT_VALUE       0XFFFFC000
#define FLASH_PAGE_SIZE      0x100
typedef enum {
    SPI_FLASH_WR_BYTE  = 1,
    SPI_FLASH_WR_2BYTE = 2,
    SPI_FLASH_WR_WORD  = 4,
} SPI_FLASH_WR_NUM_T;

typedef enum {
    SPI_FAST_READ_DEFAULT = 0,
    SPI_FAST_READ_ENABLE  = 3 /*WHEN SPI QUAD0 OR DUAL MODE*/
} SPI_FAST_READ_CTL_T;


typedef enum { STANDARD_SPI = 0, DUAL_SPI, QUAD_SPI } SPI_BUS_MODE_T;

typedef enum {
    SPIC_CMD_TYPE_SPI_PROGRAM = 0,
    SPIC_CMD_TYPE_WRITE_STATUS_REGISTER,
    SPIC_CMD_TYPE_READ_STATUS_REGISTER,
    SPIC_CMD_TYPE_SECTOR_ERASE,
    SPIC_CMD_TYPE_BLOCK_ERASE_TYPE1,
    SPIC_CMD_TYPE_BLOCK_ERASE_TYPE2,
    SPIC_CMD_TYPE_CHIP_ERASE,
    SPIC_CMD_TYPE_POWER_DOWN,
    SPIC_CMD_TYPE_RELEASE_POWER_DOWM,
    SPIC_CMD_TYPE_ENTER_OR_EXIT_32BIT_MODE,
    SPIC_CMD_TYPE_READ_SECURITY_REG,
    SPIC_CMD_TYPE_ERASE_SECURITY_REG,
    SPIC_CMD_TYPE_WRITE_SECURITY_REG,
    SPIC_CMD_TYPE_READ_DATA,
    SPIC_CMD_TYPE_READ_MANUFACTURED_ID,
    SPIC_CMD_TYPE_READ_JEDEC_ID
} SPI_FLASH_COMMAND_TYPE_T;

#define SPINOR_OP_RDSR3		0x15	/* Read status register 3 */
#define SPINOR_OP_WRSR3		0x11	/* Write status register 3 */

#define SPINOR_BLOCK_LOCK		0x36	/* Individual Block/Sector Lock */
#define SPINOR_BLOCK_UNLOCK		0x39	/* Individual Block/Sector UnLock */
#define SPINOR_GLOBAL_BLOCK_LOCK		0x7E	/* global Block/Sector UnLock */
#define SPINOR_GLOBAL_BLOCK_UNLOCK		0x98	/* global Block/Sector UnLock */

#define SPIC_CMD_CODE_POWER_DOWN              0xb9
#define SPIC_CMD_CODE_RELEASE_POWER_DOWN      0xab

struct es_spi_plat {
	s32 frequency;		/* Default clock frequency, -1 for none */
	void __iomem *sys_regs;
	void __iomem *regs;
	s32 write_status_reg_time;
	s32 page_program_time;
	s32 sector_erase_time;
	s32 small_block_erase_time;
	s32 large_block_erase_time;
	s32 chip_erase_time;
};

struct es_spi_priv {
	struct clk clk;
	struct reset_ctl_bulk resets;
	struct dw_axi_dma dma;
	struct gpio_desc *cs_gpio;	/* External chip-select gpio */
	struct gpio_desc *wp_gpio;	/* External wp gpio */

	void __iomem *regs;
	void __iomem *sys_regs;
	void __iomem *flash_base;
	unsigned long bus_clk_rate;
	unsigned int freq;		/* Default frequency */
	unsigned int mode;

	const void *tx;
	u32 opcode;
	u32 cmd_type;
	u64 addr;
	void *rx;
	u32 fifo_len;			/* depth of the FIFO buffer */
	u32 max_xfer;			/* Maximum transfer size (in bits) */

	int bits_per_word;
	int len;
	u8 cs;				/* chip select pin */
	bool wp_enabled;
	u8 tmode;			/* TR/TO/RO/EEPROM */
	u8 type;			/* SPI/SSP/MicroWire */
	s32 write_status_reg_time;
	s32 page_program_time;
	s32 sector_erase_time;
	s32 small_block_erase_time;
	s32 large_block_erase_time;
	s32 chip_erase_time;
};

uint8_t es_read_flash_status_register(struct es_spi_priv *priv, uint8_t *register_data, int flash_cmd);

static inline u32 es_read(struct es_spi_priv *priv, u32 offset)
{
	return readl(priv->regs + offset);
}

static inline void es_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->regs + offset);
}

static inline u32 es_syscrg_read(struct es_spi_priv *priv, u32 offset)
{
	return readl(priv->sys_regs + offset);
}

static inline void es_syscrg_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->sys_regs + offset);
}

static inline void es_concrg_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->sys_regs + offset - 0x18000);
}

static inline u32 es_data_read(struct es_spi_priv *priv, u32 offset)
{
	return readl(priv->flash_base + offset);
}

static inline void es_data_write(struct es_spi_priv *priv, u32 offset, u32 val)
{
	writel(val, priv->flash_base + offset);
}

static int es_spi_dwc_init(struct udevice *bus, struct es_spi_priv *priv)
{
	priv->max_xfer = 32;
	return 0;
}

static int spi_wait_over(struct es_spi_priv *priv)
{
	while(es_read(priv, ES_SPI_CSR_06) & 0x1);
	return 0;
}

static int spi_flash_idle(struct es_spi_priv *priv)
{
	uint32_t register_data = 0xff;
	// check flash status register' busy bit to make sure operation is finish.
	while (register_data & 0x1) {
		es_read_flash_status_register(priv, (uint8_t *)&register_data, SPINOR_OP_RDSR);
	}
	return 0;
}

static int request_gpio_cs(struct udevice *bus)
{

	struct es_spi_priv *priv = dev_get_priv(bus);

	priv->cs_gpio = devm_gpiod_get(bus, "cs", GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (IS_ERR(priv->cs_gpio)) {
		dev_err(bus, "Couldn't request gpio! (error %ld)\n", PTR_ERR(priv->cs_gpio));
		return PTR_ERR(priv->cs_gpio);
	}
	if (dm_gpio_is_valid(priv->cs_gpio)) {
		dm_gpio_set_dir_flags(priv->cs_gpio, GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	}

	dev_dbg(bus, "Using external gpio for CS management\n");
	return 0;
}

static int es_spi_of_to_plat(struct udevice *bus)
{
	struct es_spi_plat *plat = bus->plat_;

	plat->regs = dev_read_addr_ptr(bus);
	if (!plat->regs)
		return -EINVAL;

	/* Use 500KHz as a suitable default */
	plat->frequency = dev_read_u32_default(bus, "spi-max-frequency",
					       500000);

	if (dev_read_bool(bus, "spi-slave"))
		return -EINVAL;

	dev_info(bus, "max-frequency=%d\n", plat->frequency);
	plat->sys_regs = (void *)(uintptr_t)dev_read_u32_default(bus, "es,sysscr_reg",
					       0x51828000);
	plat->write_status_reg_time = (uintptr_t)dev_read_u32_default(bus, "write-status-reg-time", 15);
	plat->page_program_time = (uintptr_t)dev_read_u32_default(bus, "page-program-time", 3);
	plat->sector_erase_time = (uintptr_t)dev_read_u32_default(bus, "sector-erase-time", 400);
	plat->small_block_erase_time = (uintptr_t)dev_read_u32_default(bus, "small-block-erase-time", 1600);
	plat->large_block_erase_time = (uintptr_t)dev_read_u32_default(bus, "large-block-erase-time", 2000);
	plat->chip_erase_time = (uintptr_t)dev_read_u32_default(bus, "chip-erase-time", 200);

	dev_dbg(bus, "write_status_reg_time %d, page_program_time %d "
			"sector_erase_time %d, small_block_erase_time %d "
			"large_block_erase_time %d, chip_erase_time %d\n",
			plat->write_status_reg_time, plat->page_program_time,
			plat->sector_erase_time, plat->small_block_erase_time,
			plat->large_block_erase_time, plat->chip_erase_time);

	return request_gpio_cs(bus);
}

/* Restart the controller, disable all interrupts, clean rx fifo */
static void spi_hw_init(struct udevice *bus, struct es_spi_priv *priv)
{
	// Convert times from ms to cycles
	unsigned long freq = priv->bus_clk_rate;

	uint32_t write_status_reg_time_cycles = priv->write_status_reg_time * (freq / 1000);
	uint32_t page_program_time_cycles = priv->page_program_time * (freq / 1000);
	uint32_t sector_erase_time_cycles = priv->sector_erase_time * (freq / 1000);
	uint32_t small_block_erase_time_cycles = priv->small_block_erase_time * (freq / 1000);
	uint32_t large_block_erase_time_cycles = priv->large_block_erase_time * (freq / 1000);
	uint32_t chip_erase_time_cycles = priv->chip_erase_time * (freq / 1000);

	es_write(priv, ES_SPI_CSR_00, write_status_reg_time_cycles);
	es_write(priv, ES_SPI_CSR_10, page_program_time_cycles);
	es_write(priv, ES_SPI_CSR_11, sector_erase_time_cycles);
	es_write(priv, ES_SPI_CSR_12, small_block_erase_time_cycles);
	es_write(priv, ES_SPI_CSR_13, large_block_erase_time_cycles);
	es_write(priv, ES_SPI_CSR_14, chip_erase_time_cycles);

	if (!priv->fifo_len) {
		priv->fifo_len = 256;
	}
	dev_dbg(bus, "write_status_reg_time_cycles 0x%x, page_program_time_cycles 0x%x\n"
			"\tsector_erase_time_cycles 0x%x, small_block_erase_time_cycles 0x%x\n"
			"\tlarge_block_erase_time_cycles 0x%x, chip_erase_time_cycles 0x%x\n"
			"\tfreq %ld\n",
			write_status_reg_time_cycles, page_program_time_cycles,
			sector_erase_time_cycles, small_block_erase_time_cycles,
			large_block_erase_time_cycles, chip_erase_time_cycles, freq);
}

/*
 * We define es_spi_get_clk function as 'weak' as some targets
 * (like SOCFPGA_GEN5 and SOCFPGA_ARRIA10) don't use standard clock API
 * and implement es_spi_get_clk their own way in their clock manager.
 */
__weak int es_spi_get_clk(struct udevice *bus, ulong *rate)
{
	struct es_spi_priv *priv = dev_get_priv(bus);
	int ret;

	ret = clk_get_by_index(bus, 0, &priv->clk);
	if (ret)
		return ret;

	ret = clk_enable(&priv->clk);
	if (ret && ret != -ENOSYS && ret != -ENOTSUPP)
		return ret;
	ret = clk_set_rate(&priv->clk, 40000000);
	if (ret)
		return ret;
	*rate = clk_get_rate(&priv->clk);
	if (!*rate)
		goto err_rate;

	dev_dbg(bus, "Got clock via device tree: %lu Hz\n", *rate);

	return 0;

err_rate:
	clk_disable(&priv->clk);
	clk_free(&priv->clk);

	return -EINVAL;
}

static int es_spi_reset(struct udevice *bus)
{
	int ret;
	struct es_spi_priv *priv = dev_get_priv(bus);

	ret = reset_get_bulk(bus, &priv->resets);
	if (ret) {
		/*
		 * Return 0 if error due to !CONFIG_DM_RESET and reset
		 * DT property is not present.
		 */
		if (ret == -ENOENT || ret == -ENOTSUPP)
			return 0;

		dev_warn(bus, "Couldn't find/assert reset device (error %d)\n",
			 ret);
		return ret;
	}
	ret = reset_release_bulk(&priv->resets);
	if (ret)
		return ret;
	ret = reset_deassert_bulk(&priv->resets);
	if (ret) {
		reset_release_bulk(&priv->resets);
		dev_err(bus, "Failed to de-assert reset for SPI (error %d)\n",
			ret);
		return ret;
	}

	return 0;
}

typedef int (*es_spi_init_t)(struct udevice *bus, struct es_spi_priv *priv);


#define HAL_DMA0_BASE            (0x518c0000ul)
#define GET_DIE_OFFSET(a) (a - 0x51800000UL)
void dma_init(struct es_spi_priv *priv)
{
	priv->dma.dma_base		= GET_DIE_OFFSET(priv->regs) + HAL_DMA0_BASE;
	priv->dma.chan_id		= 0;
	priv->dma.config.chan_priority = 0;
	priv->dma.config.src_ts_width = DWAXIDMAC_TRANS_WIDTH_32;
	priv->dma.config.dst_ts_width = DWAXIDMAC_TRANS_WIDTH_32;
	priv->dma.config.src_mblock_type = DWAXIDMAC_MBLK_TYPE_CONTIGUOUS;
	priv->dma.config.dst_mblock_type = DWAXIDMAC_MBLK_TYPE_CONTIGUOUS;
	priv->dma.config.src_hs_port = DWAXIDMAC_HS_PORT_BOOTSPI_REQ;
	priv->dma.config.dst_hs_port = DWAXIDMAC_HS_PORT_BOOTSPI_REQ;
}
static int es_spi_probe(struct udevice *bus)
{
	es_spi_init_t init = (es_spi_init_t)dev_get_driver_data(bus);
	struct es_spi_plat *plat = dev_get_plat(bus);
	struct es_spi_priv *priv = dev_get_priv(bus);
	int ret;

	priv->regs = plat->regs;
	priv->sys_regs = plat->sys_regs;
	priv->freq = plat->frequency;
	priv->flash_base = (void *)(uintptr_t)(plat->regs + ES_SPI_DR);
	priv->write_status_reg_time = plat->write_status_reg_time;
	priv->page_program_time = plat->page_program_time;
	priv->sector_erase_time = plat->sector_erase_time;
	priv->small_block_erase_time = plat->small_block_erase_time;
	priv->large_block_erase_time = plat->large_block_erase_time;
	priv->chip_erase_time = plat->chip_erase_time ;
	ret = es_spi_get_clk(bus, &priv->bus_clk_rate);
	if (ret)
		return ret;

	ret = es_spi_reset(bus);
	if (ret)
		return ret;

	if (!init)
		return -EINVAL;
	ret = init(bus, priv);
	if (ret)
		return ret;

	dev_dbg(bus, "ssi_max_xfer_size=%u\n", priv->max_xfer);

	/* Currently only bits_per_word == 8 supported */
	priv->bits_per_word = 8;

	priv->tmode = 0; /* Tx & Rx */

	/* Basic HW init */
	spi_hw_init(bus, priv);
	dma_init(priv);

	priv->wp_gpio = devm_gpiod_get(bus, "wp", GPIOD_IS_OUT | GPIOD_IS_OUT_ACTIVE);
	if (IS_ERR(priv->wp_gpio)) {
		dev_err(bus, "%s %d: couldn't request wp gpio! (error %ld)\n", __func__,__LINE__,
			PTR_ERR(priv->wp_gpio));
		return PTR_ERR(priv->wp_gpio);
	}

	return 0;
}

/**
 * @brief spi read and write cfg
 */
static void spi_read_write_cfg(struct es_spi_priv *priv, u32 byte, u32 addr)
{
    es_write(priv, ES_SPI_CSR_05, byte);
    es_write(priv, ES_SPI_CSR_09, addr);
    es_write(priv, ES_SPI_CSR_04, SPI_FAST_READ_DEFAULT);
    es_write(priv, ES_SPI_CSR_01, STANDARD_SPI);
}

/**
 *  @brief write data from dest address to flash
 */
static void spi_send_data(struct es_spi_priv *priv, u32 *dest, u32 size)
{
	es_write(priv, ES_SPI_CSR_08, 0x5);
	size = (size + SPI_FLASH_WR_WORD -1) / SPI_FLASH_WR_WORD;
	priv->dma.config.direction = DWAXIDMAC_TT_FC_MEM_TO_PER_DMAC;
	priv->dma.config.src_inc = DWAXIDMAC_CH_CTL_L_INC;
	priv->dma.config.dst_inc = DWAXIDMAC_CH_CTL_L_NOINC;
	priv->dma.config.chan_block_ts = size;
	priv->dma.config.src_burst = DWAXIDMAC_BURST_TRANS_LEN_64;
	priv->dma.config.dst_burst = DWAXIDMAC_BURST_TRANS_LEN_64;
	priv->dma.config.src_addr = (unsigned long)dest;
	priv->dma.config.dst_addr = (unsigned long)priv->flash_base;
	priv->dma.config.src_hs_sel = DWAXIDMAC_HS_SEL_SW;
	priv->dma.config.dst_hs_sel = DWAXIDMAC_HS_SEL_HW;
	dma_enable(&priv->dma);
}

/**
 *  @brief Read data from flash to dest address
 */
static void spi_recv_data(struct es_spi_priv *priv, u32 *dest, u32 size)
{
	es_write(priv, ES_SPI_CSR_08, 0x0);
	size = (size + SPI_FLASH_WR_WORD -1) / SPI_FLASH_WR_WORD;
	priv->dma.config.direction = DWAXIDMAC_TT_FC_MEM_TO_MEM_DMAC;
	priv->dma.config.src_inc = DWAXIDMAC_CH_CTL_L_INC;
	priv->dma.config.dst_inc = DWAXIDMAC_CH_CTL_L_INC;
	priv->dma.config.chan_block_ts = size;
	priv->dma.config.src_burst = DWAXIDMAC_BURST_TRANS_LEN_1;
	priv->dma.config.dst_burst = DWAXIDMAC_BURST_TRANS_LEN_1;
	priv->dma.config.src_addr = (unsigned long)priv->flash_base;
	priv->dma.config.dst_addr = (unsigned long)dest;
	priv->dma.config.src_hs_sel = DWAXIDMAC_HS_SEL_HW;
	priv->dma.config.dst_hs_sel = DWAXIDMAC_HS_SEL_SW;
	dma_enable(&priv->dma);
}


/**
 * @brief spi send command
 */
static void spi_command_cfg(struct es_spi_priv *priv, u32 code, u32 type, u32 dma)
{
	if(dma == SPI_COMMAND_MOVE_DMA)
		es_write(priv, ES_SPI_CSR_07, 0x4);
	else
		es_write(priv, ES_SPI_CSR_07, 0x0);

    u32 command = ((code << SPI_COMMAND_CODE_FIELD_POSITION) |
                (dma << SPI_COMMAND_MOVE_FIELD_POSITION) |
                (type << SPI_COMMAND_TYPE_FIELD_POSITION) | SPI_COMMAND_VALID);
    es_write(priv, ES_SPI_CSR_06, command);
}

int wait_dma_irq(struct es_spi_priv *priv)
{
	u64 chan_status = 0;
	while(1){
		chan_status = axi_chan_get_irq_state(&priv->dma);
		if (chan_status & (0x1  <<  0)) {
			axi_chan_irq_clear(&priv->dma, chan_status);
			axi_chan_disable(&priv->dma);
			axi_chan_irq_disable(&priv->dma, DWAXIDMAC_IRQ_ALL);
			break;
		}
		axi_chan_irq_clear(&priv->dma, chan_status);
	}
	return 0;
}
int wait_spi_irq(struct es_spi_priv *priv)
{
	while(!(es_read(priv, ES_SPI_CSR_07) & 0x1));
	es_write(priv, ES_SPI_CSR_07, 0x2);
	return 0;
}


/**
 * @brief  spi write flash
 * @param [in]  offset: address of flash to be write
 * @param [in]  wr_dest: Address of data to be sent
 * @param [in]  size: size of flash to be write
 */
void es_writer(struct es_spi_priv *priv)
{
	u32 write_size = 0, offset, cmd_code;

	offset = priv->addr;
	cmd_code = priv->opcode;
	u32 cmd_type = priv->cmd_type;
	u8 *wr_dest = (u8 *)(priv->tx);
	u8 *buf = memalign(64, FLASH_PAGE_SIZE);
	int size = priv->len;
	// printf("%s %d cmd_code %x cmd_type %x\r\n",__func__,__LINE__,cmd_code,cmd_type);
	if (size == 0)
	{
		// if(SPIC_CMD_TYPE_SECTOR_ERASE == cmd_type)
		{
			spi_read_write_cfg(priv, write_size, (uintptr_t)offset);
			spi_command_cfg(priv, cmd_code, cmd_type, SPI_COMMAND_MOVE_VALUE);
			spi_wait_over(priv);
			spi_flash_idle(priv);
		}
	}
	while (size > 0) {
		write_size = size;
		if (write_size > FLASH_PAGE_SIZE) {
			write_size = FLASH_PAGE_SIZE;
		}
		memset(buf, 0, write_size);
		memcpy(buf, wr_dest, write_size);
		flush_cache((unsigned long)buf, write_size);

		spi_read_write_cfg(priv, write_size, offset);
		spi_send_data(priv, (u32 *)buf, write_size);
		spi_command_cfg(priv, cmd_code, cmd_type, SPI_COMMAND_MOVE_DMA);
		wait_spi_irq(priv);
		wait_dma_irq(priv);
		spi_flash_idle(priv);

		wr_dest += write_size;
		offset += write_size;
		size = size - write_size;
	}
	free(buf);
}


static void es_reader(struct es_spi_priv *priv)
{
	int       read_size = 0;
	u32 offset = priv->addr;
	u32 cmd_code = priv->opcode;
	u32 cmd_type = priv->cmd_type;
	u8 *mem_dest = priv->rx;
	int size = priv->len;
	u8 *buf = memalign(64, FLASH_PAGE_SIZE);
	memset(mem_dest, 0, size);
	while (size > 0) {
		read_size = size;
		if (read_size > FLASH_PAGE_SIZE) {
			read_size = FLASH_PAGE_SIZE;
		}
		memset(buf, 0, read_size);
		flush_cache((unsigned long)buf,read_size);
		spi_read_write_cfg(priv, read_size, offset);
		spi_command_cfg(priv, cmd_code, cmd_type, SPI_COMMAND_MOVE_VALUE);
		spi_wait_over(priv);
		spi_recv_data(priv, (u32 *)buf, read_size);
		wait_dma_irq(priv);

		memcpy(mem_dest, buf, read_size);
		mem_dest += read_size;
		offset += read_size;
		size = size - read_size;
	}
	free(buf);
}

static int poll_transfer(struct es_spi_priv *priv)
{
	es_writer(priv);
	es_reader(priv);

	return 0;
}

/*
 * We define es_external_cs_manage function as 'weak' as some targets
 * (like MSCC Ocelot) don't control the external CS pin using a GPIO
 * controller. These SoCs use specific registers to control by
 * software the SPI pins (and especially the CS).
 */
__weak void es_external_cs_manage(struct es_spi_priv *priv, bool on)
{
	if (!dm_gpio_is_valid(priv->cs_gpio))
		return;

	dm_gpio_set_value(priv->cs_gpio, on ? 1 : 0);
}

static int es_spi_xfer(struct udevice *dev, unsigned int bitlen,
		       const void *dout, void *din, unsigned long flags)
{
	struct udevice *bus = dev->parent;
	struct es_spi_priv *priv = dev_get_priv(bus);
	const u8 *tx = dout;
	u8 *rx = din;
	int ret = 0;

	/* spi core configured to do 8 bit transfers */
	if (bitlen % 8) {
		dev_err(dev, "Non byte aligned SPI transfer.\n");
		return -1;
	}

	/* Start the transaction if necessary. */
	if (flags & SPI_XFER_BEGIN)
		es_external_cs_manage(priv, false);

	priv->len = bitlen >> 3;

	priv->tx = (void *)tx;
	priv->rx = rx;

	/* wait the spi idle before writing control registers */
	spi_wait_over(priv);

	dev_dbg(dev, "rx=%p tx=%p len=%d [bytes]\n", rx, tx,
		priv->len);

	/* Start transfer in a polling loop */
	ret = poll_transfer(priv);

	/*
	 * Wait for current transmit operation to complete.
	 * Otherwise if some data still exists in Tx FIFO it can be
	 * silently flushed, i.e. dropped on disabling of the controller,
	 * which happens when writing 0 to es_SPI_SSIENR which happens
	 * in the beginning of new transfer.
	 */
	if (spi_wait_over(priv) < 0) {
		ret = -ETIMEDOUT;
	}

	/* Stop the transaction if necessary */
	if (flags & SPI_XFER_END)
		es_external_cs_manage(priv, true);

	return ret;
}

static int es_spi_exec_op(struct spi_slave *slave, const struct spi_mem_op *op)
{
	bool read = op->data.dir == SPI_MEM_DATA_IN;
	int ret = 0;
	struct udevice *bus = slave->dev->parent;
	struct es_spi_priv *priv = dev_get_priv(bus);


	priv->addr = op->addr.val;
	priv->opcode = op->cmd.opcode;

	// dev_err(bus, "addr=%lx opcode=%lx\n", priv->addr, priv->opcode);
	if( priv->opcode == SPINOR_OP_WREN || priv->opcode == SPINOR_OP_WRDI)
		return 0;
	switch(priv->opcode) {
		case SPINOR_OP_RDID:
		case SPINOR_OP_RDSFDP:
			priv->cmd_type = SPIC_CMD_TYPE_READ_JEDEC_ID;
			break;
		case SPINOR_OP_BE_4K:
		case SPINOR_OP_BE_4K_PMC:
			priv->opcode = SPINOR_OP_BE_4K;
			priv->cmd_type = SPIC_CMD_TYPE_SECTOR_ERASE;
			break;
		case SPINOR_OP_BE_32K:
			priv->cmd_type = SPIC_CMD_TYPE_BLOCK_ERASE_TYPE1;
			break;
		case SPINOR_OP_SE:
			priv->cmd_type = SPIC_CMD_TYPE_BLOCK_ERASE_TYPE2;
			break;
		case SPINOR_OP_CHIP_ERASE:
			priv->cmd_type = SPIC_CMD_TYPE_CHIP_ERASE;
			break;
		case SPINOR_OP_PP:
		case SPINOR_OP_PP_1_1_4:
		case SPINOR_OP_PP_1_4_4:
		case SPINOR_OP_PP_1_1_8:
		case SPINOR_OP_PP_1_8_8:
			priv->opcode = SPINOR_OP_PP;
			priv->cmd_type = SPIC_CMD_TYPE_SPI_PROGRAM;
			break;
		case SPINOR_OP_READ:
		case SPINOR_OP_READ_FAST:
		case SPINOR_OP_READ_1_1_2:
		case SPINOR_OP_READ_1_2_2:
		case SPINOR_OP_READ_1_1_4:
		case SPINOR_OP_READ_1_4_4:
		case SPINOR_OP_READ_1_1_8:
		case SPINOR_OP_READ_1_8_8:
			priv->opcode = SPINOR_OP_READ;
			priv->cmd_type = SPIC_CMD_TYPE_READ_DATA;
			break;
		case SPINOR_OP_RDSR:
		case SPINOR_OP_RDSR2:
			priv->cmd_type = SPIC_CMD_TYPE_READ_STATUS_REGISTER;
			break;
		case SPINOR_OP_WRSR:
		case SPINOR_OP_WRSR2:
			priv->cmd_type = SPIC_CMD_TYPE_WRITE_STATUS_REGISTER;
			break;
		case SPIC_CMD_CODE_POWER_DOWN:
			priv->cmd_type = SPIC_CMD_TYPE_POWER_DOWN;
			break;
		case SPIC_CMD_CODE_RELEASE_POWER_DOWN:
			priv->cmd_type = SPIC_CMD_TYPE_RELEASE_POWER_DOWM;
			break;
		case SPINOR_OP_SRSTEN:
		case SPINOR_OP_SRST:
			priv->cmd_type = SPIC_CMD_TYPE_SPI_PROGRAM;
			break;
		default:
			priv->cmd_type = SPIC_CMD_TYPE_SPI_PROGRAM;
			break;
	}
	// dev_err(bus, "opcode = %x cmd_type %x\n", priv->opcode, priv->cmd_type);
	es_external_cs_manage(priv, false);

	// dev_err(bus, "read=%lx\n", read);
	if (read) {
		priv->rx = op->data.buf.in;

		priv->len = op->data.nbytes;
		// dev_err(bus, "read len = %u\n", op->data.nbytes);
		es_reader(priv);
	} else {
		priv->tx = op->data.buf.out;
		priv->len = op->data.nbytes;
		// dev_err(bus, "write len = 0x%x  tx_addr %x\n", op->data.nbytes, priv->tx);
		/* Fill up the write fifo before starting the transfer */
		es_writer(priv);

		if (spi_wait_over(priv) < 0) {
			printf("spi_wait_over ETIMEDOUT\n");
			ret = -ETIMEDOUT;
		}
	}

	es_external_cs_manage(priv, true);

	dev_dbg(bus, "%u bytes xfered\n", op->data.nbytes);
	return ret;
}

/* The size of ctrl1 limits data transfers to 64K */
static int es_spi_adjust_op_size(struct spi_slave *slave, struct spi_mem_op *op)
{
	op->data.nbytes = min(op->data.nbytes, (unsigned int)SZ_64K);

	return 0;
}

static const struct spi_controller_mem_ops es_spi_mem_ops = {
	.exec_op = es_spi_exec_op,
	.adjust_op_size = es_spi_adjust_op_size,
};

static int es_spi_set_speed(struct udevice *bus, uint speed)
{
	struct es_spi_plat *plat = dev_get_plat(bus);
	struct es_spi_priv *priv = dev_get_priv(bus);

	if (speed > plat->frequency)
		speed = plat->frequency;

	priv->freq = speed;

	return 0;
}

static int es_spi_set_mode(struct udevice *bus, uint mode)
{
	struct es_spi_priv *priv = dev_get_priv(bus);

	/*
	 * Can't set mode yet. Since this depends on if rx, tx, or
	 * rx & tx is requested. So we have to defer this to the
	 * real transfer function.
	 */
	priv->mode = mode;
	dev_dbg(bus, "mode=%d\n", priv->mode);

	return 0;
}

static int es_spi_remove(struct udevice *bus)
{
	struct es_spi_priv *priv = dev_get_priv(bus);
	int ret;

	ret = reset_release_bulk(&priv->resets);
	if (ret)
		return ret;

#if CONFIG_IS_ENABLED(CLK)
	ret = clk_disable(&priv->clk);
	if (ret)
		return ret;
	clk_free(&priv->clk);
#endif
	return 0;
}

static const struct dm_spi_ops es_spi_ops = {
	.xfer		= es_spi_xfer,
	.mem_ops	= &es_spi_mem_ops,
	.set_speed	= es_spi_set_speed,
	.set_mode	= es_spi_set_mode,
	/*
	 * cs_info is not needed, since we require all chip selects to be
	 * in the device tree explicitly
	 */
};

static const struct udevice_id es_spi_ids[] = {
	/* Generic compatible strings */
	{ .compatible = "eswin,es-apb-spi-1.0", .data = (ulong)es_spi_dwc_init },
};

U_BOOT_DRIVER(es_spi) = {
	.name = "es_spi",
	.id = UCLASS_SPI,
	.of_match = es_spi_ids,
	.ops = &es_spi_ops,
	.of_to_plat = es_spi_of_to_plat,
	.plat_auto = sizeof(struct es_spi_plat),
	.priv_auto = sizeof(struct es_spi_priv),
	.probe = es_spi_probe,
	.remove = es_spi_remove,
};



uint8_t es_read_flash_status_register(struct es_spi_priv *priv, uint8_t *register_data, int flash_cmd)
{
	u8 *buf = memalign(64, sizeof(uint32_t));
	memset(register_data, 0, sizeof(uint32_t));
	memset(buf, 0, sizeof(uint32_t));
	flush_cache((unsigned long)buf, sizeof(uint32_t));

	//Flash status register is 1byte
	spi_read_write_cfg(priv, 1, 0);
	//Set SPI_FLASH_COMMAND
	spi_command_cfg(priv, flash_cmd, SPIC_CMD_TYPE_READ_STATUS_REGISTER, SPI_COMMAND_MOVE_VALUE);
	spi_wait_over(priv);
	//Read back data
	spi_recv_data(priv, (u32 *)buf, 1);
	wait_dma_irq(priv);

	memcpy(register_data, buf, sizeof(uint32_t));
	// printf("[%s %d]: command 0x%x, status register_data 0x%x buf 0x%x\n",__func__,__LINE__, flash_cmd, *register_data, *buf);
	free(buf);
	return 0;
}

void es_write_flash_status_register(struct es_spi_priv *priv, uint32_t register_data, int flash_cmd)
{
	u8 *buf = memalign(64, sizeof(uint32_t));
	memset(buf, 0, sizeof(uint32_t));
	memcpy(buf, &register_data, sizeof(uint32_t));
	flush_cache((unsigned long)buf, sizeof(uint32_t));

	//Flash status register-2 is 1byte
	spi_read_write_cfg(priv, 1, 0);
	spi_send_data(priv, (u32 *)buf, 1);

	spi_command_cfg(priv, flash_cmd, SPIC_CMD_TYPE_WRITE_STATUS_REGISTER, SPI_COMMAND_MOVE_DMA);
	//Wait command finish
	wait_dma_irq(priv);
	wait_spi_irq(priv);
	spi_flash_idle(priv);
	free(buf);
	// printf("[%s %d]: command 0x%x, status register_data 0x%x\n",__func__,__LINE__, flash_cmd, register_data);
}

void es_write_flash_global_block_lock_register(struct es_spi_priv *priv, int flash_cmd)
{

	//Flash global block lock register not need data
	spi_read_write_cfg(priv, 0, 0);

	spi_command_cfg(priv, flash_cmd, SPIC_CMD_TYPE_CHIP_ERASE, SPI_COMMAND_MOVE_VALUE);

	//Wait command finish
	spi_wait_over(priv);
	spi_flash_idle(priv);
	// printf("[%s %d]: command 0x%x\n",__func__,__LINE__, flash_cmd);
}

void es_flash_global_wp_cfg(struct es_spi_priv *priv, int enable)
{
	uint32_t register_data, request_register_data;

	es_external_cs_manage(priv, false);

	//Update status register1
	es_read_flash_status_register(priv, (uint8_t *)&register_data, SPINOR_OP_RDSR);
	request_register_data = register_data;
		/*
			  SRP SEC TB BP2 BP1 BP0 WEL BUSY
	 	*/
	request_register_data |= (1 << 5);  //TB 1, bottom
	//request_register_data &= ~(1 << 5);  //TB 0, top
	request_register_data &= ~(1 << 6);  // SEC 0, 64K
	if (request_register_data != register_data) {
		es_write_flash_status_register(priv, request_register_data, SPINOR_OP_WRSR);
	}

	//Update status register3
	es_read_flash_status_register(priv, (uint8_t *)&register_data, SPINOR_OP_RDSR3);
	request_register_data = register_data;
		/*
			  R DRV1 DRV0 R R WPS R R
	 	*/
	request_register_data |= (1 << 2);   //WPS 1, individual block
	if (request_register_data != register_data) {
		es_write_flash_status_register(priv, request_register_data, SPINOR_OP_WRSR3);
	}

	//Update global lock/unlock register
	if (enable) {
		es_write_flash_global_block_lock_register(priv, SPINOR_GLOBAL_BLOCK_LOCK);
	} else {
		es_write_flash_global_block_lock_register(priv, SPINOR_GLOBAL_BLOCK_UNLOCK);
	}
	es_external_cs_manage(priv, true);
}

void es_write_flash_individual_block_lock_register(struct es_spi_priv *priv, void *addr, int flash_cmd)
{
	spi_read_write_cfg(priv, 3, (uintptr_t)addr);

	spi_command_cfg(priv, flash_cmd, SPIC_CMD_TYPE_BLOCK_ERASE_TYPE2, SPI_COMMAND_MOVE_VALUE);

	//Wait command finish
	spi_wait_over(priv);

	//printf("[%s %d]: command 0x%x, addr 0x%px, flash_cmd 0x%x\n",__func__,__LINE__,
	//	command, addr, flash_cmd);
}

#define ALIGNMENT_SIZE 0x10000    // 64KB alignment
int es_flash_region_wp_cfg(struct spi_flash *flash, void *addr, int size, bool lock)
{
	uint32_t i;
	int flash_cmd;
	struct udevice *dev = flash->spi->dev->parent;
	struct es_spi_priv *priv = dev_get_priv(dev);

	if((uintptr_t)0x51800000 != (uintptr_t)priv->regs && (uintptr_t)0x71800000 != (uintptr_t)priv->regs)
		return -3;
	// Check if the address is 64KB aligned
	if ((uintptr_t)addr % ALIGNMENT_SIZE != 0) {
		printf("error: addr 0x%px is not aligned to 64KB\n", addr);
		return -1;
	}

	// Check if the size is 64KB aligned
	if (size % ALIGNMENT_SIZE != 0) {
		printf("error: size 0x%d is not aligned to 64KB\n", size);
		return -2;
	}

	flash_cmd = lock ? SPINOR_BLOCK_LOCK : SPINOR_BLOCK_UNLOCK;

	es_external_cs_manage(priv, false);

	for (i = 0; i < size; i += ALIGNMENT_SIZE) {
		es_write_flash_individual_block_lock_register(priv, addr + i, flash_cmd);
	}

	es_external_cs_manage(priv, true);
	return 0;
}

/*
	0: disable write_protection
	1: enable write_protection
*/
extern void gpio_force(int num, int inout);
extern void gpio_level_cfg(int index, int level);
int es_bootspi_wp_cfg(struct spi_flash *flash, int enable)
{
	struct udevice *dev = flash->spi->dev->parent;
	struct es_spi_priv *priv = dev_get_priv(dev);
	if((uintptr_t)0x51800000 != (uintptr_t)priv->regs && (uintptr_t)0x71800000 != (uintptr_t)priv->regs)
		return -1;

	if (enable) {
		es_flash_global_wp_cfg(priv, enable);
		dm_gpio_set_value(priv->wp_gpio, enable); //gpio output low, enable protection
		priv->wp_enabled = 1;
	} else {
		dm_gpio_set_value(priv->wp_gpio, enable); //gpio output high, disable protection
		es_flash_global_wp_cfg(priv, enable);
		priv->wp_enabled = 0;
	}

	printf("Bootspi flash write protection %s\n", enable ? "enabled" : "disabled");
	return 0;
}


struct spi_flash *boot_flash = NULL;
int bootspi_probe(char *node_name)
{
	struct udevice *bus, *dev;
	if(boot_flash)
		spi_flash_free(boot_flash);

	boot_flash = NULL;
	int ret = uclass_get_device_by_name(UCLASS_SPI, node_name, &bus);
	if(ret) {
		return ret;
	}
	ret = spi_find_chip_select(bus, 0, &dev);
	if(ret) {
		printf("Invalid chip select :%d (err=%d)\n", 0, ret);
		return ret;
	}

	if (!device_active(dev)) {
		if(device_probe(dev))
			return -1;
	}
	boot_flash = dev_get_uclass_priv(dev);

	if (!boot_flash) {
		printf("Failed to initialize SPI flash at %s (error %d)\n",
		       node_name, ret);
		return 1;
	}
	printf("Success to initialize SPI flash at %s\n",node_name);
	return 0;
}

static int do_spi_flash_probe(int argc, char *const argv[])
{
	const char *node_name_d0 = "spi@51800000";
	const char *node_name_d1 = "spi@71800000";
	char *node_name = (char *)node_name_d0;
	if (argc > 3) {
		printf("Usage: bootspi probe <0|1>\n");
		return CMD_RET_USAGE;
	}
	if (argc == 2) {
		int dienum = simple_strtoul(argv[1], NULL, 10);
		if (dienum != 0 && dienum != 1) {
			printf("Invalid argument. Use 0 to select die0 bootspi flash and 1 to elect die1 bootspi flash.\n");
			return CMD_RET_USAGE;
		}
		if(dienum) {
			node_name = (char *)node_name_d1;
		}
	}
	return bootspi_probe(node_name);
}

static int do_bootspi_wp_cfg(int argc, char * const argv[])
{
	if (argc == 1) {
		printf("Usage: bootspi_wp <0|1> %x\n",argc);
		return CMD_RET_USAGE;
	}
	int enable = simple_strtoul(argv[1], NULL, 10);
	if (enable != 0 && enable != 1) {
		printf("Invalid argument. Use 0 to disable and 1 to enable write protection.\n");
		return CMD_RET_USAGE;
	}

	es_bootspi_wp_cfg(boot_flash, enable);
	return CMD_RET_SUCCESS;
}

static int do_spi_region_wp(int argc, char * const argv[])
{
	if (argc != 4) {
		printf("Usage: spi_region_wp <addr> <size> <enable>\n");
		return CMD_RET_USAGE;
	}

	void *addr = (void *)simple_strtoul(argv[1], NULL, 16);
	int size = simple_strtoul(argv[2], NULL, 16);
	bool lock = simple_strtoul(argv[3], NULL, 16);

	int ret = es_flash_region_wp_cfg(boot_flash, addr, size, lock);
	if (ret == 0) {
		printf("SPI region 0x%px-0x%px write protection %s succeeded.\n",
			addr, addr + size , lock ? "enable" : "disable");
	} else {
		printf("SPI region 0x%px-0x%px write protection %s failed with error code %d.\n",
			addr, addr + size, lock ? "enable" : "disable", ret);
	}

	return ret == 0 ? CMD_RET_SUCCESS : CMD_RET_FAILURE;
}

static int do_bootspi_flash(struct cmd_tbl *cmdtp, int flag, int argc,
			char *const argv[])
{
	const char *cmd;
	int ret;

	/* need at least two arguments */
	if (argc < 2)
		return CMD_RET_USAGE;

	cmd = argv[1];
	--argc;
	++argv;

	if (strcmp(cmd, "probe") == 0)
		return do_spi_flash_probe(argc, argv);

	/* The remaining commands require a selected device */
	if (!boot_flash) {
		puts("No BOOTSPI flash selected. Please run `bootspi probe'\n");
		return CMD_RET_FAILURE;
	}
	if (strcmp(cmd, "wp") == 0)
		ret = do_bootspi_wp_cfg(argc, argv);
	else if (strcmp(cmd, "region_wp") == 0)
		ret = do_spi_region_wp(argc, argv);
	else
		ret = CMD_RET_USAGE;

	return ret;
}

U_BOOT_LONGHELP(bootspi,
	"probe [dienum]	- init flash device on given die bootspi number\n"
	"bootspi wp <0|1>	- read `len' bytes starting at\n"
	"					    Enable or disable BootSPI write protection\n"
	"				    <0|1> - 0 to disable, 1 to enable write protection\n"
	"bootspi region_wp addr size lock	Enable/Disable write protection for boot spi flash region\n"
	"					- addr: start address (hex, 64K aligned)\n"
	"					- size: size in bytes (hex, 64K aligned)\n"
	"					- enable: 1 to enable, 0 to disable\n"
);

U_BOOT_CMD(
	bootspi,	5,	1,	do_bootspi_flash,
	"BOOTSPI flash sub-system", bootspi_help_text
);

int es_bootspi_write_protection_init(void)
{
	if(!bootspi_probe("spi@51800000")) {
		es_bootspi_wp_cfg(boot_flash, 1);
		spi_flash_free(boot_flash);
		boot_flash = NULL;
	}

	if(!bootspi_probe("spi@71800000")) {
		es_bootspi_wp_cfg(boot_flash, 1);
		spi_flash_free(boot_flash);
		boot_flash = NULL;
	}
	return 0;
}
