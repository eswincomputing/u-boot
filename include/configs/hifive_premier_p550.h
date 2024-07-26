// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 * Author: Xiang Xu <xuxiang@eswincomputing.com>
 */

#ifndef __CONFIG_H
#define __CONFIG_H

#include <linux/sizes.h>

#define CFG_MALLOC_F_ADDR   0xf0000000
// #define CONFIG_BOOT_SATA

/* Environment options */
#define BOOT_TARGET_DEVICES(func) \
    func(MMC, mmc, 0) \
    func(DHCP, dhcp, na)

#include <config_distro_bootcmd.h>

#define SATA_ENV \
    "sata_init=sata init\0" \
    "loadimage=fatload sata 0:1 0x90000000 fitimage\0"

#define CFG_EXTRA_ENV_SETTINGS \
    "bootdelay=2\0" \
    "fdt_high=0xffffffffffffffff\0" \
    "initrd_high=0xffffffffffffffff\0" \
    "kernel_addr_r=0x84000000\0" \
    "fdt_addr_r=0x88000000\0" \
    "scriptaddr=0x88100000\0" \
    "pxefile_addr_r=0x88200000\0" \
    "ramdisk_addr_r=0x88300000\0" \
    "stdin=serial,usbkbd\0" \
    "stderr=serial,vidconsole\0" \
    "stdout=serial,vidconsole\0" \
    "kernel_comp_addr_r=0xa0000000\0" \
    "kernel_comp_size=0x4000000\0" \
    "emmc_dev=0\0" \
    "usbupdate=ext4load usb 0 0x90000000 usbupdate.scr;source 0x90000000\0" \
    "sdupdate=ext4load mmc 1:1 0x90000000 sdupdate.scr;source 0x90000000\0" \
    "typeid_efi=C12A7328-F81F-11D2-BA4B-00A0C93EC93B\0" \
    "partitions=name=misc,start=1MiB,size=512KiB;name=env,size=512KiB;name=boot,size=100MiB,type=${typeid_efi};name=rootfs,size=30GiB;name=userdata,size=-;\0" \
    "gpt_partition=gpt write mmc ${emmc_dev} $partitions\0"

#undef CONFIG_BOOTCOMMAND
#define CONFIG_BOOTCOMMAND \
    "bootflow scan;"


#endif /* __CONFIG_H */