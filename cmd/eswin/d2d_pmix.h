// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN D2D PMIX layout
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
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
 * Authors: Jun Xie <xiejun@eswincomputing.com>
 */
#ifndef _D2D_PMIX_H_
#define _D2D_PMIX_H_

#ifdef CONFIG_VPU_LAYOUT
#define __ES_VPU_LAYOUT__
#endif

#define static_assert(expr, ...) __static_assert(expr, ##__VA_ARGS__, #expr)
#define __static_assert(expr, msg, ...) _Static_assert(expr, msg)

#define PMIX_MAGIC 0x504d4958
#ifdef __ES_VPU_LAYOUT__
#define PMIX_RECORD_ADDR                0xe80000     /* 0xe80000 - 0xe8ffff (64KiB) */
#else
#define PMIX_RECORD_ADDR                0x40000     /* 0x40000 - 0x4ffff (64KiB) */
#endif
#define PMIX_RECORD_MAX_SIZE            (64*1024)

#define PMIX_LEGACY_VERSION 0x4
#define PMIX_LEGACY_UPGRADED_MAJOR_VERSION 0x5
#define PMIX_LEGACY_UPGRADED_MINOR_VERSION 0x0
#define PMIX_LEGACY_UPGRADED_VERSION (PMIX_LEGACY_UPGRADED_MAJOR_VERSION | (PMIX_LEGACY_UPGRADED_MINOR_VERSION << 16))

#define PMIX_MAJOR_VERSION 0x5
#define PMIX_MINOR_VERSION 0x1
#define PMIX_VERSION (PMIX_MAJOR_VERSION | (PMIX_MINOR_VERSION << 16))

#define GET_PMIX_MAJOR_VERSION(version) ((version) & 0xffff)
#define GET_PMIX_MINOR_VERSION(version) (((version) >> 16) & 0xffff)

struct One_pmix {
	uint8_t phase0;
	uint8_t phase90;
	uint8_t phase180;
	uint8_t phase270;
	uint8_t width;
};

struct pmix_entry {
	struct One_pmix pmix[8];
	uint16_t temperature;
	uint8_t valid;
	uint8_t reserved;
};

struct pmix_cfg {
	union {
		struct {
			uint8_t polarity_inverted;
			uint8_t sweep_method;
			uint8_t debug_print_enabled;
			uint8_t fit_valid_degree;
			uint8_t reserved[28];
		};
		uint8_t data[32];
	};
};

struct fit_coeffs {
	double c0;
	double c1;
};

struct pmix_fit_params {
	struct fit_coeffs phase0_coeffs;
	struct fit_coeffs phase90_coeffs;
	struct fit_coeffs phase180_coeffs;
	struct fit_coeffs phase270_coeffs;
};

#define PMIX_LEGACY_ENTRY_NUM 350
struct pmix_lookup_table_legacy {
	uint32_t magic;
	uint32_t version;
	struct pmix_cfg cfg;
	struct pmix_fit_params fit_params[8];
	struct pmix_entry pmix_list[PMIX_LEGACY_ENTRY_NUM];
	uint32_t valid_cnt;
	uint32_t crc;
};
static_assert(sizeof(struct pmix_lookup_table_legacy) <= PMIX_RECORD_MAX_SIZE);

#define PMIX_ENTRY_NUM 600
struct pmix_lookup_table {
	uint32_t magic;
	uint32_t version;
	struct pmix_cfg cfg;
	struct pmix_fit_params fit_params[8];
	struct pmix_entry pmix_list[PMIX_ENTRY_NUM];
	uint32_t valid_cnt;
	uint32_t crc;
};
static_assert(sizeof(struct pmix_lookup_table) <= PMIX_RECORD_MAX_SIZE);

#endif /* end of include guard: _D2D_PMIX_H_ */
