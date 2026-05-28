// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2026, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 *
 */
/* Application-side partition state management */
#include <stdio.h>
#include "bootloader.h"
#include "bootloader_port.h"
#include "bootloader_update.h"

bool update_firmware(FirmwareHeader *fht, uint32_t fw_addr) {
    int32_t ret = 0;
    uint32_t crc_raw = 0;
    uint32_t crc_flash = 0;
    uint32_t size = 0;
    uint32_t retry_count = 3;
    uint64_t dst_addr = 0;
    FirmwareEntryHeader *feht = NULL;

    dst_addr = fw_addr + NSIGNHEAD_SIZE;
    for(int i = 0; i < fht->num_entries; i++) {
        feht = (FirmwareEntryHeader *) &fht->entries[i];
        size = feht->size + SIGNATURE_SIZE;
retry:
        printf("%s writing...\r\n", get_payload_name(feht->payload_type));

        // write fw in flash
        es_fs_copy_data(dst_addr, feht->offset, size);
        // check crc
        crc_raw = calculate_fs_firmware_crc32(feht->offset, size);
        crc_flash = calculate_firmware_crc32(dst_addr, size);
        if(crc_flash != crc_raw) {
            retry_count--;
            if(retry_count)
                goto retry;
            printf("crc32 check error\r\n");
            ret = -1;
        }
        printf("SF: 0x%x bytes @ %#x Written: %s\r\n",
            (uint32_t)size, (uint32_t)dst_addr, ret?"ERROR":"OK");
        if(ret)
            return false;

        // update fw offset
        feht->offset = dst_addr;
        dst_addr += (size + ALIGN_4K - 1) / ALIGN_4K * ALIGN_4K;
    }

    flash_erase(fw_addr, NSIGNHEAD_SIZE);
    flash_write(fw_addr, fht, ENTRYHEAD_SIZE);

    return true;
}

/* Update firmware function */
bool update_table_and_firmware(uint32_t target_partition) {
    uint8_t fw_head_buf[ENTRYHEAD_SIZE];
    uint32_t table_addr, fw_addr;
    PartitionTable new_table;
    uint32_t next_version;
    uint32_t slot_version;
    /* Determine target addresses */
    if ((target_partition == 0) || ((get_flash_size() <= FLASH_SZ_8M) && (DIE_NUM == 0))) {  /* Partition A */
        table_addr = TABLE_A_ADDR;
        fw_addr = FW_A_ADDR;
        printf("Update partition A\r\n");
    } else {  /* Partition B */
        table_addr = TABLE_B_ADDR;
        fw_addr = FW_B_ADDR;
        printf("Update partition B\r\n");
    }

    /* Get next version number (simplified implementation) */
    PartitionTable current_table;
    if (find_latest_table_slot(table_addr, &current_table, &slot_version)) {
        next_version = current_table.version + 1;
        slot_version += 1;
    } else {
        next_version = 1;  /* First version */
        slot_version = 1;
    }

    /* Prepare new partition table */
    new_table.magic = MAGIC_PART;  /* "PART" */
    new_table.version = next_version;
    new_table.status = PART_STATUS_UPDATING;
    new_table.boot_attempts = 0;
    new_table.max_boot_attempts = 3;
    new_table.fw_size = ENTRYHEAD_SIZE;
    es_fs_read(0, fw_head_buf, ENTRYHEAD_SIZE);
    new_table.fw_crc32 = calculate_crc32(fw_head_buf, new_table.fw_size);
    // new_table.update_timestamp = get_current_timestamp();
    new_table.previous_version = (next_version > 1) ? (next_version - 1) : 0;

    /* Set table CRC (temporary value, will update after firmware write) */
    new_table.table_crc32 = calculate_crc32((uint8_t*)&new_table,
                                               sizeof(PartitionTable) - sizeof(uint32_t));

    /* Step 1: Write partition table with UPDATING status */
    if (!write_table_slot(table_addr, &new_table, slot_version)) {
        return false;
    }

    /* Step 2: Erase and write firmware */
    /* In actual implementation, need to call flash_erase and flash_write functions here */
    update_firmware((FirmwareHeader *)fw_head_buf, fw_addr);

    /* Step 3: Verify firmware CRC */
    new_table.fw_crc32 = calculate_crc32(fw_head_buf, new_table.fw_size);
    uint32_t calc_crc = calculate_firmware_crc32(fw_addr, new_table.fw_size);
    if (calc_crc != new_table.fw_crc32) {
        /* Firmware CRC mismatch, mark as ERROR */
        new_table.status = PART_STATUS_ERROR;
        new_table.table_crc32 = calculate_crc32((uint8_t*)&new_table,
                                               sizeof(PartitionTable) - sizeof(uint32_t));
        write_table_slot(table_addr, &new_table, slot_version + 1);
        return false;
    }

    /* Step 4: Update status to PENDING_VERIFY */
    new_table.status = PART_STATUS_PENDING_VERIFY;
    new_table.table_crc32 = calculate_crc32((uint8_t*)&new_table,
                                           sizeof(PartitionTable) - sizeof(uint32_t));

    if (!write_table_slot(table_addr, &new_table, slot_version + 1)) {
        return false;
    }

    return true;
}


/**
 * Check if current bootloader supports A/B partition
 * @return false: not supported, treu: supported
 */
bool is_ab_partition_supported(FirmwareHeader *fht)
{
    // Check bootloader Check magic number, fw number and payloadtype
    if(FHT_MAGIC != fht->magic){
        printf("FHT magic should be %x, but %x!\r\n",
                FHT_MAGIC, fht->magic);
        return false;
    }

    if(fht->num_entries != 1)
        return false;

    if(fht->entries[0].payload_type != FIRMWARE)
        return false;

    return true;
}

/* Change to AB partition support bootloader, erase flash, and write the first table */
int init_ab_partition_support(uint32_t target_partition,
                     uint32_t firmware_data,
                     uint32_t fw_size,
                     PartitionStatus new_status) {
    uint32_t table_addr, fw_addr;
    PartitionTable new_table;
    uint32_t next_version;
    uint32_t slot_version;
    // return false;
    /* Determine target addresses */
    if (target_partition == 0) {  /* Partition A */
        table_addr = TABLE_A_ADDR;
        fw_addr = FW_A_ADDR;
    } else {  /* Partition B */
        table_addr = TABLE_B_ADDR;
        fw_addr = FW_B_ADDR;
    }
    flash_erase(table_addr, 0x1000);

    if(new_status == PART_STATUS_EMPTY)
        return 0;

    next_version = 1;  /* First version */
    slot_version = 1;

    /* Prepare new partition table */
    new_table.magic = MAGIC_PART;  /* "PART" */
    new_table.version = next_version;
    new_table.status = new_status;
    new_table.boot_attempts = 0;
    new_table.max_boot_attempts = 3;
    new_table.fw_size = fw_size;
    new_table.fw_crc32 = calculate_firmware_crc32(firmware_data, fw_size);
    // new_table.update_timestamp = get_current_timestamp();
    new_table.previous_version = (next_version > 1) ? (next_version - 1) : 0;

    /* Set table CRC (temporary value, will update after firmware write) */
    new_table.table_crc32 = calculate_crc32((uint8_t*)&new_table,
                                               sizeof(PartitionTable) - sizeof(uint32_t));

    /* Step 1: Write partition table */
    if (!write_table_slot(table_addr, &new_table, slot_version)) {
        return -1;
    }
    return 0;
}

bool update_ab_partition_supported(void) {
    uint8_t fw_head_buf[ENTRYHEAD_SIZE];
    FirmwareHeader *fht = NULL;

    /* Read nsign bootloader head */
    flash_read(FW_HEAD_OFFSET, fw_head_buf, ENTRYHEAD_SIZE);
    fht = (FirmwareHeader *) fw_head_buf;

    /* Check if the bootlaoder supports A/B partition boot*/
    if(!is_ab_partition_supported(fht)) {
        return false;
    }
    return true;
}

bool update_bootloader(char *name, uint64_t base_addr) {
    uint8_t fw_head_buf[ENTRYHEAD_SIZE];
    BootContext ctx = {0};
    FirmwareHeader *fht = NULL;
    FirmwareEntryHeader *feht = NULL;
    int ret = 0;

    if(!update_ab_partition_supported()) {
        printf("Current bootloader does not support A/B partition update.\n");
        printf("This firmware requires the new A/B bootloader layout.\n");
        printf("Bootloader upgrade is required before continuing.\n");
        printf("Upgrade procedure:\n");
        printf("    1. switch device to USB boot mode\n");
        printf("    2. flash recovery_bootloader_xxx.bin\n");
        printf("After bootloader upgrade:\n");
        printf("    1. reboot device\n");
        printf("    2. retry firmware update\n");
        return -1;
    }

    es_fs_open(name, base_addr);
    es_fs_read(0, fw_head_buf, ENTRYHEAD_SIZE);
    fht = (FirmwareHeader *) fw_head_buf;
    if(FHT_MAGIC != fht->magic) {
        printf("FHT magic should be %x, but %x!\r\n",
                FHT_MAGIC, fht->magic);
        ret = -1;
        goto out;
    }

    if(fht->num_entries == 0){
        printf("fw file error(entries is 0)!\r\n");
        ret = -1;
        goto out;
    }

    for(int i = 0; i < fht->num_entries; i++) {
        feht = (FirmwareEntryHeader *) &fht->entries[i];
        if(feht->nsign_version >= NSIGN_MINIMUM_VERSION) {
            if(!verify_fs_firmware_crc(feht->offset + SIGNATURE_SIZE, feht->size, feht->crc32)) {
                printf("%s verify_fs_firmware_crc error\r\n", get_payload_name(feht->payload_type));
                ret = -1;
                goto out;
            }
        } else {
            printf("nsign tool version must be 3.0.0 or higher, Currently the version is %d.%d.%d \r\n",
                VERSION_MAJOR(feht->nsign_version), VERSION_MINOR(feht->nsign_version), VERSION_PATCH(feht->nsign_version));
            ret = -1;
            goto out;
        }
    }

    /* Load both partition tables */
    ctx.table_a_valid = find_latest_table_slot(TABLE_A_ADDR, &ctx.table_a, NULL);
    ctx.table_b_valid = find_latest_table_slot(TABLE_B_ADDR, &ctx.table_b, NULL);

    get_upgrade_partition(&ctx);

    ret = update_table_and_firmware(ctx.selected_partition);

out:
    es_fs_close();
    return ret;
}
