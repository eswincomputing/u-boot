// SPDX-License-Identifier: GPL-2.0-only
/*
 * Transfer the som uart console through telenet
 *
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *   Authors:
 *    HuJiamiao<hujiamiao@eswincomputing.com>
 *
 */

#include <common.h>
#include <command.h>
#include <dm.h>
#include <malloc.h>
#include <mapmem.h>
#include <errno.h>
#include <asm/io.h>
#include <dm/root.h>
#include <dm/util.h>
#include <asm/mbox.h>
#include <common.h>
#include <dm/test.h>
#include <asm/mbox.h>
#include <test/test.h>
#include <test/ut.h>
#include <linux/delay.h>
struct unit_test_state global_dm_test_state;
static struct unit_test_state *uts = &global_dm_test_state;//added by xzq

void ut_failf(struct unit_test_state *uts, const char *fname, int line,
	      const char *func, const char *cond, const char *fmt, ...)
{
	va_list args;

	gd->flags &= ~(GD_FLG_SILENT | GD_FLG_RECORD);
	printf("%s:%d, %s(): %s: ", fname, line, func, cond);
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	putc('\n');
	uts->fail_count++;
}

static int do_mailbox_test(struct cmd_tbl *cmdtp, int flag, int argc,
			  char *const argv[])
{
	struct udevice *dev;
	uint32_t msg;

	printk("/*****====================uboot: start test lpcpu mailbox =================================*****/\r\n");

	ut_assertok(uclass_get_device_by_name(UCLASS_MISC, "mbox_srvc1@d0", &dev));

	ut_assertok(eswin_umbox_service_get(dev));
	// mdelay(5);
	// ut_asserteq(-ETIMEDOUT, eswin_umbox_service_recv(dev, &msg));
	// ut_assertok(eswin_umbox_service_send_lpcpu(dev, 0xaaff9955UL));
	// mdelay(5);
	// ut_assertok(eswin_umbox_service_recv(dev, &msg));
	// // ut_asserteq(msg, 0xaaff9955UL ^ WIN2030_MBOX_PING_XOR);
	// ut_asserteq(-ETIMEDOUT, eswin_umbox_service_recv(dev, &msg));


	// eswin_umbox_service_recv(dev, &msg);
	// eswin_umbox_service_send_lpcpu(dev, 0xaaff9955UL);
	// eswin_umbox_service_recv(dev, &msg);

	ut_assertok(eswin_umbox_service_free(dev));

	printk("uboot: mbox-test ok.\r\n");
	return 0;
}




static struct cmd_tbl test_commands[] = {
	U_BOOT_CMD_MKENT(mbox, 0, 1, do_mailbox_test, "", ""),
};



static int do_umbox(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	struct cmd_tbl *test_cmd;
	int ret;

	if (argc < 2)
		return CMD_RET_USAGE;
	test_cmd = find_cmd_tbl(argv[1], test_commands,
				ARRAY_SIZE(test_commands));
	argc -= 2;
	argv += 2;
	if (!test_cmd || argc > test_cmd->maxargs)
		return CMD_RET_USAGE;

	ret = test_cmd->cmd(test_cmd, flag, argc, argv);

	return cmd_process_error(test_cmd, ret);
}

U_BOOT_CMD(
	 umbox,	3,	1,	do_umbox,
	"Test mailbox in u-boot",
	"mbox             Test mailbox\n"
);
