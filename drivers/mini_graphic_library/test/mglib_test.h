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
 * Authors: JackYaun <yuanxiaofeng@eswin.com>
 */

#ifndef __MGLIB_TEST_H__
#define __MGLIB_TEST_H__

#include "../mglib_driver.h"

err_no mgl_test_init(u32* osdBuf);
err_no eswin_mgl_draw_point_test(u32* osdBuf);
err_no eswin_mgl_draw_line_test(u32* osdBuf);
err_no eswin_mgl_draw_string_test(u32* osdBuf);
err_no eswin_mgl_draw_textbar_test(u32* osdBuf);
err_no eswin_mgl_draw_rect_test(u32* osdBuf);
err_no eswin_mgl_draw_progressBar_test(u32* osdBuf);
err_no eswin_mgl_draw_background_test(u32* osdBuf);
err_no mglib_test_all(u32* osdBuf);
err_no mgl_test_demo(void);

#endif
