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
#include <common.h>
#include <dm.h>
#include <eswin/eswin-umbox-srvc.h>
#include <mailbox.h>
#include <malloc.h>

int eswin_umbox_service_get(struct udevice* dev)
{
	struct eswin_umbox_srvc* sbmt = dev_get_priv(dev);

	return mbox_get_by_name(dev, "tx", &sbmt->chan);
}

int eswin_umbox_service_send(struct udevice* dev, u8* data)
{
	int ret = 0;

	ret = ServiceSessionSendReq(dev, (REQ_SRVC_T*)data);
	if (ret)
		printk("eswin_umbox_srvc_send:ServiceSessionSendReq/Res failed.\r\n");
	return ret;
}

int eswin_umbox_service_recv(struct udevice* dev, uint32_t* msg)
{
	int ret = 0;
	struct eswin_umbox_srvc* sbmt = dev_get_priv(dev);

	ret = mbox_get_by_name(dev, "rx", &sbmt->chan);
	if (ret) {
		printk("mbox_get_by_name failed\r\n");
		return ret;
	}
	return mbox_recv(&sbmt->chan, msg, 100);  // call eswin_umbox_recv
}

int eswin_umbox_service_free(struct udevice* dev)
{
	struct eswin_umbox_srvc* sbmt = dev_get_priv(dev);

	return mbox_free(&sbmt->chan);
}

static const struct udevice_id eswin_umbox_srvc_ids[] = {
	{
		.compatible = "eswin,mbox-srvc"
	},
	{
	}
};

U_BOOT_DRIVER(eswin_umbox_srvc) = {
	.name = "eswin_umbox_srvc",
	.id = UCLASS_MISC,
	.of_match = eswin_umbox_srvc_ids,
	.priv_auto = sizeof(struct eswin_umbox_srvc),
};
