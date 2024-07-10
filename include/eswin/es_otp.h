// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

/*
 *  Include files
 */
#ifndef __ES_OTP_H__
#define __ES_OTP_H__

#include <asm/types.h>
#include "es_otp_map.h"

#define ERR_OK 0
#define ERR_OTP  -1
#define ERR_OTP_LEN (ERR_OTP + 2)

#define DEV_MODE     0x00
#define DEV_MODE_BIT 0x01

typedef struct {
	u8 en_hevc_encode : 1;
	u8 en_hevc_decode : 1;
	u8 en_avc_encode  : 1;
	u8 en_avc_decode  : 1;
	u8 reserved0      : 4;

	u8 reserved1;
	u8 reserved2;
	u8 reserved3;
} FUNC_FEATURE_LIST_T;

typedef struct {
	u8 size_rsakey  : 1;
	u8 size_m2mkey  : 1;
	u8 size_bootkey : 1;
	u8 whl_memkey   : 1;
	u8 ecdsa_key    : 1;
	u8 hdcp_key     : 1;
	u8 dbg_pwd      : 1;
	u8 scpu_fw_key  : 1;

	u8 reserved1;
	u8 reserved2;
	u8 reserved3;
} SEC_FEATURE_LIST_T;

typedef struct {
	FUNC_FEATURE_LIST_T func_list;
	SEC_FEATURE_LIST_T  sec_list;
	u8 dev_mode;
} OTP_FEATURE_T;

#ifndef SWAP
/**
 * @def SWAP
 * @brief Swap two variables.
 */
#define SWAP(a, b)  \
    {               \
        (a) ^= b;   \
        (b) ^= (a); \
        (a) ^= (b); \
    }
#endif /* SWAP */

extern OTP_FEATURE_T otp_feature;

extern const struct OTP_MAP_ITEM otp_map[];

extern u32 otp_read32(u32 index);
extern void otp_write32(u32 index, u32 value);

extern s32 otp_read_data(u8 *data, u32 index, u32 size, u8 enxor);
extern s32 otp_write_data(u32 addr, u8 data, u8 bit_mask);
extern s32 get_otp_value(u8 *data, s32 otp_id, u32 size, u8 enxor);
extern s32 otp_init(void);
extern s32 otp_pgmlock(void);
extern s32 otp_read_status(void);

#endif /*__ES_OTP_H__*/
