// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2026, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 *
 */
#ifndef __BOOTLOADER_PORT_H
#define __BOOTLOADER_PORT_H
#include <stdint.h>

int flash_init(void *flash_str);
int32_t get_flash_size(void);
int flash_read(uint32_t addr, void* buffer, uint32_t size);
int flash_erase(uint32_t addr, uint32_t size);
int flash_write(uint32_t addr, void* buffer, uint32_t size);
int flash_copy_data(uint32_t dest_addr, uint32_t src_addr, uint32_t size);

int es_fs_open(char *file_name, uint64_t base_addr);
int es_fs_close(void);
int es_fs_read(uint32_t addr, void* buffer, uint32_t size);
int es_fs_copy_data(uint32_t dest_addr, uint32_t src_addr, uint32_t size);

#endif
