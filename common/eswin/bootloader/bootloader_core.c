// SPDX-License-Identifier: GPL-2.0
/*
 *
 * Copyright 2026, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 *
 */
#include <stdio.h>
#include <string.h>
#include "bootloader.h"
#include "bootloader_port.h"
#include <u-boot/crc.h>
#include <stdio.h>

uint32_t TABLE_A_ADDR;
uint32_t FW_A_ADDR;
uint32_t FW_A_SIZE;
uint32_t TABLE_B_ADDR;
uint32_t FW_B_ADDR;
uint32_t FW_B_SIZE;

const struct partitiontype partititon_ids[] = {
	{ .name = "PART_STATUS_EMPTY", .id = PART_STATUS_EMPTY},
	{ .name = "PART_STATUS_PENDING_VERIFY", .id = PART_STATUS_PENDING_VERIFY},
	{ .name = "PART_STATUS_VALID", .id = PART_STATUS_VALID},
	{ .name = "PART_STATUS_UPDATING", .id = PART_STATUS_UPDATING},
	{ .name = "PART_STATUS_INVALID", .id = PART_STATUS_INVALID},
	{ .name = "PART_STATUS_ERROR", .id = PART_STATUS_ERROR}
};

char* get_status_name(uint32_t status) {
    int num = sizeof(partititon_ids)/sizeof(struct partitiontype);
    for(int id = 0; id < num; id++) {
        if(partititon_ids[id].id == status)
            return partititon_ids[id].name;
    }
    return "ERROR TYPE";
}

const struct partitiontype payload_types[] = {
	{ .name = "DDR", .id = DDR},
	{ .name = "D2D", .id = D2D},
	{ .name = "BOOTLOADER", .id = BOOTLOADER},
	{ .name = "KERNEL", .id = KERNEL},
	{ .name = "ROOTFS", .id = ROOTFS},
	{ .name = "APPLICATION", .id = APPLICATION},
	{ .name = "FIRMWARE", .id = FIRMWARE}
};

char* get_payload_name(uint32_t typ_id) {
    int num = sizeof(payload_types)/sizeof(struct partitiontype);
    for(int id = 0; id < num; id++) {
        if(payload_types[id].id == typ_id)
            return payload_types[id].name;
    }
    return "ERROR TYPE";
}


/* Calculate CRC32 for data */
uint32_t calculate_crc32(const uint8_t* data, uint32_t length) {
    uint32_t crc = 0xFFFFFFFF;
    return crc32(crc, data, length);
}

/* Verify table CRC32 */
bool verify_table_crc(PartitionTable* table) {
    /* Calculate table CRC (excluding crc32 field itself) */
    uint32_t crc = calculate_crc32((uint8_t*)table, 
                                   sizeof(PartitionTable) - sizeof(uint32_t));
    
    return (crc == table->table_crc32);
}

/* Calculate CRC32 for firmware */
uint32_t calculate_firmware_crc32(uint32_t fw_addr, uint32_t fw_size) {
    uint32_t chunk_size = 1024;
    uint8_t buffer[1024];
    uint32_t remaining = fw_size;
    uint32_t offset = 0;
    uint32_t crc = 0xFFFFFFFF;

    while (remaining > 0) {
        uint32_t read_size = (remaining > chunk_size) ? chunk_size : remaining;
        /* Read data from flash */
        flash_read(fw_addr + offset, buffer, read_size);

        /* Update CRC */
        crc = crc32(crc, buffer, read_size);

        offset += read_size;
        remaining -= read_size;
    }
    return (crc);
}

uint32_t calculate_fs_firmware_crc32(uint32_t fw_addr, uint32_t fw_size) {
    uint32_t chunk_size = 1024;
    uint8_t buffer[1024];
    uint32_t remaining = fw_size;
    uint32_t offset = 0;
    uint32_t crc = 0xFFFFFFFF;

    while (remaining > 0) {
        uint32_t read_size = (remaining > chunk_size) ? chunk_size : remaining;
        /* Read data from flash */
        es_fs_read(fw_addr + offset, buffer, read_size);

        /* Update CRC */
        crc = crc32(crc, buffer, read_size);

        offset += read_size;
        remaining -= read_size;
    }
    return (crc);
}

/* Verify firmware CRC32 */
bool verify_firmware_crc(uint32_t fw_addr, uint32_t fw_size, uint32_t expected_crc) {
    uint32_t chunk_size = 1024;
    uint8_t buffer[1024];
    uint32_t remaining = fw_size;
    uint32_t offset = 0;
    uint32_t crc = 0xFFFFFFFF;

    while (remaining > 0) {
        uint32_t read_size = (remaining > chunk_size) ? chunk_size : remaining;
        /* Read data from flash */
        flash_read(fw_addr + offset, buffer, read_size);

        /* Update CRC */
        crc = crc32(crc, buffer, read_size);

        offset += read_size;
        remaining -= read_size;
    }

    return (crc == expected_crc);
}

int verify_nsign_fw_crc(uint32_t fw_addr) {
    uint8_t fw_head_buf[ENTRYHEAD_SIZE];
    FirmwareEntryHeader *feht;
    FirmwareHeader *fw_fht;

    /* Read nsign bootloader head */
    flash_read(fw_addr, fw_head_buf, ENTRYHEAD_SIZE);
    fw_fht = (FirmwareHeader *) fw_head_buf;
    if(FHT_MAGIC != fw_fht->magic){
        printf("FHT magic should be %x, but %x!\r\n",
                FHT_MAGIC, fw_fht->magic);
        return -1;
    }

    if(fw_fht->num_entries == 0){
        printf("fw file error(entries is 0)!\r\n");
        return -1;
    }

    for(int i = 0; i < fw_fht->num_entries; i++) {
        feht = (FirmwareEntryHeader *) &fw_fht->entries[i];
        if(feht->nsign_version != 0) {
            if(!verify_firmware_crc(feht->offset + SIGNATURE_SIZE, feht->size, feht->crc32)){
                return -1;
            }
        }
    }
    return 0;
}

bool verify_fs_firmware_crc(uint32_t fw_addr, uint32_t fw_size, uint32_t expected_crc) {
    uint32_t chunk_size = 1024;
    uint8_t buffer[1024];
    uint32_t remaining = fw_size;
    uint32_t offset = 0;
    uint32_t crc = 0xFFFFFFFF;

    while (remaining > 0) {
        uint32_t read_size = (remaining > chunk_size) ? chunk_size : remaining;
        /* Read data from flash */
        es_fs_read(fw_addr + offset, buffer, read_size);

        /* Update CRC */
        crc = crc32(crc, buffer, read_size);

        offset += read_size;
        remaining -= read_size;
    }

    return (crc == expected_crc);
}

void dump_table_info(uint32_t table_base_addr) {
    TableSlot slot;
    printf("\n\n\n+++++++++++++++++++++++++++++++++++\r\n");
    printf("table_base_addr %x\r\n",table_base_addr);
    for (int i = 0; i < TABLE_SLOTS_COUNT; i++) {
        uint32_t slot_addr = table_base_addr + (i * TABLE_SLOT_SIZE);
        /* Read slot */
        flash_read(slot_addr, &slot, sizeof(TableSlot));
        
        /* Check magic number */
        if (slot.table.magic != MAGIC_PART) {  /* "PART" */
            printf("Invalid slot (MAGIC_PART)\r\n");
            continue;  /* Invalid slot */
        }
        
        /* Verify slot CRC */
        uint32_t calc_slot_crc = calculate_crc32((uint8_t*)&slot, 
                                                sizeof(TableSlot) - sizeof(uint32_t));
        if (calc_slot_crc != slot.slot_crc32) {
            printf("Invalid slot (CRC)\r\n");
            continue;  /* CRC mismatch */
        }
        
        /* Verify table CRC */
        if (!verify_table_crc(&slot.table)) {
            printf("Invalid slot (Table CRC)\r\n");
            continue;  /* Table CRC invalid */
        }
        printf("slot info: \r\n");
        printf("slot version %08x crc32 %08x \r\n", slot.slot_version, slot.slot_crc32);
        printf("\tPartitionTable:\r\n");
        printf("\t magic: %08x verion: %08x status: %s\r\n",slot.table.magic, slot.table.version, get_status_name(slot.table.status));
        printf("\t boot_attempts: %08x max_boot_attempts: %08x\r\n",slot.table.boot_attempts, slot.table.max_boot_attempts);
        printf("\t fw_size: %08x fw_crc32: %08x \r\n",slot.table.fw_size, slot.table.fw_crc32);
        printf("\t update_timestamp: %08x previous_version: %08x table_crc32: %08x\r\n",slot.table.update_timestamp, slot.table.previous_version, slot.table.table_crc32);
    }
}

/* Find the latest valid table slot */
bool find_latest_table_slot(uint32_t table_base_addr, PartitionTable* result, uint32_t* version) {
    TableSlot slot;
    TableSlot latest_slot = {0};
    PartitionTable latest_table;
    uint32_t latest_version = 0;
    bool found = false;

    /* Iterate through all slots */
    for (int i = 0; i < TABLE_SLOTS_COUNT; i++) {
        uint32_t slot_addr = table_base_addr + (i * TABLE_SLOT_SIZE);
        
        /* Read slot */
        flash_read(slot_addr, &slot, sizeof(TableSlot));
        
        /* Check magic number */
        if (slot.table.magic != MAGIC_PART) {  /* "PART" */
            continue;  /* Invalid slot */
        }
        
        /* Verify slot CRC */
        uint32_t calc_slot_crc = calculate_crc32((uint8_t*)&slot, 
                                                sizeof(TableSlot) - sizeof(uint32_t));
        if (calc_slot_crc != slot.slot_crc32) {
            continue;  /* CRC mismatch */
        }
        
        /* Verify table CRC */
        if (!verify_table_crc(&slot.table)) {
            continue;  /* Table CRC invalid */
        }

        /* Check if this is the latest version */
        if (slot.slot_version >= latest_version) {
            latest_version = slot.slot_version;
            memcpy((void *)&latest_slot, (void *)&slot, sizeof(TableSlot));
            latest_table = latest_slot.table;
            found = true;
        }
    }
    if (found) {
        *result = latest_table;
    }
    if(version) {
        *version = latest_version;
    }
    return found;
}

/* Write new table slot */
bool write_table_slot(uint32_t table_base_addr, PartitionTable* table, uint32_t slot_version) {
    TableSlot new_slot;
    uint32_t slot_index = 0;
    bool empty_slot_found = false;

    /* Find empty slot */
    for (slot_index = 0; slot_index < TABLE_SLOTS_COUNT; slot_index++) {
        uint32_t slot_addr = table_base_addr + (slot_index * TABLE_SLOT_SIZE);
        uint32_t first_word;

        /* Read first word of slot */
        flash_read(slot_addr, &first_word, sizeof(uint32_t));
        
        /* If first word is 0xFFFFFFFF, slot is empty (erased) */
        if (first_word == 0xFFFFFFFF) {
            empty_slot_found = true;
            break;
        }
    }

    /* If no empty slot found, need to erase entire table area */
    if (!empty_slot_found) {
        /* In actual implementation, need to call flash_erase function here */
        flash_erase(table_base_addr, TABLE_SIZE);
        
        /* After erase, start from first slot */
        slot_index = 0;
        slot_version = 0;
    }

    /* Prepare new slot */
    new_slot.table = *table;
    new_slot.slot_version = slot_version;

    /* Calculate slot CRC (excluding crc32 field itself) */
    new_slot.slot_crc32 = 0;  /* Temporarily set to 0 for CRC calculation */
    new_slot.slot_crc32 = calculate_crc32((uint8_t*)&new_slot, 
                                         sizeof(TableSlot) - sizeof(uint32_t));

    /* Write slot to flash */
    uint32_t slot_addr = table_base_addr + (slot_index * TABLE_SLOT_SIZE);

    /* In actual implementation, need to call flash_write function here */
    flash_write(slot_addr, (void*)&new_slot, sizeof(TableSlot));
    return true;
}

/* Update table status (handle Flash write limitations) */
bool update_table_status(uint32_t table_addr, PartitionStatus new_status) {
    PartitionTable current_table;
    uint32_t slot_version = 0;
    
    /* Find current latest table */
    if (!find_latest_table_slot(table_addr, &current_table, &slot_version)) {
        return false;  /* No valid table found */
    }
    
    /* Read current slot version (simplified implementation, should read from slot) */
    /* Here we simply increment version number */
    slot_version = slot_version + 1;
    
    /* Create updated table */
    PartitionTable new_table = current_table;
    new_table.status = new_status;
    
    /* If status changes to PENDING_VERIFY, reset boot attempts */
    if (new_status == PART_STATUS_PENDING_VERIFY) {
        new_table.boot_attempts = 0;
    }
    
    /* Update table CRC */
    new_table.table_crc32 = 0;  /* Reset for calculation */
    new_table.table_crc32 = calculate_crc32((uint8_t*)&new_table,
                                           sizeof(PartitionTable) - sizeof(uint32_t));
    
    /* Write new slot */
    return write_table_slot(table_addr, &new_table, slot_version);
}

/* Increment boot attempts (special handling for Flash write limitations) */
bool increment_boot_attempts(uint32_t table_addr) {
    PartitionTable current_table;
    uint32_t slot_version = 0;

    /* Find current latest table */
    if (!find_latest_table_slot(table_addr, &current_table, &slot_version)) {
        return false;
    }
    
    /* Only increment attempts for PENDING_VERIFY partitions */
    if (current_table.status != PART_STATUS_PENDING_VERIFY) {
        return true;  /* No update needed */
    }
    
    /* Check if max attempts reached */
    if (current_table.boot_attempts >= current_table.max_boot_attempts) {
        /* Mark as ERROR state */
        return update_table_status(table_addr, PART_STATUS_ERROR);
    }
    
    /* Increment version number */
    slot_version = slot_version + 1;
    
    /* Create updated table */
    PartitionTable new_table = current_table;
    new_table.boot_attempts = current_table.boot_attempts + 1;
    
    // /* Check if new value can be written without erase */
    // uint32_t current_attempts = current_table.boot_attempts;
    // uint32_t new_attempts = new_table.boot_attempts;

    /* Update new table */
    new_table.table_crc32 = 0;
    new_table.table_crc32 = calculate_crc32((uint8_t*)&new_table,
                                            sizeof(PartitionTable) - sizeof(uint32_t));
    return write_table_slot(table_addr, &new_table, slot_version);
}

/* Select boot partition based on state machine */
uint32_t select_boot_partition(BootContext* ctx) {
    bool a_valid = ctx->table_a_valid;
    bool b_valid = ctx->table_b_valid;
    
    PartitionTable* table_a = &ctx->table_a;
    PartitionTable* table_b = &ctx->table_b;
    

    
    /* Rule 1: Check PENDING_VERIFY partitions */
    if (a_valid && table_a->status == PART_STATUS_PENDING_VERIFY) {
        /* Increment boot attempts */
        if (increment_boot_attempts(TABLE_A_ADDR)) {
            /* Reload table to get updated boot_attempts */
            find_latest_table_slot(TABLE_A_ADDR, table_a, NULL);
            
            /* Check if max attempts exceeded */
            if (table_a->status != PART_STATUS_ERROR) {
                ctx->selected_partition = 0;
                return FW_A_ADDR;
            }
        }
    }

    if (b_valid && table_b->status == PART_STATUS_PENDING_VERIFY) {
        /* Increment boot attempts */
        if (increment_boot_attempts(TABLE_B_ADDR)) {
            /* Reload table to get updated boot_attempts */
            find_latest_table_slot(TABLE_B_ADDR, table_b, NULL);
            
            /* Check if max attempts exceeded */
            if (table_b->status != PART_STATUS_ERROR) {
                ctx->selected_partition = 1;
                return FW_B_ADDR;
            }
        }
    }

    /* Rule 2: Prefer VALID partitions */
    if (a_valid && table_a->status == PART_STATUS_VALID) {
        ctx->selected_partition = 0;
        return FW_A_ADDR;
    }
    if (b_valid && table_b->status == PART_STATUS_VALID) {
        ctx->selected_partition = 1;
        return FW_B_ADDR;
    }

    /* Rule 3: Try INVALID partitions as last resort */
    if (a_valid && table_a->status == PART_STATUS_INVALID) {
        ctx->selected_partition = 0;
        return FW_A_ADDR;
    }
    if (b_valid && table_b->status == PART_STATUS_INVALID) {
        ctx->selected_partition = 1;
        return FW_B_ADDR;
    }
    
    /* No bootable partition found */
    return 0;
}

uint32_t get_upgrade_partition(BootContext* ctx)
{
    bool a_valid = ctx->table_a_valid;
    bool b_valid = ctx->table_b_valid;
    PartitionTable* table_a = &ctx->table_a;
    PartitionTable* table_b = &ctx->table_b;

    if (!a_valid && !b_valid) {
        ctx->selected_partition = 0;
        return TABLE_A_ADDR;
    }

    if (a_valid && table_a->status != PART_STATUS_VALID) {
        ctx->selected_partition = 0;
        return TABLE_A_ADDR;
    }

    if (b_valid && table_b->status != PART_STATUS_VALID) {
        ctx->selected_partition = 1;
        return TABLE_B_ADDR;
    }

    if (!b_valid) {
        ctx->selected_partition = 1;
        return TABLE_B_ADDR;
    }

    ctx->selected_partition = 0;
    return TABLE_A_ADDR;
}

/* Application API for updating partition status */
uint32_t get_boot_partition(void)
{
    BootContext ctx = {0};

    /* Load both partition tables */
    ctx.table_a_valid = find_latest_table_slot(TABLE_A_ADDR, &ctx.table_a, NULL);
    ctx.table_b_valid = find_latest_table_slot(TABLE_B_ADDR, &ctx.table_b, NULL);

    /* Rule 1: Check PENDING_VERIFY partitions */
    if (ctx.table_a_valid && ctx.table_a.status == PART_STATUS_PENDING_VERIFY) {
        return TABLE_A_ADDR;
    }
    
    if (ctx.table_b_valid && ctx.table_b.status == PART_STATUS_PENDING_VERIFY) {
        return TABLE_B_ADDR;
    }

    /* Rule 2: Prefer VALID partitions */
    if (ctx.table_a_valid && ctx.table_a.status == PART_STATUS_VALID) {
        return TABLE_A_ADDR;
    }
    if (ctx.table_b_valid && ctx.table_b.status == PART_STATUS_VALID) {
        return TABLE_B_ADDR;
    }

    /* Rule 3: Try INVALID partitions as last resort */
    if (ctx.table_a_valid && ctx.table_a.status == PART_STATUS_INVALID) {
        return  TABLE_A_ADDR;
    }
    if (ctx.table_b_valid && ctx.table_b.status == PART_STATUS_INVALID) {
        return TABLE_B_ADDR;
    }

    return 0;
}

int hardware_init(void *flash_str) {

	int ret;
    ret = flash_init(flash_str);
	if(ret) {
		return ret;
	}

    uint32_t flash_size = get_flash_size();

    if(flash_size <= FLASH_SZ_8M) {
        TABLE_A_ADDR = (TABLE_A_ADDR_8M);
        FW_A_ADDR    = (FW_A_ADDR_8M);
        FW_A_SIZE    = (TABLE_B_ADDR_8M - FW_A_ADDR_8M);
        TABLE_B_ADDR = (TABLE_B_ADDR_8M);
        FW_B_ADDR    = (FW_B_ADDR_8M);
        FW_B_SIZE    = (IMG_OFFSET_8M - FW_B_ADDR_8M);
    } else {
        TABLE_A_ADDR = (TABLE_A_ADDR_16M);
        FW_A_ADDR    = (FW_A_ADDR_16M);
        FW_A_SIZE    = (TABLE_B_ADDR_16M - FW_A_ADDR_16M);
        TABLE_B_ADDR = (TABLE_B_ADDR_16M);
        FW_B_ADDR    = (FW_B_ADDR_16M);
        FW_B_SIZE    = (IMG_OFFSET_16M - FW_B_ADDR_16M);
    }
    return 0;
}

/* Called by application after successful boot from PENDING_VERIFY partition */
bool application_confirm_valid(void *flash_str) {
    int ret = hardware_init(flash_str);
    if(ret)
        return false;

    uint32_t active_table_addr = get_boot_partition();
    if(active_table_addr == 0)
        return false;

    // dump_table_info(TABLE_A_ADDR);
    // dump_table_info(TABLE_B_ADDR);
    PartitionTable current_table;
    // printf("active_table_addr %x \r\n",active_table_addr);
    /* Read current table */
    if (!find_latest_table_slot(active_table_addr, &current_table, NULL)) {
        return false;
    }

    /* Only update if in PENDING_VERIFY state */
    if (current_table.status != PART_STATUS_PENDING_VERIFY) {
        return false;
    }

    /* Mark other partition as INVALID */
    uint32_t other_table_addr = (active_table_addr == TABLE_A_ADDR) ? 
                               TABLE_B_ADDR : TABLE_A_ADDR;
    printf("other_table_addr %x \r\n",other_table_addr);
    if (find_latest_table_slot(other_table_addr, &current_table, NULL)) {
        if (current_table.status == PART_STATUS_VALID || 
            current_table.status == PART_STATUS_PENDING_VERIFY) {
            printf("other_table_addr %x PART_STATUS_INVALID\r\n",other_table_addr);
            update_table_status(other_table_addr, PART_STATUS_INVALID);
        }
    }

    /* Mark current partition as VALID */
    printf("Mark current partition as VALID\r\n");
    if (!update_table_status(active_table_addr, PART_STATUS_VALID)) {
        return false;
    }

    return true;
}

/* Called by application if boot from PENDING_VERIFY partition fails */
bool application_mark_error(void) {
    uint32_t active_table_addr = get_boot_partition();
    PartitionTable current_table;

    if(active_table_addr == 0)
        return false;

    /* Read current table */
    if (!find_latest_table_slot(active_table_addr, &current_table, NULL)) {
        return false;
    }
    
    /* Only update if in PENDING_VERIFY state */
    if (current_table.status != PART_STATUS_PENDING_VERIFY) {
        return false;
    }
    
    /* Mark as ERROR state */
    return update_table_status(active_table_addr, PART_STATUS_ERROR);
}
