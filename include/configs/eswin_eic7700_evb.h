// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Xiang Xu <xuxiang@eswincomputing.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

#define CONFIG_STANDALONE_LOAD_ADDR 0x80200000

/* Environment options */
#define BOOT_TARGET_DEVICES(func) \
    func(MMC, mmc, 0) \
    func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>

#define CFG_EXTRA_ENV_SETTINGS \
    "fdtfile=" CONFIG_DEFAULT_FDT_FILE "\0" \
    "bootdelay=5\0" \
    "fdt_high=0xffffffffffffffff\0" \
    "initrd_high=0xffffffffffffffff\0" \
    "kernel_addr_r=0x84000000\0" \
    "kernel_comp_addr_r=0x98300000\0" \
    "kernel_comp_size=0x10000000\0" \
    "boot_conf_addr_r=0xc0000000\0" \
    "fdt_addr_r=0x88000000\0" \
    "scriptaddr=0x88100000\0" \
    "pxefile_addr_r=0x88200000\0" \
    "ramdisk_addr_r=0x88300000\0" \
    "typeid_swap=0657FD6D-A4AB-43C4-84E5-0933C84B4F4F\0" \
    "typeid_filesystem=0FC63DAF-8483-4772-8E79-3D69D8477DE4\0" \
    "uuid_rootfsA=80a5a8e9-c744-491a-93c1-4f4194fd690a\0" \
    "uuid_swap=5ebcaaf0-e098-43b9-beef-1f8deedd135e\0" \
    "usbupdate=ext4load usb 0 0x90000000 usbupdate.scr;source 0x90000000\0" \
    "sdupdate=ext4load mmc 1:1 0x90000000 sdupdate.scr;source 0x90000000\0" \
    "partitions=name=misc,start=1MiB,size=512KiB;name=env,size=512KiB;name=vendor,size=1MiB;name=boot,start=5MiB,size=500MiB,type=${typeid_filesystem};name=swap,size=4096MiB,type=${typeid_swap},uuid=${uuid_swap};name=root,size=-,type=${typeid_filesystem},uuid=${uuid_rootfsA}\0" \
    "gpt_partition=gpt write mmc ${emmc_dev} $partitions\0" \
    "emmc_dev=0\0" \
    "boot_conf_file=/extlinux/extlinux.conf\0" \
    "stdin=serial,usbkbd\0" \
    "stderr=serial,vidconsole\0" \
    "stdout=serial,vidconsole\0"
    //BOOTENV
#undef CONFIG_BOOTCOMMAND

#define CONFIG_BOOTCOMMAND \
    "sysboot mmc ${emmc_dev}:4 any $boot_conf_addr_r $boot_conf_file;"

#endif /* __CONFIG_H */
