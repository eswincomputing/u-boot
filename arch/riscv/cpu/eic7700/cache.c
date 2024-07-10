// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 * Author: Xiang Xu <xuxiang@eswincomputing.com>
 */

#include <common.h>
#include <cpu_func.h>
#include <asm/io.h>
#include <asm/cache.h>

#define SIFIVE_L3_FLUSH64 0x200
#define SIFIVE_L3_FLUSH64_LINE_LEN          64
#define CONFIG_SIFIVE_DIE0_L3_FLUSH_START        0x80000000UL
#define CONFIG_SIFIVE_DIE0_L3_FLUSH_SIZE         0x80000000UL
#define CONFIG_SIFIVE_DIE1_L3_FLUSH_START      0x2000000000UL
#define CONFIG_SIFIVE_DIE1_L3_FLUSH_SIZE         0x80000000UL

#define L3_DIE0_CTRL_BASE  0x2010000UL
#define L3_DIE1_CTRL_BASE 0x22010000UL

void sifive_l3_flush64_range(unsigned long start, unsigned long len)
{
    unsigned long line;
    unsigned long l3_base = L3_DIE0_CTRL_BASE;

    if(!len)
        return;

    len = len + (start % SIFIVE_L3_FLUSH64_LINE_LEN);
    start = ALIGN_DOWN(start, SIFIVE_L3_FLUSH64_LINE_LEN);

    /* make sure the address is in the range */
    if((start >= CONFIG_SIFIVE_DIE0_L3_FLUSH_START) &&
       ((start + len) <= (CONFIG_SIFIVE_DIE0_L3_FLUSH_START + CONFIG_SIFIVE_DIE0_L3_FLUSH_SIZE)))
    {
        l3_base = L3_DIE0_CTRL_BASE;
    }
    else if((start >= CONFIG_SIFIVE_DIE1_L3_FLUSH_START) &&
       ((start + len) <= (CONFIG_SIFIVE_DIE1_L3_FLUSH_START + CONFIG_SIFIVE_DIE1_L3_FLUSH_SIZE)))
    {
        l3_base = L3_DIE1_CTRL_BASE;
    }
    else
    {
        // printf("L2CACHE: flush64 out of range: %lx(%lx), skip flush\n", start, len);
        return;
    }

    for (line = start; line < start + len; line += SIFIVE_L3_FLUSH64_LINE_LEN)
    {
        writeq(line,(void __iomem*)(l3_base + SIFIVE_L3_FLUSH64));
        mb();
    }
}
void flush_dcache_all(void)
{

}

void flush_dcache_range(unsigned long start, unsigned long end)
{
    sifive_l3_flush64_range(start, end - start);
}

void invalidate_dcache_range(unsigned long start, unsigned long end)
{
    sifive_l3_flush64_range(start, end - start);
}