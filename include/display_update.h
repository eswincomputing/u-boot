// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef _DISPLAY_UPDATE_H
#define _DISPLAY_UPDATE_H

#include <common.h>
#include <u-boot/crc.h>
#include <linux/err.h>
#include <part.h>

int draw_progressBar(u16 percentage);
int draw_updateinfo(u32 textBar_x, u32 textBar_y, char* versionInfo_text);

#endif /* _DISPLAY_UPDATE_H */
