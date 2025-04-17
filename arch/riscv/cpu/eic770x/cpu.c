// SPDX-License-Identifier: GPL-2.0-only
/*
 *
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 * Author: Xiang Xu <xuxiang@eswincomputing.com>
 */

#include <irq_func.h>
#include <asm/cache.h>
#include <env.h>
#include <stdio.h>
#include <linux/string.h>
#include <eswin/es_otp.h>

#define OTP_FT_FLAG_BIT (3364 * 8)
#define OTP_FT_1600M_FAIL_BIT (3256 * 8 + 5)

/*
 * cleanup_before_linux() is called just before we call linux
 * it prepares the processor for linux
 *
 * we disable interrupt and caches.
 */
int cleanup_before_linux(void)
{
	disable_interrupts();

	cache_flush();

	return 0;
}

void eswin_update_bootargs(void)
{
	const char *old_bootargs;
	char new_bootargs[1024] = {0};  // Allocate a large enough buffer to hold the old and new parameters
	const char *extra_args = " cpu_no_boost_1_6ghz";
	uint8_t bit_data = 1;
	int ret;

	ret = eswin_otp_read_bit(OTP_FT_FLAG_BIT, &bit_data);
	if (ERR_OK != ret) {
		printf("Failed to check OTP FT bit, ret %d\n", ret);
		return;
	}
	if (0 == bit_data) {
		printf("Chip has not undergone ATE wafer screening!\n");
		return;
	}

	bit_data = 0;
	ret = eswin_otp_read_bit(OTP_FT_1600M_FAIL_BIT, &bit_data);
	if (ERR_OK != ret) {
		printf("Chip to check OTP 1600M fail bit, ret %d\n", ret);
		return;
	}
	if (1 == bit_data) {
		printf("Cpu volatge need boost above 1.6 Ghz!\n");
		return;
	} else {
		printf("Cpu volatge no need boost at 1.6 Ghz!\n");
	}
	// Get the current bootargs
	old_bootargs = env_get("bootargs");
	// If bootargs exists, check if the new parameter is already present
	if (old_bootargs) {
		// Check if the current bootargs already contains the extra argument
		if (strstr(old_bootargs, extra_args) == NULL) {
			// If it doesn't contain the argument, append it
			strcpy(new_bootargs, old_bootargs);
			strcat(new_bootargs, extra_args);
		} else {
			// If it already contains the argument, just keep the existing bootargs
			strcpy(new_bootargs, old_bootargs);
		}
	} else {
		// If bootargs doesn't exist, use the new parameter directly
		strcpy(new_bootargs, extra_args);
	}
	// Set the new bootargs
	env_set("bootargs", new_bootargs);
}
