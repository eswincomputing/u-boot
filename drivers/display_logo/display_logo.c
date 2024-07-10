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
 */

#include "common.h"
#include "display_logo.h"
#include "mmc.h"
#include "blk.h"
#include "malloc.h"
#include "dm/ofnode.h"

#include <dm.h>
#include <dm/of_access.h>
#include <dm/ofnode.h>

#ifndef USE_HOSTCC
#include <fdt.h>
#include <linux/libfdt.h>
#else
#include "fdt_host.h"
#endif

#define DEVTYPE "mmc"
#define DEVNUM 0

PICTURESRC_S picture;
RESOLUTION_S resolution_pic;
COLOR_FMT_E colorFmt_pic = ARGB8888;

err_no eswin_display_logo_get_logo_data(u8* osdBuf, RESOLUTION_S resolution_osd, COLOR_FMT_E colorFmt_osd)
{
    u8 devType = UCLASS_MMC;
    int devNum = DEVNUM;
    struct blk_desc* dev_desc;
    u32 ret = 0;
    u8* dataBuf = malloc(picture.blkCnt * picture.blkSize);

    if((resolution_osd.w != resolution_pic.w) || (resolution_osd.h != resolution_pic.h))
    {
        printf("display logo : %s : picture resolution is not suit osd resolution, please change picture\n", __func__);
        return ERR_INVAL;
    }

    if(colorFmt_osd != colorFmt_pic)
    {
        printf("display logo : %s : picture colorFmt is not suit osd colorFmt, please change picture\n", __func__);
        return ERR_INVAL;
    }

    dev_desc = blk_get_devnum_by_uclass_id(devType, devNum);
    ret = blk_dread(dev_desc, picture.blkStartAddr, picture.blkCnt, dataBuf);
    if (ret < 0)
    {
        printf("read picture data failed\n");
        return ERR_UNKNOWN;
    }
    memcpy(osdBuf, dataBuf, resolution_osd.w * resolution_osd.h * colorFmt_osd / 8);

    return RET_OK;
}

static int eswin_display_logo_probe(struct udevice *dev)
{
    ofnode node = dev->node_;
    const char* name = NULL;

    picture.blkStartAddr = ofnode_read_u32_default(node, "blk_start_addr", -1);
    if (-1 == picture.blkStartAddr)
    {
        printf("display_logo : %s : get_picture_info failed!\n", __func__);
        return ERR_UNKNOWN;
    }
    printf("****blkStartAddr = %x****\n", picture.blkStartAddr);
    picture.blkCnt = ofnode_read_u32_default(node, "blk_cnt", -1);
    if (-1 == picture.blkCnt)
    {
        printf("display_logo : %s : get_picture_info failed!\n", __func__);
        return ERR_UNKNOWN;
    }
    picture.blkSize = ofnode_read_u32_default(node, "blk_size", -1);
    if (-1 == picture.blkSize)
    {
        printf("display_logo : %s : get_picture_info failed!\n", __func__);
        return ERR_UNKNOWN;
    }
    printf("****blkCnt = %d****\n", picture.blkCnt);
    resolution_pic.w = ofnode_read_u32_default(node, "width", -1);
    if (-1 == resolution_pic.w)
    {
        printf("display_logo : %s : get_picture_info failed!\n", __func__);
        return ERR_UNKNOWN;
    }
    printf("****resolution.w = %d****\n", resolution_pic.w);
    resolution_pic.h = ofnode_read_u32_default(node, "height", -1);
    if (-1 == resolution_pic.h)
    {
        printf("display_logo : %s : get_picture_info failed!\n", __func__);
        return ERR_UNKNOWN;
    }
    printf("****resolution.h = %d****\n", resolution_pic.h);

    u32 ret = 0;
    ret = ofnode_read_string_index(node, "color_format", 0, &name);
    printf("****colorFmt = %s****\n", name);
    if (!ret)
    {
        if(!strcmp(name, "ARGB8888"))
            colorFmt_pic = ARGB8888;
        else if(!strcmp(name, "RGB888"))
            colorFmt_pic = RGB888;
        else if(!strcmp(name, "RGB565"))
            colorFmt_pic = RGB565;
        else
        {
            printf("display_logo : %s : invalid param!\n",__func__);
            return ERR_INVAL;
        }
    }
    return RET_OK;
}

int eswin_display_logo_bind(struct udevice *dev)
{
    printf("*****************display logo test bind ****************\n");
    return 0;
}

static const struct udevice_id eswin_display_logo_ids[] =
{
    { .compatible = "eswin,display-logo"},
    { }
};

U_BOOT_DRIVER(eswin_display_logo) =
{
    .name   = "eswin_display_logo",
    .id = UCLASS_DISPLAY,
    .of_match = eswin_display_logo_ids,
    .bind   = eswin_display_logo_bind,
    .probe  = eswin_display_logo_probe,
};
