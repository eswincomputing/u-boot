// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef _ESWIN_U_MSG_H
#define _ESWIN_U_MSG_H

#include <common.h>
#include <eswin/0ctr.h>
#include "eswin/eswin-uMsg-security.h"


#define MAX_IPC_DATA_BUFF_LEN           256

#define IPC_ADDR_LEN                    4

#define FRAME_HEAD_U84                  0xBB
#define FRAME_HEAD_E21                  0xCC

#define R_DATA_LEN                      (IPC_ADDR_LEN + 3)

typedef struct {
    u8 bFrameHead;
    u8 bFrameLen;
    u8 bIpcAddr[IPC_ADDR_LEN];
    u8 bChkRlt;
} REGISTER_DATA_T;

/* SERVICE TYPE DEFINITION */
typedef enum {
    SRVC_TYPE_SIGN_CHECK,               //0x00
    SRVC_TYPE_IMG_DECRPT,               //0x01
    SRVC_TYPE_FIRMWARE_DOWNLOAD,        //0x02
    SRVC_TYPE_PUBKEY_DOWNLOAD,          //0x03
    SRVC_TYPE_RSA_CRYPT_DECRYPT,        //0x04
    SRVC_TYPE_ECDH_KEY,                 //0x05
    SRVC_TYPE_SYM_CRYPT_DECRYPT,        //0x06
    SRVC_TYPE_DIGEST,                   //0x07
    SRVC_TYPE_HMAC,                     //0x08
    SRVC_TYPE_OTP_READ_PROGRAM,         //0x09
    SRVC_TYPE_TRNG,                     //0x0A
    SRVC_TYPE_ADDR_REGION_PROTECTION,   //0x0B
    SRVC_TYPE_DOWNLOADABLE,             //0x0C
    SRVC_TYPE_BASIC_IO,                 //0x0D
    SRVC_TYPE_AXPROT,                   //0x0E
    SRVC_TYPE_MAX
} SRVC_TYPE;

/* FLAGS DEFINITION */
typedef struct /*4.1.1*/
{
    // byte0:
    u8 bAlgorithm :2;
    u8 bReserved0 :6;
    // byte1:
    u8 bKeyId;
    // byte2:
    u8 bReserved1;
    // byte3:
    u8 bReserved2;
} FLAG1_T;

typedef struct
{
    // byte0:
    u8 bOpt : 1;
    u8 bReserved0 : 7;
    // byte1:
    u8 bReserved1;
    // byte2:
    u8 bReserved2;
    // byte3:
    u8 bReserved3;
} FLAG4_T;

typedef struct /*4.4.1*/
{
    //byte0:
    u8 bAlgorithm :2;
    u8 bReserved0 :6;
    //byte1:
    u8 bKeyId;
    //byte2:
    u8 bCrypto :1;
    u8 bReserved1 :7;
    //byte3:
    u8 bReserved2;
} FLAG5_T;

typedef struct __attribute__((packed, aligned(1)))
{
    // byte0:
    u8 bReserved0;
    // byte1:
    u8 bReserved1;
    // byte2:
    u8 bReserved2;
    // byte3:
    u8 opt;
} flag6_t;

typedef struct /*4.5.1*/
{
    //byte0:
    u8 bCrypToMode;
    //byte1:
    u8 bKeyId;
    //byte2:
    u8 bCrypto :1;
    u8 bReserved1 :7;
    //byte3:
    u8 bReserved2;
} FLAG7_T;

typedef struct /*4.8.1*/
{
    //byte0:
    u8 bCrypToMode;
    //byte1:
    u8 bReserved1;
    //byte2:
    u8 bReserved2;
    //byte3:
    u8 bReserved3;
} FLAG8_T;

typedef struct /*4.7.1*/
{
    //byte0:
    u8 bAlgorithm;
    //byte1:
    u8 bReserved1;
    //byte2:
    u8 bReserved2;
    //byte3:
    u8 bReserved3;
} FLAG9_T;

typedef struct /*4.8.1*/
{
    //byte0:
    u8 read_program:1;
    u8 bReserved0:3;
    u8 otp_enxor:1;
    u8 bReserved1:3;
    //byte1:
    u8 bReserved2;
    //byte2:
    u8 bReserved3;
    //byte3:
    u8 bReserved4;
} FLAG10_T;

typedef struct /*4.14.1*/
{
// byte0:
    u8 bAlgorithm :2;
    u8 bReserved0 :6;
// byte1:
    u8 bKeyId;
// byte2:
    u8 bReserved1;
// byte3:
    u8 bServiceType;
} FLAG14_T;


typedef struct /*4.15.1*/
{
//byte0:
    u8 bRw :2;
    u8 bReserved0 :6;
//byte1:
    u8 bDataWidth :2;
    u8 bReserved1 :6;
//byte2:
    u8 bReserved2;
//byte3:
    u8 bReserved3;
} FLAG15_T;

typedef struct /*4.16.1*/
{
//byte0:
    u8 bRw :2;
    u8 bReserved0 :6;
//byte1:
    u8 bReserved1;
//byte2:
    u8 bReserved2;
//byte3:
    u8 bReserved3;
} FLAG16_T;

/* MIXED DEFINITION */
#define MAX_SIGN_SIZE               256
#define MAX_PAY_LOAD_SIZE           4096
#define MAX_IMG_DATA_SIZE           (1024*1024*1024)
#define MAX_FLAG_LEN                8

typedef struct {
    FLAG1_T flag;
    u32 dwSignSize;
    u32 bSignAddr;
    u32 dwPayloadSize;
    u32 bPayloadAddr;
    u32 dwDestAddr;
} SignatureValidationCheck_REQ;

typedef struct {
    u32 dwSignSize;
    u32 bSignAddr;
} SignatureValidationCheck_RES;

typedef struct {
    FLAG1_T flag;
    u32 dwSignSize;
    u32 bSignAddr;
    u32 dwDataSize;
    u32 bDataAddr;
    u32 bDestAddr;
} ImageDecryption_REQ;

typedef struct {
    u32 dwSignSize;
    u32 bSignAddr;
    u32 dwDataSize;
    u32 bDataAddr;
} ImageDecryption_RES;

typedef struct {
    FLAG1_T flag;
    u32 dwSignSize;
    u32 bSignAddr;
    u32 dwImgSize;
    u32 bImgAddr;
} FirmwareDownload_REQ;

typedef struct {
    u32 dwRetVal;
} FirmwareDownload_RES;

typedef struct {
    FLAG1_T flag;
    u32 dwSignSize;
    u32 dwSignAddr;
    u32 dwKeySize;
    u32 dwKeyAddr;
} PubkeyDownload_REQ;

typedef union {
    RSA1024_PUBKEY_T Rsa1024PubKey;
    RSA1024_PRIVKEY_T Rsa1024PrvKey;
    RSA2048_PUBKEY_T Rsa2048PubKey;
    RSA2048_PRIVKEY_T Rsa2048PrvKey;
    RSA4096_PUBKEY_T Rsa4096PubKey;
    RSA4096_PRIVKEY_T Rsa4096PrvKey;
    RSA_PRIVKEY_T KeyRsaPrv;
} RSA_KEYSTRUCT_T;

typedef struct
{
    FLAG5_T flag;
    u32 dwDataSize;
    u32 dwDataAddr;
    u32 dwDestAddr;
    u32 KeyStruct_size;  //len=dwKeySize, <=RSA_ECC_KEY_LEN
    RSA_KEYSTRUCT_T tKey;
} RsaEncDec_REQ; //SRVC_TYPE_RSA_CRYPT_DECRYPT

typedef struct {
    u32 dwDataSize;
    u8 bDestAddr[IPC_ADDR_LEN];
} RsaEncDec_RES;

/* ECDH KEY */
typedef enum
{
    OPT_GEN,
    OPT_AGR,
    OPT_DRIV,
}ecdh_key_opt_t;
typedef struct
{
    u32 ecc_curve;
} ecdh_key_gen_t;
typedef struct
{
    u32 ecc_curve;
    ecdh_pubk_t pubk_remote;
    ecdh_privk_t privk_local;
} ecdh_key_agr_t;
typedef struct
{
    ecdh_pubk_t seck;
    u32 rand;
} ecdh_key_driv_t;
typedef union
{
    ecdh_key_gen_t gen;
    ecdh_key_agr_t agr;
    ecdh_key_driv_t driv;
}ecdh_key_data_t;
typedef struct __attribute__((packed, aligned(1)))
{
    flag6_t flag;
    ecdh_key_data_t data;
} ecdh_key_req_t;

typedef union __attribute__((packed, aligned(1))) {
    ecdh_key_t ecdh_key;
    ecdh_pubk_t secure_key;
    u8 sym_key[64];
} ecdh_key_res_t;

#define MAX_ECC_PUBKEY_LEN          (ECC_PUBLICKEY_LEN*2+4)
#define MAX_ECC_RAND_LEN            MAX_ECC_PUBKEY_LEN
typedef struct //4.6.1
{
    FLAG5_T flag;
    u32 dwDataSize;
    u32 dwDataAddr;
    u32 dwDestAddr;
    ecc_curve_t ecc_curve;
    u32 dwPubKeySize;
    ecdh_pubk_t tKey;
    u32 dwRandSize;
    u8 *pRand;
} EccEncDec_REQ;

typedef struct {
    u32 dwDataSize;
    u32 dwDestAddr;
} EccEncDec_RES;

#define AES_SM4_KEY_LEN 32
#define IV_LEN 32
typedef struct  // 4.7.1
{
    FLAG7_T flag;
    u32 dwDataSize;
    u32 bDataAddr;
    u32 bDestAddr;
    u32 dwKeySize;
    u8 bKey[AES_SM4_KEY_LEN]; //len= dwKeySize,<=AES_SM4_KEY_LEN
    u32 dwIvSize;
    u8 bIV[IV_LEN];
} AesSm4EncDec_REQ;

typedef struct {
    u32 dwDataSize;
    u32 bDestAddr;
} AesSm4EncDec_RES;

typedef struct  // 4.8.1
{
    FLAG8_T flag;
    u32 dwDataSize;
    u32 bDataAddr;
} Sha1Sha256Sm3Digest_REQ;

#define DIGEST_LEN 64
typedef struct {
    u32 dwDigestSize;
    u8 bDigest[DIGEST_LEN];
} Sha1Sha256Sm3Digest_RES;

#define HMAC_KEY_LEN 256
typedef struct {
    FLAG9_T flag;
    u32 dwDataSize;
    u32 bDataAddr;
    u32 dwKeySize;
    u8 bKey[HMAC_KEY_LEN];  //len=dwKeySize,<=HMAC_KEY_LEN
} HMAC_REQ;

#define MAX_HMAC_SIZE 64
typedef struct {
    u32 dwHmacSz;
    u8 bHmac[MAX_HMAC_SIZE];
} HMAC_RES;

#define MAX_OTP_DATA_LEN 920  //#define OTP_PUBLIC_CERTIFICATION_SIZE (522)

typedef struct {
    FLAG10_T flag;
    // u8 bOtpAddr[IPC_ADDR_LEN];
    u32 bOtpAddr;
    u32 dwDataSize;
    u32 bDataAddr;
    u32 bMaskAddr;
} OTP_REQ;

typedef struct {
    u32 dwDataSize;  // len=dwDataSize,<=MAX_OTP_DATA_LEN
    u8 bData[MAX_OTP_DATA_LEN];
} OTP_RES;

typedef struct {
    u32 dwFlag;
} TRNG_REQ;

typedef struct {
    // u32 bLen;
    u8 bData[0];
} TRNG_RES;
typedef struct
{
    u8 region;
    u32 addr_h;
    u32 addr_l;
    u32 config;
}AddrRegionProtection_REQ;

typedef struct {
    u8 region_id;
} AddrRegionProtection_RES;

typedef struct {
    FLAG14_T flag;
    u32 dwSignSize;
    u32 bSignAddr;
    u32 dwCodeSize;
    u32 bCodeAddr;
} DownloadableInit_REQ;

typedef struct {
    FLAG14_T flag;
    u32 ServiceId;
} DownloadableDestory_REQ;

typedef struct {
    FLAG14_T flag;
    u32 ServiceId;
    u32 UserCmd;
    u32 ParamSize;
    u8  Param[0];
} DownloadableIoctl_REQ;

/*typedef struct
 {
 }Downloadable_RES;*/

typedef struct {
    FLAG15_T flag;
    u32 dwAddr;
    u32 dwData;
    u32 dwMask;
} BasicIO_REQ;

typedef struct {
    u32 dwData;
} BasicIO_RES;

typedef struct {
    FLAG16_T flag;
    u32 dwMasterID;
    u32 dwConfig;
} Axprot_REQ;

typedef struct {
    u32 dwConfig;
} Axprot_RES;

typedef union {
    SignatureValidationCheck_REQ SigChkReq;
    ImageDecryption_REQ ImgDecReq;
    FirmwareDownload_REQ FirmDlReq;
    PubkeyDownload_REQ PubkeyDlReq;
    RsaEncDec_REQ RsaEncDecReq;
    ecdh_key_req_t ecdh_key_req;
    EccEncDec_REQ EccEncDecReq;
    AesSm4EncDec_REQ AesSm4EncDecReq;
    Sha1Sha256Sm3Digest_REQ DigestReq;
    HMAC_REQ HmacReq;
    OTP_REQ OtpReq;
    TRNG_REQ TrngReq;
    AddrRegionProtection_REQ AddrRegProReq;
    DownloadableInit_REQ DlInitReq;
    DownloadableDestory_REQ DlDestory;
    DownloadableIoctl_REQ DlIoctl;
    BasicIO_REQ BasicIoReq;
    Axprot_REQ AxprotReq;
} REQ_DATA_DOMAIN_T;

typedef union {
    SignatureValidationCheck_RES SigChkRes;
    ImageDecryption_RES ImgDecRes;
    FirmwareDownload_RES FirmDlRes;
    RsaEncDec_RES RsaEncDecRes;
    ecdh_key_res_t ecdh_key_res;
    AesSm4EncDec_RES AesSm4EncDecRes;
    Sha1Sha256Sm3Digest_RES DigestRes;
    HMAC_RES HmacRes;
    OTP_RES OtpRes;
    TRNG_RES TrngRes;
    AddrRegionProtection_RES AddrRegProRes;
    //Downloadable_RES               DlRes;
    BasicIO_RES BasicIoRes;
    Axprot_RES AxprotRes;
} RES_DATA_DOMAIN_T;

static const u32 req_pkt_size[SRVC_TYPE_MAX+2] = {
        sizeof(SignatureValidationCheck_REQ),
        sizeof(ImageDecryption_REQ),
        sizeof(FirmwareDownload_REQ),
        sizeof(PubkeyDownload_REQ),
        sizeof(RsaEncDec_REQ),
        sizeof(ecdh_key_req_t),
        sizeof(AesSm4EncDec_REQ),
        sizeof(Sha1Sha256Sm3Digest_REQ),
        sizeof(HMAC_REQ),
        sizeof(OTP_REQ),
        sizeof(TRNG_REQ),
        sizeof(AddrRegionProtection_REQ),
        sizeof(DownloadableInit_REQ),
        sizeof(BasicIO_REQ),
        sizeof(Axprot_REQ),
        sizeof(DownloadableDestory_REQ),
        sizeof(DownloadableIoctl_REQ)
};

typedef struct {
    u32 dwFrameHead; /*Fix value: 0xBB4565A6*/
    u32 dwNum;
    SRVC_TYPE dwServiceID;
    u32 dwDataLen;  // dwFrameLen
    uint8_t *pData;
} IPC_REQ_T;

typedef struct {
    u32 dwFrameHead; /*Fix value: 0xCC4578B5*/
    u32 dwNum;
    SRVC_TYPE dwServiceID;
    u32 IpcStatus;
    u32 ServiceStatus;
    u32 dwDataLen;  // dwFrameLen;
    uint8_t *pData;
} IPC_RES_T;
typedef struct {
    SRVC_TYPE SrvcType;
    REQ_DATA_DOMAIN_T data;
} REQ_SRVC_T;

#endif
