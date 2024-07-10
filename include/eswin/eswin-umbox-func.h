// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef __ESWIN_IPC_FUNC_H__
#define __ESWIN_IPC_FUNC_H__

#include <eswin/ipc.h>
#include <eswin/eswin-uMsg.h>
#include <mailbox.h>
#include "eswin/eswin-umbox.h"

extern int SliceRData(REGISTER_DATA_T* pRegData, u8* pData0, u8* pData1 );
extern int Fill_IPC_Data_Req(MSG_SEND_T *msg,IPC_REQ_T*pIpcReqData);
extern int Fill_IPC_Data_Res(MSG_RECV_T *msg,IPC_RES_T*pIpcResData);
extern int EncodeSendBuf_Req(u8* pSendBuf,IPC_REQ_T*pIpcReqData);
extern int EncodeSendBuf_Res(u8* pSendBuf,IPC_RES_T*pIpcResData);
extern void u32_to_u8(u8*pU8Buf, u32* pDWValue);
extern void u8_to_u32(u32* pDWValue, u8*pU8Buf);
extern int Fill_REQ_DATA_DOMAIN_T(REQ_DATA_DOMAIN_T* pReqData,SRVC_TYPE SerType);
extern int Fill_RES_DATA_DOMAIN_T(RES_DATA_DOMAIN_T* pResData,SRVC_TYPE SerType);
extern int Fill_RData( REGISTER_DATA_T* pRegData,struct mbox_chan *chan,u8* pIpcAddr);
extern bool IsChanBusy(struct eswin_umbox *umbox, struct mbox_chan *chan);
extern void TriggerRemoteIrq(struct eswin_umbox *umbox);

#endif //__ESWIN_IPC_FUNC_H__
