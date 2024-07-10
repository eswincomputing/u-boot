// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#include <common.h>
#include <blk.h>
#include <u-boot/crc.h>
#include <linux/err.h>
#include <part.h>
#include <system_update.h>

uint32_t bootmessage_compute_crc(struct bootloader_message *abc);
uint32_t bootbank_compute_crc(struct boot_bank *bank);
int bootloader_message_store(struct blk_desc *dev_desc,struct disk_partition *part_info,struct bootloader_message *abc);
int boot_bank_store(struct blk_desc *dev_desc, struct disk_partition *part_info, int block, struct boot_bank *bank);
int get_bootmessage_from_disk(struct blk_desc *dev_desc, const struct disk_partition *part_info, struct bootloader_message **abc);
int get_bootbank_from_disk(struct blk_desc *dev_desc, const struct disk_partition *part_info, int block, struct boot_bank **bank);
int set_bootable_devplist(void);
