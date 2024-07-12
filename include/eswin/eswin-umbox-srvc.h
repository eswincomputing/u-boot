// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef __ESWIN_MBOX_TEST_H__
#define __ESWIN_MBOX_TEST_H__

#include <eswin/ipc.h>
#include <eswin/eswin-uMsg.h>
#include <linux/compat.h>
#include <mailbox.h>

struct eswin_umbox_srvc {
    struct mbox_chan chan;
    /* temporarily store the SOC msg */
};
extern int ServiceSessionSendReq(struct udevice *dev, REQ_SRVC_T *pReqData);
extern void DbgPrintSerDataU84(MSG_SEND_T *msg_t);
extern void DbgPrintSerDataE21(MSG_RECV_T *msg_t);
extern int lpcpu_misc_func(void);
#endif
