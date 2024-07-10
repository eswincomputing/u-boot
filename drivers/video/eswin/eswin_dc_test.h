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
 */

#ifndef _ESWIN_DC_TEST_H_
#define _ESWIN_DC_TEST_H_

struct dc8000_reg_tab{
	unsigned int offset;
	unsigned int value;
};

extern int dc_reg_test_read(void *regs);
extern int dc_reg_test_write(void *regs, unsigned int regVal);
extern int dc_test_writeback(void *regs, unsigned int scaler);

#endif
