// SPDX-License-Identifier: GPL-2.0
/*
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

#ifndef ES_MEMPOOL_H
#define ES_MEMPOOL_H

#include <stdint.h>
#include <stddef.h>
#include <linux/compiler.h>

#define ES_MEMPOOL_SUCCESS         0
#define ES_MEMPOOL_ERR_NO_MEMORY  -1
#define ES_MEMPOOL_ERR_FRAGMENTED -2
#define ES_MEMPOOL_ERR_INVALID    -3
#define ES_MEMPOOL_ERR_RESERVED   -4
typedef enum {
    PAGE_FREE = 0,
    PAGE_ALLOC = 1,
    PAGE_RESERVED = 2,
} page_state_t;

typedef struct {
    uintptr_t addr;
    size_t pages;
} alloc_entry_t;

typedef struct {
    uintptr_t base_addr;      /**< Base address of the memory pool */
    size_t pool_size;         /**< Total size of the pool in bytes */
    size_t page_size;         /**< Size of each page in bytes */
    size_t total_pages;       /**< Total pages in the pool */

    page_state_t *page_map;   /**< Array of page states */
    alloc_entry_t *alloc_list;/**< Allocation records */
    size_t alloc_cnt;         /**< Number of active allocations */
    size_t max_alloc_entries; /**< Maximum allocation entries allowed */
} es_mempool_t;

/**
 * @brief Initialize the memory pool.
 * 
 * @param pool Pointer to the memory pool structure.
 * @param base_addr Base address of the pool.
 * @param pool_size Total size of the pool in bytes.
 * @param page_size Page size in bytes.
 * @param max_entries Maximum number of allocation records.
 * @return 0 on success, -1 on failure.
 */
int es_mem_pool_init(es_mempool_t *pool, uintptr_t base_addr, size_t pool_size, size_t page_size, size_t max_entries);

/**
 * @brief Destroy and clean up memory pool resources.
 * 
 * @param pool Pointer to the memory pool.
 */
void es_mem_pool_destroy(es_mempool_t *pool);

/**
 * @brief Allocate memory from the pool.
 * 
 * @param pool Pointer to the memory pool.
 * @param size Size in bytes to allocate.
 * @return Pointer to allocated memory, or NULL on failure.
 */
void *es_mem_pool_alloc(es_mempool_t *pool, size_t size);

/**
 * @brief Try to allocate memory from the memory pool with diagnostic feedback.
 * 
 * This function attempts to allocate a memory block of the given size from the pool.
 * Unlike `es_mem_pool_alloc()`, this function provides extended feedback to help
 * the caller decide whether memory is insufficient or simply fragmented.
 * 
 * @param pool Pointer to the memory pool.
 * @param size Number of bytes to allocate.
 * @param out_ptr Pointer to a void* variable that will receive the allocated address on success.
 * 
 * @return One of the following values:
 *   - ES_MEMPOOL_SUCCESS (0): Allocation succeeded, *out_ptr is valid.
 *   - ES_MEMPOOL_ERR_FRAGMENTED (-2): Allocation failed due to fragmentation, 
 *     but total free space is sufficient; caller may attempt defragmentation and retry.
 *   - ES_MEMPOOL_ERR_NO_MEMORY (-1): Allocation failed due to insufficient memory in pool.
 *   - ES_MEMPOOL_ERR_INVALID (-3): Invalid parameters (e.g. null pointer or zero size).
 * 
 * Example usage:
 *   void *ptr = NULL;
 *   int ret = es_mem_pool_try_alloc(&pool, size, &ptr);
 *   if (ret == ES_MEMPOOL_SUCCESS) {
 *       // Allocation successful
 *   } else if (ret == ES_MEMPOOL_ERR_FRAGMENTED) {
 *       // Suggest user to run es_mem_pool_defrag() and retry
 *   } else {
 *       // Allocation failed permanently (not enough memory)
 *   }
 */
int es_mem_pool_try_alloc(es_mempool_t *pool, size_t size, void **out_ptr);

/**
 * @brief Allocate memory at a specific address in the pool.
 * 
 * @param pool Pointer to the memory pool.
 * @param addr Target address within the pool.
 * @param size Size in bytes to allocate.
 * @return 0 on success, negative error code on failure.
 */
int es_mem_pool_alloc_at(es_mempool_t *pool, uintptr_t addr, size_t size);

/**
 * @brief Free a previously allocated memory block.
 * 
 * @param pool Pointer to the memory pool.
 * @param ptr Pointer to the memory block to free.
 */
void es_mem_pool_free(es_mempool_t *pool, void *ptr);

/**
 * @brief Reserve memory range in the pool, preventing allocation in that area.
 * 
 * @param pool Pointer to the memory pool.
 * @param addr Start address to reserve.
 * @param size Size in bytes to reserve.
 */
void es_mem_pool_reserve(es_mempool_t *pool, uintptr_t addr, size_t size);

/**
 * @brief Dump internal state of the memory pool for debugging.
 * 
 * @param pool Pointer to the memory pool.
 */
void es_mem_pool_dump(es_mempool_t *pool);

/**
 * @brief Information of a defragmentation move.
 */
typedef struct {
    uintptr_t src_addr;  /**< Source address of data to move */
    uintptr_t dst_addr;  /**< Destination address to move data to */
    size_t size;         /**< Size in bytes to move */
} defrag_info_t;

/**
 * @brief Callback type used during defragmentation to notify required moves.
 * 
 * @param user_data User defined pointer passed through.
 * @param info Pointer to the defrag_info_t describing the move.
 */
typedef void (*es_mempool_defrag_cb_t)(void *user_data, const defrag_info_t *info);

/**
 * @brief Perform defragmentation, notifying application of required moves via callback.
 * 
 * @param pool Pointer to the memory pool.
 * @param cb Callback function to be called for each data move.
 * @param user_data User data pointer passed to callback.
 * @return Number of move segments, or negative on error.
 */
int es_mem_pool_defrag(es_mempool_t *pool, es_mempool_defrag_cb_t cb, void *user_data);

/**
 * @brief Count the total number of free pages currently available.
 * 
 * @param pool Pointer to the memory pool.
 * @return Number of free pages.
 */
size_t es_mem_pool_count_free_pages(es_mempool_t *pool);

#endif /* ES_MEMPOOL_H */

