// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN drm driver
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
 * Authors: Denglei <denglei@eswincomputing.com>
 */

#include <dm.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>
#include <linux/compat.h>
#include "eswin_dc_mmu.h"
#include "eswin_vo_log.h"
#include "eswin_dc.h"

#define PAGE_SIZE 4096
#define DIE1_MMU_ADDR_BASE 0x20e1000000
#define DIE0_MMU_ADDR_BASE 0xe1000000

extern void sifive_l3_flush64_range(unsigned long start, unsigned long len);

void flush_mmu_cache(struct udevice *dev)
{
	u32 node_id;
	int ret;

	ret = dev_read_u32(dev, "numa-node-id", &node_id);
	if (ret) {
		vo_err("failed to get node id, ret %d\n", ret);
		return;
	}

	if (node_id) {
		sifive_l3_flush64_range(DIE1_MMU_ADDR_BASE, 0x1000000);
	} else {
		sifive_l3_flush64_range(DIE0_MMU_ADDR_BASE, 0x1000000);
	}
}

static u64 die0_addr_base = DIE0_MMU_ADDR_BASE;
static u64 die1_addr_base = DIE1_MMU_ADDR_BASE;

static inline void *dma_alloc_wc(struct udevice *dev, size_t size, u64 *dma_addr)
{
	u64 addr;
	u32 node_id;
	int ret;

	ret = dev_read_u32(dev, "numa-node-id", &node_id);
	if (ret) {
		vo_err("failed to get node id, ret %d\n", ret);
		return NULL;
	}

	if (ALIGN(size, PAGE_SIZE) > 0x1000000) {
		vo_err("can not alloc the size:%ld\n", size);
		return NULL;
	}

	if (node_id) {
		addr = die1_addr_base;
		die1_addr_base += ALIGN(size, PAGE_SIZE);
		if (die1_addr_base >= DIE1_MMU_ADDR_BASE + 0x1000000) {
			die1_addr_base =  DIE1_MMU_ADDR_BASE;
			addr = die1_addr_base;
		}
	} else {
		addr = die0_addr_base;
		die0_addr_base += ALIGN(size, PAGE_SIZE);
		if (die0_addr_base >= DIE0_MMU_ADDR_BASE + 0x1000000) {
			die0_addr_base =  DIE0_MMU_ADDR_BASE;
			addr = die0_addr_base;
		}
	}
	vo_debug("numa id:%d, addr:0x%llx, size:0x%lx, real size:0x%lx\n",
			 node_id, addr, size, ALIGN(size, PAGE_SIZE));

	*dma_addr = addr;
	return (void *)addr;
}

int _allocate_memory(u32 bytes, void **memory)
{
	void *mem = NULL;

	if (bytes == 0 || memory == NULL) {
		vo_err("has invalid arguments.\n");
		return -EINVAL;
	}

	mem = kmalloc(bytes, GFP_KERNEL);
	if (!mem) {
		vo_err("out of memory.\n");
		return -ENOMEM;
	}

	memset((u8 *)mem, 0, bytes);
	*memory = mem;

	return 0;
}

static u32 _mtlb_offset(u32 address)
{
	return (address & MMU_MTLB_MASK) >> MMU_MTLB_SHIFT;
}

static u32 _stlb_offset(u32 address)
{
	return (address & MMU_STLB_4K_MASK) >> MMU_STLB_4K_SHIFT;
}

static u32 _address_to_index(dc_mmu *mmu, u32 address)
{
	return _mtlb_offset(address) * MMU_STLB_4K_ENTRY_NUM +
	       _stlb_offset(address);
}

static u32 _set_page(u32 page_address, u32 page_address_ext, bool writable)
{
	u32 entry = page_address
		    /* AddressExt */
		    | (page_address_ext << 4)
		    /* Ignore exception */
		    | (0 << 1)
		    /* Present */
		    | (1 << 0);

	if (writable) {
		/* writable */
		entry |= (1 << 2);
	}

	return entry;
}

static void _write_page_entry(u32 *page_entry, u32 entry_value)
{
	*page_entry = entry_value;
}

static u32 _read_page_entry(u32 *page_entry)
{
	return *page_entry;
}

int _allocate_stlb(dc_mmu_stlb_pt *stlb)
{
	dc_mmu_stlb_pt stlb_t = NULL;
	void *mem = NULL;

	mem = kzalloc(sizeof(dc_mmu_stlb), GFP_KERNEL);
	if (!mem)
		return -ENOMEM;

	stlb_t = (dc_mmu_stlb_pt)mem;

	stlb_t->size = MMU_STLB_4K_SIZE;

	*stlb = stlb_t;

	return 0;
}

int _allocate_all_stlb(struct udevice *dev, dc_mmu_stlb_pt *stlb)
{
	dc_mmu_stlb_pt stlb_t = NULL;
	void *mem = NULL;
	void *cookie = NULL;
	u64 dma_addr;
	size_t size;

	mem = kzalloc(sizeof(dc_mmu_stlb), GFP_KERNEL);
	if (!mem)
		return -ENOMEM;

	stlb_t = (dc_mmu_stlb_pt)mem;

	stlb_t->size = MMU_STLB_4K_SIZE * MMU_MTLB_ENTRY_NUM;
	size = ALIGN(stlb_t->size, PAGE_SIZE);

	cookie = dma_alloc_wc(dev, size, &dma_addr);
	if (!cookie) {
		vo_err("Failed to alloc stlb buffer.\n");
		return -ENOMEM;
	}

	stlb_t->logical = cookie;
	stlb_t->physBase = dma_addr;
	memset(stlb_t->logical, 0, size);

	*stlb = stlb_t;

	return 0;
}

int _setup_process_address_space(struct udevice *dev, dc_mmu *mmu)
{
	u32 *map = NULL;
	u32 free, i;
	u32 dynamic_mapping_entries, address;
	dc_mmu_stlb_pt all_stlb;
	int ret = 0;

	dynamic_mapping_entries = MMU_MTLB_ENTRY_NUM;
	mmu->dynamic_mapping_start = 0;
	mmu->page_table_size = dynamic_mapping_entries * MMU_STLB_4K_SIZE;

	mmu->page_table_entries = mmu->page_table_size / sizeof(u32);

	ret = _allocate_memory(mmu->page_table_size,
			       (void **)&mmu->map_logical);
	if (ret) {
		vo_err("Failed to alloc mmu map buffer.\n");
		return ret;
	}

	ret = _allocate_memory(mmu->page_table_size,
			       (void **)&mmu->page_entry_info);
	if (ret) {
		vo_err("Failed to alloc entry info buffer.\n");
		return ret;
	}

	map = mmu->map_logical;

	/* Initialize free area */
	free = mmu->page_table_entries;
	_write_page_entry(map, (free << 8) | DC_MMU_FREE);
	_write_page_entry(map + 1, ~0U);

	mmu->heap_list = 0;
	mmu->free_nodes = false;

	ret = _allocate_all_stlb(dev, &all_stlb);
	if (ret)
		return ret;

	if(((all_stlb->physBase & (0xffUL << 32)) !=
	       (mmu->mtlb_physical & (0xffUL << 32))))
	{
		vo_err("hi physBase is not same!!!\n");
		return -EFAULT;
	}

	mmu->stlb_physical = all_stlb->physBase;
	mmu->stlb_bytes = all_stlb->size;

	for (i = 0; i < dynamic_mapping_entries; i++) {
		dc_mmu_stlb_pt stlb;
		dc_mmu_stlb_pt *stlbs = (dc_mmu_stlb_pt *)mmu->stlbs;

		ret = _allocate_stlb(&stlb);
		if (ret)
			return ret;

		stlb->physBase = all_stlb->physBase + i * MMU_STLB_4K_SIZE;
		stlb->logical =
			all_stlb->logical + i * MMU_STLB_4K_SIZE / sizeof(u32);

		stlbs[i] = stlb;
	}

	address = (u32)all_stlb->physBase;


	for (i = mmu->dynamic_mapping_start;
	     i < mmu->dynamic_mapping_start + dynamic_mapping_entries; i++) {
		u32 mtlb_entry;

		mtlb_entry = address | MMU_MTLB_4K_PAGE | MMU_MTLB_PRESENT;

		address += MMU_STLB_4K_SIZE;

		/* Insert Slave TLB address to Master TLB entry.*/
		_write_page_entry(mmu->mtlb_logical + i, mtlb_entry);
	}

	kfree(all_stlb);

	return 0;
}

/* MMU Construct */
int dc_mmu_construct(struct dc8000_dc *dc)
{
	void *mem = NULL;
	void *cookie = NULL, *cookie_safe = NULL;
	u64 dma_addr, dma_addr_safe;
	u32 size = 0;
	int ret = 0;

	mem = kzalloc(sizeof(dc_mmu), GFP_KERNEL);
	if (!mem)
		return -ENOMEM;

	dc->mmu = (dc_mmu *)mem;
	dc->mmu->mtlb_bytes = MMU_MTLB_SIZE;
	size = ALIGN(dc->mmu->mtlb_bytes, PAGE_SIZE);
	if (size == PAGE_SIZE) {
		size = PAGE_SIZE * 2;
	}

	/* Allocate MTLB */
	cookie = dma_alloc_wc(dc->dev, size, &dma_addr);
	if (!cookie) {
		vo_err("Failed to alloc mtlb buffer.\n");
		return -ENOMEM;
	}

	dc->mmu->mtlb_logical = cookie;
	dc->mmu->mtlb_physical = dma_addr;
	memset(dc->mmu->mtlb_logical, 0, size);

	size = MMU_MTLB_ENTRY_NUM * sizeof(dc_mmu_stlb_pt);

	ret = _allocate_memory(size, &dc->mmu->stlbs);
	if (ret)
		return ret;

	dc->mmu->mode = MMU_MODE_1K;

	ret = _setup_process_address_space(dc->dev, dc->mmu);
	if (ret)
		return ret;

	/* Allocate safe page */
	cookie_safe = dma_alloc_wc(dc->dev, 4096, &dma_addr_safe);
	if (!cookie_safe) {
		vo_err("Failed to alloc safe page.\n");
		return -ENOMEM;
	}

	dc->mmu->safe_page_logical = cookie_safe;
	dc->mmu->safe_page_physical = (u64)dma_addr_safe;
	memset(dc->mmu->safe_page_logical, 0, size);

	return 0;
}

int dc_mmu_get_page_entry(dc_mmu *mmu, u32 address, u32 **page_table)
{
	dc_mmu_stlb_pt stlb;
	dc_mmu_stlb_pt *stlbs = (dc_mmu_stlb_pt *)mmu->stlbs;
	u32 mtlb_offset = _mtlb_offset(address);
	u32 stlb_offset = _stlb_offset(address);

	stlb = stlbs[mtlb_offset - mmu->dynamic_mapping_start];
	if (stlb == NULL) {
		vo_err("BUG: invalid stlb,  mmu=%p stlbs=%p  mtlb_offset=0x%x\n",
		       mmu, stlbs, mtlb_offset);
		return -ENXIO;
	}

	*page_table = &stlb->logical[stlb_offset];

	return 0;
}

int _link(dc_mmu *mmu, u32 index, u32 node)
{
	if (index >= mmu->page_table_entries) {
		mmu->heap_list = node;
	} else {
		u32 *map = mmu->map_logical;

		switch (DC_ENTRY_TYPE(_read_page_entry(&map[index]))) {
		case DC_MMU_SINGLE:
			/* Previous is a single node, link to it*/
			_write_page_entry(&map[index],
					  (node << 8) | DC_MMU_SINGLE);
			break;
		case DC_MMU_FREE:
			/* Link to FREE TYPE node */
			_write_page_entry(&map[index + 1], node);
			break;
		default:
			vo_err("MMU table corrupted at index %u!", index);
			return -EINVAL;
		}
	}

	return 0;
}

int _add_free(dc_mmu *mmu, u32 index, u32 node, u32 count)
{
	u32 *map = mmu->map_logical;

	if (count == 1) {
		/* Initialize a single page node */
		_write_page_entry(map + node, DC_SINGLE_PAGE_NODE_INITIALIZE |
						      DC_MMU_SINGLE);
	} else {
		/* Initialize the FREE node*/
		_write_page_entry(map + node, (count << 8) | DC_MMU_FREE);
		_write_page_entry(map + node + 1, ~0U);
	}

	return _link(mmu, index, node);
}

/* Collect free nodes */
int _collect(dc_mmu *mmu)
{
	u32 *map = mmu->map_logical;
	u32 count = 0, start = 0, i = 0;
	u32 previous = ~0U;
	int ret = 0;

	mmu->heap_list = ~0U;
	mmu->free_nodes = false;

	/* Walk the entire page table */
	for (i = 0; i < mmu->page_table_entries; i++) {
		switch (DC_ENTRY_TYPE(_read_page_entry(&map[i]))) {
		case DC_MMU_SINGLE:
			if (count++ == 0) {
				/* Set new start node */
				start = i;
			}
			break;
		case DC_MMU_FREE:
			if (count == 0) {
				/* Set new start node */
				start = i;
			}

			count += _read_page_entry(&map[i]) >> 8;
			/* Advance the index of the page table */
			i += (_read_page_entry(&map[i]) >> 8) - 1;
			break;
		case DC_MMU_USED:
			/* Meet used node, start to collect */
			if (count > 0) {
				/* Add free node to list*/
				ret = _add_free(mmu, previous, start, count);
				if (ret)
					return ret;
				/* Reset previous unused node index */
				previous = start;
				count = 0;
			}
			break;
		default:
			vo_err("MMU page table corrupted at index %u!", i);
			return -EINVAL;
		}
	}

	/* If left node is an open node. */
	if (count > 0) {
		ret = _add_free(mmu, previous, start, count);
		if (ret)
			return ret;
	}

	return 0;
}

int _fill_page_table(u32 *page_table, u32 page_count, u32 entry_value)
{
	u32 i;

	for (i = 0; i < page_count; i++) {
		_write_page_entry(page_table + i, entry_value);
	}

	return 0;
}

int dc_mmu_allocate_pages(dc_mmu *mmu, u32 page_count, u32 *address)
{
	bool got = false;
	u32 *map;
	u32 index = 0, vaddr, left;
	u32 previous = ~0U;
	u32 mtlb_offset, stlb_offset;
	int ret = 0;

	if (page_count == 0 || page_count > mmu->page_table_entries) {
		vo_err("has invalid arguments.\n");
		return -EINVAL;
	}

	for (map = mmu->map_logical; !got;) {
		for (index = mmu->heap_list;
		     !got && (index < mmu->page_table_entries);) {
			switch (DC_ENTRY_TYPE(_read_page_entry(&map[index]))) {
			case DC_MMU_SINGLE:
				if ((index != 0) && (page_count == 1)) {
					got = true;
				} else {
					/* Move to next node */
					previous = index;
					index = _read_page_entry(&map[index]) >>
						8;
				}
				break;
			case DC_MMU_FREE:
				if ((index == 0) &&
				    (page_count <
				     ((_read_page_entry(&map[index]) >> 8)))) {
					got = true;
				} else if ((index != 0) &&
					   (page_count <=
					    ((_read_page_entry(&map[index]) >>
					      8)))) {
					got = true;
				} else {
					/* Move to next node */
					previous = index;
					index = _read_page_entry(
						&map[index + 1]);
				}
				break;
			default:
				/* Only link SINGLE and FREE node */
				vo_err("MMU table corrupted at index %u!",
				       index);
				ret = -EINVAL;
				goto OnError;
			}
		}

		/* If out of index */
		if (index >= mmu->page_table_entries) {
			if (mmu->free_nodes) {
				/* Collect the free node */
				ret = _collect(mmu);
				if (ret)
					goto OnError;
				previous = ~0U;
			} else {
				ret = -ENODATA;
				goto OnError;
			}
		}
	}

	switch (DC_ENTRY_TYPE(_read_page_entry(&map[index]))) {
	case DC_MMU_SINGLE:
		/* Unlink single node from node list */
		ret = _link(mmu, previous, _read_page_entry(&map[index]) >> 8);
		if (ret)
			goto OnError;
		break;

	case DC_MMU_FREE:
		left = (_read_page_entry(&map[index]) >> 8) - page_count;
		switch (left) {
		case 0:
			/* Unlink the entire FREE type node */
			ret = _link(mmu, previous,
				    _read_page_entry(&map[index + 1]));
			if (ret)
				goto OnError;
			break;
		case 1:
			/* Keep the map[index] as a single node,
             * mark the left as used
             */
			_write_page_entry(&map[index],
					  (_read_page_entry(&map[index + 1])
					   << 8) | DC_MMU_SINGLE);
			index++;
			break;
		default:
			/* FREE type node left */
			_write_page_entry(&map[index],
					  (left << 8) | DC_MMU_FREE);
			index += left;
			if (index + page_count > mmu->page_table_entries) {
				vo_err("Error: index %d , left %d\n", index,
				       left);
				ret = -EINVAL;
				goto OnError;
			}

			break;
		}
		break;
	default:
		/* Only link SINGLE and FREE node */
		vo_err("MMU table corrupted at index %u!", index);
		ret = -EINVAL;
		goto OnError;
	}

	if (index + page_count > mmu->page_table_entries) {
		vo_err("Fatal ERROR: This may caused by free pages more than allocated pages\n");
		_write_page_entry(mmu->map_logical,
				  (mmu->page_table_entries << 8) | DC_MMU_FREE);
		_write_page_entry(mmu->map_logical + 1, ~0U);
		previous = ~0U;
		mmu->heap_list = 0;
		memset(mmu->page_entry_info, 0,
		       mmu->page_table_entries * sizeof(u32));

		ret = -EINVAL;
		goto OnError;
	}

	/* Mark node as used */
	ret = _fill_page_table(&map[index], page_count, DC_MMU_USED);
	if (ret)
		goto OnError;

	mtlb_offset =
		index / MMU_STLB_4K_ENTRY_NUM + mmu->dynamic_mapping_start;
	stlb_offset = index % MMU_STLB_4K_ENTRY_NUM;

	vaddr = (mtlb_offset << MMU_MTLB_SHIFT) |
		(stlb_offset << MMU_STLB_4K_SHIFT);

	if (vaddr == 0) {
		vo_err("Error occured on alloc vaddr\n");
		ret = -ENODATA;
		goto OnError;
	}

	mmu->page_entry_info[index] = (index << 12) | (page_count & 0xfff);

	if (address != NULL) {
		*address = vaddr;
	}

	return 0;

OnError:

	return ret;
}

int dc_mmu_free_pages(dc_mmu *mmu, u32 address, u32 page_count)
{
	u32 *node, index;

	if (page_count == 0)
		return -EINVAL;

	index = _address_to_index(mmu, address);
	node = mmu->map_logical + _address_to_index(mmu, address);

	if (((mmu->page_entry_info[index] & 0xfff) != (page_count & 0xfff)) ||
	    ((mmu->page_entry_info[index] >> 12) != index)) {
		vo_err("index %d, page_count %d: free pages not equal to alloc\n",
		       index, page_count);
		return -EINVAL;
	} else {
		mmu->page_entry_info[index] = 0;
	}

	if (page_count == 1) {
		/* Mark the Single page node free */
		_write_page_entry(node, DC_SINGLE_PAGE_NODE_INITIALIZE |
						DC_MMU_SINGLE);
	} else {
		/* Mark the FREE type node free */
		_write_page_entry(node, (page_count << 8) | DC_MMU_FREE);
		_write_page_entry(node + 1, ~0U);
	}

	mmu->free_nodes = true;

	return 0;
}

int dc_mmu_set_page(dc_mmu *mmu, u64 page_address, u32 *page_entry)
{
	u32 address_ext;
	u32 address;

	if (page_entry == NULL || (page_address & 0xFFF)) {
		return -EINVAL;
	}

	/* [31:0]. */
	address = (u32)(page_address & 0xFFFFFFFF);
	/* [39:32]. */
	address_ext = (u32)((page_address >> 32) & 0xFF);

	_write_page_entry(page_entry, _set_page(address, address_ext, true));

	return 0;
}

int dc_mmu_map_memory(dc_mmu *mmu, u64 physical, u32 page_count, u32 *address)
{
	u32 virutal_address, i = 0;
	bool allocated = false;
	int ret = 0;

	ret = dc_mmu_allocate_pages(mmu, page_count, &virutal_address);
	if (ret)
		goto OnError;

	*address = virutal_address | (physical & MMU_PAGE_4K_MASK);

	allocated = true;

	/* Fill in page table */
	for (i = 0; i < page_count; i++) {
		u64 page_phy;
		u32 *page_entry;

		page_phy = physical + i * MMU_PAGE_4K_SIZE;

		ret = dc_mmu_get_page_entry(mmu, virutal_address, &page_entry);
		if (ret)
			goto OnError;

		/* Write the page address to the page entry */
		ret = dc_mmu_set_page(mmu, page_phy, page_entry);
		if (ret)
			goto OnError;

		/* Get next page */
		virutal_address += MMU_PAGE_4K_SIZE;
	}

	

	return 0;

OnError:
	if (allocated)
		dc_mmu_free_pages(mmu, virutal_address, page_count);
	vo_err("map fail!\n");

	return ret;
}

int dc_mmu_unmap_memory(dc_mmu *mmu, u32 gpu_address, u32 page_count)
{
	return dc_mmu_free_pages(mmu, gpu_address, page_count);
}
