// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef __ESWIN_SERVICE_H__
#define __ESWIN_SERVICE_H__

/************************************************************************
 *  SIGN CHECK definition
 ************************************************************************/
/* signature type */
typedef enum
{
    SIGN_TYPE_PLAINTEXT = 0,
    SIGN_TYPE_RSA2048,
    SIGN_TYPE_RSA4096,
    SIGN_TYPE_ECDSA,
} SIGN_ALGO_TYPE;

/* crypto type */
typedef enum
{
    CRYPTO_TYPE_RSA1024 = 0,
    CRYPTO_TYPE_RSA2048,
    CRYPTO_TYPE_RSA4096,
    CRYPTO_TYPE_ECDSA,
} CRYPTO_ALGO_TYPE;

typedef enum
{
    OP_ENCRYPT = 0,
    OP_DECRYPT
}CRYPTO_TYPE;
/************************************************************************
 *  BASIC IO definition
 ************************************************************************/
typedef enum
{
    BASIC_IO_READ,
    BASIC_IO_WRITE,
    BASIC_IO_SET,
    BASIC_IO_CLEAR
}BASIC_IO_RW_T;

typedef enum
{
    IO_WIDTH_8,
    IO_WIDTH_16,
    IO_WIDTH_32
}BASIC_IO_WIDTH_T;
#endif
