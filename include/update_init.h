// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef __UPDATE_INIT_H_
#define __UPDATE_INIT_H_

#define FHT_MAGIC                0x42575345
#define VENDOR_MAGIC             0x44565345
#define SIGN_MAGIC               0x575345bb
#define BOOTMESS_MAGIC           0x10101010
#define BOOTBANK_MAGIC           0x10101010

#define MMC_DEV_IFACE            "mmc"
#define USB_DEV_IFACE            "usb"
#define SD_DEV_PART              "1:0"
#define USB_DEV_PART             "0"

#define MMC_DEV                  0
#define PART_MISC                "misc"

struct item_t {
        uint32_t id;
        uint32_t offset;
        uint32_t size;
        uint32_t flag;
};

struct vendor_info_t {
        uint32_t magic;
        uint32_t version;
        uint32_t total_item;
        uint32_t free_offset;
        uint32_t free_size;
        struct item_t item[0];
};

#define VENDER_SIZE   0x100000
#define VENDER_OFFSET 0xF00000

#if defined(CONFIG_SYSTEM_UPDATE_B) || defined(CONFIG_SYSTEM_UPDATE_C)
#define UPDATE_BOOT_DEV_PART    "0#boota"
#define UPDATE_BOOTB_DEV_PART    "0#bootb"
#define UPDATE_ROOTFSA_DEV_PART  "0#rootfsa"
#define UPDATE_ROOTFSB_DEV_PART  "0#rootfsb"
#define MISC_DEV_PART            "0#misc"
#define BOOTCHAIN_DEV_PART       "0#bootchain"
#define USERDATA_DEV_PART        "0#userdata"
#else
#define UPDATE_BOOT_DEV_PART    "0#boot"
#define UPDATE_ROOTFSA_DEV_PART  "0#rootfs"
#define UPDATE_ROOT_DEV_PART  "0#root"
#define MISC_DEV_PART            "0#misc"
#define BOOTCHAIN_DEV_PART       "0#bootchain"
#define USERDATA_DEV_PART        "0#userdata"
#endif


#define __bswap32(x) \
        ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
        (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))

#define UPDATE_FILE_NAME         "update.img"
#define BOOT_FILE_IMG            "fitimage"
#define ROOTFS_FILE_IMG          "rootfs.img"

/* where to load files into memory */
#define LOAD_ADDR_FHT               (ulong)0x8ffffdb0
#define LOAD_ADDR_FEHT              (ulong)0x8ffffdc0
#define LOAD_ADDR_SIG_IMA           (ulong)0x90000000

/*signature*/
#define SIGN_SIZE          256
#define TEST_IMAGE         0x90000300
#define TEST_SIGN          0x90000200
#define TEST_DSET          0x94400250

/*display setting*/
#ifdef CONFIG_DRM_ESWIN_DW_HDMI
#define TEXTBAR_WIDTH       400
#define HEIGHT_VER          200
#define HEIGHT_ADD          250
#define HEIGHT_STA          300
#else
#define TEXTBAR_WIDTH       100
#define HEIGHT_VER          130
#define HEIGHT_ADD          150
#define HEIGHT_STA          170
#endif

#define STR_LENGTH          100
#define DEV_STR_LEN         10

/*progressbar percentage*/
#define PROGRESS_1           1
#define PROGRESS_2           2
#define PROGRESS_5           5
#define PROGRESS_8           8
#define PROGRESS_10          10
#define PROGRESS_15          15
#define PROGRESS_20          20
#define PROGRESS_25          25
#define PROGRESS_40          40
#define PROGRESS_50          50
#define PROGRESS_60          60
#define PROGRESS_90          90
#define PROGRESS_95          95
#define PROGRESS_98          98
#define PROGRESS_100         100

enum update_mode {
    UPDATE_MODE_NORMAL = 0,
    UPDATE_MODE_OTA,
    UPDATE_MODE_OTA_SD,
    UPDATE_MODE_OTA_USB,
    UPDATE_MODE_RECOVERY,
    UPDATE_MODE_SD,
    UPDATE_MODE_USB,
    UPDATE_MODE_UNDEFINE,
};

int ota_update_init(void);
void ota_update_exit(void);

int mmc_update_init(void);
void mmc_update_exit(void);

int usb_update_init(void);
void usb_update_exit(void);

int draw_background(void);

void es_bootspi_wp_cfg(int enable);

#endif
