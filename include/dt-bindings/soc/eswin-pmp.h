// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef _DT_BINDINGS_ESWIN_PMP_H
#define _DT_BINDINGS_ESWIN_PMP_H

/************************************************************************
 *  Memory Mode
 ************************************************************************/
#define MEM_MODE_FLAT           0
#define MEM_MODE_INTERLEAVE     1


/************************************************************************
 *  PMP region definition
 ************************************************************************/
#define MEM_SIZE_32K            0x00 0x8000
#define MEM_SIZE_64K            0x00 0x10000
#define MEM_SIZE_128K           0x00 0x20000
#define MEM_SIZE_256K           0x00 0x40000
#define MEM_SIZE_512K           0x00 0x80000
#define MEM_SIZE_1M             0x00 0x100000
#define MEM_SIZE_2M             0x00 0x200000
#define MEM_SIZE_4M             0x00 0x400000
#define MEM_SIZE_8M             0x00 0x800000
#define MEM_SIZE_16M            0x00 0x1000000
#define MEM_SIZE_32M            0x00 0x2000000
#define MEM_SIZE_64M            0x00 0x4000000
#define MEM_SIZE_128M           0x00 0x8000000
#define MEM_SIZE_256M           0x00 0x10000000
#define MEM_SIZE_512M           0x00 0x20000000
#define MEM_SIZE_1G             0x00 0x40000000
#define MEM_SIZE_2G             0x00 0x80000000
#define MEM_SIZE_4G             0x00 0x100000000
#define MEM_SIZE_8G             0x00 0x200000000
#define MEM_SIZE_16G            0x00 0x400000000
#define MEM_SIZE_32G            0x00 0x800000000
#define MEM_SIZE_64G            0x10 0x00000000
#define MEM_SIZE_128G           0x20 0x00000000
#define MEM_SIZE_256G           0x40 0x00000000
#define MEM_SIZE_512G           0x80 0x00000000

/************************************************************************
 *  AXPROT definition
 ************************************************************************/
#define		NON_SECURE		0
#define		SECURE			1

//this master order and content must not chage
#define MASTER_ID_LPCPU         0
#define MASTER_ID_AONDMA        1
#define MASTER_ID_DSP           2
#define MASTER_ID_NPU           3
#define MASTER_ID_RESERVED      4	//Reserved, don't to use it
#define MASTER_ID_U84           5
#define MASTER_ID_GPU           6
#define MASTER_ID_G2D0          7
#define MASTER_ID_G2D1          8
#define MASTER_ID_VD            9
#define MASTER_ID_VC            10
#define MASTER_ID_JE            11
#define MASTER_ID_JD            12
#define MASTER_ID_ISP           13
#define MASTER_ID_ISP1          14
#define MASTER_ID_DVP2AXI       15
#define MASTER_ID_DWE           16
#define MASTER_ID_DC            17
#define MASTER_ID_HDMI          18
#define MASTER_ID_ETH0          19
#define MASTER_ID_ETH1          20
#define MASTER_ID_SATA          21
#define MASTER_ID_DMA           22
#define MASTER_ID_EMMC          23
#define MASTER_ID_SD            24
#define MASTER_ID_SDIO          25
#define MASTER_ID_PCIE          26
#define MASTER_ID_SCPU          27
#define MASTER_ID_USB0          33
#define MASTER_ID_USB1          34
#define MASTER_ID_MAX           35

#endif /* _DT_BINDINGS_ESWIN_PMP_H */
