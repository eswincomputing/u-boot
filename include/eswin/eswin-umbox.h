// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef __ESWIN_UMBOX_H__
#define __ESWIN_UMBOX_H__

#include <common.h>
#include <linux/compat.h>

#define ESWIN_MBOX_CHANNELS 2

//struct eswin_mbox_chan
//{
//    bool rx_msg_valid;
//    uint32_t rx_msg;
//};

struct eswin_umbox_session {
    struct mutex lock;

    /* temporarily store the SOC msg */
    u32 *msg;
    char *rx_buffer;
    u8 count;

    u32 num;
    SRVC_TYPE curSerType;
};

struct eswin_umbox
{
//    struct eswin_mbox_chan chans[ESWIN_MBOX_CHANNELS];

    //added by xzq begin:
    //struct mutex lock;
    void __iomem *tx_base;
    void __iomem *rx_base;
    int irq;
    struct eswin_umbox_session* umbox_session;
    //added by xzq end.
};

#endif
