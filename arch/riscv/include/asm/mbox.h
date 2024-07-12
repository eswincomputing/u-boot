// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 */

#ifndef __ESWIN_MBOX_H
#define __ESWIN_MBOX_H

#include <common.h>

#define ESWIN_MBOX_PING_XOR 0x12345678

struct udevice;

int eswin_umbox_service_get(struct udevice *dev);
int eswin_umbox_service_send(struct udevice *dev, u8*data);
int eswin_umbox_service_send_lpcpu(struct udevice* dev, u8* data);
int eswin_umbox_service_recv(struct udevice *dev, uint32_t *msg);
int eswin_umbox_service_free(struct udevice *dev);

#endif
