// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef __EIC7700_COMMON_H
#define __EIC7700_COMMON_H

#define HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET	0xf80000
#define HARDWARE_BOARD_INFO_FLASH_BACKUP_OFFSET	(HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET + 1024 * 256)
#define HARDWARE_BOARD_INFO_MAGIC_NUMBER 0x05454943

typedef struct HardwareBoardInfo{
	uint32_t magicNumber;   // Magic “number” 0x05454943
    uint16_t updateCount;   // record update count, if update boardinfo, this count need be added by 1
    uint8_t formatVersionNumber;    // Format version number in unsigned 8-bit integer format, must be 4
	uint8_t pcbRevision;    // PCB revision
	uint8_t bomRevision;    // BOM revision
	uint8_t bomVariant;     // BOM variant
    uint16_t productIdentifier;     // Product identifier, 0x0000: Unknown, 0x0001: EVB A1, 0x0002: EVB A2, 0x0003: HiFive Premier P550, 0x0004: EVB A3?,...
	char boardSerialNumber[22];     // Board serial number(Stored in ASCII) , Vendor(2B)+Board(1B)+ProductNum(4B)+Version(1B)+RamType(1B)+RamSize(2B)+Factory(2B)+Year(2B)+Week(2B)+BoardNum(5B) 
	uint8_t manufacturingTestStatus;// Manufacturing test status, 0:Unknown,1:Pass,2:Fail
    uint8_t Reserved0[1];   // keep all 0xff
    uint8_t ethernetMAC1[6];	// Ethernet MAC address for the first on-board Ethernet, (if applicable in Format-3, else leave blank, i.e., all 0xFF) [Sample] SOC-LOCAL ETH0
	uint8_t ethernetMAC2[6];	// Ethernet MAC address for the second on-board Ethernet [Sample] SOC-LOCAL ETH1
    // If applicable for Device info, else leave blank, i.e., all 0x0. 
    uint8_t DDR[16];        // Device tyep(Stored in ASCII), SN(15B)+CH(1B) [Sample] K3LKCKC0BM-MGCP2 -> SAMSUNG:LP5:16G / CUSTOMER-AUTO____LP5_016
    uint8_t EMMC[16];       // Device info(Stored in ASCII), SN(16B) [Sample] KLMDG4UCTB-B041 -> SAMSUNG:128G
    uint8_t SPI[16];        // Device info(Stored in ASCII), SN(16B) [Sample] W25Q128JWSIQ___ -> HUABANG:16M
    uint8_t ETH_PHY[16];    // Device info(Stored in ASCII), SN(16B)
    uint8_t PMIC[16];       // Device info(Stored in ASCII), SN(16B)
    uint8_t CODEC[16];      // Device info(Stored in ASCII), SN(16B)
    uint8_t DEBUG[16];      // Device info(Stored in ASCII), SN(16B)/0x0 [Sample] FT4232HL-REEL___
    uint8_t MCU[16];        // Device info(Stored in ASCII), SN(16B)/0x0 [Sample] STM32F407VET6___
    uint8_t WIFI[16];       // Device info(Stored in ASCII), SN(16B)/0x0 [Sample] AP12275_M2______
    uint8_t WIFI_MAC[6];    // leave blank, i.e., all 0xFF
    uint8_t BT_MAC[6];      // leave blank, i.e., all 0xFF

    uint8_t Reserved[3888];  // Reserved must be fill all 0x0
    uint32_t crc32Checksum; // hardware info CRC32 checksum (zlib)
    // packet to 4KB align, because the hardware information is basically unchanged, so it is not modified as much as possible

} __attribute__((packed)) HardwareBoardInfo_t;

typedef struct SoftwareBoardInfo {
        uint32_t magicNumber;   // keep with hardware description, 0x05454943
        uint16_t updateCount;    // record update count, if update boardinfo, this count need be added by 1
        uint8_t formatVersionNumber;    // keep with hardware description, 4
        uint8_t Reserved0;      // keep all 0xff
        uint8_t ddrType[8];     // (Stored in ASCII), Type(1B)+size(2B) [Sample] X/5  04/08/16/32/64
        uint8_t distro[16];     // distro version (Stored in ASCII), [Sample] debian-12/ubuntu-20-04/embedlinux/unknown
        uint16_t sysVersion;    // System version (Stored in hex), majorNum(1B)+minor(1B) [Sample] 0x0108
        uint16_t mcuFirmware;   // firmware version (Stored in hex), majorNum(1B)+minor(1B) [Sample] 0x021e
        uint16_t ddrFirmware;   // firmware version (Stored in hex), majorNum(1B)+minor(1B) [Sample] 0x021e
        uint8_t Reserved[470];  // Reserved for later use, must be all 0x0
        uint32_t crc32Checksum; // hardware+software info CRC32 checksum (zlib)
} __attribute__((packed)) SoftwareBoardInfo_t;

#endif