// SPDX-License-Identifier: GPL-2.0
/*
 * eic7702 D2D related command
 *
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
 * Authors: Huangyifeng <huangyifeng@eswincomputing.com>
 */

#include <common.h>
#include <command.h>
#include <spi.h>
#include <spi_flash.h>
#include <dm/uclass.h>
#include <init.h>
#include <dm/device-internal.h>
#include <stdint.h>
#include <linux/io.h>
#include <u-boot/crc.h>

#define DIE0_SYS_CON_TESTREG0  		(0x51810668)
#define DIE1_SYS_CON_TESTREG0  		(0x51810668 + 0x20000000)

#define PMIX_RECORD_ADDR                0xefc000    /* 0xefc000 - 0xefffff (16KiB) */
#define PMIX_ENTRY_NUM 372  /* (16KiB - 12B) / sizeof(struct pmix_entry) */
#define PMIX_MAGIC 0x504d4958

struct One_pmix {
	uint8_t phase0;
	uint8_t phase90;
	uint8_t phase180;
	uint8_t phase270;
	uint8_t width;
} __packed;

struct pmix_entry
{
	struct One_pmix pmix[8];
	uint16_t temperature;
	uint8_t valid;
} __packed;

struct pmix_cfg
{
	uint8_t polarity_inverted;
	uint8_t reserved[31];
} __packed;

struct pmix_lookup_table
{
	uint32_t magic;
	uint32_t version;
	struct pmix_cfg cfg;
	struct pmix_entry pmix_list[PMIX_ENTRY_NUM];
	uint32_t valid_cnt;
	uint32_t crc;
} __packed;

/*
 * struct pvt_poly_term - a term descriptor of the PVT data translation
 *			  polynomial
 * @deg: degree of the term.
 * @coef: multiplication factor of the term.
 * @divider: distributed divider per each degree.
 * @divider_leftover: divider leftover, which couldn't be redistributed.
 */
struct pvt_poly_term {
	unsigned int deg;
	long coef;
	long divider;
	long divider_leftover;
};

/*
 * struct pvt_poly - PVT data translation polynomial descriptor
 * @total_divider: total data divider.
 * @terms: polynomial terms up to a free one.
 */
struct pvt_poly {
	long total_divider;
	struct pvt_poly_term terms[];
};

#define mult_frac(x, numer, denom)(			\
{							\
	typeof(x) quot = (x) / (denom);			\
	typeof(x) rem  = (x) % (denom);			\
	(quot * (numer)) + ((rem * (numer)) / (denom));	\
}							\
)

const struct pvt_poly poly_N_to_temp = {
	.total_divider = 1,
	.terms = {
		{4, -18439, 1000, 1},
		{3, 80705, 1000, 1},
		{2, -185010, 1000, 1},
		{1, 328430, 1000, 1},
		{0, -48690, 1, 1}
	}
};

long eswin_pvt_calc_poly(const struct pvt_poly *poly, long data)
{
	const struct pvt_poly_term *term = poly->terms;
	long tmp, ret = 0;
	unsigned int deg;
	do {
		tmp = term->coef;
		for (deg = 0; deg < term->deg; ++deg)
			tmp = mult_frac(tmp, data, term->divider);
		ret += tmp / term->divider_leftover;
	} while ((term++)->deg);

	return ret / poly->total_divider;
}

/**
 * pmix_table_idx_lookup - Retrieve pmix_entry by index
 * @tbl: Pointer to the pmix_lookup_table
 * @idx: Index of the desired pmix_entry (0-based)
 *
 * Returns:
 * - Pointer to the idx-th pmix_entry if valid parameters
 * - NULL for invalid table, magic mismatch, or out-of-bound index
 */
static struct pmix_entry *pmix_table_idx_lookup(struct pmix_lookup_table *tbl, int idx)
{
	/* Validate input table */
	if (!tbl || tbl->magic != PMIX_MAGIC) {
		return NULL;
	}

	/* Check index boundaries */
	if (idx < 0 || idx >= PMIX_ENTRY_NUM) {
		return NULL;
	}

	/* Directly return the indexed entry */
	return &tbl->pmix_list[idx];
}

/**
 * pmix_table_idx_degree_lookup - Retrieve temperature by index
 * @tbl: Pointer to the pmix_lookup_table
 * @idx: Index of the desired pmix_entry (0-based)
 *
 * Returns:
 * - Temperature(in degrees) on success
 * - -1 for invalid parameters or lookup failure
 */
static long pmix_table_idx_degree_lookup(struct pmix_lookup_table *tbl, int idx)
{
	struct pmix_entry *entry;
	long full_degree;

	/* Step 1: Get entry by index */
	entry = pmix_table_idx_lookup(tbl, idx);
	if (!entry) {
		return -1; // Invalid index or table
	}

	/* Step 2: Convert raw temperature to degrees */
	full_degree = eswin_pvt_calc_poly(&poly_N_to_temp, entry->temperature);

	/* Step 3: Extract integer part */
	return full_degree ;
}

static int d2d_pmix_load(const char *node_name, struct pmix_lookup_table *tbl)
{
	struct spi_flash *flash = NULL;
	struct udevice *bus, *dev;
	int ret;

	/* Get SPI bus device */
	ret = uclass_get_device_by_name(UCLASS_SPI, node_name, &bus);
	if (ret) {
		printf("SPI flash, failed to get node %s\n", node_name);
		return ret;
	}

	/* Find device with chip select 0 */
	ret = spi_find_chip_select(bus, 0, &dev);
	if (ret) {
		printf("SPI flash, Invalid chip select: %d (err=%d)\n", 0, ret);
		return ret;
	}

	/* Ensure the device is active */
	if (!device_active(dev)) {
		ret = device_probe(dev);
		if (ret) {
			printf("SPI flash, device_probe fail (err=%d)\n", ret);
			return ret;
		}
	}

	/* Get SPI flash operation handle */
	flash = dev_get_uclass_priv(dev);
	if (!flash) {
		printf("SPI flash, dev_get_uclass_priv failed\n");
		return -ENODEV;
	}

	/* Read data from PMIX region */
	ret = spi_flash_read(flash, PMIX_RECORD_ADDR, sizeof(struct pmix_lookup_table), tbl);
	if (ret) {
		printf("Failed to read PMIX data from %s (err=%d)\n", node_name, ret);
		return ret;
	}

	return 0;
}

static int d2d_pmix_erase(const char *node_name)
{
	struct spi_flash *flash = NULL;
	struct udevice *bus, *dev;
	int ret;
	void *addr;

	ret = uclass_get_device_by_name(UCLASS_SPI, node_name, &bus);
	if(ret) {
		printf("SPI flash, failed to get node %s\n", node_name);
		return ret;
	}

	ret = spi_find_chip_select(bus, 0, &dev);
	if(ret) {
		printf("SPI flash, Invalid chip select :%d (err=%d)\n", 0, ret);
		return ret;
	}

	if (!device_active(dev)) {
		if(device_probe(dev)) {
			printf("SPI flash, device_probe fail\n");
			return -1;
		}
	}
	flash = dev_get_uclass_priv(dev);
	if(!flash) {
		printf("SPI flash, dev_get_uclass_priv failed\n");
		return -1;
	}
	addr = (void *)ALIGN_DOWN(PMIX_RECORD_ADDR, SZ_64K);
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 0);
	ret = spi_flash_erase(flash, PMIX_RECORD_ADDR, SZ_4K);
	if(ret) {
		printf("D2D PMIX Data erase failed\n");
	}
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 1);
	printf("D2D PMIX Data has been invalid\n");
	return 0;
}

static int do_d2d_pmix_invalid(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	d2d_pmix_erase("spi@51800000");
	d2d_pmix_erase("spi@71800000");
	return 0;
}

struct pmix_lookup_table tbl;
static int do_d2d_pmix_get_low_temp(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	long die0_degree, die1_degree;
	long temp_diff;
	int ret;

	/* Load PMIX data from Die 0 */
	ret = d2d_pmix_load("spi@51800000", &tbl);
	if (ret) {
		printf("Failed to load Die0 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	/* Get start temperature from Die 0 */
	die0_degree = pmix_table_idx_degree_lookup(&tbl, 0);
	if (die0_degree < 0) {
		printf("Failed to get Die0 temperature (err=%ld)\n", die0_degree);
		return CMD_RET_FAILURE;
	}

	/* Load PMIX data from Die 1 */
	ret = d2d_pmix_load("spi@71800000", &tbl);
	if (ret) {
		printf("Failed to load Die1 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	/* Get start temperature from Die 1 */
	die1_degree = pmix_table_idx_degree_lookup(&tbl, 0);
	if (die1_degree < 0) {
		printf("Failed to get Die1 temperature (err=%ld)\n", die1_degree);
		return CMD_RET_FAILURE;
	}

	/* Print formatted temperature results */
	printf("Die0 PMIX start temperature: %ld.%03ld °C\n",
		die0_degree / 1000, abs(die0_degree % 1000));
	printf("Die1 PMIX start temperature: %ld.%03ld °C\n",
		die1_degree / 1000, abs(die1_degree % 1000));

	/* Calculate and print temperature difference */
	temp_diff = die0_degree - die1_degree;

	/* Determine sign and absolute value */
	const char *sign = (temp_diff >= 0) ? "+" : "-";
	long abs_diff = abs(temp_diff);

	printf("Temperature difference: %s%ld.%03ld °C (Die0 - Die1)\n",
		sign, abs_diff / 1000, abs_diff % 1000);

	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_get_curr_temp(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	long die0_temp, die1_temp;
	long die0_degree, die1_degree;
	long temp_diff;

	/* Read PVT raw temperature value from Die 0 */
	die0_temp = ioread32((void *)DIE0_SYS_CON_TESTREG0);

	/* Read PVT raw temperature value from Die 1 */
	die1_temp = ioread32((void *)DIE1_SYS_CON_TESTREG0);

	/* Convert to degrees Celsius (in milli-degree units) */
	die0_degree = eswin_pvt_calc_poly(&poly_N_to_temp, die0_temp);
	die1_degree = eswin_pvt_calc_poly(&poly_N_to_temp, die1_temp);

	/* Print raw values and converted temperatures */
	printf("Die0 PVT temperature: %ld.%03ld °C\n",
		die0_degree / 1000, abs(die0_degree % 1000));
	printf("Die1 PVT temperature: %ld.%03ld °C\n",
		die1_degree / 1000, abs(die1_degree % 1000));

	/* Calculate and print temperature difference */
	temp_diff = die0_degree - die1_degree;
	const char *sign = (temp_diff >= 0) ? "+" : "-";
	long abs_diff = abs(temp_diff);

	printf("Temperature difference: %s%ld.%03ld °C (Die0 - Die1)\n",
		sign, abs_diff / 1000, abs_diff % 1000);

	return CMD_RET_SUCCESS;
}

#define PERIPH_D2D_BASE 0x52100000
#define D2D_INTERLAKEN_CLK_REG (PERIPH_D2D_BASE + 0x1000)

static int do_d2d_pmix_get_mode(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	uint32_t reg_value;
	const char *mode_str;

	/* Read D2D Interlaken clock register */
	reg_value = ioread32((void *)D2D_INTERLAKEN_CLK_REG);

	/* Determine operating mode */
	switch (reg_value) {
	case 0x207c81:
		mode_str = "High speed mode";
		break;
	case 0x607c81:
		mode_str = "Low speed mode";
		break;
	default:
		mode_str = "Unknown mode";
		break;
	}

	/* Print current mode */
	printf("D2D operating mode: %s\n", mode_str);

	/* If mode is unknown, print register value for debugging */
	if (reg_value != 0x207c81 && reg_value != 0x607c81) {
		printf("D2D Interlaken Clock Register: 0x%06x\n", reg_value);
		printf("  Expected values: 0x207c81 (High) or 0x607c81 (Low)\n");
	}
	return CMD_RET_SUCCESS;
}

static int d2d_pmix_verify(struct pmix_lookup_table *tbl)
{
	uint32_t calculated_crc;
	uint32_t stored_crc;

	/* Check magic value */
	if (tbl->magic != PMIX_MAGIC) {
		printf("  Error: Invalid magic value (0x%08x != 0x%08x)\n",
		       tbl->magic, PMIX_MAGIC);
		return CMD_RET_FAILURE;
	}

	/* Calculate and verify CRC */
	stored_crc = tbl->crc;
	calculated_crc = crc32(0xffffffff, (const void *)tbl, sizeof(*tbl) -4);

	if (calculated_crc != stored_crc) {
		printf("  Error: CRC mismatch (stored: 0x%08x, calculated: 0x%08x)\n",
		       stored_crc, calculated_crc);
		return CMD_RET_FAILURE;
	}

	/* Check valid entry count */
	if (tbl->valid_cnt > PMIX_ENTRY_NUM) {
		printf("  Error: Invalid entry count (%u > %d)\n",
		       tbl->valid_cnt, PMIX_ENTRY_NUM);
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_validate(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;

	printf("Validating Die 0 PMIX data...\n");

	/* Load PMIX data from Die 0 */
	ret = d2d_pmix_load("spi@51800000", &tbl);
	if (ret) {
		printf("  Error: Failed to load Die0 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	/* Verify Die 0 PMIX data */
	ret = d2d_pmix_verify(&tbl);
	if (ret != CMD_RET_SUCCESS)
		return ret;

	printf("Die 0 PMIX data validation passed!\n");

	printf("\nValidating Die 1 PMIX data...\n");

	/* Load PMIX data from Die 1 */
	ret = d2d_pmix_load("spi@71800000", &tbl);
	if (ret) {
		printf("  Error: Failed to load Die1 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	/* Verify Die 1 PMIX data */
	ret = d2d_pmix_verify(&tbl);
	if (ret != CMD_RET_SUCCESS)
		return ret;

	printf("Die 1 PMIX data validation passed!\n");

	return CMD_RET_SUCCESS;
}

/* Extend subcommand list */
static struct cmd_tbl d2d_sub[] = {
	U_BOOT_CMD_MKENT(pmix invalid, 1, 0, do_d2d_pmix_invalid, "", ""),
	U_BOOT_CMD_MKENT(pmix validate, 1, 0, do_d2d_pmix_validate, "", ""),
	U_BOOT_CMD_MKENT(get low-temperature, 1, 0, do_d2d_pmix_get_low_temp, "", ""),
	U_BOOT_CMD_MKENT(get curr-temperature, 1, 0, do_d2d_pmix_get_curr_temp, "", ""),
	U_BOOT_CMD_MKENT(get mode, 1, 0, do_d2d_pmix_get_mode, "", ""),
};

/* Parent command handler remains unchanged */
static int do_d2d(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	struct cmd_tbl *sub_cmd;
	char combined_cmd[64] = {0};
	const char *cmd = (argc > 1) ? argv[1] : NULL;

	if (!cmd)
		return CMD_RET_USAGE;

	/* Combine the first two arguments to support multi-word commands */
	if (argc > 2) {
		snprintf(combined_cmd, sizeof(combined_cmd), "%s %s", argv[1], argv[2]);
		cmd = combined_cmd;
	}

	sub_cmd = find_cmd_tbl(cmd, d2d_sub, ARRAY_SIZE(d2d_sub));
	if (sub_cmd) {
		/* For multi-word commands, skip the first two arguments */
		int skip = (strchr(cmd, ' ') != NULL) ? 2 : 1;
		return sub_cmd->cmd(sub_cmd, flag, argc - skip, argv + skip);
	}

	return CMD_RET_USAGE;
}

U_BOOT_CMD(
	d2d,	/* Command name */
	3,		/* Increase max argument count to 3 (d2d + pmix + subcommand) */
	0,		/* Repeatable flag */
	do_d2d,	/* Handler function */
	"D2D subsystem commands",	/* Short help text */
	"pmix invalid - Invalidate D2D PMIX Data\n"
	"pmix validate - Validate PMIX data integrity\n"
	"get low-temperature - Get PMIX data start temperature\n"
	"get curr-temperature - Get current PVT temperature\n"
	"get mode - Get D2D operating mode\n"
);
