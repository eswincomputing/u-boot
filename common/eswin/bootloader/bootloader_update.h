// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2026, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 *
 */
#ifndef __BOOTLOADER_UPDATE_H
#define __BOOTLOADER_UPDATE_H

#include <stdint.h>
#include "bootloader.h"

bool update_ab_partition_supported(void);
uint32_t get_boot_partition(void);
uint32_t get_upgrade_partition(BootContext* ctx);
bool is_ab_partition_supported(FirmwareHeader *fht);
#endif
