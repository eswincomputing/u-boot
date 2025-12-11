// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
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
 * Authors: Xiang Xu <xuxiang@eswincomputing.com>
 */

#include <efi_loader.h>
#include <fdtdec.h>
#include <init.h>
#include <mapmem.h>
#include <asm/global_data.h>
#include <linux/sizes.h>
#include <asm/io.h>
DECLARE_GLOBAL_DATA_PTR;

#define MR_TYPE_READ 1
#define MR_TYPE_WRITE 0

#define DDR_CTRL_ADDR 0x52300000
#define CONTRLLER_NUM 2

#define DDR_CTRL_MSTR0	0x10000
#define DDR_CTRL_MRCTRL0 0x10080
#define DDR_CTRL_MRCTRL1 0x10084

#define DDR_CTRL_MRSTAT 0x10090

#define DDR_CTRL_MRRDATA0 0x10094
#define DDR_CTRL_MRRDATA1 0x10098
/*
mr_type:0-write;1-read
rank: Ox1 - select rank 0 onlyOx2- select rank 1 onlyOx5 - select ranks 0 and 2OxA - select ranks 1 and 3OxF - select ranks 0, 1, 2 and 3
addr:mrX*/
int mr_operation(uint32_t ctrl_base_addr, uint8_t mr_type, uint8_t rank, uint8_t addr)
{
    uint32_t value = 0;
    uint32_t data = 0;
    debug("mr_type:%d rank:%d addr:0x%x data:0x%x\n\r", mr_type, rank, addr, data);
    /* 1. If performing MRR and using MRRDATAO/MRRDATA1 registers, write the MRCTRLO.mrr_done_clr to '1'.
    This bit is self-clearing, and clears the MRSTAT.mrr_done register. */
    value = readl((void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRCTRL0));
    value = value | (1 << 24);
    writel(value, (void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRCTRL0));

    /* 2. Poll MRSTAT.mr_wr_busy until it is 'O'. This checks that there is no outstanding MR transaction.
    No writes must be performed to MRCTRLO and MRCTRL1 if MRSTAT .mr_wr_busy=1. */
    while (1) {
        value = readl((void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRSTAT));
        if (0x0 == (value & 0x1)) {
            break;
        }
    }

    /*   3. Write the MRCTRLO.mr_type,MRCTRLO.mr_addr,MRCTRLO.mr_rank and (for MRWs) MRCTRL1 .mr_data to define the MR transaction. */
    value = readl((void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRCTRL0));
    value = value & 0xFFFF0FFE; // if can't read,modify 0xFFFF0FCE to 0xFFFF0FFE
    value = value | (mr_type & 0x1) | ((rank & 0x3) << 4);
    writel(value, (void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRCTRL0));
    value = ((addr & 0xff) << 8) | (data & 0xff);
    writel(value, (void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRCTRL1));

    /* 4. In a separate APB transaction, write the MRCTRLO.mr wr to '1'. This bit is self-clearing, and triggers the MR transaction.
       The DDRCTL then asserts the MRSTAT.mr wr busy while it performs the MR transaction to SDRAM, and no further accesses can be initiated until it is de-asserted. */
    value = readl((void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRCTRL0));
    value = value | (1u << 31);
    writel(value, (void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRCTRL0));
    if (MR_TYPE_READ == mr_type) {
        /* 5. If performing MRR, the MRR data is made available on the hif_mrr_data signals and theMRRDATAO/MRRDATA1 registers. */
        while (1) {
            value = readl((void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRSTAT));
            if (0x1 == ((value >> 16) & 0x1)) {
                break;
            }
        }
        value = readl((void __iomem *)(uintptr_t)(ctrl_base_addr + DDR_CTRL_MRRDATA0));
    }
    return value;
}

static int count_ones(u32 x)
{
    int cnt = 0;
    while(x) {
        x &= (x - 1);
        cnt++;
    }
    return cnt;
}

uint32_t ddr_sw_mr_size_mb(uint8_t mr_value, uint32_t rank_num)
{
    uint32_t chip_num = 0;
    uint32_t density = 0;
    uint8_t cache = ((mr_value >> 6) & 0x3);
    uint32_t chn_ddr_size_mb = 0;

    /*0:X16     1:X8  others:reserved*/
    if (0 == cache) {
        chip_num = 2; /* 32/16 */
    } else if (1 == cache) {
        chip_num = 4; /* 32/8 */
    } else {
        printf("!!!!!!!!!!!!!!!!!!!!not support IO Width:%d\n\r", cache);
        while (1)
            ;
    }
    cache = ((mr_value >> 2) & 0xf);
    switch (cache) {
    case 0:
        density = 2;
        break;
    case 1:
        density = 3;
        break;
    case 2:
        density = 4;
        break;
    case 3:
        density = 6;
        break;
    case 4:
        density = 8;
        break;
    case 5:
        density = 12;
        break;
    case 6:
        density = 16;
        break;
    case 7:
        density = 24;
        break;
    case 8:
        density = 32;
        break;
    default:
        printf("!!!!!!!!!!!!!!!!!!!!not support density:%d\n\r", cache);
        while (1)
            ;
        break;
    }
    chn_ddr_size_mb = (density * 1024 /* gb to mb */ * chip_num * count_ones(rank_num) /* rank */) >> 3; /* bit to Byte */
    return chn_ddr_size_mb;
}

/* 32 GB */
#define DDR_SIZE_MAX    	0x800000000

/* 128 MB offset */
#define RAM_BASE_OFFSET 	0x8000000
#define DDR_MEM_ADDR0   0x0080000000ul
#define DDR_MEM_ADDR1   0x2000000000ul

#define DDR_CTRL_ADDR 0x52300000

/* get dram size */
uint64_t get_dram_info(int nid, uint64_t *dram_size)
{
	uint64_t ctrl_base_addr;
	uint64_t dram_size_mb, dram_size_bytes;
	uint32_t reg, rank_num;
	volatile void *ctrl_base;
	if ((nid +1) > CONFIG_NR_DRAM_BANKS) {
		printf("DRAM: Input DRAM BANKS %d is invalid\n", nid);
		return -1;
	}

	ctrl_base_addr = DDR_CTRL_ADDR + nid*0x20000000;
	ctrl_base = (volatile void *)ctrl_base_addr;
	reg = readl(ctrl_base + DDR_CTRL_MSTR0);
	rank_num = (reg >> 24) & 0xf;
	dram_size_mb = (ddr_sw_mr_size_mb(mr_operation(ctrl_base_addr, MR_TYPE_READ, 0x1, 0x8), rank_num)) * 2;
	dram_size_bytes = dram_size_mb << 20;

	#if (ENABLE_DDR_ECC == 1)
		dram_size_bytes = dram_size_bytes - (dram_size_bytes / 8);
		debug("ECC Eabled\n");
	#else
		debug("ECC Disabled\n");
	#endif
	*dram_size = dram_size_bytes;

	return 0;
}
int dram_init(void)
{
	int ret = fdtdec_setup_mem_size_base();

	if(ret) {
		return ret;
	}

	get_dram_info(0, &gd->ram_size);

	return ret;
}

int dram_init_banksize(void)
{
	int ret = 0;
	uint64_t size;
	uint64_t dram_size;

	ret = fdtdec_setup_memory_banksize();

	if(ret) {
		return ret;
	}

	for (uint64_t i = size = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		if(DDR_MEM_ADDR0 == gd->bd->bi_dram[i].start) {
			get_dram_info(0, &dram_size);
			gd->bd->bi_dram[i].size = dram_size;
		} else if(DDR_MEM_ADDR1 == gd->bd->bi_dram[i].start) {
			get_dram_info(1, &dram_size);
			gd->bd->bi_dram[i].size = dram_size;
		}
	}

	return ret;
}

phys_addr_t board_get_usable_ram_top(phys_size_t total_size)
{
#ifdef CONFIG_64BIT
	/*
	 * Ensure that we run from first 4GB so that all
	 * addresses used by U-Boot are 32bit addresses.
	 *
	 * This in-turn ensures that 32bit DMA capable
	 * devices work fine because DMA mapping APIs will
	 * provide 32bit DMA addresses only.
	 */
	if (gd->ram_top > SZ_4G)
		return SZ_4G;
#endif
	return gd->ram_top;
}

static int update_memory_nodes_match_start(void *fdt, u64 start[], u64 size[], int banks)
{
    int offset = -1;
    const char *dtype;
    int len, ret;
    const fdt64_t *reg_prop;
    fdt64_t reg_val[2];
    for(int i = 0; i < banks; i++) {
        debug("Searching for memory node with start=0x%llx to update...\n", start[i]);
        while ((offset = fdt_next_node(fdt, offset, NULL)) >= 0) {
            dtype = fdt_getprop(fdt, offset, "device_type", &len);
            if (!dtype || strcmp(dtype, "memory") != 0)
                continue;

            reg_prop = fdt_getprop(fdt, offset, "reg", &len);
            if (!reg_prop || len < 16) {
                debug("Node offset %d: invalid reg property\n", offset);
                continue;
            }

            u64 node_start = fdt64_to_cpu(reg_prop[0]);
            u64 node_size  = fdt64_to_cpu(reg_prop[1]);

            debug("Found memory node at offset %d: start=0x%llx size=0x%llx\n", 
                offset, node_start, node_size);

            if (node_start == start[i]) {
                debug("Match found. Updating node at offset %d to start=0x%llx size=0x%llx\n",
                    offset, start[i], size[i]);

                reg_val[0] = cpu_to_fdt64(start[i]);
                reg_val[1] = cpu_to_fdt64(size[i]);

                ret = fdt_setprop(fdt, offset, "reg", reg_val, sizeof(reg_val));
                if (ret < 0) {
                    debug("Failed to update reg: %s\n", fdt_strerror(ret));
                    return ret;
                }
                break;
            }
        }
    }

    return 0;
}

int eic770x_fdt_fixup_matched_memory_banks(void *blob, struct bd_info *bd)
{
	u32 banks = 0;
	u64 start[CONFIG_NR_DRAM_BANKS];
	u64 size[CONFIG_NR_DRAM_BANKS];

	for (int i = 0; i < CONFIG_NR_DRAM_BANKS; i++) {
		if(gd->bd->bi_dram[i].start != 0 && gd->bd->bi_dram[i].size != 0) {
			start[i] = gd->bd->bi_dram[i].start;
			size[i] = gd->bd->bi_dram[i].size;
			banks++;
		}
	}
	return update_memory_nodes_match_start(blob, start, size, banks);
}
