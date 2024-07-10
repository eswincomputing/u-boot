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

#include <common.h>
#include <linux/delay.h>
#include <malloc.h>
#include "eswin_dc_test.h"
#include "eswin_dc_reg.h"

#define CONFIG_BPP32	1
#define TEST_VIDEO 		1
#define TEST_SCALER 	1

#if TEST_SCALER
#define TEST_FRAME_WIDTH	400
#define TEST_FRAME_HIGHT	400
#define TEST_DEST_WIDTH		1024
#define TEST_DEST_HIGHT		600
#else
#define TEST_FRAME_WIDTH    720
#define TEST_FRAME_HIGHT    480
#define TEST_DEST_WIDTH 	720
#define TEST_DEST_HIGHT 	480
#endif

#if CONFIG_BPP32
#define TEST_FRAME_BPP		32	//argb8888
#else
#define TEST_FRAME_BPP		16	//rgb565
#endif

#define REG_OVERLAY_SRC_BUFFER_ADDR0	0x000015c0
#define REG_VIDEO_SRC_BUFFER_ADDR0		0x00001400
#define REG_DST1_BUFFER_ADDR0			0x000014e8
#define REG_DST2_BUFFER_ADDR0			0x000014f0
#define REG_VIDEO_VERTI_KERNEL_INDEX 	0x00001a08
#define REG_OVERLAY_VERTI_KERNEL_INDEX	0x00001b40

#define DEST_BUF_ADDR		(0x80000000U + (32 * 1024 * 1024))

#if CONFIG_BPP32
unsigned int dc8000_input_video_image_buf[TEST_FRAME_WIDTH * TEST_FRAME_HIGHT] __attribute__((aligned(256))) = {0x00989898};
unsigned int dc8000_input_overlay_image_buf[TEST_FRAME_WIDTH * TEST_FRAME_HIGHT] __attribute__((aligned(256))) = {0x0};
#else
unsigned short dc8000_input_video_image_buf[TEST_FRAME_WIDTH * TEST_FRAME_HIGHT] __attribute__((aligned(256))) = {0xf800};
unsigned short dc8000_input_overlay_image_buf[TEST_FRAME_WIDTH * TEST_FRAME_HIGHT] __attribute__((aligned(256))) = {0x0};
#endif

static unsigned int horKernel[] =
{
    0x00000000, 0x20000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x23fd1c03, 0x00000000,
    0x00000000, 0x00000000, 0x181f0000, 0x000027e1, 0x00000000, 0x00000000, 0x00000000, 0x2b981468,
    0x00000000, 0x00000000, 0x00000000, 0x10f00000, 0x00002f10, 0x00000000, 0x00000000, 0x00000000,
    0x32390dc7, 0x00000000, 0x00000000, 0x00000000, 0x0af50000, 0x0000350b, 0x00000000, 0x00000000,
    0x00000000, 0x3781087f, 0x00000000, 0x00000000, 0x00000000, 0x06660000, 0x0000399a, 0x00000000,
    0x00000000, 0x00000000, 0x3b5904a7, 0x00000000, 0x00000000, 0x00000000, 0x033c0000, 0x00003cc4,
    0x00000000, 0x00000000, 0x00000000, 0x3de1021f, 0x00000000, 0x00000000, 0x00000000, 0x01470000,
    0x00003eb9, 0x00000000, 0x00000000, 0x00000000, 0x3f5300ad, 0x00000000, 0x00000000, 0x00000000,
    0x00480000, 0x00003fb8, 0x00000000, 0x00000000, 0x00000000, 0x3fef0011, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00004000, 0x00000000, 0x00000000, 0x00000000, 0x20002000, 0x00000000,
    0x00000000, 0x00000000, 0x1c030000, 0x000023fd, 0x00000000, 0x00000000, 0x00000000, 0x27e1181f,
    0x00000000, 0x00000000, 0x00000000, 0x14680000, 0x00002b98, 0x00000000, 0x00000000, 0x00000000,
    0x2f1010f0, 0x00000000, 0x00000000, 0x00000000, 0x0dc70000, 0x00003239, 0x00000000, 0x00000000,
    0x00000000, 0x350b0af5, 0x00000000, 0x00000000, 0x00000000, 0x087f0000, 0x00003781, 0x00000000,
    0x00000000, 0x00000000, 0x399a0666, 0x00000000, 0x00000000, 0x00000000, 0x04a70000, 0x00003b59,
    0x00000000, 0x00000000, 0x00000000, 0x3cc4033c, 0x00000000, 0x00000000, 0x00000000, 0x021f0000,
};

static unsigned int verKernel[] =
{
    0x00000000, 0x20000000, 0x00002000, 0x00000000, 0x00000000, 0x00000000, 0x23fd1c03, 0x00000000,
    0x00000000, 0x00000000, 0x181f0000, 0x000027e1, 0x00000000, 0x00000000, 0x00000000, 0x2b981468,
    0x00000000, 0x00000000, 0x00000000, 0x10f00000, 0x00002f10, 0x00000000, 0x00000000, 0x00000000,
    0x32390dc7, 0x00000000, 0x00000000, 0x00000000, 0x0af50000, 0x0000350b, 0x00000000, 0x00000000,
    0x00000000, 0x3781087f, 0x00000000, 0x00000000, 0x00000000, 0x06660000, 0x0000399a, 0x00000000,
    0x00000000, 0x00000000, 0x3b5904a7, 0x00000000, 0x00000000, 0x00000000, 0x033c0000, 0x00003cc4,
    0x00000000, 0x00000000, 0x00000000, 0x3de1021f, 0x00000000, 0x00000000, 0x00000000, 0x01470000,
    0x00003eb9, 0x00000000, 0x00000000, 0x00000000, 0x3f5300ad, 0x00000000, 0x00000000, 0x00000000,
    0x00480000, 0x00003fb8, 0x00000000, 0x00000000, 0x00000000, 0x3fef0011, 0x00000000, 0x00000000,
    0x00000000, 0x00000000, 0x00004000, 0x00000000, 0xcdcd0000, 0xfdfdfdfd, 0xabababab, 0xabababab,
    0x00000000, 0x00000000, 0x5ff5f456, 0x000f5f58, 0x02cc6c78, 0x02cc0c28, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
    0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee, 0xfeeefeee,
};

//dc8k base addr:0x502c0000
static struct dc8000_reg_tab regTab[] = {
	/* dc8000 reset */
	{0x00001518, 0x00000010}, {0x00001518, 0x00000000},

	{0x00001480, 0x00000001}, {0x0000080c, 0x00000000}, {0x00000810, 0x00000000}, {0x00000814, 0x0000ffff}, {0x00000800, 0x021d0188}, 
	{0x00000804, 0x00080000}, {0x00000808, 0x003fc810}, 
	{0x00000a80, 0xffffffff}, {0x00000b80, 0xffffffff}, {0x00001080, 0xffffffff}, {0x00001300, 0x00000000}, {0x00001380, 0x00000000},
	{0x00000a84, 0xffffffff}, {0x00000b84, 0xffffffff}, {0x00001084, 0xffffffff}, {0x00001304, 0x00000000}, {0x00001384, 0x00000000},
	{0x00000a88, 0xffffffff}, {0x00000b88, 0xffffffff}, {0x00001088, 0xffffffff}, {0x00001308, 0x00000000}, {0x00001388, 0x00000000},
	{0x00000a8c, 0xffffffff}, {0x00000b8c, 0xffffffff}, {0x0000108c, 0xffffffff}, {0x0000130c, 0x00000000}, {0x0000138c, 0x00000000},
	{0x00000a90, 0xffffffff}, {0x00000b90, 0xffffffff}, {0x00001090, 0xffffffff}, {0x00001310, 0x00000000}, {0x00001390, 0x00000000},
	{0x00000a94, 0xffffffff}, {0x00000b94, 0xffffffff}, {0x00001094, 0xffffffff}, {0x00001314, 0x00000000}, {0x00001394, 0x00000000},
	{0x00000a98, 0xffffffff}, {0x00000b98, 0xffffffff}, {0x00001098, 0xffffffff}, {0x00001318, 0x00000000}, {0x00001398, 0x00000000},
	{0x00000a9c, 0xffffffff}, {0x00000b9c, 0xffffffff}, {0x0000109c, 0xffffffff}, {0x0000131c, 0x00000000}, {0x0000139c, 0x00000000},
	{0x00000aa0, 0xffffffff}, {0x00000ba0, 0xffffffff}, {0x000010a0, 0xffffffff}, {0x00001320, 0x00000000}, {0x000013a0, 0x00000000},
	{0x00000aa4, 0xffffffff}, {0x00000ba4, 0xffffffff}, {0x000010a4, 0xffffffff}, {0x00001324, 0x00000000}, {0x000013a4, 0x00000000},
	{0x00000aa8, 0xffffffff}, {0x00000ba8, 0xffffffff}, {0x000010a8, 0xffffffff}, {0x00001328, 0x00000000}, {0x000013a8, 0x00000000},
	{0x00000aac, 0xffffffff}, {0x00000bac, 0xffffffff}, {0x000010ac, 0xffffffff}, {0x0000132c, 0x00000000}, {0x000013ac, 0x00000000},
	{0x00000ab0, 0xffffffff}, {0x00000bb0, 0xffffffff}, {0x000010b0, 0xffffffff}, {0x00001330, 0x00000000}, {0x000013b0, 0x00000000},
	{0x00000ab4, 0xffffffff}, {0x00000bb4, 0xffffffff}, {0x000010b4, 0xffffffff}, {0x00001334, 0x00000000}, {0x000013b4, 0x00000000},
	{0x00000ab8, 0xffffffff}, {0x00000bb8, 0xffffffff}, {0x000010b8, 0xffffffff}, {0x00001338, 0x00000000}, {0x000013b8, 0x00000000},
	{0x00000abc, 0xffffffff}, {0x00000bbc, 0xffffffff}, {0x000010bc, 0xffffffff}, {0x0000133c, 0x00000000}, {0x000013bc, 0x00000000},
	{0x00000880, 0x00020000}, {0x00000884, 0x00020000}, {0x00000888, 0x00020000}, {0x0000088c, 0x00020000},
	{0x00000890, 0x00020000}, {0x00000894, 0x00020000}, {0x00000898, 0x00020000}, {0x0000089c, 0x00020000}, 
	{0x000008a0, 0x00020000}, {0x000008a4, 0x00020000}, {0x000008a8, 0x00020000}, {0x000008ac, 0x00020000}, 
	{0x000008b0, 0x00020000}, {0x000008b4, 0x00020000}, {0x000008b8, 0x00020000}, {0x000008bc, 0x00020000},

#if TEST_VIDEO   //video
	#if TEST_SCALER	//video scaler
    /* display registers */
    {0x00001400, 0xffffffff},  //framebuffer0 address
    {0x00001408, 0x00000680},  //framebuffer stride, in bytes(width = 640 x 4 = 2560)[0x00000a00, 0x00000b40]
    {0x00001520, 0x00000033}, {0x00001828, 0x000063d8}, {0x00001830, 0x0000aa86}, //framebuffer scaler and framebuffer background
    {0x00001810, 0x00c80190}, {0x00001820, 0x00ffffff}, //framebuffer0 size, (width = 640 x 480)[0x00f00280, 0x00f002d0]
    {0x00001a20, 0x80008000}, {0x00001410, 0x00000000}, {0x00001580, 0x00002040},	//framebuffer0 initialOffset and dither config
    {0x00001430, 0x04a00400},   //display HDisplay(h_display[14:0]:640, h_total[30:16]:800)[0x03200280, 0x035a02d0]
    {0x00001438, 0x42380410},   //display HSync(start[14:0]:656, End[29:15]:752, Len_hsync:96)[0x41780290, 0x418f02e0]
    {0x00001440, 0x06d10258},   //display VDisplay(v_display[14:0]:480, v_total[30:16]:525)[0x020d01e0, 0x020d01e0]
    {0x00001448, 0x41320262},   //display VSync(start[14:0]:490, End[29:15]:492, Len_vsync:2)[0x40f601ea, 0x40f601ea]

    {0x000014e8, 0x00000000},   //dc8000 writeback memory address
    {0x000014f0, 0x00000000},   //dc8000 writeback panel mem address
    {0x00001500, 0x00001000},   //dc8000 writeback mem stride[0x00000a00, 0x00000b40]
    {0x000014f8, 0x00030000},   //dc8000 writeback enable
	{0x00001cb0, 0x00000000}, {0x00001cb8, 0x00000000}, {0x0000146c, 0xfff5c000}, {0x00001474, 0x00345678}, {0x00001478, 0x00654321},
    {0x00001418, 0x00000111}, {0x000014b8, 0x00000005}, {0x00001468, 0x00000000}, {0x00001484, 0x00000001},
	{0x00001e20, 0x15132827}, {0x00001e28, 0x046c02c5}, {0x00001e30, 0x00bb3ad9}, {0x00001e38, 0x05a2010d}, {0x00001e40, 0x00003952},
    {0x000014b0, 0x00000000}, {0x00001508, 0x00000000}, {0x00001510, 0x00000000}, {0x00001a28, 0x00000e03}, {0x00001cc8, 0x00000000},
    {0x00001cd0, 0x00000000}, {0x00001ce8, 0x00000000}, {0x000024d0, 0x00000000}, {0x00001cc0, 0x00000010}, {0x00001838, 0x00000000},
	{0x00001838, 0x00000000}, {0x00001a08, 0x00000000}, {0x00001ac0, 0x00000000}, {0x00001b40, 0x00000000}, {0x00002430, 0x00000000},
    {0x00001420, 0x00000000}, {0x00001428, 0x00000000}, {0x00001518, 0x18404011},  //dc8000 framebuffer config (enable output)
	#else	//video layer
	/* display registers */
	{0x00001540, 0x00060020}, {0x00001400, 0xffffffff},	//overlay0 config and framebuffer0 address
	{0x00001408, 0x00000b40},	//framebuffer stride, in bytes(width = 640 x 4 = 2560)[0x00000a00, 0x00000b40]
	{0x00001520, 0x00000033}, {0x00001528, 0x00000000}, //framebuffer scaler and framebuffer background
	{0x00001810, 0x00f002d0},	//framebuffer0 size, (width = 640 x 480)[0x00f00280, 0x00f002d0]
	{0x00001a20, 0x1f401f40}, {0x00001410, 0x00000000},	//framebuffer0 initialOffset and dither config
	{0x00001430, 0x035a02d0},	//display HDisplay(h_display[14:0]:640, h_total[30:16]:800)[0x03200280, 0x035a02d0]
	{0x00001438, 0x418f02e0},	//display HSync(start[14:0]:656, End[29:15]:752, Len_hsync:96)[0x41780290, 0x418f02e0]
	{0x00001440, 0x065901e0},	//display VDisplay(v_display[14:0]:480, v_total[30:16]:525)[0x020d01e0, 0x020d01e0]
	{0x00001448, 0x40f601ea},	//display VSync(start[14:0]:490, End[29:15]:492, Len_vsync:2)[0x40f601ea, 0x40f601ea]
	
	{0x000014e8, 0x00000000},	//dc8000 writeback memory address
	{0x000014f0, 0x00000000},	//dc8000 writeback panel mem address
	{0x00001500, 0x00000b40},	//dc8000 writeback mem stride[0x00000a00, 0x00000b40]
	{0x000014f8, 0x00030000},	//dc8000 writeback enable

	{0x00001418, 0x00000111}, {0x000014b8, 0x00000005}, {0x00001468, 0x00023344}, {0x00001484, 0x00000000},
	{0x000014b0, 0x00000000}, {0x00001508, 0x1d2a2915}, {0x00001510, 0xebdacdcf}, {0x00001a28, 0x00000000}, {0x00001cc8, 0x0000003d},
	{0x00001cd0, 0x00000000}, {0x00001ce8, 0x000f7483},	{0x000024d0, 0x00000009}, {0x00001cc0, 0x00000010}, {0x00001838, 0x00000000},
	{0x00001a08, 0x00000000}, {0x00001ac0, 0x00000000}, {0x00001b40, 0x00000000}, {0x00001e98, 0x00000000}, {0x00002430, 0x00000001},
	{0x00001420, 0x6e4ca280}, {0x00001428, 0x5d7f91b3}, {0x00001518, 0x18004011},	//dc8000 framebuffer config (enable output)
	#endif
#else	//overlay
	#if TEST_SCALER		//overlay scaler
	/* display registers */
	{0x00001400, 0xffffffff}, {0x000015c0, 0xffffffff},	//overlay0 config , video0 and overlay0 address
	{0x00001408, 0x00000680}, {0x00001520, 0x00000033}, {0x00001528, 0x00000000},	//video0
	{0x00001600, 0x00000680}, {0x00001640, 0x00000000},	{0x00001680, 0x012c0400}, {0x000016c0, 0xff000000}, {0x00001700, 0xff000000},
	{0x00001740, 0x00000000}, {0x00001780, 0x00000000}, {0x000017c0, 0x00c80190}, {0x00001580, 0x0000040a}, {0x00001c00, 0x00000133},
	{0x00001810, 0x00c80190}, {0x00001828, 0x000063d8}, {0x00001830, 0x0000aa86}, {0x00001a40, 0x000063d8}, {0x00001a80, 0x0000aa86},
	{0x00001a20, 0x80008000}, {0x00001bc0, 0x80008000}, {0x00001410, 0x00000000},	//overlay0 initialOffset and display dither config
	{0x00001430, 0x04a00400},	//display HDisplay(h_display[14:0]:640, h_total[30:16]:800)[0x03200280, 0x035a02d0]
	{0x00001438, 0x42380410},	//display HSync(start[14:0]:656, End[29:15]:752, Len_hsync:96)[0x41780290, 0x418f02e0]
	{0x00001440, 0x06d10258},	//display VDisplay(v_display[14:0]:480, v_total[30:16]:525)[0x020d01e0, 0x020d01e0]
	{0x00001448, 0x41320262},	//display VSync(start[14:0]:490, End[29:15]:492, Len_vsync:2)[0x40f601ea, 0x40f601ea]

	{0x000014e8, 0x00000000},	//dc8000 writeback memory address
	{0x000014f0, 0x00000000},	//dc8000 writeback panel mem address
	{0x00001500, 0x00001000},	//dc8000 writeback mem stride[0x00000a00, 0x00000b40]
	{0x000014f8, 0x00030000},	//dc8000 writeback enable

	{0x00001418, 0x00000111}, {0x000014b8, 0x00000005}, {0x00001468, 0x00000000}, {0x00001484, 0x00000001}, {0x000014b0, 0x00000000},
	{0x00001508, 0x00000000}, {0x00001510, 0x00000000}, {0x00001a28, 0x00000000}, {0x00001dc0, 0x00000000}, {0x00001d00, 0x00000000},
	{0x00001d40, 0x00000000}, {0x00001cb0, 0x00000000}, {0x00001cb8, 0x00000000}, {0x00001cc8, 0x00000000}, {0x00001cd0, 0x00000000},
	{0x00001ce8, 0x00000000}, {0x00001cc0, 0x00000010}, {0x00001838, 0x00000000}, {0x00001a08, 0x00000000}, {0x00001ac0, 0x00000000},
	{0x00001b40, 0x00000000}, {0x00001e98, 0x00000000}, {0x00002430, 0x00000001}, {0x00001420, 0x00000000}, {0x00001428, 0x00000000},  
	{0x000024d0, 0x00000000}, {0x00001540, 0x01060020}, {0x00001518, 0x18404011},	//dc8000 framebuffer config (enable output)
	#else   //overlay layer
    /* display registers */
    {0x00001400, 0xffffffff}, {0x000015c0, 0xffffffff}, //overlay0 config , video0 and overlay0 address
    {0x00001408, 0x00000b40}, {0x00001520, 0x00000000}, {0x00001528, 0x00000000},   //video0
    {0x00001600, 0x00000b40}, {0x00001640, 0x00000000}, {0x00001680, 0x00f002d0}, {0x000016c0, 0xff000000}, {0x00001700, 0xff000000},
    {0x00001740, 0x00000000}, {0x00001780, 0x00000000}, {0x000017c0, 0x00f002d0}, {0x00001580, 0x0000040a}, {0x00001c00, 0x00000000},
    {0x00001810, 0x00f002d0}, //overlay0 size, (width = 640 x 480)[0x00f00280, 0x00f002d0]
    {0x00001a20, 0x1f401f40}, {0x00001410, 0x00000000}, //overlay0 initialOffset and display dither config
    {0x00001430, 0x035a02d0},   //display HDisplay(h_display[14:0]:640, h_total[30:16]:800)[0x03200280, 0x035a02d0]
    {0x00001438, 0x418f02e0},   //display HSync(start[14:0]:656, End[29:15]:752, Len_hsync:96)[0x41780290, 0x418f02e0]
    {0x00001440, 0x065901e0},   //display VDisplay(v_display[14:0]:480, v_total[30:16]:525)[0x020d01e0, 0x020d01e0]
    {0x00001448, 0x40f601ea},   //display VSync(start[14:0]:490, End[29:15]:492, Len_vsync:2)[0x40f601ea, 0x40f601ea]

    {0x000014e8, 0x00000000},   //dc8000 writeback memory address
    {0x000014f0, 0x00000000},   //dc8000 writeback panel mem address
    {0x00001500, 0x00000b40},   //dc8000 writeback mem stride[0x00000a00, 0x00000b40]
    {0x000014f8, 0x00030000},   //dc8000 writeback enable

    {0x00001418, 0x00000111}, {0x000014b8, 0x00000005}, {0x00001468, 0x00000000}, {0x00001484, 0x00000001}, {0x000014b0, 0x00000000},
    {0x00001508, 0x00000000}, {0x00001510, 0x00000000}, {0x00001a28, 0x00000000}, {0x00001dc0, 0x00000000}, {0x00001d00, 0x00000000},
    {0x00001d40, 0x00000000}, {0x00001cb0, 0x00000000}, {0x00001cb8, 0x00000000}, {0x00001cc8, 0x00000000}, {0x00001cd0, 0x00000000},
    {0x00001ce8, 0x00000000}, {0x00001cc0, 0x00000010}, {0x00001838, 0x00000000}, {0x00001a08, 0x00000000}, {0x00001ac0, 0x00000000},
    {0x00001b40, 0x00000000}, {0x00001e98, 0x00000000}, {0x00002430, 0x00000001}, {0x00001420, 0x00000000}, {0x00001428, 0x00000000},
    {0x000024d0, 0x00000000}, {0x00001540, 0x01060020}, {0x00001518, 0x18004011},   //dc8000 framebuffer config (enable output)
	#endif
#endif
};

extern void *get_display_buffer(int size);
extern unsigned int eswin_dc_read_reg(void *regs, unsigned int addr);
extern void eswin_dc_write_reg(void *regs, unsigned int addr, unsigned int data);

int dc_reg_test_read(void *regs)
{
    unsigned int offset, regVal;
	unsigned int i;

	printf("\n");
	printf("++++++++++++++++++++dc_reg_test_read++++++++++++++++++++++++\n");
    //host interface
    printf("\n");
    printf("++++++++++++++++++++host interface registers++++++++++++++++++++++++\n");
    printf("\n");

    regVal = eswin_dc_read_reg(regs, 0);
    if(regVal != 0x900)
        printf("dc_reg_test_read: AQHiClockControl[0x0, 0x900, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 4);
    if(regVal != 0x10000)
        printf("dc_reg_test_read: AQHiIdle[0x4, 0x10000, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 8);
    if(regVal != 0x222200)
        printf("dc_reg_test_read: AQAxiConfig[0x8, 0x222200, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x0C);
    if(regVal != 0x0)
        printf("dc_reg_test_read: AQAxiStatus[0x0c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x10);
    if(regVal != 0x0)
        printf("dc_reg_test_read: AQIntrAcknowledge[0x10, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x14);
    if(regVal != 0x0)
        printf("dc_reg_test_read: AQIntrEnbl[0x14, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x20);
    if(regVal != 0x8000)
        printf("dc_reg_test_read: GCChipId[0x20, 0x8000, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x24);
    if(regVal != 0x5701)
        printf("dc_reg_test_read: GCChipRev[0x24, 0x5701, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x28);
    if(regVal != 0x20210309)
        printf("dc_reg_test_read: GCChipDate[0x28, 0x20210309, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x40);
    if(regVal != 0x0)
        printf("dc_reg_test_read: gcTotalReads[0x40, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x78);
    if(regVal != 0x0)
        printf("dc_reg_test_read: gcTotalCycles[0x78, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x98);
    if(regVal != 0x10)
        printf("dc_reg_test_read: gcregHIChipPatchRev[0x98, 0x10, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0xA8);
    if(regVal != 0x80000)
        printf("dc_reg_test_read: gcProductId[0xa8, 0x80000, 0x%x]\n", regVal);
	
    //memory control
    printf("\n");
    printf("++++++++++++++++++++memory control registers++++++++++++++++++++++++\n");
    printf("\n");
    regVal = eswin_dc_read_reg(regs, 0x184);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUConfiguration[0x184, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x18C);
    if(regVal != 0x1f0)
    	printf("dc_reg_test_read: gcregMMUControl[0x18c, 0x1f0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1AC);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUConfig[0x1ac, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x380);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBException[0x380, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x384);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBStatus[0x384, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x388);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBControl[0x388, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x38C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBTableArrayBaseAddressLow[0x38c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x390);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBTableArrayBaseAddressHigh[0x390, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x394);
	if(regVal != 0xffff)
    	printf("dc_reg_test_read: gcregMMUAHBTableArraySize[0x394, 0xffff, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x398);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBSafeNonSecureAddress[0x398, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x39C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBSafeSecureAddress[0x39c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x3A0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBSafeAddressExt[0x3a0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x3AC);
	if(regVal != 0x202200)
    	printf("dc_reg_test_read: gcregAxiAHBConfig[0x3ac, 0x202200, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x3B4);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregMMUAHBContextPDEEntry0[0x3b4, 0x0, 0x%x]\n", regVal);

    //dec control
    printf("\n");
    printf("++++++++++++++++++++dec control registers++++++++++++++++++++++++\n");
    printf("\n");
    regVal = eswin_dc_read_reg(regs, 0x800);
	if(regVal != 0x201010a)
    	printf("dc_reg_test_read: gcregAHBDECControl[0x800, 0x201010a, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x804);
	if(regVal != 0x80000)
    	printf("dc_reg_test_read: gcregAHBDECControlEx[0x804, 0x80000, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x808);
	if(regVal != 0x3fc810)
    	printf("dc_reg_test_read: gcregAHBDECControlEx2[0x808, 0x3fc810, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x80C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECIntrEnbl[0x80c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x814);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECIntrEnblEx2[0x814, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x818);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECIntrAcknowledge[0x818, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x820);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECIntrAcknowledgeEx2[0x820, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x824);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTileStatusDebug[0x824, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x828);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECEncoderDebug[0x828, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x82C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDecoderDebug[0x82c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x830);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadsIn[0x830, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x834);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWritesIn[0x834, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x838);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadBurstsIn[0x838, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x83C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWriteBurstsIn[0x83c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x840);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadsReqIn[0x840, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x844);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWritesReqIn[0x844, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x848);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadLastsIn[0x848, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x84C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWriteLastsIn[0x84c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x850);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWriteResponseIn[0x850, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x854);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadsOUT[0x854, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x858);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWritesOUT[0x858, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x85C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadBurstsOUT[0x85c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x860);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWriteBurstsOUT[0x860, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x864);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadsReqOUT[0x864, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x868);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWritesReqOUT[0x868, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x86C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalReadLastsOUT[0x86c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x870);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWriteLastsOUT[0x870, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x874);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECTotalWriteResponseOUT[0x874, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x878);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECStatus[0x878, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x87C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugInfoSelect[0x87c, 0x0, 0x%x]\n", regVal);
	
	for(i = 0; i< 16; i++)
	{
		offset = 0x880 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x20000)
    		printf("dc_reg_test_read: gcregAHBDECReadConfig%d[0x%x, 0x20000, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0x900 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECReadExConfig%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0x980 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x20000)
    		printf("dc_reg_test_read: gcregAHBDECWriteConfig%d[0x%x, 0x20000, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0xA80 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0xffffffff)
    		printf("dc_reg_test_read: gcregAHBDECReadBufferBase%d[0x%x, 0xffffffff, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0xB00 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0xffffffff)
    		printf("dc_reg_test_read: gcregAHBDECReadBufferBaseEx%d[0x%x, 0xffffffff, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0xB80 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0xffffffff)
   			printf("dc_reg_test_read: gcregAHBDECReadBufferEnd%d[0x%x, 0xffffffff, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0xC00 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0xffffffff)
    		printf("dc_reg_test_read: gcregAHBDECReadBufferEndEx%d[0x%x, 0xffffffff, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0x1080 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECReadCacheBase%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}
	
	for(i = 0; i< 16; i++)
	{
		offset = 0x1100 + i * 4;
    	regVal = eswin_dc_read_reg(regs, 0x1100);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECReadCacheBaseEx%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}

	for(i = 0; i< 6; i++)
	{
		offset = 0x1284 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECDebug%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}

	for(i = 0; i< 4; i++)
	{
		offset = 0x12A8 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECCrDebugTile128Type%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}

	for(i = 0; i< 8; i++)
	{
		offset = 0x12B8 + i * 4;
    	regVal = eswin_dc_read_reg(regs, 0x12B8);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECCrDebugTile256Type%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}
	
    regVal = eswin_dc_read_reg(regs, 0x12D8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugReadGateDomainClkCounter[0x12d8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12DC);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugWriteGateDomainClkCounter[0x12dc, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12E0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugOtherGateDomainClkCounter[0x12e0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12E4);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugTSCWrHitCount[0x12e4, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12E8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugTSCRdHitCount[0x12e8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12EC);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugTSCWrMissCount[0x12ec, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12F0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugTSCRdMissCount[0x12f0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12F4);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugWriteSmallTileSize[0x12f4, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12F8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugWriteLargeTileSize[0x12f8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x12FC);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: gcregAHBDECDebugReadSmallTileSize[0x12fc, 0x0, 0x%x]\n", regVal);

	for(i = 0; i< 16; i++)
	{
		offset = 0x1300 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECFastClearValue%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0x1380 + i * 4;
    	regVal = eswin_dc_read_reg(regs, offset);
		if(regVal != 0x0)
    		printf("dc_reg_test_read: gcregAHBDECFastClearValueEx%d[0x%x, 0x0, 0x%x]\n", i, offset, regVal);
	}
	
	//display control
    printf("\n");
    printf("++++++++++++++++++++display control registers++++++++++++++++++++++++\n");
    printf("\n");
    regVal = eswin_dc_read_reg(regs, 0x1400);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferAddress0[0x1400, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1408);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferStride0[0x1408, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1410);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDisplayDitherConfig0[0x1410, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1418);
	if(regVal != 0x100)
    	printf("dc_reg_test_read: dcregPanelConfig0[0x1418, 0x100, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1420);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDisplayDitherTableLow0[0x1420, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1428);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDisplayDitherTableHigh0[0x1428, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1430);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregHDisplay0[0x1430, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1438);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregHSync0[0x1438, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1440);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregVDisplay0[0x1440, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1448);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregVsync0[0x1448, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1450);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDisplayCurrentLocation0[0x1450, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1468);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregCursorConfig[0x1468, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x146C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregCursorAddress[0x146c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1470);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregCursorLocation[0x1470, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1474);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregCursorBackground[0x1474, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1478);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregCursorForeground[0x1478, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x147C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDisplayIntr[0x147c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1480);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDisplayIntrEnable[0x1480, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1484);
	if(regVal != 0x1)
    	printf("dc_reg_test_read: dcregCursorModuleClockGatingControl[0x1484, 0x1, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x14B0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregGeneralConfig0[0x14b0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x14B8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDpiConfig0[0x14b8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x14D0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDebugCounterSelect0[0x14d0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x14D8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDebugCounterValue0[0x14d8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x14E8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregMemDestAddress0[0x14e8, 0x0, 0x%x]\n", regVal);

	regVal = eswin_dc_read_reg(regs, 0x14F0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregPanelDestAddressRegAddrs0[0x14f0, 0x0, 0x%x]\n", regVal);

    regVal = eswin_dc_read_reg(regs, 0x14F8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDestConfig0[0x14f8, 0x0, 0x%x]\n", regVal);

	regVal = eswin_dc_read_reg(regs, 0x1500);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDestStrideRegAddrs[0x1500, 0x0, 0x%x]\n", regVal);

    regVal = eswin_dc_read_reg(regs, 0x1508);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferColorKey0[0x1508, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1510);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferColorKeyHigh0[0x1510, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1518);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferConfig0[0x1518, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1520);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferScaleConfig0[0x1520, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1528);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferGBColor0[0x1528, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1530);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferUPlanarAddress0[0x1530, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1538);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferVPlanarAddress0[0x1538, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1540);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayConfig0[0x1540, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1580);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayAlphaBlendConfig0[0x1580, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x15C0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayAddress0[0x15c0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1600);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayStride0[0x1600, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1640);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayTL0[0x1640, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1680);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayBR0[0x1680, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x16C0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlaySrcGlobalColor0[0x16c0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1700);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayDstGlobalColor0[0x1700, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1740);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayColorkey0[0x1740, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1780);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayColorKeyHigh0[0x1780, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x17C0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlaySize0[0x17c0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1800);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferUStride0[0x1800, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1808);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferVStride0[0x1808, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1810);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferSize0[0x1810, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1818);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregIndexColorTableIndex0[0x1818, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1820);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregIndexColorTableData0[0x1820, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1828);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferScaleFactorX0[0x1828, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1830);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferScaleFactorY0[0x1830, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1838);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregHoriFilterKernelIndex0[0x1838, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1840);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayUPlanarAddress0[0x1840, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1880);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayVPlanarAddress0[0x1880, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x18C0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayUStride0[0x18c0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1900);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayVStride0[0x1900, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1940);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayClearValue0[0x1940, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1980);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayIndexColorTableIndex0[0x1980, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x19C0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayIndexColorTableData0[0x19c0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A00);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregHoriFilterKernel0[0x1a00, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A08);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregVertiFilterKernelIndex0[0x1a08, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A10);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregVertiFilterKernel0[0x1a10, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A18);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferClearValue0[0x1a18, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A20);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferInitialOffset0[0x1a20, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A28);
	if(regVal != 0xe03)
    	printf("dc_reg_test_read: dcregModuleClockGatingControl0[0x1a28, 0xe03, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A40);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayScaleFactorX0[0x1a40, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1A80);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayScaleFactorY0[0x1a80, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1AC0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayHoriFilterKernelIndex0[0x1ac0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1B00);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayHoriFilterKernel0[0x1b00, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1B40);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayVertiFilterKernelIndex0[0x1b40, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1B80);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayVertiFilterKernel0[0x1b80, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1BC0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayInitialOffset0[0x1bc0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1C00);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayScaleConfig0[0x1c00, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CB0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferROIOrigin0[0x1cb0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CB8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferROISize0[0x1cb8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CC0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferConfigEx0[0x1cc0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CD0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDpConfig0[0x1cd0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CD8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregThreedLutIndex0[0x1cd8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CE0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregThreedLutData0[0x1ce0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CE8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameBufferWaterMark0[0x1ce0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CF0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregGammaExIndex0[0x1cf0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1CF8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregGammaExData0[0x1cf8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1D00);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayROIOrigin0[0x1d00, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1D40);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayROISize0[0x1d40, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1D80);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregGammaExOneData0[0x1d80, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1D88);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDeGammaIndex0[0x1d88, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1D90);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDeGammaData0[0x1d90, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1D98);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregDeGammaExData0[0x1d98, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DA0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef00[0x1da0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DA4);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef01[0x1da4, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DA8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef10[0x1da8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DAC);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef11[0x1dac, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DB0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef20[0x1db0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DB4);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef21[0x1db4, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DB8);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef30[0x1db8, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DBC);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef31[0x1dbc, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1DC0);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregOverlayWaterMark0[0x1dc0, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1E00);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef40[0x1e00, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1E04);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoef41[0x1e04, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1E08);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoefD00[0x1e08, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1E0C);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoefD01[0x1e0c, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1E10);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoefD10[0x1e10, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1E14);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoefD11[0x1e14, 0x0, 0x%x]\n", regVal);
	
    regVal = eswin_dc_read_reg(regs, 0x1E18);
	if(regVal != 0x0)
    	printf("dc_reg_test_read: dcregFrameYUVToRGBCoefD20[0x1e18, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E1C);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameYUVToRGBCoefD21[0x1e1c, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E20);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef00[0x1e20, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E24);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef01(0x0)=0x%x\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E28);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef10[0x1e28, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E2C);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef11[0x1e2c, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E30);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef20[0x1e30, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E34);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef21[0x1e34, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E38);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef30[0x1e38, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E3C);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef31[0x1e3c, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E40);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef40[0x1e40, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E44);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameRGBToRGBCoef41[0x1e44, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E48);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef00[0x1e48, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E4C);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef01[0x1e4c, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E50);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef10[0x1e50, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E54);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef11[0x1e54, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E58);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef20[0x1e58, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E5C);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef21[0x1e5c, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E60);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef30[0x1e60, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E64);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef31[0x1e64, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E68);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef40[0x1e68, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E6C);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoef41[0x1e6c, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E70);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoefD00[0x1e70, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E74);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoefD01[0x1e74, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E78);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoefD10[0x1e78, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E7C);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoefD11[0x1e7c, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E80);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoefD20[0x1e80, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E84);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregRGBToYUVCoefD21[0x1e84, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E88);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameYClampBound0[0x1e88, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E90);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameUVClampBound0[0x1e90, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1E98);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregMMUPrefetch0[0x1e98, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1EA0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameYOrArgbSTLBStartAddr0[0x1ea0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1EA8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameYOrArgbSTLBEndAddr0[0x1ea8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1EB0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameYOrArgbSTLBStride0[0x1eb0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1EB8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregFrameYOrArgbSTLBMaxNum0[0x1eb8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1EC0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoef00[0x1ec0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1F00);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoef10[0x1f00, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1F40);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoef20[0x1f40, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1F80);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoef30[0x1f80, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x1FC0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoef40[0x1fc0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2000);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoefD00[0x2000, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2040);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoefD10[0x2040, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2080);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYUVToRGBCoefD20[0x2080, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x20C0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayRGBToRGBCoef00[0x20c0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2100);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayRGBToRGBCoef10[0x2100, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2140);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayRGBToRGBCoef20[0x2140, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2180);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayRGBToRGBCoef30[0x2180, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x21C0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayRGBToRGBCoef40[0x21c0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2200);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayDeGammaIndex0[0x2200, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2240);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayDeGammaData0[0x2240, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2280);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayDeGammaExData0[0x2280, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x22C0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayYClampBound0[0x22c0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2300);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregOverlayUVClampBound0[0x2300, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2340);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvSTLBStartAddr0[0x2340, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2348);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvSTLBEndAddr0[0x2348, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2350);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvSTLBStride0[0x2350, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2358);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvSTLBMaxNum0[0x2358, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2360);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameVSTLBStartAddr0[0x2360, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2368);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameVSTLBEndAddr0[0x2368, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2370);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameVSTLBStride0[0x2370, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2378);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameVSTLBMaxNum0[0x2378, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2380);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbSTLBStartAddr0[0x2380, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2388);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbSTLBEndAddr0[0x2388, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2390);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbSTLBStride0[0x2390, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2398);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbSTLBMaxNum0[0x2398, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23A0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvSTLBStartAddr0[0x23a0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23A8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvSTLBEndAddr0[0x23a8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23B0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvSTLBStride0[0x23b0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23B8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvSTLBMaxNum0[0x23b8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23C0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayVSTLBStartAddr0[0x23c0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23C8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayVSTLBEndAddr0[0x23c8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23D0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayVSTLBStride0[0x23d0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23D8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayVSTLBMaxNum0[0x23d8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23E0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregCursorSTLBStartAddr0[0x23e0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23E8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregCursorSTLBEndAddr0[0x23e8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23F0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameYOrArgbTscSTLBStartAddr0[0x23f0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x23F8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameYOrArgbTscSTLBEndAddr0[0x23f8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2400);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvTscSTLBStartAddr0[0x2400, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2408);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvTscSTLBEndAddr0[0x2408, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2410);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbTscSTLBStartAddr0[0x2410, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2418);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbTscSTLBEndAddr0[0x2418, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2420);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvTscSTLBStartAddr0[0x2420, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2428);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvTscSTLBEndAddr0[0x2428, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2430);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregTSCPrefetch0[0x2430, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2438);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameYOrArgbDecTscStartAddr0[0x2438, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2440);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameYOrArgbDecTscEndAddr0[0x2440, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2448);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameYOrArgbDecTscStride0[0x2448, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2450);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameYOrArgbDecTscMaxNum0[0x2450, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2458);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvDecTscStartAddr0[0x2458, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2460);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvDecTscEndAddr0[0x2460, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2468);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvDecTscStride0[0x2468, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2470);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregFrameUvDecTscMaxNum0[0x2470, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2478);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbDecTscStartAddr0[0x2478, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2480);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbDecTscEndAddr0[0x2480, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2488);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbDecTscStride0[0x2488, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2490);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayYOrArgbDecTscMaxNum0[0x2490, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x2498);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvDecTscStartAddr0[0x2498, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x24A0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvDecTscEndAddr0[0x24a0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x24A8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvDecTscStride0[0x24a8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x24B0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: gcregOverlayUvDecTscMaxNum0[0x24b0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x24B8);
	if(regVal != 0x10040100)
		printf("dc_reg_test_read: dcregThreedLutScale0[0x24b8, 0x10040100, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x24C0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregThreedLutOffset0[0x24c0, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x24C8);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregThreedLutOffsetEx0[0x24c8, 0x0, 0x%x]\n", regVal);
	
	regVal = eswin_dc_read_reg(regs, 0x24D0);
	if(regVal != 0x0)
		printf("dc_reg_test_read: dcregThreedLutEnlarge0[0x24d0, 0x0, 0x%x]\n", regVal);
	
	return 0;
}

int dc_reg_test_write(void *regs, unsigned int regVal)
{
    unsigned int offset, readVal;
	unsigned int i;

	printf("\n");
    printf("++++++++++++++++++++dc_reg_test_write[0x%x]++++++++++++++++++++++++\n", regVal);
    //host interface
    printf("\n");
    printf("++++++++++++++++++++host interface registers++++++++++++++++++++++++\n");
    printf("\n");

    eswin_dc_write_reg(regs, 0, 0xfff00900);
	readVal = eswin_dc_read_reg(regs, 0);
	if(readVal != 0xfff00900)
    	printf("dc_reg_test_write: AQHiClockControl[0x0, 0xfff00900, 0x%x]\n", readVal);
	
    eswin_dc_write_reg(regs, 8, 0xff222200);
	readVal = eswin_dc_read_reg(regs, 8);
	if(readVal != 0xff222200)
    	printf("dc_reg_test_write: AQAxiConfig[0x8, 0xff222200, 0x%x]\n", readVal);

	eswin_dc_write_reg(regs, 0x14, regVal);
	readVal = eswin_dc_read_reg(regs, 0x14);
	if(readVal != regVal)
    	printf("dc_reg_test_write: AQIntrEnbl[0x14, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x78, regVal);
	readVal = eswin_dc_read_reg(regs, 0x78);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcTotalCycles[0x78, 0x%x, 0x%x]\n", regVal, readVal);
	
	//memory control
	printf("\n");
    printf("++++++++++++++++++++memory control registers++++++++++++++++++++++++\n");
    printf("\n");
	eswin_dc_write_reg(regs, 0x184, 0xffffff6f);
	readVal = eswin_dc_read_reg(regs, 0x184);
	if(readVal != 0xffffff6f)
    	printf("dc_reg_test_write: gcregMMUConfiguration[0x184, 0xffffff6f, 0x%x]\n", readVal);

	eswin_dc_write_reg(regs, 0x18C, regVal);
	readVal = eswin_dc_read_reg(regs, 0x18C);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUControl[0x18c, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1AC, 0xdffeffff);
	readVal = eswin_dc_read_reg(regs, 0x1AC);
	if(readVal != 0xdffeffff)
    	printf("dc_reg_test_write: gcregMMUConfig[0x1ac, 0xdffeffff, 0x%x]\n", readVal);

	eswin_dc_write_reg(regs, 0x380, regVal);
	readVal = eswin_dc_read_reg(regs, 0x380);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUAHBException[0x380, 0x%x, 0x%x]\n", regVal, readVal);

    eswin_dc_write_reg(regs, 0x388, 0xffffffe0);
	readVal = eswin_dc_read_reg(regs, 0x388);
	if(readVal != 0xffffffe0)
    	printf("dc_reg_test_write: gcregMMUAHBControl[0x388, 0xffffffe0, 0x%x]\n", readVal);

	eswin_dc_write_reg(regs, 0x38C, regVal);
	readVal = eswin_dc_read_reg(regs, 0x38C);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUAHBTableArrayBaseAddressLow[0x38c, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x390, regVal);
	readVal = eswin_dc_read_reg(regs, 0x390);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUAHBTableArrayBaseAddressHigh[0x390, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x394, regVal);
	readVal = eswin_dc_read_reg(regs, 0x394);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUAHBTableArraySize[0x394, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x398, regVal);
	readVal = eswin_dc_read_reg(regs, 0x398);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUAHBSafeNonSecureAddress[0x398, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x39C, regVal);
	readVal = eswin_dc_read_reg(regs, 0x39C);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUAHBSafeSecureAddress[0x39c, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x3A0, 0xffff7fff);
	readVal = eswin_dc_read_reg(regs, 0x3A0);
	if(readVal != 0xffff7fff)
    	printf("dc_reg_test_write: gcregMMUAHBSafeAddressExt[0x3a0, 0xffff7fff, 0x%x]\n", readVal);

	eswin_dc_write_reg(regs, 0x3AC, regVal);
	readVal = eswin_dc_read_reg(regs, 0x3AC);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregAxiAHBConfig[0x3ac, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x3B4, regVal);
	readVal = eswin_dc_read_reg(regs, 0x3B4);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregMMUAHBContextPDEEntry0[0x3b4, 0x%x, 0x%x]\n", regVal, readVal);

	//dec control
	printf("\n");
    printf("++++++++++++++++++++dec control registers++++++++++++++++++++++++\n");
    printf("\n");
	eswin_dc_write_reg(regs, 0x800, 0xffffffee);
    readVal = eswin_dc_read_reg(regs, 0x800);
	if(readVal != 0xffffffee)
    	printf("dc_reg_test_write: gcregAHBDECControl[0x800, 0xffffffee, 0x%x]\n", readVal);

	eswin_dc_write_reg(regs, 0x804, regVal);
	readVal = eswin_dc_read_reg(regs, 0x804);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregAHBDECControlEx[0x804, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x808, regVal);
	readVal = eswin_dc_read_reg(regs, 0x808);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregAHBDECControlEx2[0x808, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x80C, regVal);
	readVal = eswin_dc_read_reg(regs, 0x80C);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregAHBDECIntrEnbl[0x80c, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x814, regVal);
	readVal = eswin_dc_read_reg(regs, 0x814);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregAHBDECIntrEnblEx2[0x814, 0x%x, 0x%x]\n", regVal, readVal);

    eswin_dc_write_reg(regs, 0x87C, regVal);
	readVal = eswin_dc_read_reg(regs, 0x87C);
	if(readVal != regVal)
    	printf("dc_reg_test_write: gcregAHBDECDebugInfoSelect[0x87c, 0x%x, 0x%x]\n", regVal, readVal);

	for(i = 0; i< 16; i++)
	{
		offset = 0x880 + i * 4;
		eswin_dc_write_reg(regs, offset, regVal);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != regVal)
    		printf("dc_reg_test_write: gcregAHBDECReadConfig%d[0x%x, 0x%x, 0x%x]\n", i, offset, regVal, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0x900 + i * 4;
		eswin_dc_write_reg(regs, offset, regVal);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != regVal)
    		printf("dc_reg_test_write: gcregAHBDECReadExConfig%d[0x%x, 0x%x, 0x%x]\n", i, offset, regVal, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0x980 + i * 4;
		eswin_dc_write_reg(regs, offset, regVal);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != regVal)
    		printf("dc_reg_test_write: gcregAHBDECWriteConfig%d[0x%x, 0x%x, 0x%x]\n", i, offset, regVal, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0xA80 + i * 4;
		eswin_dc_write_reg(regs, offset, 0x0);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != 0x0)
    		printf("dc_reg_test_write: gcregAHBDECReadBufferBase%d[0x%x, 0x0, 0x%x]\n", i, offset, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0xB00 + i * 4;
		eswin_dc_write_reg(regs, offset, 0x0);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != 0x0)
    		printf("dc_reg_test_write: gcregAHBDECReadBufferBaseEx%d[0x%x, 0x0, 0x%x]\n", i, offset, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0xB80 + i * 4;
		eswin_dc_write_reg(regs, offset, 0x0);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != 0x0)
    		printf("dc_reg_test_write: gcregAHBDECReadBufferEnd%d[0x%x, 0x0, 0x%x]\n", i, offset, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0xC00 + i * 4;
		eswin_dc_write_reg(regs, offset, 0x0);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != 0x0)
    		printf("dc_reg_test_write: gcregAHBDECReadBufferEndEx%d[0x%x, 0x0, 0x%x]\n", i, offset, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0x1080 + i * 4;
		eswin_dc_write_reg(regs, offset, regVal);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != regVal)
    		printf("dc_reg_test_write: gcregAHBDECReadCacheBase%d[0x%x, 0x%x, 0x%x]\n", i, offset, regVal, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0x1100 + i * 4;
		eswin_dc_write_reg(regs, offset, regVal);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != regVal)
    		printf("dc_reg_test_write: gcregAHBDECReadCacheBaseEx%d[0x%x, 0x%x, 0x%x]\n", i, offset, regVal, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0x1300 + i * 4;
		eswin_dc_write_reg(regs, offset, regVal);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != regVal)
    		printf("dc_reg_test_write: gcregAHBDECFastClearValue%d[0x%x, 0x%x, 0x%x]\n", i, offset, regVal, readVal);
	}

	for(i = 0; i< 16; i++)
	{
		offset = 0x1380 + i * 4;
		eswin_dc_write_reg(regs, offset, regVal);
		readVal = eswin_dc_read_reg(regs, offset);
		if(readVal != regVal)
    		printf("dc_reg_test_write: gcregAHBDECFastClearValueEx%d[0x%x, 0x%x, 0x%x]\n", i, offset, regVal, readVal);
	}
	
	//display control
	printf("\n");
    printf("++++++++++++++++++++display control registers++++++++++++++++++++++++\n");
    printf("\n");
	eswin_dc_write_reg(regs, 0x1400, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1400);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferAddress0[0x1400, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1408, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1408);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferStride0[0x1408, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1410, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1410);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDisplayDitherConfig0[0x1410, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1418, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1418);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregPanelConfig0[0x1418, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1420, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1420);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDisplayDitherTableLow0[0x1420, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1428, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1428);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDisplayDitherTableHigh0[0x1428, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1430, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1430);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregHDisplay0[0x1430, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1438, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1438);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregHSync0[0x1438, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1440, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1440);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregVDisplay0[0x1440, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1448, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1448);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregVsync0[0x1448, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1468, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1468);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregCursorConfig[0x1468, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x146C, regVal);
	readVal = eswin_dc_read_reg(regs, 0x146C);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregCursorAddress[0x146c, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1470, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1470);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregCursorLocation[0x1470, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1474, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1474);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregCursorBackground[0x1474, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1478, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1478);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregCursorForeground[0x1478, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1480, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1480);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDisplayIntrEnable[0x1480, 0x%x, 0x%x]\n", regVal, readVal);
	
	eswin_dc_write_reg(regs, 0x1484, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1484);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregCursorModuleClockGatingControl[0x1484, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x14B0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x14B0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregGeneralConfig0[0x14b0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x14B8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x14B8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDpiConfig0[0x14b8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x14D0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x14D0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDebugCounterSelect0[0x14d0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x14E8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x14E8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregMemDestAddress0[0x14e8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x14F0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x14F0);
	if(readVal != regVal)
    	printf("dc_reg_test_read: dcregPanelDestAddressRegAddrs0[0x14f0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x14F8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x14F8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDestConfig0[0x14f8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1500, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1500);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDestStrideRegAddrs[0x1500, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1508, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1508);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferColorKey0[0x1508, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1510, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1510);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferColorKeyHigh0[0x1510, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1518, 0xffffff8f);
	readVal = eswin_dc_read_reg(regs, 0x1518);
	if(readVal != 0xffffff8f)
		printf("dc_reg_test_write: dcregFrameBufferConfig0[0x1518, 0xffffff8f, 0x%x]\n", readVal);

	eswin_dc_write_reg(regs, 0x1520, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1520);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferScaleConfig0[0x1520, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1528, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1528);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferGBColor0[0x1528, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1530, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1530);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferUPlanarAddress0[0x1530, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1538, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1538);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferVPlanarAddress0[0x1538, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1540, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1540);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayConfig0[0x1540, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1580, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1580);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayAlphaBlendConfig0[0x1580, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x15C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x15C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayAddress0[0x15c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1600, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1600);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayStride0[0x1600, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1640, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1640);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayTL0[0x1640, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1680, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1680);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayBR0[0x1680, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x16C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x16C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlaySrcGlobalColor0[0x16c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1700, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1700);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayDstGlobalColor0[0x1700, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1740, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1740);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayColorkey0[0x1740, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1780, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1780);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayColorKeyHigh0[0x1780, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x17C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x17C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlaySize0[0x17c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1800, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1800);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferUStride0[0x1800, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1808, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1808);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferVStride0[0x1808, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1810, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1810);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferSize0[0x1810, 0x%x, 0x%x]\n", regVal, readVal);
	
	eswin_dc_write_reg(regs, 0x1818, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1818);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregIndexColorTableIndex0[0x1818, 0x%x, 0x%x]\n", regVal, readVal);
	
	eswin_dc_write_reg(regs, 0x1820, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1820);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregIndexColorTableData0[0x1820, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1828, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1828);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferScaleFactorX0[0x1828, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1830, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1830);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferScaleFactorY0[0x1830, 0x%x, 0x%x]\n", regVal, readVal);
	
	eswin_dc_write_reg(regs, 0x1838, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1838);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregHoriFilterKernelIndex0[0x1838, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1840, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1840);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayUPlanarAddress0[0x1840, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1880, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1880);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayVPlanarAddress0[0x1880, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x18C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x18C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayUStride0[0x18c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1900, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1900);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayVStride0[0x1900, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1940, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1940);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayClearValue0[0x1940, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1980, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1980);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayIndexColorTableIndex0[0x1980, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x19C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x19C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayIndexColorTableData0[0x19c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A00, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A00);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregHoriFilterKernel0[0x1a00, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A08, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A08);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregVertiFilterKernelIndex0[0x1a08, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A10, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A10);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregVertiFilterKernel0[0x1a10, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A18, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A18);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferClearValue0[0x1a18, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A20, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A20);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferInitialOffset0[0x1a20, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A28, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A28);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregModuleClockGatingControl0[0x1a28, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A40, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A40);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayScaleFactorX0[0x1a40, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1A80, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1A80);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayScaleFactorY0[0x1a80, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1AC0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1AC0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayHoriFilterKernelIndex0[0x1ac0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1B00, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1B00);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayHoriFilterKernel0[0x1b00, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1B40, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1B40);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayVertiFilterKernelIndex0[0x1b40, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1B80, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1B80);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayVertiFilterKernel0[0x1b80, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1BC0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1BC0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayInitialOffset0[0x1bc0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1C00, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1C00);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayScaleConfig0[0x1c00, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CB0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CB0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferROIOrigin0[0x1cb0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CB8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CB8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferROISize0[0x1cb8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CC0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CC0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferConfigEx0[0x1cc0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CD0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CD0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDpConfig0[0x1cd0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CD8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CD8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregThreedLutIndex0[0x1cd8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CE0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CE0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregThreedLutData0[0x1ce0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CE8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CE8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameBufferWaterMark0[0x1ce8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CF0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CF0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregGammaExIndex0[0x1cf0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1CF8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1CF8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregGammaExData0[0x1cf8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1D00, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1D00);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayROIOrigin0[0x1d00, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1D40, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1D40);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayROISize0[0x1d40, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1D80, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1D80);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregGammaExOneData0[0x1d80, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1D88, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1D88);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDeGammaIndex0[0x1d88, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1D90, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1D90);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDeGammaData0[0x1d90, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1D98, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1D98);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregDeGammaExData0[0x1d98, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1DA0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1DA0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoef00[0x1da0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1DA8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1DA8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoef10[0x1da8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1DB0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1DB0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoef20[0x1db0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1DB8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1DB8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoef30[0x1db8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1DC0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1DC0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayWaterMark0[0x1dc0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E00, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E00);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoef40[0x1e00, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E08, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E08);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoefD00[0x1e08, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E10, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E10);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoefD10[0x1e10, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E18, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E18);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYUVToRGBCoefD20[0x1e18, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E20, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E20);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameRGBToRGBCoef00[0x1e20, 0x%x, 0x%x]\n", regVal, readVal);

    eswin_dc_write_reg(regs, 0x1E28, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E28);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameRGBToRGBCoef10[0x1e28, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E30, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E30);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameRGBToRGBCoef20[0x1e30, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E38, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E38);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameRGBToRGBCoef30[0x1e38, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E40, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E40);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameRGBToRGBCoef40[0x1e40, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E48, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E48);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoef00[0x1e48, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E50, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E50);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoef10[0x1e50, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E58, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E58);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoef20[0x1e58, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E60, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E60);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoef30[0x1e60, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E68, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E68);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoef40[0x1e68, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E70, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E70);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoefD00[0x1e70, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E78, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E78);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoefD10[0x1e78, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E80, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E80);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregRGBToYUVCoefD20[0x1e80, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E88, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E88);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYClampBound0[0x1e88, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1E90, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E90);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameUVClampBound0[0x1e90, 0x%x, 0x%x]\n", regVal, readVal);
	
	eswin_dc_write_reg(regs, 0x1E98, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1E98);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregMMUPrefetch0[0x1e98, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1EA0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1EA0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYOrArgbSTLBStartAddr0[0x1ea0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1EA8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1EA8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYOrArgbSTLBEndAddr0[0x1ea8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1EB0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1EB0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYOrArgbSTLBStride0[0x1eb0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1EB8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1EB8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregFrameYOrArgbSTLBMaxNum0[0x1eb8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1EC0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1EC0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoef00[0x1ec0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1F00, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1F00);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoef10[0x1f00, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1F40, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1F40);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoef20[0x1f40, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1F80, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1F80);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoef30[0x1f80, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x1FC0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x1FC0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoef40[0x1fc0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2000, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2000);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoefD00[0x2000, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2040, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2040);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoefD10[0x2040, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2080, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2080);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYUVToRGBCoefD20[0x2080, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x20C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x20C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayRGBToRGBCoef00[0x20c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2100, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2100);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayRGBToRGBCoef10[0x2100, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2140, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2140);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayRGBToRGBCoef20[0x2140, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2180, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2180);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayRGBToRGBCoef30[0x2180, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x21C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x21C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayRGBToRGBCoef40[0x21c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2200, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2200);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayDeGammaIndex0[0x2200, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2240, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2240);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayDeGammaData0[0x2240, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2280, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2280);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayDeGammaExData0[0x2280, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x22C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x22C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayYClampBound0[0x22c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2300, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2300);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregOverlayUVClampBound0[0x2300, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2340, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2340);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvSTLBStartAddr0[0x2340, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2348, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2348);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvSTLBEndAddr0[0x2348, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2350, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2350);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvSTLBStride0[0x2350, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2358, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2358);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvSTLBMaxNum0[0x2358, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2360, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2360);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameVSTLBStartAddr0[0x2360, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2368, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2368);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameVSTLBEndAddr0[0x2368, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2370, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2370);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameVSTLBStride0[0x2370, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2378, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2378);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameVSTLBMaxNum0[0x2378, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2380, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2380);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbSTLBStartAddr0[0x2380, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2388, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2388);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbSTLBEndAddr0[0x2388, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2390, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2390);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbSTLBStride0[0x2390, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2398, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2398);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbSTLBMaxNum0[0x2398, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23A0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23A0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvSTLBStartAddr0[0x23a0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23A8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23A8);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvSTLBEndAddr0[0x23a8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23B0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23B0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvSTLBStride0[0x23b0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23B8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23B8);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvSTLBMaxNum0[0x23b8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayVSTLBStartAddr0[0x23c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23C8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23C8);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayVSTLBEndAddr0[0x23c8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23D0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23D0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayVSTLBStride0[0x23d0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23D8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23D8);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayVSTLBMaxNum0[0x23d8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23E0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23E0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregCursorSTLBStartAddr0[0x23e0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23E8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23E8);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregCursorSTLBEndAddr0[0x23e8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23F0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23F0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameYOrArgbTscSTLBStartAddr0[0x23f0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x23F8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x23F8);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameYOrArgbTscSTLBEndAddr0[0x23f8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2400, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2400);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvTscSTLBStartAddr0[0x2400, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2408, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2408);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvTscSTLBEndAddr0[0x2408, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2410, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2410);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbTscSTLBStartAddr0[0x2410, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2418, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2418);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbTscSTLBEndAddr0[0x2418, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2420, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2420);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvTscSTLBStartAddr0[0x2420, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2428, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2428);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvTscSTLBEndAddr0[0x2428, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2430, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2430);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregTSCPrefetch0[0x2430, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2438, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2438);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameYOrArgbDecTscStartAddr0[0x2438, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2440, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2440);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameYOrArgbDecTscEndAddr0[0x2440, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2448, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2448);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameYOrArgbDecTscStride0[0x2448, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2450, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2450);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameYOrArgbDecTscMaxNum0[0x2450, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2458, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2458);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvDecTscStartAddr0[0x2458, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2460, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2460);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvDecTscEndAddr0[0x2460, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2468, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2468);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvDecTscStride0[0x2468, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2470, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2470);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregFrameUvDecTscMaxNum0[0x2470, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2478, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2478);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbDecTscStartAddr0[0x2478, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2480, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2480);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbDecTscEndAddr0[0x2480, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2488, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2488);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbDecTscStride0[0x2488, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2490, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2490);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayYOrArgbDecTscMaxNum0[0x2490, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x2498, regVal);
	readVal = eswin_dc_read_reg(regs, 0x2498);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvDecTscStartAddr0[0x2498, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x24A0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x24A0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvDecTscEndAddr0[0x24a0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x24A8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x24A8);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvDecTscStride0[0x24a8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x24B0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x24B0);
	if(readVal != regVal)
		printf("dc_reg_test_write: gcregOverlayUvDecTscMaxNum0[0x24b0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x24B8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x24B8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregThreedLutScale0[0x24b8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x24C0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x24C0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregThreedLutOffset0[0x24c0, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x24C8, regVal);
	readVal = eswin_dc_read_reg(regs, 0x24C8);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregThreedLutOffsetEx0[0x24c8, 0x%x, 0x%x]\n", regVal, readVal);

	eswin_dc_write_reg(regs, 0x24D0, regVal);
	readVal = eswin_dc_read_reg(regs, 0x24D0);
	if(readVal != regVal)
		printf("dc_reg_test_write: dcregThreedLutEnlarge0[0x24d0, 0x%x, 0x%x]\n", regVal, readVal);
	
	return 0;
}

/* dc clock Operation */
static void eswin_dc8k_reset(void *regs)
{
    unsigned int idle = 0;
    unsigned int val;

    val = eswin_dc_read_reg(regs, 0x0);
    eswin_dc_write_reg(regs, 0x0, val | (1 << 12));
    eswin_dc_write_reg(regs, 0x0, val);

    do
    {
    	eswin_dc_write_reg(regs, 0x04, 0);
        idle = eswin_dc_read_reg(regs, 0x04);

        if (idle == 0)
        {
            udelay(100000);
        }
    } while ((idle & (1 << 16)) == 0);
}

int dc_test_writeback(void *regs, unsigned int scaler)
{
	int ret;
    unsigned int i, j;
	unsigned int src_size, dest_size;
	unsigned int len;
	unsigned int *src_video_buf = NULL, *src_overlay_buf = NULL, *dst_buf = NULL;

	printf("\n");
	printf("++++++++++++++++++++dc_test_writeback[%d]++++++++++++++++++++++++\n", scaler);
    printf("\n");

	src_size = (TEST_FRAME_WIDTH * TEST_FRAME_HIGHT * (TEST_FRAME_BPP / 8));
	printf("dc_test_writeback: src_size = %d\n", src_size);
	src_video_buf = (unsigned int *)dc8000_input_video_image_buf;
	src_overlay_buf = (unsigned int *)dc8000_input_overlay_image_buf;

	dest_size = TEST_DEST_WIDTH * TEST_DEST_HIGHT * 4;
	dst_buf = (unsigned int *)DEST_BUF_ADDR;
	memset(dst_buf, 0xff, dest_size);
#if TEST_VIDEO
	printf("dc_test_writeback: before writeback => *src_video_buf = 0x%x, *dst_buf = 0x%x.\n", *src_video_buf, *dst_buf);
#else
	printf("dc_test_writeback: before writeback => *src_overlay_buf = 0x%x, *dst_buf = 0x%x.\n", *src_overlay_buf, *dst_buf);
#endif
	//sysconfig
    eswin_syscrg_config(0);
	//reset dc8000
	eswin_dc8k_reset(regs);

	len = sizeof(regTab) / sizeof(regTab[0]);
	printf("dc_test_writeback: len = %d\n", len);
	for(i = 0; i < len; i++)
	{
		switch (regTab[i].offset)
		{
		case REG_VIDEO_SRC_BUFFER_ADDR0:
			printf("dc_test_writeback: src_video_buf = 0x%n\n", src_video_buf);
			eswin_dc_write_reg(regs, regTab[i].offset, (u32)(u64)src_video_buf);
			break;
		case REG_OVERLAY_SRC_BUFFER_ADDR0:
			printf("dc_test_writeback: src_overlay_buf = 0x%n\n", src_overlay_buf);
			eswin_dc_write_reg(regs, regTab[i].offset, (u32)(u64)src_overlay_buf);
			break;
		case REG_DST1_BUFFER_ADDR0:
			printf("dc_test_writeback: dst_buf1 = 0x%px\n", dst_buf);
            eswin_dc_write_reg(regs, regTab[i].offset, (u32)(u64)dst_buf);
            break;
		case REG_DST2_BUFFER_ADDR0:
			printf("dc_test_writeback: dst_buf2 = 0x%px\n", dst_buf);
			eswin_dc_write_reg(regs, regTab[i].offset, (u32)(u64)dst_buf);
			break;
#if TEST_SCALER
#if TEST_VIDEO
		case REG_VIDEO_VERTI_KERNEL_INDEX:
			printf("dc_test_writeback: video horKernel/VerKernel write.\n");
			eswin_dc_write_reg(regs, regTab[i].offset, 0x0);
			for(j = 0; j < 128; j++)
				eswin_dc_write_reg(regs, 0x1a00, horKernel[j]);

			for(j = 0; j < 128; j++)
                eswin_dc_write_reg(regs, 0x1a10, verKernel[j]);
			break;
#else
		case REG_OVERLAY_VERTI_KERNEL_INDEX:
            printf("dc_test_writeback: overlay horKernel/VerKernel write.\n");
            eswin_dc_write_reg(regs, regTab[i].offset, 0x0);
            for(j = 0; j < 128; j++)
                eswin_dc_write_reg(regs, 0x1b00, horKernel[j]);

            for(j = 0; j < 128; j++)
                eswin_dc_write_reg(regs, 0x1b80, verKernel[j]);
            break;
#endif
#endif
		default:
			eswin_dc_write_reg(regs, regTab[i].offset, regTab[i].value);
			break;
		}
	}

	unsigned int src_addr, dst1_addr, dst2_addr;
	unsigned int read_val;
#if TEST_VIDEO
	src_addr = eswin_dc_read_reg(regs, REG_VIDEO_SRC_BUFFER_ADDR0);
#else
	src_addr = eswin_dc_read_reg(regs, REG_OVERLAY_SRC_BUFFER_ADDR0);
#endif
	dst1_addr = eswin_dc_read_reg(regs, REG_DST1_BUFFER_ADDR0);
	dst2_addr = eswin_dc_read_reg(regs, REG_DST2_BUFFER_ADDR0);
	printf("dc_test_writeback: after writeback => src_addr=0x%x, dst1_addr=0x%x, dst2_addr=0x%x.\n", src_addr, dst1_addr, dst2_addr);

	read_val = eswin_dc_read_reg(regs, 0x0004);
	printf("dc_test_writeback: after writeback => offset(0x0004)=0x%x.\n", read_val);
	read_val = eswin_dc_read_reg(regs, 0x000c);
	printf("dc_test_writeback: after writeback => offset(0x000c)=0x%x.\n", read_val);
#if TEST_VIDEO
	read_val = eswin_dc_read_reg(regs, 0x1408);
	printf("dc_test_writeback: after writeback => offset(0x1408)=0x%x.\n", read_val);
#else
	read_val = eswin_dc_read_reg(regs, 0x1600);
	printf("dc_test_writeback: after writeback => offset(0x1600)=0x%x.\n", read_val);
#endif
	read_val = eswin_dc_read_reg(regs, 0x1500);
	printf("dc_test_writeback: after writeback => offset(0x1500)=0x%x.\n", read_val);
	read_val = eswin_dc_read_reg(regs, 0x14f8);
	printf("dc_test_writeback: after writeback => offset(0x14f8)=0x%x.\n", read_val);
#if TEST_VIDEO
	read_val = eswin_dc_read_reg(regs, 0x1810);
	printf("dc_test_writeback: after writeback => offset(0x1810)=0x%x.\n", read_val);
#else
	read_val = eswin_dc_read_reg(regs, 0x17c0);
	printf("dc_test_writeback: after writeback => offset(0x17c0)=0x%x.\n", read_val);
#endif
	read_val = eswin_dc_read_reg(regs, 0x1430);
	printf("dc_test_writeback: after writeback => offset(0x1430)=0x%x.\n", read_val);
	read_val = eswin_dc_read_reg(regs, 0x1438);
	printf("dc_test_writeback: after writeback => offset(0x1438)=0x%x.\n", read_val);
	read_val = eswin_dc_read_reg(regs, 0x1440);
	printf("dc_test_writeback: after writeback => offset(0x1440)=0x%x.\n", read_val);
	read_val = eswin_dc_read_reg(regs, 0x1448);
	printf("dc_test_writeback: after writeback => offset(0x1448)=0x%x.\n", read_val);
	read_val = eswin_dc_read_reg(regs, 0x1540);
	printf("dc_test_writeback: after writeback => offset(0x1540)=0x%x.\n", read_val);
	read_val = eswin_dc_read_reg(regs, 0x1518);
	printf("dc_test_writeback: after writeback => offset(0x1518)=0x%x.\n", read_val);
#if TEST_VIDEO
#if TEST_SCALER
	ret = memcmp(src_video_buf, dst_buf, TEST_FRAME_BPP / 8);
#else
	ret = memcmp(src_video_buf, dst_buf, src_size);
#endif
	printf("dc_test_writeback: after writeback => *src_video_buf = 0x%x, *dst_buf = 0x%x.\n", *src_video_buf, *dst_buf);
#else
#if TEST_SCALER
	ret = memcmp(src_overlay_buf, dst_buf, TEST_FRAME_BPP / 8);
#else
    ret = memcmp(src_video_buf, dst_buf, src_size);
#endif
	printf("dc_test_writeback: after writeback => *src_overlay_buf = 0x%x, *dst_buf = 0x%x.\n", *src_overlay_buf, *dst_buf);
#endif
	if(ret == 0) {
		printf("dc_test_writeback: test result => success.\n");
	} else {
		printf("dc_test_writeback: test result => failed.\n");
	}
	return 0;
}

