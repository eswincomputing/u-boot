// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef _SYSTEM_UPDATE_H
#define _SYSTEM_UPDATE_H

#define BTL_HASH_DIG_SIZE 32
#define SIGNATURE_SIZE    256

typedef struct __attribute__((packed)) firmware_entry_header_t{
    uint32_t version;
    uint64_t offset;       /* Signature offset between offset 0 */
    uint64_t size;         /* Signature size + payload size */
    uint8_t sign_type;     /* See BR_SIGN_TYPE_ECDSA|RSA|PLAINTETXT */
    uint8_t key_index;     /* Which key to verify the Signature, see BR_SIGN_TYPE_RSA */
    uint8_t payload_type; /* Payload type */
    uint8_t  last_flag;
    uint8_t reserved0[4];
    uint32_t reserved1;
    uint32_t Reserved2;
}firmware_entry_header_t;

typedef struct __attribute__((packed)) firmware_header_t{
    uint32_t magic;
    uint32_t num_entries;  /* Number of entries following this structures */
    firmware_entry_header_t entries[0];
}firmware_header_t;

typedef struct __attribute__((packed)) loadable_info_t{
    uint32_t load_addr;
    uint32_t init_ofs;
    uint32_t destroy_ofs;
    uint32_t ioctl_ofs;
    uint32_t load_flags;
    uint32_t irq_num;
    uint32_t irq_ofs;
}loadable_info_t;

enum payload_type {
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
};

typedef struct __attribute__((packed)) signature_content{
    uint32_t magic;
    uint32_t reserved0;
    uint64_t link_addr;        /* in use for loadable service */
    uint64_t payload_offset;
    uint64_t payload_size;     /* size in byte */
    uint64_t load_addr;        /* where to load the image */
    uint64_t entry_addr;       /* Entry address of the program and CPU will jump into */
    uint8_t  payload_flags;  /* Payload is encrypted or not */
    uint8_t  digest_mthd;    /* digest algorithm use SHA256 or SM3 */
    uint8_t  encrypted_mthd; /* Payload encrypted algorithm */
    uint8_t  vid;           /* Vendor Identifier */
    uint8_t reserved1;
    uint8_t lang[3];
    uint64_t mid;              /* market id */
    uint8_t  payload_type;     /* Payload type */
    uint8_t  boot_flags;       /* Boot by E21 or U84 */
    uint8_t  reserved2[6];
    uint64_t devid;            /* device id */
    uint8_t  params[16];       /* Parameters for next boot stage */
    uint8_t reserved3[16];
    loadable_info_t load_info;
    uint32_t reserved4;
    uint8_t  digest[BTL_HASH_DIG_SIZE];
}signature_content;

typedef struct __attribute__((packed)) bootloader_message{
    uint32_t magic;
    char command[32];    /* Boot command */
    char status[32];     /* Boot status */
    char package[256];
    uint8_t bm_version;
    uint8_t nb_bank;     /* Number of bank being managed */
    uint8_t curr_bank;   /* THe currect runing bank */
    uint8_t modulID;     /* ID number of modul used for compare */
    uint8_t regionID;    /* ID number of region used for compare */
    uint8_t language;    /* Language of product used for compare */
    uint8_t rtfs_bank;   /* THe currect rootfs runing bank */
    uint8_t update_type; /* update type : A/B/C */
    uint8_t reserved[177];
    uint32_t crc32_bc;
}bootloader_message;

typedef struct __attribute__((packed)) boot_bank{
    uint32_t magic;
    uint32_t version;
    uint32_t boot_version;   /* The current version of partition of boot */
    uint32_t rootfs_version; /* The update version of partition of rootfs */
    uint32_t serial_number;
    uint8_t successful_flag; /* 1 if this bank has booted successfully, 0 otherwise */
    uint8_t reserved[7];
    uint32_t crc32_bb;
}boot_bank;

int esw_update(int mode);
#endif
