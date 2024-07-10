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
#include <eswin/eswin-uMsg.h>
#include <eswin/eswin-umbox-func.h>
#include <eswin/eswin-umbox.h>
#include <linux/bitops.h>
#include <linux/bug.h>
#include <log.h>
#include <mailbox-uclass.h>
#include <malloc.h>

void u32_to_u8(u8 *pU8Buf, u32 *pDWValue)
{
    u32 dwVal;
    dwVal = *pDWValue;
    *pU8Buf = (u8) (0x000000ff & dwVal);
    *(pU8Buf + 1) = (u8) ((0x0000ff00 & dwVal) >> 8);
    *(pU8Buf + 2) = (u8) ((0x00ff0000 & dwVal) >> 16);
    *(pU8Buf + 3) = (u8) ((0xff000000 & dwVal) >> 24);
}

void u8_to_u32(u32 *pDWValue, u8 *pU8Buf)
{
    u32 dwVal;
    dwVal = (u32) (*pU8Buf) + (u32) (*(pU8Buf + 1) << 8) + (u32) (*(pU8Buf + 2) << 16) + (u32) (*(pU8Buf + 3) << 24);
    *pDWValue = dwVal;
}

void Print_IPC_ADDR(u8 *pAddr, char *pcAddr)
{
    u8 cTmp[2];
    int i;

    if (NULL == pAddr || NULL == pcAddr)
    {
        printk("Print_IPC_ADDR: pAddr or pcAddr is NULL error!\r\n");
        return;
    }
    memset(pcAddr, 0, IPC_ADDR_LEN * 2 + 1);

    for (i = 0; i < IPC_ADDR_LEN; i++)
    {
        sprintf((char*)cTmp, "%02x", *(pAddr + i));
        strcat(pcAddr, (char*)cTmp);
    }
}

void Print_Packet_Data(u8 *pData, unsigned int dwlen)
{
    unsigned int i;
    if (NULL == pData)
        return;

    debug("packet data is:\r\n");
    for (i = 0; i < dwlen; i++)
    {
        debug("0x%02X ", *((char*) pData + i));
        if (((i + 1) & 0xf) == 0)
            debug("\r\n");
    }

    debug("\r\n");
}

void DbgPrintSerDataU84(MSG_SEND_T *msg_t)
{
    if (NULL == msg_t)
        return;

    debug("req service is: %02x.\r\n", msg_t->srv_type);
    Print_Packet_Data(msg_t->pData, msg_t->size);
}

void DbgPrintSerDataE21(MSG_RECV_T *msg_t)
{
    if (NULL == msg_t)
        return;

    debug("res service is: %02x.\r\n", msg_t->dwServiceID);
    Print_Packet_Data(msg_t->pData, msg_t->size);
}

static inline int eswin_to_linux_errno(int errno)
{
    // if (errno >= ESWIN_ERR_NONE && errno < ESWIN_ERR_LAST)
    //    return eswin_linux_errmap[errno];
    // return -EIO;
    return 0;
}

void TriggerRemoteIrq(struct eswin_umbox *umbox)
{
    unsigned int dst_irq = 1;
    writel(BIT(dst_irq), umbox->tx_base + REG_MB_INT);
}

bool IsChanBusy(struct eswin_umbox *umbox, struct mbox_chan *chan)
{
    u32 dwInVal, dwOutVal;
    dwInVal = E21_CHK_BUSY_VAL;
    writel(dwInVal, umbox->tx_base + REG_WR_LOCK);
    dwOutVal = readl(umbox->tx_base + REG_WR_LOCK);

    if (dwInVal != dwOutVal)
        return 1;
    return 0;
}

#ifdef CONFIG_SIM
extern void* g_IpcAddr;
extern void* g_Offset;
#endif

int Fill_RData(REGISTER_DATA_T *pRegData, struct mbox_chan *chan, u8 *pIpcAddr)
{
    u8 bChkRlt;
    int i;
    int len;
    u8 *pByte;
    int ret = 0;

    if (NULL == pRegData)
        return 1;

    memset(pRegData, 0, sizeof(*pRegData));

    pRegData->bFrameLen = (u8) 0x04;


    pRegData->bIpcAddr[0] = (u8) (0x000000ff & ((u32) (u64) (u32*) pIpcAddr));
    pRegData->bIpcAddr[1] = (u8) ((0x0000ff00 & ((u32) (u64) (u32*) pIpcAddr)) >> 8);
    pRegData->bIpcAddr[2] = (u8) ((0x00ff0000 & ((u32) (u64) (u32*) pIpcAddr)) >> 16);
    pRegData->bIpcAddr[3] = (u8) ((0xff000000 & ((u32) (u64) (u32*) pIpcAddr)) >> 24);

    /*pRegData will be decoded in eic7700_mbox_interrupt in
     * eic7700-mailbox.c*/
    // memcpy(pRegData->bIpcAddr, &(mbox->ipc), 8);
    pRegData->bFrameHead = FRAME_HEAD_U84;

    len = sizeof(*pRegData) - 1;
    pByte = (u8*) pRegData;
    bChkRlt = *(pByte + 0);
    for (i = 1; i < len; i++)
    {
        bChkRlt = bChkRlt ^ (*(pByte + i));
    }
    *(pByte + i) = bChkRlt;

    return ret;
}

int SliceRData(REGISTER_DATA_T *pRegData, u8 *pData0, u8 *pData1)
{
    u8 *pData;
    int i, len;
    if (NULL == pRegData || NULL == pData0 || NULL == pData1)
        return 1;

    // printk("Enter SliceRData.\r\n");

    memset(pData0, 0, 8);
    memset(pData1, 0, 8);
    pData = (u8*) pRegData;

    for (i = 0; i < 7; i++)
    {
        *(pData0 + i) = *(pData + i);
    }
    *(pData0 + i) = 0x80;

    len = sizeof(REGISTER_DATA_T) - 7;

    // printk("SliceRData: len=%d.\r\n",len);

    for (i = 0; i < len; i++)
    {
        *(pData1 + i) = *(pData + 7 + i);
    }

    *(pData1 + 7) = 0x80;

    return 0;
}

int Fill_IPC_Data_Req(MSG_SEND_T *msg, IPC_REQ_T *pIpcReqData)
{
    if (NULL == msg || NULL == pIpcReqData)
        return 1;
    memset(pIpcReqData, 0, sizeof(*pIpcReqData));

    pIpcReqData->dwFrameHead = 0xBB4565A6;
    pIpcReqData->dwNum = msg->num;
    pIpcReqData->dwServiceID = msg->srv_type;
    pIpcReqData->dwDataLen = msg->size;
    pIpcReqData->pData = msg->pData;

    return 0;
}

int Fill_IPC_Data_Res(MSG_RECV_T *msg, IPC_RES_T *pIpcResData)
{
    if (NULL == msg || NULL == pIpcResData)
        return 1;
    memset(pIpcResData, 0, sizeof(*pIpcResData));

    pIpcResData->dwFrameHead = 0xCC4578B5;
    pIpcResData->dwNum = msg->num;
    pIpcResData->dwServiceID = msg->dwServiceID;
    pIpcResData->IpcStatus = msg->IpcStatus;
    pIpcResData->ServiceStatus = msg->ServiceStatus;
    pIpcResData->dwDataLen = msg->size;
    pIpcResData->pData = msg->pData;

    return 0;
}

int EncodeSendBuf_Req(u8 *pSendBuf, IPC_REQ_T *pIpcReqData)
{
    int ret = 0;
    u8 bVal;
    int i, len;
    if (NULL == pSendBuf || NULL == pIpcReqData)
        return 1;

    memcpy(pSendBuf + 0, (void*) &pIpcReqData->dwFrameHead, 4);
    u32_to_u8(pSendBuf + 4, &pIpcReqData->dwNum);
    u32_to_u8(pSendBuf + 8, &pIpcReqData->dwServiceID);
    u32_to_u8(pSendBuf + 12, &pIpcReqData->dwDataLen);

    len = pIpcReqData->dwDataLen;

    if (NULL == pIpcReqData->pData) {
        printk("eic7700-umbox-func: in EncodeSendBuf_Req,pIpcReqData->pData "
                "is NULL, return 1.\r\n");
        return 1;
    }

    memcpy(pSendBuf + 16, pIpcReqData->pData, len);

    bVal = *pSendBuf;
    len = 4 + sizeof(pIpcReqData->dwNum) + sizeof(pIpcReqData->dwServiceID) + sizeof(pIpcReqData->dwDataLen) + len;
    for (i = 1; i < len; i++) {
        bVal = bVal ^ (*(pSendBuf + i));
    }

    *(pSendBuf + i) = bVal;

    return ret;
}

int EncodeSendBuf_Res(u8 *pSendBuf, IPC_RES_T *pIpcResData)
{
    int ret = 0;
    u8 bVal;
    int i, len;
    if (NULL == pSendBuf || NULL == pIpcResData)
        return 1;

    memcpy(pSendBuf + 0, (void*) &pIpcResData->dwFrameHead, 4);

    u32_to_u8(pSendBuf + 4, &pIpcResData->dwNum);
    u32_to_u8(pSendBuf + 8, &pIpcResData->dwServiceID);
    u32_to_u8(pSendBuf + 12, &pIpcResData->IpcStatus);
    u32_to_u8(pSendBuf + 16, &pIpcResData->ServiceStatus);
    u32_to_u8(pSendBuf + 20, &pIpcResData->dwDataLen);

    len = pIpcResData->dwDataLen;

    if (NULL != pIpcResData->pData)
        memcpy(pSendBuf + 24, pIpcResData->pData, len);

    bVal = *pSendBuf;
    len = 4 + sizeof(pIpcResData->dwNum) + sizeof(pIpcResData->dwServiceID) + sizeof(pIpcResData->IpcStatus) + sizeof(pIpcResData->ServiceStatus) + sizeof(pIpcResData->dwDataLen) + len;
    for (i = 1; i < len; i++)
    {
        bVal = bVal ^ (*(pSendBuf + i));
    }

    *(pSendBuf + i) = bVal;

    return ret;
}
