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

#include <drm/dw_mipi_dsi.h>

#include <config.h>
#include <common.h>
#include <errno.h>
#include <asm/unaligned.h>
#include <asm/io.h>
#include <dm/device.h>
#include <dm/read.h>
#include <dm/of_access.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <syscon.h>
#include <linux/iopoll.h>
#include <dm/device_compat.h>
#include <div64.h>

#include "eswin_display.h"
#include "eswin_crtc.h"
#include "eswin_connector.h"
#include "eswin_panel.h"
#include "eswin_phy.h"
#include "linux/delay.h"
// #include "linux/unaligned/le_byteshift.h"
#include "eswin_dc_reg.h"

#define __HAPS

#define UPDATE(v, h, l) (((v) << (l)) & GENMASK((h), (l)))

#define HWVER_131 0x31333100 /* IP version 1.31 */
#define VERSION GENMASK(31, 8)

#define DSI_VERSION 0x00
#define DSI_PWR_UP 0x04
#define RESET 0
#define POWERUP BIT(0)

#define DSI_CLKMGR_CFG 0x08
#define TO_CLK_DIVIDSION(div) (((div) & 0xff) << 8)
#define TX_ESC_CLK_DIVIDSION(div) (((div) & 0xff) << 0)

#define DSI_DPI_VCID 0x0c
#define DPI_VID(vid) (((vid) & 0x3) << 0)

#define DSI_DPI_COLOR_CODING 0x10
#define LOOSELY18_EN BIT(8)
#define EN18_LOOSELY BIT(8)
#define DPI_COLOR_CODING_16BIT_1 0x0
#define DPI_COLOR_CODING_16BIT_2 0x1
#define DPI_COLOR_CODING_16BIT_3 0x2
#define DPI_COLOR_CODING_18BIT_1 0x3
#define DPI_COLOR_CODING_18BIT_2 0x4
#define DPI_COLOR_CODING_24BIT 0x5

#define DSI_DPI_CFG_POL 0x14
#define COLORM_ACTIVE_LOW BIT(4)
#define SHUTD_ACTIVE_LOW BIT(3)
#define HSYNC_ACTIVE_LOW BIT(2)
#define VSYNC_ACTIVE_LOW BIT(1)
#define DATAEN_ACTIVE_LOW BIT(0)

#define DSI_DPI_LP_CMD_TIM 0x18
#define OUTVACT_LPCMD_TIME(p) (((p) & 0xff) << 16)
#define INVACT_LPCMD_TIME(p) ((p) & 0xff)

#define DSI_DBI_VCID 0x1c
#define DBI_VCID(x) UPDATE(x, 1, 0)
#define DSI_DBI_CFG 0x20
#define DSI_DBI_CMDSIZE 0x28

#define DSI_PCKHDL_CFG 0x2c
#define CRC_RX_EN BIT(4)
#define ECC_RX_EN BIT(3)
#define BTA_EN BIT(2)
#define EOTP_RX_EN BIT(1)
#define EOTP_TX_EN BIT(0)

#define DSI_MODE_CFG 0x34
#define ENABLE_VIDEO_MODE 0
#define ENABLE_CMD_MODE BIT(0)

#define CMD_VIDEO_MODE BIT(0)
#define COMMAND_MODE BIT(0)
#define VIDEO_MODE 0

#define DSI_VID_MODE_CFG 0x38
#define ENABLE_LOW_POWER (0x3f << 8)
#define ENABLE_LOW_POWER_MASK (0x3f << 8)
#define VID_MODE_TYPE_NON_BURST_SYNC_PULSES 0x0
#define VID_MODE_TYPE_NON_BURST_SYNC_EVENTS 0x1
#define VID_MODE_TYPE_BURST 0x2
#define VID_MODE_TYPE_MASK 0x3

#define ENABLE_LOW_POWER_CMD BIT(15)
#define VID_MODE_VPG_ENABLE BIT(16)
#define VID_MODE_VPG_MODE BIT(20)
#define VID_MODE_VPG_HORIZONTAL BIT(24)

#define VPG_HORIZONTAL BIT(24)
#define VPG_VERTICAL_ONLY BIT(20)
#define VPG_EN BIT(16)
#define LP_CMD_EN BIT(15)
#define FRAME_BTA_ACK BIT(14)
#define LP_HFP_EN BIT(13)
#define LP_HBP_EN BIT(12)
#define LP_VACT_EN BIT(11)
#define LP_VFP_EN BIT(10)
#define LP_VBP_EN BIT(9)
#define LP_VSA_EN BIT(8)
#define VID_MODE_TYPE_BURST_SYNC_PULSES 0x0
#define VID_MODE_TYPE_BURST_SYNC_EVENTS 0x1
#define VID_MODE_TYPE_BURST 0x2

#define DSI_VID_PKT_SIZE 0x3c
#define VID_PKT_SIZE(p) (((p) & 0x3fff) << 0)
#define VID_PKT_MAX_SIZE 0x3fff

#define DSI_VID_NUM_CHUMKS 0x40
#define DSI_VID_NULL_PKT_SIZE 0x44
#define DSI_VID_HSA_TIME 0x48
#define DSI_VID_HBP_TIME 0x4c
#define DSI_VID_HLINE_TIME 0x50
#define DSI_VID_VSA_LINES 0x54
#define DSI_VID_VBP_LINES 0x58
#define DSI_VID_VFP_LINES 0x5c
#define DSI_VID_VACTIVE_LINES 0x60
#define DSI_EDPI_CMD_SIZE 0x64
#define DSI_CMD_MODE_CFG 0x68
#define MAX_RD_PKT_SIZE BIT(24)
#define DCS_LW_TX BIT(19)
#define DCS_SR_0P_TX BIT(18)
#define DCS_SW_1P_TX BIT(17)
#define DCS_SW_0P_TX BIT(16)
#define GEN_LW_TX BIT(14)
#define GEN_SR_2P_TX BIT(13)
#define GEN_SR_1P_TX BIT(12)
#define GEN_SR_0P_TX BIT(11)
#define GEN_SW_2P_TX BIT(10)
#define GEN_SW_1P_TX BIT(9)
#define GEN_SW_0P_TX BIT(8)
#define ACK_RQST_EN BIT(1)
#define TEAR_FX_EN BIT(0)

#define CMD_MODE_ALL_LP                                                                                  \
    (MAX_RD_PKT_SIZE | DCS_LW_TX | DCS_SR_0P_TX | DCS_SW_1P_TX | DCS_SW_0P_TX | GEN_LW_TX | GEN_SR_2P_TX \
     | GEN_SR_1P_TX | GEN_SR_0P_TX | GEN_SR_2P_TX | GEN_SW_2P_TX | GEN_SW_1P_TX | GEN_SW_0P_TX)

#define DSI_GEN_HDR 0x6c
#define GEN_HDATA(data) (((data) & 0xffff) << 8)
#define GEN_HDATA_MASK (0xffff << 8)
#define GEN_HTYPE(type) (((type) & 0xff) << 0)
#define GEN_HTYPE_MASK 0xff

#define DSI_GEN_PLD_DATA 0x70

#define DSI_CMD_PKT_STATUS 0x74
#define GEN_CMD_EMPTY BIT(0)
#define GEN_CMD_FULL BIT(1)
#define GEN_PLD_W_EMPTY BIT(2)
#define GEN_PLD_W_FULL BIT(3)
#define GEN_PLD_R_EMPTY BIT(4)
#define GEN_PLD_R_FULL BIT(5)
#define GEN_RD_CMD_BUSY BIT(6)

#define DSI_TO_CNT_CFG 0x78
#define HSTX_TO_CNT(p) (((p) & 0xffff) << 16)
#define LPRX_TO_CNT(p) ((p) & 0xffff)

#define DSI_BTA_TO_CNT 0x8c
#define DSI_LPCLK_CTRL 0x94
#define AUTO_CLKLANE_CTRL BIT(1)
#define PHY_TXREQUESTCLKHS BIT(0)

#define DSI_PHY_TMR_LPCLK_CFG 0x98
#define PHY_CLKHS2LP_TIME(lbcc) (((lbcc) & 0x3ff) << 16)
#define PHY_CLKLP2HS_TIME(lbcc) ((lbcc) & 0x3ff)

#define DSI_PHY_TMR_CFG 0x9c
#define PHY_HS2LP_TIME(lbcc) (((lbcc) & 0xff) << 24)
#define PHY_LP2HS_TIME(lbcc) (((lbcc) & 0xff) << 16)
#define MAX_RD_TIME(lbcc) ((lbcc) & 0x7fff)
#define PHY_HS2LP_TIME_V131(lbcc) (((lbcc) & 0x3ff) << 16)
#define PHY_LP2HS_TIME_V131(lbcc) ((lbcc) & 0x3ff)

#define DSI_PHY_RSTZ 0xa0
#define PHY_DISFORCEPLL 0
#define PHY_ENFORCEPLL BIT(3)
#define PHY_DISABLECLK 0
#define PHY_ENABLECLK BIT(2)
#define PHY_RSTZ 0
#define PHY_UNRSTZ BIT(1)
#define PHY_SHUTDOWNZ 0
#define PHY_UNSHUTDOWNZ BIT(0)

#define DSI_PHY_IF_CFG 0xa4
#define N_LANES(n) ((((n)-1) & 0x3) << 0)
#define PHY_STOP_WAIT_TIME(cycle) (((cycle) & 0xff) << 8)

#define DSI_PHY_STATUS 0xb0
#define PHY_STOP_STATE_CLK_LANE BIT(2)

#define PHY_STOPSTATE0LANE BIT(4)
#define PHY_STOPSTATECLKLANE BIT(2)
#define PHY_LOCK BIT(0)
#define PHY_STOPSTATELANE (PHY_STOPSTATE0LANE | PHY_STOPSTATECLKLANE)

#define DSI_PHY_TST_CTRL0 0xb4
#define PHY_TESTCLK BIT(1)
#define PHY_UNTESTCLK 0
#define PHY_TESTCLR BIT(0)
#define PHY_UNTESTCLR 0

#define DSI_PHY_TST_CTRL1 0xb8
#define PHY_TESTEN BIT(16)
#define PHY_UNTESTEN 0
#define PHY_TESTDOUT(n) (((n) & 0xff) << 8)
#define PHY_TESTDIN(n) (((n) & 0xff) << 0)
#define INPUT_DIVIDER(val) (val & 0x7f)

#define DSI_INT_ST0 0xbc
#define DSI_INT_ST1 0xc0
#define DSI_INT_MSK0 0xc4
#define DSI_INT_MSK1 0xc8

#define DSI_PHY_TMR_RD_CFG 0xf4
#define MAX_RD_TIME_V131(lbcc) ((lbcc) & 0x7fff)

#define PHY_STATUS_TIMEOUT_US 10000
#define CMD_PKT_STATUS_TIMEOUT_US 50000

/* Test Code: 0x44 (HS RX Control of Lane 0) */
#define HSFREQRANGE(x) UPDATE(x, 6, 1)
/* Test Code: 0x17 (PLL Input Divider Ratio) */
#define INPUT_DIV(x) UPDATE(x, 6, 0)
/* Test Code: 0x18 (PLL Loop Divider Ratio) */
#define FEEDBACK_DIV_LO(x) UPDATE(x, 4, 0)
#define FEEDBACK_DIV_HI(x) (BIT(7) | UPDATE(x, 3, 0))

#define TO_CLK_DIVISION(div) (((div) & 0xff) << 8)
#define TX_ESC_CLK_DIVISION(div) ((div) & 0xff)
#define PHYS_BUS_ADDR 0x502c0000
#define USEC_PER_SEC 1000000
#define MSEC_PER_SEC 1000
// #define DIV_ROUND_UP(n, d) (((n) + (d)-1) / (d))
// #define ULONG_MAX 4294967295
#define PLL_INPUT_REF_CLK 20000000

#define LOW_PROGRAM_EN 0
#define HIGH_PROGRAM_EN BIT(7)
#define LOOP_DIV_LOW_SEL(val) (val & 0x1f)
#define LOOP_DIV_HIGH_SEL(val) ((val >> 5) & 0xf)
#define PLL_LOOP_DIV_EN BIT(5)
#define PLL_INPUT_DIV_EN BIT(4)

#define LPF_RESISTORS_15_5KOHM 0x1
#define LPF_RESISTORS_13KOHM 0x2
#define LPF_RESISTORS_11_5KOHM 0x4
#define LPF_RESISTORS_10_5KOHM 0x8
#define LPF_RESISTORS_8KOHM 0x10
#define LPF_PROGRAM_EN BIT(6)
#define LPF_RESISTORS_SEL(val) ((val) & 0x3f)

#define PLL_BIAS_CUR_SEL_CAP_VCO_CONTROL 0x10
#define PLL_CP_CONTROL_PLL_LOCK_BYPASS 0x11
#define PLL_LPF_AND_CP_CONTROL 0x12
#define PLL_INPUT_DIVIDER_RATIO 0x17
#define PLL_LOOP_DIVIDER_RATIO 0x18
#define PLL_INPUT_AND_LOOP_DIVIDER_RATIOS_CONTROL 0x19

#define PLL_PARAM(_maxfreq, _vco_ctl, _cpbias_ctl, _gmp_ctl, _int_ctl, _prop_ctl)                        \
    {                                                                                                    \
        .max_freq = _maxfreq, .vco_cntrl = _vco_ctl, .cpbias_cntrl = _cpbias_ctl, .gmp_cntrl = _gmp_ctl, \
        .int_cntrl = _int_ctl, .prop_cntrl = _prop_ctl,                                                  \
    }

#define HSTT(_maxfreq, _c_lp2hs, _c_hs2lp, _d_lp2hs, _d_hs2lp) \
    {                                                          \
        .maxfreq = _maxfreq, .timing = {                       \
            .clk_lp2hs = _c_lp2hs,                             \
            .clk_hs2lp = _c_hs2lp,                             \
            .data_lp2hs = _d_lp2hs,                            \
            .data_hs2lp = _d_hs2lp,                            \
        }                                                      \
    }

static int eswin_dw_mipi_dsi_bind(struct udevice *dev);

struct eswin_dw_mipi_dsi_plat_data {
    unsigned long max_bit_rate_per_lane;
};

struct eswin_mipi_dphy {
    /* Non-SNPS PHY */
    struct eswin_phy *phy;

    u16 input_div;
    u16 feedback_div;
};

struct pll_parameter {
    u32 max_freq;
    u32 actual_freq;
    u16 cfg_freq_range;
    u16 hs_freq_range;
    u16 pll_n;
    u16 pll_m;
    u8 vco_cntrl;
    u8 cpbias_cntrl;
    u8 gmp_cntrl;
    u8 int_cntrl;
    u8 prop_cntrl;
};

struct dw_mipi_dsi_dphy_timing {
    u16 data_hs2lp;
    u16 data_lp2hs;
    u16 clk_hs2lp;
    u16 clk_lp2hs;
};

struct hstt {
    unsigned int maxfreq;
    struct dw_mipi_dsi_dphy_timing timing;
};

struct eswin_dw_mipi_dsi {
    struct udevice *dev;
    void *base;
    int id;
    struct eswin_dw_mipi_dsi *master;
    struct eswin_dw_mipi_dsi *slave;
    bool prepared;

    unsigned int lane_mbps; /* per lane */
    u32 channel;
    u32 lanes;
    u32 format;
    u32 mode_flags;
    struct eswin_mipi_dphy dphy;
    struct drm_display_mode mode;

    const struct eswin_dw_mipi_dsi_plat_data *pdata;
    struct pll_parameter pll_param;
    // unsigned int lane_mbps;
};

struct pll_parameter pll_tbl[] = {
    PLL_PARAM(55, 0x3f, 0x10, 0x01, 0x00, 0x0D),   PLL_PARAM(82, 0x37, 0x10, 0x01, 0x00, 0x0D),
    PLL_PARAM(110, 0x2f, 0x10, 0x01, 0x00, 0x0D),  PLL_PARAM(165, 0x27, 0x10, 0x01, 0x00, 0x0D),
    PLL_PARAM(220, 0x1f, 0x10, 0x01, 0x00, 0x0D),  PLL_PARAM(330, 0x17, 0x10, 0x01, 0x00, 0x0D),
    PLL_PARAM(440, 0x0f, 0x10, 0x01, 0x00, 0x0D),  PLL_PARAM(660, 0x07, 0x10, 0x01, 0x00, 0x0D),
    PLL_PARAM(1149, 0x03, 0x10, 0x01, 0x00, 0x0D), PLL_PARAM(1152, 0x01, 0x10, 0x01, 0x00, 0x0D),
    PLL_PARAM(1250, 0x01, 0x10, 0x01, 0x00, 0x0E),
};

/* Table A-4 High-Speed Transition Times (High-Speed Entry LP->HS DATA LANE(Considers HS_CLK_LANE_ENTRY)d)*/
struct hstt hstt_table[] = {
    HSTT(80, 21, 17, 35, 10),      HSTT(90, 23, 17, 39, 10),      HSTT(100, 22, 17, 37, 10),
    HSTT(110, 25, 18, 43, 11),     HSTT(120, 26, 20, 46, 11),     HSTT(130, 27, 19, 46, 11),
    HSTT(140, 27, 19, 46, 11),     HSTT(150, 28, 20, 47, 12),     HSTT(160, 30, 21, 53, 13),
    HSTT(170, 30, 21, 55, 13),     HSTT(180, 31, 21, 53, 13),     HSTT(190, 32, 22, 58, 13),
    HSTT(205, 35, 22, 58, 13),     HSTT(220, 37, 26, 63, 15),     HSTT(235, 38, 28, 65, 16),
    HSTT(250, 41, 29, 71, 17),     HSTT(275, 43, 29, 74, 18),     HSTT(300, 45, 32, 80, 19),
    HSTT(325, 48, 33, 86, 18),     HSTT(350, 51, 35, 91, 20),     HSTT(400, 59, 37, 102, 21),
    HSTT(450, 65, 40, 115, 23),    HSTT(500, 71, 41, 126, 24),    HSTT(550, 77, 44, 135, 26),
    HSTT(600, 82, 46, 147, 27),    HSTT(650, 87, 48, 156, 28),    HSTT(700, 94, 52, 166, 29),
    HSTT(750, 99, 52, 175, 31),    HSTT(800, 105, 55, 187, 32),   HSTT(850, 110, 58, 196, 32),
    HSTT(900, 115, 58, 206, 35),   HSTT(950, 120, 62, 213, 36),   HSTT(1000, 128, 63, 225, 38),
    HSTT(1050, 132, 65, 234, 38),  HSTT(1100, 138, 67, 243, 39),  HSTT(1150, 146, 69, 259, 42),
    HSTT(1200, 151, 71, 269, 43),  HSTT(1250, 153, 74, 273, 45),  HSTT(1300, 160, 73, 282, 46),
    HSTT(1350, 165, 76, 294, 47),  HSTT(1400, 172, 78, 304, 49),  HSTT(1450, 177, 80, 314, 49),
    HSTT(1500, 183, 81, 326, 52),  HSTT(1550, 191, 84, 339, 52),  HSTT(1600, 194, 85, 345, 52),
    HSTT(1650, 201, 86, 355, 53),  HSTT(1700, 208, 88, 368, 53),  HSTT(1750, 212, 89, 378, 53),
    HSTT(1800, 220, 90, 389, 54),  HSTT(1850, 223, 92, 401, 54),  HSTT(1900, 231, 91, 413, 55),
    HSTT(1950, 236, 95, 422, 56),  HSTT(2000, 243, 97, 432, 56),  HSTT(2050, 248, 99, 442, 58),
    HSTT(2100, 252, 100, 454, 59), HSTT(2150, 259, 102, 460, 61), HSTT(2200, 266, 105, 476, 62),
    HSTT(2250, 269, 109, 481, 63), HSTT(2300, 272, 109, 490, 65), HSTT(2350, 281, 112, 502, 66),
    HSTT(2400, 283, 115, 509, 66), HSTT(2450, 282, 115, 510, 67), HSTT(2500, 281, 118, 508, 67),
};

u32 eswin_read_reg(void *regs, u32 addr) {
    u32 read = 0;

    read = readl((unsigned char *)regs + addr);
    return read;
}

void eswin_write_reg(void *regs, u32 addr, u32 data) {
    writel(data, (u32 *)regs + addr);
}

static inline void eswin_dsi_write(struct eswin_dw_mipi_dsi *dsi, u32 reg, u32 val) {
    writel(val, dsi->base + reg);
    // printf("mipi reg :0x%04x --> write val:0x%x\n", reg, val);
    udelay(10);
#if defined(__DEBUG)
    printf("reg offset:%x --> write val:%x\n", reg, val);
    int reg_val = 0;
    reg_val = readl(dsi->base + reg);
    reg_val = readl(dsi->base + reg);
    printf("reg offset: %x --> read value:%x\n", reg, reg_val);
#endif
}

static inline u32 eswin_dsi_read(struct eswin_dw_mipi_dsi *dsi, u32 reg) {
    u32 val = 0;
    val = readl(dsi->base + reg);
    val = readl(dsi->base + reg);
    // return readl(dsi->base + reg);
    return val;
}

static inline void dsi_write(struct eswin_dw_mipi_dsi *dsi, u32 reg, u32 val) {
    writel(val, dsi->base + reg);
    // printf("mipi reg :0x%04x --> write val:0x%x\n", reg, val);
    udelay(10);
#if defined(__DEBUG)
    printf("reg offset:%x --> write val:%x\n", reg, val);
    int reg_val = 0;
    reg_val = readl(dsi->base + reg);
    reg_val = readl(dsi->base + reg);
    printf("reg offset: %x --> read value:%x\n", reg, reg_val);
#endif
}

static inline u32 dsi_read(struct eswin_dw_mipi_dsi *dsi, u32 reg) {
    u32 val = 0;
    val = readl(dsi->base + reg);
    val = readl(dsi->base + reg);
    // return readl(dsi->base + reg);
    return val;
}

static inline void eswin_dsi_update_bits(struct eswin_dw_mipi_dsi *dsi, u32 reg, u32 mask, u32 val) {
    u32 orig, tmp;

    orig = eswin_dsi_read(dsi, reg);
    tmp = orig & ~mask;
    tmp |= val & mask;
    eswin_dsi_write(dsi, reg, tmp);
}

static inline void eswin_mipi_dphy_enableclk_assert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_ENABLECLK, PHY_ENABLECLK);
    udelay(1);
}

static inline void eswin_mipi_dphy_enableclk_deassert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_ENABLECLK, 0);
    udelay(1);
}

static inline void eswin_mipi_dphy_shutdownz_assert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_SHUTDOWNZ, 0);
    udelay(1);
}

static inline void eswin_mipi_dphy_shutdownz_deassert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_SHUTDOWNZ, PHY_SHUTDOWNZ);
    udelay(1);
}

static inline void eswin_mipi_dphy_rstz_assert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_RSTZ, 0);
    udelay(1);
}

static inline void eswin_mipi_dphy_rstz_deassert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_RSTZ, PHY_RSTZ, PHY_RSTZ);
    udelay(1);
}

static inline void eswin_testif_testclk_assert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK, PHY_TESTCLK);
    udelay(1);
}

static inline void eswin_testif_testclk_deassert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK, 0);
    udelay(1);
}

static inline void eswin_testif_testclr_assert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLR, PHY_TESTCLR);
}

static inline void eswin_testif_testclr_deassert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLR, 0);
}

static inline void eswin_testif_testen_assert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_TST_CTRL1, PHY_TESTEN, PHY_TESTEN);
    udelay(1);
}

static inline void eswin_testif_testen_deassert(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_update_bits(dsi, DSI_PHY_TST_CTRL1, PHY_TESTEN, 0);
    udelay(1);
}

static void eswin_dw_mipi_message_config(struct eswin_dw_mipi_dsi *dsi, const struct mipi_dsi_msg *msg) {
    bool lpm = msg->flags & MIPI_DSI_MSG_USE_LPM;
    u32 val = 0;

    /*
     * TODO dw drv improvements
     * largest packet sizes during hfp or during vsa/vpb/vfp
     * should be computed according to byte lane, lane number and only
     * if sending lp cmds in high speed is enable (PHY_TXREQUESTCLKHS)
     */
    eswin_dsi_write(dsi, DSI_DPI_LP_CMD_TIM, OUTVACT_LPCMD_TIME(16) | INVACT_LPCMD_TIME(0));

    if (msg->flags & MIPI_DSI_MSG_REQ_ACK) val |= ACK_RQST_EN;
    if (lpm) val |= CMD_MODE_ALL_LP;

    eswin_dsi_write(dsi, DSI_CMD_MODE_CFG, val);

    val = eswin_dsi_read(dsi, DSI_VID_MODE_CFG);
    if (lpm)
        val |= ENABLE_LOW_POWER_CMD;
    else
        val &= ~ENABLE_LOW_POWER_CMD;
    eswin_dsi_write(dsi, DSI_VID_MODE_CFG, val);
}

static int dw_mipi_dsi_gen_pkt_hdr_write(struct eswin_dw_mipi_dsi *dsi, u32 hdr_val) {
    int ret;
    u32 val, mask;

    ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS, val, !(val & GEN_CMD_FULL), CMD_PKT_STATUS_TIMEOUT_US);
    if (ret) {
        dev_err(dsi->dev, "failed to get available command FIFO\n");
        return ret;
    }

    eswin_dsi_write(dsi, DSI_GEN_HDR, hdr_val);

    // 4lanes panel send cmd need delay
    if (dsi->lanes > 3) {
        udelay(10);
    }

    mask = GEN_CMD_EMPTY | GEN_PLD_W_EMPTY;
    ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS, val, (val & mask) == mask, CMD_PKT_STATUS_TIMEOUT_US);
    if (ret) {
        dev_err(dsi->dev, "failed to write command FIFO\n");
        return ret;
    }

    return 0;
}

static int dw_mipi_dsi_write(struct eswin_dw_mipi_dsi *dsi, const struct mipi_dsi_packet *packet) {
    const u8 *tx_buf = packet->payload;
    int len = packet->payload_length, pld_data_bytes = sizeof(u32), ret;
    __le32 word;
    u32 val;

    while (len) {
        if (len < pld_data_bytes) {
            word = 0;
            memcpy(&word, tx_buf, len);
            eswin_dsi_write(dsi, DSI_GEN_PLD_DATA, le32_to_cpu(word));
            len = 0;
        } else {
            memcpy(&word, tx_buf, pld_data_bytes);
            eswin_dsi_write(dsi, DSI_GEN_PLD_DATA, le32_to_cpu(word));
            tx_buf += pld_data_bytes;
            len -= pld_data_bytes;
        }

        ret =
            readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS, val, !(val & GEN_PLD_W_FULL), CMD_PKT_STATUS_TIMEOUT_US);
        if (ret) {
            dev_err(dsi->dev, "failed to get available write payload FIFO\n");
            return ret;
        }
    }

    word = 0;
    memcpy(&word, packet->header, sizeof(packet->header));
    return dw_mipi_dsi_gen_pkt_hdr_write(dsi, le32_to_cpu(word));
}

static int dw_mipi_dsi_read(struct eswin_dw_mipi_dsi *dsi, const struct mipi_dsi_msg *msg) {
    int i, j, ret, len = msg->rx_len;
    u8 *buf = msg->rx_buf;
    u32 val;

    /* Wait end of the read operation */
    ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS, val, !(val & GEN_RD_CMD_BUSY), CMD_PKT_STATUS_TIMEOUT_US);
    if (ret) {
        printf("Timeout during read operation\n");
        return ret;
    }

    for (i = 0; i < len; i += 4) {
        /* Read fifo must not be empty before all bytes are read */
        ret = readl_poll_timeout(dsi->base + DSI_CMD_PKT_STATUS, val, !(val & GEN_PLD_R_EMPTY),
                                 CMD_PKT_STATUS_TIMEOUT_US);
        if (ret) {
            printf("Read payload FIFO is empty\n");
            return ret;
        }

        val = eswin_dsi_read(dsi, DSI_GEN_PLD_DATA);
        for (j = 0; j < 4 && j + i < len; j++) buf[i + j] = val >> (8 * j);
    }

    return ret;
}

static ssize_t eswin_dw_mipi_dsi_transfer(struct eswin_dw_mipi_dsi *dsi, const struct mipi_dsi_msg *msg) {
    struct mipi_dsi_packet packet;
    int ret;
    int nb_bytes;

    /* create a packet to the DSI protocol */
    ret = mipi_dsi_create_packet(&packet, msg);
    if (ret) {
        printf("failed to create packet: %d\n", ret);
        return ret;
    }

    eswin_dw_mipi_message_config(dsi, msg);

    ret = dw_mipi_dsi_write(dsi, &packet);
    if (ret) return ret;

    if (msg->rx_buf && msg->rx_len) {
        ret = dw_mipi_dsi_read(dsi, msg);
        if (ret) return ret;
        nb_bytes = msg->rx_len;
    } else {
        nb_bytes = packet.size;
    }

    return nb_bytes;
}

static void eswin_dw_mipi_dsi_video_mode_config(struct eswin_dw_mipi_dsi *dsi) {
    u32 val;

    /*
     * TODO dw drv improvements
     * enabling low power is panel-dependent, we should use the
     * panel configuration here...
     */
    val = ENABLE_LOW_POWER;

    if (dsi->mode_flags & MIPI_DSI_MODE_VIDEO_BURST)
        val |= VID_MODE_TYPE_BURST;
    else if (dsi->mode_flags & MIPI_DSI_MODE_VIDEO_SYNC_PULSE)
        val |= VID_MODE_TYPE_NON_BURST_SYNC_PULSES;
    else
        val |= VID_MODE_TYPE_NON_BURST_SYNC_EVENTS;

    eswin_dsi_write(dsi, DSI_VID_MODE_CFG, val);
}

static void eswin_dw_mipi_dsi_disable(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_write(dsi, DSI_PWR_UP, RESET);
    eswin_dsi_write(dsi, DSI_LPCLK_CTRL, 0);
    eswin_dsi_write(dsi, DSI_EDPI_CMD_SIZE, 0);
    eswin_dsi_update_bits(dsi, DSI_MODE_CFG, CMD_VIDEO_MODE, COMMAND_MODE);
    eswin_dsi_write(dsi, DSI_PWR_UP, POWERUP);
}

static void eswin_dw_mipi_dsi_post_disable(struct eswin_dw_mipi_dsi *dsi) {
    if (!dsi->prepared) return;

    eswin_dsi_write(dsi, DSI_PWR_UP, RESET);
    eswin_dsi_write(dsi, DSI_PHY_RSTZ, 0);

    dsi->prepared = false;
}

static void eswin_dw_mipi_dsi_dpi_config(struct eswin_dw_mipi_dsi *dsi, const struct drm_display_mode *mode) {
    u32 val = 0, color = 0;

    switch (dsi->format) {
        case MIPI_DSI_FMT_RGB888:
            color = DPI_COLOR_CODING_24BIT;
            break;
        case MIPI_DSI_FMT_RGB666:
            color = DPI_COLOR_CODING_18BIT_2 | LOOSELY18_EN;
            break;
        case MIPI_DSI_FMT_RGB666_PACKED:
            color = DPI_COLOR_CODING_18BIT_1;
            break;
        case MIPI_DSI_FMT_RGB565:
            color = DPI_COLOR_CODING_16BIT_1;
            break;
    }

    if (!(mode->flags & DRM_MODE_FLAG_PVSYNC)) val |= VSYNC_ACTIVE_LOW;
    if (!(mode->flags & DRM_MODE_FLAG_PHSYNC)) val |= HSYNC_ACTIVE_LOW;

    eswin_dsi_write(dsi, DSI_DPI_VCID, DPI_VID(dsi->channel));
    eswin_dsi_write(dsi, DSI_DPI_COLOR_CODING, color);
    eswin_dsi_write(dsi, DSI_DPI_CFG_POL, val);
    /*
     * TODO dw drv improvements
     * largest packet sizes during hfp or during vsa/vpb/vfp
     * should be computed according to byte lane, lane number and only
     * if sending lp cmds in high speed is enable (PHY_TXREQUESTCLKHS)
     */
    // eswin_dsi_write(dsi, DSI_DPI_LP_CMD_TIM, OUTVACT_LPCMD_TIME(4) | INVACT_LPCMD_TIME(4));
}

static void eswin_dw_mipi_dsi_packet_handler_config(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_write(dsi, DSI_PCKHDL_CFG, CRC_RX_EN | ECC_RX_EN | BTA_EN);
}

static void eswin_dw_mipi_dsi_video_packet_config(struct eswin_dw_mipi_dsi *dsi, struct drm_display_mode *mode) {
    eswin_dsi_write(dsi, DSI_VID_PKT_SIZE, VID_PKT_SIZE(mode->hdisplay));
}

static void eswin_dw_mipi_dsi_command_mode_config(struct eswin_dw_mipi_dsi *dsi) {
    /*
     * TODO dw drv improvements
     * compute high speed transmission counter timeout according
     * to the timeout clock division (TO_CLK_DIVISION) and byte lane...
     */
    /* FIXME: Don't trigger time out. */
    eswin_dsi_write(dsi, DSI_TO_CNT_CFG, HSTX_TO_CNT(0) | LPRX_TO_CNT(0));
    /*
     * TODO dw drv improvements
     * the Bus-Turn-Around Timeout Counter should be computed
     * according to byte lane...
     */
    eswin_dsi_write(dsi, DSI_BTA_TO_CNT, 0xd00);
    eswin_dsi_write(dsi, DSI_MODE_CFG, ENABLE_CMD_MODE);
}

/* Get lane byte clock cycles. */
static int eswin_dw_mipi_dsi_get_hcomponent_lbcc(struct eswin_dw_mipi_dsi *dsi,
                                                 struct drm_display_mode *mode,
                                                 u32 hcomponent) {
    u32 frac, lbcc;

    lbcc = hcomponent * dsi->lane_mbps * MSEC_PER_SEC / 8;

    frac = lbcc % mode->clock;
    lbcc = lbcc / mode->clock;
    if (frac) lbcc++;

    return lbcc;
}

static void eswin_dw_mipi_dsi_line_timer_config(struct eswin_dw_mipi_dsi *dsi) {
    int htotal, hsa, hbp, lbcc;
    struct drm_display_mode *mode = &dsi->mode;
    htotal = mode->htotal;
    hsa = mode->hsync_end - mode->hsync_start;
    hbp = mode->htotal - mode->hsync_end;
    lbcc = eswin_dw_mipi_dsi_get_hcomponent_lbcc(dsi, mode, htotal);
    eswin_dsi_write(dsi, DSI_VID_HLINE_TIME, lbcc);
    lbcc = eswin_dw_mipi_dsi_get_hcomponent_lbcc(dsi, mode, hsa);
    eswin_dsi_write(dsi, DSI_VID_HSA_TIME, lbcc);
    lbcc = eswin_dw_mipi_dsi_get_hcomponent_lbcc(dsi, mode, hbp);
    eswin_dsi_write(dsi, DSI_VID_HBP_TIME, lbcc);
}

static void eswin_dw_mipi_dsi_vertical_timing_config(struct eswin_dw_mipi_dsi *dsi) {
    u32 vactive, vsa, vfp, vbp;
    struct drm_display_mode *mode = &dsi->mode;

    vactive = mode->vdisplay;
    vsa = mode->vsync_end - mode->vsync_start;
    vfp = mode->vsync_start - mode->vdisplay;
    vbp = mode->vtotal - mode->vsync_end;
    eswin_dsi_write(dsi, DSI_VID_VACTIVE_LINES, vactive);
    eswin_dsi_write(dsi, DSI_VID_VSA_LINES, vsa);
    eswin_dsi_write(dsi, DSI_VID_VFP_LINES, vfp);
    eswin_dsi_write(dsi, DSI_VID_VBP_LINES, vbp);
}

static int dw_mipi_dsi_phy_get_timing(struct eswin_dw_mipi_dsi *dsi, struct dw_mipi_dsi_dphy_timing *timing) {
    int i;
    unsigned int lane_mbps = dsi->lane_mbps;

    for (i = 0; i < ARRAY_SIZE(hstt_table); i++)
        if (lane_mbps < hstt_table[i].maxfreq) break;

    if (i == ARRAY_SIZE(hstt_table)) i--;

    *timing = hstt_table[i].timing;

    return 0;
}

static void eswin_dw_mipi_dsi_dphy_timing_config(struct eswin_dw_mipi_dsi *dsi) {
    struct dw_mipi_dsi_dphy_timing timing;
    int ret;

    ret = dw_mipi_dsi_phy_get_timing(dsi, &timing);
    if (ret) printk("Retrieving phy timings failed\n");

    /*
     * TODO dw drv improvements
     * data & clock lane timers should be computed according to panel
     * blankings and to the automatic clock lane control mode...
     * note: DSI_PHY_TMR_CFG.MAX_RD_TIME should be in line with
     * DSI_CMD_MODE_CFG.MAX_RD_PKT_SIZE_LP (see CMD_MODE_ALL_LP)
     */

    // hw_version = dsi_read(dsi, DSI_VERSION) & VERSION;

    /* FIXME: hw_version is lower than HWVER_131. But mipi works
     * correctly with DSI_PHY_TMR_CFG register using HWVER_131
     * configuration. */
    if (1 /*hw_version >= HWVER_131*/) {
        eswin_dsi_write(dsi, DSI_PHY_TMR_CFG,
                        PHY_HS2LP_TIME_V131(timing.data_hs2lp) | PHY_LP2HS_TIME_V131(timing.data_lp2hs));
        eswin_dsi_write(dsi, DSI_PHY_TMR_RD_CFG, MAX_RD_TIME_V131(10000));
    } else {
        eswin_dsi_write(dsi, DSI_PHY_TMR_CFG,
                        PHY_HS2LP_TIME(timing.data_hs2lp) | PHY_LP2HS_TIME(timing.data_lp2hs) | MAX_RD_TIME(10000));
    }

    eswin_dsi_write(dsi, DSI_PHY_TMR_LPCLK_CFG,
                    PHY_CLKHS2LP_TIME(timing.clk_hs2lp) | PHY_CLKLP2HS_TIME(timing.clk_lp2hs));
}

static void eswin_dw_mipi_dsi_dphy_interface_config(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_write(dsi, DSI_PHY_IF_CFG, PHY_STOP_WAIT_TIME(0x20) | N_LANES(dsi->lanes));
}

static void eswin_dw_mipi_dsi_clear_err(struct eswin_dw_mipi_dsi *dsi) {
    eswin_dsi_read(dsi, DSI_INT_ST0);
    eswin_dsi_read(dsi, DSI_INT_ST1);
    eswin_dsi_write(dsi, DSI_INT_MSK0, 0);
    eswin_dsi_write(dsi, DSI_INT_MSK1, 0);
}

static int dsi_phy_pll_get_params(struct eswin_dw_mipi_dsi *dsi_priv, u64 lane_bps) {
    int i;
    unsigned int min_prediv, max_prediv;
    unsigned int _prediv0 = 1, _prediv, best_prediv = 0;
    unsigned long best_freq = 0;
    unsigned long fvco_min, fvco_max, fin, fout, fvco;
    unsigned long f_multi, best_multi;
    unsigned long min_delta = ULONG_MAX;
    struct pll_parameter *pll_param = &dsi_priv->pll_param;

    // todo: fin defined in dts tree
    // fin = clk_get_rate(dsi_priv->pll_ref_clk);
    // if (!fin) {
    //     DRM_ERROR("Can not get pll ref clk for DPHY\n");
    //     return -EINVAL;
    // }
    fin = PLL_INPUT_REF_CLK;

    fout = lane_bps / 2;

    /* constraint: 2Mhz <= Fref / N <= 8MHz */
    min_prediv = DIV_ROUND_UP(fin, 8 * USEC_PER_SEC);
    max_prediv = fin / (2 * USEC_PER_SEC);

    /* constraint: 320MHz <= Fvco <= 1250Mhz */
    fvco_min = 320 * USEC_PER_SEC;
    fvco_max = 1250 * USEC_PER_SEC;

    if (fout * 4 < fvco_min)
        _prediv0 = 8;
    else if (fout * 2 < fvco_min)
        _prediv0 = 4;
    else if (fout < fvco_min)
        _prediv0 = 2;

    fvco = fout * _prediv0;

    for (_prediv = min_prediv; _prediv <= max_prediv; _prediv++) {
        u64 tmp;
        u32 delta;

        /* Fvco = Fref * M / N */
        tmp = (u64)fvco * _prediv;
        do_div(tmp, fin);
        f_multi = tmp;
        /* M range */
        if (f_multi < 64 || f_multi > 625) continue;

        tmp = (u64)f_multi * fin;
        do_div(tmp, _prediv);  // fvco
        if (tmp < fvco_min || tmp > fvco_max) continue;

        delta = abs(fvco - tmp);
        if (delta < min_delta) {
            best_prediv = _prediv;
            best_multi = f_multi;  // M
            min_delta = delta;
            best_freq = tmp / _prediv0;  // fout
        }
    }

    if (best_freq) {
        pll_param->pll_n = best_prediv - 1;
        pll_param->pll_m = best_multi - 2;
        pll_param->actual_freq = best_freq;
    } else {
        printf("Can not find best_freq for DPHY\n");
        return -EINVAL;
    }

    for (i = 0; i < ARRAY_SIZE(pll_tbl); i++) {
        if (pll_param->actual_freq <= pll_tbl[i].max_freq * USEC_PER_SEC) {
            pll_param->max_freq = pll_tbl[i].max_freq * USEC_PER_SEC;
            /* cfgclkfreqrange[5:0] = round[(Fcfg_clk(MHz)-17)*4] */
            pll_param->cfg_freq_range = (fin / USEC_PER_SEC - 17) * 4;
            // pll_param->hs_freq_range = dsi_phy_pll_get_hs_range(pll_param->actual_freq * 2);
            pll_param->vco_cntrl = pll_tbl[i].vco_cntrl;
            pll_param->cpbias_cntrl = pll_tbl[i].cpbias_cntrl;
            pll_param->gmp_cntrl = pll_tbl[i].gmp_cntrl;
            pll_param->int_cntrl = pll_tbl[i].int_cntrl;
            pll_param->prop_cntrl = pll_tbl[i].prop_cntrl;
            break;
        }
    }

    printf("max_freq = %d, actual_freq = %d, pll_n = %d, pll_m = %d, i=%d\n", pll_param->max_freq,
           pll_param->actual_freq, pll_param->pll_n, pll_param->pll_m, i);

    return 0;
}

static int es_dsi_get_lane_mbps(struct eswin_dw_mipi_dsi *dsi_priv,
                                struct drm_display_mode *mode,
                                unsigned long mode_flags,
                                u32 lanes,
                                u32 format,
                                unsigned int *lane_mbps) {
    int ret, bpp;
    u64 tmp;

    dsi_priv->format = format;
    bpp = mipi_dsi_pixel_format_to_bpp(dsi_priv->format);
    if (bpp < 0) {
        printf("failed to get bpp for pixel format %d\n", dsi_priv->format);
        return bpp;
    }

    tmp = (u64)mode->clock * 1000;
    /* Add 25% to pll out to be higher than pixel bandwidth (burst mode only) */
    if (mode_flags & MIPI_DSI_MODE_VIDEO_BURST) tmp = tmp * 125 / 100;

    tmp = tmp * bpp / lanes;

    ret = dsi_phy_pll_get_params(dsi_priv, tmp);
    if (ret) {
        return ret;
    }

    *lane_mbps = DIV_ROUND_UP(dsi_priv->pll_param.actual_freq * 2, USEC_PER_SEC);

    printf("bpp = %d, lane_mbps = %d\n", bpp, *lane_mbps);

    return 0;
}

static void eswin_dw_mipi_dsi_init(struct eswin_dw_mipi_dsi *dsi) {
    u8 esc_clk;
    u32 esc_clk_division;

    /* limit esc clk on FPGA */
    // if (!dsi->pclk)
    //     esc_clk = 5; /* 5MHz */
    // else
    esc_clk = 20; /* 20MHz */

    esc_clk_division = (dsi->lane_mbps >> 3) / esc_clk + 1;

    eswin_dsi_write(dsi, DSI_PWR_UP, RESET);

    /*
     * TODO dw drv improvements
     * timeout clock division should be computed with the
     * high speed transmission counter timeout and byte lane...
     */
    eswin_dsi_write(dsi, DSI_CLKMGR_CFG, TO_CLK_DIVISION(10) | TX_ESC_CLK_DIVISION(esc_clk_division));
}

static int eswin_dw_mipi_dsi_connector_init(struct display_state *state) {
    struct connector_state *conn_state = &state->conn_state;
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);

    printf("********[%s] enter*********\n", __func__);
    memcpy(&dsi->mode, &conn_state->mode, sizeof(struct drm_display_mode));

    dsi->dphy.phy = conn_state->phy;
    conn_state->output_mode = ESWIN_OUT_MODE_P888;
    conn_state->color_space = V4L2_COLORSPACE_DEFAULT;
    conn_state->type = DRM_MODE_CONNECTOR_DSI;
    printf("********[%s] leave*********\n", __func__);
    return 0;
}

static void eswin_dw_mipi_dsi_dphy_init(struct eswin_dw_mipi_dsi *dsi) {
    /* Clear PHY state */
    eswin_dsi_write(dsi, DSI_PHY_RSTZ, PHY_DISFORCEPLL | PHY_DISABLECLK | PHY_RSTZ | PHY_SHUTDOWNZ);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLR);
}

static void dw_mipi_dsi_phy_write_m(struct eswin_dw_mipi_dsi *dsi, u16 m) {
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL1, PHY_TESTEN | PHY_TESTDOUT(0) | PLL_LOOP_DIVIDER_RATIO);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK | PHY_UNTESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLK | PHY_UNTESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL1, LOOP_DIV_HIGH_SEL(m) | HIGH_PROGRAM_EN);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK | PHY_UNTESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLK | PHY_UNTESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL1, LOOP_DIV_LOW_SEL(m) | LOW_PROGRAM_EN);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK | PHY_UNTESTCLR);
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLK | PHY_UNTESTCLR);
}

static void dw_mipi_dsi_phy_write(struct eswin_dw_mipi_dsi *dsi, u8 test_code, u8 test_data) {
    /*
     * With the falling edge on TESTCLK, the TESTDIN[7:0] signal content
     * is latched internally as the current test code. Test data is
     * programmed internally by rising edge on TESTCLK.
     */
    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK | PHY_UNTESTCLR);

    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL1, PHY_TESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(test_code));

    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_UNTESTCLK | PHY_UNTESTCLR);

    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL1, PHY_UNTESTEN | PHY_TESTDOUT(0) | PHY_TESTDIN(test_data));

    eswin_dsi_write(dsi, DSI_PHY_TST_CTRL0, PHY_TESTCLK | PHY_UNTESTCLR);
}

static void mipi_tx_pll_setup(struct eswin_dw_mipi_dsi *dsi) {
    struct pll_parameter *pll_param = &dsi->pll_param;

    dw_mipi_dsi_phy_write(dsi, PLL_INPUT_AND_LOOP_DIVIDER_RATIOS_CONTROL, PLL_LOOP_DIV_EN | PLL_INPUT_DIV_EN);

    printf("set pll m:0x%x, n:0x%x, vco_cntrl:0x%x ,cpbias:0x%x, gmp:0x%x, int:0x%x, prop:0x%x\n", pll_param->pll_m,
           pll_param->pll_n, pll_param->vco_cntrl, pll_param->cpbias_cntrl, pll_param->gmp_cntrl, pll_param->int_cntrl,
           pll_param->prop_cntrl);
    //  0x17 cfg -> n
    dw_mipi_dsi_phy_write(dsi, PLL_INPUT_DIVIDER_RATIO, INPUT_DIVIDER(pll_param->pll_n));

    dw_mipi_dsi_phy_write_m(dsi, pll_param->pll_m);

    // 0x12 cfg
    dw_mipi_dsi_phy_write(dsi, PLL_LPF_AND_CP_CONTROL, LPF_PROGRAM_EN | pll_param->vco_cntrl);

    // 0x1c cfg
    dw_mipi_dsi_phy_write(dsi, 0x1c, pll_param->cpbias_cntrl);

    // 0x13 cfg
    dw_mipi_dsi_phy_write(dsi, 0x13, pll_param->gmp_cntrl << 4);

    // 0x0f cfg
    dw_mipi_dsi_phy_write(dsi, 0x0f, pll_param->int_cntrl);
    // 0x0f cfg
    dw_mipi_dsi_phy_write(dsi, 0x0e, pll_param->prop_cntrl);
}

static void eswin_dw_mipi_dsi_dphy_enable(struct eswin_dw_mipi_dsi *dsi) {
    u32 val;
    int ret;

    eswin_dsi_write(dsi, DSI_PHY_RSTZ, PHY_ENFORCEPLL | PHY_ENABLECLK | PHY_UNRSTZ | PHY_UNSHUTDOWNZ);

    ret = readl_poll_timeout(dsi->base + DSI_PHY_STATUS, val, val & PHY_LOCK, PHY_STATUS_TIMEOUT_US * 10);
    if (ret) printf("failed to wait phy lock state val:0x%x\n", val);
    // -------- dsi reg 0xb0 is 0x1fbd

    ret =
        readl_poll_timeout(dsi->base + DSI_PHY_STATUS, val, val & PHY_STOP_STATE_CLK_LANE, PHY_STATUS_TIMEOUT_US * 10);
    if (ret) printf("failed to wait phy clk lane stop state val:0x%x\n", val);
}

/*
 * The controller should generate 2 frames before
 * preparing the peripheral.
 */
static void eswin_dw_mipi_dsi_wait_for_two_frames(struct drm_display_mode *mode) {
    int refresh, two_frames;

    refresh = drm_mode_vrefresh(mode);
    two_frames = DIV_ROUND_UP(MSEC_PER_SEC, refresh) * 2;
    udelay(two_frames * 1000);
}

static void eswin_dw_mipi_dsi_set_mode(struct eswin_dw_mipi_dsi *dsi, unsigned long mode_flags) {
    u32 val;

    eswin_dsi_write(dsi, DSI_PWR_UP, RESET);

    if (mode_flags & MIPI_DSI_MODE_VIDEO) {
        eswin_dsi_write(dsi, DSI_MODE_CFG, ENABLE_VIDEO_MODE);
        // eswin_dsi_write(dsi, 0x38, 0x1010002);
        eswin_dw_mipi_dsi_video_mode_config(dsi);
    } else {
        eswin_dsi_write(dsi, DSI_MODE_CFG, ENABLE_CMD_MODE);
    }

    val = PHY_TXREQUESTCLKHS;
    if (mode_flags & MIPI_DSI_CLOCK_NON_CONTINUOUS) {
        val |= AUTO_CLKLANE_CTRL;
    }

    eswin_dsi_write(dsi, DSI_LPCLK_CTRL, val);
    eswin_dsi_write(dsi, DSI_PWR_UP, POWERUP);
}

int eswin_dw_mipi_dsi_connector_prepare(struct display_state *state) {
    printf("********[%s] enter*********\n", __func__);
    struct connector_state *conn_state = &state->conn_state;
    struct drm_display_mode *mode;
    int bpp;
    u32 value;
#if !defined(__HAPS)
    struct crtc_state *crtc_state = &state->crtc_state;
    unsigned long lane_rate;
#endif
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);
    if (dsi->prepared) return 0;
    memcpy(&dsi->mode, &conn_state->mode, sizeof(struct drm_display_mode));
    mode = &dsi->mode;

    printf("mode: clk:%d, hdsplay:%d, hstart:%d, hend:%d, htot:%d\n", mode->clock, mode->hdisplay, mode->hsync_start,
           mode->hsync_end, mode->htotal);
    printf("mode: vdsplay:%d, vstart:%d, vend:%d, vtot:%d, flags:0x%x\n", mode->vdisplay, mode->vsync_start,
           mode->vsync_end, mode->vtotal, mode->flags);
    printf("lanes: %d, format: %d\n", dsi->lanes, dsi->format);

    bpp = mipi_dsi_pixel_format_to_bpp(dsi->format);

    es_dsi_get_lane_mbps(dsi, mode, 0, dsi->lanes, dsi->format, &dsi->lane_mbps);

    eswin_dw_mipi_dsi_init(dsi);
    eswin_dw_mipi_dsi_dpi_config(dsi, mode);
    eswin_dw_mipi_dsi_packet_handler_config(dsi);
    eswin_dw_mipi_dsi_video_mode_config(dsi);
    eswin_dw_mipi_dsi_video_packet_config(dsi, mode);
    eswin_dw_mipi_dsi_command_mode_config(dsi);

    eswin_dw_mipi_dsi_line_timer_config(dsi);
    eswin_dw_mipi_dsi_vertical_timing_config(dsi);

    eswin_dw_mipi_dsi_dphy_init(dsi);
    eswin_dw_mipi_dsi_dphy_timing_config(dsi);
    eswin_dw_mipi_dsi_dphy_interface_config(dsi);

    eswin_dw_mipi_dsi_clear_err(dsi);

    mipi_tx_pll_setup(dsi);  // dphy pll set

    eswin_dw_mipi_dsi_dphy_enable(dsi);
    eswin_dw_mipi_dsi_wait_for_two_frames(mode);
    /* Switch to cmd mode for panel-bridge pre_enable & panel prepare */
    eswin_dw_mipi_dsi_set_mode(dsi, 0);

    dsi->prepared = true;
    value = eswin_dsi_read(dsi, 0x74);
    printf("0x74 = 0x%x\n", value);
    printf("********[%s] leave*********\n", __func__);
    return 0;
}

static void eswin_dw_mipi_dsi_connector_unprepare(struct display_state *state) {
    printf("********%s********\n", __func__);
    struct connector_state *conn_state = &state->conn_state;
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);

    eswin_dw_mipi_dsi_post_disable(dsi);
}

static int eswin_dw_mipi_dsi_connector_enable(struct display_state *state) {
    printf("********[%s] enter*********\n", __func__);
    struct connector_state *conn_state = &state->conn_state;
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);
    eswin_dw_mipi_dsi_set_mode(dsi, MIPI_DSI_MODE_VIDEO);
    printf("********[%s] leave*********\n", __func__);
    return 0;
}

static int eswin_dw_mipi_dsi_connector_disable(struct display_state *state) {
    struct connector_state *conn_state = &state->conn_state;
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(conn_state->dev);

    eswin_dw_mipi_dsi_disable(dsi);

    return 0;
}

static const struct eswin_connector_funcs eswin_dw_mipi_dsi_connector_funcs = {
    .init = eswin_dw_mipi_dsi_connector_init,
    .prepare = eswin_dw_mipi_dsi_connector_prepare,
    .unprepare = eswin_dw_mipi_dsi_connector_unprepare,
    .enable = eswin_dw_mipi_dsi_connector_enable,
    .disable = eswin_dw_mipi_dsi_connector_disable,
};

static int eswin_dw_mipi_dsi_probe(struct udevice *dev) {
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(dev);
    const struct eswin_connector *connector = (const struct eswin_connector *)dev_get_driver_data(dev);
    const struct eswin_dw_mipi_dsi_plat_data *pdata = connector->data;
    int id;

    dsi->base = dev_read_addr_ptr(dev);

    id = of_alias_get_id(ofnode_to_np(dev->node_), "dsi");
    if (id < 0) id = 0;

    dsi->dev = dev;
    dsi->pdata = pdata;
    dsi->id = id;

    eswin_syscrg_config(1485000000);
    eswin_dw_mipi_dsi_bind(dev);
    return 0;
}

static const struct eswin_dw_mipi_dsi_plat_data eswin_mipi_dsi_plat_data = {
    .max_bit_rate_per_lane = 2500000000UL,
};

static const struct eswin_connector eswin_mipi_dsi_driver_data = {
    .funcs = &eswin_dw_mipi_dsi_connector_funcs,
    .data = &eswin_mipi_dsi_plat_data,
};

static const struct udevice_id eswin_dw_mipi_dsi_ids[] = {{
                                                              .compatible = "eswin,mipi-dsi",
                                                              .data = (ulong)&eswin_mipi_dsi_driver_data,
                                                          },
                                                          {}};

static ssize_t eswin_dw_mipi_dsi_host_transfer(struct mipi_dsi_host *host, const struct mipi_dsi_msg *msg) {
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(host->dev);

    return eswin_dw_mipi_dsi_transfer(dsi, msg);
}

static int eswin_dw_mipi_dsi_host_attach(struct mipi_dsi_host *host, struct mipi_dsi_device *device) {
    struct eswin_dw_mipi_dsi *dsi = dev_get_priv(host->dev);

    if (device->lanes < 1 || device->lanes > 8) return -EINVAL;

    dsi->lanes = device->lanes;
    dsi->channel = device->channel;
    dsi->format = device->format;
    dsi->mode_flags = device->mode_flags;

    return 0;
}

static const struct mipi_dsi_host_ops eswin_dw_mipi_dsi_host_ops = {
    .attach = eswin_dw_mipi_dsi_host_attach,
    .transfer = eswin_dw_mipi_dsi_host_transfer,
};

static int eswin_dw_mipi_dsi_bind(struct udevice *dev) {
    struct mipi_dsi_host *host = dev_get_plat(dev);

    host->dev = dev;
    host->ops = &eswin_dw_mipi_dsi_host_ops;

    return dm_scan_fdt_dev(dev);
}

static int eswin_dw_mipi_dsi_child_post_bind(struct udevice *dev) {
    struct mipi_dsi_host *host = dev_get_plat(dev->parent);
    struct mipi_dsi_device *device = dev_get_parent_plat(dev);
    char name[20];

    sprintf(name, "%s.%d", host->dev->name, device->channel);
    device_set_name(dev, name);
    device->dev = dev;
    device->host = host;
    device->lanes = dev_read_u32_default(dev, "dsi,lanes", 4);
    device->format = dev_read_u32_default(dev, "dsi,format", MIPI_DSI_FMT_RGB888);
    device->mode_flags = dev_read_u32_default(dev, "dsi,flags",
                                              MIPI_DSI_MODE_VIDEO | MIPI_DSI_MODE_VIDEO_BURST | MIPI_DSI_MODE_VIDEO_HBP
                                                  | MIPI_DSI_MODE_EOT_PACKET | MIPI_DSI_MODE_VIDEO_HFP);
    device->channel = dev_read_u32_default(dev, "reg", 0);

    return 0;
}

static int eswin_dw_mipi_dsi_child_pre_probe(struct udevice *dev) {
    struct mipi_dsi_device *device = dev_get_parent_plat(dev);
    int ret;

    printf("%s IN--\n", __FUNCTION__);
    ret = mipi_dsi_attach(device);
    if (ret) {
        dev_err(dev, "mipi_dsi_attach() failed: %d\n", ret);
        return ret;
    }

    return 0;
}

U_BOOT_DRIVER(eswin_dw_mipi_dsi) = {
    .name = "eswin_dw_mipi_dsi",
    .id = UCLASS_DISPLAY,
    .of_match = eswin_dw_mipi_dsi_ids,
    .probe = eswin_dw_mipi_dsi_probe,
    .priv_auto = sizeof(struct eswin_dw_mipi_dsi),
    .per_child_plat_auto = sizeof(struct mipi_dsi_device),
    .plat_auto = sizeof(struct mipi_dsi_host),
    .child_post_bind = eswin_dw_mipi_dsi_child_post_bind,
    .child_pre_probe = eswin_dw_mipi_dsi_child_pre_probe,
};
