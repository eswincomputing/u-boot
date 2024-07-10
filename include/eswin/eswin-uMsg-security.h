// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef INCLUDE_ESWIN_EIC7700_UMSG_SECURITY_H_
#define INCLUDE_ESWIN_EIC7700_UMSG_SECURITY_H_

/////////////////////////////////  SERVICE DATA DEFINITION  ////////////////////////////////////
#define RSA1024_PUBLICKEY_LEN 128
#define RSA1024_PRIKEY_LEN 128
#define RSA2048_PUBLICKEY_LEN 256
#define RSA2048_PRIKEY_LEN 256
#define RSA4096_PUBLICKEY_LEN 512
#define RSA4096_PRIKEY_LEN 512
#define RSA_PRIKEY_LEN 512
#define ECC_PUBLICKEY_LEN 32

// RSA1024 public key structure
typedef struct __RSA1024_PUBKEY_ST
{
  u32 keylen;
  u32 exponent;
  u8 mod[RSA1024_PUBLICKEY_LEN];
} RSA1024_PUBKEY_T;

// RSA2048 public key structure
typedef struct __RSA2048_PUBKEY_ST
{
  u32 keylen;
  u32 exponent;
  u8 mod[RSA2048_PUBLICKEY_LEN];
} RSA2048_PUBKEY_T;

// RSA4096 public key structure
typedef struct __RSA4096_PUBKEY_ST
{
  u32 keylen;
  u32 exponent;
  u8 mod[RSA4096_PUBLICKEY_LEN];
} RSA4096_PUBKEY_T;

// RSA1024 private key structure
typedef struct __RSA1024_PRIVKEY_ST
{
    u32 keylen;
    u8 d[RSA1024_PRIKEY_LEN];
    u8 mod[RSA1024_PRIKEY_LEN];
} RSA1024_PRIVKEY_T;

// RSA2048 private key structure
typedef struct __RSA2048_PRIVKEY_ST
{
    u32 keylen;
    u8 d[RSA2048_PRIKEY_LEN];
    u8 mod[RSA2048_PRIKEY_LEN];
} RSA2048_PRIVKEY_T;

// RSA4096 private key structure
typedef struct __RSA4096_PRIVKEY_ST
{
    u32 keylen;
    u8 d[RSA4096_PRIKEY_LEN];
    u8 mod[RSA4096_PRIKEY_LEN];
} RSA4096_PRIVKEY_T;

// RSA public key structure
typedef struct __RSA_PUBKEY_ST
{
  u32 keylen;
  u32 exponent;
  u8 mod[RSA4096_PUBLICKEY_LEN];
} RSA_PUBKEY_T;

// RSA private key structure
typedef struct __RSA_PRIVKEY_ST
{
  u32 keylen;
  u8 d[RSA4096_PUBLICKEY_LEN];
  u8 mod[RSA4096_PUBLICKEY_LEN];
} RSA_PRIVKEY_T;

typedef enum
{
  PKA_SW_CURVE_NIST_P256 = 0,
  PKA_SW_CURVE_NIST_P384,
  PKA_SW_CURVE_NIST_P512,
  PKA_SW_CURVE_BRAINPOOL_P256R1,
  PKA_SW_CURVE_BRAINPOOL_P384R1,
  PKA_SW_CURVE_BRAINPOOL_P512R1,
} ecc_curve_t;

/**
 * @}
 */

/**
 * @{
 *  \ingroup A_PKLibDefs
 *
 * \details
 * ECC curve sizes in bytes used in the PKA APIs
 *
 */
enum eccCurveByteSize {
    PKA_CURVE_NIST_P256_BYTE = 32, ///<  32 bytes.
    PKA_CURVE_NIST_P384_BYTE = 48, ///<  48 bytes
    PKA_CURVE_NIST_P521_BYTE = 66, ///<  66 bytes
    PKA_CURVE_BRAINPOOL_P256R1_BYTE = 32, ///<  32 bytes
    PKA_CURVE_BRAINPOOL_P384R1_BYTE = 48, ///<  48 bytes
    PKA_CURVE_BRAINPOOL_P512R1_BYTE = 64, ///<  64 bytes
    PKA_CURVE_SM2_SCA256_BYTE = 32, ///<  32 bytes
    PKA_CURVE_ED25519_BYTE = 32, ///<  32 bytes
    PKA_CURVE_CURVE25519_BYTE = 32 ///<  32 bytes
};
/**
 * @}
 */
/**
 * @{
 *  \ingroup A_PKLibDefs
 *
 * \details
 * ECC curve sizes used in the PKA APIs
 *
 */
static const u8 pka_curve_size[10] = { PKA_CURVE_NIST_P256_BYTE, //
                                                       PKA_CURVE_NIST_P384_BYTE, //
                                                       PKA_CURVE_NIST_P521_BYTE, //
                                                       PKA_CURVE_BRAINPOOL_P256R1_BYTE, //
                                                       PKA_CURVE_BRAINPOOL_P384R1_BYTE, //
                                                       PKA_CURVE_BRAINPOOL_P512R1_BYTE, //
                                                       PKA_CURVE_SM2_SCA256_BYTE, //
                                                       PKA_CURVE_ED25519_BYTE, //
                                                       PKA_CURVE_CURVE25519_BYTE }; //

// ECC public key structure
typedef struct __ECC_PUBKEY_ST
{
  u32 keylen;
  u8 x[ECC_PUBLICKEY_LEN];
  u8 y[ECC_PUBLICKEY_LEN];
} ECC_PUBKEY_T;

typedef struct __ECC_PRIVKEY_ST
{
  u32 keylen;
  u8 z[ECC_PUBLICKEY_LEN];
} ECC_PRIVKEY_T;

typedef struct
{
  ECC_PUBKEY_T ecc_pubk;
  ECC_PRIVKEY_T ecc_privk;
} BRT_ECCKEY_T;

typedef union
{
  RSA2048_PUBKEY_T rsa2048_pubkey;
  RSA4096_PUBKEY_T rsa4096_pubkey;
  ECC_PUBKEY_T ecc_pubkey;
} PUBKEY_T;


/************************************************************************
 * ECDH Key Definitions
 ************************************************************************/
typedef struct __ECDH_KEY_ST
{
    u32 keylen;
    u8 *x;
    u8 *y;
    u8 *z;
} ecdh_key_t;
/* ECDH public key structure */
typedef struct __ECDH_PUBKEY_ST
{
    u32 keylen;
    u8 *x;
    u8 *y;
} ecdh_pubk_t;

typedef struct __ECDH_PRIVKEY_ST
{
    u32 keylen;
    u8 *z;
} ecdh_privk_t;

typedef struct
{
    ecdh_pubk_t ecdh_pubk;
    ecdh_privk_t ecdh_privk;
} ECDHKEY_T;

typedef struct
{
    ecc_curve_t ecdh_curve;
    ECDHKEY_T ecdh_key;
} ECDH_CURVE_KEY_T;

typedef enum
{
  CRYPTO_MODE_NULL,
  CRYPTO_MODE_AES_ECB = 0x4,
  CRYPTO_MODE_AES_CBC,
  CRYPTO_MODE_AES_CTR,
  CRYPTO_MODE_AES_CCM,
  CRYPTO_MODE_AES_GCM,
  CRYPTO_MODE_AES_F8,
  CRYPTO_MODE_AES_XTS,
  CRYPTO_MODE_AES_CFB,
  CRYPTO_MODE_AES_OFB,
  CRYPTO_MODE_AES_CS1,
  CRYPTO_MODE_AES_CS2,
  CRYPTO_MODE_AES_CS3,
  CRYPTO_MODE_MULTI2_ECB,
  CRYPTO_MODE_MULTI2_CBC,
  CRYPTO_MODE_MULTI2_OFB,
  CRYPTO_MODE_MULTI2_CFB,
  CRYPTO_MODE_3DES_CBC = 0x14,
  CRYPTO_MODE_3DES_ECB,
  CRYPTO_MODE_DES_CBC,
  CRYPTO_MODE_DES_ECB,
  CRYPTO_MODE_SM4_ECB = 0x1e,
  CRYPTO_MODE_SM4_CBC,
  CRYPTO_MODE_SM4_CFB,
  CRYPTO_MODE_SM4_OFB,
  CRYPTO_MODE_SM4_CTR,
  CRYPTO_MODE_SM4_CCM,
  CRYPTO_MODE_SM4_GCM,
  CRYPTO_MODE_SM4_F8,
  CRYPTO_MODE_SM4_XTS,
  CRYPTO_MODE_SM4_CS1,
  CRYPTO_MODE_SM4_CS2,
  CRYPTO_MODE_SM4_CS3,

  CRYPTO_MODE_HASH_MD5 = 42,
  CRYPTO_MODE_HMAC_MD5,
  CRYPTO_MODE_HASH_SHA1,
  CRYPTO_MODE_HMAC_SHA1,
  CRYPTO_MODE_HASH_SHA224,
  CRYPTO_MODE_HMAC_SHA224,
  CRYPTO_MODE_HASH_SHA256,
  CRYPTO_MODE_HMAC_SHA256,
  CRYPTO_MODE_HASH_SHA384,
  CRYPTO_MODE_HMAC_SHA384,
  CRYPTO_MODE_HASH_SHA512,
  CRYPTO_MODE_HMAC_SHA512,
  CRYPTO_MODE_HASH_SHA512_224,
  CRYPTO_MODE_HMAC_SHA512_224,
  CRYPTO_MODE_HASH_SHA512_256,
  CRYPTO_MODE_HMAC_SHA512_256,
  CRYPTO_MODE_MAC_XCBC,
  CRYPTO_MODE_MAC_CMAC,
  CRYPTO_MODE_HASH_CRC32 = 66,
  CRYPTO_MODE_HASH_SM3 = 80,
  CRYPTO_MODE_HMAC_SM3,
  CRYPTO_MODE_MAC_SM4_XCBC,
  CRYPTO_MODE_MAC_SM4_CMAC,
} CRYPTO_MODE_T;

typedef enum
{
    OTP_KEY = 0,
    ROM_KEY_1,
    ROM_KEY_2,
    ROM_KEY_3,
    ROM_KEY_4,
    ROM_KEY_5,
    ROM_KEY_6,
    ROM_KEY_7,
    RAM_KEY_1,
    RAM_KEY_2,
    RAM_KEY_3,
    RAM_KEY_4,
    RAM_KEY_5,
    RAM_KEY_6,
    RAM_KEY_7,
    RAM_KEY_8,
    EXT_PRIV_KEY = 254,
    EXT_PUB_KEY,
} ASYMM_KEY_TYPE;

/* ASYMM algo type */
typedef enum
{
    ASYMM_TYPE_RSA1024 = 0,
    ASYMM_TYPE_RSA2048,
    ASYMM_TYPE_RSA4096,
    ASYMM_TYPE_ECC,
} ASYMM_ALGO_TYPE;

enum{
    OPT_GET_KEY,
    OPT_UPDATE_GET_KEY
};

#endif /* INCLUDE_ESWIN_EIC7700_UMSG_SECURITY_H_ */
