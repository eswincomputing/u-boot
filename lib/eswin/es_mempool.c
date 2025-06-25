// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN Memory Pool Module
 *
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


#include "eswin/es_mempool.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int record_alloc(es_mempool_t *pool, uintptr_t addr, size_t pages)
{
    if (pool->alloc_cnt >= pool->max_alloc_entries)
        return ES_MEMPOOL_ERR_NO_MEMORY;

    int i = 0;
    while (i < (int)pool->alloc_cnt && pool->alloc_list[i].addr < addr)
        i++;

    for (int j = (int)pool->alloc_cnt; j > i; --j)
        pool->alloc_list[j] = pool->alloc_list[j - 1];

    pool->alloc_list[i].addr = addr;
    pool->alloc_list[i].pages = pages;
    pool->alloc_cnt++;

    return ES_MEMPOOL_SUCCESS;
}

static size_t lookup_alloc(es_mempool_t *pool, uintptr_t addr)
{
    for (size_t i = 0; i < pool->alloc_cnt; i++) {
        if (pool->alloc_list[i].addr == addr)
            return pool->alloc_list[i].pages;
    }
    return 0;
}

static void remove_alloc(es_mempool_t *pool, uintptr_t addr)
{
    for (size_t i = 0; i < pool->alloc_cnt; ++i) {
        if (pool->alloc_list[i].addr == addr) {
            for (size_t j = i; j < pool->alloc_cnt - 1; ++j)
                pool->alloc_list[j] = pool->alloc_list[j + 1];
            pool->alloc_cnt--;
            return;
        }
    }
}

int es_mem_pool_init(es_mempool_t *pool, uintptr_t base_addr, size_t pool_size, size_t page_size, size_t max_entries)
{
    if (!pool || page_size == 0 || pool_size < page_size)
        return ES_MEMPOOL_ERR_INVALID;

    pool->base_addr = base_addr;
    pool->pool_size = pool_size;
    pool->page_size = page_size;
    pool->total_pages = pool_size / page_size;
    pool->alloc_cnt = 0;
    pool->max_alloc_entries = max_entries;

    pool->page_map = calloc(pool->total_pages, sizeof(page_state_t));
    pool->alloc_list = calloc(max_entries, sizeof(alloc_entry_t));

    return (pool->page_map && pool->alloc_list) ? ES_MEMPOOL_SUCCESS : ES_MEMPOOL_ERR_NO_MEMORY;
}

void es_mem_pool_destroy(es_mempool_t *pool)
{
    if (!pool) return;
    free(pool->page_map);
    free(pool->alloc_list);
    memset(pool, 0, sizeof(es_mempool_t));
}

void es_mem_pool_reserve(es_mempool_t *pool, uintptr_t addr, size_t size)
{
    if (!pool) return;

    size_t start = (addr - pool->base_addr) / pool->page_size;
    size_t pages = (size + pool->page_size - 1) / pool->page_size;

    for (size_t i = 0; i < pages && (start + i < pool->total_pages); ++i)
        pool->page_map[start + i] = PAGE_RESERVED;
}

void *es_mem_pool_alloc(es_mempool_t *pool, size_t size)
{
    void *ptr = NULL;
    if (es_mem_pool_try_alloc(pool, size, &ptr) == ES_MEMPOOL_SUCCESS)
        return ptr;
    return NULL;
}

int es_mem_pool_try_alloc(es_mempool_t *pool, size_t size, void **out_ptr)
{
    if (!pool || size == 0 || !out_ptr) return ES_MEMPOOL_ERR_INVALID;

    size_t pages_needed = (size + pool->page_size - 1) / pool->page_size;

    for (size_t i = 0; i <= pool->total_pages - pages_needed; ++i) {
        int found = 1;
        for (size_t j = 0; j < pages_needed; ++j) {
            if (pool->page_map[i + j] != PAGE_FREE) {
                found = 0;
                break;
            }
        }
        if (found) {
            for (size_t j = 0; j < pages_needed; ++j)
                pool->page_map[i + j] = PAGE_ALLOC;

            uintptr_t addr = pool->base_addr + i * pool->page_size;
            record_alloc(pool, addr, pages_needed);
            *out_ptr = (void *)addr;
            return ES_MEMPOOL_SUCCESS;
        }
    }

    size_t free_pages = es_mem_pool_count_free_pages(pool);
    if (free_pages * pool->page_size >= size)
        return ES_MEMPOOL_ERR_FRAGMENTED; // Fragmentation
    return ES_MEMPOOL_ERR_NO_MEMORY;      // Not enough memory
}

int es_mem_pool_alloc_at(es_mempool_t *pool, uintptr_t addr, size_t size)
{
    if (!pool || size == 0) return ES_MEMPOOL_ERR_INVALID;
    if (addr < pool->base_addr || addr + size > pool->base_addr + pool->pool_size)
        return ES_MEMPOOL_ERR_INVALID;
    if (addr % pool->page_size != 0) return ES_MEMPOOL_ERR_INVALID;

    size_t offset = addr - pool->base_addr;
    size_t start_page = offset / pool->page_size;
    size_t pages = (size + pool->page_size - 1) / pool->page_size;

    if (start_page + pages > pool->total_pages)
        return ES_MEMPOOL_ERR_NO_MEMORY;

    for (size_t i = 0; i < pages; ++i) {
        if (pool->page_map[start_page + i] != PAGE_FREE)
            return ES_MEMPOOL_ERR_RESERVED;
    }

    for (size_t i = 0; i < pages; ++i)
        pool->page_map[start_page + i] = PAGE_ALLOC;

    return record_alloc(pool, addr, pages);
}

void es_mem_pool_free(es_mempool_t *pool, void *ptr)
{
    if (!pool || !ptr) return;

    uintptr_t addr = (uintptr_t)ptr;
    if (addr < pool->base_addr || addr >= pool->base_addr + pool->pool_size)
        return;

    size_t offset = addr - pool->base_addr;
    size_t start_page = offset / pool->page_size;
    size_t pages = lookup_alloc(pool, addr);

    if (pages == 0) return;

    for (size_t i = 0; i < pages && (start_page + i < pool->total_pages); ++i) {
        if (pool->page_map[start_page + i] == PAGE_ALLOC)
            pool->page_map[start_page + i] = PAGE_FREE;
    }

    remove_alloc(pool, addr);
}

/**
 * @brief Count free pages in the pool.
 */
size_t es_mem_pool_count_free_pages(es_mempool_t *pool)
{
    if (!pool) return 0;
    size_t free_pages = 0;
    for (size_t i = 0; i < pool->total_pages; ++i) {
        if (pool->page_map[i] == PAGE_FREE)
            ++free_pages;
    }
    return free_pages;
}

/**
 * @brief Check whether a range of pages is writable (not reserved).
 */
static int is_writable_range(es_mempool_t *pool, size_t start_idx, size_t pages)
{
    if (start_idx + pages > pool->total_pages)
        return 0;
    for (size_t i = 0; i < pages; ++i) {
        if (pool->page_map[start_idx + i] == PAGE_RESERVED)
            return 0;
    }
    return 1;
}

int es_mem_pool_defrag(es_mempool_t *pool, es_mempool_defrag_cb_t cb, void *user_data)
{
    if (!pool || !cb) return ES_MEMPOOL_ERR_INVALID;

    size_t write_idx = 0;
    int move_count = 0;

    for (size_t i = 0; i < pool->alloc_cnt; ++i) {
        uintptr_t curr_addr = pool->alloc_list[i].addr;
        size_t pages = pool->alloc_list[i].pages;
        size_t curr_idx = (curr_addr - pool->base_addr) / pool->page_size;

        // Skip write_idx until a suitable non-reserved block is found
        while (!is_writable_range(pool, write_idx, pages)) {
            write_idx++;
            if (write_idx >= pool->total_pages)
                return move_count;  // No more space
        }

        // If already in the correct spot, skip
        if (curr_idx == write_idx) {
            write_idx += pages;
            continue;
        }

        uintptr_t new_addr = pool->base_addr + write_idx * pool->page_size;
        defrag_info_t info = {
            .src_addr = curr_addr,
            .dst_addr = new_addr,
            .size = pages * pool->page_size,
        };
        cb(user_data, &info);

        // Update page map
        for (size_t j = 0; j < pages; ++j) {
            pool->page_map[curr_idx + j] = PAGE_FREE;
            pool->page_map[write_idx + j] = PAGE_ALLOC;
        }

        // Update alloc record
        pool->alloc_list[i].addr = new_addr;
        write_idx += pages;
        ++move_count;
    }

    return move_count;
}

void es_mem_pool_dump(es_mempool_t *pool)
{
    if (!pool) return;

    printf("Memory Pool @ 0x%08lx size=%zu page_size=%zu pages=%zu\n",
           pool->base_addr, pool->pool_size, pool->page_size, pool->total_pages);
    printf("Memory Pool @ count free pages %zu free size=%zu KB\n", es_mem_pool_count_free_pages(pool), (es_mem_pool_count_free_pages(pool) * pool->page_size) / 1024);
    for (size_t i = 0; i < pool->alloc_cnt; ++i) {
        printf("Alloc %zu: addr=0x%08lx size=%zu KB\n", i,
               pool->alloc_list[i].addr,
               (pool->alloc_list[i].pages * pool->page_size) / 1024);
    }
    // for (size_t i = 0; i < pool->total_pages; ++i) {
    //     char c = '.';
    //     if (pool->page_map[i] == PAGE_ALLOC) c = 'A';
    //     else if (pool->page_map[i] == PAGE_RESERVED) c = 'R';
    //     printf("%c", c);
    //     if ((i + 1) % 64 == 0) printf("\n");
    // }
    // printf("\n");
}
