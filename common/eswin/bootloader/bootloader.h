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
 */

#ifndef __BOOTLOADER_H
#define __BOOTLOADER_H

#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#define NSIGN_MINIMUM_VERSION 0x00030000

#define VERSION_MAJOR(v)   (((v) >> 16) & 0xFF)
#define VERSION_MINOR(v)   (((v) >> 8) & 0xFF)
#define VERSION_PATCH(v)   ((v) & 0xFF)

#define FLASH_SZ_8M				0x00800000
#define FLASH_SZ_16M			0x01000000

#define ENTRYHEAD_SIZE    512
#define SIGNATURE_SIZE    256
#define NSIGNHEAD_SIZE    (4 * 1024)
#define BTR_HASH_DIG_SIZE  32

#define TABLE_SIZE             (4 * 1024)         /* 4KB */
#define ALIGN_4K               (4 * 1024)         /* 4KB */

extern uint32_t TABLE_A_ADDR;
extern uint32_t FW_A_ADDR;
extern uint32_t FW_A_SIZE;
extern uint32_t TABLE_B_ADDR;
extern uint32_t FW_B_ADDR;
extern uint32_t FW_B_SIZE;

typedef enum fw_offset {
	/* Head */
	FW_HEAD_OFFSET      = 0x0000UL,     /* HEAD 4K */
	/* Used to store infomation such as the space offset and size of each part of the fw */
	BOOTLOAD_INFO_OFFSET      = 0x1000UL,     /* 4K */
	/* Data */
	DDR_OFFSET          = 0x2000UL,     /* 248K */
	D2D_OFFSET          = 0x40000UL,    /* 64K */
	/* FW */
	BOOTCHAIN_OFFSET    = 0x50000UL,    /* 64K */

    TABLE_A_ADDR_16M =  0x60000UL, /* 4K */
    FW_A_ADDR_16M    =  0x61000UL, /* 7M - 388K */
    TABLE_B_ADDR_16M = 0x700000UL, /* 4K */
    FW_B_ADDR_16M    = 0x701000UL, /* 6.5M - 4K */

	IMG_OFFSET_16M          = 0xd80000UL,   /* 1.5M */
	ENV_OFFSET_16M          = 0xf00000UL,   /* 512K */
	BOARDINFO_OFFSET_16M    = 0xf80000UL,   /* 512K */


    TABLE_A_ADDR_8M =  0x60000UL, /* 4K */
    FW_A_ADDR_8M    =  0x61000UL, /* 3.5M - 388K */
    TABLE_B_ADDR_8M = 0x380000UL, /* 4K */
    FW_B_ADDR_8M    = 0x381000UL, /* 3M - 4K */

	IMG_OFFSET_8M          = 0x680000UL,   /* 512k */
	ENV_OFFSET_8M          = 0x700000UL,   /* 512K */
	BOARDINFO_OFFSET_8M    = 0x780000UL,   /* 512K */

}FirmwareOffset;

typedef enum {
    PUBKEY_RSA = 0,
    PUBKEY_ECC,
    DDR = 0x10,
    D2D = 0x20,
    BOOTLOADER = 0x30,
    KERNEL = 0x40,
    ROOTFS = 0x50,
    APPLICATION = 0x60,
    FIRMWARE = 0x70,
    PATCH = 0x80,
    LOADABLE_SRVC = 0x90,
} PayloadTypes;

/* Execution unit flag */
#define SIGN_EXEC_UNIT_E21 0
#define SIGN_EXEC_UNIT_U84 1
#define SIGN_EXEC_UNIT_ECB 2


typedef struct loadable_info {
    uint32_t load_addr;
    uint32_t init_ofs;
    uint32_t destroy_ofs;
    uint32_t ioctl_ofs;
    uint32_t load_flags;
    uint32_t irq_num;
    uint32_t irq_ofs;
} LoadableInfo;

typedef struct __attribute__((packed, aligned(1))) sign_info {
    uint32_t magic;
    uint32_t reserved0;
    uint64_t link_addr; /* in use for loadable service */
    uint64_t payload_offset;
    uint64_t payload_size; /* size in byte */
    uint64_t load_addr;    /* where to load the image */
    uint64_t entry_addr; /* Entry address of the program and CPU will jump into */
    uint8_t  payload_flags;  /* Payload is encrypted or not */
    uint8_t  digest_mthd;    /* digest algorithm use SHA256 or SM3 */
    uint8_t  encrypted_mthd; /* Payload encrypted algorithm */
    uint8_t  vid;            /* vendor id */
    uint8_t  reserved1;
    uint8_t  lang[3];
    uint64_t mid;          /* market id */
    uint8_t  payload_type; /* Payload type */
    uint8_t  boot_flags;   /* Boot by E21 or U84 */
    uint8_t  reserved2[6];
    uint64_t devid;      /* device id */
    uint8_t  params[16]; /* Parameters for next boot stage */
    uint8_t  reserved3[16];
    LoadableInfo load_info;
    uint32_t               reserved4;
    uint8_t                digest[BTR_HASH_DIG_SIZE];
} SignInfo;

typedef struct __attribute__((packed)) firmware_entry_header{
    uint32_t version;
    uint64_t offset;       /* Signature offset between offset 0 */
    uint64_t size;         /* Signature size + payload size */
    uint8_t sign_type;     /* See BR_SIGN_TYPE_ECDSA|RSA|PLAINTETXT */
    uint8_t key_index;     /* Which key to verify the Signature, see BR_SIGN_TYPE_RSA */
    uint8_t payload_type; /* Payload type */
    uint8_t  last_flag;
    uint8_t reserved0[4];
    uint32_t nsign_version;
    uint32_t crc32;
}FirmwareEntryHeader;

typedef struct __attribute__((packed)) firmware_header{
    uint32_t magic;
    uint32_t num_entries;  /* Number of entries following this structures */
    FirmwareEntryHeader entries[0];
} FirmwareHeader;

#define FHT_MAGIC                0x42575345
#define MAGIC_PART 0x50415254

/* Partition Status Definition (as specified) */
typedef enum {
    PART_STATUS_EMPTY          = 0xFFFF,   /* Empty/unprogrammed */
    PART_STATUS_PENDING_VERIFY = 0xEEEE,   /* Update complete, pending verification */
    PART_STATUS_VALID          = 0xEECC,   /* Valid and verified */
    PART_STATUS_UPDATING       = 0xEE88,   /* Update in progress */
    PART_STATUS_INVALID        = 0x8888,   /* Invalid (old version) */
    PART_STATUS_ERROR          = 0x8080    /* Error state (failed verification) */
} PartitionStatus;

struct partitiontype {
    char *name;
    uint16_t id;
};

/* Partition Information Structure (stored in 4KB table area) */
typedef struct __attribute__((packed)) {
    /* Identification */
    uint32_t magic;                     /* Magic number: 0x50415254 ("PART") */
    uint32_t version;                   /* Firmware version */

    /* Partition Metadata */
    uint16_t status;                    /* PartitionStatus enum value */
    uint8_t boot_attempts;              /* Boot attempts in PENDING_VERIFY state */
    uint8_t max_boot_attempts;          /* Maximum allowed boot attempts (default: 3) */

    /* Firmware Information */
    uint32_t fw_size;                   /* Firmware size in bytes */
    uint32_t fw_crc32;                  /* CRC32 of firmware */
    
    /* Update Information */
    uint32_t update_timestamp;          /* Last update timestamp */
    uint32_t previous_version;          /* Previous firmware version */

    /* Integrity Check */
    uint32_t table_crc32;               /* CRC32 of this table (excluding this field) */

} PartitionTable;

/* Since Nor Flash can only change 1 to 0, not 0 to 1, we need special handling for state updates */

/* Flash table slot structure */
#define TABLE_SLOT_SIZE        512  /* Each slot is 128 bytes */
#define TABLE_SLOTS_COUNT      (TABLE_SIZE / TABLE_SLOT_SIZE)  /* 4KB/128B = 32 slots */

/* Table slot structure */
typedef struct __attribute__((packed)) {
    PartitionTable table;              /* Partition table data */
    uint32_t slot_version;             /* Slot version (incremental) */
    uint32_t slot_crc32;               /* CRC32 of the entire slot */
} TableSlot;

/* Bootloader context */
typedef struct {
    PartitionTable table_a;
    PartitionTable table_b;
    uint32_t selected_partition;        /* 0 = A, 1 = B */
    bool table_a_valid;
    bool table_b_valid;
} BootContext;

/* Function prototypes */
uint32_t calculate_crc32(const uint8_t* data, uint32_t length);
uint32_t calculate_firmware_crc32(uint32_t fw_addr, uint32_t fw_size);
uint32_t calculate_fs_firmware_crc32(uint32_t fw_addr, uint32_t fw_size);
bool verify_table_crc(PartitionTable* table);
bool verify_firmware_crc(uint32_t fw_addr, uint32_t fw_size, uint32_t expected_crc);
int verify_nsign_fw_crc(uint32_t fw_addr);
bool verify_fs_firmware_crc(uint32_t fw_addr, uint32_t fw_size, uint32_t expected_crc);
uint32_t select_boot_partition(BootContext* ctx);
bool update_table_status(uint32_t table_addr, PartitionStatus new_status);
bool increment_boot_attempts(uint32_t table_addr);
void jump_to_firmware(uint32_t fw_addr);
void enter_recovery_mode(void);
bool find_latest_table_slot(uint32_t table_base_addr, PartitionTable* result, uint32_t* version);
bool write_table_slot(uint32_t table_base_addr, PartitionTable* table, uint32_t slot_version);
bool load_partition_table(uint32_t table_addr, PartitionTable* table);
char* get_payload_name(uint32_t typ_id);
void dump_table_info(uint32_t table_base_addr);

#endif
