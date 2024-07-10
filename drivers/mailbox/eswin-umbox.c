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
#include <asm/mbox.h>
#include <common.h>
#include <cpu_func.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <dm/devres.h>
#include <dm/of.h>
#include <dm/of_addr.h>
#include <eswin/ipc.h>
#include <eswin/eswin-uMsg.h>
#include <eswin/eswin-umbox-func.h>
#include <eswin/eswin-umbox-srvc.h>
#include <eswin/eswin-umbox.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <log.h>
#include <mailbox-uclass.h>
#include <malloc.h>

#define DDR_BASE			0x80000000
#define SYSTEM_PORT_BASE	0xc000000000
#define SYSTEM_PORT_ADDR(mem_port_addr)		(mem_port_addr - DDR_BASE + SYSTEM_PORT_BASE)

#define MBOX_MSG_LEN 4  // unit 4bytes,total=4*4bytes

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

/* Mailbox Registers */
#define MBOX_OFF(m) (0x40 * (m))
#define MBOX_MODE_REG(m) (MBOX_OFF(m) + 0x0)
#define MBOX_DATA_REG(m) (MBOX_OFF(m) + 0x4)

#define MBOX_STATE_MASK (0xF << 4)
#define MBOX_STATE_IDLE (0x1 << 4)
#define MBOX_STATE_TX (0x2 << 4)
#define MBOX_STATE_RX (0x4 << 4)
#define MBOX_STATE_ACK (0x8 << 4)
#define MBOX_ACK_CONFIG_MASK (0x1 << 0)
#define MBOX_ACK_AUTOMATIC (0x1 << 0)
#define MBOX_ACK_IRQ (0x0 << 0)

/* IPC registers */
#define ACK_INT_RAW_REG(i) ((i) + 0x400)
#define ACK_INT_MSK_REG(i) ((i) + 0x404)
#define ACK_INT_STAT_REG(i) ((i) + 0x408)
#define ACK_INT_CLR_REG(i) ((i) + 0x40c)
#define ACK_INT_ENA_REG(i) ((i) + 0x500)
#define ACK_INT_DIS_REG(i) ((i) + 0x504)
#define DST_INT_RAW_REG(i) ((i) + 0x420)

#ifdef CONFIG_SIM
void* g_IpcAddr;
void* g_Offset;
#endif

MSG_RECV_T gSessionMsgRcv;
MSG_SEND_T gSessionMsgSnd;

extern struct device_node* of_parse_phandle(const struct device_node *np, const char *phandle_name, int index);

typedef struct {
    u8 *sysport_addr;
    u8 *addr;
    u32 size;
} MBOX_BUF_T;

MBOX_BUF_T g_shared_buff;

#define FIFO_LEN 2
u32 g_u32_RD_Data[FIFO_LEN * 2];
u32 g_u32_RD_Index = 0;
u32 g_u32_RD_Cnt = 0;

int lock_session(struct eswin_umbox_session *session)
{
    if (!session)
        return -EPROBE_DEFER; mutex_lock(&session->lock);

    return 0;
}

int unlock_session(struct eswin_umbox_session *session)
{
    if (!session)
        return -EPROBE_DEFER;

    mutex_unlock(&session->lock);
    return 0;
}

#ifndef CONFIG_SIM
static void mbox_set_mode(struct eswin_umbox *umbox, unsigned int slot, u32 val)
{
    u32 mode;

    mode = readl(umbox->tx_base + MBOX_MODE_REG(slot));
    mode = (mode & ~MBOX_ACK_CONFIG_MASK) | val;
    writel(mode, umbox->tx_base + MBOX_MODE_REG(slot));
}

static void mbox_set_state(struct eswin_umbox *umbox, unsigned int slot, u32 val)
{
    u32 status;

    status = readl(umbox->tx_base + MBOX_MODE_REG(slot));
    status = (status & ~MBOX_STATE_MASK) | val;
    writel(status, umbox->tx_base + MBOX_MODE_REG(slot));
}
#endif

u32 get_recv_buff_size(IPC_REQ_T* ipc_req)
{
    u32 size = 0;

    switch(ipc_req->dwServiceID)
    {
        case SRVC_TYPE_SIGN_CHECK:
        case SRVC_TYPE_IMG_DECRPT:
        case SRVC_TYPE_FIRMWARE_DOWNLOAD:
        case SRVC_TYPE_PUBKEY_DOWNLOAD:
        case SRVC_TYPE_RSA_CRYPT_DECRYPT:
        case SRVC_TYPE_ECDH_KEY:
        case SRVC_TYPE_SYM_CRYPT_DECRYPT:
        case SRVC_TYPE_DIGEST:
        case SRVC_TYPE_HMAC:
        case SRVC_TYPE_OTP_READ_PROGRAM:
        case SRVC_TYPE_ADDR_REGION_PROTECTION:
        case SRVC_TYPE_DOWNLOADABLE:
        case SRVC_TYPE_BASIC_IO:
        case SRVC_TYPE_AXPROT:
            size = 4 + sizeof(RECV_MSG_EXT_T) + 4;
            break;
        case SRVC_TYPE_TRNG:
            size = 4 + sizeof(RECV_MSG_EXT_T) - sizeof(RES_DATA_DOMAIN_T) + *(u32*)ipc_req->pData + 4;  /* 4: address alignment for 4byte */
            break;
        case SRVC_TYPE_MAX:
        default :
            break;
    }

    return size;
}

int PackRData(u8 *pData0, u8 *pData1, REGISTER_DATA_T *pRData)
{
    u8 buf[R_DATA_LEN], bVal;
    int len, i;
    u8 bFrameHead;

    u8 cTmp[2];
    char cData0[R_DATA_LEN * 2];

    if (NULL == pData0 || NULL == pData1 || NULL == pRData)
        return 1;

#ifdef CONFIG_SIM
  bFrameHead = FRAME_HEAD_U84;
#else
    bFrameHead = FRAME_HEAD_E21;
#endif
    memcpy(buf, pData0, 7);
    len = R_DATA_LEN - 7;
    memcpy(buf + 7, pData1, len);

#ifdef CONFIG_SIM

#else
    u8 buf1[R_DATA_LEN];
    for (i = 0; i < R_DATA_LEN; i++)
    {
        buf1[i] = buf[R_DATA_LEN - 1 - i];
    }
    memcpy(buf, buf1, R_DATA_LEN);

#endif

    bVal = buf[0];
    for (i = 1; i < R_DATA_LEN - 1; i++)
    {
        bVal = bVal ^ buf[i];
    }

    if (bVal != buf[i])
    {
        int j;
        memset(cData0, 0, R_DATA_LEN * 2);
        for (j = 0; j < R_DATA_LEN; j++)
        {
            sprintf((char*) cTmp, "%02x", buf[j]);
            strcat(cData0, (char*) cTmp);
        }
        printk("xor check failed, bVal=%02x,buf[%d]=%02x,cData0=%s, "
                "error.\r\n", bVal, i, buf[i], cData0);
        return 2;
    }

    memset(pRData, 0, sizeof(REGISTER_DATA_T));
    pRData->bFrameHead = buf[0];
    if (bFrameHead != pRData->bFrameHead)
    {
        printk("PackRData: FrameHead=0x%x, not 0x%x ,error.\r\n", pRData->bFrameHead, bFrameHead);
        return 3;
    }
    pRData->bFrameLen = buf[1];
    if (IPC_ADDR_LEN != pRData->bFrameLen)
    {
        printk("PackRData: pRData->bFrameLen(0x%x)!=IPC_ADDR_LEN(0x%x) "
                ",error.\r\n", pRData->bFrameLen, IPC_ADDR_LEN);
        return 4;
    }
    for (i = 0; i < IPC_ADDR_LEN; i++)
        pRData->bIpcAddr[i] = buf[i + 2];

#if U_MAILBOX_DBG
    if (1)
    {
        int j;
        memset(cData0, 0, R_DATA_LEN * 2);
        for (j = 0; j < IPC_ADDR_LEN; j++)
        {
            sprintf((char*) cTmp, "%02x", pRData->bIpcAddr[IPC_ADDR_LEN - 1 - j]);
            strcat(cData0, (char*) cTmp);
        }

        debug("PackRData: pRData->bIpcAddr= %s .\r\n", cData0);
    }
#endif

    pRData->bChkRlt = bVal;
    return 0;
}

int ChkResData(IPC_RES_T *pIpcResData, u8 *pRecBuf, int len0)
{
    int i, len;
    u8 bChkRlt;
    u8 *pBValue;
    u8 bFHead[4];
    if (NULL == pIpcResData || NULL == pRecBuf)
        return 1;
#if 0
    bFHead[0]=0xCC;
    bFHead[1]=0x45;
    bFHead[2]=0x78;
    bFHead[3]=0xB5;
#else
    bFHead[0] = 0xB5;
    bFHead[1] = 0x78;
    bFHead[2] = 0x45;
    bFHead[3] = 0xCC;

#endif
    pBValue = (u8*) pRecBuf;
    bChkRlt = *pBValue;
    len = len0 - 1;
    for (i = 1; i < len; i++)
    {
        bChkRlt = bChkRlt ^ (*(pBValue + i));
    }

    if (bChkRlt != (*(pBValue + i)))  // check failed
        return 2;
    // 0xCC4578B5
    if (bFHead[0] != *(pBValue + 0) || bFHead[1] != *(pBValue + 1) || bFHead[2] != *(pBValue + 2) || bFHead[3] != *(pBValue + 3))
    {
        printk("ChkResData: FrameHead is 0x%2x%2x%2x%2x not "
                "0x%2x%2x%2x%2x.\r\n", *pBValue, *(pBValue + 1), *(pBValue + 2), *(pBValue + 3), bFHead[0], bFHead[1], bFHead[2], bFHead[3]);
        return 3;
    }

    u8_to_u32(&pIpcResData->dwNum, pBValue + 4);
    u8_to_u32(&pIpcResData->dwServiceID, pBValue + 8);
    u8_to_u32(&pIpcResData->IpcStatus, pBValue + 12);
    u8_to_u32(&pIpcResData->ServiceStatus, pBValue + 16);
    u8_to_u32(&pIpcResData->dwDataLen, pBValue + 20);

    return 0;
}

int ChkReqData(IPC_REQ_T *pIpcReqData, u8 *pRecBuf, int len0)
{
    int i, len;
    u8 bChkRlt;
    u8 *pBValue;
    u8 bFHead[4];
    if (NULL == pIpcReqData || NULL == pRecBuf)
        return 1;
#if 0
    bFHead[0]=0xBB;
    bFHead[1]=0x45;
    bFHead[2]=0x65;
    bFHead[3]=0xA6;
#else
    bFHead[0] = 0xA6;
    bFHead[1] = 0x65;
    bFHead[2] = 0x45;
    bFHead[3] = 0xBB;
#endif

    pBValue = (u8*) pRecBuf;
    bChkRlt = *pBValue;
    len = len0 - 1;
    for (i = 1; i < len; i++)
    {
        bChkRlt = bChkRlt ^ (*(pBValue + i));
    }

    if (bChkRlt != (*(pBValue + i)))  // check failed
        return 2;
    // 0xBB4565A6
    if (bFHead[0] != *(pBValue + 0) || bFHead[1] != *(pBValue + 1) || bFHead[2] != *(pBValue + 2) || bFHead[3] != *(pBValue + 3))
    {
        printk("ChkReqData: FrameHead is 0x%2x%2x%2x%2x not "
                "0x%2x%2x%2x%2x.\r\n", *pBValue, *(pBValue + 1), *(pBValue + 2), *(pBValue + 3), bFHead[0], bFHead[1], bFHead[2], bFHead[3]);
        return 3;
    }

    u8_to_u32(&pIpcReqData->dwNum, pBValue + 4);
    u8_to_u32(&pIpcReqData->dwServiceID, pBValue + 8);
    u8_to_u32(&pIpcReqData->dwDataLen, pBValue + 12);

    return 0;
}

static int eswin_umbox_rx_callback(struct mbox_chan *chan, void *msg)
{
    struct eswin_umbox *umbox = dev_get_priv(chan->dev);
    struct eswin_umbox_session *soc_session = umbox->umbox_session;

#ifdef CONFIG_SIM
    //eswin_local_msg_from_user    MsgFromUser;
    MSG_SEND_T *hdr = (MSG_SEND_T *)msg;
    MSG_RECV_T *hdr0 = (MSG_RECV_T *)msg;

    REQ_DATA_DOMAIN_T ReqDataDomain;
    if(soc_session->num != hdr->num)
    {
        printk("eswin_umbox_rx_callback: soc_session->num(%d)!=hdr->num(%d),error!\r\n",
        soc_session->num,hdr->num );
        return 1;
    }
    if(soc_session->curSerType!=hdr->srv_type)
    {
        printk("eswin_umbox_rx_callback: curSerType(%d)!=hdr->srv_type(%d),error!\r\n",
        soc_session->curSerType,hdr->srv_type );
        return 1;
    }

    memset(&ReqDataDomain,0,sizeof(ReqDataDomain));
    printk("eswin_umbox_rx_callback: recv num=%d,serv_type=%d,size=%d.\r\n",
    hdr->num, hdr->srv_type, hdr->size);
    DecodeReqData(hdr->srv_type,hdr->pData,hdr->size,&ReqDataDomain);
    //DbgPrintSerDataU84(hdr->srv_type,&ReqDataDomain);

#else
    MSG_RECV_T *hdr = (MSG_RECV_T*) msg;

    if (soc_session->num != hdr->num)
    {
        printk("eswin_umbox_rx_callback: soc_session->num(%d)!=hdr->num(%d),error!\r\n", soc_session->num, hdr->num);
        return 1;
    }
    if (soc_session->curSerType != hdr->dwServiceID)
    {
        printk("eswin_umbox_rx_callback: curSerType(%d)!=hdr->dwServiceID(%d),error!\r\n", soc_session->curSerType, hdr->dwServiceID);
        return 1;
    }

    debug("eswin_umbox_rx_callback: recv num=%u,ServiceID=%u,IpcStatus=%u,ServiceStatus=%u,size=%u.\r\n", hdr->num, hdr->dwServiceID, hdr->IpcStatus, hdr->ServiceStatus, hdr->size);

    DbgPrintSerDataE21(msg);

    debug("eswin_umbox_rx_callback: end.\n");
#endif

    return 0;
}
// TODO timeout ......
#define MAX_REPEAT_TIMES 100000
static int eswin_umbox_recv(struct mbox_chan *chan, void *data)
{
    struct eswin_umbox *umbox = dev_get_priv(chan->dev);
    struct eswin_umbox_session *umbox_session;
    unsigned int state0, i;
    void *pOffset;
    void *pOffset_tmp;
    u8 regData0[8];
    u8 regData1[8];
    u32 msg[MBOX_MSG_LEN];
    int len, ret;
    REGISTER_DATA_T rData;
    umbox_session = umbox->umbox_session;

    u32 dwVal;
    unsigned int state;
    unsigned int ack_irq = 0;
    RECV_MSG_EXT_T *pMsg;
    IPC_RES_T IpcResData;

    debug("\r\neic7x_umbox_recv: start...\n");
    while (1) {
        while (1) {
            state = readl(umbox->rx_base + REG_MB_INT);
            state0 = state;
            if (state0 == E21_IRQ_BIT) {
                break;
            }
        }
        if (!state0) {
            printk("eic7x_umbox_recv read REG_MB_INT failed.\r\n");
            return IRQ_NONE;
        }
        i = 0;
        while (1) {
            dwVal = readl(umbox->rx_base + REG_FIFO_STATUS);  // step-7
            dwVal = (dwVal & 0x02);
            if (0 != dwVal)
                break;
            msg[i] = readl(umbox->rx_base + REG_RD_DATA_0);      // step-8
            msg[i + 1] = readl(umbox->rx_base + REG_RD_DATA_1);  // step-8
            i += 2;
            writel(0x00000000, umbox->rx_base + REG_RD_DATA_1);  // step-9
        }
        writel(0, umbox->rx_base + REG_MB_INT);
        if (!dwVal) {
            printk("eic7x_umbox_recv read REG_MB_ERR failed.\r\n");
            return IRQ_NONE;
        }
        /*Bootrom did not do mb operation before trigger mailbox irq,
         So Here we make a delay to ensure the bootrom memory operation is finished.
        */
        mdelay(5);
        u32_to_u8(regData0, &msg[0]);
        u32_to_u8(regData0 + 4, &msg[1]);
        u32_to_u8(regData1, &msg[2]);
        u32_to_u8(regData1 + 4, &msg[3]);

        debug("eswin_umbox_recv: read REG_RD_DATA_0 return\r\n "
                "msg[0]=0x%x,msg[1]=0x%x,msg[2]=0x%x,msg[3]=0x%x.\r\n", msg[0], msg[1], msg[2], msg[3]);

        ret = PackRData(regData0, regData1, &rData);
        if (0 != ret) {
            printk("eic7x_umbox_recv PackRData return %d,error.\r\n", ret);
            udelay(10 * 1000);
            continue;
        }
        pOffset = (void*) (unsigned long) ((u32) rData.bIpcAddr[0] + (u32) (((u32) rData.bIpcAddr[1]) << 8) + (u32) (((u32) rData.bIpcAddr[2]) << 16) + (u32) (((u32) rData.bIpcAddr[3]) << 24));
        if (g_shared_buff.addr != pOffset) {
            printk("eic7x_umbox_recv "
                    "g_shared_buff.addr(0x%p)!=pOffset(0x%p) warning!\r\n", g_shared_buff.addr, pOffset);
            kfree(g_shared_buff.addr);
        }
        pOffset_tmp = pOffset;
        pOffset = SYSTEM_PORT_ADDR(pOffset);

        u8_to_u32((u32*) &len, (u8*) (pOffset + 20));

        debug("eic7x-umbox: in eic7x_umbox_recv, msg len=%d, IPC "
                "data in 0x%p is:\r\n", len, pOffset);

        for (i = 0; i < len + 24 + 1; i++) {
            debug("0x%02x ", *(u8*)(pOffset + i));
        }
        debug(".\r\n");
        ret = ChkResData(&IpcResData, pOffset, len + 9 + 16);
        if (0 != ret) {
            printk("eic7x_umbox_recv ChkResData/ChkReqData return %d, "
                    "error!\r\n", ret);
            udelay(10 * 1000);
            continue;
        }
        if (len != IpcResData.dwDataLen) {
            printk("eic7x_umbox_recv "
                    "len(%d)!=IpcResData.dwDatalen(%d),error!\r\n", len, IpcResData.dwDataLen);
            udelay(10 * 1000);
            continue;
        }
        gSessionMsgRcv.num = IpcResData.dwNum;
        gSessionMsgRcv.dwServiceID = IpcResData.dwServiceID;
        gSessionMsgRcv.IpcStatus = IpcResData.IpcStatus;
        gSessionMsgRcv.ServiceStatus = IpcResData.ServiceStatus;
        gSessionMsgRcv.size = len;
        gSessionMsgRcv.pData = pOffset + 24;

        pMsg = (RECV_MSG_EXT_T*) data;
        pMsg->num = gSessionMsgRcv.num;
        pMsg->dwServiceID = gSessionMsgRcv.dwServiceID;
        pMsg->IpcStatus = gSessionMsgRcv.IpcStatus;
        pMsg->ServiceStatus = gSessionMsgRcv.ServiceStatus;
        pMsg->size = gSessionMsgRcv.size;
        memcpy((void*) &pMsg->data_t, (void*) gSessionMsgRcv.pData, pMsg->size);
        ret = eswin_umbox_rx_callback(chan, (void*) &gSessionMsgRcv);  // Will call eic7x_rx_callback
        kfree(pOffset_tmp);
        debug("eic7x_umbox_recv: kfree 0x%p ok, ret %d!\r\n", pOffset_tmp, ret);
        if (0 == ret) {
            g_u32_RD_Index = 0;
            g_u32_RD_Cnt = 0;

            /* clear IRQ source */
            writel(BIT(ack_irq), umbox->rx_base + REG_MB_INT);  // step-11
            writel(0x00000000, umbox->rx_base + REG_WR_LOCK);   // step-12

            mbox_set_state(umbox, 0, MBOX_STATE_IDLE);
            unlock_session(umbox_session);
            debug("eswin_umbox_recv: done.\r\n");
            return 0;
        }
    }
}

static int umbox_send_data(struct eswin_umbox *umbox, void *msg)
{
    u32 uL32, uH32;
    u8_to_u32(&uL32, msg);
    u8_to_u32(&uH32, msg + 4);
    unsigned int slot = 0;
    mbox_set_state(umbox, slot, MBOX_STATE_TX);
    mbox_set_mode(umbox, slot, MBOX_ACK_AUTOMATIC);
    writel(uL32, umbox->tx_base + REG_WR_DATA_0);
    writel(uH32, umbox->tx_base + REG_WR_DATA_1);
    writel(0, umbox->rx_base + REG_MB_INT);
    return 0;
}
/*
	In this function, we send the share memory's memory port address to bootrom since it can't access address > 4G.
	But ourself use the share memory's system port address to avoid cache operation.
*/
static int eswin_umbox_send(struct mbox_chan *chan, const void *data)
{
    struct eswin_umbox *umbox = dev_get_priv(chan->dev);
    int max_send_size, max_recv_size, ret = 0;
    u8 regData0[8];
    u8 regData1[8];
    REGISTER_DATA_T rData;
    IPC_REQ_T IpcReqData;
    MSG_SEND_T *pSendMsg;

    /* lock the channel of current mailbox */
    lock_session(umbox->umbox_session);
    pSendMsg = (MSG_SEND_T*) data;

    /* Restore receive info to umbox_session */
    umbox->umbox_session->num++;
    if (0xFFFFFFFF == umbox->umbox_session->num)
        umbox->umbox_session->num = 0;

    pSendMsg->num = umbox->umbox_session->num;
    debug("eswin_umbox_send:send num is:%d\r\n", pSendMsg->num);
    umbox->umbox_session->curSerType = pSendMsg->srv_type;

    max_send_size = pSendMsg->size;
    debug("eswin_umbox_send: start...\r\n");
    /* Check size */
    if (max_send_size > ESWIN_SESSION_MAX_SIZE * 4) {
        printk("eic7x_umbox_send: msg size=%d, overflow.\r\n", max_send_size);
        ret = -EINVAL;
        goto FAILED;
    }

    if (IsChanBusy(umbox, chan)) {
        printk("eic7x_umbox_send: IsChanBusy return true, failed.\r\n");
        ret = -EINVAL;
        goto FAILED;
    }
    Fill_IPC_Data_Req(pSendMsg, &IpcReqData);
    max_send_size = 4 + sizeof(IpcReqData.dwNum) + sizeof(IpcReqData.dwServiceID) + sizeof(IpcReqData.dwDataLen) + IpcReqData.dwDataLen + 1;  // max send buffer size
    max_recv_size = get_recv_buff_size(&IpcReqData);
    g_shared_buff.size = MAX(max_send_size, max_recv_size);

    debug("eswin_umbox_send: REQ_DATA_DOMAIN_T len = [%ld] .\r\n", sizeof(REQ_DATA_DOMAIN_T));
    debug("eswin_umbox_send: RES_DATA_DOMAIN_T len = [%ld] .\r\n", sizeof(RES_DATA_DOMAIN_T));
    debug("eswin_umbox_send: max_send_size = [%d] .\r\n", max_send_size);
    debug("eswin_umbox_send: max_recv_size = [%d] .\r\n", max_recv_size);
    debug("eswin_umbox_send: g_shared_buff.size = [%d] .\r\n", g_shared_buff.size);
    g_shared_buff.addr = kmalloc(g_shared_buff.size, GFP_ATOMIC);
    g_shared_buff.sysport_addr = SYSTEM_PORT_ADDR(g_shared_buff.addr);

    if (NULL == g_shared_buff.addr) {
        printk("eic7x_umbox_send: kmalloc %d failed in "
                "eic7x_ipc_write.\r\n", g_shared_buff.size);
        ret = -ENOMEM;
        goto FAILED;
    }

    ret = EncodeSendBuf_Req(g_shared_buff.sysport_addr, &IpcReqData);
    if (0 != ret) {
        kfree(g_shared_buff.addr);
        printk("eswin_umbox_send: EncodeSendBuf return %d, failed.\r\n", ret);
        goto FAILED;
    }
    debug("eic7x_umbox_send: before Fill_RData, shared memaddr = "
            "0x%p, sysport_addr 0x%p, IPC Size=%d.\r\n",
            g_shared_buff.addr, g_shared_buff.sysport_addr, g_shared_buff.size);

    ret = Fill_RData(&rData, chan, g_shared_buff.addr);
    if (0 != ret) {
        printk("eic7x_umbox_send: Fill_RData return %d, failed.\r\n", ret);
        goto FAILED;
    }

    SliceRData(&rData, regData0, regData1);

    ret = umbox_send_data(umbox, regData0);  // step-4
    if (ret < 0) {
        printk("eic7x-umbox: in eic7x_umbox_send,umbox_send_data "
                "failed.\r\n");
        goto FAILED;
    }
    mb();
    TriggerRemoteIrq(umbox);  // step-5

    int mm;
    debug("eic7x_umbox_send: umbox_send_data is:\r\n");
    for (mm = 0; mm < max_send_size; mm++) {
        debug("0x%02x ", *(g_shared_buff.sysport_addr + mm));
        if ((mm + 1) % 8 == 0) {
            debug("\r\n");
        }
    }
    debug("eic7x_umbox_send: done.\n");
    return 0;

FAILED:
    unlock_session(umbox->umbox_session);
    return ret;
}

static int eswin_umbox_request(struct mbox_chan *chan)
{
    debug("%s(chan=%p)\n", __func__, chan);

    if (chan->id >= ESWIN_MBOX_CHANNELS)
        return -EINVAL;

    return 0;
}

static int eswin_umbox_free(struct mbox_chan *chan)
{
    debug("%s(chan=%p)\n", __func__, chan);

    return 0;
}

static int eswin_umbox_bind(struct udevice *dev)
{
    debug("%s(dev=%p)\n", __func__, dev);

    return 0;
}

static int eswin_umbox_probe(struct udevice *dev)
{
    struct eswin_umbox *umbox = dev_get_priv(dev);
    const struct device_node *node;
    struct eswin_umbox_session *umbox_session;
    fdt_addr_t tx_addr;
    fdt_addr_t rx_addr;

    debug("%s(dev=%p)\n", __func__, dev);
    node = (const struct device_node*) dev->node_.np;
    umbox_session = devm_kzalloc(dev, sizeof(*umbox_session), GFP_KERNEL);
    if (!umbox_session)
        return -ENOMEM;

    tx_addr=dev_read_addr_index(dev,0);
    if (tx_addr == FDT_ADDR_T_NONE)
        return -EINVAL;
    umbox->tx_base = (void __iomem*)tx_addr;
    debug("umbox->tx_base=0x%p, from dts.\r\n", umbox->tx_base);

    rx_addr=dev_read_addr_index(dev,1);
    if (rx_addr == FDT_ADDR_T_NONE)
        return -EINVAL;
    umbox->rx_base = (void __iomem*)rx_addr;
    debug("umbox->rx_base=0x%p, from dts.\r\n", umbox->rx_base);
    umbox->umbox_session = umbox_session;
    // eswin_umbox_handle=umbox;
    return 0;
}

static const struct udevice_id eswin_mbox_ids[] =
{
    { .compatible = "eswin,mbox" }, /*sandbox,mbox*/
    { }
};

struct mbox_ops eswin_mbox_mbox_ops =
{
    .request = eswin_umbox_request,
    .rfree = eswin_umbox_free,
    .send = eswin_umbox_send,
    .recv = eswin_umbox_recv,
};

U_BOOT_DRIVER(eswin_umbox) =
{
    .name = "eswin_umbox",
    .id = UCLASS_MAILBOX,
    .of_match = eswin_mbox_ids,
    .bind = eswin_umbox_bind,
    .probe = eswin_umbox_probe,
    .priv_auto = sizeof(struct eswin_umbox),
    .ops = &eswin_mbox_mbox_ops,
};
