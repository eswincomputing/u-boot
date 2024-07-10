// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef _ESWIN_IPC_H
#define _ESWIN_IPC_H

#include <common.h>
#include <eswin/eswin_mailbox_csr.h>
#include <eswin/eswin-uMsg.h>


#define ESWIN_SESSION_VERSION         1
#define ESWIN_SESSION_MAX_SIZE        512//unit: 4bytes(96*4=384).

#define MAX_MSG_LEN_FROM_E21            128

struct eswin_session;


struct eswin_local_msg_from_user
{
    u32 num;
    SRVC_TYPE srv_type;
    //u32 size;
    REQ_DATA_DOMAIN_T ReqData;
};

struct eswin_remote_msg_to_user
{
    u32 num;
    u32 IpcStatus;
    u32 ServiceStatus;
    //u32 size;   /* byte size */
    RES_DATA_DOMAIN_T Resdata;
};

typedef struct eswin_send_msg
{
    u32 num;
    SRVC_TYPE srv_type;
    u32 size;
    uint8_t *pData;
}MSG_SEND_T;

/*cmd,session_id,num,srv_type,size*/
#define SESSION_SEND_MAG_HEADER_DW_LEN 5

typedef struct eswin_recv_msg
{
    u32 num;
    u32 dwServiceID;
    u32 IpcStatus;
    u32 ServiceStatus;
    u32 size;   /* byte size */
    uint8_t *pData;
}MSG_RECV_T;

typedef struct __attribute__((packed, aligned(1))) eswin_recv_msg_ext
{
    u32 num;
    u32 dwServiceID;
    u32 IpcStatus;
    u32 ServiceStatus;
    u32 size;   /* byte size */
    RES_DATA_DOMAIN_T data_t;
}RECV_MSG_EXT_T;

#define SESSION_RECV_MAG_HEADER_DW_LEN 5

enum eswin_rpc_svc {
    ESWIN_RPC_SVC_UNKNOWN = 0,
    ESWIN_RPC_SVC_RETURN = 1,
    ESWIN_RPC_SVC_PM = 2,
    ESWIN_RPC_SVC_RM = 3,
    ESWIN_RPC_SVC_TIMER = 5,
    ESWIN_RPC_SVC_PAD = 6,
    ESWIN_RPC_SVC_MISC = 7,
    ESWIN_RPC_SVC_IRQ = 8,
};

struct eswin_rpc_msg {
    uint8_t ver;
    uint8_t size;
    uint8_t svc;
    uint8_t func;
};

/*
 * This type is used to indicate RPC MISC function calls.
 */
enum eswin_misc_func {
    ESWIN_MISC_FUNC_UNKNOWN = 0,
    ESWIN_MISC_FUNC_SET_CONTROL = 1,
    ESWIN_MISC_FUNC_GET_CONTROL = 2,
    ESWIN_MISC_FUNC_SET_MAX_DMA_GROUP = 4,
    ESWIN_MISC_FUNC_SET_DMA_GROUP = 5,
    ESWIN_MISC_FUNC_SECO_IMAGE_LOAD = 8,
    ESWIN_MISC_FUNC_SECO_AUTHENTICATE = 9,
    ESWIN_MISC_FUNC_DEBUG_OUT = 10,
    ESWIN_MISC_FUNC_WAVEFORM_CAPTURE = 6,
    ESWIN_MISC_FUNC_BUILD_INFO = 15,
    ESWIN_MISC_FUNC_UNIQUE_ID = 19,
    ESWIN_MISC_FUNC_SET_ARI = 3,
    ESWIN_MISC_FUNC_BOOT_STATUS = 7,
    ESWIN_MISC_FUNC_BOOT_DONE = 14,
    ESWIN_MISC_FUNC_OTP_FUSE_READ = 11,
    ESWIN_MISC_FUNC_OTP_FUSE_WRITE = 17,
    ESWIN_MISC_FUNC_SET_TEMP = 12,
    ESWIN_MISC_FUNC_GET_TEMP = 13,
    ESWIN_MISC_FUNC_GET_BOOT_DEV = 16,
    ESWIN_MISC_FUNC_GET_BUTTON_STATUS = 18,
};

#endif /* _ESWIN_IPC_H */
