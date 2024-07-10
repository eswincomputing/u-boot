// SPDX-License-Identifier: GPL-2.0
/*
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
 * Authors: Huangyifeng <huangifeng@eswincomputing.com>
 */

#include <asm/io.h>
#include <clk.h>
#include <common.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <eswin/ipc.h>
#include <eswin/eswin-service.h>
#include <eswin/eswin-uMsg.h>
#include <eswin/eswin-umbox-srvc.h>
#include <eswin/eswin-umbox.h>
#include <linux/bitops.h>
#include <linux/bug.h>
#include <log.h>
#include <mailbox-uclass.h>
#include <malloc.h>


int Encode_RsaEccEncDec_REQ(RsaEncDec_REQ *pIn, u8 *pOut)
{
    int len = 0;
    int headerlen = 0;
    if (NULL == pIn || NULL == pOut)
        return len;

    if(pIn->flag.bKeyId < EXT_PRIV_KEY)
        headerlen = 0;
    else if(pIn->flag.bKeyId == EXT_PRIV_KEY)
        {
            headerlen = sizeof(pIn->tKey.KeyRsaPrv.keylen) + 2*pIn->tKey.KeyRsaPrv.keylen;
        }
    else
        {
            headerlen = sizeof(pIn->tKey.Rsa2048PubKey.keylen) + sizeof(pIn->tKey.Rsa2048PubKey.exponent) + pIn->tKey.Rsa2048PubKey.keylen;
        }

    len = 20 + headerlen;

    memcpy(pOut, (u8*) pIn, len);
    return len;
}

int encode_ecdh_key_req(ecdh_key_req_t *in, u8 *out)
{
    int len = 0;
    u32 keylen = 0;

    if (NULL == in || NULL == out)
        return len;

    if(in->flag.opt == OPT_GEN){
        len = sizeof(in->flag)+sizeof(ecdh_key_gen_t);
        memcpy(out, (u8*)in, len);
    }
    else if(in->flag.opt == OPT_AGR){
        len = sizeof(in->flag) + sizeof(u32);
        memcpy(out, (u8*)in, len);

        keylen = in->data.agr.pubk_remote.keylen;
        memcpy(out+len, (u8*)&in->data.agr.pubk_remote.keylen, sizeof(u32));
        len += sizeof(u32);

        memcpy(out+len, (u8*)in->data.agr.pubk_remote.x, keylen);
        len += keylen;

        memcpy(out+len, (u8*)in->data.agr.pubk_remote.y, keylen);
        len += keylen;

        keylen = in->data.agr.privk_local.keylen;
        memcpy(out+len, (u8*)&in->data.agr.privk_local.keylen, sizeof(u32));
        len += sizeof(u32);

        memcpy(out+len, (u8*)in->data.agr.privk_local.z, keylen);
        len += keylen;
    }
    else if(in->flag.opt == OPT_DRIV){
        len = sizeof(in->flag);
        memcpy(out, (u8*)in, len);

        keylen = in->data.driv.seck.keylen;
        memcpy(out+len, (u8*)&in->data.driv.seck.keylen, sizeof(u32));
        len += sizeof(u32);

        memcpy(out+len, (u8*)in->data.driv.seck.x, keylen);
        len += keylen;

        memcpy(out+len, (u8*)in->data.driv.seck.y, keylen);
        len += keylen;

        memcpy(out+len, (u8*)&in->data.driv.rand, 4);
        len += 4;
    }
    else{
        len = 0;
    }

    return len;
}

int Encode_AesSm4EncDec_REQ(AesSm4EncDec_REQ *pIn, u8 *pOut)
{
    int len = 0;
    if (NULL == pIn || NULL == pOut)
        return len;
    len = 20;
    memcpy(pOut, (u8 *) pIn, len);
    memcpy(pOut + len, (u8 *) pIn->bKey, pIn->dwKeySize);
    len += pIn->dwKeySize;
    memcpy(pOut + len, (u8 *) &pIn->dwIvSize, 4);
    len += 4;
    memcpy(pOut + len, (u8 *) pIn->bIV, pIn->dwIvSize);
    len += pIn->dwIvSize;

    printk("AesSm4EncDec_REQ size=%d\r\n", len);
    return len;
}

int Encode_HMAC_REQ(HMAC_REQ *pIn, u8 *pOut)
{
    int len = 0;
    if (NULL == pIn || NULL == pOut)
        return len;
    len = 16 + pIn->dwKeySize;;
    memcpy(pOut, (u8*) pIn, len);
    return len;
}


int Encode_OTP_REQ(OTP_REQ *pIn, u8 *pOut)
{
    int len;
    int totalLen = 0;
    u8 *pCursor;
    if (NULL == pIn || NULL == pOut)
        return totalLen;
    pCursor = (u8*) pIn;

    if(pIn->flag.read_program == 0)
    {
        len = sizeof(pIn->flag) + IPC_ADDR_LEN + sizeof(pIn->dwDataSize);
        memcpy(pOut, pCursor, len);

        pOut += len;
        totalLen += len;
        return totalLen;
    }
    else
    {
        len = sizeof(pIn->flag) + IPC_ADDR_LEN + sizeof(pIn->dwDataSize);
        memcpy(pOut, pCursor, len);

        pOut += len;
        totalLen += len;
        len = pIn->dwDataSize;

        if (len > MAX_OTP_DATA_LEN)
        {
            printk("eswin-ipc: Encode_OTP_REQ "
                    "dwDataSize(%d)>MAX_OTP_DATA_LEN error.\r\n", len);
            return totalLen;
        }

        pCursor = (u8*)(uintptr_t)pIn->bDataAddr;
        memcpy(pOut, pCursor, len);

        pOut += len;
        totalLen += len;
        pCursor = (u8*)(uintptr_t)pIn->bMaskAddr;
        memcpy(pOut, pCursor, len);

        totalLen += len;

        return totalLen;
    }
}

int Encode_EccEncDec_REQ(EccEncDec_REQ *pIn, u8 *pOut)
{
    int len = 0;
    if (NULL == pIn || NULL == pOut)
        return len;

    memcpy(pOut, (u8*) pIn, 24);
    memcpy(pOut + 24, (u8*) &pIn->tKey.keylen, 4);
    memcpy(pOut + 24 + 4, (u8*) &pIn->tKey.x[0], pIn->tKey.keylen);
    memcpy(pOut + 24 + 4 + pIn->tKey.keylen, (u8*) &pIn->tKey.y[0], pIn->tKey.keylen);
    memcpy(pOut + 24 + 4 + 2*pIn->tKey.keylen, (u8*) &pIn->dwRandSize, 4);
    memcpy(pOut + 24 + 4 + 2*pIn->tKey.keylen + 4, (u8*) pIn->pRand, pIn->dwRandSize);
    return 24 + pIn->dwPubKeySize + 4 + pIn->dwRandSize;
}

int Encode_Downloadable_REQ(DownloadableInit_REQ *pIn, u8 *pOut)
{
    int len;
    FLAG14_T *flag = (FLAG14_T*)&pIn[0];
    if(flag->bServiceType == 0)
    {
        len = sizeof(DownloadableInit_REQ);
    }
    else if(flag->bServiceType == 1)
    {
        len = sizeof(DownloadableDestory_REQ);
    }
    else if(flag->bServiceType == 2)
    {
        DownloadableIoctl_REQ* Ioctl_req = (DownloadableIoctl_REQ*)pIn;
        len = sizeof(DownloadableIoctl_REQ) + Ioctl_req->ParamSize;
    }

    memcpy((void*) pOut, (void*)pIn, len);
    return len;
}

int EncodeReqData(u8 *pOut, SRVC_TYPE SerType, REQ_DATA_DOMAIN_T *pIn)
{
    int len = 0;

    if (NULL == pOut || NULL == pIn)
        return len;

    switch (SerType)
    {
        case SRVC_TYPE_SIGN_CHECK:
        case SRVC_TYPE_IMG_DECRPT:
        case SRVC_TYPE_FIRMWARE_DOWNLOAD:
        case SRVC_TYPE_DIGEST:
        case SRVC_TYPE_TRNG:
        case SRVC_TYPE_ADDR_REGION_PROTECTION:
        case SRVC_TYPE_BASIC_IO:
        case SRVC_TYPE_AXPROT:
        case SRVC_TYPE_PUBKEY_DOWNLOAD:
        case SRVC_TYPE_OTP_READ_PROGRAM:
            len = req_pkt_size[SerType];
            memcpy((void*) pOut, (void*)pIn, len);
            break;
        case SRVC_TYPE_ECDH_KEY:
            len = encode_ecdh_key_req(&(pIn->ecdh_key_req), pOut);
            break;
        case SRVC_TYPE_DOWNLOADABLE:
            len = Encode_Downloadable_REQ(&(pIn->DlInitReq), pOut);
            break;
        case SRVC_TYPE_RSA_CRYPT_DECRYPT:
            len = Encode_RsaEccEncDec_REQ(&(pIn->RsaEncDecReq), pOut);
            break;
        case SRVC_TYPE_SYM_CRYPT_DECRYPT:
            len = Encode_AesSm4EncDec_REQ(&(pIn->AesSm4EncDecReq), pOut);
            break;
        case SRVC_TYPE_HMAC:
            len = Encode_HMAC_REQ(&(pIn->HmacReq), pOut);
            break;
        default:
            break;
    }
    return len;
}

int GetReqDataSize(u8 bSrvType, u8 *pData)
{
    int size = 0;
    switch (bSrvType)
    {
        case SRVC_TYPE_SIGN_CHECK:  //0x00
        case SRVC_TYPE_IMG_DECRPT:  //0x01--DATA_IMG_DECRYPTION_T
        case SRVC_TYPE_FIRMWARE_DOWNLOAD:  //0x02
        case SRVC_TYPE_TRNG:  //0x08--DATA_TRNG_T
        case SRVC_TYPE_ADDR_REGION_PROTECTION:  //0x0A
        case SRVC_TYPE_BASIC_IO:
        case SRVC_TYPE_AXPROT:
        case SRVC_TYPE_PUBKEY_DOWNLOAD:  // 0x03
        case SRVC_TYPE_OTP_READ_PROGRAM:
            size = req_pkt_size[bSrvType];
            break;
        case SRVC_TYPE_ECDH_KEY:
            {
                unsigned int keylen = 0;
                ecdh_key_req_t *in = (ecdh_key_req_t*)pData;
                if(in->flag.opt == OPT_GEN){
                    size = sizeof(in->flag)+sizeof(ecdh_key_gen_t);
                }
                else if(in->flag.opt == OPT_AGR){
                    size = sizeof(in->flag) + sizeof(u32); /* flag */

                    keylen = in->data.agr.pubk_remote.keylen;
                    size += 4 + 2*keylen; /* pubk size */

                    keylen = in->data.agr.privk_local.keylen;
                    size += 4 + keylen; /* privk size */
                }
                else if(in->flag.opt == OPT_DRIV){
                    size = sizeof(in->flag);

                    keylen = in->data.driv.seck.keylen;
                    size += sizeof(u32) + 2*keylen;

                    size += 4; /* rand */
                }
                else{
                    size = 0;
                }
            }
            break;
        case SRVC_TYPE_DOWNLOADABLE:
            {
                FLAG14_T *flag = (FLAG14_T*)&pData[0];
                if(flag->bServiceType == 0)
                {
                    size = req_pkt_size[bSrvType];
                }
                else if(flag->bServiceType == 1)
                {
                    size = sizeof(DownloadableDestory_REQ);
                }
                else if(flag->bServiceType == 2)
                {
                    DownloadableIoctl_REQ* Ioctl_req = (DownloadableIoctl_REQ*)pData;
                    size = sizeof(DownloadableIoctl_REQ) + Ioctl_req->ParamSize;
                }
            }
            break;
        case SRVC_TYPE_RSA_CRYPT_DECRYPT:  // 0x03
            {
                RsaEncDec_REQ *pReq;
                int keysize = 0;
                pReq = (RsaEncDec_REQ*) pData;
                if(pReq->flag.bKeyId < EXT_PRIV_KEY)
                    keysize = 0;
                else if(pReq->flag.bKeyId == EXT_PRIV_KEY)
                {
                    keysize = sizeof(pReq->tKey.KeyRsaPrv.keylen) + 2*pReq->tKey.KeyRsaPrv.keylen;
                }
                else
                {
                    keysize = sizeof(pReq->tKey.Rsa2048PubKey.keylen) + sizeof(pReq->tKey.Rsa2048PubKey.exponent) + pReq->tKey.Rsa2048PubKey.keylen;
                }

                size = 20 + keysize;
            }
            break;
        case SRVC_TYPE_SYM_CRYPT_DECRYPT:  // 0x04
            {
                AesSm4EncDec_REQ *pReq;
                pReq = (AesSm4EncDec_REQ*) pData;
                size = 24 + pReq->dwKeySize + pReq->dwIvSize;
            }
            break;
        case SRVC_TYPE_DIGEST:  // 0x06
            {
                Sha1Sha256Sm3Digest_REQ *pReq;
                pReq = (Sha1Sha256Sm3Digest_REQ*) pData;
                size = 12;
            }
            break;
        case SRVC_TYPE_HMAC:  // 0x06
            {
                HMAC_REQ *pReq;
                pReq = (HMAC_REQ*) pData;
                size = 16 + pReq->dwKeySize;
            }
            break;
        default:
            break;
    }

#if K_MAILBOX_DBG
    debug("GetReqDataSize: SRVC_TYPE[%d] size = [%d]!\r\n", bSrvType, size);
#endif

    if (0 != (size % 4))
    {
        if (SRVC_TYPE_OTP_READ_PROGRAM != bSrvType)
        {
            printk("GetReqDataSize: SRVC_TYPE[%d] size = [%d], not 4 "
                    "times, error!\r\n", bSrvType, size);
            return 0;
        }
    }

    return size;
}

int eswin_session_process_req(struct udevice *dev, MSG_SEND_T *msg_send)
{
    int ret = 0;
    struct eswin_umbox_srvc *sbmt = dev_get_priv(dev);

    if (WARN_ON(!msg_send))
        return -EINVAL;

    ret = mbox_send(&sbmt->chan, (void*) msg_send);  // call eswin_umbox_send
    if (ret < 0)
    {
        printk("eswin_session_process_req: send msg failed: %d\n", ret);
    }

    return ret;
}

int eswin_service(struct udevice *dev, uint8_t *data)
{
    int ret, len;
    MSG_SEND_T msg_send;
    REQ_SRVC_T *pSrvcReq = (REQ_SRVC_T*) data;

    debug("\neswin-ipc: eswin_service.\r\n");

    msg_send.srv_type = pSrvcReq->SrvcType;
    msg_send.size = GetReqDataSize(msg_send.srv_type, (u8*)(&pSrvcReq->data));  //unit: dw32
    if (0 >= msg_send.size)
    {
        return 1;
    }

    msg_send.pData = kmalloc(msg_send.size, GFP_ATOMIC);
    if (NULL == msg_send.pData)
    {
        printk("eswin_service: kmalloc %d failed in eswin_service.\r\n", msg_send.size);
        return 1;
    }

    len = EncodeReqData(msg_send.pData, msg_send.srv_type, (REQ_DATA_DOMAIN_T*) &pSrvcReq->data);
    if (len != msg_send.size)
    {
        printk("eswin_service: Encode SerType=%d ,len(=%d) != size(%d), error!\r\n", msg_send.srv_type, len, msg_send.size);
        kfree(msg_send.pData);
        return 2;
    }

    DbgPrintSerDataU84(&msg_send);

    ret = eswin_session_process_req(dev, &msg_send);
    if (ret)
    {
        kfree(msg_send.pData);
        printk("eswin_service: send service session command failed, ret %d\n", ret);
        return ret;
    }

    kfree(msg_send.pData);
    return 0;
}

int ServiceSessionSendReq(struct udevice *dev, REQ_SRVC_T *pReqData)
{
    int ret = 0;
    if (NULL == pReqData || NULL == dev)
        return 1;

    ret = eswin_service(dev, (uint8_t*) pReqData);
    return ret;
}


#ifdef CONFIG_SIM


void Decode_SignatureValidationCheck_REQ(u8 *pIn, u32 size, SignatureValidationCheck_REQ *pOut)
{
    int len = 0;

    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    len = sizeof(*pOut);
    if (size != len)
    {
        printk("eswin-ipc: Decode_SignatureValidationCheck_REQ size=%d "
                "error.\r\n", size);
        return;
    }
    memcpy((u8*) pOut, pIn, len);
}

void Decode_ImageDecryption_REQ(u8 *pIn, u32 size, ImageDecryption_REQ *pOut)
{
    int len;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;

    len = sizeof(*pOut);

    if (size != len)
    {
        printk("eswin-ipc: Decode_ImageDecryption_REQ size=%d error.\r\n", size);
        return;
    }
    memcpy((u8*) pOut, pIn, len);
}

void Decode_FirmwareDownload_REQ(u8 *pIn, u32 size, FirmwareDownload_REQ *pOut)
{
    int len;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    len = sizeof(*pOut);

    if (size != len)
    {
        printk("eswin-ipc: Decode_FirmwareDownload_REQ size=%d error.\r\n", size);
        return;
    }
    memcpy((u8*) pOut, pIn, len);
}
void Decode_RsaEccEncDec_REQ(u8 *pIn, u32 size, RsaEncDec_REQ *pOut)
{
    int len;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    len = sizeof(*pOut);

    if (size != len)
    {
        printk("eswin-ipc: RsaEncDec_RES size=%d error.\r\n", size);
        return;
    }
    memcpy((u8*) pOut, pIn, len);
}
void Decode_AesSm4EncDec_REQ(u8 *pIn, u32 size, AesSm4EncDec_REQ *pOut)
{
    int len;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    len = sizeof(*pOut);

    if (size != len)
    {
        printk("eswin-ipc: AesSm4EncDec_REQ size=%d error.\r\n", size);
        return;
    }
    memcpy((u8*) pOut, pIn, len);
}

void Decode_Sha1Sha256Sm3Digest_REQ(u8 *pIn, u32 size, Sha1Sha256Sm3Digest_REQ *pOut)
{
    int len;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    len = sizeof(*pOut);
    if (size != len)
    {
        printk("eswin-ipc: Decode_FirmwareDownload_REQ size=%d error.\r\n", size);
        return;
    }
    memcpy((u8*) pOut, pIn, len);
}

void Decode_HMAC_REQ(u8 *pIn, u32 size, HMAC_REQ *pOut)
{
    int len;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    len = sizeof(*pOut);
    if (size != len)
    {
        printk("eswin-ipc: Decode_FirmwareDownload_REQ size=%d error.\r\n", size);
        return;
    }
    memcpy((u8*) pOut, pIn, len);
}
void Decode_OTP_REQ(u8 *pIn, u32 size, OTP_REQ *pOut)
{
    int len;
    int totalLen = 0;
    u8 *pCursor;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;

    pCursor = pIn;
    len = sizeof(pOut->flag) + IPC_ADDR_LEN + sizeof(pOut->dwDataSize);
    memcpy((u8*) pOut, pCursor, len);
    pCursor += len;
    totalLen += len;

    len = pOut->dwDataSize;
    if (len > MAX_OTP_DATA_LEN)
    {
        printk("eswin-ipc: Decode_OTP_REQ "
                "dwDataSize(%d)>MAX_OTP_DATA_LEN error.\r\n", len);
        return;
    }

    memcpy(pOut->bData, pCursor, len);
    pCursor += len;
    totalLen += len;

    memcpy(pOut->bMask, pCursor, len);
    totalLen += len;

    if (totalLen != size)
        printk("eswin-ipc: Decode_OTP_REQ totalLen(%d)!=size(%d) error.\r\n", totalLen, size);
}
void Decode_TRNG_REQ(u8 *pIn, u32 size, TRNG_REQ *pOut)
{
    int len;
    int totalLen = 0;
    u8 *pCursor;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    pCursor = pIn;
    len = sizeof(pOut->dwFlag);
    memcpy((u8*) pOut, pCursor, len);

    totalLen += len;

    if (totalLen != size)
        printk("eswin-ipc: Decode_TRNG_REQ totalLen(%d)!=size(%d) "
                "error.\r\n", totalLen, size);
}

void Decode_AddrRegionProtection_REQ(u8 *pIn, u32 size, AddrRegionProtection_REQ *pOut)
{
    int len;
    int totalLen = 0;
    u8 *pCursor;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    pCursor = pIn;
    len = sizeof(*pOut);
    memcpy((u8*) pOut, pCursor, len);

    totalLen += len;

    if (totalLen != size)
        printk("eswin-ipc: Decode_AddrRegionProtection_REQ "
                "totalLen(%d)!=size(%d) error.\r\n", totalLen, size);
}
void Decode_Downloadable_REQ(u8 *pIn, u32 size, DownloadableInit_REQ *pOut)
{
    int len;
    int totalLen = 0;
    u8 *pCursor;
    if (NULL == pIn || NULL == pOut || 0 >= size)
        return;
    pCursor = pIn;

    len = sizeof(*pOut);
    memcpy((u8*) pOut, pCursor, len);

    totalLen += len;

    if (totalLen != size)
        printk("eswin-ipc: Decode_Downloadable_REQ "
                "totalLen(%d)!=size(%d) error.\r\n", totalLen, size);
}

void DecodeReqData(SRVC_TYPE SerType, u8* pIn, u32 size, REQ_DATA_DOMAIN_T* pOut)
{
  if (NULL == pIn || NULL == pOut || 0 >= size) return;

  switch (SerType)
  {
    case SRVC_TYPE_SIGN_CHECK:  // 0x00
      Decode_SignatureValidationCheck_REQ(pIn, size, &(pOut->SigChkReq));
      break;
    case SRVC_TYPE_IMG_DECRPT:  // 0x01--DATA_IMG_DECRYPTION_T
      Decode_ImageDecryption_REQ(pIn, size, &(pOut->ImgDecReq));
      break;
    case SRVC_TYPE_FIRMWARE_DOWNLOAD:  // 0x02
      Decode_FirmwareDownload_REQ(pIn, size, &(pOut->FirmDlReq));
      break;
    case SRVC_TYPE_RSA_CRYPT_DECRYPT:  // 0x03--DATA_RSA_ENCRYPTION_T/DATA_RSA_DECRYPTION_T
      Decode_RsaEccEncDec_REQ(pIn, size, &(pOut->RsaEncDecReq));
      break;
    case SRVC_TYPE_SYM_CRYPT_DECRYPT:  // 0x04
      Decode_AesSm4EncDec_REQ(pIn, size, &(pOut->AesSm4EncDecReq));
      break;
    case SRVC_TYPE_DIGEST:  // 0x05
      Decode_Sha1Sha256Sm3Digest_REQ(pIn, size, &(pOut->DigestReq));
      break;
    case SRVC_TYPE_HMAC:  // 0x06--DATA_SHA_DIGESTS_T
      Decode_HMAC_REQ(pIn, size, &(pOut->HmacReq));
      break;
    case SRVC_TYPE_OTP_READ_PROGRAM:  // 0x07
      Decode_OTP_REQ(pIn, size, &(pOut->OtpReq));
      break;
    case SRVC_TYPE_TRNG:  // 0x08--DATA_TRNG_T
      Decode_TRNG_REQ(pIn, size, &(pOut->TrngReq));
      break;
    case SRVC_TYPE_ADDR_REGION_PROTECTION:  // 0x0A
      Decode_AddrRegionProtection_REQ(pIn, size, &(pOut->AddrRegProReq));
      break;
    case SRVC_TYPE_DOWNLOADABLE:  // 0x0C
      Decode_Downloadable_REQ(pIn, size, &(pOut->DlInitReq));
      break;
    default:
      break;
  }
}
#endif
