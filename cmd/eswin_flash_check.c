// SPDX-License-Identifier: GPL-2.0
/*
 * ESWIN flash check driver
 *
 * Copyright 2024, Beijing ESWIN Computing Technology Co., Ltd.. All rights reserved.
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
 * Authors: Shenlin Liang <liangshenlin@eswincomputing.com>
 */

#include <common.h>
#include <command.h>
#include <spi_flash.h>
#include <dm.h>
#include <linux/types.h>

#define TEST_PATTERN 0xAA
#define TEST_SIZE    0x1000000 //flash max size

static int do_flash_check(struct cmd_tbl *cmdtp, int flag, int argc,
    char *const argv[])
{
    unsigned long flash_offset = 0x0;
    unsigned long test_size = TEST_SIZE;
    unsigned char *write_buf, *read_buf;
    int ret;

    if (argc > 1) {
        flash_offset = simple_strtoul(argv[1], NULL, 16);
    }
    if (argc > 2) {
        test_size = simple_strtoul(argv[2], NULL, 16);
    }

    printf("Starting SPI Flash verification...\n");
    printf("Flash offset: 0x%08lx, Test size: 0x%08lx\n", flash_offset, test_size);

    write_buf = malloc(test_size);
    read_buf = malloc(test_size);
    if (!write_buf || !read_buf) {
        printf("Failed to allocate memory\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    memset(write_buf, TEST_PATTERN, test_size);

    printf("Probing SPI Flash...\n");
    ret = run_command("bootspi probe 0", 0);
    if (ret) {
        printf("Failed to probe SPI Flash\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    ret = run_command("bootspi wp 0", 0);
    if (ret) {
        printf("Failed to probe SPI Flash\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    ret = run_command("sf probe 0", 0);
    if (ret) {
        printf("Failed to probe SPI Flash\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    printf("Erasing SPI Flash...\n");
    char erase_cmd[64];
    snprintf(erase_cmd, sizeof(erase_cmd), "sf erase 0x%lx +0x%lx", flash_offset, test_size);
    ret = run_command(erase_cmd, 0);
    if (ret) {
        printf("Failed to erase SPI Flash\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    printf("Writing test data to SPI Flash...\n");
    char write_cmd[64];
    snprintf(write_cmd, sizeof(write_cmd), "sf write 0x%p 0x%lx 0x%lx", write_buf,
        flash_offset, test_size);
    ret = run_command(write_cmd, 0);
    if (ret) {
        printf("Failed to write to SPI Flash\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    printf("Reading back data from SPI Flash...\n");
    char read_cmd[64];
    snprintf(read_cmd, sizeof(read_cmd), "sf read 0x%p 0x%lx 0x%lx", read_buf,
        flash_offset, test_size);
    ret = run_command(read_cmd, 0);
    if (ret) {
        printf("Failed to read from SPI Flash\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    printf("Verifying data...\n");
    if (memcmp(write_buf, read_buf, test_size) != 0) {
        printf("Data mismatch detected!\n");
        ret = CMD_RET_FAILURE;
        goto cleanup;
    }

    printf("SPI Flash verification successful!\n");
    ret = CMD_RET_SUCCESS;

cleanup:
    if (write_buf)
        free(write_buf);
    if (read_buf)
        free(read_buf);
    return ret;
}

U_BOOT_CMD(
    flash_check, 3, 0, do_flash_check,
    "Perform NOR Flash read/write test",
    "[<start_offset> <size>] - Test NOR Flash from <start_offset> for <size> bytes"
);