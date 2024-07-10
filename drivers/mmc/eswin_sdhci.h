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
 * Authors: liangshuang <liangshuang@eswincomputing.com>
 */

#ifndef _ESWIN_SDHCI_H_
#define _ESWIN_SDHCI_H_

#include <linux/bitops.h>

#define ESWIN_MSHC_CORE_CLK_REG 0x51828160UL
#define ESWIN_HSPDMA_RST_CTRL   0x5182841cUL
#define ESWIN_HSPDMA_SD_RST ((0x7 << 3) | (0x7 << 6) | (1 << 17) | (1 << 18) | (1 << 21) | (1 << 22))
#define ESWIN_HSPDMA_EMMC_RST ((0x7 << 3) | (0x7 << 6) | (1 << 19) | (1 << 23))

#define MSHC_CORE_CLK_ENABLE BIT(16)
#define MSHC_CORE_CLK_FREQ_BIT_SHIFT 4
#define MSHC_CORE_CLK_FREQ_BIT_MASK 0xfffu
#define MSHC_CORE_CLK_SEL_BIT BIT(0)

/* DWC_mshc_map/DWC_mshc_phy_block register */
#define DWC_MSHC_PTR_PHY_R 0x300
#define PHY_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x00)
#define PHY_CMDPAD_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x04)
#define PHY_DATAPAD_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x06)
#define PHY_CLKPAD_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x08)
#define PHY_STBPAD_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x0a)
#define PHY_RSTNPAD_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x0c)
#define PHY_PADTEST_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x0e)
#define PHY_PADTEST_OUT_R (DWC_MSHC_PTR_PHY_R + 0x10)
#define PHY_PADTEST_IN_R (DWC_MSHC_PTR_PHY_R + 0x12)
#define PHY_PRBS_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x18)
#define PHY_PHYLBK_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x1a)
#define PHY_COMMDL_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x1c)
#define PHY_SDCLKDL_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x1d)
#define PHY_SDCLKDL_DC_R (DWC_MSHC_PTR_PHY_R + 0x1e)
#define PHY_SMPLDL_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x20)
#define PHY_ATDL_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x21)
#define PHY_DLL_CTRL_R (DWC_MSHC_PTR_PHY_R + 0x24)
#define PHY_DLL_CNFG1_R (DWC_MSHC_PTR_PHY_R + 0x25)
#define PHY_DLL_CNFG2_R (DWC_MSHC_PTR_PHY_R + 0x26)
#define PHY_DLLDL_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x28)
#define PHY_DLL_OFFST_R (DWC_MSHC_PTR_PHY_R + 0x29)
#define PHY_DLLMST_TSTDC_R (DWC_MSHC_PTR_PHY_R + 0x2a)
#define PHY_DLLBT_CNFG_R (DWC_MSHC_PTR_PHY_R + 0x2c)
#define PHY_DLL_STATUS_R (DWC_MSHC_PTR_PHY_R + 0x2e)
#define PHY_DLLDBG_MLKDC_R (DWC_MSHC_PTR_PHY_R + 0x30)
#define PHY_DLLDBG_SLKDC_R (DWC_MSHC_PTR_PHY_R + 0x32)

#define ENABLE 1
#define DISABLE 0
/* strength definition */
#define PHYCTRL_DR_33OHM 0xee
#define PHYCTRL_DR_40OHM 0xcc
#define PHYCTRL_DR_50OHM 0x88
#define PHYCTRL_DR_66OHM 0x44
#define PHYCTRL_DR_100OHM 0x00

#define PHY_PAD_MAX_DRIVE_STRENGTH 0xf
#define PHY_CLK_MAX_DELAY_MASK 0x7f
#define PHY_PAD_SP_DRIVE_SHIF 16
#define PHY_PAD_SN_DRIVE_SHIF 20

#define PHY_RSTN BIT(0)
#define PHY_UPDATE_DELAY_CODE BIT(4)

#define VENDOR_MSHC_CTRL_R 0x508
#define VENDOR_EMMC_CTRL_R 0x52c
#define EMMC_CRAD_PRESENT BIT(0)
#define PHY_SLEW_0 0x0
#define PHY_SLEW_1 0x1
#define PHY_SLEW_2 0x2
#define PHY_SLEW_3 0x3
#define PHY_TX_SLEW_CTRL_P_BIT_SHIFT 5
#define PHY_TX_SLEW_CTRL_N_BIT_SHIFT 9

#define PHY_PULL_BIT_SHIF 0x3
#define PHY_PULL_DISABLED 0x0
#define PHY_PULL_UP 0x1
#define PHY_PULL_DOWN 0x2
#define PHY_PULL_MASK 0x3

#define PHY_PAD_RXSEL_0 0x0
#define PHY_PAD_RXSEL_1 0x1
#define PHY_PAD_RXSEL_2 0x2
#define PHY_PAD_RXSEL_3 0x3

#define VENDOR_AT_CTRL_R 0x540
#define LATENCY_LT_BIT_OFFSET 19
#define LATENCY_LT_MASK 0x3

#define LATENCY_LT_1 0x0
#define LATENCY_LT_2 0x1
#define LATENCY_LT_3 0x2
#define LATENCY_LT_4 0x3
#define SW_TUNE_ENABLE BIT(4)

#define VENDOR_AT_SATA_R 0x544
#define MAX_PHASE_CODE 0x7f

#define DLL_ENABEL BIT(0)
#define DLL_LOCK_STS BIT(0)
#define DLL_ERROR_STS BIT(1)
#define PHY_DELAY_CODE_MASK 0x7f
#define PHY_DELAY_CODE_MAX 0x7f

#define SDHCI_CLK_208M 208000000
#define SDHCI_CLK_200M 200000000
#define MAX_CORE_CLK_DIV 0xfff

/* dwc_mshc_map/DWC_mshc_block registers */
#define SDMASA_R                   0x00
#define BLOCKSIZE_R                0x04
#define SDMA_BUF_BDARY_4K          (0 << 12)
#define SDMA_BUF_BDARY_8K          (1 << 12)
#define SDMA_BUF_BDARY_16K         (2 << 12)
#define SDMA_BUF_BDARY_32K         (3 << 12)
#define SDMA_BUF_BDARY_64K         (4 << 12)
#define SDMA_BUF_BDARY_128K        (5 << 12)
#define SDMA_BUF_BDARY_256K        (6 << 12)
#define SDMA_BUF_BDARY_512K        (7 << 12)
#define BLOCKCOUNT_R               0x06
#define ARGUMENT_R                 0x08
#define XFER_MODE_R                0x0c
#define XFR_MODE_DMA_EN            BIT(0)
#define XFR_MODE_BLKCNT_EN         BIT(1)
#define XFR_MODE_AUTOCMD12_EN      (0x01 << 2)
#define XFR_MODE_AUTOCMD23_EN      (0x02 << 2)
#define XFR_MODE_AUTOCMD_AUTOSEL   (0x03 << 2)
#define XFR_MODE_DATA_READ         BIT(4)
#define XFR_MODE_MULTBLK_SEL       BIT(5)
#define XFR_MODE_RESP_TYPE         BIT(6)
#define XFR_MODE_RESP_ERRCHK_EN    BIT(7)
#define XFR_MODE_RESP_INT_EN       BIT(8)
#define CMD_R                      0x0e
#define SUB_CMD_FLAG               BIT(2)
#define CMD_CRC_CHK_ENABLE         BIT(3)
#define CMD_IDX_CHK_ENABLE         BIT(4)
#define DATA_PRESENT_SEL           BIT(5)
#define CMD_TYPE_NORMAL            (0 << 6)
#define CMD_TYPE_SUSPEND           (1 << 6)
#define CMD_TYPE_RESUME            (2 << 6)
#define CMD_TYPE_ABORT             (3 << 6)
#define RESP01_R                   0x10
#define RESP23_R                   0x14
#define RESP45_R                   0x18
#define RESP67_R                   0x1c
#define BUF_DATA_R                 0x20
#define PSTATE_REG_R               0x24
#define CMD_INHIBIT                BIT(0)
#define CMD_INHIBIT_DAT            BIT(1)
#define DAT_LINE_ACTIVE            BIT(2)
#define RE_TUNE_REQ                BIT(3)
#define DAT_7_4_MASK               (0xF << 4)
#define WR_XFER_ACTIVE             BIT(8)
#define RD_XFER_ACTIVE             BIT(9)
#define BUF_WR_ENABLE              BIT(10)
#define BUF_RD_ENABLE              BIT(11)
#define CARD_INSERTED              BIT(16)
#define CARD_STABLE                BIT(17)
#define HOST_CTRL1_R               0x28
#define LED_CTRL_ON                (1 << 0)
#define DAT_XFER_WIDTH_4BIT        (1 << 1)
#define HIGH_SPEED_EN              (1 << 2)
#define DMASEL_MASK                (0x3 << 3)
#define DMASEL_SDMA                (0 << 3)
#define DMASEL_ADMA2               (2 << 3)
#define DMASEL_ADMA3               (3 << 3)
#define EXTDAT_XFER_WIDTH8         (1 << 5)
#define PWR_CTRL_R                 0x29
#define SD_BUS_PWR_VDD1            (0x1 << 0)
#define SD_BUS_VOL_VDD1_1P8V       (0x5 << 1)
#define SD_BUS_VOL_VDD1_3P0V       (0x6 << 1)
#define SD_BUS_VOL_VDD1_3P3V       (0x7 << 1)
#define SD_BUS_PWR_VDD2            0x10
#define BGAP_CTRL_R                0x2a
#define WUP_CTRL_R                 0x2b
#define CLK_CTRL_R                 0x2c
#define INTERNAL_CLK_EN            BIT(0)
#define INTERNAL_CLK_STABLE        BIT(1)
#define SD_CLK_EN                  BIT(2)
#define PLL_ENABLE                 BIT(3)
#define CLK_GEN_SELECT             BIT(5)
#define UPPER_FREQ_SEL_MASK        0xc0
#define FREQ_SEL_MASK              0xff00
#define FREQ_SEL_VAL               (0x7d << 8) /* 0x7a-400khz , 0xfa-200Khz */
#define TOUT_CTRL_R                0x2e
#define SW_RST_R                   0x2f
#define SW_RST_ALL                 BIT(0)
#define SW_RST_CMD                 BIT(1)
#define SW_RST_DAT                 BIT(2)
#define NORMAL_INT_STAT_R          0x30
#define ERROR_INT_STAT_R           0x32
#define NORMAL_INT_STAT_EN_R       0x34
#define ERROR_INT_STAT_EN_R        0x36
#define NORMAL_INT_SIGNAL_EN_R     0x38
#define ERROR_INT_SIGNAL_EN_R      0x3a
#define CMD_COMPLETE_SIGNAL_EN     BIT(0) /* intr when response received */
#define XFER_COMPLETE_SIGNAL_EN    BIT(1) /* intr when data read/write xfer completed */
#define BGAP_EVENT_SIGNAL_EN       BIT(2)
#define DMA_INTERRUPT_SIGNAL_EN    BIT(3)
#define BUF_WR_READY_SIGNAL_EN     BIT(4)
#define BUF_RD_READY_SIGNAL_EN     BIT(5)
#define CARD_INSERTION_SIGNAL_EN   BIT(6)
#define CARD_REMOVAL_SIGNAL_EN     BIT(7)
#define CARD_INTERRUPT_SIGNAL_EN   BIT(8)
#define INT_A_SIGNAL_EN            BIT(9)
#define INT_B_SIGNAL_EN            BIT(10)
#define INT_C_SIGNAL_EN            BIT(11)
#define RE_TUNE_EVENT_SIGNAL_EN    BIT(12)
#define FX_EVENT_SIGNAL_EN         BIT(13)
#define CQE_EVENT_SIGNAL_EN        BIT(14)
#define CMD_TOUT_ERR_SIGNAL_EN     BIT(16) /* bit0 */
#define CMD_CRC_ERR_SIGNAL_EN      BIT(17) /* bit1 */
#define CMD_END_BIT_ERR_SIGNAL_EN  BIT(18)
#define CMD_IDX_ERR_SIGNAL_EN      BIT(19)
#define DATA_TOUT_ERR_SIGNAL_EN    BIT(20)
#define DATA_CRC_ERR_SIGNAL_EN     BIT(21)
#define DATA_END_BIT_ERR_SIGNAL_EN BIT(22)
#define CUR_LMT_ERR_SIGNAL_EN      BIT(23) /* bit7 */
#define AUTO_CMD_ERR_SIGNAL_EN     BIT(24) /* bit8 */
#define ADMA_ERR_SIGNAL_EN         BIT(25)
#define TUNING_ERR_SIGNAL_EN       BIT(26)
#define RESP_ERR_SIGNAL_EN         BIT(27)
#define BOOT_ACK_ERR_SIGNAL_EN     BIT(28)
#define VENDOR_ERR_SIGNAL_EN1      BIT(29)
#define VENDOR_ERR_SIGNAL_EN2      BIT(30)
#define VENDOR_ERR_SIGNAL_EN3      BIT(31)
#define SD_CMD_INTR_MASK                                                                           \
    (CMD_COMPLETE_SIGNAL_EN | CMD_TOUT_ERR_SIGNAL_EN | CMD_CRC_ERR_SIGNAL_EN |                     \
     CMD_END_BIT_ERR_SIGNAL_EN | CMD_IDX_ERR_SIGNAL_EN)
#define SD_DAT_INTR_MASK                                                                           \
    (BUF_RD_READY_SIGNAL_EN | BUF_WR_READY_SIGNAL_EN | XFER_COMPLETE_SIGNAL_EN |                   \
     DMA_INTERRUPT_SIGNAL_EN | DATA_TOUT_ERR_SIGNAL_EN | DATA_CRC_ERR_SIGNAL_EN |                  \
     DATA_END_BIT_ERR_SIGNAL_EN | BGAP_EVENT_SIGNAL_EN)
#define SD_BUS_PWR_INTR_MASK         (CUR_LMT_ERR_SIGNAL_EN)
#define SD_CARD_INTR_CHANGE          (CARD_INSERTION_SIGNAL_EN | CARD_REMOVAL_SIGNAL_EN)
#define SD_ERROR_MASK                (0xFFFF8000)

#define AUTO_CMD_STAT_R              0x3c
#define HOST_CTRL2_R                 0x3e
#define USH_MODE_MASK                0x3
#define UHS_MODE_SDR12               (0)
#define UHS_MODE_SDR25               (1)
#define UHS_MODE_SDR50               (2)
#define UHS_MODE_SDR104              (3)
#define UHS_MODE_DDR50               (4)
#define UHS_MODE_UHS2                (7)
#define SIGNAL_1P8V_EN               (1 << 3)
/* eMMC speed mode */
#define UHS_MODE_EMMC_LEGACY         0 /* 0-26Mhz */
#define UHS_MODE_EMMC_HSSDR          1 /* 0-52Mhz */
#define UHS_MODE_EMMC_HS200          3 /* 0-200Mhz */
#define UHS_MODE_EMMC_HSDDR          4 /* 0-50Mhz DOUBLE SAMBLE*/
#define USH_MODE_EMMC_HS400          7 /* 0-200 Double SAMBLE ONLY 8bit data width support*/

#define CAPABILITIES1_R              0x40
#define CAPABILITIES2_R              0x44
#define CURR_CAPABILITIES1_R         0x48
#define CURR_CAPABILITIES2_R         0x4c
#define FORCE_AUTO_CMD_STAT_R        0x50
#define FORCE_ERROR_INT_STAT_R       0x52
#define ADMA_ERR_STAT_R              0x54
#define ADMA_SA_LOW_R                0x58
#define ADMA_SA_HIGH_R               0x5c
#define PRESET_INT_R                 0x60
#define PRESET_DS_R                  0x62
#define PRESET_HS_R                  0x64
#define PRESET_SD12_R                0x66
#define PRESET_SD25_R                0x68
#define PRESET_SDR50_R               0x6a
#define PRESET_SDR104_R              0x6c
#define PRESET_DDR50_R               0x6e
#define PRESET_UHS2_R                0x74
#define ADMA_ID_LOW_R                0x78
#define ADMA_ID_HIGH_R               0x7c
#define UHS2_BLOCK_SIZE_R            0x80
#define UHS2_BLOCK_COUNT_R           0x84
#define UHS2_CMD_PKT_0_3_R           0x88
#define UHS2_CMD_PKT_4_7_R           0x8c
#define UHS2_CMD_PKT_8_11_R          0x90
#define UHS2_CMD_PKT_12_15_R         0x94
#define UHS2_CMD_PKT_16_19_R         0X98
#define UHS2_XFER_MODE_R             0x9c
#define UHS2_CMD_R                   0x9e
#define UHS2_RESP_0_3_R              0xa0
#define UHS2_RESP_4_7_R              0xa4
#define UHS2_RESP_8_11_R             0xa8
#define UHS2_RESP_12_15_R            0xac
#define UHS2_RESP_16_19_R            0xb0
#define UHS2_MSG_SEL_R               0xb4
#define UHS2_MSG_R                   0xb8
#define UHS2_DEV_INTR_STATUS_R       0xbc
#define UHS2_DEV_SEL_R               0xbe
#define UHS2_DEV_INR_CODE_R          0xbf
#define UHS2_SOFT_RESET_R            0xc0
#define UHS2_TIMER_CNTRL_R           0xc2
#define UHS2_ERR_INTR_STATUS_R       0xc4
#define UHS2_ERR_INTR_STATUS_EN_R    0xc8
#define UHS2_ERR_INTR_SIGNAL_EN_R    0xcc
#define P_UHS2_SETTINGS_R            0xe0
#define P_UHS2_HOST_CAPAB            0xe2
#define P_UHS2_TEST                  0xe4
#define P_EMBEDDED_CNTRL             0xe6
#define P_VENDOR_1_SPECIFIC_AREA     0xe8
#define P_VENDOR_2_SPECIFIC_AREA     0xea
#define SLOT_INTR_STATUS_R           0xfc
#define HOST_CNTRL_VERS_R            0xfe

/* DWC_mshc_map/DWC_mshc_UHS2_setting block register */
#define UHS2_GEN_SET_R               0x00
#define UHS2_PHY_SET_R               0x04
#define USH2_LNK_TRAN_SET_1_R        0x08
#define UHS2_LNK_TRAN_SET_2_R        0x0c

/* DWC_mshc_map/DWC_mshc_UHS2_capability register */
#define UHS2_GEN_CAP_R               0x00
#define UHS2_PHY_CAP_R               0x04
#define UHS2_LNK_TRAN_CAP_1_R        0x08
#define UHS2_LNK_TRAN_CAP_2_R        0x0c

/* DWC_mshc_map/DWC_mshc_UHS2_test block egister */
#define FORCE_UHS2_ERR_INTR_STATUS_R 0x00

/* DWC_mshc_map/DWC_mshc_UHS2_ embedded control block egister */
#define EMBEDDED_CTRL_R              0x00

/* DWC_mshc_map/DWC_mshc_phy_block register */
#define DWC_MSHC_PTR_PHY_R           0x300
#define PHY_CNFG_R                   (DWC_MSHC_PTR_PHY_R + 0x00)
#define PHY_CMDPAD_CNFG_R            (DWC_MSHC_PTR_PHY_R + 0x04)
#define PHY_DATAPAD_CNFG_R           (DWC_MSHC_PTR_PHY_R + 0x06)
#define PHY_CLKPAD_CNFG_R            (DWC_MSHC_PTR_PHY_R + 0x08)
#define PHY_STBPAD_CNFG_R            (DWC_MSHC_PTR_PHY_R + 0x0a)
#define PHY_RSTNPAD_CNFG_R           (DWC_MSHC_PTR_PHY_R + 0x0c)
#define PHY_PADTEST_CNFG_R           (DWC_MSHC_PTR_PHY_R + 0x0e)
#define PHY_PADTEST_OUT_R            (DWC_MSHC_PTR_PHY_R + 0x10)
#define PHY_PADTEST_IN_R             (DWC_MSHC_PTR_PHY_R + 0x12)
#define PHY_PRBS_CNFG_R              (DWC_MSHC_PTR_PHY_R + 0x18)
#define PHY_PHYLBK_CNFG_R            (DWC_MSHC_PTR_PHY_R + 0x1a)
#define PHY_COMMDL_CNFG_R            (DWC_MSHC_PTR_PHY_R + 0x1c)
#define PHY_SDCLKDL_CNFG_R           (DWC_MSHC_PTR_PHY_R + 0x1d)
#define PHY_SDCLKDL_DC_R             (DWC_MSHC_PTR_PHY_R + 0x1e)
#define PHY_SMPLDL_CNFG_R            (DWC_MSHC_PTR_PHY_R + 0x20)
#define PHY_ATDL_CNFG_R              (DWC_MSHC_PTR_PHY_R + 0x21)
#define PHY_DLL_CTRL_R               (DWC_MSHC_PTR_PHY_R + 0x24)
#define PHY_DLL_CNFG1_R              (DWC_MSHC_PTR_PHY_R + 0x25)
#define PHY_DLLDL_CNFG_R             (DWC_MSHC_PTR_PHY_R + 0x28)
#define PHY_DLL_OFFST_R              (DWC_MSHC_PTR_PHY_R + 0x29)
#define PHY_DLLMST_TSTDC_R           (DWC_MSHC_PTR_PHY_R + 0x2a)
#define PHY_DLLBT_CNFG_R             (DWC_MSHC_PTR_PHY_R + 0x2c)
#define PHY_DLL_STATUS_R             (DWC_MSHC_PTR_PHY_R + 0x2e)
#define PHY_DLLDBG_MLKDC_R           (DWC_MSHC_PTR_PHY_R + 0x30)
#define PHY_DLLDBG_SLKDC_R           (DWC_MSHC_PTR_PHY_R + 0x32)

/* DWC_mshc_map/DWC_mshc_UHS2 mhsc vendor1_block register */
#define MHSC_CTRL_R                  0x8
#define CMD_CNFLT_CHK_STBIT          0
#define AT_CTRL_R                    0x40
#define SW_TUNE_EN_STBIT             4
#define AT_STAT_R                    0x44

/* DWC_mshc_map/DWC_mshc_UHS2 mhsc vendor2_block CQE register */
#define CQVER_R                      0x00
#define CQCAP_R                      0x04
#define CQCFG_R                      0x08
#define CQCTL_R                      0x0c
#define CQIS_R                       0x10
#define CQISE_R                      0x14
#define CQISGE_R                     0x18
#define CQIC_R                       0x1c
#define CQTDLBA_R                    0x20
#define CQTDLBAU_R                   0x24
#define CQTDBR_R                     0x28
#define CQTCN_R                      0x2c
#define CQDQS_R                      0x30
#define CQDPT_R                      0x34
#define CQTCLR_R                     0x38
#define CQSSC1_R                     0x40
#define CQSSC2_R                     0x44
#define CQCRDCT_R                    0x48
#define CQRMEM_R                     0x50
#define CQTERRI_R                    0x54
#define CQCRI_R                      0x58
#define CQCRA_R                      0x5c

/* DWC_mshc_map/DWC_mshc_UHS2 mhsc vendor1_block CQE register *
 * P_VENDOR_1_SPECIFIC_AREA
 */
#define SNPS_MSHC_VER_ID_R           0x00
#define SNPS_MSHC_VER_TYPE_R         0x04
#define SNPS_MSHC_CTRL_R             0x08
#define SNPS_MBIU_CTRL_R             0x10
#define SNPS_EMMC_CTRL_R             0x2c
#define SNPS_BOOT_CTRL_R             0x2e
#define SNPS_GP_IN_R                 0x30
#define SNPS_GP_OUT_R                0x34
#define SNPS_AT_CTRL_R               0x40
#define AUTO_TUNE_EN                 0
#define CI_SEL                       1
#define SWIN_TH_EN_BITP              2
#define SWIN_TH_EN_BITW              1
#define RPT_TUNE_ERR                 3
#define SW_TUNE_EN                   4
#define WIN_EDGE_SEL                 8
#define SNPS_AT_STAT_R               0x44

#define OFF                          0
#define ON                           1
#define DISABLE                      0
#define ENABLE                       1

#define FREQSEL_CLK_400KHZ           0x7d
#define FREQSEL_SDR12                0x02 /* 25Mhz */
#define FREQSEL_SDR25                0x01 /* 50Mhz */
#define FREQSEL_SDR50                0x00 /* 100Mhz */
#define FREQSEL_SDR104               0x00
#define FREQSEL_DDR50                0x01

/* MMCM module reset through mshc gpio controls */
#define MMCM_RESET_REG_OFFS          0x534
#define MMCM_RESET_ON                (1 << 0)


#define EMMC_BASE_REG  0x50450000
#define HSP_IO_MEM     0x50440000;
#define SDHCI_RESET_TIMEOUT 0xF

#define syscrg_csr_base 0x51828000
#define syscrg_csr ((SCU_TypeDef*)syscrg_csr_base)

typedef struct
{
    volatile unsigned int REV_PLL_CFG[64];
    volatile unsigned int noc_clk_ctrl;            /* 0x100 */
    volatile unsigned int bootspi_clk_ctrl;        /* 0x104 */
    volatile unsigned int bootspi_cfgclk_ctrl;     /* 0x108 */
    volatile unsigned int scpu_coreclk_ctrl;       /* 0x10c */
    volatile unsigned int scpu_busclk_ctrl;        /* 0x110 */
    volatile unsigned int lpcpu_coreclk_ctrl;      /* 0x114 */
    volatile unsigned int lpcpu_busclk_ctrl;       /* 0x118 */
    volatile unsigned int tcu_aclk_ctrl;           /* 0x11C */
    volatile unsigned int tcu_cfg_ctrl;            /* 0x120 */
    volatile unsigned int ddr0_clk_ctrl;           /* 0x124 */
    volatile unsigned int ddr1_clk_ctrl;           /* 0x128 */
    volatile unsigned int gpu_aclk_ctrl;           /* 0x12c */
    volatile unsigned int gpu_cfg_ctrl;            /* 0x130 */
    volatile unsigned int gpu_gray_ctrl;           /* 0x134 */
    volatile unsigned int dsp_aclk_ctrl;           /* 0x138 */
    volatile unsigned int dsp_cfg_ctrl;            /* 0x13c */
    volatile unsigned int d2d_aclk_ctrl;           /* 0x140 */
    volatile unsigned int d2d_cfg_ctrl;            /* 0x144 */
    volatile unsigned int hsp_aclk_ctrl;           /* 0x148 */
    volatile unsigned int hsp_cfg_ctrl;            /* 0x14c */
    volatile unsigned int sata_rbc_ctrl;           /* 0x150 */
    volatile unsigned int sata_oob_ctrl;           /* 0x154 */
    volatile unsigned int eth0_ctrl;               /* 0x158 */
    volatile unsigned int eth1_ctrl;               /* 0x15c */
    volatile unsigned int mshc0_coreclk_ctrl;      /* 0x160 */
    volatile unsigned int mshc1_coreclk_ctrl;      /* 0x164 */
    volatile unsigned int mshc2_coreclk_ctrl;      /* 0x168 */
    volatile unsigned int mshc_usb_slwclk;         /* 0x16c */
    volatile unsigned int pcie_aclk_ctrl;          /* 0x170 */
    volatile unsigned int pcie_cfg_ctrl;           /* 0x174 */
    volatile unsigned int npu_aclk_ctrl;           /* 0x178 */
    volatile unsigned int npu_llc_ctrl;            /* 0x17c */
    volatile unsigned int npu_core_ctrl;           /* 0x180 */
    volatile unsigned int vi_dwclk_ctrl;           /* 0x184 */
    volatile unsigned int vi_aclk_ctrl;            /* 0x188 */
    volatile unsigned int vi_dig_ctrl;             /* 0x18c */
    volatile unsigned int vi_dvp_ctrl;             /* 0x190 */
    volatile unsigned int vi_shutter0;             /* 0x194 */
    volatile unsigned int vi_shutter1;             /* 0x198 */
    volatile unsigned int vi_shutter2;             /* 0x19C */
    volatile unsigned int vi_shutter3;             /* 0x1a0 */
    volatile unsigned int vi_shutter4;             /* 0x1a4 */
    volatile unsigned int vi_shutter5;             /* 0x1a8 */
    volatile unsigned int vi_phy_clkctrl;          /* 0x1ac */
    volatile unsigned int vo_aclk_ctrl;            /* 0x1b0 */
    volatile unsigned int vo_iesmclk_ctrl;         /* 0x1b4 */
    volatile unsigned int vo_pixel_ctrl;           /* 0x1b8 */
    volatile unsigned int vo_mclk_ctrl;            /* 0x1bc */
    volatile unsigned int vo_phy_clkctrl;          /* 0x1c0 */
    volatile unsigned int vc_aclk_ctrl;            /* 0x1c4 */
    volatile unsigned int vc_cfg_ctrl;             /* 0x1c8 */
    volatile unsigned int g2d_ctrl0;               /* 0x1cc */
    volatile unsigned int vc_clken_ctrl;           /* 0x1d0 */
    volatile unsigned int je_clk_ctrl;             /* 0x1d4 */
    volatile unsigned int jd_clk_ctrl;             /* 0x1d8 */
    volatile unsigned int vd_clk_ctrl;             /* 0x1dc */
    volatile unsigned int ve_clk_ctrl;             /* 0x1e0 */
    volatile unsigned int aon_dma_clk_ctrl;        /* 0x1e4 */
    volatile unsigned int timer_clk_ctrl;          /* 0x1e8 */
    volatile unsigned int rtc_clk_ctrl;            /* 0x1ec */
    volatile unsigned int pka_clk_ctrl;            /* 0x1f0 */
    volatile unsigned int spacc_clk_ctrl;          /* 0x1f4 */
    volatile unsigned int trng_clk_ctrl;           /* 0x1f8 */
    volatile unsigned int otp_clk_ctrl;            /* 0x1fc */
    volatile unsigned int lsp_clk_en0;             /* 0x200 */
    volatile unsigned int lsp_clk_en1;             /* 0x204 */
    volatile unsigned int u84_clk_ctrl;            /* 0x208 */
    volatile unsigned int syscfg_clk_ctrl;         /* 0x20c */
    volatile unsigned int i2c0_clk_ctrl;           /* 0x210 */
    volatile unsigned int i2c1_clk_ctrl;           /* 0x214 */
    volatile unsigned int REV_CLK_CFG[58];
    volatile unsigned int sys_swrst_value;         /* 0x300 */
    volatile unsigned int clr_rst_status;          /* 0x304 */
    volatile unsigned int die_status;              /* 0x308 */
    volatile unsigned int clr_boot_info;           /* 0x30C */
    volatile unsigned int scpu_boot_address;       /* 0x310 */
    volatile unsigned int lpcpu_boot_address;      /* 0x314 */
    volatile unsigned int npue31_boot_address;     /* 0x318 */
    volatile unsigned int u84_boot_address0_hi;    /* 0x31C */
    volatile unsigned int u84_boot_address0_low;   /* 0x320 */
    volatile unsigned int u84_boot_address1_hi;    /* 0x324 */
    volatile unsigned int u84_boot_address1_low;   /* 0x328 */
    volatile unsigned int u84_boot_address2_hi;    /* 0x32C */
    volatile unsigned int u84_boot_address2_low;   /* 0x330 */
    volatile unsigned int u84_boot_address3_hi;    /* 0x334 */
    volatile unsigned int u84_boot_address3_low;   /* 0x338 */
    volatile unsigned int boot_sel_stat;           /* 0x33C */
    volatile unsigned int boot_spi_cfg;            /* 0x340 */
    volatile unsigned int REV_RST_CFG[47];
    volatile unsigned int noc_rst_ctrl;            /* 0x400 */
    volatile unsigned int gpu_rst_ctrl;            /* 0x404 */
    volatile unsigned int dsp_rst_ctrl;            /* 0x408 */
    volatile unsigned int d2d_rst_ctrl;            /* 0x40c */
    volatile unsigned int ddr_rst_ctrl;            /* 0x410 */
    volatile unsigned int tcu_rst_ctrl;            /* 0x414 */
    volatile unsigned int npu_rst_ctrl;            /* 0x418 */
    volatile unsigned int hspdma_rst_ctrl;         /* 0x41c */
    volatile unsigned int pcie_rst_ctrl;           /* 0x420 */
    volatile unsigned int i2c_rst_ctrl;            /* 0x424 */
    volatile unsigned int fan_rst_ctrl;            /* 0x428 */
    volatile unsigned int pvt_rst_ctrl;            /* 0x42C */
    volatile unsigned int mbox_rst_ctrl;           /* 0x430 */
    volatile unsigned int uart_rst_ctrl;           /* 0x434 */
    volatile unsigned int gpio_rst_ctrl;           /* 0x438 */
    volatile unsigned int timer_rst_ctrl;          /* 0x43C */
    volatile unsigned int ssi_rst_ctrl;            /* 0x440 */
    volatile unsigned int wdt_rst_ctrl;            /* 0x444 */
    volatile unsigned int lsp_cfgrst_ctrl;         /* 0x448 */
    volatile unsigned int u84_rst_ctrl;            /* 0x44c */
    volatile unsigned int scpu_rst_ctrl;           /* 0x450 */
    volatile unsigned int lpcpu_rst_ctrl;          /* 0x454 */
    volatile unsigned int vc_rst_ctrl;             /* 0x458 */
    volatile unsigned int jd_rst_ctrl;             /* 0x45C */
    volatile unsigned int je_rst_ctrl;             /* 0x460 */
    volatile unsigned int vd_rst_ctrl;             /* 0x464 */
    volatile unsigned int ve_rst_ctrl;             /* 0x468 */
    volatile unsigned int g2d_rst_ctrl;            /* 0x46C */
    volatile unsigned int vi_rst_ctrl;             /* 0x470 */
    volatile unsigned int dvp_rst_ctrl;            /* 0x474 */
    volatile unsigned int isp0_rst_ctrl;           /* 0x478 */
    volatile unsigned int isp1_rst_ctrl;           /* 0x47C */
    volatile unsigned int shutter_rst_ctrl;        /* 0x480 */
    volatile unsigned int vo_phyrst_ctrl;          /* 0x484 */
    volatile unsigned int vo_i2srst_ctrl;          /* 0x488 */
    volatile unsigned int vo_rst_ctrl;             /* 0x48C */
    volatile unsigned int bootspi_rst_ctrl;        /* 0x490 */
    volatile unsigned int i2c1_rst_ctrl;           /* 0x494 */
    volatile unsigned int i2c0_rst_ctrl;           /* 0x498 */
    volatile unsigned int dma1_rst_ctrl;           /* 0x49C */
    volatile unsigned int fprt_rst_ctrl;           /* 0x4A0 */
    volatile unsigned int hblock_rst_ctrl;         /* 0x4A4 */
    volatile unsigned int secfg_rst_ctrl;          /* 0x4A8 */
    volatile unsigned int otp_rst_ctrl;            /* 0x4AC */
    volatile unsigned int pka_rst_ctrl;            /* 0x4B0 */
    volatile unsigned int spacc_rst_ctrl;          /* 0x4B4 */
    volatile unsigned int trng_rst_ctrl;           /* 0x4B8 */
    volatile unsigned int timer0_rst_ctrl;         /* 0x4C0 */
    volatile unsigned int timer1_rst_ctrl;         /* 0x4C4 */
    volatile unsigned int timer2_rst_ctrl;         /* 0x4C8 */
    volatile unsigned int timer3_rst_ctrl;         /* 0x4cc */
    volatile unsigned int rtc_rst_ctrl;            /* 0x4D0 */
} SCU_TypeDef;

#endif