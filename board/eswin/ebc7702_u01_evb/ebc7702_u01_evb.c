// SPDX-License-Identifier: GPL-2.0
/*
 * u2 evb board init
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
 * Authors: Joey Zheng <zhengyu@eswincomputing.com>
 */
#include <common.h>
#include <dm.h>
#include <linux/delay.h>
#include <linux/io.h>
#include "env.h"
#include <errno.h>
#include <blk.h>
#include <net.h>
#include <stdlib.h>
#include <init.h>
#include <spi.h>
#include <spi_flash.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>
#include <dm/pinctrl.h>
#include <u-boot/crc.h>
#include <eic7700_common.h>
#include <eswin/cpu.h>
#include <asm/gpio.h>
#include <fdt_support.h>

int misc_init_r(void)
{

#ifdef CONFIG_ESWIN_PMP
	eswin_pmp_init();
#endif

#if defined(CONFIG_ESWIN_SPI)
	es_bootspi_write_protection_init();
#endif

	if (NULL == env_get("fdtfile")) {
		env_set("fdtfile","eswin/ebc7702-u01-evb.dtb");
	}
	eswin_update_bootargs();
	return 0;
}



void irq_mux_route(void)
{
	unsigned int val;

	/* Route all interrupts from default LPCPU/SCPU to MCPU
	 * I2C0 bit16
	 * I2C1 bit15
	 * RTC  bit14~13
	 * SPI  bit11~10
	 * DMA  bit9
	 * MPMP bit8
	 * TIMER1 bit5~4
	 * TIMER2 bit3~2
	 * TIMER3 bit1~0
	 *
	 * Route GPIO, TIMER0 to LPCPU
	*/

	val = 0x1040;
	writel(val,(void *)(0x51810000+0x3c0));
	writel(val,(void *)(0x71810000+0x3c0));
}

int board_init(void)
{
	irq_mux_route();
	return 0;
}

int board_late_init(void)
{
	return 0;
}

int ft_board_setup(void *blob, struct bd_info *bd)
{
	return eic770x_fdt_fixup_matched_memory_banks(blob, bd);
}
