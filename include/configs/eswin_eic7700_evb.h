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

#if (defined CONFIG_SYSTEM_UPDATE_B) || (defined CONFIG_SYSTEM_UPDATE_C)
#define MMC_ENV \
    "emmc_dev=0\0" \
    "partitions=name=misc,start=1MiB,size=100KiB;name=env,size=10KiB;name=vendor,size=1MiB;name=boota,size=100MiB;name=bootb,size=100MiB;name=rootfsa,size=2GiB;name=rootfsb,size=2GiB;name=userdata,size=-;\0" \
    "gpt_partition=gpt write mmc ${emmc_dev} $partitions\0" \
    "mmcroot=/dev/mmcblk0p6 rootfstype=ext4 rootwait\0" \
    "loadimage=fatload mmc 0#boota 0x90000000 fitimage\0"
#else
#define MMC_ENV \
    "emmc_dev=0\0" \
    "partitions=name=misc,start=1MiB,size=100KiB;name=env,size=10KiB;name=vendor,size=1MiB;name=boot,size=100MiB;name=rootfs,size=2GiB;name=userdata,size=-;\0" \
    "gpt_partition=gpt write mmc ${emmc_dev} $partitions\0" \
    "mmcroot=/dev/mmcblk0p5 rootfstype=ext4 rootwait\0" \
    "loadimage=fatload mmc 0#boot 0x90000000 fitimage\0"
#endif

#define SATA_ENV \
    "sata_init=sata init\0" \
    "loadimage=fatload sata 0:1 0x90000000 fitimage\0"

#if !CONFIG_BOOT_NFS
#ifndef CONFIG_BOOT_SATA
#define CFG_EXTRA_ENV_SETTINGS \
    "bootdelay=2\0" \
    "fdt_high=0xffffffffffffffff\0" \
    "initrd_high=0xffffffffffffffff\0" \
    "kernel_addr_r=0x84000000\0" \
    "fdt_addr_r=0x88000000\0" \
    "scriptaddr=0x88100000\0" \
    "pxefile_addr_r=0x88200000\0" \
    "ramdisk_addr_r=0x88300000\0" \
    MMC_ENV \
    "mmcargs=setenv bootargs root=${mmcroot}\0" \
    "addargs=setenv bootargs ${bootargs} video=mxcfb1\0" \
    "mmcboot=echo Booting from mmc ...;" \
            "run mmcargs; " \
            "run addargs; " \
            "run loadimage\0" \
    "bootkernel=bootm 0x90000000 \0"
#else
#define CFG_EXTRA_ENV_SETTINGS \
    "bootdelay=2\0" \
    "fdt_high=0xffffffffffffffff\0" \
    "initrd_high=0xffffffffffffffff\0" \
    "kernel_addr_r=0x84000000\0" \
    "fdt_addr_r=0x88000000\0" \
    "scriptaddr=0x88100000\0" \
    "pxefile_addr_r=0x88200000\0" \
    "ramdisk_addr_r=0x88300000\0" \
    SATA_ENV \
    "sataboot=echo Booting from sata ...;" \
            "run sata_init; " \
            "run loadimage\0" \
    "bootkernel=bootm 0x90000000 \0"
#endif
#else
#define CFG_EXTRA_ENV_SETTINGS \
    "bootdelay=2\0" \
    "fdt_high=0xffffffffffffffff\0" \
    "initrd_high=0xffffffffffffffff\0" \
    "kernel_addr_r=0x84000000\0" \
    "fdt_addr_r=0x88000000\0" \
    "scriptaddr=0x88100000\0" \
    "pxefile_addr_r=0x88200000\0" \
    "ramdisk_addr_r=0x88300000\0" \
    MMC_ENV \
    "serverip="CONFIG_TFTP_SERVERIP"\0" \
    "nfsargs=setenv bootargs root=/dev/nfs init=/linuxrc ip=dhcp nfsroot="CONFIG_ROOTFS_NFS_PATH"/${board_info}/rootfs,proto=tcp,nfsvers=3,nolock\0" \
    "setimageload=setenv loadimage dhcp 0x90000000 ${board_info}/fitImage\0" \
    "nfsboot=echo Set nfs parameter ...;" \
            "run nfsargs;" \
            "run setimageload;" \
            "run loadimage\0" \
    "bootkernel=bootm 0x90000000 \0"
#endif

#ifndef CONFIG_BOOT_SATA
#define deviceboot "run mmcboot;"
#else
#define deviceboot "run sataboot;"
#endif

    //BOOTENV
#undef CONFIG_BOOTCOMMAND
#if !CONFIG_BOOT_NFS
#if (defined CONFIG_SYSTEM_UPDATE_B) || (defined CONFIG_SYSTEM_UPDATE_C)
#define CONFIG_BOOTCOMMAND \
    "bootesw;" \
    "eswupdate;" \
    "bootesw;" \
    deviceboot \
    "run bootkernel;"
#else
#define CONFIG_BOOTCOMMAND \
    "eswupdate;" \
    deviceboot \
    "run bootkernel;"
#endif
#else
#define CONFIG_BOOTCOMMAND \
    "run nfsboot;" \
    "run bootkernel;"
#endif

#undef CONFIG_PREBOOT
#define CONFIG_PREBOOT \
    "setenv fdt_addr ${fdtcontroladdr};" \
    "fdt addr ${fdtcontroladdr};" \
    "usb start;"


#endif /* __CONFIG_H */
