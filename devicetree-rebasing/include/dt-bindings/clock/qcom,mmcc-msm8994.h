/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2020, Konrad Dybcio
 */

#ifndef _DT_BINDINGS_CLK_MSM_MMCC_8994_H
#define _DT_BINDINGS_CLK_MSM_MMCC_8994_H

/* Clocks */
#define MMPLL0_EARLY					0
#define MMPLL0_PLL						1
#define MMPLL1_EARLY					2
#define MMPLL1_PLL						3
#define MMPLL3_EARLY					4
#define MMPLL3_PLL						5
#define MMPLL4_EARLY					6
#define MMPLL4_PLL						7
#define MMPLL5_EARLY					8
#define MMPLL5_PLL						9
#define AXI_CLK_SRC						10
#define RBBMTIMER_CLK_SRC				11
#define PCLK0_CLK_SRC					12
#define PCLK1_CLK_SRC					13
#define MDP_CLK_SRC						14
#define VSYNC_CLK_SRC					15
#define BYTE0_CLK_SRC					16
#define BYTE1_CLK_SRC					17
#define ESC0_CLK_SRC					18
#define ESC1_CLK_SRC					19
#define MDSS_AHB_CLK					20
#define MDSS_PCLK0_CLK					21
#define MDSS_PCLK1_CLK					22
#define MDSS_VSYNC_CLK					23
#define MDSS_BYTE0_CLK					24
#define MDSS_BYTE1_CLK					25
#define MDSS_ESC0_CLK					26
#define MDSS_ESC1_CLK					27
#define CSI0_CLK_SRC					28
#define CSI1_CLK_SRC					29
#define CSI2_CLK_SRC					30
#define CSI3_CLK_SRC					31
#define VFE0_CLK_SRC					32
#define VFE1_CLK_SRC					33
#define CPP_CLK_SRC						34
#define JPEG0_CLK_SRC					35
#define JPEG1_CLK_SRC					36
#define JPEG2_CLK_SRC					37
#define CSI2PHYTIMER_CLK_SRC			38
#define FD_CORE_CLK_SRC					39
#define OCMEMNOC_CLK_SRC				40
#define CCI_CLK_SRC						41
#define MMSS_GP0_CLK_SRC				42
#define MMSS_GP1_CLK_SRC				43
#define JPEG_DMA_CLK_SRC				44
#define MCLK0_CLK_SRC					45
#define MCLK1_CLK_SRC					46
#define MCLK2_CLK_SRC					47
#define MCLK3_CLK_SRC					48
#define CSI0PHYTIMER_CLK_SRC			49
#define CSI1PHYTIMER_CLK_SRC			50
#define EXTPCLK_CLK_SRC					51
#define HDMI_CLK_SRC					52
#define CAMSS_AHB_CLK					53
#define CAMSS_CCI_CCI_AHB_CLK			54
#define CAMSS_CCI_CCI_CLK				55
#define CAMSS_VFE_CPP_AHB_CLK			56
#define CAMSS_VFE_CPP_AXI_CLK			57
#define CAMSS_VFE_CPP_CLK				58
#define CAMSS_CSI0_AHB_CLK				59
#define CAMSS_CSI0_CLK					60
#define CAMSS_CSI0PHY_CLK				61
#define CAMSS_CSI0PIX_CLK				62
#define CAMSS_CSI0RDI_CLK				63
#define CAMSS_CSI1_AHB_CLK				64
#define CAMSS_CSI1_CLK					65
#define CAMSS_CSI1PHY_CLK				66
#define CAMSS_CSI1PIX_CLK				67
#define CAMSS_CSI1RDI_CLK				68
#define CAMSS_CSI2_AHB_CLK				69
#define CAMSS_CSI2_CLK					70
#define CAMSS_CSI2PHY_CLK				71
#define CAMSS_CSI2PIX_CLK				72
#define CAMSS_CSI2RDI_CLK				73
#define CAMSS_CSI3_AHB_CLK				74
#define CAMSS_CSI3_CLK					75
#define CAMSS_CSI3PHY_CLK				76
#define CAMSS_CSI3PIX_CLK				77
#define CAMSS_CSI3RDI_CLK				78
#define CAMSS_CSI_VFE0_CLK				79
#define CAMSS_CSI_VFE1_CLK				80
#define CAMSS_GP0_CLK					81
#define CAMSS_GP1_CLK					82
#define CAMSS_ISPIF_AHB_CLK				83
#define CAMSS_JPEG_DMA_CLK				84
#define CAMSS_JPEG_JPEG0_CLK			85
#define CAMSS_JPEG_JPEG1_CLK			86
#define CAMSS_JPEG_JPEG2_CLK			87
#define CAMSS_JPEG_JPEG_AHB_CLK			88
#define CAMSS_JPEG_JPEG_AXI_CLK			89
#define CAMSS_MCLK0_CLK					90
#define CAMSS_MCLK1_CLK					91
#define CAMSS_MCLK2_CLK					92
#define CAMSS_MCLK3_CLK					93
#define CAMSS_MICRO_AHB_CLK				94
#define CAMSS_PHY0_CSI0PHYTIMER_CLK		95
#define CAMSS_PHY1_CSI1PHYTIMER_CLK		96
#define CAMSS_PHY2_CSI2PHYTIMER_CLK		97
#define CAMSS_TOP_AHB_CLK				98
#define CAMSS_VFE_VFE0_CLK				99
#define CAMSS_VFE_VFE1_CLK				100
#define CAMSS_VFE_VFE_AHB_CLK			101
#define CAMSS_VFE_VFE_AXI_CLK			102
#define FD_AXI_CLK						103
#define FD_CORE_CLK						104
#define FD_CORE_UAR_CLK					105
#define MDSS_AXI_CLK					106
#define MDSS_EXTPCLK_CLK				107
#define MDSS_HDMI_AHB_CLK				108
#define MDSS_HDMI_CLK					109
#define MDSS_MDP_CLK					110
#define MMSS_MISC_AHB_CLK				111
#define MMSS_MMSSNOC_AXI_CLK			112
#define MMSS_S0_AXI_CLK					113
#define OCMEMCX_OCMEMNOC_CLK			114
#define OXILI_GFX3D_CLK					115
#define OXILI_RBBMTIMER_CLK				116
#define OXILICX_AHB_CLK					117
#define VENUS0_AHB_CLK					118
#define VENUS0_AXI_CLK					119
#define VENUS0_OCMEMNOC_CLK				120
#define VENUS0_VCODEC0_CLK				121
#define VENUS0_CORE0_VCODEC_CLK			122
#define VENUS0_CORE1_VCODEC_CLK			123
#define VENUS0_CORE2_VCODEC_CLK			124
#define AHB_CLK_SRC						125
#define FD_AHB_CLK						126

/* GDSCs */
#define VENUS_GDSC						0
#define VENUS_CORE0_GDSC				1
#define VENUS_CORE1_GDSC				2
#define VENUS_CORE2_GDSC				3
#define CAMSS_TOP_GDSC					4
#define MDSS_GDSC						5
#define JPEG_GDSC						6
#define VFE_GDSC						7
#define CPP_GDSC						8
#define OXILI_GX_GDSC					9
#define OXILI_CX_GDSC					10
#define FD_GDSC							11

/* Resets */
#define CAMSS_MICRO_BCR					0

#endif