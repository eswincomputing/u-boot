// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 * Author: Xiang Xu <xuxiang@eswincomputing.com>
 */

#include <irq_func.h>
#include <asm/cache.h>

/*
 * cleanup_before_linux() is called just before we call linux
 * it prepares the processor for linux
 *
 * we disable interrupt and caches.
 */
int cleanup_before_linux(void)
{
	disable_interrupts();

	cache_flush();

	return 0;
}
