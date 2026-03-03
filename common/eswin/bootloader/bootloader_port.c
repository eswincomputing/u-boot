// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2026, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 *
 */
#include <init.h>
#include <stdlib.h>
#include <common.h>
#include <dm.h>
#include <spi.h>
#include <spi_flash.h>
#include <dm/uclass-internal.h>
#include <dm/device-internal.h>
#include <string.h>
#include "bootloader_port.h"



#define BOOTCHAIN_PACKAGE_SIZE (0x100000ul)

static struct spi_flash *flash = NULL;

int flash_init(void *flash_str) {

	int ret;
	struct udevice *bus, *dev;

	if (flash)
		spi_flash_free(flash);

    ret = uclass_get_device_by_name(UCLASS_SPI, flash_str, &bus);
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
	flash = dev_get_uclass_priv(dev);
	if(!flash) {
		printf("SPI dev_get_uclass_priv failed\n");
		return -1;
	}
    return 0;

}

int32_t get_flash_size(void) {

    return flash->size;
}

int flash_read(uint32_t addr, void* buffer, uint32_t size) {
    /* Implementation depends on your Nor Flash hardware */
    spi_flash_read(flash, addr, size, buffer);
    return 0;
}

int flash_erase(uint32_t addr, uint32_t size) {
    addr = DIV_ROUND_UP(addr, 0x1000) * 0x1000;
    size = DIV_ROUND_UP(size, 0x1000) * 0x1000;
    es_bootspi_wp_cfg(flash, 0);
    spi_flash_erase(flash, addr, size);
    es_bootspi_wp_cfg(flash, 1);
    return 0;
}

int flash_write(uint32_t addr, void* buffer, uint32_t size) {
    es_bootspi_wp_cfg(flash, 0);
    spi_flash_write(flash, addr, size, buffer);
    es_bootspi_wp_cfg(flash, 1);

    return 0;
}

/**
 * @brief Copy data from source address to destination address in Flash
 * @param dest_addr: Destination Flash address
 * @param src_addr: Source Flash address
 * @param size: Data size to copy (in bytes)
 * @return Returns 0 on success, error code on failure
 */
int flash_copy_data(uint32_t dest_addr, uint32_t src_addr, uint32_t size) {
	uint64_t package_blk, total_size, write_size;

    // Parameter validation
    if (dest_addr == src_addr) {
        return -1;  // Source and destination addresses are the same, no need to copy
    }

    if (size == 0) {
        return -1;  // size is zero, no need to copy
    }

    char *temp_buf = (char *)malloc(BOOTCHAIN_PACKAGE_SIZE); 
    package_blk = DIV_ROUND_UP(size, BOOTCHAIN_PACKAGE_SIZE); 
    total_size = size;
    flash_erase(dest_addr, size);
    for(int i = 0;i < package_blk; i++) {
        if(total_size > BOOTCHAIN_PACKAGE_SIZE)
        {
            write_size = BOOTCHAIN_PACKAGE_SIZE;
        } else {
            write_size = total_size;
        }
        total_size -= write_size;
        flash_read(src_addr + BOOTCHAIN_PACKAGE_SIZE * i, (void *)temp_buf, write_size);
        flash_write(dest_addr + BOOTCHAIN_PACKAGE_SIZE * i, (void *)temp_buf, write_size);
    }
    free(temp_buf);
    return 0;  // Operation successful
}

static uint64_t fs_base_addr = 0;
int es_fs_open(char *file_name, uint64_t base_addr) {
    fs_base_addr = base_addr;
    return 0;
}

int es_fs_close(void) {
    return 0;
}

int es_fs_read(uint32_t addr, void* buffer, uint32_t size) {
    addr = fs_base_addr + addr;
    memcpy(buffer, (void *)addr, size);
    return 0;
}

/**
 * @brief Copy data from source address in ddr to destination address in Flash
 * @param dest_addr: Destination Flash address
 * @param src_addr: Source Flash address
 * @param size: Data size to copy (in bytes)
 * @return Returns 0 on success, error code on failure
 */
int es_fs_copy_data(uint32_t dest_addr, uint32_t src_addr, uint32_t size) {
	uint64_t package_blk, total_size, write_size, currentIndex = 0;

    // Parameter validation
    if (dest_addr == src_addr) {
        return -1;  // Source and destination addresses are the same, no need to copy
    }

    if (size == 0) {
        return -1;  // size is zero, no need to copy
    }

    char *temp_buf = (char *)malloc(BOOTCHAIN_PACKAGE_SIZE); /* Temporary buffer */
    package_blk = DIV_ROUND_UP(size, BOOTCHAIN_PACKAGE_SIZE); 
    total_size = size;
    flash_erase(dest_addr, size);
	printf("\rWrite progress: %3d%%:\r", 0);
    for(int i = 0;i < package_blk; i++) {
        if(total_size > BOOTCHAIN_PACKAGE_SIZE)
        {
            write_size = BOOTCHAIN_PACKAGE_SIZE;
        } else {
            write_size = total_size;
        }
        total_size -= write_size;
        es_fs_read(src_addr + BOOTCHAIN_PACKAGE_SIZE * i, (void *)temp_buf, write_size);
        flash_write(dest_addr + BOOTCHAIN_PACKAGE_SIZE * i, (void *)temp_buf, write_size);
		currentIndex = (uint64_t)i * 100 / package_blk;
		printf("Write progress: %3lld%%:", currentIndex);
		for(int col = 0; col < currentIndex/2; col++) {
			printf("%s","+");
		}
		printf("\r");
    }
    printf("Write progress: %3d%%:", 100);
    for(int j = 0; j < 100/2; j ++)
        printf("%s","+");
    printf("\r\n");
    free(temp_buf);
    return 0;  // Operation successful
}
