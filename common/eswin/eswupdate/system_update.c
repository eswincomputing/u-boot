// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
 * SPDX-License-Identifier: GPL-2.0
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

#include <common.h>
#include <blk.h>
#include <fs.h>
#include <fat.h>
#include <image.h>
#include <mmc.h>
#include <errno.h>
#include <update_init.h>
#include <system_update.h>
#include <boot_ab.h>
#include <display_update.h>
#include <asm/cache.h>
#include <asm/mbox.h>
#include <eswin/eswin-service.h>
#include <eswin/eswin-uMsg.h>
#include <eswin/esw_mkfs.h>
#include <eswin/ipc.h>
#include <dm/uclass.h>

static char dev_iface[DEV_STR_LEN];
static char dev_part[DEV_STR_LEN];

static struct blk_desc *mmc_dev_desc;
static struct disk_partition misc_part_info;
static struct disk_partition dev_part_info;
static struct bootloader_message *abc = NULL;
static struct boot_bank *bank = NULL;
static char eswversion[STR_LENGTH];
static char eswaddress[STR_LENGTH];
static char eswstatus[STR_LENGTH];
static int fs_initialize(int mode, char *dev_iface, char *dev_part)
{
    int fs_type = FS_TYPE_ANY;
    if(mode == UPDATE_MODE_OTA){
        fs_type = FS_TYPE_EXT;
    }else{
        fs_type = FS_TYPE_FAT;
    }
    if (fs_set_blk_dev(dev_iface, dev_part, fs_type)) {
        return -ENXIO;
    }

    return 0;
}

static int get_num_entries(int mode)
{
    loff_t len_boot;
    struct firmware_header_t *fht;

    fs_initialize(mode, dev_iface, dev_part);
    /* Read header of update file */
    if(fs_read(UPDATE_FILE_NAME, LOAD_ADDR_FHT, 0, sizeof(struct firmware_header_t), &len_boot)){
        printf("UPDATE: Struct firmware_header_t of %s load failed!\n", UPDATE_FILE_NAME);
        return -ENXIO;
    }

    if(len_boot < 0 || len_boot < sizeof(struct firmware_header_t)){
        printf("The length of %s header should be %ld, but %lld!\n",
                UPDATE_FILE_NAME,sizeof(struct firmware_header_t), len_boot);
        return -ENODATA;
    }
    fht = (struct firmware_header_t *) LOAD_ADDR_FHT;

    if(FHT_MAGIC != fht->magic){
        printf("UPDATE: FHT magic should be %d, but %d!\n",
                FHT_MAGIC, fht->magic);
        return -ENODATA;
    }

    return fht->num_entries;
}

/* Check the signature value of the update file.
 *
 * if verify correct, return 0;
 * else return -1.
 *
 * */
static int check_signature_value(int mode, int index)
{
    loff_t len_boot;
    struct firmware_entry_header_t *feht;
    int ret = 0;
    int i;
    struct udevice *dev;
    uint64_t offset, size;
    uint8_t sign_type, key_index;
    REQ_SRVC_T srvcReq;
    u32* signAddr;
    u32* imageAddr;
    u32* destAddr;
    RECV_MSG_EXT_T recvMsg;

    fs_initialize(mode, dev_iface, dev_part);
    // read struct of firmware_entry_header_t
    if(fs_read(UPDATE_FILE_NAME, LOAD_ADDR_FEHT,
                sizeof(struct firmware_header_t) + index*sizeof(struct firmware_entry_header_t),
                sizeof(struct firmware_entry_header_t), &len_boot)){
        printf("update: struct firmware_entry_header_t of %s load failed!\n", UPDATE_FILE_NAME);
        return -ENOENT;
    }
    feht = (struct firmware_entry_header_t *) LOAD_ADDR_FEHT;

    offset = feht->offset;             //signature offset between offset 0
    size = feht->size + SIGN_SIZE;     //payload size + signature size
    sign_type = feht->sign_type;
    key_index = feht->key_index;

    fs_initialize(mode, dev_iface, dev_part);
    if(fs_read(UPDATE_FILE_NAME, LOAD_ADDR_SIG_IMA, offset, size, &len_boot)){
        printf("update: signature and image of %s load failed!\n", UPDATE_FILE_NAME);
        return -ENOENT;
    }
    /* prepare service request data */
    if(sign_type == SIGN_TYPE_PLAINTEXT)
        return 0;
    size = feht->size;
    signAddr = (void *)LOAD_ADDR_SIG_IMA;
    imageAddr = (void *)(LOAD_ADDR_SIG_IMA + SIGN_SIZE);
    destAddr = (void *)TEST_DSET;

    srvcReq.SrvcType = SRVC_TYPE_SIGN_CHECK;                             //service ID
    srvcReq.data.SigChkReq.flag.bAlgorithm = (u8)sign_type;
    srvcReq.data.SigChkReq.flag.bKeyId = (u8)key_index;
    //sign address and size
    srvcReq.data.SigChkReq.bSignAddr = (u32)(u64)signAddr;
    srvcReq.data.SigChkReq.dwSignSize = SIGN_SIZE;
    //image address and size
    srvcReq.data.SigChkReq.bPayloadAddr = (u32)(u64)imageAddr;
    srvcReq.data.SigChkReq.dwPayloadSize = (u32)size;
    //dest address
    srvcReq.data.SigChkReq.dwDestAddr = (u32)(u64)destAddr;

    debug("check_signature_value: signAddr = [0x%8x].\r\n", srvcReq.data.SigChkReq.bSignAddr);
    debug("check_signature_value: imageAddr = [0x%8x].\r\n", srvcReq.data.SigChkReq.bPayloadAddr);

    /* flush data to ddr */
    flush_cache((unsigned long)signAddr, SIGN_SIZE);
    flush_cache((unsigned long)imageAddr, (u32)size);
    flush_cache((unsigned long)destAddr, SIGN_SIZE);
    uclass_get_device_by_name(UCLASS_MISC, "mbox_srvc@d0", &dev);
    eswin_umbox_service_get(dev);
    ret = eswin_umbox_service_send(dev, (u8*)&srvcReq);
    if (0 != ret) goto Failed;

    ret = eswin_umbox_service_recv(dev, (u32*)&recvMsg);
    if (0 != ret) goto Failed;

    debug("\r\n");
    debug("exec status: service_id     = [0x%08x]\r\n", recvMsg.dwServiceID);
    debug("             status         = [0x%08x]\r\n", recvMsg.IpcStatus);
    debug("             service_status = [0x%08x]\r\n", recvMsg.ServiceStatus);
    debug("             data_len       = [0x%08x]\r\n", recvMsg.size);

    eswin_umbox_service_free(dev);
    debug("do_sign_check: [signature] rsa + sha256, err = [0x%8x].\r\n", ret);

    return 0;

Failed:
    debug("eswin_umbox_test failed\n");
    return -EPERM;

}

static int get_abc_and_bank(void)
{
    int block = 1;

    if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, MISC_DEV_PART,
                &mmc_dev_desc, &misc_part_info, true) < 0) {
        printf("UPDATE: Get information of misc partiyion failed!\n");
        return -ENOENT;
    }

    get_bootmessage_from_disk(mmc_dev_desc, &misc_part_info, &abc);
#ifdef CONFIG_SYSTEM_UPDATE_B
    if(0 == abc->curr_bank){
        block = 1;
    }else if(1 == abc->curr_bank){
        block = 2;
    }else{
        printf("UPDATE: Read curr_bank failed!\n");
        free(abc);
        return -ENOENT;
    }
#endif
    get_bootbank_from_disk(mmc_dev_desc, &misc_part_info, block, &bank);

    return 0;
}

static int compare_version(uint32_t a, uint32_t b)
{
    //major.minor.patch.build
    printf("The update version number : %d\n the current version number : %d\n", a, b);
    return a>b?1:(a==b)?0:-1;
}

static unsigned long long int ntohl64(unsigned long long int netlong)
{
    return (((uint64_t)__bswap32(netlong) << 32) | __bswap32((uint64_t)(netlong)>>32));
}

/* Check the header of the update file.
 *
 * if verify correct, return 0;
 * else return -1.
 *
 */
static void *sign_destaddr = NULL;
static int check_header_valid(uint64_t size, uint8_t sign_type, uint8_t key_index, uint32_t version)
{
    int srvc_type = 3;    /* SRVC_TYPE_RSA_CRYPT_DECRYPT 0x03 */
    int i, ret;
    struct udevice *dev;
    struct signature_content *sc = NULL;
    struct firmware_entry_header_t *feht;
    REQ_SRVC_T srvcReq;
    u32* signAddr;
    u32* imageAddr;
    u32* destAddr;
    RECV_MSG_EXT_T recvMsg;

    /* prepare service request data */
    signAddr = (void *)LOAD_ADDR_SIG_IMA;
    imageAddr = (void *)(LOAD_ADDR_SIG_IMA + SIGN_SIZE);
    destAddr = (void *)TEST_DSET;
    sign_destaddr = (void *)TEST_DSET;
    if(sign_type == SIGN_TYPE_PLAINTEXT)
    {
        sign_destaddr = signAddr;
        sc = (struct signature_content *)sign_destaddr;
    }
    else
    {
        srvcReq.SrvcType = SRVC_TYPE_SIGN_CHECK;                             //service ID
        srvcReq.data.SigChkReq.flag.bAlgorithm = (u8)sign_type;
        srvcReq.data.SigChkReq.flag.bKeyId = (u8)key_index;
        //sign address and size
        srvcReq.data.SigChkReq.bSignAddr = (u32)(u64)signAddr;
        srvcReq.data.SigChkReq.dwSignSize = SIGN_SIZE;
        //image address and size
        srvcReq.data.SigChkReq.bPayloadAddr = (u32)(u64)imageAddr;
        srvcReq.data.SigChkReq.dwPayloadSize = (u32)size;
        //dest address
        srvcReq.data.SigChkReq.dwDestAddr = (u32)(u64)destAddr;

        debug("do_sign_check: signAddr = [0x%8x].\r\n", srvcReq.data.SigChkReq.bSignAddr);
        debug("do_sign_check: imageAddr = [0x%8x].\r\n", srvcReq.data.SigChkReq.bPayloadAddr);

        /* flush data to ddr */
        flush_cache((unsigned long)signAddr, SIGN_SIZE);
        flush_cache((unsigned long)imageAddr, (u32)size);
        flush_cache((unsigned long)destAddr, SIGN_SIZE);

        uclass_get_device_by_name(UCLASS_MISC, "mbox_srvc@d0", &dev);
        eswin_umbox_service_get(dev);
        ret = eswin_umbox_service_send(dev, (u8*)&srvcReq);
        if (0 != ret){
            printf("eswin_umbox_test_send failed\n");
        }
        ret = eswin_umbox_service_recv(dev, (u32*)&recvMsg);
        if (0 != ret){
            printf("eswin_umbox_test_recv failed\n");
        }

        debug("\r\n");
        debug("exec status: service_id     = [0x%08x]\r\n", recvMsg.dwServiceID);
        debug("             status         = [0x%08x]\r\n", recvMsg.IpcStatus);
        debug("             service_status = [0x%08x]\r\n", recvMsg.ServiceStatus);
        debug("             data_len       = [0x%08x]\r\n", recvMsg.size);

        eswin_umbox_service_free(dev);
        sc = (struct signature_content *)TEST_DSET;
    }


    if(ntohl(sc->magic) != SIGN_MAGIC){
        printf("signature_content magic %#x %#x\n",
                ntohl(sc->magic), SIGN_MAGIC);
        return -ENODATA;
    }

#ifdef CHECK_VALID_DEBUG
    printf("signature_content magic %#x %#x\n",
            ntohl(sc->magic), SIGN_MAGIC);
    printf("signature_content version %d\n",
            ntohl(sc->payload_flags));
    printf("signature_content modulid %d\n",
            ntohl(sc->mid));
    printf("signature_content vendorid%d\n",
            ntohl(sc->vendorid));
    printf("signature_content language %d\n",
            ntohl(hdr->language));
#endif

    switch(sc->payload_type){
        case KERNEL:
            return compare_version(version, bank->boot_version);
            break;
        case APPLICATION:
            return compare_version(version, 0);
            break;
        case ROOTFS:
            return compare_version(version, bank->rootfs_version);
            break;
        case PATCH:
            return compare_version(version, 0);
            break;
        default:
            printf("Invalid image file!\n");
            return -ENODATA;
    }

    return 0;
}

static uint32_t load_signature_and_payload(int mode, int index)
{
    int ret;
    loff_t len_boot;
    uint64_t offset, size;
    uint32_t version;
    uint8_t sign_type, key_index;
    struct firmware_entry_header_t *feht;
    struct signature_content *sc = NULL;
    const char *dev_part_str;
    int i;

    fs_initialize(mode, dev_iface, dev_part);
    /* read struct of firmware_entry_header_t */
    if(fs_read(UPDATE_FILE_NAME, LOAD_ADDR_FEHT,
                sizeof(struct firmware_header_t) + index * sizeof(struct firmware_entry_header_t),
                sizeof(struct firmware_entry_header_t), &len_boot)){
        printf("UPDATE: Struct firmware_entry_header_t of %s load failed!\n", UPDATE_FILE_NAME);
        return -ENOENT;
    }
    feht = (struct firmware_entry_header_t *) LOAD_ADDR_FEHT;

    version = feht->version;
    offset = feht->offset;
    size = feht->size + SIGN_SIZE;
    sign_type = feht->sign_type;
    key_index = feht->key_index;

    fs_initialize(mode, dev_iface, dev_part);
    if(fs_read(UPDATE_FILE_NAME, LOAD_ADDR_SIG_IMA, offset, size, &len_boot)){
        printf("UPDATE: Struct firmware_entry_header_t of %s load failed!\n", UPDATE_FILE_NAME);
        return -ENOENT;
    }

    /* header check */
    ret = get_abc_and_bank();
    if(ret < 0)
        return -ENXIO;

    size = feht->size;
    ret = check_header_valid(size, sign_type, key_index, version);
    if(ret < 0)
        return -ENXIO;

    return version;
}

static int emmc_write_kernel(uint32_t version, uint64_t offset, uint64_t size)
{
    int ret;
    int curr_bank = 0;
    int block = 1;
    loff_t len;
    loff_t len_boot;
    unsigned long time;
    const char *filename;
    const char *dev_part_str;
    dev_part_str = UPDATE_BOOTA_DEV_PART;      /* "0#boota" */

#ifdef CONFIG_SYSTEM_UPDATE_B
    if(0 == abc->curr_bank){
        dev_part_str = UPDATE_BOOTB_DEV_PART;  /* "0#bootb" */
    }else if(1 == abc->curr_bank){
        dev_part_str = UPDATE_BOOTA_DEV_PART;
    }else{
        printf("UPDATE: Read curr_bank failed!\n");
        free(abc);
        return -ENXIO;
    }
#endif
    if (fs_set_blk_dev(MMC_DEV_IFACE, dev_part_str, FS_TYPE_FAT)) {
        ret =  mkfs_fat(MMC_DEV_IFACE, dev_part_str);
        if( 0 != ret ) {
            printf("mkfs_fat failed\n");
            return -ENXIO;
        }
        if (fs_set_blk_dev(MMC_DEV_IFACE, dev_part_str, FS_TYPE_FAT)) {
            printf("part_get_info_by_dev_and_name_or_num failed\n");
            return -ENXIO;
        }
    }

    filename = BOOT_FILE_IMG;    /* "fitimage" */

    time = get_timer(0);
    ret = fs_write(filename, LOAD_ADDR_SIG_IMA + SIGN_SIZE, 0x0, size, &len);
	fs_close();
    time = get_timer(time);
    if(0 == ret ){

#ifdef CONFIG_SYSTEM_UPDATE_B
        if(0 == abc->curr_bank){
            curr_bank = 1;
            block = 2;
        }else{
            curr_bank = 0;
        }
#endif
        abc->curr_bank = curr_bank;
        bank->boot_version = version;
        bank->successful_flag = 1;
        printf("%s(version %d) has been successfully writen in %s\n", filename, bank->boot_version, dev_part_str);
        bootloader_message_store(mmc_dev_desc, &misc_part_info, abc);
        boot_bank_store(mmc_dev_desc, &misc_part_info, block, bank);
    }
    else{
        /*TODO*/
        // UI_DISPLAY();
        strcpy(eswstatus,"status: writing error!");
        draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);

#ifndef CONFIG_SYSTEM_UPDATE_A
        strcpy(abc->command, "boot_normal");
#endif
        bank->successful_flag = 0;
#ifdef CONFIG_SYSTEM_UPDATE_C
        abc->curr_bank = 1;
#endif
        abc->crc32_bc = bootmessage_compute_crc(abc);
        bootloader_message_store(mmc_dev_desc, &misc_part_info, abc);

        bank->crc32_bb = bootbank_compute_crc(bank);
        boot_bank_store(mmc_dev_desc, &misc_part_info, block, bank);

        free(abc);
        free(bank);
        printf("UPDATE: emmc_write_op write boot part failed!\n");
        return -1;
    }
    printf("%s:%llu bytes written in %lu ms\n", filename, len, time);

    return block;
}

static int emmc_write_application(uint32_t version, uint64_t offset, uint64_t size)
{
    /*TODO*/
    return 0;
}

static int emmc_write_rootfs(uint32_t version, uint64_t offset, uint64_t size)
{
    void *addr;
    uint64_t cnt, ret;
    int block = 1;
    const char *dev_part_str;
    struct disk_partition rootfs_part_info;

    /* addr blk cnt */
    dev_part_str = UPDATE_ROOTFSA_DEV_PART;  /* "rootfsa" */

    if (part_get_info_by_dev_and_name_or_num(MMC_DEV_IFACE, dev_part_str,
                &mmc_dev_desc, &rootfs_part_info, true) < 0) {
        printf("UPDATE: Get information of rootfs partiyion failed!\n");
        return -ENOENT;
    }

    addr = (void *)(LOAD_ADDR_SIG_IMA + SIGN_SIZE);
    cnt = DIV_ROUND_UP(size, rootfs_part_info.blksz);  /* blkcnt */

    ret = blk_dwrite(mmc_dev_desc, rootfs_part_info.start, cnt, addr);

    printf("%lld blocks written: %s\n", ret, (ret == cnt) ? "OK" : "ERROR");
    if(ret == cnt){
        bank->rootfs_version = version;
        abc->rtfs_bank = 0;
        bootloader_message_store(mmc_dev_desc, &misc_part_info, abc);
        boot_bank_store(mmc_dev_desc, &misc_part_info, block, bank);
        printf("rootfs (version : %d) has been successfully writen in %s\n", bank->rootfs_version, dev_part_str);
    }else{
        /*TODO*/
        // UI_DISPLAY();
        strcpy(eswstatus,"status: writing error!");
        draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);

#ifdef CONFIG_SYSTEM_UPDATE_C
        strcpy(abc->command, "boot_normal");
        abc->rtfs_bank = 1;
#endif
        abc->crc32_bc = bootmessage_compute_crc(abc);
        bootloader_message_store(mmc_dev_desc, &misc_part_info, abc);

        free(abc);
        free(bank);
        printf("UPDATE: emmc rootfs partition write failed!\n");
        return -ENXIO;
    }
    return 0;
}

static int emmc_write_patches(uint32_t version, uint64_t offset, uint64_t size)
{
    /*TODO*/
    return 0;
}

int esw_update(int mode)
{
    int block = 1;
    int i, ret;
    int num_entries;
    uint64_t payload_offset;
    struct signature_content *sc;

    switch(mode){
        case UPDATE_MODE_OTA:
            strcpy(dev_iface, MMC_DEV_IFACE);    /* "mmc" */
            strcpy(dev_part, USERDATA_DEV_PART); /* "0#userdata" */
            strcpy(eswstatus,"status: starting OTA update");
            strcpy(eswaddress,"userdata/update.img");
            break;
        case UPDATE_MODE_SD:
        case UPDATE_MODE_OTA_SD:
            strcpy(dev_iface, MMC_DEV_IFACE);    /* "mmc" */
            strcpy(dev_part, SD_DEV_PART);       /* "1" */
            strcpy(eswstatus,"status: starting SD update");
            strcpy(eswaddress,"sd/update.img");
            break;
        case UPDATE_MODE_USB:
        case UPDATE_MODE_OTA_USB:
            strcpy(dev_iface, USB_DEV_IFACE);    /* "usb" */
            strcpy(dev_part, USB_DEV_PART);      /* "0" */
            strcpy(eswstatus,"status: starting USB update");
            strcpy(eswaddress,"usb/update.img");
            run_command("usb start", 0);
            break;
        default:
            strcpy(eswstatus,"status: illegal mode");
            printf("Illegal value!");
            return -ENXIO;
    }
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_ADD, eswaddress);
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
    draw_progressBar(PROGRESS_5);
    /* get num_entries from struct of firmware_header_t */
    num_entries = get_num_entries(mode);
    if(num_entries < 0){
        strcpy(eswstatus,"status: no update image");
        draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
        draw_progressBar(PROGRESS_8);
        return -ENXIO;
    }

    /* signature verification */
    if(UPDATE_MODE_SD == mode || UPDATE_MODE_USB == mode){
        strcpy(eswstatus,"status: verifying sign...");
        draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
        for(i=0; i<num_entries; i++){
            ret = check_signature_value(mode, i);
            if(ret < 0){
                strcpy(eswstatus,"status: sign verify failed");
                draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
                draw_progressBar(PROGRESS_15);
                printf("UPDATE: The %d payload image verify error!\n", i);
                break;
            }
        }
    }
    draw_progressBar(PROGRESS_20);

    /* load signature part and payload image */
    strcpy(eswstatus,"status: loading sign...");
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
    for(i=0; i<num_entries; i++){
        ret = load_signature_and_payload(mode, i);
        if(ret < 0){
            strcpy(abc->command, "boot_normal");
            strcpy(eswstatus,"status: load sign failed");
            draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
            draw_progressBar(PROGRESS_25);
            printf("UPDATE: The %d payload image load error!\n", i);
            break;
        }
        draw_progressBar(PROGRESS_20 + (i + 1) * (PROGRESS_40 / num_entries));

        sc = (struct signature_content *)sign_destaddr;
        payload_offset = sc->payload_offset  + SIGN_SIZE;
        strcpy(eswstatus,"status: writing package...");
        draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);
        draw_progressBar(PROGRESS_20 + (i + 1) * (PROGRESS_50 / num_entries));
        switch(sc->payload_type){
            case KERNEL:
                block = emmc_write_kernel(ret, payload_offset, sc->payload_size);
                if(block < 0)
                    return block;
                break;
            case APPLICATION:
                ret = emmc_write_application(ret, payload_offset, sc->payload_size);
                if(ret < 0)
                    return ret;
                break;
            case ROOTFS:
                ret = emmc_write_rootfs(ret, payload_offset, sc->payload_size);
                if(ret < 0)
                    return ret;
                 break;
            case PATCH:
                ret = emmc_write_patches(ret, payload_offset, sc->payload_size);
                if(ret < 0)
                    return ret;
                break;
            default:
                printf("UPDATE: Invalid image file!\n");
                return -ENXIO;
        }
        draw_progressBar(PROGRESS_20 + (i + 1) * (PROGRESS_60 / num_entries));
    }

    strcpy(eswstatus,"status: writing misc!");
    draw_updateinfo(TEXTBAR_WIDTH, HEIGHT_STA, eswstatus);

    strcpy(abc->command, "boot_normal");
    abc->crc32_bc = bootmessage_compute_crc(abc);
    bootloader_message_store(mmc_dev_desc, &misc_part_info, abc);
    draw_progressBar(PROGRESS_90);

    bank->crc32_bb = bootbank_compute_crc(bank);
    boot_bank_store(mmc_dev_desc, &misc_part_info, block, bank);

    free(abc);
    free(bank);

    draw_progressBar(PROGRESS_95);

    return 0;
}
