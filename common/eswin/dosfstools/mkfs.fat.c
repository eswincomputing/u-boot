/* mkfs.fat.c - utility to create FAT/MS-DOS filesystems

Copyright (C) 1991 Linus Torvalds <torvalds@klaava.helsinki.fi>
Copyright (C) 1992-1993 Remy Card <card@masi.ibp.fr>
Copyright (C) 1993-1994 David Hudson <dave@humbug.demon.co.uk>
Copyright (C) 1998 H. Peter Anvin <hpa@zytor.com>
Copyright (C) 1998-2005 Roman Hodek <Roman.Hodek@informatik.uni-erlangen.de>
Copyright (C) 2008-2014 Daniel Baumann <mail@daniel-baumann.ch>
Copyright (C) 2015-2016 Andreas Bombe <aeb@debian.org>
Copyright (C) 2018 Pali Rohár <pali.rohar@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.

The complete text of the GNU General Public License
can be found in /usr/share/common-licenses/GPL-3 file.
*/

/* Description: Utility to allow an MS-DOS filesystem to be created
under Linux.  A lot of the basic structure of this program has been
borrowed from Remy Card's "mke2fs" code.

As far as possible the aim here is to make the "mkfs.fat" command
look almost identical to the other Linux filesystem make utilties,
eg bad blocks are still specified as blocks, not sectors, but when
it comes down to it, DOS is tied to the idea of a sector (512 bytes
as a rule), and not the block.  For example the boot block does not
occupy a full cluster.

Fixes/additions May 1998 by Roman Hodek
<Roman.Hodek@informatik.uni-erlangen.de>:
- Atari format support
- New options -A, -S, -C
- Support for filesystems > 2GB
- FAT32 support */

/* Include the header files */

#include "version.h"

#include "fcntl.h"
#include "fs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/time.h>
#include <linux/ctype.h>
#include <time.h>
#include <errno.h>
#include <stdint.h>
#include <getopt.h>
#include <rand.h>

#include "common.h"
#include "msdos_fs.h"
#include "part.h"
#include "update_init.h"
#include <command.h>
#include "./charconv.h"

/* Constant definitions */

#define TRUE 1			/* Boolean constants */
#define FALSE 0

#define BLOCK_SIZE_BITS 10
#define BLOCK_SIZE (1<<BLOCK_SIZE_BITS)
#define HARD_SECTOR_SIZE   512
#define SECTORS_PER_BLOCK ( BLOCK_SIZE / HARD_SECTOR_SIZE )

#define NO_NAME "NO NAME    "


/* Limits of integral types. */
#ifndef INT8_MIN
#define INT8_MIN               (-128)
#endif
#ifndef INT16_MIN
#define INT16_MIN              (-32767-1)
#endif
#ifndef INT32_MIN
#define INT32_MIN              (-2147483647-1)
#endif
#ifndef INT8_MAX
#define INT8_MAX               (127)
#endif
#ifndef INT16_MAX
#define INT16_MAX              (32767)
#endif
#ifndef INT32_MAX
#define INT32_MAX              (2147483647)
#endif
#ifndef UINT8_MAX
#define UINT8_MAX              (255U)
#endif
#ifndef UINT16_MAX
#define UINT16_MAX             (65535U)
#endif
#ifndef UINT32_MAX
#define UINT32_MAX             (4294967295U)
#endif


uint32_t generate_volume_id(void)
{
    /* rand() returns int from [0,RAND_MAX], therefore only 31 bits */
    return (((uint32_t)(rand() & 0xFFFF)) << 16) | ((uint32_t)(rand() & 0xFFFF));
}

/*
 * Validate volume label
 *
 * @param[in]   doslabel   Label stored according to current DOS codepage
 *
 * @return   bitmask of errors
 *           0x01 - lowercase character
 *           0x02 - character below 0x20
 *           0x04 - character in disallowed set
 *           0x08 - empty or space-only label
 *           0x10 - space at beginning
 */
int validate_volume_label(char *doslabel)
{
    int i;
    int ret = 0;
    wchar_t wlabel[12];

    if (dos_string_to_wchar_string(wlabel, doslabel, sizeof(wlabel))) {
        for (i = 0; wlabel[i]; i++) {
            /* FAT specification: Lower case characters are not allowed in DIR_Name
                                  (what these characters are is country specific)
               Original label is stored in DOS OEM code page, so islower() function
               cannot be used. Therefore convert original label to locale independent
               wchar_t* and then use iswlower() function for it.
            */
            if (islower(wlabel[i])) {
                ret |= 0x01;
                break;
            }
        }
    }

    /* According to FAT specification those bytes (after conversion to DOS OEM
       code page) are not allowed.
     */
    for (i = 0; i < 11; i++) {
        if (doslabel[i] < 0x20)
            ret |= 0x02;
        if (doslabel[i] == 0x22 ||
            (doslabel[i] >= 0x2A && doslabel[i] <= 0x2C) ||
            doslabel[i] == 0x2E ||
            doslabel[i] == 0x2F ||
            (doslabel[i] >= 0x3A && doslabel[i] <= 0x3F) ||
            (doslabel[i] >= 0x5B && doslabel[i] <= 0x5D) ||
            doslabel[i] == 0x7C)
            ret |= 0x04;
    }

    if (memcmp(doslabel, "           ", 11) == 0)
        ret |= 0x08;

    if (doslabel[0] == ' ')
        ret |= 0x10;

    return ret;
}
/* Macro definitions */

/* Compute ceil(a/b) */

static inline int cdiv(int a, int b)
{
return (a + b - 1) / b;
}

/* FAT values */
#define FAT_EOF      (0x0ffffff8)
#define FAT_BAD      0x0ffffff7

#define MSDOS_EXT_SIGN 0x29	/* extended boot sector signature */
#define MSDOS_FAT12_SIGN "FAT12   "	/* FAT12 filesystem signature */
#define MSDOS_FAT16_SIGN "FAT16   "	/* FAT16 filesystem signature */
#define MSDOS_FAT32_SIGN "FAT32   "	/* FAT32 filesystem signature */

#define BOOT_SIGN 0xAA55	/* Boot sector magic number */

/* According to Microsoft FAT specification (fatgen103.doc) disk with
* 4085 clusters (or more) is FAT16, but Microsoft Windows FAT driver
* fastfat.sys detects disk with less then 4087 clusters as FAT12.
* Linux FAT drivers msdos.ko and vfat.ko detect disk with at least
* 4085 clusters as FAT16, therefore for compatibility reasons with
* both systems disallow formatting disks to 4085 or 4086 clusters. */
#define MAX_CLUST_12	4084
#define MIN_CLUST_16	4087

/* According to Microsoft FAT specification (fatgen103.doc) disk with
* 65525 clusters (or more) is FAT32, but Microsoft Windows FAT driver
* fastfat.sys, Linux FAT drivers msdos.ko and vfat.ko detect disk as
* FAT32 when Sectors Per FAT (fat_length) is set to zero. And not by
* number of clusters. Still there is cluster upper limit for FAT16. */
#define MAX_CLUST_16	65524
#define MIN_CLUST_32	65525

/* M$ says the high 4 bits of a FAT32 FAT entry are reserved and don't belong
* to the cluster number. So the max. cluster# is based on 2^28 */
#define MAX_CLUST_32	268435446

#define OLDGEMDOS_MAX_SECTORS	32765
#define GEMDOS_MAX_SECTORS	65531
#define GEMDOS_MAX_SECTOR_SIZE	(16*1024)

#define BOOTCODE_SIZE		448
#define BOOTCODE_FAT32_SIZE	420

/* __attribute__ ((packed)) is used on all structures to make gcc ignore any
* alignments */

struct msdos_volume_info {
	uint8_t drive_number;	/* BIOS drive number */
	uint8_t boot_flags;		/* bit 0: dirty, bit 1: need surface test */
	uint8_t ext_boot_sign;	/* 0x29 if fields below exist (DOS 3.3+) */
	uint8_t volume_id[4];	/* Volume ID number */
	uint8_t volume_label[11];	/* Volume label */
	uint8_t fs_type[8];		/* Typically FAT12 or FAT16 */
} __attribute__ ((packed));

struct msdos_boot_sector {
	uint8_t boot_jump[3];	/* Boot strap short or near jump */
	uint8_t system_id[8];	/* Name - can be used to special case
					partition manager volumes */
	uint8_t sector_size[2];	/* bytes per logical sector */
	uint8_t cluster_size;	/* sectors/cluster */
	uint16_t reserved;		/* reserved sectors */
	uint8_t fats;		/* number of FATs */
	uint8_t dir_entries[2];	/* root directory entries */
	uint8_t sectors[2];		/* number of sectors */
	uint8_t media;		/* media code (unused) */
	uint16_t fat_length;	/* sectors/FAT */
	uint16_t secs_track;	/* sectors per track */
	uint16_t heads;		/* number of heads */
	uint32_t hidden;		/* hidden sectors (unused) */
	uint32_t total_sect;	/* number of sectors (if sectors == 0) */
	union {
		struct {
			struct msdos_volume_info vi;
			uint8_t boot_code[BOOTCODE_SIZE];
		} __attribute__ ((packed)) _oldfat;
		struct {
			uint32_t fat32_length;	/* sectors/FAT */
			uint16_t flags;		/* bit 8: fat mirroring, low 4: active fat */
			uint8_t version[2];		/* major, minor filesystem version */
			uint32_t root_cluster;	/* first cluster in root directory */
			uint16_t info_sector;	/* filesystem info sector */
			uint16_t backup_boot;	/* backup boot sector */
			uint16_t reserved2[6];	/* Unused */
			struct msdos_volume_info vi;
			uint8_t boot_code[BOOTCODE_FAT32_SIZE];
		} __attribute__ ((packed)) _fat32;
	} __attribute__ ((packed)) fstype;
	uint16_t boot_sign;
} __attribute__ ((packed));
#define fat32	fstype._fat32
#define oldfat	fstype._oldfat

struct fat32_fsinfo {
	uint32_t reserved1;		/* Nothing as far as I can tell */
	uint32_t signature;		/* 0x61417272L */
	uint32_t free_clusters;	/* Free cluster count.  -1 if unknown */
	uint32_t next_cluster;	/* Most recently allocated cluster.
				* Unused under Linux. */
	uint32_t reserved2[4];
} __attribute__ ((packed));

/* The "boot code" we put into the filesystem... it writes a message and
tells the user to try again. This "boot code" is in the public domain. */

unsigned char dummy_boot_jump[3] = { 0xeb, 0x3c, 0x90 };

unsigned char dummy_boot_jump_m68k[2] = { 0x60, 0x1c };

#define MSG_OFFSET_OFFSET 3
char dummy_boot_code[BOOTCODE_SIZE] = "\x0e"	/* push cs */
	"\x1f"			/* pop ds */
	"\xbe\x5b\x7c"		/* mov si, offset message_txt */
	/* write_msg: */
	"\xac"			/* lodsb */
	"\x22\xc0"			/* and al, al */
	"\x74\x0b"			/* jz key_press */
	"\x56"			/* push si */
	"\xb4\x0e"			/* mov ah, 0eh */
	"\xbb\x07\x00"		/* mov bx, 0007h */
	"\xcd\x10"			/* int 10h */
	"\x5e"			/* pop si */
	"\xeb\xf0"			/* jmp write_msg */
	/* key_press: */
	"\x32\xe4"			/* xor ah, ah */
	"\xcd\x16"			/* int 16h */
	"\xcd\x19"			/* int 19h */
	"\xeb\xfe"			/* foo: jmp foo */
	/* message_txt: */
	"This is not a bootable disk.  Please insert a bootable floppy and\r\n"
	"press any key to try again ... \r\n";

#define MESSAGE_OFFSET 29	/* Offset of message in above code */

static char initial_volume_name[] = NO_NAME; /* Initial volume name, make sure that is writable */

/* Global variables - the root of all evil :-) - see these and weep! */

static char *device_name = NULL;	/* Name of the device on which to create the filesystem */
static int verbose = 0;		/* Default to verbose mode off */
static long volume_id;		/* Volume ID number */
static char *volume_name = initial_volume_name;	/* Volume name */
static unsigned long long blocks;	/* Number of blocks in filesystem */
static unsigned sector_size = 512;	/* Size of a logical sector */
static int sector_size_set = 0;	/* User selected sector size */
static int backup_boot = 0;	/* Sector# of backup boot sector */
static int backup_boot_set = 0;	/* User selected backup boot sector */
static int info_sector = 0;	/* Sector# of FAT32 info sector */
static int reserved_sectors = 0;	/* Number of reserved sectors */
static int nr_fats = 2;		/* Default number of FATs to produce */
static int size_fat = 0;	/* Size in bits of FAT entries */
static int size_fat_by_user = 0;	/* 1 if FAT size user selected */
static off_t part_sector = 0; /* partition offset in sector */
static struct msdos_boot_sector bs;	/* Boot sector data */
static int start_data_sector;	/* Sector number for the start of the data area */
static int start_data_block;	/* Block number for the start of the data area */
static unsigned char *fat;	/* File allocation table */
static unsigned alloced_fat_length;	/* # of FAT sectors we can keep in memory */
static unsigned fat_entries;		/* total entries in FAT table (including reserved) */
static unsigned char *info_sector_buffer;	/* FAT32 info sector */
static struct msdos_dir_entry *root_dir;	/* Root directory */
static int size_root_dir;	/* Size of the root directory in bytes */
static uint32_t num_sectors;		/* Total number of sectors in device */
static int sectors_per_cluster = 0;	/* Number of sectors per disk cluster */
static int root_dir_entries = 0;	/* Number of root directory entries */
static int root_dir_entries_set = 0;	/* User selected root directory size */
static char *blank_sector;	/* Blank sector - all zeros */
static unsigned hidden_sectors = 0;	/* Number of hidden sectors */
static int drive_number_option = 0;	/* drive number */
static int drive_number_by_user = 0;	/* drive number option invoked */
static int fat_media_byte = 0;	/* media byte in header and starting FAT */
static int malloc_entire_fat = FALSE;	/* Whether we should malloc() the entire FAT or not */
static int align_structures = TRUE;	/* Whether to enforce alignment */
static int orphaned_sectors = 0;	/* Sectors that exist in the last block of filesystem */
static int invariant = 0;		/* Whether to set normally randomized or
					current time based values to
					constants */
static int fill_mbr_partition = 1;	/* Whether to fill MBR partition table or not */

static struct blk_desc *mmc_dev_desc;
static struct disk_partition mmc_part_info;

/* Function prototype definitions */

static void mark_FAT_cluster(int cluster, unsigned int value);
static void establish_params(struct disk_partition *info);
static int setup_tables(void);
static void write_tables(void);

/* The function implementations */

/* Mark the specified cluster as having a particular value */

static void mark_FAT_cluster(int cluster, unsigned int value)
{

	if (cluster < 0 || cluster >= fat_entries)
	printf("Internal error: out of range cluster number in mark_FAT_cluster");

	switch (size_fat) {
	case 12:
	value &= 0x0fff;
	if (((cluster * 3) & 0x1) == 0) {
		fat[3 * cluster / 2] = (unsigned char)(value & 0x00ff);
		fat[(3 * cluster / 2) + 1] =
		(unsigned char)((fat[(3 * cluster / 2) + 1] & 0x00f0)
				| ((value & 0x0f00) >> 8));
	} else {
		fat[3 * cluster / 2] =
		(unsigned char)((fat[3 * cluster / 2] & 0x000f) |
				((value & 0x000f) << 4));
		fat[(3 * cluster / 2) + 1] = (unsigned char)((value & 0x0ff0) >> 4);
	}
	break;

	case 16:
	value &= 0xffff;
	fat[2 * cluster] = (unsigned char)(value & 0x00ff);
	fat[(2 * cluster) + 1] = (unsigned char)(value >> 8);
	break;

	case 32:
	value &= 0xfffffff;
	fat[4 * cluster] = (unsigned char)(value & 0x000000ff);
	fat[(4 * cluster) + 1] = (unsigned char)((value & 0x0000ff00) >> 8);
	fat[(4 * cluster) + 2] = (unsigned char)((value & 0x00ff0000) >> 16);
	fat[(4 * cluster) + 3] = (unsigned char)((value & 0xff000000) >> 24);
	break;

	default:
	printf("Bad FAT size (not 12, 16, or 32)");
	}
}

/* Establish the geometry and media parameters for the device */
static void establish_params(struct disk_partition *info)
{
	unsigned int sec_per_track;
	unsigned int heads;
	unsigned int media = 0xf8;
	unsigned int cluster_size = 4;  /* starting point for FAT12 and FAT16 */
	int def_root_dir_entries = 512;
	unsigned long long total_size = 0;

	unsigned long long int total_sectors;

	total_sectors = info->size;
	total_size = info->size * info->blksz;
	// printf("total_sectors %d(0x%x), total_size %d(0x%x)", total_sectors, total_sectors, total_size, total_size);
	if (total_sectors <= 524288) {
		/* For capacity below the 256MB (with 512b sectors) use CHS Recommendation from SD Card Part 2 File System Specification */
		heads = total_sectors <=  32768 ? 2 :
				total_sectors <=  65536 ? 4 :
				total_sectors <= 262144 ? 8 : 16;
		sec_per_track = total_sectors <= 4096 ? 16 : 32;
	} else {
		/* Use LBA-Assist Translation for calculating CHS when disk geometry is not available */
		heads = total_sectors <=  16*63*1024 ? 16 :
				total_sectors <=  32*63*1024 ? 32 :
				total_sectors <=  64*63*1024 ? 64 :
				total_sectors <= 128*63*1024 ? 128 : 255;
		sec_per_track = 63;
	}

	// if (info->type != TYPE_FIXED) {
		/* enter default parameters for floppy disks if the size matches */
		switch (total_size / 1024) {
			case 360:
			sec_per_track = 9;
			heads = 2;
			media = 0xfd;
			cluster_size = 2;
			def_root_dir_entries = 112;
			break;

			case 720:
			sec_per_track = 9;
			heads = 2;
			media = 0xf9;
			cluster_size = 2;
			def_root_dir_entries = 112;
			break;

			case 1200:
			sec_per_track = 15;
			heads = 2;
			media = 0xf9;
			cluster_size = 1;
			def_root_dir_entries = 224;
			break;

			case 1440:
			sec_per_track = 18;
			heads = 2;
			media = 0xf0;
			cluster_size = 1;
			def_root_dir_entries = 224;
			break;

			case 2880:
			sec_per_track = 36;
			heads = 2;
			media = 0xf0;
			cluster_size = 2;
			def_root_dir_entries = 224;
			break;
		}
	// }

	if (!size_fat && total_size >= 512 * 1024 * 1024) {
		if (verbose)
			printf("Auto-selecting FAT32 for large filesystem\n");
		size_fat = 32;
	}
	if (size_fat == 32) {
	/*
		* For FAT32, try to do the same as M$'s format command
		* (see http://www.win.tue.nl/~aeb/linux/fs/fat/fatgen103.pdf p. 20):
		* fs size <= 260M: 0.5k clusters
		* fs size <=   8G:   4k clusters
		* fs size <=  16G:   8k clusters
		* fs size <=  32G:  16k clusters
		* fs size >   32G:  32k clusters
		*/
		unsigned long long int sectors = total_size / sector_size;
			cluster_size = sectors > 32*1024*1024*2 ? 64 :
							sectors > 16*1024*1024*2 ? 32 :
							sectors >  8*1024*1024*2 ? 16 :
							sectors >     260*1024*2 ? 8 : 1;
	}

	// if (!hidden_sectors_by_user && info->geom_start >= 0 && info->geom_start + part_sector <= UINT32_MAX)
	// 	hidden_sectors = info->geom_start + part_sector;

	if (!root_dir_entries)
	root_dir_entries = def_root_dir_entries;

	if (!bs.secs_track)
		bs.secs_track = sec_per_track;
	if (!bs.heads)
		bs.heads = heads;
	bs.media = media;
	bs.cluster_size = cluster_size;
}

/*
* If alignment is enabled, round the first argument up to the second; the
* latter must be a power of two.
*/
static unsigned int align_object(unsigned int sectors, unsigned int clustsize)
{
	if (align_structures)
	return (sectors + clustsize - 1) & ~(clustsize - 1);
	else
	return sectors;
}

/* Create the filesystem data tables */

static int setup_tables(void)
{
	unsigned cluster_count = 0, fat_length;
	struct msdos_volume_info *vi =
	(size_fat == 32 ? &bs.fat32.vi : &bs.oldfat.vi);
	char label[12] = { 0 };
	size_t len;
	int ret;
	int i;

	memcpy((char *)bs.system_id, "mkfs.fat", strlen("mkfs.fat"));
	if (sectors_per_cluster)
		bs.cluster_size = (char)sectors_per_cluster;

	if (fat_media_byte)
		bs.media = (char) fat_media_byte;

	if (bs.media == 0xf8)
		vi->drive_number=0x80;
	else
		vi->drive_number=0x00;

	if (drive_number_by_user)
		vi->drive_number= (char) drive_number_option;

	if (size_fat == 32) {
	/* Under FAT32, the root dir is in a cluster chain, and this is
		* signalled by bs.dir_entries being 0. */
		if (root_dir_entries_set)
			printf("Warning: root directory entries specified with -r have no effect on FAT32\n");
		root_dir_entries = 0;
	}


	vi->volume_id[0] = (unsigned char)(volume_id & 0x000000ff);
	vi->volume_id[1] = (unsigned char)((volume_id & 0x0000ff00) >> 8);
	vi->volume_id[2] = (unsigned char)((volume_id & 0x00ff0000) >> 16);
	vi->volume_id[3] = (unsigned char)(volume_id >> 24);

	len = sizeof(volume_name);
	if (len != (size_t)-1 && len > 11)
		printf("Label can be no longer than 11 characters");
	memcpy(label, volume_name, 12);
	for(i = 0; i < len; i++)
	{
		label[i] = toupper(label[i]);
	}

	for (i = strlen(label); i < 11; ++i)
		label[i] = ' ';
	label[11] = 0;

	if (memcmp(label, "           ", MSDOS_NAME) == 0)
		memcpy(label, NO_NAME, MSDOS_NAME);

	ret = validate_volume_label(label);
	if (ret & 0x1)
		fprintf(stderr,
			"mkfs.fat: Warning: lowercase labels might not work properly on some systems\n");
	if (ret & 0x2) {
		printf("Labels with characters below 0x20 are not allowed\n");
		return -1;
	}
	if (ret & 0x4) {
		printf("Labels with characters *?.,;:/\\|+=<>[]\" are not allowed\n");
		return -1;
	}
	if (ret & 0x10) {
		printf("Label can't start with a space character");
		return -1;
	}

	memcpy(vi->volume_label, label, 11);

	memcpy(bs.boot_jump, dummy_boot_jump, 3);
	/* Patch in the correct offset to the boot code */
	bs.boot_jump[1] = ((size_fat == 32 ?
				(char *)&bs.fat32.boot_code :
				(char *)&bs.oldfat.boot_code) - (char *)&bs) - 2;

	if (size_fat == 32) {
		int offset = (char *)&bs.fat32.boot_code -
		(char *)&bs + MESSAGE_OFFSET + 0x7c00;
		if (dummy_boot_code[BOOTCODE_FAT32_SIZE - 1])
		printf("Warning: message too long; truncated\n");
		dummy_boot_code[BOOTCODE_FAT32_SIZE - 1] = 0;
		memcpy(bs.fat32.boot_code, dummy_boot_code, BOOTCODE_FAT32_SIZE);
		bs.fat32.boot_code[MSG_OFFSET_OFFSET] = offset & 0xff;
		bs.fat32.boot_code[MSG_OFFSET_OFFSET + 1] = offset >> 8;
	} else {
		memcpy(bs.oldfat.boot_code, dummy_boot_code, BOOTCODE_SIZE);
	}
	
	bs.boot_sign = BOOT_SIGN;

	if (verbose >= 2)
		printf("Boot jump code is %02x %02x\n",
				bs.boot_jump[0], bs.boot_jump[1]);

	if (!reserved_sectors) {
		reserved_sectors = (size_fat == 32) ? 32 : 1;
	} else {
	if (size_fat == 32 && reserved_sectors < 2)
		printf("On FAT32 at least 2 reserved sectors are needed.");
	}
	bs.reserved = reserved_sectors;
	if (verbose >= 2) {
		printf("Using %d reserved sectors\n", reserved_sectors);
	}
	bs.fats = (char)nr_fats;
	if (size_fat == 32) {
		bs.hidden = hidden_sectors;
	} else {
		/* In Atari format, hidden is a 16 bit field */
		uint16_t hidden = hidden_sectors;
		if (hidden_sectors & ~0xffff) {
			printf("#hidden doesn't fit in 16bit field of Atari format\n");
		}
		memcpy(&bs.hidden, &hidden, 2);
	}

	if ((long long)(blocks * BLOCK_SIZE / sector_size) + orphaned_sectors >
		UINT32_MAX) {
		printf("Warning: target too large, space at end will be left unused\n");
		num_sectors = UINT32_MAX;
		blocks = (unsigned long long)UINT32_MAX * sector_size / BLOCK_SIZE;
	} else {
		num_sectors =
			(long long)(blocks * BLOCK_SIZE / sector_size) + orphaned_sectors;
	}

	if (align_structures) {
		/* Align number of sectors to be multiple of sectors per track, needed by DOS and mtools */
		num_sectors = num_sectors / bs.secs_track * bs.secs_track;
	}

	unsigned fatdata1216;	/* Sectors for FATs + data area (FAT12/16) */
	unsigned fatdata32;	/* Sectors for FATs + data area (FAT32) */
	unsigned fatlength12, fatlength16, fatlength32;
	unsigned maxclust12, maxclust16, maxclust32;
	unsigned clust12, clust16, clust32;
	int maxclustsize;
	unsigned root_dir_sectors = cdiv(root_dir_entries * 32, sector_size);

	/*
		* If the filesystem is 8192 sectors or less (4 MB with 512-byte
		* sectors, i.e. floppy size), don't align the data structures.
		*/
	if (num_sectors <= 8192) {
		if (align_structures && verbose >= 2)
		printf("Disabling alignment due to tiny filesystem\n");

		align_structures = FALSE;
	}

	if (sectors_per_cluster) {
		bs.cluster_size = maxclustsize = sectors_per_cluster;
	} else {
		/* An initial guess for bs.cluster_size should already be set */
		maxclustsize = 128;
	}

	do {
		fatdata32 = num_sectors
					- align_object(reserved_sectors, bs.cluster_size);
		fatdata1216 = fatdata32
					- align_object(root_dir_sectors, bs.cluster_size);

		if (verbose >= 2) {
			printf("Trying with %d sectors/cluster:\n", bs.cluster_size);
		}

		/* The factor 2 below avoids cut-off errors for nr_fats == 1.
			* The "nr_fats*3" is for the reserved first two FAT entries */
		clust12 = 2 * ((long long)fatdata1216 * sector_size + nr_fats * 3) /
					(2 * (int)bs.cluster_size * sector_size + nr_fats * 3);
		fatlength12 = cdiv(((clust12 + 2) * 3 + 1) >> 1, sector_size);
		fatlength12 = align_object(fatlength12, bs.cluster_size);
		/* Need to recalculate number of clusters, since the unused parts of the
			* FATS and data area together could make up space for an additional,
			* not really present cluster. */
		clust12 = (fatdata1216 - nr_fats * fatlength12) / bs.cluster_size;
		maxclust12 = (fatlength12 * 2 * sector_size) / 3;
		if (maxclust12 > MAX_CLUST_12) {
			maxclust12 = MAX_CLUST_12;
		}
		if (verbose >= 2 && (size_fat == 0 || size_fat == 12)) {
			printf("Trying FAT12: #clu=%u, fatlen=%u, maxclu=%u, limit=%u\n",
					clust12, fatlength12, maxclust12, MAX_CLUST_12);
		}
		if (clust12 > maxclust12) {
			clust12 = 0;
			if (verbose >= 2 && (size_fat == 0 || size_fat == 12)) {
				printf("Trying FAT12: too much clusters\n");
			}
		}

		clust16 = ((long long)fatdata1216 * sector_size + nr_fats * 4) /
					((int)bs.cluster_size * sector_size + nr_fats * 2);
		fatlength16 = cdiv((clust16 + 2) * 2, sector_size);
		fatlength16 = align_object(fatlength16, bs.cluster_size);
		/* Need to recalculate number of clusters, since the unused parts of the
			* FATS and data area together could make up space for an additional,
			* not really present cluster. */
		clust16 = (fatdata1216 - nr_fats * fatlength16) / bs.cluster_size;
		maxclust16 = (fatlength16 * sector_size) / 2;
		if (maxclust16 > MAX_CLUST_16) {
			maxclust16 = MAX_CLUST_16;
		}
		if (verbose >= 2 && (size_fat == 0 || size_fat == 16)) {
			printf("Trying FAT16: #clu=%u, fatlen=%u, maxclu=%u, limit=%u/%u\n",
					clust16, fatlength16, maxclust16, MIN_CLUST_16, MAX_CLUST_16);
		}
		if (clust16 > maxclust16) {
			if (verbose >= 2 && (size_fat == 0 || size_fat == 16)) {
				printf("Trying FAT16: too much clusters\n");
			}
			clust16 = 0;
		}
		/* This avoids that the filesystem will be misdetected as having a
			* 12 bit FAT. */
		if (clust16 && clust16 < MIN_CLUST_16) {
			if (verbose >= 2 && (size_fat == 0 || size_fat == 16)) {
				printf("Trying FAT16: not enough clusters, would be misdetected as FAT12\n");
			}
			clust16 = 0;
		}

		clust32 = ((long long)fatdata32 * sector_size + nr_fats * 8) /
					((int)bs.cluster_size * sector_size + nr_fats * 4);
		fatlength32 = cdiv((clust32 + 2) * 4, sector_size);
		fatlength32 = align_object(fatlength32, bs.cluster_size);
		/* Need to recalculate number of clusters, since the unused parts of the
			* FATS and data area together could make up space for an additional,
			* not really present cluster. */
		clust32 = (fatdata32 - nr_fats * fatlength32) / bs.cluster_size;
		maxclust32 = (fatlength32 * sector_size) / 4;
		if (maxclust32 > MAX_CLUST_32) {
			maxclust32 = MAX_CLUST_32;
		}
		if (verbose >= 2 && (size_fat == 0 || size_fat == 32)) {
			printf("Trying FAT32: #clu=%u, fatlen=%u, maxclu=%u, limit=%u/%u\n",
					clust32, fatlength32, maxclust32, MIN_CLUST_32, MAX_CLUST_32);
		}
		if (clust32 > maxclust32) {
			if (verbose >= 2 && (size_fat == 0 || size_fat == 32)) {
				printf("Trying FAT32: too much clusters\n");
			}
			clust32 = 0;
		}
		/* When explicitely asked, allow to create FAT32 with less then MIN_CLUST_32 */
		if (clust32 && clust32 < MIN_CLUST_32
			&& !(size_fat_by_user && size_fat == 32)) {
			if (verbose >= 2 && (size_fat == 0 || size_fat == 32)) {
				printf("Trying FAT32: not enough clusters\n");
			}
			clust32 = 0;
		}

		if ((clust12 && (size_fat == 0 || size_fat == 12)) ||
		(clust16 && (size_fat == 0 || size_fat == 16)) ||
		(clust32 && size_fat == 32))
			break;

		bs.cluster_size <<= 1;
	} while (bs.cluster_size && bs.cluster_size <= maxclustsize);

	/* Use the optimal FAT size if not specified;
		* establish_params() will have already set size_fat to 32 if it is not
		* specified and the filesystem size is over a specific threshold */
	if (!size_fat) {
		size_fat = (clust16 > clust12) ? 16 : 12;
		if (verbose >= 2)
			printf("Choosing %d bits for FAT\n", size_fat);
	}

	switch (size_fat) {
	case 12:
		cluster_count = clust12;
		fat_length = fatlength12;
		bs.fat_length = fatlength12;
		memcpy(vi->fs_type, MSDOS_FAT12_SIGN, 8);
		break;

	case 16:
		cluster_count = clust16;
		fat_length = fatlength16;
		bs.fat_length = fatlength16;
		memcpy(vi->fs_type, MSDOS_FAT16_SIGN, 8);
		break;

	case 32:
		if (clust32 < MIN_CLUST_32)
		printf("WARNING: Number of clusters for 32 bit FAT is less then suggested minimum.\n");
		cluster_count = clust32;
		fat_length = fatlength32;
		bs.fat_length = 0;
		bs.fat32.fat32_length = fatlength32;
		memcpy(vi->fs_type, MSDOS_FAT32_SIGN, 8);
		root_dir_entries = 0;
		break;

	default:
		printf("FAT not 12, 16 or 32 bits");
	}

	/* Adjust the reserved number of sectors for alignment */
	reserved_sectors = align_object(reserved_sectors, bs.cluster_size);
	bs.reserved = reserved_sectors;

	/* Adjust the number of root directory entries to help enforce alignment */
	if (align_structures) {
		root_dir_entries = align_object(root_dir_sectors, bs.cluster_size)
		* (sector_size >> 5);
	}


	if (fill_mbr_partition) {
		uint8_t *partition;
		uint8_t *disk_sig_ptr;
		uint32_t disk_sig;

		if (verbose)
			printf("Adding MBR table\n");

		if (size_fat == 32)
			disk_sig_ptr = bs.fat32.boot_code + BOOTCODE_FAT32_SIZE - 16*4 - 6;
		else
			disk_sig_ptr = bs.oldfat.boot_code + BOOTCODE_SIZE - 16*4 - 6;

		if (*(disk_sig_ptr-1)) {
			printf("Warning: message too long; truncated\n");
			*(disk_sig_ptr-1) = 0;
		}

		disk_sig = 0;
		memset(disk_sig_ptr, 0, 16*4 + 6);

		/* If is not available then generate random 32 bit disk signature */
		if (invariant)
			disk_sig = volume_id;
		else if (!disk_sig)
			disk_sig = generate_volume_id();

		disk_sig_ptr[0] = (disk_sig >>  0) & 0xFF;
		disk_sig_ptr[1] = (disk_sig >>  8) & 0xFF;
		disk_sig_ptr[2] = (disk_sig >> 16) & 0xFF;
		disk_sig_ptr[3] = (disk_sig >> 24) & 0xFF;

		partition = disk_sig_ptr + 6;

		/* Active flag */
		partition[0] = 0x80;

		/* CHS address of the first sector */
		partition[1] = 0;
		partition[2] = 1;
		partition[3] = 0;

		/* Partition type */
		if (bs.heads > 254 || bs.secs_track > 63) { /* CHS values are out of range for MBR, use LBA */
			if (size_fat != 32)
				partition[4] = 0x0E; /* BIG FAT16 (LBA) */
			else
				partition[4] = 0x0C; /* FAT32 (LBA) */
		} else if (size_fat == 12 && num_sectors < 65536)
			partition[4] = 0x01; /* FAT12 (CHS) */
		else if (size_fat == 16 && num_sectors < 65536)
			partition[4] = 0x04; /* FAT16 (CHS) */
		else if (size_fat != 32 && num_sectors < bs.secs_track * bs.heads * 1024)
			partition[4] = 0x06; /* BIG FAT16 or FAT12 (CHS) */
		else if (size_fat != 32)
			partition[4] = 0x0E; /* BIG FAT16 (LBA) */
		else
			partition[4] = 0x0C; /* FAT32 (LBA) */

		/* CHS address of the last sector */
		if (bs.heads > 254 || bs.secs_track > 63 || num_sectors >= bs.secs_track * bs.heads * 1024) {
			/* If CHS address is too large use tuple (1023, 254, 63) */
			partition[5] = 254;
			partition[6] = 255;
			partition[7] = 255;
		} else {
			partition[5] = (num_sectors / bs.secs_track) % bs.heads;
			partition[6] = ((1 + num_sectors % bs.secs_track) & 63) | (((num_sectors / (bs.heads * bs.secs_track)) >> 8) * 64);
			partition[7] = (num_sectors / (bs.heads * bs.secs_track)) & 255;
		}

		/* LBA of the first sector */
		partition[ 8] = 0;
		partition[ 9] = 0;
		partition[10] = 0;
		partition[11] = 0;

		/* Number of sectors */
		partition[12] = (num_sectors >>  0) & 0xFF;
		partition[13] = (num_sectors >>  8) & 0xFF;
		partition[14] = (num_sectors >> 16) & 0xFF;
		partition[15] = (num_sectors >> 24) & 0xFF;
	}

	bs.sector_size[0] = (char)(sector_size & 0x00ff);
	bs.sector_size[1] = (char)((sector_size & 0xff00) >> 8);

	bs.dir_entries[0] = (char)(root_dir_entries & 0x00ff);
	bs.dir_entries[1] = (char)((root_dir_entries & 0xff00) >> 8);

	if (size_fat == 32) {
		/* set up additional FAT32 fields */
		bs.fat32.flags = 0;
		bs.fat32.version[0] = 0;
		bs.fat32.version[1] = 0;
		bs.fat32.root_cluster = 2;
		if (!info_sector)
			info_sector = 1;
		bs.fat32.info_sector = info_sector;
		if (!backup_boot_set)
			backup_boot = (reserved_sectors >= 7 && info_sector != 6) ? 6 :
				(reserved_sectors >= 3 + info_sector &&
				info_sector != reserved_sectors - 2 &&
				info_sector != reserved_sectors - 1) ? reserved_sectors - 2 :
				(reserved_sectors >= 3 && info_sector != reserved_sectors - 1) ?
				reserved_sectors - 1 : 0;
		if (backup_boot) {
			if (backup_boot == info_sector)
				printf("Backup boot sector must not be same as info sector (%d)", info_sector);
			else if (backup_boot >= reserved_sectors)
				printf("Backup boot sector must be a reserved sector");
		}
		if (verbose >= 2)
			printf("Using sector %d as backup boot sector (0 = none)\n",
				backup_boot);
		bs.fat32.backup_boot = backup_boot;
		memset(&bs.fat32.reserved2, 0, sizeof(bs.fat32.reserved2));
	}

	if (num_sectors >= 65536) {
		bs.sectors[0] = (char)0;
		bs.sectors[1] = (char)0;
		bs.total_sect = num_sectors;
	} else {
		bs.sectors[0] = (char)(num_sectors & 0x00ff);
		bs.sectors[1] = (char)((num_sectors & 0xff00) >> 8);
		bs.total_sect = 0;
	}

	vi->ext_boot_sign = MSDOS_EXT_SIGN;

	if (!cluster_count) {
		if (sectors_per_cluster)	/* If yes, printf if we'd spec'd sectors per cluster */
			printf("Not enough or too many clusters for filesystem - try less or more sectors per cluster");
		else 
			printf("Attempting to create a too small or a too large filesystem\n");
		return -1;
	}
	fat_entries = cluster_count + 2;

	/* The two following vars are in hard sectors, i.e. 512 byte sectors! */
	start_data_sector = (reserved_sectors + nr_fats * fat_length +
		cdiv(root_dir_entries * 32, sector_size)) *
		(sector_size / HARD_SECTOR_SIZE);
	start_data_block = (start_data_sector + SECTORS_PER_BLOCK - 1) /
		SECTORS_PER_BLOCK;

	if (blocks < start_data_block + 32) {	/* Arbitrary undersize filesystem! */
		printf("Too few blocks for viable filesystem");
		return -1;
	}

	if (verbose) {
		printf("%s has %d head%s and %d sector%s per track,\n",
				device_name, bs.heads,
				(bs.heads != 1) ? "s" : "", bs.secs_track,
				(bs.secs_track != 1) ? "s" : "");
		printf("hidden sectors 0x%04x;\n",  hidden_sectors);
		printf("logical sector size is %d,\n", sector_size);
		printf("using 0x%02x media descriptor, with %u sectors;\n",
				(int)(bs.media), (unsigned)num_sectors);
		printf("drive number 0x%02x;\n", (int) (vi->drive_number));
		printf("filesystem has %d %d-bit FAT%s and %d sector%s per cluster.\n",
				(int)(bs.fats), size_fat, (bs.fats != 1) ? "s" : "",
				(int)(bs.cluster_size), (bs.cluster_size != 1) ? "s" : "");
		printf("FAT size is %d sector%s, and provides %d cluster%s.\n",
				fat_length, (fat_length != 1) ? "s" : "",
				cluster_count, (cluster_count != 1) ? "s" : "");
		printf("There %s %u reserved sector%s.\n",
				(reserved_sectors != 1) ? "are" : "is",
				reserved_sectors, (reserved_sectors != 1) ? "s" : "");

		if (size_fat != 32) {
			unsigned root_dir_entries =
				bs.dir_entries[0] + ((bs.dir_entries[1]) * 256);
			unsigned root_dir_sectors =
				cdiv(root_dir_entries * 32, sector_size);
			printf("Root directory contains %u slots and uses %u sectors.\n",
				root_dir_entries, root_dir_sectors);
		}
		printf("Volume ID is %08lx, ", volume_id & 0xffffffff);
		if (memcmp(label, NO_NAME, MSDOS_NAME))
			printf("volume label %s.\n", volume_name);
		else
			printf("no volume label.\n");
	}

	/* Make the file allocation tables! */

	if (malloc_entire_fat)
		alloced_fat_length = fat_length;
	else
		alloced_fat_length = 1;

	if ((fat = (unsigned char *)malloc(alloced_fat_length * sector_size)) == NULL) {
		printf("unable to allocate space for FAT image in memory");
		return -1;
	}

	memset(fat, 0, alloced_fat_length * sector_size);

	mark_FAT_cluster(0, 0xffffffff);	/* Initial fat entries */
	mark_FAT_cluster(1, 0xffffffff);
	fat[0] = (unsigned char)bs.media;	/* Put media type in first byte! */
	if (size_fat == 32) {
		/* Mark cluster 2 as EOF (used for root dir) */
		mark_FAT_cluster(2, FAT_EOF);
	}

	/* Make the root directory entries */

	size_root_dir = (size_fat == 32) ?
		bs.cluster_size * sector_size :
		(((int)bs.dir_entries[1] * 256 + (int)bs.dir_entries[0]) *
		sizeof(struct msdos_dir_entry));

	if ((root_dir = (struct msdos_dir_entry *)malloc(size_root_dir)) == NULL) {
		free(fat);		/* Tidy up before we printf! */
		printf("unable to allocate space for root directory in memory");
		return -1;
	}

	memset(root_dir, 0, size_root_dir);
	if (memcmp(label, NO_NAME, MSDOS_NAME)) {
		struct msdos_dir_entry *de = &root_dir[0];
		memcpy(de->name, label, MSDOS_NAME);
		if (de->name[0] == 0xe5)
			de->name[0] = 0x05;
		de->attr = ATTR_VOLUME;

		/* fallback to 1.1.1980 00:00:00 */
		de->time = 0;
		de->date = 1 + (1 << 5);

		de->ctime_cs = 0;
		de->ctime = de->time;
		de->cdate = de->date;
		de->adate = de->date;
		de->starthi = 0;
		de->start = 0;
		de->size = 0;
	}

	if (size_fat == 32) {
		/* For FAT32, create an info sector */
		struct fat32_fsinfo *info;

		if (!(info_sector_buffer = malloc(sector_size))) {
			free(fat);		/* Tidy up before we printf! */
			free(root_dir);		/* Tidy up before we printf! */
			printf("Out of memory");
			return -1;
		}
		memset(info_sector_buffer, 0, sector_size);
		/* fsinfo structure is at offset 0x1e0 in info sector by observation */
		info = (struct fat32_fsinfo *)(info_sector_buffer + 0x1e0);

		/* Info sector magic */
		info_sector_buffer[0] = 'R';
		info_sector_buffer[1] = 'R';
		info_sector_buffer[2] = 'a';
		info_sector_buffer[3] = 'A';

		/* Magic for fsinfo structure */
		info->signature = 0x61417272;
		/* We've allocated cluster 2 for the root dir. */
		info->free_clusters = cluster_count - 1;
		info->next_cluster = 2;

		/* Info sector also must have boot sign */
		*(uint16_t *) (info_sector_buffer + 0x1fe) = BOOT_SIGN;
	}

	if (!(blank_sector = malloc(sector_size))) {
		free(fat);		/* Tidy up before we printf! */
		free(root_dir);		/* Tidy up before we printf! */
		free(info_sector_buffer);		/* Tidy up before we printf! */
		printf("Out of memory");
		return -1;
	}
	memset(blank_sector, 0, sector_size);
	return 0;
}

/* Write the new filesystem's data tables to wherever they're going to end up! */
unsigned long start_addr = 0;

#define seekto(pos,errstr)						\
do {									\
	start_addr = pos;	\
} while(0)

#define writebuf(buf,size,errstr)			\
do {							\
	unsigned int ret = 0;	\
	unsigned long mmc_offset = DIV_ROUND_UP(start_addr, mmc_part_info.blksz);	\
	unsigned int blksz = DIV_ROUND_UP(size, mmc_part_info.blksz);	\
	ret = blk_dwrite(mmc_dev_desc, mmc_part_info.start + mmc_offset, blksz, buf);	\
	start_addr = start_addr + size;	\
	if(ret != blksz)	\
		printf(" size 0x%lx whilst writing %s\n",(unsigned long)size, errstr);	\
} while(0)

static void write_tables(void)
{
	int x;
	int fat_length;
	
	fat_length = (size_fat == 32) ?
		bs.fat32.fat32_length : bs.fat_length;

	seekto(0, "start of device");
	/* clear all reserved sectors */
	for (x = 0; x < reserved_sectors; ++x) {
		writebuf(blank_sector, sector_size, "reserved sector");
	}
	/* seek back to sector 0 and write the boot sector */
	seekto(0, "boot sector");
	writebuf((char *)&bs, sizeof(struct msdos_boot_sector), "boot sector");
	/* on FAT32, write the info sector and backup boot sector */
	if (size_fat == 32) {
		seekto(bs.fat32.info_sector * sector_size, "info sector");
		writebuf(info_sector_buffer, 512, "info sector");
		if (backup_boot != 0) {
			seekto(backup_boot * sector_size, "backup boot sector");
			writebuf((char *)&bs, sizeof(struct msdos_boot_sector),
					"backup boot sector");
			if (backup_boot + bs.fat32.info_sector != bs.fat32.info_sector &&
					backup_boot + bs.fat32.info_sector < reserved_sectors) {
				seekto((backup_boot + bs.fat32.info_sector) * sector_size, "backup info sector");
				writebuf(info_sector_buffer, 512, "backup info sector");
			}
		}
	}
	/* seek to start of FATS and write them all */
	seekto(reserved_sectors * sector_size, "first FAT");
	for (x = 1; x <= nr_fats; x++) {
		int y;
		int blank_fat_length = fat_length - alloced_fat_length;
		writebuf(fat, alloced_fat_length * sector_size, "FAT");
		for (y = 0; y < blank_fat_length; y++)
			writebuf(blank_sector, sector_size, "FAT");
	}
	/* Write the root directory directly after the last FAT. This is the root
		* dir area on FAT12/16, and the first cluster on FAT32. */
	writebuf((char *)root_dir, size_root_dir, "root directory");

	if (blank_sector)
		free(blank_sector);
	free(info_sector_buffer);
	free(root_dir);		/* Free up the root directory space from setup_tables */
	free(fat);			/* Free up the fat table space reserved during setup_tables */
}

/* The "main" entry point into the utility - we pick up the options and attempt to process them in some sort of sensible
way.  In the event that some/all of the options are invalid we need to tell the user so that something can be done! */
int mkfs_fat(const char *ifname, const char *dev_part_str)
{
	int part, ret;
	unsigned long long cblocks = 0;
	unsigned long long total_size = 0;

	part = part_get_info_by_dev_and_name_or_num(ifname, dev_part_str, &mmc_dev_desc, &mmc_part_info, true);

	if (part < 0)
		return CMD_RET_FAILURE;

	volume_id = generate_volume_id();

	volume_name = (char *)mmc_part_info.name;
	printf("mkfs.fat " VERSION " (" VERSION_DATE ")\n");

	total_size = mmc_part_info.size * mmc_part_info.blksz;
	if (total_size <= 0)
		printk("unable to discover size of %s", mmc_part_info.name);

	if (mmc_part_info.blksz > 0) {
		sector_size = mmc_part_info.blksz;
		sector_size_set = 1;
	}

	if (sector_size > 4096)
		fprintf(stderr,
			"Warning: sector size %d > 4096 is non-standard, filesystem may not be usable\n",
			sector_size);

	cblocks = (total_size  - part_sector * sector_size) / BLOCK_SIZE;
	orphaned_sectors = ((total_size - part_sector * sector_size) % BLOCK_SIZE) / sector_size;

	blocks = cblocks;

	/* Establish the media parameters */
	establish_params(&mmc_part_info);

	ret = setup_tables();		/* Establish the filesystem tables */
	if( 0 != ret )
		return -1;
	write_tables();		/* Write the filesystem tables away! */

	return 0;
}