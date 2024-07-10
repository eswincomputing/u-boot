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

#include <common.h>
#include <command.h>
#include <dm/device.h>
#include <dm/read.h>
#include <dm/device_compat.h>
#include <dm/devres.h>
#include <asm/bitops.h>
#include <eswin/ipc.h>
#include <eswin/eswin-uMsg.h>
#include <eswin/eswin-service.h>
#include <linux/ioport.h>
#include <misc.h>
#include <dt-bindings/soc/eswin-pmp.h>
#include <asm/mbox.h>

#define MAX_REGION_CNT		(16)
#define INTERLEAVE_DDR_START	(0x4000000000)
#define FLAT_DIE0_DDR_START	(0x80000000)
#define FLAT_DIE1_DDR_START	(0x2000000000)

enum pmp_check_type {
	/* Check write access */
	CHECK_WRITE = 0x1,
	/* Check read access */
	CHECK_READ = 0x2,
	/* Check read and write access */
	CHECK_READ_WRITE = 0x3,
	RESV             = 0
};

/*
  size = size_cfg ^ 2
*/
enum pmp_region_size_cfg {
	SIZE_CFG_32K = 0xE,
	SIZE_CFG_64K,
	SIZE_CFG_128K,
	SIZE_CFG_256K,
	SIZE_CFG_512K,
	SIZE_CFG_1M,
	SIZE_CFG_2M,
	SIZE_CFG_4M,
	SIZE_CFG_8M,
	SIZE_CFG_16M,
	SIZE_CFG_32M,
	SIZE_CFG_64M,
	SIZE_CFG_128M,
	SIZE_CFG_256M,
	SIZE_CFG_512M,
	SIZE_CFG_1G,
	SIZE_CFG_2G,
	SIZE_CFG_4G,
	SIZE_CFG_8G,
	SIZE_CFG_16G,
	SIZE_CFG_32G,
	SIZE_CFG_64G,
	SIZE_CFG_128G,
	SIZE_CFG_256G,
	SIZE_CFG_512G,
	SIZE_CFG_1T,
	/*
	  ....
	*/
	SIZE_CFG_MAX = 0x40
};
struct eswin_pmp {
	u32 node_id;
	u32 mem_mode;
};

/* Configuration for a PMP region */
struct pmp_config {
	enum pmp_check_type check_type        : 2;
	u8                _rsv0             : 6;
	u8                region_size       : 6;
	u8                _rsv1             : 2;
	u8                subregion_disable : 8;
	u8                _rsv2             : 7;
	u8                region_permission : 1;
};
/*
void zebu_stop()
{
	volatile u32 *addr;
	printf("[%s %d]: stop zebu!\n", __func__,__LINE__);
	addr = (0x51810000 + 0x668);
	*addr = 0x8000;
}
*/
int eswin_pmp_init(void)
{
	struct udevice *pmp_dev;

	uclass_get_device_by_name(UCLASS_MISC, "pmp@51b48000", &pmp_dev);

	uclass_get_device_by_name(UCLASS_MISC, "pmp@71b48000", &pmp_dev);
	return 0;
}
static int eswin_pmp_umbox_service(struct udevice *_dev,
	REQ_SRVC_T* req_t, RECV_MSG_EXT_T* res_t)
{
	struct udevice *mbox_servc_dev;
	int ret = 0;

	ret = uclass_get_device_by_phandle(UCLASS_MISC, _dev,
		"mbox-service", &mbox_servc_dev);
	if (ret) {
		dev_err(_dev, "%s %d, failed to get mbox-service dev, ret %d!\r\n",
			__func__,__LINE__, ret);
		return ret;
	}
	ret = eswin_umbox_service_get(mbox_servc_dev);
	if (ret) {
		dev_err(_dev, "%s %d, failed to get mbox-service, ret %d!\r\n",
			__func__,__LINE__, ret);
		return ret;
	}

	/* send msg to bootrom */
	ret = eswin_umbox_service_send(mbox_servc_dev, (u8*)req_t);
	if (ret) {
		dev_err(_dev, "%s %d, failed to send msg, ret %d!\r\n",
			__func__,__LINE__, ret);
		goto Failed;
	}
	/* receive msg from bootrom */
	ret = eswin_umbox_service_recv(mbox_servc_dev, (u32*)res_t);
	if (ret) {
		dev_err(_dev, "%s %d, failed to recv msg, ret %d!\r\n",
			__func__,__LINE__, ret);
		goto Failed;
	}
	if (0 != res_t->ServiceStatus || 0 != res_t->IpcStatus) {
		dev_err(_dev, "%s %d, mbox service failed\r\n",
			__func__,__LINE__);
		dev_err(_dev, "exec status:\r\n");
		dev_err(_dev, "             service_id     = [0x%08x]\r\n", res_t->dwServiceID);
		dev_err(_dev, "             status         = [0x%08x]\r\n", res_t->IpcStatus);
		dev_err(_dev, "             service_status = [0x%08x]\r\n", res_t->ServiceStatus);
		dev_err(_dev, "             data_len       = [0x%08x]\r\n", res_t->size);
		ret = -EPERM;
	}
Failed:
	eswin_umbox_service_free(mbox_servc_dev);
	return ret;
}

/**
 * int_pow - computes the exponentiation of the given base and exponent
 * @base: base which will be raised to the given power
 * @exp: power to be raised to
 *
 * Computes: pow(base, exp), i.e. @base raised to the @exp power
 */
static u64 int_pow(u64 base, unsigned int exp)
{
	u64 result = 1;

	while (exp) {
		if (exp & 1)
			result *= base;
		exp >>= 1;
		base *= base;
	}

	return result;
}

static int eswin_pmp_do_region_config(struct udevice *_dev,
	struct pmp_config cfg, u64 addr)
{
	int ret = 0;
	REQ_SRVC_T srvcReq;
	RECV_MSG_EXT_T recvMsg;
	u64 pmp_addr;
	struct eswin_pmp *esw_pmp = dev_get_priv(_dev);

	memset((u8 *)&srvcReq, 0, sizeof(REQ_SRVC_T));

	srvcReq.SrvcType                = SRVC_TYPE_ADDR_REGION_PROTECTION;
	/*Let bootrom to allocate the region id*/
	srvcReq.data.AddrRegProReq.region = 0xff;

	if (MEM_MODE_INTERLEAVE == esw_pmp->mem_mode) {
		pmp_addr = addr - INTERLEAVE_DDR_START;
	} else if (MEM_MODE_FLAT == esw_pmp->mem_mode) {
		pmp_addr = 0 == esw_pmp->node_id ? addr - FLAT_DIE0_DDR_START : \
			addr - FLAT_DIE1_DDR_START;
	} else {
		dev_err(_dev, "%s %d, unknow memory mode %d!\r\n",
			__func__, __LINE__, esw_pmp->mem_mode);
		return -EINVAL;
	}
	srvcReq.data.AddrRegProReq.addr_h = (pmp_addr >> 32) & 0xffffffff;
	srvcReq.data.AddrRegProReq.addr_l = pmp_addr & 0xffffffff;
	memcpy((uchar *)&srvcReq.data.AddrRegProReq.config,
		(uchar *)&cfg, sizeof(struct pmp_config));

	ret = eswin_pmp_umbox_service(_dev, &srvcReq, &recvMsg);
	if (ret) {
		dev_err(_dev, "%s %d, failed to do pmp region config, ret %d!\r\n",
			__func__,__LINE__, ret);
		return ret;
	}
	return 0;
}

static int eswin_pmp_get_size_cfg(struct resource *res)
{
	int i;
	u64 size;

	size = resource_size(res);
	for (i = SIZE_CFG_32K; i < SIZE_CFG_MAX; i++) {
		if (int_pow(2, i + 1) == size) {
			return i;
		}
	}
	return -EINVAL;
}

/*
   1.region size must be size_cfg ^ 2 (SIZE_CFG_32K <= size_cfg <  SIZE_CFG_MAX)
   2.region start addr must align to region size.
*/
static int eswin_pmp_region_addr_check(struct udevice *_dev, struct resource *res)
{
	u64 addr, size;
	int size_cfg;
	u64 ddr_addr;
	struct eswin_pmp *esw_pmp = dev_get_priv(_dev);

	addr = res->start;

	if (MEM_MODE_INTERLEAVE == esw_pmp->mem_mode) {
		if (addr < INTERLEAVE_DDR_START) {
			dev_err(_dev, "%s %d, addr 0x%llx is underflow ddr addr 0x%lx!\n",
				__func__,__LINE__, addr, INTERLEAVE_DDR_START);
			return -EINVAL;
		}
	} else {
		ddr_addr = 0 == esw_pmp->node_id ? FLAT_DIE0_DDR_START : FLAT_DIE1_DDR_START;
		if (addr < ddr_addr) {
			dev_err(_dev, "%s %d, addr 0x%llx is underflow ddr addr 0x%llx!\n",
				__func__,__LINE__, addr, ddr_addr);
			return -EINVAL;
		}
	}
	size_cfg = eswin_pmp_get_size_cfg(res);
	if (size_cfg > 0) {
		size = resource_size(res);
		if (IS_ALIGNED(addr, size)) {
			return 0;
		}
	}
	return -EINVAL;
}

static int eswin_pmp_region_init(struct udevice *_dev)
{
	ofnode node;
	struct resource res;
	int ret;
	u64 addr;
	struct pmp_config cfg;
	int count = 0;

	memset(&node, 0, sizeof(ofnode));
	/*region legality check*/
	dev_for_each_subnode(node, _dev) {
		if (ofnode_device_is_compatible(node, "pmp-sec-region")) {
			ret = ofnode_read_resource(node, 0, &res);
			if (ret) {
				dev_err(_dev, "%s %d, no memory address assigned "
					"to the region!\n", __func__, __LINE__);
				return -ENOMEM;
			}
			ret = eswin_pmp_region_addr_check(_dev, &res);
			if (ret) {
				dev_err(_dev, "%s %d, region memory address "
					"is illegal, addr 0x%lx, size 0x%lx!\n",
					__func__, __LINE__, res.start, resource_size(&res));
				return -EINVAL;
			}
			count++;
		}
	}
	if (count > MAX_REGION_CNT) {
		dev_err(_dev, "%s %d, total region count %d exceed "
			"the region count limit %d!\n", __func__, __LINE__, count,
			MAX_REGION_CNT);
		return -E2BIG;
	}
	dev_for_each_subnode(node, _dev) {
		if (ofnode_device_is_compatible(node, "pmp-sec-region")) {
			ret = ofnode_read_resource(node, 0, &res);
			if (ret) {
				dev_err(_dev, "%s %d, no memory address "
					"assigned to the region\n",__func__, __LINE__);
				return -ENOMEM;
			}
			memset(&cfg, 0, sizeof(struct pmp_config));
			addr = res.start;
			cfg.check_type = CHECK_READ_WRITE;
			cfg.region_size = eswin_pmp_get_size_cfg(&res);
			cfg.subregion_disable = 0; /*check the whole region*/
			cfg.region_permission = SECURE; /*only allow secure master access*/
			ret = eswin_pmp_do_region_config(_dev, cfg, addr);
			if (0 != ret) {
				dev_err(_dev, "%s %d, error while config pmp region probe, "
					"addr 0x%llx, size 0x%x\n", __func__, __LINE__,
					addr, cfg.region_size);
				return ret;
			}
			printf("PMP region 0x%lx--0x%lx init\n", res.start, res.end);
		}
	}
	return 0;
}

static int eswin_pmp_do_axprot_config(struct udevice *_dev,
	u32 dwMasterID, int security)
{
	int ret = 0;
	REQ_SRVC_T srvcReq;
	RECV_MSG_EXT_T recvMsg;
	u32 dwNewConfig = 0;
	int i;

	srvcReq.SrvcType = SRVC_TYPE_AXPROT;

	/* read current axprot config */
	srvcReq.data.AxprotReq.flag.bRw = BASIC_IO_READ;
	srvcReq.data.AxprotReq.dwMasterID = dwMasterID;

	ret = eswin_pmp_umbox_service(_dev, &srvcReq, &recvMsg);
	if (0 != ret) {
		dev_err(_dev, "%s %d, Master %d failed to read axprot config!\r\n",
			__func__,__LINE__, dwMasterID);
		return ret;
	}

	dev_dbg(_dev, "%s %d, Master %d axprot config = [0x%x].\r\n",
		__func__,__LINE__, dwMasterID, recvMsg.data_t.AxprotRes.dwConfig);
	dwNewConfig = recvMsg.data_t.AxprotRes.dwConfig;
	/*
	        BIT0            BIT1          BIT2
	  unprivileged access/secure-access/data-access
	  BIT1 determines the port's secure property
	  One master may have serval ports(mp/sp0/sp1)
	*/
	for (i = 0; i < 32; i += 4) {
		if (SECURE == security) {
			__clear_bit(i + 1, &dwNewConfig);
		} else {
			__set_bit(i + 1, &dwNewConfig);
		}
	}
	dev_dbg(_dev, "%s %d, Master %d axprot config = [0x%x].\r\n",
		__func__,__LINE__, dwMasterID, dwNewConfig);

	srvcReq.data.AxprotReq.flag.bRw = BASIC_IO_WRITE;
	srvcReq.data.AxprotReq.dwMasterID = dwMasterID;
	srvcReq.data.AxprotReq.dwConfig = dwNewConfig;

	ret = eswin_pmp_umbox_service(_dev, &srvcReq, &recvMsg);
	if (0 != ret) {
		dev_err(_dev, "%s %d, Master %d failed to write axprot config 0x%x!\r\n",
			__func__,__LINE__, dwMasterID, dwNewConfig);
		return ret;
	}
	return 0;
}

static int eswin_pmp_axprot_init(struct udevice *_dev)
{
	int ret, i;
	u32 *values_tab;
	u32 dwMasterID;
	int security;
	int size;

	size = dev_read_size(_dev, "master-security");
	if (size <= 0) {
		dev_err(_dev, "%s %d, master-security not exixt or size is zero, size %d!\n",__func__,__LINE__, size);
		return size;
	}
	size /= sizeof(u32) * 2;
	values_tab = devm_kcalloc(_dev, size * 2, sizeof(u32), GFP_KERNEL);
	if (!values_tab) {
		dev_err(_dev, "%s %d, failed to alloc memory, size %ld!\n",
			__func__,__LINE__, size * 2 * sizeof(u32));
		return -ENOMEM;
	}

	ret = dev_read_u32_array(_dev, "master-security", values_tab, size * 2);
	if (ret) {
		dev_err(_dev, "%s %d, error while parsing master-security "
			"bitfield, ret %d!\n",__func__,__LINE__, ret);
		return ret;
	}
	for (i = 0; i < size; i++) {
		dwMasterID = values_tab[2 * i];
		security = values_tab[2 * i + 1];
		ret = eswin_pmp_do_axprot_config(_dev, dwMasterID, security);
		if (ret) {
			dev_err(_dev, "%s %d, error while config axprot, master %d, "
				"security %d, ret %d!\n",__func__,__LINE__,
				dwMasterID , security, ret);
			return ret;
		}
	}
	devm_kfree(_dev, values_tab);
	return ret;
}

static int eswin_pmp_probe(struct udevice *_dev)
{
	int ret;
	struct eswin_pmp *esw_pmp = dev_get_priv(_dev);

	ret = dev_read_u32(_dev, "numa-node-id", &esw_pmp->node_id);
	if (ret) {
		dev_err(_dev, "%s %d, failed to get node id, ret %d!\r\n",
			__func__,__LINE__, ret);
		return ret;
	}
	ret = dev_read_u32(_dev, "memory-mode", &esw_pmp->mem_mode);
	if (ret) {
		dev_err(_dev, "%s %d, failed to get node id, ret %d!\r\n",
			__func__,__LINE__, ret);
		return ret;
	}
	ret = eswin_pmp_axprot_init(_dev);
	if (ret) {
		dev_err(_dev, "%s %d, error while config axprot, ret %d!\n",
			__func__,__LINE__, ret);
		return ret;
	}
	ret = eswin_pmp_region_init(_dev);
	if (ret) {
		dev_err(_dev, "%s %d, error while config pmp region, ret %d!\n",
			__func__,__LINE__, ret);
		return ret;
	}
	printf("Die %d PMP init done, memory mode %s!\n", esw_pmp->node_id,
		esw_pmp->mem_mode == MEM_MODE_FLAT ? "FLAT" : "INTERLEAVE");
	return 0;
}

static const struct udevice_id eswin_pmp_ids[] = {
	{.compatible = "eswin,pmp-manager" },
	{}
};

U_BOOT_DRIVER(eswin_pmp) = {
	.name = "eswin_pmp",
	.id = UCLASS_MISC,
	.of_match = eswin_pmp_ids,
	.probe = eswin_pmp_probe,
	.priv_auto = sizeof(struct eswin_pmp),
};

static int do_upmp(struct cmd_tbl* cmdtp, int flag, int argc, char* const argv[])
{
	return eswin_pmp_init();
}

U_BOOT_CMD(upmp, 10, 1, do_upmp, "pmp cmd", "cmd use to config pmp\n");
