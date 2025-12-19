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

#include "d2d_pmix.h"

#define DIE0_SYS_CON_TESTREG0  		(0x51810668)
#define DIE1_SYS_CON_TESTREG0  		(0x51810668 + 0x20000000)

#define TEMP_INVALID -274000

typedef enum {
	SWEEP_METHOD_NONE = 0,
	SWEEP_METHOD_NORMAL,
	SWEEP_METHOD_SMOOTH,
} sweep_method_t;

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

static struct pmix_lookup_table _pmix_tbl;

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
	if (idx < 0 || idx >= tbl->valid_cnt) {
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
 * - TEMP_INVALID for invalid parameters or lookup failure
 */
static long pmix_table_idx_degree_lookup(struct pmix_lookup_table *tbl, int idx)
{
	struct pmix_entry *entry;
	long full_degree;

	/* Step 1: Get entry by index */
	entry = pmix_table_idx_lookup(tbl, idx);
	if (!entry) {
		return TEMP_INVALID; // Invalid index or table
	}

	/* Step 2: Convert raw temperature to degrees */
	full_degree = eswin_pvt_calc_poly(&poly_N_to_temp, entry->temperature);

	/* Step 3: Extract integer part */
	return full_degree;
}

static uint32_t d2d_pmix_legacy_calc_crc(const struct pmix_lookup_table_legacy *tbl)
{
	const void *addr;
	uint32_t len;

	/* Calculate CRC only for fit_params, pmix_list and valid_cnt. */
	addr = tbl->fit_params;
	len = (void *)&tbl->crc - addr;

	return crc32(0xffffffff, addr, len);
}

static uint32_t d2d_pmix_calc_crc(const struct pmix_lookup_table *tbl)
{
	const void *addr;
	uint32_t len;

	/* Calculate CRC only for fit_params, pmix_list and valid_cnt. */
	addr = tbl->fit_params;
	len = (void *)&tbl->crc - addr;

	return crc32(0xffffffff, addr, len);
}

static bool pmix_table_legacy_upgrade(struct pmix_lookup_table_legacy *tbl_legacy)
{
	struct pmix_lookup_table *tbl = (struct pmix_lookup_table *)tbl_legacy;

	if (!tbl_legacy)
		return false;

	tbl->version = PMIX_LEGACY_UPGRADED_VERSION;
	tbl->valid_cnt = tbl_legacy->valid_cnt;
	tbl_legacy->valid_cnt = 0;
	tbl_legacy->crc = 0;
	tbl->crc = d2d_pmix_calc_crc(tbl);
	printf("PMIX data upgraded.\n");
	return true;
}

static bool pmix_table_try_upgrade(struct pmix_lookup_table *tbl)
{
	if (!tbl)
		return false;

	if (tbl->magic != PMIX_MAGIC)
		return false;

	if (tbl->version == PMIX_LEGACY_VERSION) {
		struct pmix_lookup_table_legacy *tbl_legacy = (struct pmix_lookup_table_legacy *)tbl;
		if (tbl_legacy->crc != d2d_pmix_legacy_calc_crc(tbl_legacy))
			return false;
		if ((tbl_legacy->valid_cnt == 0) || (tbl_legacy->valid_cnt > ARRAY_SIZE(tbl_legacy->pmix_list)))
			return false;

		return pmix_table_legacy_upgrade(tbl_legacy);
	}

	return false;
}

static bool pmix_table_downgrade(struct pmix_lookup_table *tbl)
{
	struct pmix_lookup_table_legacy *tbl_legacy = (struct pmix_lookup_table_legacy *)tbl;

	if (!tbl)
		return false;

	if (tbl->valid_cnt > ARRAY_SIZE(tbl_legacy->pmix_list))
		return false;

	tbl_legacy->version = PMIX_LEGACY_VERSION;
	tbl_legacy->valid_cnt = tbl->valid_cnt;
	tbl->valid_cnt = 0;
	tbl->crc = 0;
	tbl_legacy->crc = d2d_pmix_legacy_calc_crc(tbl_legacy);
	printf("PMIX data downgraded.\n");
	return true;
}

static bool pmix_table_try_downgrade(struct pmix_lookup_table *tbl)
{
	if (!tbl)
		return false;

	if (tbl->magic != PMIX_MAGIC)
		return false;

	if (tbl->version != PMIX_LEGACY_VERSION) {
		if (tbl->crc != d2d_pmix_calc_crc(tbl))
			return false;
		if ((tbl->valid_cnt == 0) || (tbl->valid_cnt > ARRAY_SIZE(tbl->pmix_list)))
			return false;

		return pmix_table_downgrade(tbl);
	}

	return false;
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

static int d2d_pmix_store(const char *node_name, struct pmix_lookup_table *tbl)
{
	struct spi_flash *flash = NULL;
	struct udevice *bus, *dev;
	int ret;
	void *addr;
	u32 erase_size;

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

	/* Write data to PMIX region */
	addr = (void *)ALIGN_DOWN(PMIX_RECORD_ADDR, SZ_64K);
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 0);

	erase_size = ALIGN(sizeof(struct pmix_lookup_table), flash->erase_size);
	ret = spi_flash_erase(flash, PMIX_RECORD_ADDR, erase_size);
	if (ret) {
		printf("Failed to erase PMIX data in %s (err=%d)\n", node_name, ret);
		goto out;
	}

	ret = spi_flash_write(flash, PMIX_RECORD_ADDR, sizeof(struct pmix_lookup_table), tbl);

	if (ret) {
		printf("Failed to write PMIX data in %s (err=%d)\n", node_name, ret);
		goto out;
	}

out:
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 0);
	return ret;
}

static int d2d_pmix_erase(const char *node_name)
{
	struct spi_flash *flash = NULL;
	struct udevice *bus, *dev;
	int ret;
	void *addr;
	u32 erase_size;

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
	erase_size = ALIGN(sizeof(struct pmix_lookup_table), flash->erase_size);
	ret = spi_flash_erase(flash, PMIX_RECORD_ADDR, erase_size);
	if(ret) {
		printf("D2D PMIX Data erase failed\n");
	}
	es_flash_region_wp_cfg(flash, addr, SZ_64K, 1);
	return 0;
}

static int do_d2d_pmix_invalid(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int die_num = 2;

	if (argc >= 2)
		die_num = simple_strtol(argv[1], NULL, 0);

	if (die_num == 0) {
		d2d_pmix_erase("spi@51800000");
		printf("Die0 D2D PMIX Data has been invalidated.\n");
	} else if (die_num == 1) {
		d2d_pmix_erase("spi@71800000");
		printf("Die1 D2D PMIX Data has been invalidated.\n");
	} else {
		d2d_pmix_erase("spi@51800000");
		d2d_pmix_erase("spi@71800000");
		printf("Die0/Die1 D2D PMIX Data has been invalidated.\n");
	}
	return 0;
}

static int do_d2d_pmix_get_low_temp(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	long die0_degree, die1_degree;
	long temp_diff;
	int ret;

	/* Load PMIX data from Die 0 */
	ret = d2d_pmix_load("spi@51800000", &_pmix_tbl);
	if (ret) {
		printf("Failed to load Die0 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	pmix_table_try_upgrade(&_pmix_tbl);

	/* Get start temperature from Die 0 */
	die0_degree = pmix_table_idx_degree_lookup(&_pmix_tbl, 0);
	if (die0_degree == TEMP_INVALID) {
		printf("Failed to get Die0 temperature (err=%ld)\n", die0_degree);
		return CMD_RET_FAILURE;
	}

	/* Load PMIX data from Die 1 */
	ret = d2d_pmix_load("spi@71800000", &_pmix_tbl);
	if (ret) {
		printf("Failed to load Die1 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	pmix_table_try_upgrade(&_pmix_tbl);

	/* Get start temperature from Die 1 */
	die1_degree = pmix_table_idx_degree_lookup(&_pmix_tbl, 0);
	if (die1_degree == TEMP_INVALID) {
		printf("Failed to get Die1 temperature (err=%ld)\n", die1_degree);
		return CMD_RET_FAILURE;
	}

	/* Print formatted temperature results */
	printf("Die0 PMIX start temperature: %ld.%03ld °C\n",
		die0_degree / 1000, abs(die0_degree) % 1000);
	printf("Die1 PMIX start temperature: %ld.%03ld °C\n",
		die1_degree / 1000, abs(die1_degree) % 1000);

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
	die0_temp = ioread32((void *)DIE0_SYS_CON_TESTREG0) & 0xFFFF;

	/* Read PVT raw temperature value from Die 1 */
	die1_temp = ioread32((void *)DIE1_SYS_CON_TESTREG0) & 0xFFFF;

	/* Convert to degrees Celsius (in milli-degree units) */
	die0_degree = eswin_pvt_calc_poly(&poly_N_to_temp, die0_temp);
	die1_degree = eswin_pvt_calc_poly(&poly_N_to_temp, die1_temp);

	/* Print raw values and converted temperatures */
	printf("Die0 PVT temperature: %ld.%03ld °C\n",
		die0_degree / 1000, abs(die0_degree) % 1000);
	printf("Die1 PVT temperature: %ld.%03ld °C\n",
		die1_degree / 1000, abs(die1_degree) % 1000);

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

	/* Check version */
	if ((GET_PMIX_MAJOR_VERSION(tbl->version) != PMIX_MAJOR_VERSION) ||
			(GET_PMIX_MINOR_VERSION(tbl->version) > PMIX_MINOR_VERSION)) {
		printf("  Error: Unmatched version (0x%08x, 0x%08x)\n",
		       tbl->version, PMIX_VERSION);
		return CMD_RET_FAILURE;
	}

	/* Calculate and verify CRC */
	stored_crc = tbl->crc;
	calculated_crc = d2d_pmix_calc_crc(tbl);
	if (calculated_crc != stored_crc) {
		printf("  Error: CRC mismatch (stored: 0x%08x, calculated: 0x%08x)\n",
		       stored_crc, calculated_crc);
		return CMD_RET_FAILURE;
	}

	/* Check valid entry count */
	if (tbl->valid_cnt > ARRAY_SIZE(tbl->pmix_list)) {
		printf("  Error: Invalid entry count (%d > %ld)\n",
		       tbl->valid_cnt, ARRAY_SIZE(tbl->pmix_list));
		return CMD_RET_FAILURE;
	}

	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_validate(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;

	printf("Validating Die 0 PMIX data...\n");

	/* Load PMIX data from Die 0 */
	ret = d2d_pmix_load("spi@51800000", &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load Die0 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	pmix_table_try_upgrade(&_pmix_tbl);

	/* Verify Die 0 PMIX data */
	ret = d2d_pmix_verify(&_pmix_tbl);
	if (ret != CMD_RET_SUCCESS)
		return ret;

	printf("Die 0 PMIX data validation passed!\n");

	printf("\nValidating Die 1 PMIX data...\n");

	/* Load PMIX data from Die 1 */
	ret = d2d_pmix_load("spi@71800000", &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load Die1 PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	pmix_table_try_upgrade(&_pmix_tbl);

	/* Verify Die 1 PMIX data */
	ret = d2d_pmix_verify(&_pmix_tbl);
	if (ret != CMD_RET_SUCCESS)
		return ret;

	printf("Die 1 PMIX data validation passed!\n");

	return CMD_RET_SUCCESS;
}

/* isinf & isnan macros should be provided by math.h */
#ifndef isinf
/* The value of Inf - Inf is NaN */
#define isinf(n) (isnan((n) - (n)) && !isnan(n))
#endif

#ifndef isnan
/* NaN is the only floating point value that does NOT equal itself.
 * Therefore if n != n, then it is NaN. */
#define isnan(n) ((n != n) ? 1 : 0)
#endif

static inline void prfloat(void (*putch)(int, void*), void *putdat,
        double num_ull, unsigned base, int width, int padc)
{
#ifdef UNITY_INCLUDE_DOUBLE
    static const int sig_digits = 9;
    static const int min_scaled = 100000000;
    static const int max_scaled = 1000000000;
#else
    static const int sig_digits = 7;
    static const int min_scaled = 1000000;
    static const int max_scaled = 10000000;
#endif

    double number = num_ull;
    /* print minus sign (does not handle negative zero) */
    if (number < 0.0f)
    {
        putch('-', putdat);
        number = -number;
    }

    /* handle zero, NaN, and +/- infinity */
    if (number == 0.0f)
    {
        putch('0', putdat);
    }
    else if (isnan(number))
    {
		putch('n', putdat);
		putch('a', putdat);
		putch('n', putdat);
    }
    else if (isinf(number))
    {
		putch('i', putdat);
		putch('n', putdat);
		putch('f', putdat);
    }
    else
    {
        int n_int = 0, n;
        int exponent = 0;
        int decimals, digits;
        char buf[16] = {0};

        /*
         * Scale up or down by powers of 10.  To minimize rounding error,
         * start with a factor/divisor of 10^10, which is the largest
         * power of 10 that can be represented exactly.  Finally, compute
         * (exactly) the remaining power of 10 and perform one more
         * multiplication or division.
         */
        if (number < 1.0f)
        {
            double factor = 1.0f;

            while (number < (double)max_scaled / 1e10f)  { number *= 1e10f; exponent -= 10; }
            while (number * factor < (double)min_scaled) { factor *= 10.0f; exponent--; }

            number *= factor;
        }
        else if (number > (double)max_scaled)
        {
            double divisor = 1.0f;

            while (number > (double)min_scaled * 1e10f)   { number  /= 1e10f; exponent += 10; }
            while (number / divisor > (double)max_scaled) { divisor *= 10.0f; exponent++; }

            number /= divisor;
        }
        else
        {
            /*
             * In this range, we can split off the integer part before
             * doing any multiplications.  This reduces rounding error by
             * freeing up significant bits in the fractional part.
             */
            double factor = 1.0f;
            n_int = (int)number;
            number -= (double)n_int;

            while (n_int < min_scaled) { n_int *= 10; factor *= 10.0f; exponent--; }

            number *= factor;
        }

        /* round to nearest integer */
        n = ((int)(number + number) + 1) / 2;

#ifndef UNITY_ROUND_TIES_AWAY_FROM_ZERO
        /* round to even if exactly between two integers */
        if ((n & 1) && (((double)n - number) == 0.5f))
            n--;
#endif

        n += n_int;

        if (n >= max_scaled)
        {
            n = min_scaled;
            exponent++;
        }

        /* determine where to place decimal point */
        decimals = ((exponent <= 0) && (exponent >= -(sig_digits + 3))) ? (-exponent) : (sig_digits - 1);
        exponent += decimals;

        /* truncate trailing zeroes after decimal point */
        while ((decimals > 0) && ((n % 10) == 0))
        {
            n /= 10;
            decimals--;
        }

        /* build up buffer in reverse order */
        digits = 0;
        while ((n != 0) || (digits < (decimals + 1)))
        {
            buf[digits++] = (char)('0' + n % 10);
            n /= 10;
        }
        while (digits > 0)
        {
            if (digits == decimals) { putch('.', putdat); }
            putch(buf[--digits], putdat);
        }

        /* print exponent if needed */
        if (exponent != 0)
        {
            putch('e', putdat);

            if (exponent < 0)
            {
                putch('-', putdat);
                exponent = -exponent;
            }
            else
            {
                putch('+', putdat);
            }

            digits = 0;
            while ((exponent != 0) || (digits < 2))
            {
                buf[digits++] = (char)('0' + exponent % 10);
                exponent /= 10;
            }
            while (digits > 0)
            {
                putch(buf[--digits], putdat);
            }
        }
    }
}

struct str_buf {
	char buf[32];
	int pos;
};

static void buf_putch(int ch, void *ptr)
{
	struct str_buf *str_buf = ptr;
	str_buf->buf[str_buf->pos++] = ch;
}

static const char *double_to_str(double number)
{
	static struct str_buf str_buf;
	memset(&str_buf, 0, sizeof(str_buf));
	prfloat(buf_putch, &str_buf, number, 10, 0, 0);
	return str_buf.buf;
}

static const char *d2d_pmix_cfg_name(size_t id)
{
	static const char *cfg_name[] = {
		"Polarity inverted",
		"Sweep method",
		"Debug print enabled",
		"Fit valid degree",
	};

	if (id >= ARRAY_SIZE(cfg_name))
		return "";

	return cfg_name[id];
}

void pmix_lookup_table_print(struct pmix_lookup_table *tbl)
{
	long degree;
    const char *sweep_method_names[] = {
		[SWEEP_METHOD_NONE]   = "NONE",
		[SWEEP_METHOD_NORMAL] = "NORMAL",
		[SWEEP_METHOD_SMOOTH] = "SMOOTH"
    };

    // Safely handle unknown sweep methods
    const char *method_name = "UNKNOWN";
    if (tbl->cfg.sweep_method < sizeof(sweep_method_names) / sizeof(sweep_method_names[0])) {
		method_name = sweep_method_names[tbl->cfg.sweep_method];
    }

	printf("lookup table print\n");

    printf("\tMagic: 0x%X, Version: 0x%X, Valid entry: %d, Sweep method: %s\n", tbl->magic, tbl->version, tbl->valid_cnt, method_name);

	if (d2d_pmix_verify(tbl) != CMD_RET_SUCCESS)
		return;

	printf("\tConfig:\n");
	for (size_t i = 0; i < ARRAY_SIZE(tbl->cfg.data); i++) {
		if (!strcmp(d2d_pmix_cfg_name(i), ""))
			continue;
		printf("\t\t%s: %d\n", d2d_pmix_cfg_name(i), tbl->cfg.data[i]);
	}

	if (tbl->cfg.fit_valid_degree != 0xff) {
		for (size_t i = 0; i < ARRAY_SIZE(tbl->fit_params); i++) {
			printf("\tFit[%ld]:\n", i);

			printf("\t\tPhase0: ");
			printf("c0=%s, ", double_to_str(tbl->fit_params[i].phase0_coeffs.c0));
			printf("c1=%s\n", double_to_str(tbl->fit_params[i].phase0_coeffs.c1));

			printf("\t\tPhase90: ");
			printf("c0=%s, ", double_to_str(tbl->fit_params[i].phase90_coeffs.c0));
			printf("c1=%s\n", double_to_str(tbl->fit_params[i].phase90_coeffs.c1));

			printf("\t\tPhase180: ");
			printf("c0=%s, ", double_to_str(tbl->fit_params[i].phase180_coeffs.c0));
			printf("c1=%s\n", double_to_str(tbl->fit_params[i].phase180_coeffs.c1));

			printf("\t\tPhase270: ");
			printf("c0=%s, ", double_to_str(tbl->fit_params[i].phase270_coeffs.c0));
			printf("c1=%s\n", double_to_str(tbl->fit_params[i].phase270_coeffs.c1));
		}
	}

	for (int i = 0; i < tbl->valid_cnt; i++) {
		struct pmix_entry *entry = &tbl->pmix_list[i];
		if (1 != entry->valid)
			continue;

		degree = eswin_pvt_calc_poly(&poly_N_to_temp, entry->temperature);
		printf("\tEntry[%d]: Temperature=0x%x((%ld.%03ld)), Valid=0x%x\n", i, entry->temperature,
				degree/1000, abs(degree%1000), entry->valid);
		for (int j = 0; j < 8; j++) {
			printf("\t\tPMIX[%d]: Phase0=0x%x, Phase90=0x%x, Phase180=0x%x, Phase270=0x%x, width=%d\n",
					j,
					entry->pmix[j].phase0,
					entry->pmix[j].phase90,
					entry->pmix[j].phase180,
					entry->pmix[j].phase270,
					entry->pmix[j].width);
		}
	}

	printf("CRC: 0x%X\n", tbl->crc);
}

static int do_d2d_pmix_show(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	char *node_name = "spi@51800000";
	int ret;

	if ((argc >= 2) && !strcmp(argv[1], "1")) {
		node_name = "spi@71800000";
		printf("Loading Die 1 PMIX data ...\n");
	} else {
		printf("Loading Die 0 PMIX data ...\n");
	}

	/* Load PMIX data from */
	ret = d2d_pmix_load(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	pmix_table_try_upgrade(&_pmix_tbl);
	pmix_lookup_table_print(&_pmix_tbl);
	return CMD_RET_SUCCESS;
}

static int d2d_pmix_prune(struct pmix_lookup_table *tbl, int min_degree, int max_degree)
{
	uint32_t i;
	uint32_t start, end;
	long degree;
	uint32_t prune_cnt = 0;

	if (min_degree > max_degree)
		return -1;

	if (min_degree < TEMP_INVALID)
		min_degree = TEMP_INVALID;

	if (max_degree > 100)
		max_degree = 100;

	start = 0;
	end = tbl->valid_cnt - 1;
	for (i = 0; i < tbl->valid_cnt; i++) {
		degree = pmix_table_idx_degree_lookup(tbl, i);
		if (degree == TEMP_INVALID)
			continue;

		if (degree >= (min_degree * 1000)) {
			start = i;
			break;
		}
	}

	if (i >= tbl->valid_cnt)
		goto out;

	for (; i < tbl->valid_cnt; i++) {
		degree = pmix_table_idx_degree_lookup(tbl, i);
		if (degree == TEMP_INVALID)
			continue;

		if (degree == (max_degree * 1000)) {
			end = i;
			break;
		} else if (degree > (max_degree * 1000)) {
			if (i == start)
				goto out;
			end = i - 1;
			break;
		}
	}

	/*
	 * [O][O][X][X][X][X][O][O][O]
	 *        ^        ^
	 *        |        |
	 *        S        E
	 *
	 * [O][O][O][O][O][X][X][X][X]
	 *        ^        ^
	 *        |        |
	 *        S        E
	 */
	prune_cnt = end - start + 1;
	if (prune_cnt > 0) {
		uint32_t mov_cnt = tbl->valid_cnt - end - 1;

		for (i = 0; i < mov_cnt; i++) {
			memcpy(&tbl->pmix_list[start + i], &tbl->pmix_list[end + 1 + i], sizeof(struct pmix_entry));
		}
		memset(&tbl->pmix_list[start + mov_cnt], 0, sizeof(struct pmix_entry) * prune_cnt);

		tbl->valid_cnt -= prune_cnt;
		tbl->crc = d2d_pmix_calc_crc(tbl);
	}

out:
	printf("====prune result====\n");
	printf("prune_cnt: %d\n", prune_cnt);
	printf("valid_cnt: %d\n", tbl->valid_cnt);
	printf("crc: 0x%08x\n", tbl->crc);
	return (int)prune_cnt;
}

static int do_d2d_pmix_prune(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int die_num = 0;
	char *node_name = "spi@51800000";
	int min_degree, max_degree;
	bool upgraded;

	if (argc < 4)
		return CMD_RET_USAGE;

	if (!strcmp(argv[1], "1")) {
		die_num = 1;
		node_name = "spi@71800000";
	}

	min_degree = simple_strtol(argv[2], NULL, 0);
	max_degree = simple_strtol(argv[3], NULL, 0);

	printf("Die %d PMIX data pruning (Min: %d, Max: %d) ...\n", die_num, min_degree, max_degree);

	/* Load PMIX data */
	ret = d2d_pmix_load(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	upgraded = pmix_table_try_upgrade(&_pmix_tbl);

	/* Verify PMIX data */
	ret = d2d_pmix_verify(&_pmix_tbl);
	if (ret != CMD_RET_SUCCESS) {
		printf("  Error: PMIX data is invalid (err=%d)\n", ret);
		return ret;
	}

	ret = d2d_pmix_prune(&_pmix_tbl, min_degree, max_degree);
	if (ret < 0) {
		printf("  Error: Failed to prune PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	} else if (ret > 0) {
		if (upgraded)
			pmix_table_try_downgrade(&_pmix_tbl);
		ret = d2d_pmix_store(node_name, &_pmix_tbl);
		if (ret) {
			printf("  Error: Failed to store PMIX data (err=%d)\n", ret);
			return CMD_RET_FAILURE;
		}
	}

	printf("Die %d PMIX data pruned (Min: %d, Max: %d) successfully.\n", die_num, min_degree, max_degree);
	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_cfg(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int die_num = 0;
	char *node_name = "spi@51800000";
	size_t id = 0;
	uint8_t data = 0;
	bool upgraded;

	if (argc < 2) {
		return CMD_RET_USAGE;
	}

	if (!strcmp(argv[1], "1")) {
		die_num = 1;
		node_name = "spi@71800000";
	}

	/* Load PMIX data */
	ret = d2d_pmix_load(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	upgraded = pmix_table_try_upgrade(&_pmix_tbl);

	/* Verify PMIX data */
	ret = d2d_pmix_verify(&_pmix_tbl);
	if (ret != CMD_RET_SUCCESS) {
		printf("  Error: PMIX data is invalid (err=%d)\n", ret);
		return ret;
	}

	if (argc < 3) {
		for (size_t i = 0; i < ARRAY_SIZE(_pmix_tbl.cfg.data); i++) {
			if (!strcmp(d2d_pmix_cfg_name(i), ""))
				continue;
			printf("Die %d CFG[%02ld] - %s: %d\n", die_num, i, d2d_pmix_cfg_name(i), _pmix_tbl.cfg.data[i]);
		}
		return CMD_RET_SUCCESS;
	}

	id = simple_strtol(argv[2], NULL, 0);
	if (id >= ARRAY_SIZE(_pmix_tbl.cfg.data)) {
		printf("  Error: invalid index.\n");
	}

	if (argc < 4) {
		printf("Die %d CFG[%02ld] - %s: %d\n", die_num, id, d2d_pmix_cfg_name(id), _pmix_tbl.cfg.data[id]);
		return CMD_RET_SUCCESS;
	}

	data = simple_strtol(argv[3], NULL, 0);
	if (_pmix_tbl.cfg.data[id] != data) {
		_pmix_tbl.cfg.data[id] = data;

		if (upgraded)
			pmix_table_try_downgrade(&_pmix_tbl);

		ret = d2d_pmix_store(node_name, &_pmix_tbl);
		if (ret) {
			printf("  Error: Failed to store PMIX data (err=%d)\n", ret);
			return CMD_RET_FAILURE;
		}
	}

	printf("Die %d CFG[%02ld] - %s set to %d successfully.\n", die_num, id, d2d_pmix_cfg_name(id), _pmix_tbl.cfg.data[id]);
	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_dump(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int die_num = 0;
	char *node_name = "spi@51800000";
	void *addr;
	size_t len;

	if (argc < 3) {
		return CMD_RET_USAGE;
	}

	if (!strcmp(argv[1], "1")) {
		die_num = 1;
		node_name = "spi@71800000";
	}

	addr = (void *)simple_strtol(argv[2], NULL, 16);

	/* Load PMIX data */
	ret = d2d_pmix_load(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	pmix_table_try_upgrade(&_pmix_tbl);

	/* Verify PMIX data */
	ret = d2d_pmix_verify(&_pmix_tbl);
	if (ret != CMD_RET_SUCCESS) {
		printf("  Error: PMIX data is invalid (err=%d)\n", ret);
		return ret;
	}

	len = sizeof(struct pmix_lookup_table);
	memcpy(addr, &_pmix_tbl, len);

	env_set_hex("fileaddr", (ulong)addr);
	env_set_hex("filesize", len);

	printf("Die %d PMIX data dumped to %p (length=0x%lx) successfully.\n", die_num, addr, len);
	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_update(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int die_num = 0;
	char *node_name = "spi@51800000";
	void *addr;
	size_t len;
	struct pmix_lookup_table *tbl;

	if (argc < 3) {
		return CMD_RET_USAGE;
	}

	if (!strcmp(argv[1], "1")) {
		die_num = 1;
		node_name = "spi@71800000";
	}

	addr = (void *)simple_strtol(argv[2], NULL, 16);

	tbl = (struct pmix_lookup_table *)addr;
	len = sizeof(struct pmix_lookup_table);

	/* Verify PMIX data */
	ret = d2d_pmix_verify(tbl);
	if (ret != CMD_RET_SUCCESS) {
		printf("  Error: PMIX data is invalid (err=%d)\n", ret);
		return ret;
	}

	/* Store PMIX data */
	ret = d2d_pmix_store(node_name, tbl);
	if (ret) {
		printf("  Error: Failed to load PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	printf("Die %d PMIX data updated from %p (length=0x%lx) successfully.\n", die_num, addr, len);
	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_upgrade(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int die_num = 0;
	char *node_name = "spi@51800000";
	bool upgraded;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (!strcmp(argv[1], "1")) {
		die_num = 1;
		node_name = "spi@71800000";
	}

	printf("Die %d PMIX data upgrading ...\n", die_num);

	/* Load PMIX data */
	ret = d2d_pmix_load(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	upgraded = pmix_table_try_upgrade(&_pmix_tbl);

	/* Verify PMIX data */
	ret = d2d_pmix_verify(&_pmix_tbl);
	if (ret != CMD_RET_SUCCESS) {
		printf("  Error: PMIX data is invalid (err=%d)\n", ret);
		return ret;
	}

	if (!upgraded) {
		printf("Die %d PMIX data is already upgraded.\n", die_num);
		return CMD_RET_SUCCESS;
	}

	ret = d2d_pmix_store(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to store PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	printf("Die %d PMIX data upgraded successfully.\n", die_num);
	return CMD_RET_SUCCESS;
}

static int do_d2d_pmix_downgrade(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	int ret;
	int die_num = 0;
	char *node_name = "spi@51800000";
	bool upgraded;

	if (argc < 2)
		return CMD_RET_USAGE;

	if (!strcmp(argv[1], "1")) {
		die_num = 1;
		node_name = "spi@71800000";
	}

	printf("Die %d PMIX data downgrading ...\n", die_num);

	/* Load PMIX data */
	ret = d2d_pmix_load(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to load PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	upgraded = pmix_table_try_upgrade(&_pmix_tbl);

	/* Verify PMIX data */
	ret = d2d_pmix_verify(&_pmix_tbl);
	if (ret != CMD_RET_SUCCESS) {
		printf("  Error: PMIX data is invalid (err=%d)\n", ret);
		return ret;
	}

	if (upgraded) {
		printf("Die %d PMIX data is already downgraded.\n", die_num);
		return CMD_RET_SUCCESS;
	}

	if (!pmix_table_try_downgrade(&_pmix_tbl)) {
		printf("  Error: Failed to downgrade PMIX data\n");
		return CMD_RET_FAILURE;
	}

	ret = d2d_pmix_store(node_name, &_pmix_tbl);
	if (ret) {
		printf("  Error: Failed to store PMIX data (err=%d)\n", ret);
		return CMD_RET_FAILURE;
	}

	printf("Die %d PMIX data downgraded successfully.\n", die_num);
	return CMD_RET_SUCCESS;
}

/* Extend subcommand list */
static struct cmd_tbl d2d_sub[] = {
	U_BOOT_CMD_MKENT(pmix invalid, 2, 0, do_d2d_pmix_invalid, "", ""),
	U_BOOT_CMD_MKENT(pmix validate, 1, 0, do_d2d_pmix_validate, "", ""),
	U_BOOT_CMD_MKENT(get low-temperature, 1, 0, do_d2d_pmix_get_low_temp, "", ""),
	U_BOOT_CMD_MKENT(get curr-temperature, 1, 0, do_d2d_pmix_get_curr_temp, "", ""),
	U_BOOT_CMD_MKENT(get mode, 1, 0, do_d2d_pmix_get_mode, "", ""),
	U_BOOT_CMD_MKENT(pmix show, 2, 0, do_d2d_pmix_show, "", ""),
	U_BOOT_CMD_MKENT(pmix prune, 4, 0, do_d2d_pmix_prune, "", ""),
	U_BOOT_CMD_MKENT(pmix cfg, 4, 0, do_d2d_pmix_cfg, "", ""),
	U_BOOT_CMD_MKENT(pmix dump, 3, 0, do_d2d_pmix_dump, "", ""),
	U_BOOT_CMD_MKENT(pmix update, 3, 0, do_d2d_pmix_update, "", ""),
	U_BOOT_CMD_MKENT(pmix upgrade, 2, 0, do_d2d_pmix_upgrade, "", ""),
	U_BOOT_CMD_MKENT(pmix downgrade, 2, 0, do_d2d_pmix_downgrade, "", ""),
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
	6,		/* Increase max argument count to 6 (d2d + pmix + subcommand) */
	0,		/* Repeatable flag */
	do_d2d,	/* Handler function */
	"D2D subsystem commands",	/* Short help text */
	"pmix invalid [die_num] - Invalidate D2D PMIX Data for `die_num`.\n"
	"				Invalidate for both dies if `die_num` is not provided.\n"
	"d2d pmix validate - Validate PMIX data integrity\n"
	"d2d get low-temperature - Get PMIX data start temperature\n"
	"d2d get curr-temperature - Get current PVT temperature\n"
	"d2d get mode - Get D2D operating mode\n"
	"d2d pmix show [die_num] - Show D2D PMIX Data for `die_num`\n"
	"d2d pmix prune die_num min_degree max_degree - Prune D2D PMIX Data using\n"
	"				`min_degree` and `max_degree` for `die_num`\n"
	"d2d pmix cfg die_num [index] [value] - Set D2D PMIX Config `index` to `value`.\n"
	"				for `die_num`. Display only if `value` is not provided.\n"
	"d2d pmix dump die_num addr - Dump D2D PMIX Data to `addr` for `die_num`.\n"
	"d2d pmix update die_num addr - Update D2D PMIX Data from `addr` for `die_num`.\n"
	"d2d pmix upgrade die_num - Upgrade D2D PMIX Data for `die_num`.\n"
	"d2d pmix downgrade die_num - Downgrade D2D PMIX Data for `die_num`.\n"
	);
