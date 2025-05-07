
#ifndef __FML13V03_COMMON_H
#define __FML13V03_COMMON_H

#define MAX_BIN_LEN 32

#define HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET   (CONFIG_BOARD_INFO_OFFSET)

#define HARDWARE_BOARD_INFO_SIZE	(1024 * 256)
#define HARDWARE_BOARD_INFO_FLASH_BACKUP_OFFSET (HARDWARE_BOARD_INFO_FLASH_MAIN_OFFSET + HARDWARE_BOARD_INFO_SIZE)
#define HARDWARE_BOARD_INFO_MAGIC_NUMBER 0x05454943


/*
 * MAGIC_NUMBER_BYTES: number of bytes used by the magic number
 */
#define MAGIC_NUMBER_BYTES		  4

/*
 * MAC_ADDR_BYTES: number of bytes used by the Ethernet MAC address
 */
#define MAC_ADDR_BYTES			  6

/*
 * MAC_ADDR_STRLEN: length of mac address string
 */
#define MAC_ADDR_STRLEN			 17

/*
 * Atom Types
 * 0x0000 = invalid
 * 0x0001 = vendor info
 * 0x0002 = GPIO map
 * 0x0003 = Linux device tree blob
 * 0x0004 = manufacturer custom data
 * 0x0005-0xfffe = reserved for future use
 * 0xffff = invalid
 */

#define HATS_ATOM_INVALID   0x0000
#define HATS_ATOM_VENDOR	0x0001
#define HATS_ATOM_GPIO	  0x0002
#define HATS_ATOM_DTB	   0x0003
#define HATS_ATOM_CUSTOM	0x0004
#define HATS_ATOM_INVALID_END   0xffff

struct eeprom_hats_header {
	char signature[MAGIC_NUMBER_BYTES]; /* ASCII table signature */
	uint8_t version;	 /* EEPROM data format version */
				/* (0x00 reserved, 0x01 = first version) */
	uint8_t reversed;		/* 0x00, Reserved field */
	uint16_t numatoms;	   /* total atoms in EEPROM */
	uint32_t eeplen;	 /* total length in bytes of all eeprom data */
				/* (including this header) */
};

struct eeprom_hats_atom_header {
	uint16_t type;
	uint16_t count;
	uint32_t dlen;
};

typedef struct FML_HardwareBoardInfo{
	// If applicable for Device info, else leave blank, i.e., all 0x0. 
	//-------------- Head (36 bytes) --------------
	uint32_t magicNumber;   // Magic “number” 0x05454943
	uint16_t updateCount;   // record update count, if update boardinfo, this count need be added by 1
	uint8_t formatVersionNumber;	// Format version number in unsigned 8-bit integer format, must be 4
	uint8_t pcbRevision;	// PCB revision
	uint8_t bomRevision;	// BOM revision
	uint8_t bomVariant;	 // BOM variant
	uint16_t productIdentifier;	 // Product identifier, 0x0000: Unknown, 0x0001: EVB A1, 0x0002: EVB A2, 0x0003: HiFive Premier P550, 0x0004: EVB A3?,...
	char boardSerialNumber[32];	 // Board serial number(Stored in ASCII) , Vendor(2B)+Board(1B)+ProductNum(4B)+Version(1B)+RamType(1B)+RamSize(2B)+Factory(2B)+Year(2B)+Week(2B)+BoardNum(5B) 
	uint8_t manufacturingTestStatus;// Manufacturing test status, 0:Unknown,1:Pass,2:Fail
	uint8_t ReservedH;	  // keep all 0xff
	//-------------- DIE0 (0x600 bytes) --------------
	uint8_t ethernetMAC1[6];	// Ethernet MAC address for the first on-board Ethernet, (if applicable in Format-3, else leave blank, i.e., all 0xFF) [Sample] SOC-LOCAL ETH0
	uint8_t ethernetMAC2[6];	// Ethernet MAC address for the second on-board Ethernet [Sample] SOC-LOCAL ETH1
	uint8_t DDR[16];		// Device tyep(Stored in ASCII), SN(15B)+CH(1B) [Sample] K3LKCKC0BM-MGCP2 -> SAMSUNG:LP5:16G / CUSTOMER-AUTO____LP5_016
	uint8_t EMMC[16];	   // Device info(Stored in ASCII), SN(16B) [Sample] KLMDG4UCTB-B041 -> SAMSUNG:128G
	uint8_t SPI[16];		// Device info(Stored in ASCII), SN(16B) [Sample] W25Q128JWSIQ___ -> HUABANG:16M
	uint8_t ETH_PHY[16];	// Device info(Stored in ASCII), SN(16B)
	uint8_t PMIC[16];	   // Device info(Stored in ASCII), SN(16B)
	uint8_t CODEC[16];	  // Device info(Stored in ASCII), SN(16B)
	uint8_t DEBUG[16];	  // Device info(Stored in ASCII), SN(16B)/0x0 [Sample] FT4232HL-REEL___
	uint8_t MCU[16];		// Device info(Stored in ASCII), SN(16B)/0x0 [Sample] STM32F407VET6___
	uint8_t WIFI[16];	   // Device info(Stored in ASCII), SN(16B)/0x0 [Sample] AP12275_M2______
	uint8_t WIFI_MAC[6];	// leave blank, i.e., all 0xFF
	uint8_t BT_MAC[6];	  // leave blank, i.e., all 0xFF
	uint8_t Reserved[1368];  // Reserved must be fill all 0x0
	//-------------- DIE1 (0x600 bytes) --------------
	uint8_t D1_ethernetMAC1[6];	// Ethernet MAC address for the first on-board Ethernet, (if applicable in Format-3, else leave blank, i.e., all 0xFF) [Sample] SOC-LOCAL ETH0
	uint8_t D1_ethernetMAC2[6];	// Ethernet MAC address for the second on-board Ethernet [Sample] SOC-LOCAL ETH1
	uint8_t D1_DDR[16];		// Device tyep(Stored in ASCII), SN(15B)+CH(1B) [Sample] K3LKCKC0BM-MGCP2 -> SAMSUNG:LP5:16G / CUSTOMER-AUTO____LP5_016
	uint8_t D1_EMMC[16];	   // Device info(Stored in ASCII), SN(16B) [Sample] KLMDG4UCTB-B041 -> SAMSUNG:128G
	uint8_t D1_SPI[16];		// Device info(Stored in ASCII), SN(16B) [Sample] W25Q128JWSIQ___ -> HUABANG:16M
	uint8_t D1_ETH_PHY[16];	// Device info(Stored in ASCII), SN(16B)
	uint8_t D1_PMIC[16];	   // Device info(Stored in ASCII), SN(16B)
	uint8_t D1_CODEC[16];	  // Device info(Stored in ASCII), SN(16B)
	uint8_t D1_DEBUG[16];	  // Device info(Stored in ASCII), SN(16B)/0x0 [Sample] FT4232HL-REEL___
	uint8_t D1_MCU[16];		// Device info(Stored in ASCII), SN(16B)/0x0 [Sample] STM32F407VET6___
	uint8_t D1_WIFI[16];	   // Device info(Stored in ASCII), SN(16B)/0x0 [Sample] AP12275_M2______
	uint8_t D1_WIFI_MAC[6];	// leave blank, i.e., all 0xFF
	uint8_t D1_BT_MAC[6];	  // leave blank, i.e., all 0xFF
	uint8_t D1_Reserved[1368];  // Reserved must be fill all 0x0
	//-------------- Tail --------------
	uint8_t ReservedT[984];  // Reserved must be fill all 0x0
	uint32_t crc32Checksum; // hardware info CRC32 checksum (zlib)
	// packet to 4KB align, because the hardware information is basically unchanged, so it is not modified as much as possible

} __attribute__((packed)) FMLHardwareBoardInfo_t;


#endif