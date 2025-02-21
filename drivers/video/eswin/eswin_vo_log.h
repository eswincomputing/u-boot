// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * ESWIN VO driver
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
 * Authors: DengLei <denglei@eswincomputing.com>
 */

#ifndef __ESWIN_VO_LOG_H
#define __ESWIN_VO_LOG_H

#include <stdio.h>

#ifdef VO_DEBUG
#define LOG_LEAVEL 4
#else
#define LOG_LEAVEL 3
#endif

#define VO_PRINT_ERROR 1
#define VO_PRINT_WARN 2
#define VO_PRINT_INFO 3
#define VO_PRINT_DEBUG 4

#define vo_log(level, fmt, ...)                        \
	do {                                               \
		if (level <= LOG_LEAVEL)                       \
			printf("[VO][%s] %s:%d " fmt,                 \
            level == VO_PRINT_ERROR ? "ERROR" :        \
			level == VO_PRINT_WARN	? "WARNING" :      \
			level == VO_PRINT_INFO	? "INFO" :         \
			       "DEBUG",                            \
            __FUNCTION__, __LINE__, ##__VA_ARGS__);              \
	} while (0)

#define vo_err(fmt, ...) vo_log(VO_PRINT_ERROR, fmt, ##__VA_ARGS__);
#define vo_warn(fmt, ...) vo_log(VO_PRINT_WARN, fmt, ##__VA_ARGS__);
#define vo_info(fmt, ...) vo_log(VO_PRINT_INFO, fmt, ##__VA_ARGS__);
#define vo_debug(fmt, ...) vo_log(VO_PRINT_DEBUG, fmt, ##__VA_ARGS__);

#endif