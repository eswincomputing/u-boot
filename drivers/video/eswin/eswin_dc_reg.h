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

#ifndef _ESWIN_DC_REG_H
#define _ESWIN_DC_REG_H

#include "eswin_dc.h"
#include <drm/eswin_dc_type.h>
/*******************************************************************************
**                            ~~~~~~~~~~~~~~~~~~~~                            **
**                            Module HostInterface                            **
**                            ~~~~~~~~~~~~~~~~~~~~                            **
*******************************************************************************/

/* Register AQHiClockControl **
** ~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Clock control register. */

#define AQHiClockControlRegAddrs                                          0x0000
#define AQ_HI_CLOCK_CONTROL_Address                                      0x00000
#define AQ_HI_CLOCK_CONTROL_MSB                                               15
#define AQ_HI_CLOCK_CONTROL_LSB                                                0
#define AQ_HI_CLOCK_CONTROL_BLK                                                0
#define AQ_HI_CLOCK_CONTROL_Count                                              1
#define AQ_HI_CLOCK_CONTROL_FieldMask                                 0x1FFF7FFF
#define AQ_HI_CLOCK_CONTROL_ReadMask                                  0x1FFF7FFF
#define AQ_HI_CLOCK_CONTROL_WriteMask                                 0x1FF87FFF
#define AQ_HI_CLOCK_CONTROL_ResetValue                                0x00000900

/* Disable 3D clock. */
#define AQ_HI_CLOCK_CONTROL_CLK3D_DIS                                        0:0
#define AQ_HI_CLOCK_CONTROL_CLK3D_DIS_End                                      0
#define AQ_HI_CLOCK_CONTROL_CLK3D_DIS_Start                                    0
#define AQ_HI_CLOCK_CONTROL_CLK3D_DIS_Type                                   U01

/* Disable 2D clock. */
#define AQ_HI_CLOCK_CONTROL_CLK2D_DIS                                        1:1
#define AQ_HI_CLOCK_CONTROL_CLK2D_DIS_End                                      1
#define AQ_HI_CLOCK_CONTROL_CLK2D_DIS_Start                                    1
#define AQ_HI_CLOCK_CONTROL_CLK2D_DIS_Type                                   U01

/* Core clock frequency scale value.(reset=1) */
#define AQ_HI_CLOCK_CONTROL_FSCALE_VAL                                       8:2
#define AQ_HI_CLOCK_CONTROL_FSCALE_VAL_End                                     8
#define AQ_HI_CLOCK_CONTROL_FSCALE_VAL_Start                                   2
#define AQ_HI_CLOCK_CONTROL_FSCALE_VAL_Type                                  U07

/* Core clock frequency scale value. */
#define AQ_HI_CLOCK_CONTROL_FSCALE_CMD_LOAD                                  9:9
#define AQ_HI_CLOCK_CONTROL_FSCALE_CMD_LOAD_End                                9
#define AQ_HI_CLOCK_CONTROL_FSCALE_CMD_LOAD_Start                              9
#define AQ_HI_CLOCK_CONTROL_FSCALE_CMD_LOAD_Type                             U01

/* Disables clock gating for rams. */
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_CLOCK_GATING                       10:10
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_CLOCK_GATING_End                      10
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_CLOCK_GATING_Start                    10
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_CLOCK_GATING_Type                    U01

/* Disable debug registers. If this bit is 1, debug  registers are clock      **
** gated.(reset=1)                                                            */
#define AQ_HI_CLOCK_CONTROL_DISABLE_DEBUG_REGISTERS                        11:11
#define AQ_HI_CLOCK_CONTROL_DISABLE_DEBUG_REGISTERS_End                       11
#define AQ_HI_CLOCK_CONTROL_DISABLE_DEBUG_REGISTERS_Start                     11
#define AQ_HI_CLOCK_CONTROL_DISABLE_DEBUG_REGISTERS_Type                     U01

/* Soft resets the IP. For cores from v620x, this field is obsolete. Use      **
** gcregHiAHBControl[0]SOFT_RESET instead.                                    */
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET                                     12:12
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET_End                                    12
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET_Start                                  12
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET_Type                                  U01

/* Disables ram power optimization. */
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION                 13:13
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_End                13
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_Start              13
#define AQ_HI_CLOCK_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_Type              U01

/* Disables core speed up during AHB access. */
#define AQ_HI_CLOCK_CONTROL_DISABLE_CORE_SPEED_UP                          14:14
#define AQ_HI_CLOCK_CONTROL_DISABLE_CORE_SPEED_UP_End                         14
#define AQ_HI_CLOCK_CONTROL_DISABLE_CORE_SPEED_UP_Start                       14
#define AQ_HI_CLOCK_CONTROL_DISABLE_CORE_SPEED_UP_Type                       U01

/* 3D pipe is idle. */
#define AQ_HI_CLOCK_CONTROL_IDLE3_D                                        16:16
#define AQ_HI_CLOCK_CONTROL_IDLE3_D_End                                       16
#define AQ_HI_CLOCK_CONTROL_IDLE3_D_Start                                     16
#define AQ_HI_CLOCK_CONTROL_IDLE3_D_Type                                     U01

/* 2D pipe is idle. */
#define AQ_HI_CLOCK_CONTROL_IDLE2_D                                        17:17
#define AQ_HI_CLOCK_CONTROL_IDLE2_D_End                                       17
#define AQ_HI_CLOCK_CONTROL_IDLE2_D_Start                                     17
#define AQ_HI_CLOCK_CONTROL_IDLE2_D_Type                                     U01

/* VG pipe is idle. */
#define AQ_HI_CLOCK_CONTROL_IDLE_VG                                        18:18
#define AQ_HI_CLOCK_CONTROL_IDLE_VG_End                                       18
#define AQ_HI_CLOCK_CONTROL_IDLE_VG_Start                                     18
#define AQ_HI_CLOCK_CONTROL_IDLE_VG_Type                                     U01

/* Isolate GPU bit */
#define AQ_HI_CLOCK_CONTROL_ISOLATE_GPU                                    19:19
#define AQ_HI_CLOCK_CONTROL_ISOLATE_GPU_End                                   19
#define AQ_HI_CLOCK_CONTROL_ISOLATE_GPU_Start                                 19
#define AQ_HI_CLOCK_CONTROL_ISOLATE_GPU_Type                                 U01

/* Determines which HI/MC to use while reading registers.  */
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_REG_SELECT                          23:20
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_REG_SELECT_End                         23
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_REG_SELECT_Start                       20
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_REG_SELECT_Type                       U04

/* Force all the transactions to go to one AXI.  */
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_USE_SINGLE_AXI                      27:24
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_USE_SINGLE_AXI_End                     27
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_USE_SINGLE_AXI_Start                   24
#define AQ_HI_CLOCK_CONTROL_MULTI_PIPE_USE_SINGLE_AXI_Type                   U04

/* Soft resets the L2. */
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET_L2                                  28:28
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET_L2_End                                 28
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET_L2_Start                               28
#define AQ_HI_CLOCK_CONTROL_SOFT_RESET_L2_Type                               U01

/* Register AQHiIdle **
** ~~~~~~~~~~~~~~~~~ */

/* Idle status register. */

#define AQHiIdleRegAddrs                                                  0x0001
#define AQ_HI_IDLE_Address                                               0x00004
#define AQ_HI_IDLE_MSB                                                        15
#define AQ_HI_IDLE_LSB                                                         0
#define AQ_HI_IDLE_BLK                                                         0
#define AQ_HI_IDLE_Count                                                       1
#define AQ_HI_IDLE_FieldMask                                          0xFFFFFFFF
#define AQ_HI_IDLE_ReadMask                                           0xFFFFFFFF
#define AQ_HI_IDLE_WriteMask                                          0x00000000
#define AQ_HI_IDLE_ResetValue                                         0x7FFFFFFF

/* FE is idle. */
#define AQ_HI_IDLE_IDLE_FE                                                   0:0
#define AQ_HI_IDLE_IDLE_FE_End                                                 0
#define AQ_HI_IDLE_IDLE_FE_Start                                               0
#define AQ_HI_IDLE_IDLE_FE_Type                                              U01

/* DE is idle. */
#define AQ_HI_IDLE_IDLE_DE                                                   1:1
#define AQ_HI_IDLE_IDLE_DE_End                                                 1
#define AQ_HI_IDLE_IDLE_DE_Start                                               1
#define AQ_HI_IDLE_IDLE_DE_Type                                              U01

/* PE is idle. */
#define AQ_HI_IDLE_IDLE_PE                                                   2:2
#define AQ_HI_IDLE_IDLE_PE_End                                                 2
#define AQ_HI_IDLE_IDLE_PE_Start                                               2
#define AQ_HI_IDLE_IDLE_PE_Type                                              U01

/* SH is idle. */
#define AQ_HI_IDLE_IDLE_SH                                                   3:3
#define AQ_HI_IDLE_IDLE_SH_End                                                 3
#define AQ_HI_IDLE_IDLE_SH_Start                                               3
#define AQ_HI_IDLE_IDLE_SH_Type                                              U01

/* PA is idle. */
#define AQ_HI_IDLE_IDLE_PA                                                   4:4
#define AQ_HI_IDLE_IDLE_PA_End                                                 4
#define AQ_HI_IDLE_IDLE_PA_Start                                               4
#define AQ_HI_IDLE_IDLE_PA_Type                                              U01

/* SE is idle. */
#define AQ_HI_IDLE_IDLE_SE                                                   5:5
#define AQ_HI_IDLE_IDLE_SE_End                                                 5
#define AQ_HI_IDLE_IDLE_SE_Start                                               5
#define AQ_HI_IDLE_IDLE_SE_Type                                              U01

/* RA is idle. */
#define AQ_HI_IDLE_IDLE_RA                                                   6:6
#define AQ_HI_IDLE_IDLE_RA_End                                                 6
#define AQ_HI_IDLE_IDLE_RA_Start                                               6
#define AQ_HI_IDLE_IDLE_RA_Type                                              U01

/* TX is idle. */
#define AQ_HI_IDLE_IDLE_TX                                                   7:7
#define AQ_HI_IDLE_IDLE_TX_End                                                 7
#define AQ_HI_IDLE_IDLE_TX_Start                                               7
#define AQ_HI_IDLE_IDLE_TX_Type                                              U01

/* VG is idle. */
#define AQ_HI_IDLE_IDLE_VG                                                   8:8
#define AQ_HI_IDLE_IDLE_VG_End                                                 8
#define AQ_HI_IDLE_IDLE_VG_Start                                               8
#define AQ_HI_IDLE_IDLE_VG_Type                                              U01

/* IM is idle. */
#define AQ_HI_IDLE_IDLE_IM                                                   9:9
#define AQ_HI_IDLE_IDLE_IM_End                                                 9
#define AQ_HI_IDLE_IDLE_IM_Start                                               9
#define AQ_HI_IDLE_IDLE_IM_Type                                              U01

/* FP is idle. */
#define AQ_HI_IDLE_IDLE_FP                                                 10:10
#define AQ_HI_IDLE_IDLE_FP_End                                                10
#define AQ_HI_IDLE_IDLE_FP_Start                                              10
#define AQ_HI_IDLE_IDLE_FP_Type                                              U01

/* TS is idle. */
#define AQ_HI_IDLE_IDLE_TS                                                 11:11
#define AQ_HI_IDLE_IDLE_TS_End                                                11
#define AQ_HI_IDLE_IDLE_TS_Start                                              11
#define AQ_HI_IDLE_IDLE_TS_Type                                              U01

/* BLT is idle. */
#define AQ_HI_IDLE_IDLE_BLT                                                12:12
#define AQ_HI_IDLE_IDLE_BLT_End                                               12
#define AQ_HI_IDLE_IDLE_BLT_Start                                             12
#define AQ_HI_IDLE_IDLE_BLT_Type                                             U01

/* FE BLT Parser is idle. */
#define AQ_HI_IDLE_IDLE_FE_BLT                                             13:13
#define AQ_HI_IDLE_IDLE_FE_BLT_End                                            13
#define AQ_HI_IDLE_IDLE_FE_BLT_Start                                          13
#define AQ_HI_IDLE_IDLE_FE_BLT_Type                                          U01

/* MC is idle. */
#define AQ_HI_IDLE_IDLE_MC                                                 14:14
#define AQ_HI_IDLE_IDLE_MC_End                                                14
#define AQ_HI_IDLE_IDLE_MC_Start                                              14
#define AQ_HI_IDLE_IDLE_MC_Type                                              U01

/* PPA is idle. */
#define AQ_HI_IDLE_IDLE_PPA                                                15:15
#define AQ_HI_IDLE_IDLE_PPA_End                                               15
#define AQ_HI_IDLE_IDLE_PPA_Start                                             15
#define AQ_HI_IDLE_IDLE_PPA_Type                                             U01

/* DC is idle. */
#define AQ_HI_IDLE_IDLE_DC                                                 16:16
#define AQ_HI_IDLE_IDLE_DC_End                                                16
#define AQ_HI_IDLE_IDLE_DC_Start                                              16
#define AQ_HI_IDLE_IDLE_DC_Type                                              U01

/* WD is idle. */
#define AQ_HI_IDLE_IDLE_WD                                                 17:17
#define AQ_HI_IDLE_IDLE_WD_End                                                17
#define AQ_HI_IDLE_IDLE_WD_Start                                              17
#define AQ_HI_IDLE_IDLE_WD_Type                                              U01

/* NN is idle. */
#define AQ_HI_IDLE_IDLE_NN                                                 18:18
#define AQ_HI_IDLE_IDLE_NN_End                                                18
#define AQ_HI_IDLE_IDLE_NN_Start                                              18
#define AQ_HI_IDLE_IDLE_NN_Type                                              U01

/* TP is idle. */
#define AQ_HI_IDLE_IDLE_TP                                                 19:19
#define AQ_HI_IDLE_IDLE_TP_End                                                19
#define AQ_HI_IDLE_IDLE_TP_Start                                              19
#define AQ_HI_IDLE_IDLE_TP_Type                                              U01

/* Vip Scaler is idle. */
#define AQ_HI_IDLE_IDLE_VSC                                                20:20
#define AQ_HI_IDLE_IDLE_VSC_End                                               20
#define AQ_HI_IDLE_IDLE_VSC_Start                                             20
#define AQ_HI_IDLE_IDLE_VSC_Type                                             U01

/* MMU is idle. */
#define AQ_HI_IDLE_IDLE_MMU                                                21:21
#define AQ_HI_IDLE_IDLE_MMU_End                                               21
#define AQ_HI_IDLE_IDLE_MMU_Start                                             21
#define AQ_HI_IDLE_IDLE_MMU_Type                                             U01

/* Unused bits reserved for future expansion. */
#define AQ_HI_IDLE_UNUSED                                                  30:22
#define AQ_HI_IDLE_UNUSED_End                                                 30
#define AQ_HI_IDLE_UNUSED_Start                                               22
#define AQ_HI_IDLE_UNUSED_Type                                               U09

/* AXI is in low power mode. */
#define AQ_HI_IDLE_AXI_LP                                                  31:31
#define AQ_HI_IDLE_AXI_LP_End                                                 31
#define AQ_HI_IDLE_AXI_LP_Start                                               31
#define AQ_HI_IDLE_AXI_LP_Type                                               U01

/* Register AQAxiConfig **
** ~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Axi configuration register. */

#define AQAxiConfigRegAddrs                                          	  0x0002
#define AQ_AXI_CONFIG_Address                                            0x00008
#define AQ_AXI_CONFIG_ResetValue                                      0x00222200


/* Register AQIntrEnbl **
** ~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Interrupt enable register. */

#define AQIntrEnblRegAddrs                                                0x0005
#define AQ_INTR_ENBL_Address                                             0x00014
#define AQ_INTR_ENBL_ResetValue                                       0x00000000


/* Register GCChipId **
** ~~~~~~~~~~~~~~~~~ */

/* Shows the ID for the chip in BCD.  This register has no set reset value.   **
** It varies with the implementation.READ_ONLY.                               */

#define GCChipIdRegAddrs                                                  0x0008
#define GC_CHIP_ID_Address                                               0x00020
#define GC_CHIP_ID_MSB                                                        15
#define GC_CHIP_ID_LSB                                                         0
#define GC_CHIP_ID_BLK                                                         0
#define GC_CHIP_ID_Count                                                       1
#define GC_CHIP_ID_FieldMask                                          0xFFFFFFFF
#define GC_CHIP_ID_ReadMask                                           0xFFFFFFFF
#define GC_CHIP_ID_WriteMask                                          0x00000000
#define GC_CHIP_ID_ResetValue                                         0x00000000

/* Id. */
#define GC_CHIP_ID_ID                                                       31:0
#define GC_CHIP_ID_ID_End                                                     31
#define GC_CHIP_ID_ID_Start                                                    0
#define GC_CHIP_ID_ID_Type                                                   U32

/* Register GCChipDate **
** ~~~~~~~~~~~~~~~~~~~ */

/* Shows the release date for the IP.  This register has no set reset value.  **
** It varies with the implementation.                                         */

#define GCChipDateRegAddrs                                                0x000A
#define GC_CHIP_DATE_Address                                             0x00028
#define GC_CHIP_DATE_MSB                                                      15
#define GC_CHIP_DATE_LSB                                                       0
#define GC_CHIP_DATE_BLK                                                       0
#define GC_CHIP_DATE_Count                                                     1
#define GC_CHIP_DATE_FieldMask                                        0xFFFFFFFF
#define GC_CHIP_DATE_ReadMask                                         0xFFFFFFFF
#define GC_CHIP_DATE_WriteMask                                        0x00000000
#define GC_CHIP_DATE_ResetValue                                       0x00000000

/* Date. */
#define GC_CHIP_DATE_DATE                                                   31:0
#define GC_CHIP_DATE_DATE_End                                                 31
#define GC_CHIP_DATE_DATE_Start                                                0
#define GC_CHIP_DATE_DATE_Type                                               U32

/* Register GCChipTime **
** ~~~~~~~~~~~~~~~~~~~ */

/* Shows the release time for the IP.  This register has no set reset value.  **
** It varies with the implementation.                                         */

#define GCChipTimeRegAddrs                                                0x000B
#define GC_CHIP_TIME_Address                                             0x0002C
#define GC_CHIP_TIME_MSB                                                      15
#define GC_CHIP_TIME_LSB                                                       0
#define GC_CHIP_TIME_BLK                                                       0
#define GC_CHIP_TIME_Count                                                     1
#define GC_CHIP_TIME_FieldMask                                        0xFFFFFFFF
#define GC_CHIP_TIME_ReadMask                                         0xFFFFFFFF
#define GC_CHIP_TIME_WriteMask                                        0x00000000
#define GC_CHIP_TIME_ResetValue                                       0x00000000

/* Time */
#define GC_CHIP_TIME_TIME                                                   31:0
#define GC_CHIP_TIME_TIME_End                                                 31
#define GC_CHIP_TIME_TIME_Start                                                0
#define GC_CHIP_TIME_TIME_Type                                               U32

/* Register GCChipCustomer **
** ~~~~~~~~~~~~~~~~~~~~~~~ */

/* Shows the customer and group for the IP.  This register has no set reset   **
** value.  It varies with the implementation.                                 */

#define GCChipCustomerRegAddrs                                            0x000C
#define GC_CHIP_CUSTOMER_Address                                         0x00030
#define GC_CHIP_CUSTOMER_MSB                                                  15
#define GC_CHIP_CUSTOMER_LSB                                                   0
#define GC_CHIP_CUSTOMER_BLK                                                   0
#define GC_CHIP_CUSTOMER_Count                                                 1
#define GC_CHIP_CUSTOMER_FieldMask                                    0xFFFFFFFF
#define GC_CHIP_CUSTOMER_ReadMask                                     0xFFFFFFFF
#define GC_CHIP_CUSTOMER_WriteMask                                    0x00000000
#define GC_CHIP_CUSTOMER_ResetValue                                   0x00000000

/* Company. */
#define GC_CHIP_CUSTOMER_COMPANY                                           31:16
#define GC_CHIP_CUSTOMER_COMPANY_End                                          31
#define GC_CHIP_CUSTOMER_COMPANY_Start                                        16
#define GC_CHIP_CUSTOMER_COMPANY_Type                                        U16

/* Group. */
#define GC_CHIP_CUSTOMER_GROUP                                              15:0
#define GC_CHIP_CUSTOMER_GROUP_End                                            15
#define GC_CHIP_CUSTOMER_GROUP_Start                                           0
#define GC_CHIP_CUSTOMER_GROUP_Type                                          U16

/* Register GCChipRev **
** ~~~~~~~~~~~~~~~~~~ */

/* Shows the revision for the chip in BCD.  This register has no set reset    **
** value.  It varies with the implementation.                                 */

#define GCChipRevRegAddrs                                                 0x0009
#define GC_CHIP_REV_Address                                              0x00024
#define GC_CHIP_REV_MSB                                                       15
#define GC_CHIP_REV_LSB                                                        0
#define GC_CHIP_REV_BLK                                                        0
#define GC_CHIP_REV_Count                                                      1
#define GC_CHIP_REV_FieldMask                                         0xFFFFFFFF
#define GC_CHIP_REV_ReadMask                                          0xFFFFFFFF
#define GC_CHIP_REV_WriteMask                                         0x00000000
#define GC_CHIP_REV_ResetValue                                        0x00000000

/* Revision. */
#define GC_CHIP_REV_REV                                                     31:0
#define GC_CHIP_REV_REV_End                                                   31
#define GC_CHIP_REV_REV_Start                                                  0
#define GC_CHIP_REV_REV_Type                                                 U32

/* Register gcregHIChipPatchRev **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Patch revision level for the chip */

#define gcregHIChipPatchRevRegAddrs                                       0x0026
#define GCREG_HI_CHIP_PATCH_REV_Address                                  0x00098
#define GCREG_HI_CHIP_PATCH_REV_MSB                                           15
#define GCREG_HI_CHIP_PATCH_REV_LSB                                            0
#define GCREG_HI_CHIP_PATCH_REV_BLK                                            0
#define GCREG_HI_CHIP_PATCH_REV_Count                                          1
#define GCREG_HI_CHIP_PATCH_REV_FieldMask                             0x000000FF
#define GCREG_HI_CHIP_PATCH_REV_ReadMask                              0x000000FF
#define GCREG_HI_CHIP_PATCH_REV_WriteMask                             0x00000000
#define GCREG_HI_CHIP_PATCH_REV_ResetValue                            0x00000000

#define GCREG_HI_CHIP_PATCH_REV_PATCH_REV                                    7:0
#define GCREG_HI_CHIP_PATCH_REV_PATCH_REV_End                                  7
#define GCREG_HI_CHIP_PATCH_REV_PATCH_REV_Start                                0
#define GCREG_HI_CHIP_PATCH_REV_PATCH_REV_Type                               U08

/* Register gcChipInfo **
** ~~~~~~~~~~~~~~~~~~~ */

/* Shows chip info */

#define gcChipInfoRegAddrs                                                0x0029
#define GC_CHIP_INFO_Address                                             0x000A4
#define GC_CHIP_INFO_MSB                                                      15
#define GC_CHIP_INFO_LSB                                                       0
#define GC_CHIP_INFO_BLK                                                       0
#define GC_CHIP_INFO_Count                                                     1
#define GC_CHIP_INFO_FieldMask                                        0x7F33333F
#define GC_CHIP_INFO_ReadMask                                         0x7F33333F
#define GC_CHIP_INFO_WriteMask                                        0x00000000
#define GC_CHIP_INFO_ResetValue                                       0x00000000

/* 1 : indicating that 2D Engine is included */
#define GC_CHIP_INFO_ENGINE2_D                                               0:0
#define GC_CHIP_INFO_ENGINE2_D_End                                             0
#define GC_CHIP_INFO_ENGINE2_D_Start                                           0
#define GC_CHIP_INFO_ENGINE2_D_Type                                          U01
#define   GC_CHIP_INFO_ENGINE2_D_NONE                                        0x0
#define   GC_CHIP_INFO_ENGINE2_D_AVAILABLE                                   0x1

/* 1 : indicating that 3D Engine is included */
#define GC_CHIP_INFO_ENGINE3_D                                               1:1
#define GC_CHIP_INFO_ENGINE3_D_End                                             1
#define GC_CHIP_INFO_ENGINE3_D_Start                                           1
#define GC_CHIP_INFO_ENGINE3_D_Type                                          U01
#define   GC_CHIP_INFO_ENGINE3_D_NONE                                        0x0
#define   GC_CHIP_INFO_ENGINE3_D_AVAILABLE                                   0x1

/* 1 : indicating that  VG Engine is included */
#define GC_CHIP_INFO_ENGINE_VG                                               2:2
#define GC_CHIP_INFO_ENGINE_VG_End                                             2
#define GC_CHIP_INFO_ENGINE_VG_Start                                           2
#define GC_CHIP_INFO_ENGINE_VG_Type                                          U01
#define   GC_CHIP_INFO_ENGINE_VG_NONE                                        0x0
#define   GC_CHIP_INFO_ENGINE_VG_AVAILABLE                                   0x1

/* 1 :  indicating that DEC Decoder/Encoder Engine is included */
#define GC_CHIP_INFO_ENGINE_DEC                                              3:3
#define GC_CHIP_INFO_ENGINE_DEC_End                                            3
#define GC_CHIP_INFO_ENGINE_DEC_Start                                          3
#define GC_CHIP_INFO_ENGINE_DEC_Type                                         U01
#define   GC_CHIP_INFO_ENGINE_DEC_NONE                                       0x0
#define   GC_CHIP_INFO_ENGINE_DEC_AVAILABLE                                  0x1

/* 1: indicating that Display Controller is included */
#define GC_CHIP_INFO_ENGINE_DC                                               4:4
#define GC_CHIP_INFO_ENGINE_DC_End                                             4
#define GC_CHIP_INFO_ENGINE_DC_Start                                           4
#define GC_CHIP_INFO_ENGINE_DC_Type                                          U01
#define   GC_CHIP_INFO_ENGINE_DC_NONE                                        0x0
#define   GC_CHIP_INFO_ENGINE_DC_AVAILABLE                                   0x1

#define GC_CHIP_INFO_ENGINE_SC                                               5:5
#define GC_CHIP_INFO_ENGINE_SC_End                                             5
#define GC_CHIP_INFO_ENGINE_SC_Start                                           5
#define GC_CHIP_INFO_ENGINE_SC_Type                                          U01
#define   GC_CHIP_INFO_ENGINE_SC_NONE                                        0x0
#define   GC_CHIP_INFO_ENGINE_SC_AVAILABLE                                   0x1

/* Single AXI port */
#define GC_CHIP_INFO_ONE_SET_BUS                                             8:8
#define GC_CHIP_INFO_ONE_SET_BUS_End                                           8
#define GC_CHIP_INFO_ONE_SET_BUS_Start                                         8
#define GC_CHIP_INFO_ONE_SET_BUS_Type                                        U01
#define   GC_CHIP_INFO_ONE_SET_BUS_NONE                                      0x0
#define   GC_CHIP_INFO_ONE_SET_BUS_AVAILABLE                                 0x1

/* Dual AXI ports */
#define GC_CHIP_INFO_TWO_SET_BUS                                             9:9
#define GC_CHIP_INFO_TWO_SET_BUS_End                                           9
#define GC_CHIP_INFO_TWO_SET_BUS_Start                                         9
#define GC_CHIP_INFO_TWO_SET_BUS_Type                                        U01
#define   GC_CHIP_INFO_TWO_SET_BUS_NONE                                      0x0
#define   GC_CHIP_INFO_TWO_SET_BUS_AVAILABLE                                 0x1

/* 1 : ahb interface is included */
#define GC_CHIP_INFO_INTERFACE_AHB                                         12:12
#define GC_CHIP_INFO_INTERFACE_AHB_End                                        12
#define GC_CHIP_INFO_INTERFACE_AHB_Start                                      12
#define GC_CHIP_INFO_INTERFACE_AHB_Type                                      U01
#define   GC_CHIP_INFO_INTERFACE_AHB_NONE                                    0x0
#define   GC_CHIP_INFO_INTERFACE_AHB_AVAILABLE                               0x1

/* 1 : apb interface is included */
#define GC_CHIP_INFO_INTERFACE_APB                                         13:13
#define GC_CHIP_INFO_INTERFACE_APB_End                                        13
#define GC_CHIP_INFO_INTERFACE_APB_Start                                      13
#define GC_CHIP_INFO_INTERFACE_APB_Type                                      U01
#define   GC_CHIP_INFO_INTERFACE_APB_NONE                                    0x0
#define   GC_CHIP_INFO_INTERFACE_APB_AVAILABLE                               0x1

/* 1 :  SingleGPU configuration */
#define GC_CHIP_INFO_ONE_GPU                                               16:16
#define GC_CHIP_INFO_ONE_GPU_End                                              16
#define GC_CHIP_INFO_ONE_GPU_Start                                            16
#define GC_CHIP_INFO_ONE_GPU_Type                                            U01
#define   GC_CHIP_INFO_ONE_GPU_NONE                                          0x0
#define   GC_CHIP_INFO_ONE_GPU_AVAILABLE                                     0x1

/* 1 : Dual GPU Configuration */
#define GC_CHIP_INFO_DUAL_GPU                                              17:17
#define GC_CHIP_INFO_DUAL_GPU_End                                             17
#define GC_CHIP_INFO_DUAL_GPU_Start                                           17
#define GC_CHIP_INFO_DUAL_GPU_Type                                           U01
#define   GC_CHIP_INFO_DUAL_GPU_NONE                                         0x0
#define   GC_CHIP_INFO_DUAL_GPU_AVAILABLE                                    0x1

/* 1 : axi bus width is 64-bit */
#define GC_CHIP_INFO_BUS64_BITS                                            20:20
#define GC_CHIP_INFO_BUS64_BITS_End                                           20
#define GC_CHIP_INFO_BUS64_BITS_Start                                         20
#define GC_CHIP_INFO_BUS64_BITS_Type                                         U01
#define   GC_CHIP_INFO_BUS64_BITS_NONE                                       0x0
#define   GC_CHIP_INFO_BUS64_BITS_AVAILABLE                                  0x1

/* 1 :  axi bus width is 128-bit */
#define GC_CHIP_INFO_BUS128_BITS                                           21:21
#define GC_CHIP_INFO_BUS128_BITS_End                                          21
#define GC_CHIP_INFO_BUS128_BITS_Start                                        21
#define GC_CHIP_INFO_BUS128_BITS_Type                                        U01
#define   GC_CHIP_INFO_BUS128_BITS_NONE                                      0x0
#define   GC_CHIP_INFO_BUS128_BITS_AVAILABLE                                 0x1

/* DEC Engine has Decoder, Encoder and Tile Status Manager */
#define GC_CHIP_INFO_DEC200                                                24:24
#define GC_CHIP_INFO_DEC200_End                                               24
#define GC_CHIP_INFO_DEC200_Start                                             24
#define GC_CHIP_INFO_DEC200_Type                                             U01
#define   GC_CHIP_INFO_DEC200_NONE                                           0x0
#define   GC_CHIP_INFO_DEC200_AVAILABLE                                      0x1

/* DEC Engine has Decoder and Tile Status Manager */
#define GC_CHIP_INFO_DEC200D                                               25:25
#define GC_CHIP_INFO_DEC200D_End                                              25
#define GC_CHIP_INFO_DEC200D_Start                                            25
#define GC_CHIP_INFO_DEC200D_Type                                            U01
#define   GC_CHIP_INFO_DEC200D_NONE                                          0x0
#define   GC_CHIP_INFO_DEC200D_AVAILABLE                                     0x1

/* DEC Engine has Encoder and Tile Status Manager */
#define GC_CHIP_INFO_DEC200E                                               26:26
#define GC_CHIP_INFO_DEC200E_End                                              26
#define GC_CHIP_INFO_DEC200E_Start                                            26
#define GC_CHIP_INFO_DEC200E_Type                                            U01
#define   GC_CHIP_INFO_DEC200E_NONE                                          0x0
#define   GC_CHIP_INFO_DEC200E_AVAILABLE                                     0x1

/* DEC Engine has Decoder Only and has no Tile Status Manager */
#define GC_CHIP_INFO_DEC200DO                                              27:27
#define GC_CHIP_INFO_DEC200DO_End                                             27
#define GC_CHIP_INFO_DEC200DO_Start                                           27
#define GC_CHIP_INFO_DEC200DO_Type                                           U01
#define   GC_CHIP_INFO_DEC200DO_NONE                                         0x0
#define   GC_CHIP_INFO_DEC200DO_AVAILABLE                                    0x1

/* DC compression */
#define GC_CHIP_INFO_COMPRESSION                                           28:28
#define GC_CHIP_INFO_COMPRESSION_End                                          28
#define GC_CHIP_INFO_COMPRESSION_Start                                        28
#define GC_CHIP_INFO_COMPRESSION_Type                                        U01
#define   GC_CHIP_INFO_COMPRESSION_NONE                                      0x0
#define   GC_CHIP_INFO_COMPRESSION_AVAILABLE                                 0x1

/* four port (2R-2W) ram instantiated in the design */
#define GC_CHIP_INFO_FOUR_PORT_RAM                                         29:29
#define GC_CHIP_INFO_FOUR_PORT_RAM_End                                        29
#define GC_CHIP_INFO_FOUR_PORT_RAM_Start                                      29
#define GC_CHIP_INFO_FOUR_PORT_RAM_Type                                      U01
#define   GC_CHIP_INFO_FOUR_PORT_RAM_NONE                                    0x0
#define   GC_CHIP_INFO_FOUR_PORT_RAM_AVAILABLE                               0x1

/* Includes shader clock which is indeoendent from core clock. */
#define GC_CHIP_INFO_EXTRA_CLOCK_FOR_SH                                    30:30
#define GC_CHIP_INFO_EXTRA_CLOCK_FOR_SH_End                                   30
#define GC_CHIP_INFO_EXTRA_CLOCK_FOR_SH_Start                                 30
#define GC_CHIP_INFO_EXTRA_CLOCK_FOR_SH_Type                                 U01
#define   GC_CHIP_INFO_EXTRA_CLOCK_FOR_SH_NONE                               0x0
#define   GC_CHIP_INFO_EXTRA_CLOCK_FOR_SH_AVAILABLE                          0x1

/* Register gcProductId **
** ~~~~~~~~~~~~~~~~~~~~ */

/* Shows Product ID */

#define gcProductIdRegAddrs                                               0x002A
#define GC_PRODUCT_ID_Address                                            0x000A8
#define GC_PRODUCT_ID_MSB                                                     15
#define GC_PRODUCT_ID_LSB                                                      0
#define GC_PRODUCT_ID_BLK                                                      0
#define GC_PRODUCT_ID_Count                                                    1
#define GC_PRODUCT_ID_FieldMask                                       0xFFFFFFFF
#define GC_PRODUCT_ID_ReadMask                                        0xFFFFFFFF
#define GC_PRODUCT_ID_WriteMask                                       0x00000000
#define GC_PRODUCT_ID_ResetValue                                      0x00000000

/* 0:None-no extra letter on the product name  1:N-Nano  2:L-Lite  3:UL-Ultra **
** Lite  4:XS-Ts/Gs core  5:NU-Nano Ultra  6:NL-Nano Lite  7:NU3-Nano Ultra3  **
** 8:XSVX  9:NU2(NanoUltra2)  A:LXS(Lite with TS/GS)  B:LXSVX(Lite with TS/GS **
** + Vision)  C:ULXS(UltraLite with TS/GS)  D:VX(Vision, no TS/GS)            **
** E:LVX(Lite with Vision)  F:ULVX(UntraLite with Vision)  If TYPE=5(VIP),bit **
** 3 indicates float support: 0 = Float is supported; 1= Float is not         **
** supported.                                                                 */
#define GC_PRODUCT_ID_GRADE_LEVEL                                            3:0
#define GC_PRODUCT_ID_GRADE_LEVEL_End                                          3
#define GC_PRODUCT_ID_GRADE_LEVEL_Start                                        0
#define GC_PRODUCT_ID_GRADE_LEVEL_Type                                       U04

/* Product Number */
#define GC_PRODUCT_ID_NUM                                                   23:4
#define GC_PRODUCT_ID_NUM_End                                                 23
#define GC_PRODUCT_ID_NUM_Start                                                4
#define GC_PRODUCT_ID_NUM_Type                                               U20

/* 0:GC (GCOCORE 3D and 2D+3D Graphic Cores) 1:DEC (Decode/Encode engine)     **
** 2:DC (Dosplay Controller) 3:VG (Vector Graphics) 4:SC (Scalar Graphics)    **
** 5:VIP (Vision Image Processor: OpenVX/VIP only core) 6:CC (Compute Core:   **
** OpenCL, no Vision, no 3D) 7:GC GCCORE 2D only                              */
#define GC_PRODUCT_ID_TYPE                                                 27:24
#define GC_PRODUCT_ID_TYPE_End                                                27
#define GC_PRODUCT_ID_TYPE_Start                                              24
#define GC_PRODUCT_ID_TYPE_Type                                              U04

#define GC_PRODUCT_ID_AUX                                                  31:28
#define GC_PRODUCT_ID_AUX_End                                                 31
#define GC_PRODUCT_ID_AUX_Start                                               28
#define GC_PRODUCT_ID_AUX_Type                                               U04

/* Register gcEcoId **
** ~~~~~~~~~~~~~~~~ */

/* Shows the ID for the chip ECO. */

#define gcEcoIdRegAddrs                                                   0x003A
#define GC_ECO_ID_Address                                                0x000E8
#define GC_ECO_ID_MSB                                                         15
#define GC_ECO_ID_LSB                                                          0
#define GC_ECO_ID_BLK                                                          0
#define GC_ECO_ID_Count                                                        1
#define GC_ECO_ID_FieldMask                                           0xFF0000FF
#define GC_ECO_ID_ReadMask                                            0xFF0000FF
#define GC_ECO_ID_WriteMask                                           0x00000000
#define GC_ECO_ID_ResetValue                                          0x00000000

/* ECO Id. */
#define GC_ECO_ID_ID                                                         7:0
#define GC_ECO_ID_ID_End                                                       7
#define GC_ECO_ID_ID_Start                                                     0
#define GC_ECO_ID_ID_Type                                                    U08

/* For example:  0x00: No convolution cores.  0x01: 1 CC, (= .5 NN).  0x02: 2 **
** CC, S variant (Single = 1NN).  0x03: 3 CC, S+ variant (= 1.5 NN).  0x04: 4 **
** CC, D variant (Dual = 2NN).  0x06: 6 CC, D+ variant (= 3 NN).  0x08: 8 CC, **
** Q variant (Quad = 4 NN).  0x10: 16 CC, O variant (Octo = 8 NN).  0x20: 32  **
** CC, H variant (Hex = 16 NN).                                               */
#define GC_ECO_ID_CONV_COUNT                                               31:24
#define GC_ECO_ID_CONV_COUNT_End                                              31
#define GC_ECO_ID_CONV_COUNT_Start                                            24
#define GC_ECO_ID_CONV_COUNT_Type                                            U08


/*******************************************************************************
**                            ~~~~~~~~~~~~~~~~~~~~                            **
**                            Module Memory Managerment                       **
**                            ~~~~~~~~~~~~~~~~~~~~                            **
*******************************************************************************/

/* Register gcregMMUConfiguration **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* MMU Master TLB Configuration register. */

#define gcregMMUConfigurationRegAddrs                                    0x0061
#define GCREG_MMU_CONFIGURATION_Address                                  0x00184
#define GCREG_MMU_CONFIGURATION_ResetValue                               0x00000000


/* Register gcregMMUControl **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* MMU request ID setting register. */

#define gcregMMUControlRegAddrs                                          0x0063
#define GCREG_MMU_CONTROL_Address                                        0x0018C
#define GCREG_MMU_CONTROL_ResetValue                                     0x000001F0


/* Register gcregMMUConfig **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* MMU Configuration register. */

#define gcregMMUConfigRegAddrs                                           0x006B
#define GCREG_MMU_CONFIG_Address                                         0x001AC
#define GCREG_MMU_CONFIG_ResetValue                                      0x00000000


/* Register gcregMMUAHBControl **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Control register that enables the MMU (only time shot). */

#define gcregMMUAHBControlRegAddrs                                        0x00E2
#define GCREG_MMUAHB_CONTROL_Address                                     0x00388
#define GCREG_MMUAHB_CONTROL_MSB                                              15
#define GCREG_MMUAHB_CONTROL_LSB                                               0
#define GCREG_MMUAHB_CONTROL_BLK                                               0
#define GCREG_MMUAHB_CONTROL_Count                                             1
#define GCREG_MMUAHB_CONTROL_FieldMask                                0x000003FF
#define GCREG_MMUAHB_CONTROL_ReadMask                                 0x000003E0
#define GCREG_MMUAHB_CONTROL_WriteMask                                0x000003FF
#define GCREG_MMUAHB_CONTROL_ResetValue                               0x00000000

/* Enable the MMU. For security reasons, once the MMU is  enabled it cannot   **
** be disabled anymore.writeOnce                                              */
#define GCREG_MMUAHB_CONTROL_MMU                                             0:0
#define GCREG_MMUAHB_CONTROL_MMU_End                                           0
#define GCREG_MMUAHB_CONTROL_MMU_Start                                         0
#define GCREG_MMUAHB_CONTROL_MMU_Type                                        U01
#define   GCREG_MMUAHB_CONTROL_MMU_ENABLE                                    0x1
#define   GCREG_MMUAHB_CONTROL_MMU_DISABLE                                   0x0

/* StrictMode enable. it is for secure mode. */
#define GCREG_MMUAHB_CONTROL_STRICT_MODE                                     1:1
#define GCREG_MMUAHB_CONTROL_STRICT_MODE_End                                   1
#define GCREG_MMUAHB_CONTROL_STRICT_MODE_Start                                 1
#define GCREG_MMUAHB_CONTROL_STRICT_MODE_Type                                U01
#define   GCREG_MMUAHB_CONTROL_STRICT_MODE_ENABLE                            0x1
#define   GCREG_MMUAHB_CONTROL_STRICT_MODE_DISABLE                           0x0

/* OutOfBoundException enable. */
#define GCREG_MMUAHB_CONTROL_OUT_OF_BOUND_EXCEPTION                          2:2
#define GCREG_MMUAHB_CONTROL_OUT_OF_BOUND_EXCEPTION_End                        2
#define GCREG_MMUAHB_CONTROL_OUT_OF_BOUND_EXCEPTION_Start                      2
#define GCREG_MMUAHB_CONTROL_OUT_OF_BOUND_EXCEPTION_Type                     U01
#define   GCREG_MMUAHB_CONTROL_OUT_OF_BOUND_EXCEPTION_ENABLE                 0x1
#define   GCREG_MMUAHB_CONTROL_OUT_OF_BOUND_EXCEPTION_DISABLE                0x0

/* SecurityViolationException enable. */
#define GCREG_MMUAHB_CONTROL_SECURITY_VIOL_EXCEPTION                         3:3
#define GCREG_MMUAHB_CONTROL_SECURITY_VIOL_EXCEPTION_End                       3
#define GCREG_MMUAHB_CONTROL_SECURITY_VIOL_EXCEPTION_Start                     3
#define GCREG_MMUAHB_CONTROL_SECURITY_VIOL_EXCEPTION_Type                    U01
#define   GCREG_MMUAHB_CONTROL_SECURITY_VIOL_EXCEPTION_ENABLE                0x1
#define   GCREG_MMUAHB_CONTROL_SECURITY_VIOL_EXCEPTION_DISABLE               0x0

/* When Enable. NS master reads => AxPort[1]NS, S master reads => AxPort[1]   **
** MTLB securebit   else AxPROT[1] = master_NS ? NS : S                       */
#define GCREG_MMUAHB_CONTROL_FORCE_NSBUFFER_READ_COHERENCY                   4:4
#define GCREG_MMUAHB_CONTROL_FORCE_NSBUFFER_READ_COHERENCY_End                 4
#define GCREG_MMUAHB_CONTROL_FORCE_NSBUFFER_READ_COHERENCY_Start               4
#define GCREG_MMUAHB_CONTROL_FORCE_NSBUFFER_READ_COHERENCY_Type              U01
#define   GCREG_MMUAHB_CONTROL_FORCE_NSBUFFER_READ_COHERENCY_ENABLE          0x1
#define   GCREG_MMUAHB_CONTROL_FORCE_NSBUFFER_READ_COHERENCY_DISABLE         0x0

/* Set up MMU from CMD or REG. */
#define GCREG_MMUAHB_CONTROL_SET_UP_MMU                                      5:5
#define GCREG_MMUAHB_CONTROL_SET_UP_MMU_End                                    5
#define GCREG_MMUAHB_CONTROL_SET_UP_MMU_Start                                  5
#define GCREG_MMUAHB_CONTROL_SET_UP_MMU_Type                                 U01
#define   GCREG_MMUAHB_CONTROL_SET_UP_MMU_FROM_CMD                           0x0
#define   GCREG_MMUAHB_CONTROL_SET_UP_MMU_FROM_REG                           0x1

/* Bits for hi address (above 4G). */
#define GCREG_MMUAHB_CONTROL_HI_ADDRESS_BITS                                 9:6
#define GCREG_MMUAHB_CONTROL_HI_ADDRESS_BITS_End                               9
#define GCREG_MMUAHB_CONTROL_HI_ADDRESS_BITS_Start                             6
#define GCREG_MMUAHB_CONTROL_HI_ADDRESS_BITS_Type                            U04

/* Register gcregMMUAHBTableArraySize **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* 16 bit MMUTableArraySize. */

#define gcregMMUAHBTableArraySizeRegAddrs                                 0x00E5
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_Address                            0x00394
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_MSB                                     15
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_LSB                                      0
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_BLK                                      0
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_Count                                    1
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_FieldMask                       0x0000FFFF
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_ReadMask                        0x0000FFFF
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_WriteMask                       0x0000FFFF
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_ResetValue                      0x0000FFFF

#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_SIZE                                  15:0
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_SIZE_End                                15
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_SIZE_Start                               0
#define GCREG_MMUAHB_TABLE_ARRAY_SIZE_SIZE_Type                              U16

/* Register gcregMMUAHBSafeNonSecureAddress **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* A 64-byte address that will acts as a 'safe' zone.  Any  address that      **
** would cause an exception is routed to this safe  zone.  Reads will happend **
** and writes will go to this address,  but with a write-enable of 0.  This   **
** register can only be programmed once after a reset - any  attempt to write **
** to this register after the initial  write-after-reset will be ignored.     **
** This is in Non Secure memory.                                              */

#define gcregMMUAHBSafeNonSecureAddressRegAddrs                           0x00E6
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_Address                     0x00398
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_MSB                              15
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_LSB                               0
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_BLK                               0
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_Count                             1
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_FieldMask                0xFFFFFFFF
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_ReadMask                 0xFFFFFFC0
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_WriteMask                0xFFFFFFC0
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_ResetValue               0x00000000

#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_ADDRESS                        31:0
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_ADDRESS_End                      31
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_ADDRESS_Start                     0
#define GCREG_MMUAHB_SAFE_NON_SECURE_ADDRESS_ADDRESS_Type                    U32

/* Register gcregMMUAHBSafeSecureAddress **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* A 64-byte address that will acts as a 'safe' zone.  Any  address that      **
** would cause an exception is routed to this safe  zone.  Reads will happend **
** and writes will go to this address,  but with a write-enable of 0.  This   **
** register can only be programmed once after a reset - any  attempt to write **
** to this register after the initial  write-after-reset will be ignored.     **
** This is in Secure memory.                                                  */

#define gcregMMUAHBSafeSecureAddressRegAddrs                              0x00E7
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_Address                         0x0039C
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_MSB                                  15
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_LSB                                   0
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_BLK                                   0
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_Count                                 1
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_FieldMask                    0xFFFFFFFF
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_ReadMask                     0xFFFFFFC0
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_WriteMask                    0xFFFFFFC0
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_ResetValue                   0x00000000

#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_ADDRESS                            31:0
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_ADDRESS_End                          31
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_ADDRESS_Start                         0
#define GCREG_MMUAHB_SAFE_SECURE_ADDRESS_ADDRESS_Type                        U32

/* Register gcregMMUAHBSafeAddressExt **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Extended configuration register that holds the upper bits of safeAddress */

#define gcregMMUAHBSafeAddressExtRegAddrs                                 0x00E8
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_Address                            0x003A0
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MSB                                     15
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_LSB                                      0
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_BLK                                      0
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_Count                                    1
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_FieldMask                       0xC0FFC0FF
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_ReadMask                        0xC0FFC0FF
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_WriteMask                       0xC0FFC0FF
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_ResetValue                      0x00000000

/* Upper 8-bits of the safe address to form a true 40-bit address. This is    **
** for Non Secure Address                                                     */
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE                7:0
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE_End              7
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE_Start            0
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE_Type           U08

/* Bit that defines whether the safe address is shareable or not. */
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE_SHAREABLE    14:14
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE_SHAREABLE_End   14
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE_SHAREABLE_Start 14
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_NON_SECURE_SHAREABLE_Type U01

/* Mask bits 0 through 14. */
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_NON_SECURE         15:15
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_NON_SECURE_End        15
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_NON_SECURE_Start      15
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_NON_SECURE_Type      U01
#define   GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_NON_SECURE_ENABLED 0x0
#define   GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_NON_SECURE_MASKED  0x1

/* Upper 8-bits of the safe address to form a true 40-bit address. This is    **
** for Secure Address                                                         */
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE                  23:16
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE_End                 23
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE_Start               16
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE_Type               U08

/* Bit that defines whether the safe address is shareable or not. */
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE_SHAREABLE        30:30
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE_SHAREABLE_End       30
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE_SHAREABLE_Start     30
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_SAFE_ADDRESS_SECURE_SHAREABLE_Type     U01

/* Mask bits 0 through 14. */
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_SECURE             31:31
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_SECURE_End            31
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_SECURE_Start          31
#define GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_SECURE_Type          U01
#define   GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_SECURE_ENABLED     0x0
#define   GCREG_MMUAHB_SAFE_ADDRESS_EXT_MASK_SAFE_ADDRESS_SECURE_MASKED      0x1

/* Register gcregMMUAHBContextPDEntry0 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* MMU context page descriptor register. */

#define gcregMMUAHBContextPDEntry0RegAddrs                                0x00ED
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_Address                           0x003B4
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MSB                                    15
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_LSB                                     0
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_BLK                                     0
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_Count                                   1
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_FieldMask                      0xFFFFFFFF
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_ReadMask                       0xFFFFFFFF
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_WriteMask                      0xFFFFFFFF
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_ResetValue                     0x00000000

/* Master TLB mode. */
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_MODE                       1:0
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_MODE_End                     1
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_MODE_Start                   0
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_MODE_Type                  U02
#define   GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_MODE_FOUR_K_MODE         0x0
#define   GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_MODE_ONE_K_MODE          0x1
#define   GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_MODE_BY_PASS_MODE        0x2

/* Bit that defines whether the master TLB address is shareable or not. */
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SHARABLE                   2:2
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SHARABLE_End                 2
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SHARABLE_Start               2
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SHARABLE_Type              U01

/* Bit that defines whether the master TLB address is secure or not. */
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SECURE                     3:3
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SECURE_End                   3
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SECURE_Start                 3
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_MASTER_TLB_SECURE_Type                U01

/* PD address [39:12], 4K aligned.  Lower 12 bits are zeros. */
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_PD_ADDRESS                           31:4
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_PD_ADDRESS_End                         31
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_PD_ADDRESS_Start                        4
#define GCREG_MMUAHB_CONTEXT_PD_ENTRY0_PD_ADDRESS_Type                       U28

/* Register gcregMMUSafeAddress **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* A 64-byte address that will acts as a 'safe' zone.  Any address that would **
** cause an exception is routed to this safe zone.  Reads will happend and    **
** writes will go to this address, but with a write-enable of 0. This         **
** register can only be programmed once after a reset - any attempt to write  **
** to this register after the initial write-after-reset will be ignored. This **
** is in Non-Secure memory. This register is obsolete.                        */

#define gcregMMUSafeAddressRegAddrs                                       0x0060
#define GCREG_MMU_SAFE_ADDRESS_Address                                   0x00180
#define GCREG_MMU_SAFE_ADDRESS_MSB                                            15
#define GCREG_MMU_SAFE_ADDRESS_LSB                                             0
#define GCREG_MMU_SAFE_ADDRESS_BLK                                             0
#define GCREG_MMU_SAFE_ADDRESS_Count                                           1
#define GCREG_MMU_SAFE_ADDRESS_FieldMask                              0xFFFFFFFF
#define GCREG_MMU_SAFE_ADDRESS_ReadMask                               0xFFFFFFC0
#define GCREG_MMU_SAFE_ADDRESS_WriteMask                              0xFFFFFFC0
#define GCREG_MMU_SAFE_ADDRESS_ResetValue                             0x00000000

#define GCREG_MMU_SAFE_ADDRESS_ADDRESS                                      31:0
#define GCREG_MMU_SAFE_ADDRESS_ADDRESS_End                                    31
#define GCREG_MMU_SAFE_ADDRESS_ADDRESS_Start                                   0
#define GCREG_MMU_SAFE_ADDRESS_ADDRESS_Type                                  U32

/* Register gcregMMUConfiguration **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* This register controls the master TLB of the MMU. */

#define gcregMMUConfigurationRegAddrs                                     0x0061
#define GCREG_MMU_CONFIGURATION_Address                                  0x00184
#define GCREG_MMU_CONFIGURATION_MSB                                           15
#define GCREG_MMU_CONFIGURATION_LSB                                            0
#define GCREG_MMU_CONFIGURATION_BLK                                            0
#define GCREG_MMU_CONFIGURATION_Count                                          1
#define GCREG_MMU_CONFIGURATION_FieldMask                             0xFFFFFD99
#define GCREG_MMU_CONFIGURATION_ReadMask                              0xFFFFFD99
#define GCREG_MMU_CONFIGURATION_WriteMask                             0xFFFFFD99
#define GCREG_MMU_CONFIGURATION_ResetValue                            0x00000000

/* Set the mode for the Master TLB. This field is obsolete. */
#define GCREG_MMU_CONFIGURATION_MODE                                         0:0
#define GCREG_MMU_CONFIGURATION_MODE_End                                       0
#define GCREG_MMU_CONFIGURATION_MODE_Start                                     0
#define GCREG_MMU_CONFIGURATION_MODE_Type                                    U01
/* The Master TLB is 4kB in size and contains 1024 entries. Each page can be  **
** 4kB or 64kB in size.                                                       */
#define   GCREG_MMU_CONFIGURATION_MODE_MODE4_K                               0x0
/* The Master TLB is 1kB in size and contains 256 entries. Each page can be   **
** 4kB, 64kB, 1MB or 16MB in size.                                            */
#define   GCREG_MMU_CONFIGURATION_MODE_MODE1_K                               0x1

/* Mask Mode field. This field is obsolete. */
#define GCREG_MMU_CONFIGURATION_MASK_MODE                                    3:3
#define GCREG_MMU_CONFIGURATION_MASK_MODE_End                                  3
#define GCREG_MMU_CONFIGURATION_MASK_MODE_Start                                3
#define GCREG_MMU_CONFIGURATION_MASK_MODE_Type                               U01
#define   GCREG_MMU_CONFIGURATION_MASK_MODE_ENABLED                          0x0
#define   GCREG_MMU_CONFIGURATION_MASK_MODE_MASKED                           0x1

/* Flush the MMU caches. */
#define GCREG_MMU_CONFIGURATION_FLUSH                                        4:4
#define GCREG_MMU_CONFIGURATION_FLUSH_End                                      4
#define GCREG_MMU_CONFIGURATION_FLUSH_Start                                    4
#define GCREG_MMU_CONFIGURATION_FLUSH_Type                                   U01
#define   GCREG_MMU_CONFIGURATION_FLUSH_FLUSH                                0x1

/* Mask Flush field. */
#define GCREG_MMU_CONFIGURATION_MASK_FLUSH                                   7:7
#define GCREG_MMU_CONFIGURATION_MASK_FLUSH_End                                 7
#define GCREG_MMU_CONFIGURATION_MASK_FLUSH_Start                               7
#define GCREG_MMU_CONFIGURATION_MASK_FLUSH_Type                              U01
#define   GCREG_MMU_CONFIGURATION_MASK_FLUSH_ENABLED                         0x0
#define   GCREG_MMU_CONFIGURATION_MASK_FLUSH_MASKED                          0x1

/* Mask for Address field. This field is obsolete. */
#define GCREG_MMU_CONFIGURATION_MASK_ADDRESS                                 8:8
#define GCREG_MMU_CONFIGURATION_MASK_ADDRESS_End                               8
#define GCREG_MMU_CONFIGURATION_MASK_ADDRESS_Start                             8
#define GCREG_MMU_CONFIGURATION_MASK_ADDRESS_Type                            U01
#define   GCREG_MMU_CONFIGURATION_MASK_ADDRESS_ENABLED                       0x0
#define   GCREG_MMU_CONFIGURATION_MASK_ADDRESS_MASKED                        0x1

/* Upper bits of the page aligned (depending on the mode) master TLB. This    **
** field is obsolete.                                                         */
#define GCREG_MMU_CONFIGURATION_ADDRESS                                    31:10
#define GCREG_MMU_CONFIGURATION_ADDRESS_End                                   31
#define GCREG_MMU_CONFIGURATION_ADDRESS_Start                                 10
#define GCREG_MMU_CONFIGURATION_ADDRESS_Type                                 U22

/* Register gcregMMUControl **
** ~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Control register that enables the MMU (only time shot). This register is   **
** obsolete.                                                                  */

#define gcregMMUControlRegAddrs                                           0x0063
#define GCREG_MMU_CONTROL_Address                                        0x0018C
#define GCREG_MMU_CONTROL_MSB                                                 15
#define GCREG_MMU_CONTROL_LSB                                                  0
#define GCREG_MMU_CONTROL_BLK                                                  0
#define GCREG_MMU_CONTROL_Count                                                1
#define GCREG_MMU_CONTROL_FieldMask                                   0x000003FF
#define GCREG_MMU_CONTROL_ReadMask                                    0x000003F0
#define GCREG_MMU_CONTROL_WriteMask                                   0x000003FF
#define GCREG_MMU_CONTROL_ResetValue                                  0x000001F0

/* Enable the MMU. For security reasons, once the MMU is  enabled it cannot   **
** be disabled anymore.                                                       */
#define GCREG_MMU_CONTROL_ENABLE                                             0:0
#define GCREG_MMU_CONTROL_ENABLE_End                                           0
#define GCREG_MMU_CONTROL_ENABLE_Start                                         0
#define GCREG_MMU_CONTROL_ENABLE_Type                                        U01
#define   GCREG_MMU_CONTROL_ENABLE_ENABLE                                    0x1

/* StrictMode enable. it is for secure mode. */
#define GCREG_MMU_CONTROL_STRICT_MODE                                        1:1
#define GCREG_MMU_CONTROL_STRICT_MODE_End                                      1
#define GCREG_MMU_CONTROL_STRICT_MODE_Start                                    1
#define GCREG_MMU_CONTROL_STRICT_MODE_Type                                   U01
#define   GCREG_MMU_CONTROL_STRICT_MODE_ENABLE                               0x1

/* OutOfBoundException enable. */
#define GCREG_MMU_CONTROL_OUT_OF_BOUND_EXCEPTION                             2:2
#define GCREG_MMU_CONTROL_OUT_OF_BOUND_EXCEPTION_End                           2
#define GCREG_MMU_CONTROL_OUT_OF_BOUND_EXCEPTION_Start                         2
#define GCREG_MMU_CONTROL_OUT_OF_BOUND_EXCEPTION_Type                        U01
#define   GCREG_MMU_CONTROL_OUT_OF_BOUND_EXCEPTION_ENABLE                    0x1

/* SecurityViolationException enable. */
#define GCREG_MMU_CONTROL_SECURITY_VIOL_EXCEPTION                            3:3
#define GCREG_MMU_CONTROL_SECURITY_VIOL_EXCEPTION_End                          3
#define GCREG_MMU_CONTROL_SECURITY_VIOL_EXCEPTION_Start                        3
#define GCREG_MMU_CONTROL_SECURITY_VIOL_EXCEPTION_Type                       U01
#define   GCREG_MMU_CONTROL_SECURITY_VIOL_EXCEPTION_ENABLE                   0x1

/* The ID for MMU */
#define GCREG_MMU_CONTROL_MMU_ID                                             9:4
#define GCREG_MMU_CONTROL_MMU_ID_End                                           9
#define GCREG_MMU_CONTROL_MMU_ID_Start                                         4
#define GCREG_MMU_CONTROL_MMU_ID_Type                                        U06


/*******************************************************************************
**                            ~~~~~~~~~~~~~~~~~~~~                            **
**                                Module DEC                                  **
**                            ~~~~~~~~~~~~~~~~~~~~                            **
*******************************************************************************/

/* Register gcregAHBDECControl **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Global control register for dec core. */

#define gcregAHBDECControlRegAddrs                                        0x0200
#define GCREG_AHBDEC_CONTROL_Address                                     0x00800
#define GCREG_AHBDEC_CONTROL_MSB                                              15
#define GCREG_AHBDEC_CONTROL_LSB                                               0
#define GCREG_AHBDEC_CONTROL_BLK                                               0
#define GCREG_AHBDEC_CONTROL_Count                                             1
#define GCREG_AHBDEC_CONTROL_FieldMask                                0xFFFFFFFF
#define GCREG_AHBDEC_CONTROL_ReadMask                                 0xFFFFFFFF
#define GCREG_AHBDEC_CONTROL_WriteMask                                0x4AFF01DF
#define GCREG_AHBDEC_CONTROL_ResetValue                               0x0201018A

/* Flush tile status cache. This bit is Volatile.  */
#define GCREG_AHBDEC_CONTROL_FLUSH                                           0:0
#define GCREG_AHBDEC_CONTROL_FLUSH_End                                         0
#define GCREG_AHBDEC_CONTROL_FLUSH_Start                                       0
#define GCREG_AHBDEC_CONTROL_FLUSH_Type                                      U01
#define   GCREG_AHBDEC_CONTROL_FLUSH_DISABLE                                 0x0
#define   GCREG_AHBDEC_CONTROL_FLUSH_ENABLE                                  0x1

/* Bypass compression for all streams.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION                             1:1
#define GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION_End                           1
#define GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION_Start                         1
#define GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION_Type                        U01
/* * Global bypass *  */
#define   GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION_DISABLE            (0x1 << 1)
#define   GCREG_AHBDEC_CONTROL_DISABLE_COMPRESSION_ENABLE                    0x0

/* Disable clock gating for rams.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_CLOCK_GATING                        2:2
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_CLOCK_GATING_End                      2
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_CLOCK_GATING_Start                    2
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_CLOCK_GATING_Type                   U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_RAM_CLOCK_GATING_DISABLE              0x0
#define   GCREG_AHBDEC_CONTROL_DISABLE_RAM_CLOCK_GATING_ENABLE               0x1

/* Disable debug registers.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_DEBUG_REGISTERS                         3:3
#define GCREG_AHBDEC_CONTROL_DISABLE_DEBUG_REGISTERS_End                       3
#define GCREG_AHBDEC_CONTROL_DISABLE_DEBUG_REGISTERS_Start                     3
#define GCREG_AHBDEC_CONTROL_DISABLE_DEBUG_REGISTERS_Type                    U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_DEBUG_REGISTERS_DISABLE               0x1
#define   GCREG_AHBDEC_CONTROL_DISABLE_DEBUG_REGISTERS_ENABLE                0x0

/* Soft reset the DEC IP. This bit is Volatile.  */
#define GCREG_AHBDEC_CONTROL_SOFT_RESET                                      4:4
#define GCREG_AHBDEC_CONTROL_SOFT_RESET_End                                    4
#define GCREG_AHBDEC_CONTROL_SOFT_RESET_Start                                  4
#define GCREG_AHBDEC_CONTROL_SOFT_RESET_Type                                 U01
#define   GCREG_AHBDEC_CONTROL_SOFT_RESET_DISABLE                            0x0
#define   GCREG_AHBDEC_CONTROL_SOFT_RESET_ENABLE                             0x1

/* Disable ram power optimization.  This is obsolete for dec400. */
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION                  5:5
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_End                5
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_Start              5
#define GCREG_AHBDEC_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_Type             U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_DISABLE        0x0
#define   GCREG_AHBDEC_CONTROL_DISABLE_RAM_POWER_OPTIMIZATION_ENABLE         0x1

/* Flush Read Data cache. This bit is Volatile.  */
#define GCREG_AHBDEC_CONTROL_FLUSH_DCACHE                                    6:6
#define GCREG_AHBDEC_CONTROL_FLUSH_DCACHE_End                                  6
#define GCREG_AHBDEC_CONTROL_FLUSH_DCACHE_Start                                6
#define GCREG_AHBDEC_CONTROL_FLUSH_DCACHE_Type                               U01
#define   GCREG_AHBDEC_CONTROL_FLUSH_DCACHE_DISABLE                          0x0
#define   GCREG_AHBDEC_CONTROL_FLUSH_DCACHE_ENABLE                           0x1

/* Disable Read data cache.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_DCACHE                                  7:7
#define GCREG_AHBDEC_CONTROL_DISABLE_DCACHE_End                                7
#define GCREG_AHBDEC_CONTROL_DISABLE_DCACHE_Start                              7
#define GCREG_AHBDEC_CONTROL_DISABLE_DCACHE_Type                             U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_DCACHE_DISABLE                        0x0
#define   GCREG_AHBDEC_CONTROL_DISABLE_DCACHE_ENABLE                         0x1

/* Enable 64 bytes Write Request alignment mode.  1: align to 64 bytes        **
** (default); 0: align to 32 bytes.                                           */
#define GCREG_AHBDEC_CONTROL_WRITE_REQ_ALIGN_MODE                            8:8
#define GCREG_AHBDEC_CONTROL_WRITE_REQ_ALIGN_MODE_End                          8
#define GCREG_AHBDEC_CONTROL_WRITE_REQ_ALIGN_MODE_Start                        8
#define GCREG_AHBDEC_CONTROL_WRITE_REQ_ALIGN_MODE_Type                       U01
#define   GCREG_AHBDEC_CONTROL_WRITE_REQ_ALIGN_MODE_DISABLE                  0x0
#define   GCREG_AHBDEC_CONTROL_WRITE_REQ_ALIGN_MODE_ENABLE                   0x1

#define GCREG_AHBDEC_CONTROL_RESERVE0                                       15:9
#define GCREG_AHBDEC_CONTROL_RESERVE0_End                                     15
#define GCREG_AHBDEC_CONTROL_RESERVE0_Start                                    9
#define GCREG_AHBDEC_CONTROL_RESERVE0_Type                                   U07

/* Flush generated from EndAddr will be disabled.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_HW_FLUSH                              16:16
#define GCREG_AHBDEC_CONTROL_DISABLE_HW_FLUSH_End                             16
#define GCREG_AHBDEC_CONTROL_DISABLE_HW_FLUSH_Start                           16
#define GCREG_AHBDEC_CONTROL_DISABLE_HW_FLUSH_Type                           U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_HW_FLUSH_DISABLE                      0x1
#define   GCREG_AHBDEC_CONTROL_DISABLE_HW_FLUSH_ENABLE                       0x0

/* Disable clock. */
#define GCREG_AHBDEC_CONTROL_CLK_DIS                                       17:17
#define GCREG_AHBDEC_CONTROL_CLK_DIS_End                                      17
#define GCREG_AHBDEC_CONTROL_CLK_DIS_Start                                    17
#define GCREG_AHBDEC_CONTROL_CLK_DIS_Type                                    U01
#define   GCREG_AHBDEC_CONTROL_CLK_DIS_DISABLE                               0x1
#define   GCREG_AHBDEC_CONTROL_CLK_DIS_ENABLE                                0x0

/* ID of tile status flush.  bit[23] represent the request type, 0 is read, 1 **
** is write.  bit[22:18] is the stream id.  Just valid in DEC400.  Invalid in **
** DEC400_L and DEC400_UL.                                                    */
#define GCREG_AHBDEC_CONTROL_SW_FLUSH_ID                                   23:18
#define GCREG_AHBDEC_CONTROL_SW_FLUSH_ID_End                                  23
#define GCREG_AHBDEC_CONTROL_SW_FLUSH_ID_Start                                18
#define GCREG_AHBDEC_CONTROL_SW_FLUSH_ID_Type                                U06

/* Tile status cache mode for different walking pattern.  This is obsolete    **
** for dec400.                                                                */
#define GCREG_AHBDEC_CONTROL_TILE_CACHE_MODE                               24:24
#define GCREG_AHBDEC_CONTROL_TILE_CACHE_MODE_End                              24
#define GCREG_AHBDEC_CONTROL_TILE_CACHE_MODE_Start                            24
#define GCREG_AHBDEC_CONTROL_TILE_CACHE_MODE_Type                            U01
#define   GCREG_AHBDEC_CONTROL_TILE_CACHE_MODE_MODE16X64                     0x0
#define   GCREG_AHBDEC_CONTROL_TILE_CACHE_MODE_MODE64X16                     0x1

/* Disable cache prefetch.  0 indicates enable cache prefetch.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_CACHE_PREFETCH                        25:25
#define GCREG_AHBDEC_CONTROL_DISABLE_CACHE_PREFETCH_End                       25
#define GCREG_AHBDEC_CONTROL_DISABLE_CACHE_PREFETCH_Start                     25
#define GCREG_AHBDEC_CONTROL_DISABLE_CACHE_PREFETCH_Type                     U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_CACHE_PREFETCH_DISABLED               0x1
#define   GCREG_AHBDEC_CONTROL_DISABLE_CACHE_PREFETCH_ENABLED                0x0

/* AHB or LoadState mode.  This is obsolete for dec400.  */
#define GCREG_AHBDEC_CONTROL_CONFIGURE_MODE                                26:26
#define GCREG_AHBDEC_CONTROL_CONFIGURE_MODE_End                               26
#define GCREG_AHBDEC_CONTROL_CONFIGURE_MODE_Start                             26
#define GCREG_AHBDEC_CONTROL_CONFIGURE_MODE_Type                             U01
#define   GCREG_AHBDEC_CONTROL_CONFIGURE_MODE_AHB                            0x0
#define   GCREG_AHBDEC_CONTROL_CONFIGURE_MODE_LOAD_STATE                     0x1

/* Disable fetch destination.  This is obsolete for dec400.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_PREFETCH                              27:27
#define GCREG_AHBDEC_CONTROL_DISABLE_PREFETCH_End                             27
#define GCREG_AHBDEC_CONTROL_DISABLE_PREFETCH_Start                           27
#define GCREG_AHBDEC_CONTROL_DISABLE_PREFETCH_Type                           U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_PREFETCH_DISABLED                     0x0
#define   GCREG_AHBDEC_CONTROL_DISABLE_PREFETCH_ENABLED                      0x1

/* Configure the updating mode of shader registers.  Also used for flushing   **
** tile status cache.                                                         */
#define GCREG_AHBDEC_CONTROL_HW_UPDATE_SHADOW_REG_MODE                     28:28
#define GCREG_AHBDEC_CONTROL_HW_UPDATE_SHADOW_REG_MODE_End                    28
#define GCREG_AHBDEC_CONTROL_HW_UPDATE_SHADOW_REG_MODE_Start                  28
#define GCREG_AHBDEC_CONTROL_HW_UPDATE_SHADOW_REG_MODE_Type                  U01
#define   GCREG_AHBDEC_CONTROL_HW_UPDATE_SHADOW_REG_MODE_TRIGGER             0x0
#define   GCREG_AHBDEC_CONTROL_HW_UPDATE_SHADOW_REG_MODE_FRAME_END           0x1

/* Only used when HW flush is disabled.  */
#define GCREG_AHBDEC_CONTROL_SOFT_UPDATE_SHADOW_REG                        29:29
#define GCREG_AHBDEC_CONTROL_SOFT_UPDATE_SHADOW_REG_End                       29
#define GCREG_AHBDEC_CONTROL_SOFT_UPDATE_SHADOW_REG_Start                     29
#define GCREG_AHBDEC_CONTROL_SOFT_UPDATE_SHADOW_REG_Type                     U01
#define   GCREG_AHBDEC_CONTROL_SOFT_UPDATE_SHADOW_REG_DISABLED               0x0
#define   GCREG_AHBDEC_CONTROL_SOFT_UPDATE_SHADOW_REG_ENABLED                0x1

/* Disable clock gating for sub-modules.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_MODULE_CLOCK_GATING                   30:30
#define GCREG_AHBDEC_CONTROL_DISABLE_MODULE_CLOCK_GATING_End                  30
#define GCREG_AHBDEC_CONTROL_DISABLE_MODULE_CLOCK_GATING_Start                30
#define GCREG_AHBDEC_CONTROL_DISABLE_MODULE_CLOCK_GATING_Type                U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_MODULE_CLOCK_GATING_DISABLE           0x0
#define   GCREG_AHBDEC_CONTROL_DISABLE_MODULE_CLOCK_GATING_ENABLE            0x1

/* Globally disable clock gating.  */
#define GCREG_AHBDEC_CONTROL_DISABLE_GLOBAL_CLOCK_GATING                   31:31
#define GCREG_AHBDEC_CONTROL_DISABLE_GLOBAL_CLOCK_GATING_End                  31
#define GCREG_AHBDEC_CONTROL_DISABLE_GLOBAL_CLOCK_GATING_Start                31
#define GCREG_AHBDEC_CONTROL_DISABLE_GLOBAL_CLOCK_GATING_Type                U01
#define   GCREG_AHBDEC_CONTROL_DISABLE_GLOBAL_CLOCK_GATING_DISABLE           0x0
#define   GCREG_AHBDEC_CONTROL_DISABLE_GLOBAL_CLOCK_GATING_ENABLE            0x1

/* Register gcregAHBDECControlEx **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Global control extension register for dec core. */

#define gcregAHBDECControlExRegAddrs                                      0x0201
#define GCREG_AHBDEC_CONTROLEX_Address                                   0x00804
#define GCREG_AHBDEC_CONTROLEX_ResetValue                             0x00080000

/* Register gcregAHBDECControlEx2 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Global control extension2 register for dec core. */

#define gcregAHBDECControlEx2RegAddrs                                     0x0202
#define GCREG_AHBDEC_CONTROLEX2_Address                                  0x00808
#define GCREG_AHBDEC_CONTROLEX2_ResetValue                            0x003FC810

/* Register gcregAHBDECIntrEnbl **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DEC interrupt enable register. */

#define gcregAHBDECIntrEnblRegAddrs                                       0x0203
#define GCREG_AHBDEC_INTRENBL_Address                                    0x0080C
#define GCREG_AHBDEC_INTRENBL_ResetValue                              0x00000000

/* Register gcregAHBDECIntrEnblEx2 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#define gcregAHBDECIntrEnblExRegAddrs                                     0x0204
#define GCREG_AHBDEC_INTRENBLEX_Address                                  0x00810
#define GCREG_AHBDEC_INTRENBLEX_ResetValue                            0x00000000

/* Register gcregAHBDECIntrEnblEx2 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DEC interrupt enable extension2 register. */

#define gcregAHBDECIntrEnblEx2RegAddrs                                    0x0205
#define GCREG_AHBDEC_INTRENBLEX2_Address                                 0x00814
#define GCREG_AHBDEC_INTRENBLEX2_ResetValue                           0x00000000

/* Register gcregAHBDEReadConfig0 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DEC configuration register for read ID.  */

#define gcregAHBDECReadConfigRegAddrs                                     0x0220
#define GCREG_AHBDEC_READ_CONFIG_Address                                 0x00880
#define GCREG_AHBDEC_READ_CONFIG_MSB                                          15
#define GCREG_AHBDEC_READ_CONFIG_LSB                                           5
#define GCREG_AHBDEC_READ_CONFIG_BLK                                           5
#define GCREG_AHBDEC_READ_CONFIG_Count                                        32
#define GCREG_AHBDEC_READ_CONFIG_FieldMask                            0x7FFFFFFF
#define GCREG_AHBDEC_READ_CONFIG_ReadMask                             0x7FFFFFFF
#define GCREG_AHBDEC_READ_CONFIG_WriteMask                            0x7E0300FF
#define GCREG_AHBDEC_READ_CONFIG_ResetValue                           0x00020000

/* Compression enable for individual streams.  */
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ENABLE                          0:0
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ENABLE_End                        0
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ENABLE_Start                      0
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ENABLE_Type                     U01
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ENABLE_DISABLE                0x0
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ENABLE_ENABLE                 0x1

/* Support 64B/128B/256B/512B. This is obsolete for dec400. */
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE                            2:1
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE_End                          2
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE_Start                        1
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE_Type                       U02
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE_SIZE64_BYTE              0x0
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE_SIZE128_BYTE             0x1
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE_SIZE256_BYTE             0x2
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_SIZE_SIZE512_BYTE             0x3

/* Compression color format.  For YV12/NV12/NV16's Y plane and YV12's U or V  **
** plane, set to YUV_ONLY.  For NV12/NV16's UV plane, set to UV_MIX.  For     **
** compression core, the AYUV will be same with ARGB.                         */
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT                          7:3
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_End                        7
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_Start                      3
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_Type                     U05
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_ARGB8                 0x00
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_XRGB8                 0x01
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_AYUV                  0x02
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_UYVY                  0x03
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_YUY2                  0x04
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_YUV_ONLY              0x05
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_UV_MIX                0x06
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_ARGB4                 0x07
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_XRGB4                 0x08
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_A1RGB5                0x09
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_X1RGB5                0x0A
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_R5G6B5                0x0B
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_A2R10G10B10           0x0F
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_BAYER                 0x10
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_COEFFICIENT           0x12
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_ARGB16                0x13
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_FORMAT_X2RGB10               0x15

#define GCREG_AHBDEC_READ_CONFIG_RESERVE1                                   15:8
#define GCREG_AHBDEC_READ_CONFIG_RESERVE1_End                                 15
#define GCREG_AHBDEC_READ_CONFIG_RESERVE1_Start                                8
#define GCREG_AHBDEC_READ_CONFIG_RESERVE1_Type                               U08

/* Compression result size alignment mode.  Only support Align32Byte.  */
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE                    17:16
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE_End                   17
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE_Start                 16
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE_Type                 U02
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE_ALIGN1_BYTE        0x0
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE_ALIGN16_BYTE       0x1
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE_ALIGN32_BYTE       0x2
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE_ALIGN64_BYTE       0x3

/* Compression result size alignment mode.  */
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE1                   20:18
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE1_End                  20
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE1_Start                18
#define GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE1_Type                U03
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE1_ALIGN256_BYTE     0x4
#define   GCREG_AHBDEC_READ_CONFIG_COMPRESSION_ALIGN_MODE1_ALIGN512_BYTE     0x5

#define GCREG_AHBDEC_READ_CONFIG_RESERVE2                                  21:21
#define GCREG_AHBDEC_READ_CONFIG_RESERVE2_End                                 21
#define GCREG_AHBDEC_READ_CONFIG_RESERVE2_Start                               21
#define GCREG_AHBDEC_READ_CONFIG_RESERVE2_Type                               U01

/* The tile alignment mode. 0: A single tile forms a block; 1: Two tiles form **
** a block 2: Four tiles form a block.                                        */
#define GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE                           24:22
#define GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE_End                          24
#define GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE_Start                        22
#define GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE_Type                        U03
#define   GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE_TILE1_ALIGN               0x0
#define   GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE_TILE2_ALIGN               0x1
#define   GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE_TILE4_ALIGN               0x2
#define   GCREG_AHBDEC_READ_CONFIG_TILE_ALIGN_MODE_CBSR_ALIGN                0x3

/* How many pixels in the tile and the walking direction in the tile.  The    **
** values 0x1F, 0x20 and 0x21 are not available for all variants.             */
#define GCREG_AHBDEC_READ_CONFIG_TILE_MODE                                 30:25
#define GCREG_AHBDEC_READ_CONFIG_TILE_MODE_End                                30
#define GCREG_AHBDEC_READ_CONFIG_TILE_MODE_Start                              25
#define GCREG_AHBDEC_READ_CONFIG_TILE_MODE_Type                              U06
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE8X8_XMAJOR                 0x00
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE8X8_YMAJOR                 0x01
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE16X4                       0x02
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE8X4                        0x03
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE4X8                        0x04
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER16X4                     0x06
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE64X4                       0x07
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE32X4                       0x08
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER256X1                    0x09
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER128X1                    0x0A
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER64X4                     0x0B
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER256X2                    0x0C
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER128X2                    0x0D
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER128X4                    0x0E
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER64X1                     0x0F
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE16X8                       0x10
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE8X16                       0x11
/* * Supported for DEC400 only*  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER512X1                    0x12
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER32X4                     0x13
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER64X2                     0x14
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER32X2                     0x15
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER32X1                     0x16
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_RASTER16X1                     0x17
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE128X4                      0x18
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE256X4                      0x19
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE512X4                      0x1A
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE16X16                      0x1B
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE32X16                      0x1C
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE64X16                      0x1D
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE128X8                      0x1E
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE8X4_S                      0x1F
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE16X4_S                     0x20
/* * Not supported for DEC400L *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE32X4_S                     0x21
/* * Only supported for GC620 *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE16X4_LSB                   0x22
/* * Only supported for GC620 *  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE32X4_LSB                   0x23
/* * Only supported for OYB DC8000 and Falcon DC8000*  */
#define   GCREG_AHBDEC_READ_CONFIG_TILE_MODE_TILE32X8                       0x24

/* Register gcregAHBDECControlEx2 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Register gcregAHBDECControlEx **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Extra Control register for DEC core. */

#define gcregAHBDECControlExRegAddrs                                      0x0201
#define GCREG_AHBDEC_CONTROL_EX_Address                                  0x00804
#define GCREG_AHBDEC_CONTROL_EX_MSB                                           15
#define GCREG_AHBDEC_CONTROL_EX_LSB                                            0
#define GCREG_AHBDEC_CONTROL_EX_BLK                                            0
#define GCREG_AHBDEC_CONTROL_EX_Count                                          1
#define GCREG_AHBDEC_CONTROL_EX_FieldMask                             0xFFFFFFFF
#define GCREG_AHBDEC_CONTROL_EX_ReadMask                              0xFFFFFFFF
#define GCREG_AHBDEC_CONTROL_EX_WriteMask                             0x200EFFFF
#define GCREG_AHBDEC_CONTROL_EX_ResetValue                            0x00080000

/* Tile status cache's AxUSER value.  */
#define GCREG_AHBDEC_CONTROL_EX_AXUSER                                      11:0
#define GCREG_AHBDEC_CONTROL_EX_AXUSER_End                                    11
#define GCREG_AHBDEC_CONTROL_EX_AXUSER_Start                                   0
#define GCREG_AHBDEC_CONTROL_EX_AXUSER_Type                                  U12

/* Tile status cache's AxQOS value.  */
#define GCREG_AHBDEC_CONTROL_EX_AXQOS                                      15:12
#define GCREG_AHBDEC_CONTROL_EX_AXQOS_End                                     15
#define GCREG_AHBDEC_CONTROL_EX_AXQOS_Start                                   12
#define GCREG_AHBDEC_CONTROL_EX_AXQOS_Type                                   U04

/* Control bit for enable split the output burst to 64 bytes  */
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT                         16:16
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_End                        16
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_Start                      16
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_Type                      U01

/* Control bit to enable end address flush check as full cache line check.  */
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_END_ADDRESS_RANGE_CHECK             17:17
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_END_ADDRESS_RANGE_CHECK_End            17
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_END_ADDRESS_RANGE_CHECK_Start          17
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_END_ADDRESS_RANGE_CHECK_Type          U01

/* The select enable of Fast clear value.  0:32bits; 1:64bits.  */
#define GCREG_AHBDEC_CONTROL_EX_FAST_CLEAR_WIDTH_SEL                       18:18
#define GCREG_AHBDEC_CONTROL_EX_FAST_CLEAR_WIDTH_SEL_End                      18
#define GCREG_AHBDEC_CONTROL_EX_FAST_CLEAR_WIDTH_SEL_Start                    18
#define GCREG_AHBDEC_CONTROL_EX_FAST_CLEAR_WIDTH_SEL_Type                    U01

/* Define two write miss policies for the TSC.  POLICY0: Write Only;          **
** POLICY1: Read Modify Write.                                                */
#define GCREG_AHBDEC_CONTROL_EX_WRITE_MISS_POLICY                          19:19
#define GCREG_AHBDEC_CONTROL_EX_WRITE_MISS_POLICY_End                         19
#define GCREG_AHBDEC_CONTROL_EX_WRITE_MISS_POLICY_Start                       19
#define GCREG_AHBDEC_CONTROL_EX_WRITE_MISS_POLICY_Type                       U01
/* * Write Only * */
#define   GCREG_AHBDEC_CONTROL_EX_WRITE_MISS_POLICY_POLICY0                  0x0
/* * Read Modify Write * */
#define   GCREG_AHBDEC_CONTROL_EX_WRITE_MISS_POLICY_POLICY1                  0x1

/* Compression mode.  */
#define GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE                           21:20
#define GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE_End                          21
#define GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE_Start                        20
#define GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE_Type                        U02
/* * Enable all compression *  */
#define   GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE_MODE0                     0x0
/* * Enable only 128 byte compression *  */
#define   GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE_MODE1                     0x1
/* * Enable all compression but disallow any scenarios > 1CL(64 bytes) * */
#define   GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE_MODE2                     0x2
#define   GCREG_AHBDEC_CONTROL_EX_COMPRESSION_MODE_RESERVED                  0x3

/* Control bit for enable split the output burst to 64/128/256 bytes          **
** Available for GWL-C4 only.                                                 */
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX                      23:22
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX_End                     23
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX_Start                   22
#define GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX_Type                   U02
#define   GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX_BYTES64              0x0
#define   GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX_BYTES128             0x1
#define   GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX_BYTES256             0x2
#define   GCREG_AHBDEC_CONTROL_EX_ENABLE_BURST_SPLIT_EX_RESERVED             0x3

/* [T3] Disable TFC Cache,Available for T3 only */
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TFC                          24:24
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TFC_End                         24
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TFC_Start                       24
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TFC_Type                       U01
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TFC_DISABLE                  0x0
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TFC_ENABLE                   0x1

/* [T3] Disable Address Hash,available for T3 only */
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_ADDRESS_HASH                       25:25
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_ADDRESS_HASH_End                      25
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_ADDRESS_HASH_Start                    25
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_ADDRESS_HASH_Type                    U01
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_ADDRESS_HASH_DISABLE               0x0
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_ADDRESS_HASH_ENABLE                0x1

/* [TGL] Bypass compression for all streams,                                  **
** gcregAHBDECConfig.disableCompression                                       */
#define GCREG_AHBDEC_CONTROL_EX_TGL_DISABLE_COMPRESSION                    26:26
#define GCREG_AHBDEC_CONTROL_EX_TGL_DISABLE_COMPRESSION_End                   26
#define GCREG_AHBDEC_CONTROL_EX_TGL_DISABLE_COMPRESSION_Start                 26
#define GCREG_AHBDEC_CONTROL_EX_TGL_DISABLE_COMPRESSION_Type                 U01
#define   GCREG_AHBDEC_CONTROL_EX_TGL_DISABLE_COMPRESSION_DISABLE            0x0
#define   GCREG_AHBDEC_CONTROL_EX_TGL_DISABLE_COMPRESSION_ENABLE             0x1

/* [T3] Disable TSC Cache,available for T3 only */
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TSC                          27:27
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TSC_End                         27
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TSC_Start                       27
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TSC_Type                       U01
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TSC_DISABLE                  0x0
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_CACHE_TSC_ENABLE                   0x1

/* [T3] Disable DCache,available for T3 only */
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_DCACHE                             28:28
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_DCACHE_End                            28
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_DCACHE_Start                          28
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_DCACHE_Type                          U01
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_DCACHE_DISABLE                     0x0
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_DCACHE_ENABLE                      0x1

/* Define two read miss policies for the TSC.  POLICY0: Normal Fill;          **
** POLICY1: Speculative Concurrent Fill.                                      */
#define GCREG_AHBDEC_CONTROL_EX_READ_MISS_POLICY                           29:29
#define GCREG_AHBDEC_CONTROL_EX_READ_MISS_POLICY_End                          29
#define GCREG_AHBDEC_CONTROL_EX_READ_MISS_POLICY_Start                        29
#define GCREG_AHBDEC_CONTROL_EX_READ_MISS_POLICY_Type                        U01
/* * Normal Fill. If TSC miss, read TSC data, then send data request * */
#define   GCREG_AHBDEC_CONTROL_EX_READ_MISS_POLICY_POLICY0                   0x0
/* * Speculative Concurrent Fill. If TSC miss, send all tile data requests    **
** and send TSC data request. Not currently supported. *                      */
#define   GCREG_AHBDEC_CONTROL_EX_READ_MISS_POLICY_POLICY1                   0x1

/* The memory layout is TileYf TGL - If this bit is 1, TileY must also be 1.  */
#define GCREG_AHBDEC_CONTROL_EX_TILE_YF                                    30:30
#define GCREG_AHBDEC_CONTROL_EX_TILE_YF_End                                   30
#define GCREG_AHBDEC_CONTROL_EX_TILE_YF_Start                                 30
#define GCREG_AHBDEC_CONTROL_EX_TILE_YF_Type                                 U01
#define   GCREG_AHBDEC_CONTROL_EX_TILE_YF_DISABLE                            0x0
#define   GCREG_AHBDEC_CONTROL_EX_TILE_YF_ENABLE                             0x1

/* [K7] Disable the TSC auto evict, available for K7 only. */
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_TSC_AUTO_EVICT                     31:31
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_TSC_AUTO_EVICT_End                    31
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_TSC_AUTO_EVICT_Start                  31
#define GCREG_AHBDEC_CONTROL_EX_DISABLE_TSC_AUTO_EVICT_Type                  U01
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_TSC_AUTO_EVICT_DISABLE             0x0
#define   GCREG_AHBDEC_CONTROL_EX_DISABLE_TSC_AUTO_EVICT_ENABLE              0x1

/* Extra Control register for DEC core. */

#define gcregAHBDECControlEx2RegAddrs                                     0x0202
#define GCREG_AHBDEC_CONTROL_EX2_Address                                 0x00808
#define GCREG_AHBDEC_CONTROL_EX2_MSB                                          15
#define GCREG_AHBDEC_CONTROL_EX2_LSB                                           0
#define GCREG_AHBDEC_CONTROL_EX2_BLK                                           0
#define GCREG_AHBDEC_CONTROL_EX2_Count                                         1
#define GCREG_AHBDEC_CONTROL_EX2_FieldMask                            0xFFFFFFFF
#define GCREG_AHBDEC_CONTROL_EX2_ReadMask                             0xFFFFFFFF
#define GCREG_AHBDEC_CONTROL_EX2_WriteMask                            0x033FFFFF
#define GCREG_AHBDEC_CONTROL_EX2_ResetValue                           0x003FC810

/* Tile status cache's AXI read ID. */
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_READ_ID                         6:0
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_READ_ID_End                       6
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_READ_ID_Start                     0
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_READ_ID_Type                    U07

/* Tile status cache's AXI write ID. */
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_WRITE_ID                       13:7
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_WRITE_ID_End                     13
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_WRITE_ID_Start                    7
#define GCREG_AHBDEC_CONTROL_EX2_TILE_STATUS_WRITE_ID_Type                   U07

/* Write OT count limitation. */
#define GCREG_AHBDEC_CONTROL_EX2_WR_OT_CNT                                 21:14
#define GCREG_AHBDEC_CONTROL_EX2_WR_OT_CNT_End                                21
#define GCREG_AHBDEC_CONTROL_EX2_WR_OT_CNT_Start                              14
#define GCREG_AHBDEC_CONTROL_EX2_WR_OT_CNT_Type                              U08
#define   GCREG_AHBDEC_CONTROL_EX2_WR_OT_CNT_RESET                          0xFF

#define GCREG_AHBDEC_CONTROL_EX2_RESERVE0                                  23:22
#define GCREG_AHBDEC_CONTROL_EX2_RESERVE0_End                                 23
#define GCREG_AHBDEC_CONTROL_EX2_RESERVE0_Start                               22
#define GCREG_AHBDEC_CONTROL_EX2_RESERVE0_Type                               U02

/* Enable the TSC hit and miss separating request. */
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_TSC_HIT_MISS_SEPARATE_REQUEST      24:24
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_TSC_HIT_MISS_SEPARATE_REQUEST_End     24
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_TSC_HIT_MISS_SEPARATE_REQUEST_Start   24
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_TSC_HIT_MISS_SEPARATE_REQUEST_Type   U01
#define   GCREG_AHBDEC_CONTROL_EX2_ENABLE_TSC_HIT_MISS_SEPARATE_REQUEST_DISABLE 0x0
#define   GCREG_AHBDEC_CONTROL_EX2_ENABLE_TSC_HIT_MISS_SEPARATE_REQUEST_ENABLE 0x1

/* Enable the global compression ratio statistics. */
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_GLOBAL_CR_STATISTICS               25:25
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_GLOBAL_CR_STATISTICS_End              25
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_GLOBAL_CR_STATISTICS_Start            25
#define GCREG_AHBDEC_CONTROL_EX2_ENABLE_GLOBAL_CR_STATISTICS_Type            U01
#define   GCREG_AHBDEC_CONTROL_EX2_ENABLE_GLOBAL_CR_STATISTICS_DISABLE       0x0
#define   GCREG_AHBDEC_CONTROL_EX2_ENABLE_GLOBAL_CR_STATISTICS_ENABLE        0x1

/* Specify the stream ID which is between 0 ~ 31 to probe. K7 - Available for **
** K7.                                                                        */
#define GCREG_AHBDEC_CONTROL_EX2_PROBE_STREAM_ID                           30:26
#define GCREG_AHBDEC_CONTROL_EX2_PROBE_STREAM_ID_End                          30
#define GCREG_AHBDEC_CONTROL_EX2_PROBE_STREAM_ID_Start                        26
#define GCREG_AHBDEC_CONTROL_EX2_PROBE_STREAM_ID_Type                        U05

/* Control the Tile Status Cache mapping pattern.  Mode0:Use the LSB 5 bits   **
** of the address to map the cache set;  Mode1:Add the LSB 5 bits of the      **
** address and the Stream ID to map the cache set. K7 - Available for K7.     */
#define GCREG_AHBDEC_CONTROL_EX2_TS_CACHE_MAP_MODE                         31:31
#define GCREG_AHBDEC_CONTROL_EX2_TS_CACHE_MAP_MODE_End                        31
#define GCREG_AHBDEC_CONTROL_EX2_TS_CACHE_MAP_MODE_Start                      31
#define GCREG_AHBDEC_CONTROL_EX2_TS_CACHE_MAP_MODE_Type                      U01
#define   GCREG_AHBDEC_CONTROL_EX2_TS_CACHE_MAP_MODE_MODE0                   0x0
#define   GCREG_AHBDEC_CONTROL_EX2_TS_CACHE_MAP_MODE_MODE1                   0x1

/* Register gcregAHBDECIntrEnblEx2 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Interrupt enable register. Each bit enables a corresponding event.  */

#define gcregAHBDECIntrEnblEx2RegAddrs                                    0x0205
#define GCREG_AHBDEC_INTR_ENBL_EX2_Address                               0x00814
#define GCREG_AHBDEC_INTR_ENBL_EX2_MSB                                        15
#define GCREG_AHBDEC_INTR_ENBL_EX2_LSB                                         0
#define GCREG_AHBDEC_INTR_ENBL_EX2_BLK                                         0
#define GCREG_AHBDEC_INTR_ENBL_EX2_Count                                       1
#define GCREG_AHBDEC_INTR_ENBL_EX2_FieldMask                          0xFFFFFFFF
#define GCREG_AHBDEC_INTR_ENBL_EX2_ReadMask                           0xFFFFFFFF
#define GCREG_AHBDEC_INTR_ENBL_EX2_WriteMask                          0xFFFFFFFF
#define GCREG_AHBDEC_INTR_ENBL_EX2_ResetValue                         0x00000000

/* 0: Disable interrupt, 1: Enable interrupt.  Bit 0 is flush done interrupt  **
** for all streams.                                                           */
#define GCREG_AHBDEC_INTR_ENBL_EX2_INTR_ENBL_VEC                            31:0
#define GCREG_AHBDEC_INTR_ENBL_EX2_INTR_ENBL_VEC_End                          31
#define GCREG_AHBDEC_INTR_ENBL_EX2_INTR_ENBL_VEC_Start                         0
#define GCREG_AHBDEC_INTR_ENBL_EX2_INTR_ENBL_VEC_Type                        U32

/* Register gcregAHBDECIntrAcknowledgeEx2 **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Interrupt acknowledge register. Each bit represents a  corresponding event **
** being triggered. Reading from this  register clears the outstanding        **
** interrupt.  READ ONLY.                                                     */

#define gcregAHBDECIntrAcknowledgeEx2RegAddrs                             0x0208
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_Address                        0x00820
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_MSB                                 15
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_LSB                                  0
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_BLK                                  0
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_Count                                1
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_FieldMask                   0xFFFFFFFF
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_ReadMask                    0xFFFFFFFF
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_WriteMask                   0x00000000
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_ResetValue                  0x00000000

/* For each interrupt event, 0=Clear, 1=Interrupt Active.  Bit 0 is flush     **
** done interrupt for all streams.                                            */
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_INTR_VEC                          31:0
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_INTR_VEC_End                        31
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_INTR_VEC_Start                       0
#define GCREG_AHBDEC_INTR_ACKNOWLEDGE_EX2_INTR_VEC_Type                      U32

/* Register gcregAHBDECReadExConfig (32 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DEC extra configuration register for read ID. */

#define gcregAHBDECReadExConfigRegAddrs                                   0x0240
#define GCREG_AHBDEC_READ_EX_CONFIG_Address                              0x00900
#define GCREG_AHBDEC_READ_EX_CONFIG_MSB                                       15
#define GCREG_AHBDEC_READ_EX_CONFIG_LSB                                        5
#define GCREG_AHBDEC_READ_EX_CONFIG_BLK                                        5
#define GCREG_AHBDEC_READ_EX_CONFIG_Count                                     32
#define GCREG_AHBDEC_READ_EX_CONFIG_FieldMask                         0x3DFFFFFF
#define GCREG_AHBDEC_READ_EX_CONFIG_ReadMask                          0x3DFFFFFF
#define GCREG_AHBDEC_READ_EX_CONFIG_WriteMask                         0x00070001
#define GCREG_AHBDEC_READ_EX_CONFIG_ResetValue                        0x00000000

/* Return data of read request goes to TPC decoder. This is obsolete for      **
** dec400.                                                                    */
#define GCREG_AHBDEC_READ_EX_CONFIG_TPC                                      0:0
#define GCREG_AHBDEC_READ_EX_CONFIG_TPC_End                                    0
#define GCREG_AHBDEC_READ_EX_CONFIG_TPC_Start                                  0
#define GCREG_AHBDEC_READ_EX_CONFIG_TPC_Type                                 U01
#define   GCREG_AHBDEC_READ_EX_CONFIG_TPC_DISABLE                            0x0
#define   GCREG_AHBDEC_READ_EX_CONFIG_TPC_ENABLE                             0x1

#define GCREG_AHBDEC_READ_EX_CONFIG_RESERVE0                                 2:1
#define GCREG_AHBDEC_READ_EX_CONFIG_RESERVE0_End                               2
#define GCREG_AHBDEC_READ_EX_CONFIG_RESERVE0_Start                             1
#define GCREG_AHBDEC_READ_EX_CONFIG_RESERVE0_Type                            U02

/* For CBSR Align mode, it indicate Y CBS number included in one CBSR since   **
** CBSG_SADDR field in CBSG table  entry is based from each CBSR start        **
** address. Also used to judge whether CbCr  needs decompression if Y is in   **
** odd CBS number.                                                            */
#define GCREG_AHBDEC_READ_EX_CONFIG_CBSR_WIDTH                              15:3
#define GCREG_AHBDEC_READ_EX_CONFIG_CBSR_WIDTH_End                            15
#define GCREG_AHBDEC_READ_EX_CONFIG_CBSR_WIDTH_Start                           3
#define GCREG_AHBDEC_READ_EX_CONFIG_CBSR_WIDTH_Type                          U13

/* Bit depth for Y/UV/Bayer format.  The values 3/4 are not available for all **
** variants.                                                                  */
#define GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH                              18:16
#define GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_End                             18
#define GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_Start                           16
#define GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_Type                           U03
#define   GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_BIT8                         0x0
#define   GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_BIT10                        0x1
#define   GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_BIT12                        0x2
#define   GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_BIT14                        0x3
#define   GCREG_AHBDEC_READ_EX_CONFIG_BIT_DEPTH_BIT16                        0x4

/* The memory layout is TileY */
#define GCREG_AHBDEC_READ_EX_CONFIG_TILE_Y                                 19:19
#define GCREG_AHBDEC_READ_EX_CONFIG_TILE_Y_End                                19
#define GCREG_AHBDEC_READ_EX_CONFIG_TILE_Y_Start                              19
#define GCREG_AHBDEC_READ_EX_CONFIG_TILE_Y_Type                              U01
#define   GCREG_AHBDEC_READ_EX_CONFIG_TILE_Y_DISABLE                         0x0
#define   GCREG_AHBDEC_READ_EX_CONFIG_TILE_Y_ENABLE                          0x1

/* mode 1-9 only valid for DEC_IPU mode 10-13 only valid for DEC_VPU mode     **
** 14-19 only valid for DEC_XYZ Stream mode enumerates the possible streams   **
** sent by clients. In addition to the basic format configured for            **
** compression, streams have additional characteristics, such as reading or   **
** writing walking pattern, which distinguish one type from another. TGL -    **
** Available for T2D                                                          */
#define GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE                            24:20
#define GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_End                           24
#define GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_Start                         20
#define GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_Type                         U05
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_DEFAULT                   0x00
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_ISA_STREAM0               0x01
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_ISA_STREAM1               0x02
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_ISA_STREAM2               0x03
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_ISA_STREAM3               0x04
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_TNR_STREAM_Y              0x05
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_TNR_STREAM_UV             0x06
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_GDC_STREAM_Y              0x07
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_GDC_STREAM_U              0x08
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_GDC_STREAM_V              0x09
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_VPU_SRC_Y                 0x0A
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_VPU_SRC_UV                0x0B
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_VPU_REF_Y                 0x0C
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_VPU_REF_UV                0x0D
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_XYZ_STREAM_AY             0x0E
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_XYZ_STREAM_AU             0x0F
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_XYZ_STREAM_AV             0x10
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_XYZ_STREAM_BY             0x11
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_XYZ_STREAM_BU             0x12
#define   GCREG_AHBDEC_READ_EX_CONFIG_STREAM_MODE_XYZ_STREAM_BV             0x13

/* By default, the TS cache for write client is write only cache. if set this **
** bit, the cache will read back the cache line first, then modify it not     **
** valid for the read ID TGL - Available for T2D                              */
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_READ_MODE                     26:26
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_READ_MODE_End                    26
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_READ_MODE_Start                  26
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_READ_MODE_Type                  U01
#define   GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_READ_MODE_DISABLE             0x0
#define   GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_READ_MODE_ENABLE              0x1

/* Two replacement method support for pixel overfetch cache TGL - Available   **
** for T2D                                                                    */
#define GCREG_AHBDEC_READ_EX_CONFIG_PIXEL_CACHE_REPLACEMENT                27:27
#define GCREG_AHBDEC_READ_EX_CONFIG_PIXEL_CACHE_REPLACEMENT_End               27
#define GCREG_AHBDEC_READ_EX_CONFIG_PIXEL_CACHE_REPLACEMENT_Start             27
#define GCREG_AHBDEC_READ_EX_CONFIG_PIXEL_CACHE_REPLACEMENT_Type             U01
#define   GCREG_AHBDEC_READ_EX_CONFIG_PIXEL_CACHE_REPLACEMENT_LRU            0x0
#define   GCREG_AHBDEC_READ_EX_CONFIG_PIXEL_CACHE_REPLACEMENT_FIFO           0x1

/* TGL: Applicable for custom P010 format for 10bit */
#define GCREG_AHBDEC_READ_EX_CONFIG_INTEL_P010                             28:28
#define GCREG_AHBDEC_READ_EX_CONFIG_INTEL_P010_End                            28
#define GCREG_AHBDEC_READ_EX_CONFIG_INTEL_P010_Start                          28
#define GCREG_AHBDEC_READ_EX_CONFIG_INTEL_P010_Type                          U01
#define   GCREG_AHBDEC_READ_EX_CONFIG_INTEL_P010_DISABLE                     0x0
#define   GCREG_AHBDEC_READ_EX_CONFIG_INTEL_P010_ENABLE                      0x1

/* Two replacement method support for tile status cache TGL - Available for   **
** T2D                                                                        */
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_REPLACEMENT                   29:29
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_REPLACEMENT_End                  29
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_REPLACEMENT_Start                29
#define GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_REPLACEMENT_Type                U01
#define   GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_REPLACEMENT_LRU               0x0
#define   GCREG_AHBDEC_READ_EX_CONFIG_TS_CACHE_REPLACEMENT_FIFO              0x1

/* Register gcregAHBDECReadBufferBase (32 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Base address low 32 bit for pixel data buffer for read ID. */

#define gcregAHBDECReadBufferBaseRegAddrs                                 0x02A0
#define GCREG_AHBDEC_READ_BUFFER_BASE_Address                            0x00A80
#define GCREG_AHBDEC_READ_BUFFER_BASE_MSB                                     15
#define GCREG_AHBDEC_READ_BUFFER_BASE_LSB                                      5
#define GCREG_AHBDEC_READ_BUFFER_BASE_BLK                                      5
#define GCREG_AHBDEC_READ_BUFFER_BASE_Count                                   32
#define GCREG_AHBDEC_READ_BUFFER_BASE_FieldMask                       0xFFFFFFFF
#define GCREG_AHBDEC_READ_BUFFER_BASE_ReadMask                        0xFFFFFFFF
#define GCREG_AHBDEC_READ_BUFFER_BASE_WriteMask                       0xFFFFFFFF
#define GCREG_AHBDEC_READ_BUFFER_BASE_ResetValue                      0xFFFFFFFF

#define GCREG_AHBDEC_READ_BUFFER_BASE_ADDRESS                               31:0
#define GCREG_AHBDEC_READ_BUFFER_BASE_ADDRESS_End                             31
#define GCREG_AHBDEC_READ_BUFFER_BASE_ADDRESS_Start                            0
#define GCREG_AHBDEC_READ_BUFFER_BASE_ADDRESS_Type                           U32

/* Register gcregAHBDECReadBufferEnd (32 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* End address for pixel buffer for read ID.  Pixel and tile status cache     **
** invalidation is  triggered automatically when address of a  request        **
** matches this address.                                                      */

#define gcregAHBDECReadBufferEndRegAddrs                                  0x02E0
#define GCREG_AHBDEC_READ_BUFFER_END_Address                             0x00B80
#define GCREG_AHBDEC_READ_BUFFER_END_MSB                                      15
#define GCREG_AHBDEC_READ_BUFFER_END_LSB                                       5
#define GCREG_AHBDEC_READ_BUFFER_END_BLK                                       5
#define GCREG_AHBDEC_READ_BUFFER_END_Count                                    32
#define GCREG_AHBDEC_READ_BUFFER_END_FieldMask                        0xFFFFFFFF
#define GCREG_AHBDEC_READ_BUFFER_END_ReadMask                         0xFFFFFFFF
#define GCREG_AHBDEC_READ_BUFFER_END_WriteMask                        0xFFFFFFFF
#define GCREG_AHBDEC_READ_BUFFER_END_ResetValue                       0xFFFFFFFF

#define GCREG_AHBDEC_READ_BUFFER_END_ADDRESS                                31:0
#define GCREG_AHBDEC_READ_BUFFER_END_ADDRESS_End                              31
#define GCREG_AHBDEC_READ_BUFFER_END_ADDRESS_Start                             0
#define GCREG_AHBDEC_READ_BUFFER_END_ADDRESS_Type                            U32

/* Register gcregAHBDECReadCacheBase (32 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Base address low 32 bit for tile status cache (TSC) buffer for read ID.  */

#define gcregAHBDECReadCacheBaseRegAddrs                                  0x0420
#define GCREG_AHBDEC_READ_CACHE_BASE_Address                             0x01080
#define GCREG_AHBDEC_READ_CACHE_BASE_MSB                                      15
#define GCREG_AHBDEC_READ_CACHE_BASE_LSB                                       5
#define GCREG_AHBDEC_READ_CACHE_BASE_BLK                                       5
#define GCREG_AHBDEC_READ_CACHE_BASE_Count                                    32
#define GCREG_AHBDEC_READ_CACHE_BASE_FieldMask                        0xFFFFFFFF
#define GCREG_AHBDEC_READ_CACHE_BASE_ReadMask                         0xFFFFFFFF
#define GCREG_AHBDEC_READ_CACHE_BASE_WriteMask                        0xFFFFFFFF
#define GCREG_AHBDEC_READ_CACHE_BASE_ResetValue                       0x00000000

#define GCREG_AHBDEC_READ_CACHE_BASE_ADDRESS                                31:0
#define GCREG_AHBDEC_READ_CACHE_BASE_ADDRESS_End                              31
#define GCREG_AHBDEC_READ_CACHE_BASE_ADDRESS_Start                             0
#define GCREG_AHBDEC_READ_CACHE_BASE_ADDRESS_Type                            U32

/* Register gcregAHBDECFastClearValue (32 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DEC Fast Clear Value Configuration Register.  Fast clear value             **
** configuration for each stream.                                             */

#define gcregAHBDECFastClearValueRegAddrs                                 0x04C0
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_Address                            0x01300
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_MSB                                     15
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_LSB                                      5
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_BLK                                      5
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_Count                                   32
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_FieldMask                       0xFFFFFFFF
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_ReadMask                        0xFFFFFFFF
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_WriteMask                       0xFFFFFFFF
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_ResetValue                      0x00000000

/* Fast clear value configuration.  */
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_FAST_CLEAR_VALUE                      31:0
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_FAST_CLEAR_VALUE_End                    31
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_FAST_CLEAR_VALUE_Start                   0
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_FAST_CLEAR_VALUE_Type                  U32

/* Register gcregAHBDECFastClearValueEx (32 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DEC Fast Clear Value Configuration Upper Bits Extension Register.  High    **
** 32-bit fast clear value configuration for each stream.                     */

#define gcregAHBDECFastClearValueExRegAddrs                               0x04E0
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_Address                         0x01380
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_MSB                                  15
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_LSB                                   5
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_BLK                                   5
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_Count                                32
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_FieldMask                    0xFFFFFFFF
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_ReadMask                     0xFFFFFFFF
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_WriteMask                    0xFFFFFFFF
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_ResetValue                   0x00000000

/* High 32-bit fast clear value configuration.  */
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_FAST_CLEAR_VALUE_EX                31:0
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_FAST_CLEAR_VALUE_EX_End              31
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_FAST_CLEAR_VALUE_EX_Start             0
#define GCREG_AHBDEC_FAST_CLEAR_VALUE_EX_FAST_CLEAR_VALUE_EX_Type            U32


/*******************************************************************************
**                        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                       **
**                        Module DisplayControllerUltra                       **
**                        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                       **
*******************************************************************************/

/* Register dcregFrameBufferAddress (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Starting address of the frame buffer. NOTE: This register is double        **
** buffered.                                                                  */

#define dcregFrameBufferAddressRegAddrs                                   0x0500
#define DCREG_FRAME_BUFFER_ADDRESS_Address                               0x01400
#define DCREG_FRAME_BUFFER_ADDRESS_MSB                                        15
#define DCREG_FRAME_BUFFER_ADDRESS_LSB                                         1
#define DCREG_FRAME_BUFFER_ADDRESS_BLK                                         1
#define DCREG_FRAME_BUFFER_ADDRESS_Count                                       2
#define DCREG_FRAME_BUFFER_ADDRESS_FieldMask                          0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ADDRESS_ReadMask                           0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ADDRESS_WriteMask                          0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ADDRESS_ResetValue                         0x00000000

#define DCREG_FRAME_BUFFER_ADDRESS_ADDRESS                                  31:0
#define DCREG_FRAME_BUFFER_ADDRESS_ADDRESS_End                                31
#define DCREG_FRAME_BUFFER_ADDRESS_ADDRESS_Start                               0
#define DCREG_FRAME_BUFFER_ADDRESS_ADDRESS_Type                              U32

/* Register dcregFrameBufferStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Stride of the frame buffer in bytes. NOTE: This register is double         **
** buffered.                                                                  */

#define dcregFrameBufferStrideRegAddrs                                    0x0502
#define DCREG_FRAME_BUFFER_STRIDE_Address                                0x01408
#define DCREG_FRAME_BUFFER_STRIDE_MSB                                         15
#define DCREG_FRAME_BUFFER_STRIDE_LSB                                          1
#define DCREG_FRAME_BUFFER_STRIDE_BLK                                          1
#define DCREG_FRAME_BUFFER_STRIDE_Count                                        2
#define DCREG_FRAME_BUFFER_STRIDE_FieldMask                           0x0003FFFF
#define DCREG_FRAME_BUFFER_STRIDE_ReadMask                            0x0003FFFF
#define DCREG_FRAME_BUFFER_STRIDE_WriteMask                           0x0003FFFF
#define DCREG_FRAME_BUFFER_STRIDE_ResetValue                          0x00000000

/* Number of bytes from start of one line to next line. */
#define DCREG_FRAME_BUFFER_STRIDE_STRIDE                                    17:0
#define DCREG_FRAME_BUFFER_STRIDE_STRIDE_End                                  17
#define DCREG_FRAME_BUFFER_STRIDE_STRIDE_Start                                 0
#define DCREG_FRAME_BUFFER_STRIDE_STRIDE_Type                                U18

/* Register dcregDisplayDitherConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Configuration register for dithering. NOTE: This register is double        **
** buffered.                                                                  */

#define dcregDisplayDitherConfigRegAddrs                                  0x0504
#define DCREG_DISPLAY_DITHER_CONFIG_Address                              0x01410
#define DCREG_DISPLAY_DITHER_CONFIG_MSB                                       15
#define DCREG_DISPLAY_DITHER_CONFIG_LSB                                        1
#define DCREG_DISPLAY_DITHER_CONFIG_BLK                                        1
#define DCREG_DISPLAY_DITHER_CONFIG_Count                                      2
#define DCREG_DISPLAY_DITHER_CONFIG_FieldMask                         0x80000000
#define DCREG_DISPLAY_DITHER_CONFIG_ReadMask                          0x80000000
#define DCREG_DISPLAY_DITHER_CONFIG_WriteMask                         0x80000000
#define DCREG_DISPLAY_DITHER_CONFIG_ResetValue                        0x00000000

/* Enabling dithering allows R8G8B8 modes to show better on panels with less  **
** bits-per-pixel. NOTE: This field is double buffered.                       */
#define DCREG_DISPLAY_DITHER_CONFIG_ENABLE                                 31:31
#define DCREG_DISPLAY_DITHER_CONFIG_ENABLE_End                                31
#define DCREG_DISPLAY_DITHER_CONFIG_ENABLE_Start                              31
#define DCREG_DISPLAY_DITHER_CONFIG_ENABLE_Type                              U01
#define DCREG_DISPLAY_DITHER_CONFIG_ENABLE_DISABLED                        0x0
#define DCREG_DISPLAY_DITHER_CONFIG_ENABLE_ENABLED                         0x1

/* Register dcregPanelConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Panel configuration. NOTE: This register is double buffered. */

#define dcregPanelConfigRegAddrs                                          0x0506
#define DCREG_PANEL_CONFIG_Address                                       0x01418
#define DCREG_PANEL_CONFIG_MSB                                                15
#define DCREG_PANEL_CONFIG_LSB                                                 1
#define DCREG_PANEL_CONFIG_BLK                                                 1
#define DCREG_PANEL_CONFIG_Count                                               2
#define DCREG_PANEL_CONFIG_FieldMask                                  0x00000333
#define DCREG_PANEL_CONFIG_ReadMask                                   0x00000333
#define DCREG_PANEL_CONFIG_WriteMask                                  0x00000333
#define DCREG_PANEL_CONFIG_ResetValue                                 0x00000100

/* Data Enable enabled/disbaled. */
#define DCREG_PANEL_CONFIG_DE                                                0:0
#define DCREG_PANEL_CONFIG_DE_End                                              0
#define DCREG_PANEL_CONFIG_DE_Start                                            0
#define DCREG_PANEL_CONFIG_DE_Type                                           U01
#define   DCREG_PANEL_CONFIG_DE_DISABLED                                     0x0
#define   DCREG_PANEL_CONFIG_DE_ENABLED                                      0x1

/* Data Enable polarity. */
#define DCREG_PANEL_CONFIG_DE_POLARITY                                       1:1
#define DCREG_PANEL_CONFIG_DE_POLARITY_End                                     1
#define DCREG_PANEL_CONFIG_DE_POLARITY_Start                                   1
#define DCREG_PANEL_CONFIG_DE_POLARITY_Type                                  U01
#define   DCREG_PANEL_CONFIG_DE_POLARITY_POSITIVE                            0x0
#define   DCREG_PANEL_CONFIG_DE_POLARITY_NEGATIVE                            0x1

/* Data enabled/disbaled. */
#define DCREG_PANEL_CONFIG_DATA_ENABLE                                       4:4
#define DCREG_PANEL_CONFIG_DATA_ENABLE_End                                     4
#define DCREG_PANEL_CONFIG_DATA_ENABLE_Start                                   4
#define DCREG_PANEL_CONFIG_DATA_ENABLE_Type                                  U01
#define   DCREG_PANEL_CONFIG_DATA_ENABLE_DISABLED                            0x0
#define   DCREG_PANEL_CONFIG_DATA_ENABLE_ENABLED                             0x1

/* Data polarity. */
#define DCREG_PANEL_CONFIG_DATA_POLARITY                                     5:5
#define DCREG_PANEL_CONFIG_DATA_POLARITY_End                                   5
#define DCREG_PANEL_CONFIG_DATA_POLARITY_Start                                 5
#define DCREG_PANEL_CONFIG_DATA_POLARITY_Type                                U01
#define   DCREG_PANEL_CONFIG_DATA_POLARITY_POSITIVE                          0x0
#define   DCREG_PANEL_CONFIG_DATA_POLARITY_NEGATIVE                          0x1

/* Clock enabled/disbaled. */
#define DCREG_PANEL_CONFIG_CLOCK                                             8:8
#define DCREG_PANEL_CONFIG_CLOCK_End                                           8
#define DCREG_PANEL_CONFIG_CLOCK_Start                                         8
#define DCREG_PANEL_CONFIG_CLOCK_Type                                        U01
#define   DCREG_PANEL_CONFIG_CLOCK_DISABLED                                  0x0
#define   DCREG_PANEL_CONFIG_CLOCK_ENABLED                                   0x1

/* Clock polarity. */
#define DCREG_PANEL_CONFIG_CLOCK_POLARITY                                    9:9
#define DCREG_PANEL_CONFIG_CLOCK_POLARITY_End                                  9
#define DCREG_PANEL_CONFIG_CLOCK_POLARITY_Start                                9
#define DCREG_PANEL_CONFIG_CLOCK_POLARITY_Type                               U01
#define   DCREG_PANEL_CONFIG_CLOCK_POLARITY_POSITIVE                         0x0
#define   DCREG_PANEL_CONFIG_CLOCK_POLARITY_NEGATIVE                         0x1

/* Register dcregDisplayDitherTableLow (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Dither table. NOTE: This register is double buffered. */

#define dcregDisplayDitherTableLowRegAddrs                                0x0508
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Address                           0x01420
#define DCREG_DISPLAY_DITHER_TABLE_LOW_MSB                                    15
#define DCREG_DISPLAY_DITHER_TABLE_LOW_LSB                                     1
#define DCREG_DISPLAY_DITHER_TABLE_LOW_BLK                                     1
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Count                                   2
#define DCREG_DISPLAY_DITHER_TABLE_LOW_FieldMask                      0xFFFFFFFF
#define DCREG_DISPLAY_DITHER_TABLE_LOW_ReadMask                       0xFFFFFFFF
#define DCREG_DISPLAY_DITHER_TABLE_LOW_WriteMask                      0xFFFFFFFF
#define DCREG_DISPLAY_DITHER_TABLE_LOW_ResetValue                     0x00000000

/* Dither threshold value for x,y=0,0. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X0                                 3:0
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X0_End                               3
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X0_Start                             0
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X0_Type                            U04

/* Dither threshold value for x,y=1,0. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X1                                 7:4
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X1_End                               7
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X1_Start                             4
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X1_Type                            U04

/* Dither threshold value for x,y=2,0. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X2                                11:8
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X2_End                              11
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X2_Start                             8
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X2_Type                            U04

/* Dither threshold value for x,y=3,0. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X3                               15:12
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X3_End                              15
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X3_Start                            12
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y0_X3_Type                            U04

/* Dither threshold value for x,y=0,1. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X0                               19:16
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X0_End                              19
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X0_Start                            16
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X0_Type                            U04

/* Dither threshold value for x,y=1,1. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X1                               23:20
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X1_End                              23
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X1_Start                            20
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X1_Type                            U04

/* Dither threshold value for x,y=2,1. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X2                               27:24
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X2_End                              27
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X2_Start                            24
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X2_Type                            U04

/* Dither threshold value for x,y=3,1. */
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X3                               31:28
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X3_End                              31
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X3_Start                            28
#define DCREG_DISPLAY_DITHER_TABLE_LOW_Y1_X3_Type                            U04

/* Register dcregDisplayDitherTableHigh (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregDisplayDitherTableHighRegAddrs                               0x050A
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Address                          0x01428
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_MSB                                   15
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_LSB                                    1
#define DCREG_DISPLAY_DITHER_TABLE_LOW_HIGH_BLK                                1
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Count                                  2
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_FieldMask                     0xFFFFFFFF
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_ReadMask                      0xFFFFFFFF
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_WriteMask                     0xFFFFFFFF
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_ResetValue                    0x00000000

/* Dither threshold value for x,y=0,2. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X0                                3:0
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X0_End                              3
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X0_Start                            0
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X0_Type                           U04

/* Dither threshold value for x,y=1,2. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X1                                7:4
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X1_End                              7
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X1_Start                            4
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X1_Type                           U04

/* Dither threshold value for x,y=2,2. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X2                               11:8
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X2_End                             11
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X2_Start                            8
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X2_Type                           U04

/* Dither threshold value for x,y=3,2. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X3                              15:12
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X3_End                             15
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X3_Start                           12
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y2_X3_Type                           U04

/* Dither threshold value for x,y=0,3. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X0                              19:16
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X0_End                             19
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X0_Start                           16
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X0_Type                           U04

/* Dither threshold value for x,y=1,3. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X1                              23:20
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X1_End                             23
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X1_Start                           20
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X1_Type                           U04

/* Dither threshold value for x,y=2,3. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X2                              27:24
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X2_End                             27
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X2_Start                           24
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X2_Type                           U04

/* Dither threshold value for x,y=3,3. */
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X3                              31:28
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X3_End                             31
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X3_Start                           28
#define DCREG_DISPLAY_DITHER_TABLE_HIGH_Y3_X3_Type                           U04

/* Register dcregHDisplay (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Horizontal Total and Display End counters. NOTE: This register is double   **
** buffered.                                                                  */

#define dcregHDisplayRegAddrs                                             0x050C
#define DCREG_HDISPLAY_Address                                           0x01430
#define DCREG_HDISPLAY_MSB                                                    15
#define DCREG_HDISPLAY_LSB                                                     1
#define DCREG_HDISPLAY_BLK                                                     1
#define DCREG_HDISPLAY_Count                                                   2
#define DCREG_HDISPLAY_FieldMask                                      0x7FFF7FFF
#define DCREG_HDISPLAY_ReadMask                                       0x7FFF7FFF
#define DCREG_HDISPLAY_WriteMask                                      0x7FFF7FFF
#define DCREG_HDISPLAY_ResetValue                                     0x00000000

/* Visible number of horizontal pixels. */
#define DCREG_HDISPLAY_DISPLAY_END                                          14:0
#define DCREG_HDISPLAY_DISPLAY_END_End                                        14
#define DCREG_HDISPLAY_DISPLAY_END_Start                                       0
#define DCREG_HDISPLAY_DISPLAY_END_Type                                      U15

/* Total number of horizontal pixels. */
#define DCREG_HDISPLAY_TOTAL                                               30:16
#define DCREG_HDISPLAY_TOTAL_End                                              30
#define DCREG_HDISPLAY_TOTAL_Start                                            16
#define DCREG_HDISPLAY_TOTAL_Type                                            U15

/* Register dcregHSync (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Horizontal Sync counters.
NOTE: This register is double buffered. */

#define dcregHSyncRegAddrs                                                0x050E
#define DCREG_HSYNC_Address                                              0x01438
#define DCREG_HSYNC_MSB                                                       15
#define DCREG_HSYNC_LSB                                                        1
#define DCREG_HSYNC_BLK                                                        1
#define DCREG_HSYNC_Count                                                      2
#define DCREG_HSYNC_FieldMask                                         0xFFFFFFFF
#define DCREG_HSYNC_ReadMask                                          0xFFFFFFFF
#define DCREG_HSYNC_WriteMask                                         0xFFFFFFFF
#define DCREG_HSYNC_ResetValue                                        0x00000000

/* Start of horizontal sync pulse. */
#define DCREG_HSYNC_START                                                   14:0
#define DCREG_HSYNC_START_End                                                 14
#define DCREG_HSYNC_START_Start                                                0
#define DCREG_HSYNC_START_Type                                               U15

/* End of horizontal sync pulse. */
#define DCREG_HSYNC_END                                                    29:15
#define DCREG_HSYNC_END_End                                                   29
#define DCREG_HSYNC_END_Start                                                 15
#define DCREG_HSYNC_END_Type                                                 U15

/* Horizontal sync pulse control. */
#define DCREG_HSYNC_PULSE                                                  30:30
#define DCREG_HSYNC_PULSE_End                                                 30
#define DCREG_HSYNC_PULSE_Start                                               30
#define DCREG_HSYNC_PULSE_Type                                               U01
#define   DCREG_HSYNC_PULSE_DISABLED                                         0x0
#define   DCREG_HSYNC_PULSE_ENABLED                                          0x1

/* Polarity of the horizontal sync pulse. */
#define DCREG_HSYNC_POLARITY                                               31:31
#define DCREG_HSYNC_POLARITY_End                                              31
#define DCREG_HSYNC_POLARITY_Start                                            31
#define DCREG_HSYNC_POLARITY_Type                                            U01
#define   DCREG_HSYNC_POLARITY_POSITIVE                                      0x0
#define   DCREG_HSYNC_POLARITY_NEGATIVE                                      0x1

/* Register dcregVDisplay (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Vertical Total and Display End counters.                                   **
** NOTE: This register is double buffered.                                    */

#define dcregVDisplayRegAddrs                                             0x0510
#define DCREG_VDISPLAY_Address                                           0x01440
#define DCREG_VDISPLAY_MSB                                                    15
#define DCREG_VDISPLAY_LSB                                                     1
#define DCREG_VDISPLAY_BLK                                                     1
#define DCREG_VDISPLAY_Count                                                   2
#define DCREG_VDISPLAY_FieldMask                                      0x7FFF7FFF
#define DCREG_VDISPLAY_ReadMask                                       0x7FFF7FFF
#define DCREG_VDISPLAY_WriteMask                                      0x7FFF7FFF
#define DCREG_VDISPLAY_ResetValue                                     0x00000000

/* Visible number of vertical lines. */
#define DCREG_VDISPLAY_DISPLAY_END                                          14:0
#define DCREG_VDISPLAY_DISPLAY_END_End                                        14
#define DCREG_VDISPLAY_DISPLAY_END_Start                                       0
#define DCREG_VDISPLAY_DISPLAY_END_Type                                      U15

/* Total number of vertical lines. */
#define DCREG_VDISPLAY_TOTAL                                               30:16
#define DCREG_VDISPLAY_TOTAL_End                                              30
#define DCREG_VDISPLAY_TOTAL_Start                                            16
#define DCREG_VDISPLAY_TOTAL_Type                                            U15

/* Register dcregVSync (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Vertical Sync counters.
NOTE: This register is double buffered. */

#define dcregVSyncRegAddrs                                                0x0512
#define DCREG_VSYNC_Address                                              0x01448
#define DCREG_VSYNC_MSB                                                       15
#define DCREG_VSYNC_LSB                                                        1
#define DCREG_VSYNC_BLK                                                        1
#define DCREG_VSYNC_Count                                                      2
#define DCREG_VSYNC_FieldMask                                         0xFFFFFFFF
#define DCREG_VSYNC_ReadMask                                          0xFFFFFFFF
#define DCREG_VSYNC_WriteMask                                         0xFFFFFFFF
#define DCREG_VSYNC_ResetValue                                        0x00000000

/* Start of the vertical sync pulse. */
#define DCREG_VSYNC_START                                                   14:0
#define DCREG_VSYNC_START_End                                                 14
#define DCREG_VSYNC_START_Start                                                0
#define DCREG_VSYNC_START_Type                                               U15

/* End of the vertical sync pulse. */
#define DCREG_VSYNC_END                                                    29:15
#define DCREG_VSYNC_END_End                                                   29
#define DCREG_VSYNC_END_Start                                                 15
#define DCREG_VSYNC_END_Type                                                 U15

/* Vertical sync pulse control. */
#define DCREG_VSYNC_PULSE                                                  30:30
#define DCREG_VSYNC_PULSE_End                                                 30
#define DCREG_VSYNC_PULSE_Start                                               30
#define DCREG_VSYNC_PULSE_Type                                               U01
#define   DCREG_VSYNC_PULSE_DISABLED                                         0x0
#define   DCREG_VSYNC_PULSE_ENABLED                                          0x1

/* Polarity of the vertical sync pulse. */
#define DCREG_VSYNC_POLARITY                                               31:31
#define DCREG_VSYNC_POLARITY_End                                              31
#define DCREG_VSYNC_POLARITY_Start                                            31
#define DCREG_VSYNC_POLARITY_Type                                            U01
#define   DCREG_VSYNC_POLARITY_POSITIVE                                      0x0
#define   DCREG_VSYNC_POLARITY_NEGATIVE                                      0x1

/* Register dcregDisplayCurrentLocation (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Current x,y location of display controller.$ */

#define dcregDisplayCurrentLocationRegAddrs                               0x0514
#define DCREG_DISPLAY_CURRENT_LOCATION_Address                           0x01450
#define DCREG_DISPLAY_CURRENT_LOCATION_MSB                                    15
#define DCREG_DISPLAY_CURRENT_LOCATION_LSB                                     1
#define DCREG_DISPLAY_CURRENT_LOCATION_BLK                                     1
#define DCREG_DISPLAY_CURRENT_LOCATION_Count                                   2
#define DCREG_DISPLAY_CURRENT_LOCATION_FieldMask                      0xFFFFFFFF
#define DCREG_DISPLAY_CURRENT_LOCATION_ReadMask                       0xFFFFFFFF
#define DCREG_DISPLAY_CURRENT_LOCATION_WriteMask                      0x00000000
#define DCREG_DISPLAY_CURRENT_LOCATION_ResetValue                     0x00000000

/* Current X location. */
#define DCREG_DISPLAY_CURRENT_LOCATION_X                                    15:0
#define DCREG_DISPLAY_CURRENT_LOCATION_X_End                                  15
#define DCREG_DISPLAY_CURRENT_LOCATION_X_Start                                 0
#define DCREG_DISPLAY_CURRENT_LOCATION_X_Type                                U16

/* Current Y location. */
#define DCREG_DISPLAY_CURRENT_LOCATION_Y                                   31:16
#define DCREG_DISPLAY_CURRENT_LOCATION_Y_End                                  31
#define DCREG_DISPLAY_CURRENT_LOCATION_Y_Start                                16
#define DCREG_DISPLAY_CURRENT_LOCATION_Y_Type                                U16

/* Register dcregGammaIndex (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Index into gamma table.  See dcregGammaData for more
information.$ */

#define dcregGammaIndexRegAddrs                                           0x0516
#define DCREG_GAMMA_INDEX_Address                                        0x01458
#define DCREG_GAMMA_INDEX_MSB                                                 15
#define DCREG_GAMMA_INDEX_LSB                                                  1
#define DCREG_GAMMA_INDEX_BLK                                                  1
#define DCREG_GAMMA_INDEX_Count                                                2
#define DCREG_GAMMA_INDEX_FieldMask                                   0x000000FF
#define DCREG_GAMMA_INDEX_ReadMask                                    0x00000000
#define DCREG_GAMMA_INDEX_WriteMask                                   0x000000FF
#define DCREG_GAMMA_INDEX_ResetValue                                  0x00000000

/* Index into gamma table. */
#define DCREG_GAMMA_INDEX_INDEX                                              7:0
#define DCREG_GAMMA_INDEX_INDEX_End                                            7
#define DCREG_GAMMA_INDEX_INDEX_Start                                          0
#define DCREG_GAMMA_INDEX_INDEX_Type                                         U08

/* Register dcregGammaData (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Translation values for the gamma table.  When this register                **
** gets written, the data gets stored in the gamma table at the               **
** index specified by the dcregGammaIndex register.  After the                **
** register is written, the index gets incremented.$                          */

#define dcregGammaDataRegAddrs                                            0x0518
#define DCREG_GAMMA_DATA_Address                                         0x01460
#define DCREG_GAMMA_DATA_MSB                                                  15
#define DCREG_GAMMA_DATA_LSB                                                   1
#define DCREG_GAMMA_DATA_BLK                                                   1
#define DCREG_GAMMA_DATA_Count                                                 2
#define DCREG_GAMMA_DATA_FieldMask                                    0x3FFFFFFF
#define DCREG_GAMMA_DATA_ReadMask                                     0x00000000
#define DCREG_GAMMA_DATA_WriteMask                                    0x3FFFFFFF
#define DCREG_GAMMA_DATA_ResetValue                                   0x00000000

/* Blue translation value. */
#define DCREG_GAMMA_DATA_BLUE                                                9:0
#define DCREG_GAMMA_DATA_BLUE_End                                              9
#define DCREG_GAMMA_DATA_BLUE_Start                                            0
#define DCREG_GAMMA_DATA_BLUE_Type                                           U10

/* Green translation value. */
#define DCREG_GAMMA_DATA_GREEN                                             19:10
#define DCREG_GAMMA_DATA_GREEN_End                                            19
#define DCREG_GAMMA_DATA_GREEN_Start                                          10
#define DCREG_GAMMA_DATA_GREEN_Type                                          U10

/* Red translation value. */
#define DCREG_GAMMA_DATA_RED                                               29:20
#define DCREG_GAMMA_DATA_RED_End                                              29
#define DCREG_GAMMA_DATA_RED_Start                                            20
#define DCREG_GAMMA_DATA_RED_Type                                            U10

/* Register dcregCursorConfig **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Configuration register for the cursor.$ */

#define dcregCursorConfigRegAddrs                                         0x051A
#define DCREG_CURSOR_CONFIG_Address                                      0x01468
#define DCREG_CURSOR_CONFIG_MSB                                               15
#define DCREG_CURSOR_CONFIG_LSB                                                0
#define DCREG_CURSOR_CONFIG_BLK                                                0
#define DCREG_CURSOR_CONFIG_Count                                              1
#define DCREG_CURSOR_CONFIG_FieldMask                                 0x001F1FF7
#define DCREG_CURSOR_CONFIG_ReadMask                                  0x001F1FF7
#define DCREG_CURSOR_CONFIG_WriteMask                                 0x001F1FF7
#define DCREG_CURSOR_CONFIG_ResetValue                                0x00000000

/* Format of the cursor.
NOTE: This field is double buffered. */
#define DCREG_CURSOR_CONFIG_FORMAT                                           1:0
#define DCREG_CURSOR_CONFIG_FORMAT_End                                         1
#define DCREG_CURSOR_CONFIG_FORMAT_Start                                       0
#define DCREG_CURSOR_CONFIG_FORMAT_Type                                      U02
#define   DCREG_CURSOR_CONFIG_FORMAT_DISABLED                                0x0
#define   DCREG_CURSOR_CONFIG_FORMAT_MASKED                                  0x1
#define   DCREG_CURSOR_CONFIG_FORMAT_A8R8G8B8                                0x2

/* Trigger to fetch cursor. */
#define DCREG_CURSOR_CONFIG_TRIGGER_FETCH                                    2:2
#define DCREG_CURSOR_CONFIG_TRIGGER_FETCH_End                                  2
#define DCREG_CURSOR_CONFIG_TRIGGER_FETCH_Start                                2
#define DCREG_CURSOR_CONFIG_TRIGGER_FETCH_Type                               U01
#define   DCREG_CURSOR_CONFIG_TRIGGER_FETCH_DISABLED                         0x0
#define   DCREG_CURSOR_CONFIG_TRIGGER_FETCH_ENABLED                          0x1

/* Display Controller owning the cursor. */
#define DCREG_CURSOR_CONFIG_DISPLAY                                          4:4
#define DCREG_CURSOR_CONFIG_DISPLAY_End                                        4
#define DCREG_CURSOR_CONFIG_DISPLAY_Start                                      4
#define DCREG_CURSOR_CONFIG_DISPLAY_Type                                     U01
#define   DCREG_CURSOR_CONFIG_DISPLAY_DISPLAY0                               0x0
#define   DCREG_CURSOR_CONFIG_DISPLAY_DISPLAY1                               0x1

/* Cursor size. */
#define DCREG_CURSOR_CONFIG_SIZE                                             7:5
#define DCREG_CURSOR_CONFIG_SIZE_End                                           7
#define DCREG_CURSOR_CONFIG_SIZE_Start                                         5
#define DCREG_CURSOR_CONFIG_SIZE_Type                                        U03
#define   DCREG_CURSOR_CONFIG_SIZE_SIZE32X32                                 0x0
#define   DCREG_CURSOR_CONFIG_SIZE_SIZE64X64                                 0x1
#define   DCREG_CURSOR_CONFIG_SIZE_SIZE128X128                               0x2
#define   DCREG_CURSOR_CONFIG_SIZE_SIZE256X256                               0x3

/* Vertical offset to cursor hotspot.
NOTE: This field is double buffered. */
#define DCREG_CURSOR_CONFIG_HOT_SPOT_Y                                      12:8
#define DCREG_CURSOR_CONFIG_HOT_SPOT_Y_End                                    12
#define DCREG_CURSOR_CONFIG_HOT_SPOT_Y_Start                                   8
#define DCREG_CURSOR_CONFIG_HOT_SPOT_Y_Type                                  U05

/* Horizontal offset to cursor hotspot.
NOTE: This field is double buffered. */
#define DCREG_CURSOR_CONFIG_HOT_SPOT_X                                     20:16
#define DCREG_CURSOR_CONFIG_HOT_SPOT_X_End                                    20
#define DCREG_CURSOR_CONFIG_HOT_SPOT_X_Start                                  16
#define DCREG_CURSOR_CONFIG_HOT_SPOT_X_Type                                  U05

/* Register dcregCursorAddress **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Address of the cursor shape.
NOTE: This register is double buffered. */

#define dcregCursorAddressRegAddrs                                        0x051B
#define DCREG_CURSOR_ADDRESS_Address                                     0x0146C
#define DCREG_CURSOR_ADDRESS_MSB                                              15
#define DCREG_CURSOR_ADDRESS_LSB                                               0
#define DCREG_CURSOR_ADDRESS_BLK                                               0
#define DCREG_CURSOR_ADDRESS_Count                                             1
#define DCREG_CURSOR_ADDRESS_FieldMask                                0xFFFFFFFF
#define DCREG_CURSOR_ADDRESS_ReadMask                                 0xFFFFFFFF
#define DCREG_CURSOR_ADDRESS_WriteMask                                0xFFFFFFFF
#define DCREG_CURSOR_ADDRESS_ResetValue                               0x00000000

#define DCREG_CURSOR_ADDRESS_ADDRESS                                        31:0
#define DCREG_CURSOR_ADDRESS_ADDRESS_End                                      31
#define DCREG_CURSOR_ADDRESS_ADDRESS_Start                                     0
#define DCREG_CURSOR_ADDRESS_ADDRESS_Type                                    U32

/* Register dcregCursorLocation **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Location of the cursor on the owning display.                              **
** NOTE: This register is double buffered.                                    */

#define dcregCursorLocationRegAddrs                                       0x051C
#define DCREG_CURSOR_LOCATION_Address                                    0x01470
#define DCREG_CURSOR_LOCATION_MSB                                             15
#define DCREG_CURSOR_LOCATION_LSB                                              0
#define DCREG_CURSOR_LOCATION_BLK                                              0
#define DCREG_CURSOR_LOCATION_Count                                            1
#define DCREG_CURSOR_LOCATION_FieldMask                               0x7FFF7FFF
#define DCREG_CURSOR_LOCATION_ReadMask                                0x7FFF7FFF
#define DCREG_CURSOR_LOCATION_WriteMask                               0x7FFF7FFF
#define DCREG_CURSOR_LOCATION_ResetValue                              0x00000000

/* X location of cursor's hotspot. */
#define DCREG_CURSOR_LOCATION_X                                             14:0
#define DCREG_CURSOR_LOCATION_X_End                                           14
#define DCREG_CURSOR_LOCATION_X_Start                                          0
#define DCREG_CURSOR_LOCATION_X_Type                                         U15

/* Y location of cursor's hotspot. */
#define DCREG_CURSOR_LOCATION_Y                                            30:16
#define DCREG_CURSOR_LOCATION_Y_End                                           30
#define DCREG_CURSOR_LOCATION_Y_Start                                         16
#define DCREG_CURSOR_LOCATION_Y_Type                                         U15

/* Register dcregCursorBackground **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The background color for Masked cursors.                                   **
** NOTE: This register is double buffered.                                    */

#define dcregCursorBackgroundRegAddrs                                     0x051D
#define DCREG_CURSOR_BACKGROUND_Address                                  0x01474
#define DCREG_CURSOR_BACKGROUND_MSB                                           15
#define DCREG_CURSOR_BACKGROUND_LSB                                            0
#define DCREG_CURSOR_BACKGROUND_BLK                                            0
#define DCREG_CURSOR_BACKGROUND_Count                                          1
#define DCREG_CURSOR_BACKGROUND_FieldMask                             0x00FFFFFF
#define DCREG_CURSOR_BACKGROUND_ReadMask                              0x00FFFFFF
#define DCREG_CURSOR_BACKGROUND_WriteMask                             0x00FFFFFF
#define DCREG_CURSOR_BACKGROUND_ResetValue                            0x00000000

/* Blue value. */
#define DCREG_CURSOR_BACKGROUND_BLUE                                         7:0
#define DCREG_CURSOR_BACKGROUND_BLUE_End                                       7
#define DCREG_CURSOR_BACKGROUND_BLUE_Start                                     0
#define DCREG_CURSOR_BACKGROUND_BLUE_Type                                    U08

/* Green value. */
#define DCREG_CURSOR_BACKGROUND_GREEN                                       15:8
#define DCREG_CURSOR_BACKGROUND_GREEN_End                                     15
#define DCREG_CURSOR_BACKGROUND_GREEN_Start                                    8
#define DCREG_CURSOR_BACKGROUND_GREEN_Type                                   U08

/* Red value. */
#define DCREG_CURSOR_BACKGROUND_RED                                        23:16
#define DCREG_CURSOR_BACKGROUND_RED_End                                       23
#define DCREG_CURSOR_BACKGROUND_RED_Start                                     16
#define DCREG_CURSOR_BACKGROUND_RED_Type                                     U08

/* Register dcregCursorForeground **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The foreground color for Masked cursors.                                   **
** NOTE: This register is double buffered.                                    */

#define dcregCursorForegroundRegAddrs                                     0x051E
#define DCREG_CURSOR_FOREGROUND_Address                                  0x01478
#define DCREG_CURSOR_FOREGROUND_MSB                                           15
#define DCREG_CURSOR_FOREGROUND_LSB                                            0
#define DCREG_CURSOR_FOREGROUND_BLK                                            0
#define DCREG_CURSOR_FOREGROUND_Count                                          1
#define DCREG_CURSOR_FOREGROUND_FieldMask                             0x00FFFFFF
#define DCREG_CURSOR_FOREGROUND_ReadMask                              0x00FFFFFF
#define DCREG_CURSOR_FOREGROUND_WriteMask                             0x00FFFFFF
#define DCREG_CURSOR_FOREGROUND_ResetValue                            0x00000000

/* Blue value. */
#define DCREG_CURSOR_FOREGROUND_BLUE                                         7:0
#define DCREG_CURSOR_FOREGROUND_BLUE_End                                       7
#define DCREG_CURSOR_FOREGROUND_BLUE_Start                                     0
#define DCREG_CURSOR_FOREGROUND_BLUE_Type                                    U08

/* Green value. */
#define DCREG_CURSOR_FOREGROUND_GREEN                                       15:8
#define DCREG_CURSOR_FOREGROUND_GREEN_End                                     15
#define DCREG_CURSOR_FOREGROUND_GREEN_Start                                    8
#define DCREG_CURSOR_FOREGROUND_GREEN_Type                                   U08

/* Red value. */
#define DCREG_CURSOR_FOREGROUND_RED                                        23:16
#define DCREG_CURSOR_FOREGROUND_RED_End                                       23
#define DCREG_CURSOR_FOREGROUND_RED_Start                                     16
#define DCREG_CURSOR_FOREGROUND_RED_Type                                     U08

/* Register dcregDisplayIntr **
** ~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregDisplayIntrRegAddrs                                          0x051F
#define DCREG_DISPLAY_INTR_Address                                       0x0147C
#define DCREG_DISPLAY_INTR_MSB                                                15
#define DCREG_DISPLAY_INTR_LSB                                                 0
#define DCREG_DISPLAY_INTR_BLK                                                 0
#define DCREG_DISPLAY_INTR_Count                                               1
#define DCREG_DISPLAY_INTR_FieldMask                                  0x00003111
#define DCREG_DISPLAY_INTR_ReadMask                                   0x00003111
#define DCREG_DISPLAY_INTR_WriteMask                                  0x00000000
#define DCREG_DISPLAY_INTR_ResetValue                                 0x00000000

/* display 0 interrupt */
#define DCREG_DISPLAY_INTR_DISP0                                             0:0
#define DCREG_DISPLAY_INTR_DISP0_End                                           0
#define DCREG_DISPLAY_INTR_DISP0_Start                                         0
#define DCREG_DISPLAY_INTR_DISP0_Type                                        U01

/* display 1 interrupt */
#define DCREG_DISPLAY_INTR_DISP1                                             4:4
#define DCREG_DISPLAY_INTR_DISP1_End                                           4
#define DCREG_DISPLAY_INTR_DISP1_Start                                         4
#define DCREG_DISPLAY_INTR_DISP1_Type                                        U01

/* cursor interrupt */
#define DCREG_DISPLAY_INTR_CURSOR                                            8:8
#define DCREG_DISPLAY_INTR_CURSOR_End                                          8
#define DCREG_DISPLAY_INTR_CURSOR_Start                                        8
#define DCREG_DISPLAY_INTR_CURSOR_Type                                       U01

/* display0 dbi configure error */
#define DCREG_DISPLAY_INTR_DISP0_DBI_CFG_ERROR                             12:12
#define DCREG_DISPLAY_INTR_DISP0_DBI_CFG_ERROR_End                            12
#define DCREG_DISPLAY_INTR_DISP0_DBI_CFG_ERROR_Start                          12
#define DCREG_DISPLAY_INTR_DISP0_DBI_CFG_ERROR_Type                          U01

/* display1 dbi configure error */
#define DCREG_DISPLAY_INTR_DISP1_DBI_CFG_ERROR                             13:13
#define DCREG_DISPLAY_INTR_DISP1_DBI_CFG_ERROR_End                            13
#define DCREG_DISPLAY_INTR_DISP1_DBI_CFG_ERROR_Start                          13
#define DCREG_DISPLAY_INTR_DISP1_DBI_CFG_ERROR_Type                          U01

/* Register dcregDisplayIntrEnable **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The interrupt enable register for display_0 and display_1.                 **
** NOTE:Interrupt enable for register dcregDisplayIntr.                       */

#define dcregDisplayIntrEnableRegAddrs                                    0x0520
#define DCREG_DISPLAY_INTR_ENABLE_Address                                0x01480
#define DCREG_DISPLAY_INTR_ENABLE_MSB                                         15
#define DCREG_DISPLAY_INTR_ENABLE_LSB                                          0
#define DCREG_DISPLAY_INTR_ENABLE_BLK                                          0
#define DCREG_DISPLAY_INTR_ENABLE_Count                                        1
#define DCREG_DISPLAY_INTR_ENABLE_FieldMask                           0x00000011
#define DCREG_DISPLAY_INTR_ENABLE_ReadMask                            0x00000011
#define DCREG_DISPLAY_INTR_ENABLE_WriteMask                           0x00000011
#define DCREG_DISPLAY_INTR_ENABLE_ResetValue                          0x00000000

/* display 0 interrupt enable */
#define DCREG_DISPLAY_INTR_ENABLE_DISP0                                      0:0
#define DCREG_DISPLAY_INTR_ENABLE_DISP0_End                                    0
#define DCREG_DISPLAY_INTR_ENABLE_DISP0_Start                                  0
#define DCREG_DISPLAY_INTR_ENABLE_DISP0_Type                                 U01
#define DCREG_DISPLAY_INTR_ENABLE_DISP0_ENABLE                               0x1

/* display 1 interrupt enable */
#define DCREG_DISPLAY_INTR_ENABLE_DISP1                                      4:4
#define DCREG_DISPLAY_INTR_ENABLE_DISP1_End                                    4
#define DCREG_DISPLAY_INTR_ENABLE_DISP1_Start                                  4
#define DCREG_DISPLAY_INTR_ENABLE_DISP1_Type                                 U01
#define DCREG_DISPLAY_INTR_ENABLE_DISP1_ENABLE                        (0x1 << 4)

/* Register dcregCursorModuleClockGatingControl **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Module level clock gating control for cursor */

#define dcregCursorModuleClockGatingControlRegAddrs                       0x0521
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_Address                 0x01484
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_MSB                          15
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_LSB                           0
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_BLK                           0
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_Count                         1
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_FieldMask            0x00000001
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_ReadMask             0x00000001
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_WriteMask            0x00000001
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_ResetValue           0x00000001

#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_CURSOR 0:0
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_CURSOR_End 0
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_CURSOR_Start 0
#define DCREG_CURSOR_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_CURSOR_Type U01

/* Register dcregDbiConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The configuration register for the DBI output */

#define dcregDbiConfigRegAddrs                                            0x0522
#define DCREG_DBI_CONFIG_Address                                         0x01488
#define DCREG_DBI_CONFIG_MSB                                                  15
#define DCREG_DBI_CONFIG_LSB                                                   1
#define DCREG_DBI_CONFIG_BLK                                                   1
#define DCREG_DBI_CONFIG_Count                                                 2
#define DCREG_DBI_CONFIG_FieldMask                                    0x00003FFF
#define DCREG_DBI_CONFIG_ReadMask                                     0x00003FFF
#define DCREG_DBI_CONFIG_WriteMask                                    0x00003FFF
#define DCREG_DBI_CONFIG_ResetValue                                   0x00000000

/* Options of DBI type C */
#define DCREG_DBI_CONFIG_DBI_TYPEC_OPT                                     13:12
#define DCREG_DBI_CONFIG_DBI_TYPEC_OPT_End                                    13
#define DCREG_DBI_CONFIG_DBI_TYPEC_OPT_Start                                  12
#define DCREG_DBI_CONFIG_DBI_TYPEC_OPT_Type                                  U02

/* Time unit for AC characteristics */
#define DCREG_DBI_CONFIG_DBI_AC_TIME_UNIT                                   11:8
#define DCREG_DBI_CONFIG_DBI_AC_TIME_UNIT_End                                 11
#define DCREG_DBI_CONFIG_DBI_AC_TIME_UNIT_Start                                8
#define DCREG_DBI_CONFIG_DBI_AC_TIME_UNIT_Type                               U04

/* D/CX Pin polarity */
#define DCREG_DBI_CONFIG_DBIX_POLARITY                                       7:7
#define DCREG_DBI_CONFIG_DBIX_POLARITY_End                                     7
#define DCREG_DBI_CONFIG_DBIX_POLARITY_Start                                   7
#define DCREG_DBI_CONFIG_DBIX_POLARITY_Type                                  U01
#define   DCREG_DBI_CONFIG_DBIX_POLARITY_DEFAULT                             0x0
#define   DCREG_DBI_CONFIG_DBIX_POLARITY_REVERSE                             0x1

/* Output bus select */
#define DCREG_DBI_CONFIG_BUS_OUTPUT_SEL                                      6:6
#define DCREG_DBI_CONFIG_BUS_OUTPUT_SEL_End                                    6
#define DCREG_DBI_CONFIG_BUS_OUTPUT_SEL_Start                                  6
#define DCREG_DBI_CONFIG_BUS_OUTPUT_SEL_Type                                 U01
#define   DCREG_DBI_CONFIG_BUS_OUTPUT_SEL_DPI                                0x0
#define   DCREG_DBI_CONFIG_BUS_OUTPUT_SEL_DBI                                0x1

/* DBI interface data format Refert to DBI spec.'Interface color coding' for  **
** detailed                                                                   */
#define DCREG_DBI_CONFIG_DBI_DATA_FORMAT                                     5:2
#define DCREG_DBI_CONFIG_DBI_DATA_FORMAT_End                                   5
#define DCREG_DBI_CONFIG_DBI_DATA_FORMAT_Start                                 2
#define DCREG_DBI_CONFIG_DBI_DATA_FORMAT_Type                                U04
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D8R3G3B2                          0x0
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D8R4G4B4                          0x1
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D8R5G6B5                          0x2
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D8R6G6B6                          0x3
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D8R8G8B8                          0x4
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D9R6G6B6                          0x5
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D16R3G3B2                         0x6
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D16R4G4B4                         0x7
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D16R5G6B5                         0x8
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D16_R6_G6_B6_OP1                  0x9
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D16_R6_G6_B6_OP2                  0xA
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D16_R8_G8_B8_OP1                  0xB
#define   DCREG_DBI_CONFIG_DBI_DATA_FORMAT_D16_R8_G8_B8_OP2                  0xC

/* DBI Type select */
#define DCREG_DBI_CONFIG_DBI_TYPE                                            1:0
#define DCREG_DBI_CONFIG_DBI_TYPE_End                                          1
#define DCREG_DBI_CONFIG_DBI_TYPE_Start                                        0
#define DCREG_DBI_CONFIG_DBI_TYPE_Type                                       U02
#define   DCREG_DBI_CONFIG_DBI_TYPE_TYPE_AFIXED_E                            0x0
#define   DCREG_DBI_CONFIG_DBI_TYPE_TYPE_ACLOCK_E                            0x1
#define   DCREG_DBI_CONFIG_DBI_TYPE_TYPE_B                                   0x2
#define   DCREG_DBI_CONFIG_DBI_TYPE_TYPE_C                                   0x3

/* Register dcregDbiIfReset (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Reset DBI interface to idle state */

#define dcregDbiIfResetRegAddrs                                           0x0524
#define DCREG_DBI_IF_RESET_Address                                       0x01490
#define DCREG_DBI_IF_RESET_MSB                                                15
#define DCREG_DBI_IF_RESET_LSB                                                 1
#define DCREG_DBI_IF_RESET_BLK                                                 1
#define DCREG_DBI_IF_RESET_Count                                               2
#define DCREG_DBI_IF_RESET_FieldMask                                  0x00000001
#define DCREG_DBI_IF_RESET_ReadMask                                   0x00000000
#define DCREG_DBI_IF_RESET_WriteMask                                  0x00000001
#define DCREG_DBI_IF_RESET_ResetValue                                 0x00000000

/* Config to 1 if only to reset DBI interface to idle state.  Need to config  **
** dcregFrameBufferConfig[20] first to 1 then to 0 if to reset pipeline       */
#define DCREG_DBI_IF_RESET_DBI_IF_LEVEL_RESET                                0:0
#define DCREG_DBI_IF_RESET_DBI_IF_LEVEL_RESET_End                              0
#define DCREG_DBI_IF_RESET_DBI_IF_LEVEL_RESET_Start                            0
#define DCREG_DBI_IF_RESET_DBI_IF_LEVEL_RESET_Type                           U01
#define   DCREG_DBI_IF_RESET_DBI_IF_LEVEL_RESET_RESET                        0x1

/* Register dcregDbiWrChar1 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DBI write AC characteristics definition register 1 */

#define dcregDbiWrChar1RegAddrs                                           0x0526
#define DCREG_DBI_WR_CHAR1_Address                                       0x01498
#define DCREG_DBI_WR_CHAR1_MSB                                                15
#define DCREG_DBI_WR_CHAR1_LSB                                                 1
#define DCREG_DBI_WR_CHAR1_BLK                                                 1
#define DCREG_DBI_WR_CHAR1_Count                                               2
#define DCREG_DBI_WR_CHAR1_FieldMask                                  0x0000FFFF
#define DCREG_DBI_WR_CHAR1_ReadMask                                   0x0000FFFF
#define DCREG_DBI_WR_CHAR1_WriteMask                                  0x0000FFFF
#define DCREG_DBI_WR_CHAR1_ResetValue                                 0x00000000

/* 0.When type A fixed E mode: Time to assert CSX 1.When type A clock E mode: **
** Not used 2.When type B mode: Time to assert CSX cycle                      **
** number=Setting*(DbiAcTimeUnit+1)                                           */
#define DCREG_DBI_WR_CHAR1_DBI_WR_CS_ASSERT                                15:12
#define DCREG_DBI_WR_CHAR1_DBI_WR_CS_ASSERT_End                               15
#define DCREG_DBI_WR_CHAR1_DBI_WR_CS_ASSERT_Start                             12
#define DCREG_DBI_WR_CHAR1_DBI_WR_CS_ASSERT_Type                             U04

/* 0.When type A fixed E mode: Not used 1.When type A clock E mode: Time to   **
** assert E  2.When type B mode: Time to assert WRX cycle                     **
** number=Setting*(DbiAcTimeUnit+1)                                           */
#define DCREG_DBI_WR_CHAR1_DBI_WR_EOR_WR_ASSERT                             11:8
#define DCREG_DBI_WR_CHAR1_DBI_WR_EOR_WR_ASSERT_End                           11
#define DCREG_DBI_WR_CHAR1_DBI_WR_EOR_WR_ASSERT_Start                          8
#define DCREG_DBI_WR_CHAR1_DBI_WR_EOR_WR_ASSERT_Type                         U04

/* Once write period duration cycle number=Setting*(DbiAcTimeUnit+1)          **
** This field must be no less than 3                                          */
#define DCREG_DBI_WR_CHAR1_DBI_WR_PERIOD                                     7:0
#define DCREG_DBI_WR_CHAR1_DBI_WR_PERIOD_End                                   7
#define DCREG_DBI_WR_CHAR1_DBI_WR_PERIOD_Start                                 0
#define DCREG_DBI_WR_CHAR1_DBI_WR_PERIOD_Type                                U08

/* Register dcregDbiWrChar2 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DBI write AC characteristics definition register 2 */

#define dcregDbiWrChar2RegAddrs                                           0x0528
#define DCREG_DBI_WR_CHAR2_Address                                       0x014A0
#define DCREG_DBI_WR_CHAR2_MSB                                                15
#define DCREG_DBI_WR_CHAR2_LSB                                                 1
#define DCREG_DBI_WR_CHAR2_BLK                                                 1
#define DCREG_DBI_WR_CHAR2_Count                                               2
#define DCREG_DBI_WR_CHAR2_FieldMask                                  0x0000FFFF
#define DCREG_DBI_WR_CHAR2_ReadMask                                   0x0000FFFF
#define DCREG_DBI_WR_CHAR2_WriteMask                                  0x0000FFFF
#define DCREG_DBI_WR_CHAR2_ResetValue                                 0x00000000

/* 0.When type A fixed E mode: Time to de-assert CSX 1.When type A clock E    **
** mode: Not used 2.When type B mode: Time to de-assert CSX cycle             **
** number=Setting*(DbiAcTimeUnit+1)                                           */
#define DCREG_DBI_WR_CHAR2_DBI_WR_CS_DE_ASRT                                15:8
#define DCREG_DBI_WR_CHAR2_DBI_WR_CS_DE_ASRT_End                              15
#define DCREG_DBI_WR_CHAR2_DBI_WR_CS_DE_ASRT_Start                             8
#define DCREG_DBI_WR_CHAR2_DBI_WR_CS_DE_ASRT_Type                            U08

/* 0.When type A fixed E mode: Not used 1.When type A clock E mode: Time to   **
** de-assert E 2.When type B mode: Time to de-assert WRX cycle                **
** number=Setting*(DbiAcTimeUnit+1)                                           */
#define DCREG_DBI_WR_CHAR2_DBI_WR_EOR_WR_DE_ASRT                             7:0
#define DCREG_DBI_WR_CHAR2_DBI_WR_EOR_WR_DE_ASRT_End                           7
#define DCREG_DBI_WR_CHAR2_DBI_WR_EOR_WR_DE_ASRT_Start                         0
#define DCREG_DBI_WR_CHAR2_DBI_WR_EOR_WR_DE_ASRT_Type                        U08

/* Register dcregDbiCmd (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DBI command in/out port */

#define dcregDbiCmdRegAddrs                                               0x052A
#define DCREG_DBI_CMD_Address                                            0x014A8
#define DCREG_DBI_CMD_MSB                                                     15
#define DCREG_DBI_CMD_LSB                                                      1
#define DCREG_DBI_CMD_BLK                                                      1
#define DCREG_DBI_CMD_Count                                                    2
#define DCREG_DBI_CMD_FieldMask                                       0xC000FFFF
#define DCREG_DBI_CMD_ReadMask                                        0x00000000
#define DCREG_DBI_CMD_WriteMask                                       0xC000FFFF
#define DCREG_DBI_CMD_ResetValue                                      0x00000000

/* DBI command flag */
#define DCREG_DBI_CMD_DBI_COMMAND_FLAG                                     31:30
#define DCREG_DBI_CMD_DBI_COMMAND_FLAG_End                                    31
#define DCREG_DBI_CMD_DBI_COMMAND_FLAG_Start                                  30
#define DCREG_DBI_CMD_DBI_COMMAND_FLAG_Type                                  U02
#define   DCREG_DBI_CMD_DBI_COMMAND_FLAG_ADDRESS                             0x0
#define   DCREG_DBI_CMD_DBI_COMMAND_FLAG_WRITE_MEM_START                     0x1
#define   DCREG_DBI_CMD_DBI_COMMAND_FLAG_PARAMETER_OR_DATA                   0x2
#define   DCREG_DBI_CMD_DBI_COMMAND_FLAG_READ                                0x3

/* To write this register will send command/data to DBI port */
#define DCREG_DBI_CMD_DBI_COMMAND_WORD                                      15:0
#define DCREG_DBI_CMD_DBI_COMMAND_WORD_End                                    15
#define DCREG_DBI_CMD_DBI_COMMAND_WORD_Start                                   0
#define DCREG_DBI_CMD_DBI_COMMAND_WORD_Type                                  U16

/* Register dcregGeneralConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* NOTE: This register is double buffered. Misc options configure. */

#define dcregGeneralConfigRegAddrs                                        0x052C
#define DCREG_GENERAL_CONFIG_Address                                     0x014B0
#define DCREG_GENERAL_CONFIG_MSB                                              15
#define DCREG_GENERAL_CONFIG_LSB                                               1
#define DCREG_GENERAL_CONFIG_BLK                                               1
#define DCREG_GENERAL_CONFIG_Count                                             2
#define DCREG_GENERAL_CONFIG_FieldMask                                0x0000000F
#define DCREG_GENERAL_CONFIG_ReadMask                                 0x0000000F
#define DCREG_GENERAL_CONFIG_WriteMask                                0x0000000F
#define DCREG_GENERAL_CONFIG_ResetValue                               0x00000000

/* Control endian swapping. */
#define DCREG_GENERAL_CONFIG_ENDIAN_CONTROL                                  1:0
#define DCREG_GENERAL_CONFIG_ENDIAN_CONTROL_End                                1
#define DCREG_GENERAL_CONFIG_ENDIAN_CONTROL_Start                              0
#define DCREG_GENERAL_CONFIG_ENDIAN_CONTROL_Type                             U02
#define   DCREG_GENERAL_CONFIG_ENDIAN_CONTROL_NO_SWAP                        0x0
#define   DCREG_GENERAL_CONFIG_ENDIAN_CONTROL_SWAP_WORD                      0x1
#define   DCREG_GENERAL_CONFIG_ENDIAN_CONTROL_SWAP_DWORD                     0x2
#define   DCREG_GENERAL_CONFIG_ENDIAN_CONTROL_SWAP_DDWORD                    0x3

/* If enabled, when FIFO underflow happens, output is stalled.  */
#define DCREG_GENERAL_CONFIG_STALL_OUTPUT_WHEN_UNDERFLOW                     2:2
#define DCREG_GENERAL_CONFIG_STALL_OUTPUT_WHEN_UNDERFLOW_End                   2
#define DCREG_GENERAL_CONFIG_STALL_OUTPUT_WHEN_UNDERFLOW_Start                 2
#define DCREG_GENERAL_CONFIG_STALL_OUTPUT_WHEN_UNDERFLOW_Type                U01
#define   DCREG_GENERAL_CONFIG_STALL_OUTPUT_WHEN_UNDERFLOW_DISABLED          0x0
#define   DCREG_GENERAL_CONFIG_STALL_OUTPUT_WHEN_UNDERFLOW_ENABLED           0x1

/* Disable idle signal. */
#define DCREG_GENERAL_CONFIG_DISABLE_IDLE                                    3:3
#define DCREG_GENERAL_CONFIG_DISABLE_IDLE_End                                  3
#define DCREG_GENERAL_CONFIG_DISABLE_IDLE_Start                                3
#define DCREG_GENERAL_CONFIG_DISABLE_IDLE_Type                               U01
#define   DCREG_GENERAL_CONFIG_DISABLE_IDLE_DISABLED                         0x0
#define   DCREG_GENERAL_CONFIG_DISABLE_IDLE_ENABLED                          0x1

/* Register dcregDpiConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The configuration register for DPI output NOTE: This register is double    **
** buffered.                                                                  */

#define dcregDpiConfigRegAddrs                                            0x052E
#define DCREG_DPI_CONFIG_Address                                         0x014B8
#define DCREG_DPI_CONFIG_MSB                                                  15
#define DCREG_DPI_CONFIG_LSB                                                   1
#define DCREG_DPI_CONFIG_BLK                                                   1
#define DCREG_DPI_CONFIG_Count                                                 2
#define DCREG_DPI_CONFIG_FieldMask                                    0x00000007
#define DCREG_DPI_CONFIG_ReadMask                                     0x00000007
#define DCREG_DPI_CONFIG_WriteMask                                    0x00000007
#define DCREG_DPI_CONFIG_ResetValue                                   0x00000000

/* DPI interface data format Refer to DPI spec.'Interface color coding' for   **
** detailed                                                                   */
#define DCREG_DPI_CONFIG_DPI_DATA_FORMAT                                     2:0
#define DCREG_DPI_CONFIG_DPI_DATA_FORMAT_End                                   2
#define DCREG_DPI_CONFIG_DPI_DATA_FORMAT_Start                                 0
#define DCREG_DPI_CONFIG_DPI_DATA_FORMAT_Type                                U03
#define   DCREG_DPI_CONFIG_DPI_DATA_FORMAT_D16CFG1                           0x0
#define   DCREG_DPI_CONFIG_DPI_DATA_FORMAT_D16CFG2                           0x1
#define   DCREG_DPI_CONFIG_DPI_DATA_FORMAT_D16CFG3                           0x2
#define   DCREG_DPI_CONFIG_DPI_DATA_FORMAT_D18CFG1                           0x3
#define   DCREG_DPI_CONFIG_DPI_DATA_FORMAT_D18CFG2                           0x4
#define   DCREG_DPI_CONFIG_DPI_DATA_FORMAT_D24                               0x5
#define   DCREG_DPI_CONFIG_DPI_DATA_FORMAT_D30                               0x6

/* Register dcregDbiTypecCfg (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Configuration for DBI type C NOTE: This register is double buffered. */

#define dcregDbiTypecCfgRegAddrs                                          0x0530
#define DCREG_DBI_TYPEC_CFG_Address                                      0x014C0
#define DCREG_DBI_TYPEC_CFG_MSB                                               15
#define DCREG_DBI_TYPEC_CFG_LSB                                                1
#define DCREG_DBI_TYPEC_CFG_BLK                                                1
#define DCREG_DBI_TYPEC_CFG_Count                                              2
#define DCREG_DBI_TYPEC_CFG_FieldMask                                 0x001FFFFF
#define DCREG_DBI_TYPEC_CFG_ReadMask                                  0x001FFFFF
#define DCREG_DBI_TYPEC_CFG_WriteMask                                 0x001FFFFF
#define DCREG_DBI_TYPEC_CFG_ResetValue                                0x00000000

/* Determine the source of output Scl.                                        **
** DIVIED_SDA_CLK follows the SCL_TWRL and SCL_TWRH setting.                  **
** SDA_CLK means output 1 bit per SdaClk, will ignore SCL_TWRL and SCL_TWRH.  */
#define DCREG_DBI_TYPEC_CFG_SCL_SEL                                        20:20
#define DCREG_DBI_TYPEC_CFG_SCL_SEL_End                                       20
#define DCREG_DBI_TYPEC_CFG_SCL_SEL_Start                                     20
#define DCREG_DBI_TYPEC_CFG_SCL_SEL_Type                                     U01
#define   DCREG_DBI_TYPEC_CFG_SCL_SEL_DIVIDED_SDA_CLK                        0x0
#define   DCREG_DBI_TYPEC_CFG_SCL_SEL_SDA_CLK                                0x1

/* SCL Twrh means number of DBI_AC_TIME_UNIT of sdaClk for scl staying low    **
** This field need to be at least 1                                           */
#define DCREG_DBI_TYPEC_CFG_SCL_TWRH                                       19:12
#define DCREG_DBI_TYPEC_CFG_SCL_TWRH_End                                      19
#define DCREG_DBI_TYPEC_CFG_SCL_TWRH_Start                                    12
#define DCREG_DBI_TYPEC_CFG_SCL_TWRH_Type                                    U08

/* SCL Twrl menas number of DBI_AC_TIME_UNIT of sdaClk for scl staying high   **
** This field need to be at least 1                                           */
#define DCREG_DBI_TYPEC_CFG_SCL_TWRL                                        11:4
#define DCREG_DBI_TYPEC_CFG_SCL_TWRL_End                                      11
#define DCREG_DBI_TYPEC_CFG_SCL_TWRL_Start                                     4
#define DCREG_DBI_TYPEC_CFG_SCL_TWRL_Type                                    U08

/* DCX Tas means number of DBI_AC_TIME_UNIT of sdaClk to be ahead of scl.     **
** Only works for Option3. Option1/2 will ignore this setting. This field     **
** need to be at least 1                                                      */
#define DCREG_DBI_TYPEC_CFG_TAS                                              3:0
#define DCREG_DBI_TYPEC_CFG_TAS_End                                            3
#define DCREG_DBI_TYPEC_CFG_TAS_Start                                          0
#define DCREG_DBI_TYPEC_CFG_TAS_Type                                         U04

/* Register dcregDCStatus (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregDCStatusRegAddrs                                             0x0532
#define DCREG_DC_STATUS_Address                                          0x014C8
#define DCREG_DC_STATUS_MSB                                                   15
#define DCREG_DC_STATUS_LSB                                                    1
#define DCREG_DC_STATUS_BLK                                                    1
#define DCREG_DC_STATUS_Count                                                  2
#define DCREG_DC_STATUS_FieldMask                                     0x00000001
#define DCREG_DC_STATUS_ReadMask                                      0x00000001
#define DCREG_DC_STATUS_WriteMask                                     0x00000000
#define DCREG_DC_STATUS_ResetValue                                    0x00000000

/* TypeC afifo full */
#define DCREG_DC_STATUS_DBI_TYPEC_FIFO_FULL                                  0:0
#define DCREG_DC_STATUS_DBI_TYPEC_FIFO_FULL_End                                0
#define DCREG_DC_STATUS_DBI_TYPEC_FIFO_FULL_Start                              0
#define DCREG_DC_STATUS_DBI_TYPEC_FIFO_FULL_Type                             U01

/* Register dcregDebugCounterSelect (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregDebugCounterSelectRegAddrs                                   0x0534
#define DCREG_DEBUG_COUNTER_SELECT_Address                               0x014D0
#define DCREG_DEBUG_COUNTER_SELECT_MSB                                        15
#define DCREG_DEBUG_COUNTER_SELECT_LSB                                         1
#define DCREG_DEBUG_COUNTER_SELECT_BLK                                         1
#define DCREG_DEBUG_COUNTER_SELECT_Count                                       2
#define DCREG_DEBUG_COUNTER_SELECT_FieldMask                          0x000000FF
#define DCREG_DEBUG_COUNTER_SELECT_ReadMask                           0x000000FF
#define DCREG_DEBUG_COUNTER_SELECT_WriteMask                          0x000000FF
#define DCREG_DEBUG_COUNTER_SELECT_ResetValue                         0x00000000

/* Write a value to this field to pick up from 0~255 counters.                **
** Then the counter will be on dcregDebugCounterValue.                        **
** 8'h0 : TotalAXIVideoRdReqCnt     //request number                          **
** 8'h1 : TotalAXIVideoRdLastCnt    //read return data last number            **
** 8'h2 : TotalAXIVideoReqBurstCnt  //number of 16bytes of reqeust bytes      **
** 8'h3 : TotalAXIVideoRdBurstCunt  //number of 16bytes of read return data   **
** 8'h4 : TotalPixelCnt        //total pixels sent                            **
** 8'h5 : TotalFrameCnt        //total frame sent                             **
** 8'h6 : TotalInputDbiCmdCnt  //total dbi input cmd                          **
** 8'h7 : TotalOutputDbiCmdCnt //total dbi output cmd                         **
** 8'h8 : DebugSignals0        //debug signals                                **
** 8'h9 : TotalAXIOverlay0RdReqCnt     //request number                       **
** 8'h10 : TotalAXIOverlay0RdLastCnt    //read return data last number        **
** 8'h11 : TotalAXIOverlay0ReqBurstCnt  //number of 16bytes of reqeust bytes  **
** 8'h12 : TotalAXIOverlay0RdBurstCunt  //number of 16bytes of read return    **
** data                                                                       **
** 8'hFF: reset all debug counters                                            */
#define DCREG_DEBUG_COUNTER_SELECT_SELECT                                    7:0
#define DCREG_DEBUG_COUNTER_SELECT_SELECT_End                                  7
#define DCREG_DEBUG_COUNTER_SELECT_SELECT_Start                                0
#define DCREG_DEBUG_COUNTER_SELECT_SELECT_Type                               U08
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_VIDEO_RD_REQ_CNT      0x00
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_VIDEO_RD_LAST_CNT     0x01
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_VIDEO_REQ_BURST_CNT   0x02
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_VIDEO_RD_BURST_CUNT   0x03
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_PIXEL_CNT                 0x04
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_FRAME_CNT                 0x05
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_INPUT_DBI_CMD_CNT         0x06
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_OUTPUT_DBI_CMD_CNT        0x07
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS0                  0x08
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_OVERLAY0_RD_REQ_CNT   0x09
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_OVERLAY0_RD_LAST_CNT  0x0A
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_OVERLAY0_REQ_BURST_CNT 0x0B
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_TOTAL_AXI_OVERLAY0_RD_BURST_CUNT 0x0C
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_FREE_POOL         0x0D
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_INFOBUF_RD        0x0E
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_INFOBUF_WR0       0x0F
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_INFOBUF_WR1       0x10
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_INFOBUF_WR2       0x11
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_INFOBUF_WR3       0x12
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_OVERLAY0_FREE_POOL 0x13
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_OVERLAY0_INFOBUF_RD 0x14
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_OVERLAY0_INFOBUF_WR0 0x15
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_OVERLAY0_INFOBUF_WR1 0x16
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_OVERLAY0_INFOBUF_WR2 0x17
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_DEBUG_SIGNALS_OVERLAY0_INFOBUF_WR3 0x18
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_OVERLAY_BLEND_DEBUGSIGNALS      0x19
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_CURSOR_BLEND_DEBUGSIGNALS       0x1A
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_SCALER_DEBUGSIGNALS             0x1B
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_VIDEO_WALKER_DEBUGSIGNALS_1     0x1C
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_VIDEO_WALKER_DEBUGSIGNALS_2     0x1D
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_VIDEO_WALKER_DEBUGSIGNALS_3     0x1E
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_OVERLAY_WALKER_DEBUGSIGNALS_1   0x1F
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_OVERLAY_WALKER_DEBUGSIGNALS_2   0x20
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_OVERLAY_WALKER_DEBUGSIGNALS_3   0x21
#define   DCREG_DEBUG_COUNTER_SELECT_SELECT_RESET_ALL_DEBUG_COUNTERS        0xFF

/* Register dcregDebugCounterValue (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregDebugCounterValueRegAddrs                                    0x0536
#define DCREG_DEBUG_COUNTER_VALUE_Address                                0x014D8
#define DCREG_DEBUG_COUNTER_VALUE_MSB                                         15
#define DCREG_DEBUG_COUNTER_VALUE_LSB                                          1
#define DCREG_DEBUG_COUNTER_VALUE_BLK                                          1
#define DCREG_DEBUG_COUNTER_VALUE_Count                                        2
#define DCREG_DEBUG_COUNTER_VALUE_FieldMask                           0xFFFFFFFF
#define DCREG_DEBUG_COUNTER_VALUE_ReadMask                            0xFFFFFFFF
#define DCREG_DEBUG_COUNTER_VALUE_WriteMask                           0x00000000
#define DCREG_DEBUG_COUNTER_VALUE_ResetValue                          0x00000000

/* Selected debug counter value */
#define DCREG_DEBUG_COUNTER_VALUE_VALUE                                     31:0
#define DCREG_DEBUG_COUNTER_VALUE_VALUE_End                                   31
#define DCREG_DEBUG_COUNTER_VALUE_VALUE_Start                                  0
#define DCREG_DEBUG_COUNTER_VALUE_VALUE_Type                                 U32

/* Register dcregDebugCompose (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregDebugComposeRegAddrs                                         0x0538
#define DCREG_DEBUG_COMPOSE_Address                                      0x014E0
#define DCREG_DEBUG_COMPOSE_MSB                                               15
#define DCREG_DEBUG_COMPOSE_LSB                                                1
#define DCREG_DEBUG_COMPOSE_BLK                                                1
#define DCREG_DEBUG_COMPOSE_Count                                              2
#define DCREG_DEBUG_COMPOSE_FieldMask                                 0x00000006
#define DCREG_DEBUG_COMPOSE_ReadMask                                  0x00000000
#define DCREG_DEBUG_COMPOSE_WriteMask                                 0x00000006
#define DCREG_DEBUG_COMPOSE_ResetValue                                0x00000000

#define DCREG_DEBUG_COMPOSE_TRIGGER                                          1:1
#define DCREG_DEBUG_COMPOSE_TRIGGER_End                                        1
#define DCREG_DEBUG_COMPOSE_TRIGGER_Start                                      1
#define DCREG_DEBUG_COMPOSE_TRIGGER_Type                                     U01

#define DCREG_DEBUG_COMPOSE_CLEAR_MPU_INTF_COUNTER                           2:2
#define DCREG_DEBUG_COMPOSE_CLEAR_MPU_INTF_COUNTER_End                         2
#define DCREG_DEBUG_COMPOSE_CLEAR_MPU_INTF_COUNTER_Start                       2
#define DCREG_DEBUG_COMPOSE_CLEAR_MPU_INTF_COUNTER_Type                      U01

/* Register dcregMemDestAddress (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Starting address of the memory to memory path dest buffer. NOTE: This      **
** register is double buffered.                                               */

#define dcregMemDestAddressRegAddrs                                       0x053A
#define DCREG_MEM_DEST_ADDRESS_Address                                   0x014E8
#define DCREG_MEM_DEST_ADDRESS_MSB                                            15
#define DCREG_MEM_DEST_ADDRESS_LSB                                             1
#define DCREG_MEM_DEST_ADDRESS_BLK                                             1
#define DCREG_MEM_DEST_ADDRESS_Count                                           2
#define DCREG_MEM_DEST_ADDRESS_FieldMask                              0xFFFFFFFF
#define DCREG_MEM_DEST_ADDRESS_ReadMask                               0xFFFFFFFF
#define DCREG_MEM_DEST_ADDRESS_WriteMask                              0xFFFFFFFF
#define DCREG_MEM_DEST_ADDRESS_ResetValue                             0x00000000

#define DCREG_MEM_DEST_ADDRESS_ADDRESS                                      31:0
#define DCREG_MEM_DEST_ADDRESS_ADDRESS_End                                    31
#define DCREG_MEM_DEST_ADDRESS_ADDRESS_Start                                   0
#define DCREG_MEM_DEST_ADDRESS_ADDRESS_Type                                  U32

/* Register dcregPanelDestAddress (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Starting address of the panel out path dest buffer. It is used for         **
** debugging. NOTE: This register is double buffered.                         */

#define dcregPanelDestAddressRegAddrs                                     0x053C
#define DCREG_PANEL_DEST_ADDRESS_Address                                 0x014F0
#define DCREG_PANEL_DEST_ADDRESS_MSB                                          15
#define DCREG_PANEL_DEST_ADDRESS_LSB                                           1
#define DCREG_PANEL_DEST_ADDRESS_BLK                                           1
#define DCREG_PANEL_DEST_ADDRESS_Count                                         2
#define DCREG_PANEL_DEST_ADDRESS_FieldMask                            0xFFFFFFFF
#define DCREG_PANEL_DEST_ADDRESS_ReadMask                             0xFFFFFFFF
#define DCREG_PANEL_DEST_ADDRESS_WriteMask                            0xFFFFFFFF
#define DCREG_PANEL_DEST_ADDRESS_ResetValue                           0x00000000

#define DCREG_PANEL_DEST_ADDRESS_ADDRESS                                    31:0
#define DCREG_PANEL_DEST_ADDRESS_ADDRESS_End                                  31
#define DCREG_PANEL_DEST_ADDRESS_ADDRESS_Start                                 0
#define DCREG_PANEL_DEST_ADDRESS_ADDRESS_Type                                U32

/* Register dcregDestConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* NOTE: This register is double buffered. */

#define dcregDestConfigRegAddrs                                           0x053E
#define DCREG_DEST_CONFIG_Address                                        0x014F8
#define DCREG_DEST_CONFIG_MSB                                                 15
#define DCREG_DEST_CONFIG_LSB                                                  1
#define DCREG_DEST_CONFIG_BLK                                                  1
#define DCREG_DEST_CONFIG_Count                                                2
#define DCREG_DEST_CONFIG_FieldMask                                   0x0003FFFF
#define DCREG_DEST_CONFIG_ReadMask                                    0x0003FFFF
#define DCREG_DEST_CONFIG_WriteMask                                   0x0003FFFF
#define DCREG_DEST_CONFIG_ResetValue                                  0x00000000

/* The format of the destination. */
#define DCREG_DEST_CONFIG_FORMAT                                            15:0
#define DCREG_DEST_CONFIG_FORMAT_End                                          15
#define DCREG_DEST_CONFIG_FORMAT_Start                                         0
#define DCREG_DEST_CONFIG_FORMAT_Type                                        U16
#define   DCREG_DEST_CONFIG_FORMAT_X4R4G4B4                               0x0000
#define   DCREG_DEST_CONFIG_FORMAT_A4R4G4B4                               0x0001
#define   DCREG_DEST_CONFIG_FORMAT_X1R5G5B5                               0x0002
#define   DCREG_DEST_CONFIG_FORMAT_A1R5G5B5                               0x0003
#define   DCREG_DEST_CONFIG_FORMAT_R5G6B5                                 0x0004
#define   DCREG_DEST_CONFIG_FORMAT_X8R8G8B8                               0x0005
#define   DCREG_DEST_CONFIG_FORMAT_A8R8G8B8                               0x0006
#define   DCREG_DEST_CONFIG_FORMAT_YUY2                                   0x0007
#define   DCREG_DEST_CONFIG_FORMAT_UYVY                                   0x0008
#define   DCREG_DEST_CONFIG_FORMAT_INDEX8                                 0x0009
#define   DCREG_DEST_CONFIG_FORMAT_MONOCHROME                             0x000A
#define   DCREG_DEST_CONFIG_FORMAT_YV12                                   0x000F
#define   DCREG_DEST_CONFIG_FORMAT_A8                                     0x0010
#define   DCREG_DEST_CONFIG_FORMAT_NV12                                   0x0011
#define   DCREG_DEST_CONFIG_FORMAT_NV16                                   0x0012
#define   DCREG_DEST_CONFIG_FORMAT_RG16                                   0x0013
#define   DCREG_DEST_CONFIG_FORMAT_R8                                     0x0014
#define   DCREG_DEST_CONFIG_FORMAT_NV12_10BIT                             0x0015
#define   DCREG_DEST_CONFIG_FORMAT_A2R10G10B10                            0x0016
#define   DCREG_DEST_CONFIG_FORMAT_NV16_10BIT                             0x0017
#define   DCREG_DEST_CONFIG_FORMAT_INDEX1                                 0x0018
#define   DCREG_DEST_CONFIG_FORMAT_INDEX2                                 0x0019
#define   DCREG_DEST_CONFIG_FORMAT_INDEX4                                 0x001A
#define   DCREG_DEST_CONFIG_FORMAT_P010                                   0x001B
#define   DCREG_DEST_CONFIG_FORMAT_NV12_10BIT_L1                          0x001C
#define   DCREG_DEST_CONFIG_FORMAT_NV16_10BIT_L1                          0x001D
#define   DCREG_DEST_CONFIG_FORMAT_I010                                   0x001E

#define DCREG_DEST_CONFIG_ENABLE                                           16:16
#define DCREG_DEST_CONFIG_ENABLE_End                                          16
#define DCREG_DEST_CONFIG_ENABLE_Start                                        16
#define DCREG_DEST_CONFIG_ENABLE_Type                                        U01
#define   DCREG_DEST_CONFIG_ENABLE_DISABLE                                   0x0
#define   DCREG_DEST_CONFIG_ENABLE_ENABLE                                    0x1

/* If enabled, result include dither, gamma and cursor. */
#define DCREG_DEST_CONFIG_INCLUDE_POST_PROCESS                             17:17
#define DCREG_DEST_CONFIG_INCLUDE_POST_PROCESS_End                            17
#define DCREG_DEST_CONFIG_INCLUDE_POST_PROCESS_Start                          17
#define DCREG_DEST_CONFIG_INCLUDE_POST_PROCESS_Type                          U01
#define   DCREG_DEST_CONFIG_INCLUDE_POST_PROCESS_DISABLE                     0x0
#define   DCREG_DEST_CONFIG_INCLUDE_POST_PROCESS_ENABLE                      0x1

/* Register dcregDestStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Stride of the dest buffer in bytes. NOTE: This register is double          **
** buffered.                                                                  */

#define dcregDestStrideRegAddrs                                           0x0540
#define DCREG_DEST_STRIDE_Address                                        0x01500
#define DCREG_DEST_STRIDE_MSB                                                 15
#define DCREG_DEST_STRIDE_LSB                                                  1
#define DCREG_DEST_STRIDE_BLK                                                  1
#define DCREG_DEST_STRIDE_Count                                                2
#define DCREG_DEST_STRIDE_FieldMask                                   0x0003FFFF
#define DCREG_DEST_STRIDE_ReadMask                                    0x0003FFFF
#define DCREG_DEST_STRIDE_WriteMask                                   0x0003FFFF
#define DCREG_DEST_STRIDE_ResetValue                                  0x00000000

/* Number of bytes from start of one line to next line. */
#define DCREG_DEST_STRIDE_STRIDE                                            17:0
#define DCREG_DEST_STRIDE_STRIDE_End                                          17
#define DCREG_DEST_STRIDE_STRIDE_Start                                         0
#define DCREG_DEST_STRIDE_STRIDE_Type                                        U18

/* Register dcregFrameBufferColorKey (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Start of color key range of framebuffer. NOTE: This register is double     **
** buffered.                                                                  */

#define dcregFrameBufferColorKeyRegAddrs                                  0x0542
#define DCREG_FRAME_BUFFER_COLOR_KEY_Address                             0x01508
#define DCREG_FRAME_BUFFER_COLOR_KEY_MSB                                      15
#define DCREG_FRAME_BUFFER_COLOR_KEY_LSB                                       1
#define DCREG_FRAME_BUFFER_COLOR_KEY_BLK                                       1
#define DCREG_FRAME_BUFFER_COLOR_KEY_Count                                     2
#define DCREG_FRAME_BUFFER_COLOR_KEY_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_BUFFER_COLOR_KEY_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_BUFFER_COLOR_KEY_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_BUFFER_COLOR_KEY_ResetValue                       0x00000000

#define DCREG_FRAME_BUFFER_COLOR_KEY_BLUE                                    7:0
#define DCREG_FRAME_BUFFER_COLOR_KEY_BLUE_End                                  7
#define DCREG_FRAME_BUFFER_COLOR_KEY_BLUE_Start                                0
#define DCREG_FRAME_BUFFER_COLOR_KEY_BLUE_Type                               U08

#define DCREG_FRAME_BUFFER_COLOR_KEY_GREEN                                  15:8
#define DCREG_FRAME_BUFFER_COLOR_KEY_GREEN_End                                15
#define DCREG_FRAME_BUFFER_COLOR_KEY_GREEN_Start                               8
#define DCREG_FRAME_BUFFER_COLOR_KEY_GREEN_Type                              U08

#define DCREG_FRAME_BUFFER_COLOR_KEY_RED                                   23:16
#define DCREG_FRAME_BUFFER_COLOR_KEY_RED_End                                  23
#define DCREG_FRAME_BUFFER_COLOR_KEY_RED_Start                                16
#define DCREG_FRAME_BUFFER_COLOR_KEY_RED_Type                                U08

#define DCREG_FRAME_BUFFER_COLOR_KEY_ALPHA                                 31:24
#define DCREG_FRAME_BUFFER_COLOR_KEY_ALPHA_End                                31
#define DCREG_FRAME_BUFFER_COLOR_KEY_ALPHA_Start                              24
#define DCREG_FRAME_BUFFER_COLOR_KEY_ALPHA_Type                              U08

/* Register dcregFrameBufferColorKeyHigh (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* End of color key range of framebuffer. NOTE: This register is double       **
** buffered.                                                                  */

#define dcregFrameBufferColorKeyHighRegAddrs                              0x0544
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_Address                        0x01510
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_MSB                                 15
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_LSB                                  1
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_BLK                                  1
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_Count                                2
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_FieldMask                   0xFFFFFFFF
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_ReadMask                    0xFFFFFFFF
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_WriteMask                   0xFFFFFFFF
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_ResetValue                  0x00000000

#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_BLUE                               7:0
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_BLUE_End                             7
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_BLUE_Start                           0
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_BLUE_Type                          U08

#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_GREEN                             15:8
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_GREEN_End                           15
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_GREEN_Start                          8
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_GREEN_Type                         U08

#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_RED                              23:16
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_RED_End                             23
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_RED_Start                           16
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_RED_Type                           U08

#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_ALPHA                            31:24
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_ALPHA_End                           31
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_ALPHA_Start                         24
#define DCREG_FRAME_BUFFER_COLOR_KEY_HIGH_ALPHA_Type                         U08

/* Register dcregFrameBufferConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Frame buffer attribute configure. NOTE: This register is double buffered.  **
** Some fields are double buffered. Some bits are READ ONLY or WRITE ONLY     */

#define dcregFrameBufferConfigRegAddrs                                    0x0546
#define DCREG_FRAME_BUFFER_CONFIG_Address                                0x01518
#define DCREG_FRAME_BUFFER_CONFIG_MSB                                         15
#define DCREG_FRAME_BUFFER_CONFIG_LSB                                          1
#define DCREG_FRAME_BUFFER_CONFIG_BLK                                          1
#define DCREG_FRAME_BUFFER_CONFIG_Count                                        2
#define DCREG_FRAME_BUFFER_CONFIG_FieldMask                           0xFFFFFFFF
#define DCREG_FRAME_BUFFER_CONFIG_ReadMask                            0xFFFFFFEF
#define DCREG_FRAME_BUFFER_CONFIG_WriteMask                           0xFFFFFF9F
#define DCREG_FRAME_BUFFER_CONFIG_ResetValue                          0x00000000

/* When Output is enabled, pixels will be displayed.  When Output is          **
** disabled, all pixels will be black.  This allows a panel to have correct   **
** timing but without any pixels. NOTE: This field is double buffered.        */
#define DCREG_FRAME_BUFFER_CONFIG_OUTPUT                                     0:0
#define DCREG_FRAME_BUFFER_CONFIG_OUTPUT_End                                   0
#define DCREG_FRAME_BUFFER_CONFIG_OUTPUT_Start                                 0
#define DCREG_FRAME_BUFFER_CONFIG_OUTPUT_Type                                U01
#define   DCREG_FRAME_BUFFER_CONFIG_OUTPUT_DISABLED                          0x0
#define   DCREG_FRAME_BUFFER_CONFIG_OUTPUT_ENABLED                           0x1

/* When SwitchPanel of panel 0 is enabled, output channel 0 will show the     **
** image of panel 1,not panel 0's.                                            **
** When SwitchPanel of panel 0 is disabeld,output channel 0 will show its     **
** original image which is from panel 0.                                      **
** When SwitchPanel of panel 1 is enabled, output channel 1 will show the     **
** image of panel 0,not panel 1's.                                            **
** When SwitchPanle of panel 1 is disabled,output channel 1 will show the     **
** original image which is from panel 1.                                      **
**                                                                            **
**                            disabled(normal)            enabled(switch)     **
** output channel 0            panel 0                            panel 1     **
** output channel 1            panel 1                            panel 0     */
#define DCREG_FRAME_BUFFER_CONFIG_SWITCHPANEL                                1:1
#define DCREG_FRAME_BUFFER_CONFIG_SWITCHPANEL_End                              1
#define DCREG_FRAME_BUFFER_CONFIG_SWITCHPANEL_Start                            1
#define DCREG_FRAME_BUFFER_CONFIG_SWITCHPANEL_Type                           U01
#define   DCREG_FRAME_BUFFER_CONFIG_SWITCHPANEL_DISABLED                     0x0
#define   DCREG_FRAME_BUFFER_CONFIG_SWITCHPANEL_ENABLED                      0x1

/* When Gamma is enabled, the R, G, and B channels will be routed through the **
** Gamma LUT to perform gamma correction. NOTE: This field is double          **
** buffered.                                                                  */
#define DCREG_FRAME_BUFFER_CONFIG_GAMMA                                      2:2
#define DCREG_FRAME_BUFFER_CONFIG_GAMMA_End                                    2
#define DCREG_FRAME_BUFFER_CONFIG_GAMMA_Start                                  2
#define DCREG_FRAME_BUFFER_CONFIG_GAMMA_Type                                 U01
#define   DCREG_FRAME_BUFFER_CONFIG_GAMMA_DISABLED                           0x0
#define   DCREG_FRAME_BUFFER_CONFIG_GAMMA_ENABLED                     (0x1 << 2)

/* The valid field defines whether we can copy a new set of registers at the  **
** next VBLANK or not.  This ensures a frame will always start with a valid   **
** working set if this register is programmed last, which reduces the need    **
** for SW to wait for the start of a VBLANK signal in order to ensure all     **
** states are loaded before the next VBLANK.                                  */
#define DCREG_FRAME_BUFFER_CONFIG_VALID                                      3:3
#define DCREG_FRAME_BUFFER_CONFIG_VALID_End                                    3
#define DCREG_FRAME_BUFFER_CONFIG_VALID_Start                                  3
#define DCREG_FRAME_BUFFER_CONFIG_VALID_Type                                 U01
#define   DCREG_FRAME_BUFFER_CONFIG_VALID_WORKING                            0x0
#define   DCREG_FRAME_BUFFER_CONFIG_VALID_PENDING                            0x1

/* Enable reset for the display controller. 0:See below for DPI. For DBI,this **
** field should be 0. 1:RESET. Enable DPI timing, start a DPI transfer. Write **
** 0 to this bit to reset the display controller, then configure the other    **
** registers and lastly write a 1 to this bit to let the display controller   **
** start. When the display controller starts, it begins at VBLANK_START, and  **
** all register get flopped to the working set at VSYNC_END. Counters will be **
** reset to the end of  HSYNC and VSYNC (Refer the Timing Diagram provided in **
** the Hardware Feature document). For DBI, do not write 1 to this field. Use **
** dcregDbiReset DBI_IF_LEVEL_RESET instead. This bit is WRITE ONLY.          */
#define DCREG_FRAME_BUFFER_CONFIG_RESET                                      4:4
#define DCREG_FRAME_BUFFER_CONFIG_RESET_End                                    4
#define DCREG_FRAME_BUFFER_CONFIG_RESET_Start                                  4
#define DCREG_FRAME_BUFFER_CONFIG_RESET_Type                                 U01
#define   DCREG_FRAME_BUFFER_CONFIG_RESET_RESET                              0x1

/* 0: NO. 1:YES. When the display FIFO underflows, this bit gets set to one.  **
** Reading this register will reset it back to zero. This field is READ ONLY. */
#define DCREG_FRAME_BUFFER_CONFIG_UNDERFLOW                                  5:5
#define DCREG_FRAME_BUFFER_CONFIG_UNDERFLOW_End                                5
#define DCREG_FRAME_BUFFER_CONFIG_UNDERFLOW_Start                              5
#define DCREG_FRAME_BUFFER_CONFIG_UNDERFLOW_Type                             U01
#define   DCREG_FRAME_BUFFER_CONFIG_UNDERFLOW_NO                             0x0
#define   DCREG_FRAME_BUFFER_CONFIG_UNDERFLOW_YES                            0x1

/* 0: NO. 1:YES. When the frame buffer address gets written to, this bit gets **
** set to one.  It will be reset to zero at the start of the next VBLANK when **
** the register gets copied into the working set. This field is READ ONLY.    */
#define DCREG_FRAME_BUFFER_CONFIG_FLIP_IN_PROGRESS                           6:6
#define DCREG_FRAME_BUFFER_CONFIG_FLIP_IN_PROGRESS_End                         6
#define DCREG_FRAME_BUFFER_CONFIG_FLIP_IN_PROGRESS_Start                       6
#define DCREG_FRAME_BUFFER_CONFIG_FLIP_IN_PROGRESS_Type                      U01
#define   DCREG_FRAME_BUFFER_CONFIG_FLIP_IN_PROGRESS_NO                      0x0
#define   DCREG_FRAME_BUFFER_CONFIG_FLIP_IN_PROGRESS_YES                     0x1

/* If enabled, send dtrc small burst for frame buffer. */
#define DCREG_FRAME_BUFFER_CONFIG_DTRC                                       7:7
#define DCREG_FRAME_BUFFER_CONFIG_DTRC_End                                     7
#define DCREG_FRAME_BUFFER_CONFIG_DTRC_Start                                   7
#define DCREG_FRAME_BUFFER_CONFIG_DTRC_Type                                  U01
#define   DCREG_FRAME_BUFFER_CONFIG_DTRC_DISABLED                            0x0
#define   DCREG_FRAME_BUFFER_CONFIG_DTRC_ENABLED                             0x1

/* When enabled, pixel value of framebuffer comes from                        **
** decregFrameBufferClearValue, otherwise pixel value comes from memory.      */
#define DCREG_FRAME_BUFFER_CONFIG_CLEAR                                      8:8
#define DCREG_FRAME_BUFFER_CONFIG_CLEAR_End                                    8
#define DCREG_FRAME_BUFFER_CONFIG_CLEAR_Start                                  8
#define DCREG_FRAME_BUFFER_CONFIG_CLEAR_Type                                 U01
#define   DCREG_FRAME_BUFFER_CONFIG_CLEAR_DISABLED                           0x0
#define   DCREG_FRAME_BUFFER_CONFIG_CLEAR_ENABLED                            0x1

/* Transparency of framebuffer.  */
#define DCREG_FRAME_BUFFER_CONFIG_TRANSPARENCY                              10:9
#define DCREG_FRAME_BUFFER_CONFIG_TRANSPARENCY_End                            10
#define DCREG_FRAME_BUFFER_CONFIG_TRANSPARENCY_Start                           9
#define DCREG_FRAME_BUFFER_CONFIG_TRANSPARENCY_Type                          U02
#define   DCREG_FRAME_BUFFER_CONFIG_TRANSPARENCY_OPAQUE                      0x0
#define   DCREG_FRAME_BUFFER_CONFIG_TRANSPARENCY_MASK                        0x1
#define   DCREG_FRAME_BUFFER_CONFIG_TRANSPARENCY_KEY                         0x2

#define DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE                                13:11
#define DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_End                               13
#define DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_Start                             11
#define DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_Type                             U03
#define   DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_ROT0                           0x0
#define   DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_FLIP_X                         0x1
#define   DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_FLIP_Y                         0x2
#define   DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_FLIP_XY                        0x3
#define   DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_ROT90                          0x4
#define   DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_ROT180                         0x5
#define   DCREG_FRAME_BUFFER_CONFIG_ROT_ANGLE_ROT270                         0x6

/* YUV standard */
#define DCREG_FRAME_BUFFER_CONFIG_YUV                                      16:14
#define DCREG_FRAME_BUFFER_CONFIG_YUV_End                                     16
#define DCREG_FRAME_BUFFER_CONFIG_YUV_Start                                   14
#define DCREG_FRAME_BUFFER_CONFIG_YUV_Type                                   U03
#define   DCREG_FRAME_BUFFER_CONFIG_YUV_709                                  0x1
#define   DCREG_FRAME_BUFFER_CONFIG_YUV_2020                                 0x3

#define DCREG_FRAME_BUFFER_CONFIG_TILE_MODE                                21:17
#define DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_End                               21
#define DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_Start                             17
#define DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_Type                             U05
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_LINEAR                        0x00
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILED4X4                      0x01
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_SUPER_TILED_XMAJOR            0x02
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_SUPER_TILED_YMAJOR            0x03
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE8X8                  0x04
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE1                    0x05
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE2                    0x06
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE8X4                  0x07
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE4                    0x08
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE5                    0x09
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE6                    0x0A
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_SUPER_TILED_XMAJOR8X4         0x0B
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_SUPER_TILED_YMAJOR4X8         0x0C
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_Y                        0x0D
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILE_MODE7                    0x0E
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILED128X1                    0x0F
#define   DCREG_FRAME_BUFFER_CONFIG_TILE_MODE_TILED256X1                    0x10

#define DCREG_FRAME_BUFFER_CONFIG_SCALE                                    22:22
#define DCREG_FRAME_BUFFER_CONFIG_SCALE_End                                   22
#define DCREG_FRAME_BUFFER_CONFIG_SCALE_Start                                 22
#define DCREG_FRAME_BUFFER_CONFIG_SCALE_Type                                 U01
#define   DCREG_FRAME_BUFFER_CONFIG_SCALE_DISABLE                            0x0
#define   DCREG_FRAME_BUFFER_CONFIG_SCALE_ENABLE                             0x1

#define DCREG_FRAME_BUFFER_CONFIG_SWIZZLE                                  24:23
#define DCREG_FRAME_BUFFER_CONFIG_SWIZZLE_End                                 24
#define DCREG_FRAME_BUFFER_CONFIG_SWIZZLE_Start                               23
#define DCREG_FRAME_BUFFER_CONFIG_SWIZZLE_Type                               U02
#define   DCREG_FRAME_BUFFER_CONFIG_SWIZZLE_ARGB                             0x0
#define   DCREG_FRAME_BUFFER_CONFIG_SWIZZLE_RGBA                             0x1
#define   DCREG_FRAME_BUFFER_CONFIG_SWIZZLE_ABGR                             0x2
#define   DCREG_FRAME_BUFFER_CONFIG_SWIZZLE_BGRA                             0x3

#define DCREG_FRAME_BUFFER_CONFIG_UV_SWIZZLE                               25:25
#define DCREG_FRAME_BUFFER_CONFIG_UV_SWIZZLE_End                              25
#define DCREG_FRAME_BUFFER_CONFIG_UV_SWIZZLE_Start                            25
#define DCREG_FRAME_BUFFER_CONFIG_UV_SWIZZLE_Type                            U01

/* The format of the framebuffer. */
#define DCREG_FRAME_BUFFER_CONFIG_FORMAT                                   31:26
#define DCREG_FRAME_BUFFER_CONFIG_FORMAT_End                                  31
#define DCREG_FRAME_BUFFER_CONFIG_FORMAT_Start                                26
#define DCREG_FRAME_BUFFER_CONFIG_FORMAT_Type                                U06
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_X4R4G4B4                         0x00
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_A4R4G4B4                         0x01
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_X1R5G5B5                         0x02
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_A1R5G5B5                         0x03
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_R5G6B5                           0x04
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_X8R8G8B8                         0x05
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_A8R8G8B8                         0x06
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_YUY2                             0x07
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_UYVY                             0x08
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_INDEX8                           0x09
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_MONOCHROME                       0x0A
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_YV12                             0x0F
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_A8                               0x10
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_NV12                             0x11
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_NV16                             0x12
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_RG16                             0x13
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_R8                               0x14
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_NV12_10BIT                       0x15
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_A2R10G10B10                      0x16
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_NV16_10BIT                       0x17
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_INDEX1                           0x18
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_INDEX2                           0x19
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_INDEX4                           0x1A
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_P010                             0x1B
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_NV12_10BIT_L1                    0x1C
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_NV16_10BIT_L1                    0x1D
#define   DCREG_FRAME_BUFFER_CONFIG_FORMAT_I010                             0x1E

/* Register dcregFrameBufferScaleConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Frame buffer scaling tap configure. NOTE: This register is double          **
** buffered.                                                                  */

#define dcregFrameBufferScaleConfigRegAddrs                               0x0548
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_Address                          0x01520
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_MSB                                   15
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_LSB                                    1
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_BLK                                    1
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_Count                                  2
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_FieldMask                     0x000000FF
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_ReadMask                      0x000000FF
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_WriteMask                     0x000000FF
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_ResetValue                    0x00000000

#define DCREG_FRAME_BUFFER_SCALE_CONFIG_FILTER_TAP                           3:0
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_FILTER_TAP_End                         3
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_FILTER_TAP_Start                       0
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_FILTER_TAP_Type                      U04

#define DCREG_FRAME_BUFFER_SCALE_CONFIG_HORIZONTAL_FILTER_TAP                7:4
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_HORIZONTAL_FILTER_TAP_End              7
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_HORIZONTAL_FILTER_TAP_Start            4
#define DCREG_FRAME_BUFFER_SCALE_CONFIG_HORIZONTAL_FILTER_TAP_Type           U04

/* Register dcregFrameBufferBGColor (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Background color used when a pixel from frame buffer falls the range of    **
** color key. NOTE: This register is double buffered.                         */

#define dcregFrameBufferBGColorRegAddrs                                   0x054A
#define DCREG_FRAME_BUFFER_BG_COLOR_Address                              0x01528
#define DCREG_FRAME_BUFFER_BG_COLOR_MSB                                       15
#define DCREG_FRAME_BUFFER_BG_COLOR_LSB                                        1
#define DCREG_FRAME_BUFFER_BG_COLOR_BLK                                        1
#define DCREG_FRAME_BUFFER_BG_COLOR_Count                                      2
#define DCREG_FRAME_BUFFER_BG_COLOR_FieldMask                         0xFFFFFFFF
#define DCREG_FRAME_BUFFER_BG_COLOR_ReadMask                          0xFFFFFFFF
#define DCREG_FRAME_BUFFER_BG_COLOR_WriteMask                         0xFFFFFFFF
#define DCREG_FRAME_BUFFER_BG_COLOR_ResetValue                        0x00000000

#define DCREG_FRAME_BUFFER_BG_COLOR_BLUE                                     7:0
#define DCREG_FRAME_BUFFER_BG_COLOR_BLUE_End                                   7
#define DCREG_FRAME_BUFFER_BG_COLOR_BLUE_Start                                 0
#define DCREG_FRAME_BUFFER_BG_COLOR_BLUE_Type                                U08

#define DCREG_FRAME_BUFFER_BG_COLOR_GREEN                                   15:8
#define DCREG_FRAME_BUFFER_BG_COLOR_GREEN_End                                 15
#define DCREG_FRAME_BUFFER_BG_COLOR_GREEN_Start                                8
#define DCREG_FRAME_BUFFER_BG_COLOR_GREEN_Type                               U08

#define DCREG_FRAME_BUFFER_BG_COLOR_RED                                    23:16
#define DCREG_FRAME_BUFFER_BG_COLOR_RED_End                                   23
#define DCREG_FRAME_BUFFER_BG_COLOR_RED_Start                                 16
#define DCREG_FRAME_BUFFER_BG_COLOR_RED_Type                                 U08

#define DCREG_FRAME_BUFFER_BG_COLOR_ALPHA                                  31:24
#define DCREG_FRAME_BUFFER_BG_COLOR_ALPHA_End                                 31
#define DCREG_FRAME_BUFFER_BG_COLOR_ALPHA_Start                               24
#define DCREG_FRAME_BUFFER_BG_COLOR_ALPHA_Type                               U08

/* Register dcregFrameBufferUPlanarAddress (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Starting address of the second planar of frame buffer if exist. NOTE: This **
** register is double buffered.                                               */

#define dcregFrameBufferUPlanarAddressRegAddrs                            0x054C
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_Address                       0x01530
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_MSB                                15
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_LSB                                 1
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_BLK                                 1
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_Count                               2
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_FieldMask                  0xFFFFFFFF
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_ReadMask                   0xFFFFFFFF
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_WriteMask                  0xFFFFFFFF
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_ResetValue                 0x00000000

#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_ADDRESS                          31:0
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_ADDRESS_End                        31
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_ADDRESS_Start                       0
#define DCREG_FRAME_BUFFER_UPLANAR_ADDRESS_ADDRESS_Type                      U32

/* Register dcregFrameBufferVPlanarAddress (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Starting address of the third planar of frame buffer if exist. NOTE: This  **
** register is double buffered.                                               */

#define dcregFrameBufferVPlanarAddressRegAddrs                            0x054E
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_Address                       0x01538
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_MSB                                15
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_LSB                                 1
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_BLK                                 1
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_Count                               2
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_FieldMask                  0xFFFFFFFF
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_ReadMask                   0xFFFFFFFF
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_WriteMask                  0xFFFFFFFF
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_ResetValue                 0x00000000

#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_ADDRESS                          31:0
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_ADDRESS_End                        31
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_ADDRESS_Start                       0
#define DCREG_FRAME_BUFFER_VPLANAR_ADDRESS_ADDRESS_Type                      U32

/* Register dcregOverlayConfig (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Overlay attributes control. NOTE: This register is double buffered. */

#define dcregOverlayConfigRegAddrs                                        0x0550
#define DCREG_OVERLAY_CONFIG_Address                                     0x01540
#define DCREG_OVERLAY_CONFIG_MSB                                              15
#define DCREG_OVERLAY_CONFIG_LSB                                               4
#define DCREG_OVERLAY_CONFIG_BLK                                               4
#define DCREG_OVERLAY_CONFIG_Count                                            16
#define DCREG_OVERLAY_CONFIG_FieldMask                                0x7FBFFFFF
#define DCREG_OVERLAY_CONFIG_ReadMask                                 0x7FBFFFFF
#define DCREG_OVERLAY_CONFIG_WriteMask                                0x7F3FFFFF
#define DCREG_OVERLAY_CONFIG_ResetValue                               0x00000000

/* Transparency of the overlay. */
#define DCREG_OVERLAY_CONFIG_TRANSPARENCY                                    1:0
#define DCREG_OVERLAY_CONFIG_TRANSPARENCY_End                                  1
#define DCREG_OVERLAY_CONFIG_TRANSPARENCY_Start                                0
#define DCREG_OVERLAY_CONFIG_TRANSPARENCY_Type                               U02
#define   DCREG_OVERLAY_CONFIG_TRANSPARENCY_OPAQUE                           0x0
#define   DCREG_OVERLAY_CONFIG_TRANSPARENCY_MASK                             0x1
#define   DCREG_OVERLAY_CONFIG_TRANSPARENCY_KEY                              0x2

#define DCREG_OVERLAY_CONFIG_ROT_ANGLE                                       4:2
#define DCREG_OVERLAY_CONFIG_ROT_ANGLE_End                                     4
#define DCREG_OVERLAY_CONFIG_ROT_ANGLE_Start                                   2
#define DCREG_OVERLAY_CONFIG_ROT_ANGLE_Type                                  U03
#define   DCREG_OVERLAY_CONFIG_ROT_ANGLE_ROT0                                0x0
#define   DCREG_OVERLAY_CONFIG_ROT_ANGLE_FLIP_X                              0x1
#define   DCREG_OVERLAY_CONFIG_ROT_ANGLE_FLIP_Y                              0x2
#define   DCREG_OVERLAY_CONFIG_ROT_ANGLE_FLIP_XY                             0x3
#define   DCREG_OVERLAY_CONFIG_ROT_ANGLE_ROT90                               0x4
#define   DCREG_OVERLAY_CONFIG_ROT_ANGLE_ROT180                              0x5
#define   DCREG_OVERLAY_CONFIG_ROT_ANGLE_ROT270                              0x6

/* YUV standard */
#define DCREG_OVERLAY_CONFIG_YUV                                             7:5
#define DCREG_OVERLAY_CONFIG_YUV_End                                           7
#define DCREG_OVERLAY_CONFIG_YUV_Start                                         5
#define DCREG_OVERLAY_CONFIG_YUV_Type                                        U03
#define   DCREG_OVERLAY_CONFIG_YUV_709                                       0x1
#define   DCREG_OVERLAY_CONFIG_YUV_2020                                      0x3

#define DCREG_OVERLAY_CONFIG_TILE_MODE                                      12:8
#define DCREG_OVERLAY_CONFIG_TILE_MODE_End                                    12
#define DCREG_OVERLAY_CONFIG_TILE_MODE_Start                                   8
#define DCREG_OVERLAY_CONFIG_TILE_MODE_Type                                  U05
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_LINEAR                             0x00
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILED4X4                           0x01
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_SUPER_TILED_XMAJOR                 0x02
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_SUPER_TILED_YMAJOR                 0x03
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE8X8                       0x04
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE1                         0x05
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE2                         0x06
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE8X4                       0x07
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE4                         0x08
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE5                         0x09
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE6                         0x0A
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_SUPER_TILED_XMAJOR8X4              0x0B
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_SUPER_TILED_YMAJOR4X8              0x0C
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_Y                             0x0D
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILE_MODE7                         0x0E
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILED128X1                         0x0F
#define   DCREG_OVERLAY_CONFIG_TILE_MODE_TILED256X1                         0x10

#define DCREG_OVERLAY_CONFIG_SWIZZLE                                       14:13
#define DCREG_OVERLAY_CONFIG_SWIZZLE_End                                      14
#define DCREG_OVERLAY_CONFIG_SWIZZLE_Start                                    13
#define DCREG_OVERLAY_CONFIG_SWIZZLE_Type                                    U02
#define   DCREG_OVERLAY_CONFIG_SWIZZLE_ARGB                                  0x0
#define   DCREG_OVERLAY_CONFIG_SWIZZLE_RGBA                                  0x1
#define   DCREG_OVERLAY_CONFIG_SWIZZLE_ABGR                                  0x2
#define   DCREG_OVERLAY_CONFIG_SWIZZLE_BGRA                                  0x3

#define DCREG_OVERLAY_CONFIG_UV_SWIZZLE                                    15:15
#define DCREG_OVERLAY_CONFIG_UV_SWIZZLE_End                                   15
#define DCREG_OVERLAY_CONFIG_UV_SWIZZLE_Start                                 15
#define DCREG_OVERLAY_CONFIG_UV_SWIZZLE_Type                                 U01

/* The format of the overlay. */
#define DCREG_OVERLAY_CONFIG_FORMAT                                        21:16
#define DCREG_OVERLAY_CONFIG_FORMAT_End                                       21
#define DCREG_OVERLAY_CONFIG_FORMAT_Start                                     16
#define DCREG_OVERLAY_CONFIG_FORMAT_Type                                     U06
#define   DCREG_OVERLAY_CONFIG_FORMAT_X4R4G4B4                              0x00
#define   DCREG_OVERLAY_CONFIG_FORMAT_A4R4G4B4                              0x01
#define   DCREG_OVERLAY_CONFIG_FORMAT_X1R5G5B5                              0x02
#define   DCREG_OVERLAY_CONFIG_FORMAT_A1R5G5B5                              0x03
#define   DCREG_OVERLAY_CONFIG_FORMAT_R5G6B5                                0x04
#define   DCREG_OVERLAY_CONFIG_FORMAT_X8R8G8B8                              0x05
#define   DCREG_OVERLAY_CONFIG_FORMAT_A8R8G8B8                              0x06
#define   DCREG_OVERLAY_CONFIG_FORMAT_YUY2                                  0x07
#define   DCREG_OVERLAY_CONFIG_FORMAT_UYVY                                  0x08
#define   DCREG_OVERLAY_CONFIG_FORMAT_INDEX8                                0x09
#define   DCREG_OVERLAY_CONFIG_FORMAT_MONOCHROME                            0x0A
#define   DCREG_OVERLAY_CONFIG_FORMAT_YV12                                  0x0F
#define   DCREG_OVERLAY_CONFIG_FORMAT_A8                                    0x10
#define   DCREG_OVERLAY_CONFIG_FORMAT_NV12                                  0x11
#define   DCREG_OVERLAY_CONFIG_FORMAT_NV16                                  0x12
#define   DCREG_OVERLAY_CONFIG_FORMAT_RG16                                  0x13
#define   DCREG_OVERLAY_CONFIG_FORMAT_R8                                    0x14
#define   DCREG_OVERLAY_CONFIG_FORMAT_NV12_10BIT                            0x15
#define   DCREG_OVERLAY_CONFIG_FORMAT_A2R10G10B10                           0x16
#define   DCREG_OVERLAY_CONFIG_FORMAT_NV16_10BIT                            0x17
#define   DCREG_OVERLAY_CONFIG_FORMAT_INDEX1                                0x18
#define   DCREG_OVERLAY_CONFIG_FORMAT_INDEX2                                0x19
#define   DCREG_OVERLAY_CONFIG_FORMAT_INDEX4                                0x1A
#define   DCREG_OVERLAY_CONFIG_FORMAT_P010                                  0x1B
#define   DCREG_OVERLAY_CONFIG_FORMAT_NV12_10BIT_L1                         0x1C
#define   DCREG_OVERLAY_CONFIG_FORMAT_NV16_10BIT_L1                         0x1D
#define   DCREG_OVERLAY_CONFIG_FORMAT_I010                                  0x1E

/* When the overlay FIFO underflows, this bit gets set to one.  Reading this  **
** register will reset it back to zero.                                       */
#define DCREG_OVERLAY_CONFIG_UNDERFLOW                                     23:23
#define DCREG_OVERLAY_CONFIG_UNDERFLOW_End                                    23
#define DCREG_OVERLAY_CONFIG_UNDERFLOW_Start                                  23
#define DCREG_OVERLAY_CONFIG_UNDERFLOW_Type                                  U01
#define   DCREG_OVERLAY_CONFIG_UNDERFLOW_NO                                  0x0
#define   DCREG_OVERLAY_CONFIG_UNDERFLOW_YES                                 0x1

/* Enable this overlay layer.   */
#define DCREG_OVERLAY_CONFIG_ENABLE                                        24:24
#define DCREG_OVERLAY_CONFIG_ENABLE_End                                       24
#define DCREG_OVERLAY_CONFIG_ENABLE_Start                                     24
#define DCREG_OVERLAY_CONFIG_ENABLE_Type                                     U01
#define   DCREG_OVERLAY_CONFIG_ENABLE_DISABLE                                0x0
#define   DCREG_OVERLAY_CONFIG_ENABLE_ENABLE                         (0x1 << 24)

/* When enabled, pixel value of the overlay comes from                        **
** decregOverlayClearValue, otherwise pixel value comes from memory.          */
#define DCREG_OVERLAY_CONFIG_CLEAR                                         25:25
#define DCREG_OVERLAY_CONFIG_CLEAR_End                                        25
#define DCREG_OVERLAY_CONFIG_CLEAR_Start                                      25
#define DCREG_OVERLAY_CONFIG_CLEAR_Type                                      U01
#define   DCREG_OVERLAY_CONFIG_CLEAR_DISABLED                                0x0
#define   DCREG_OVERLAY_CONFIG_CLEAR_ENABLED                                 0x1

/* If enabled, send dtrc small burst for the overlay. */
#define DCREG_OVERLAY_CONFIG_DTRC                                          26:26
#define DCREG_OVERLAY_CONFIG_DTRC_End                                         26
#define DCREG_OVERLAY_CONFIG_DTRC_Start                                       26
#define DCREG_OVERLAY_CONFIG_DTRC_Type                                       U01
#define   DCREG_OVERLAY_CONFIG_DTRC_DISABLED                                 0x0
#define   DCREG_OVERLAY_CONFIG_DTRC_ENABLED                                  0x1

/* Indicate whether it's compressed by dec. 0: Uncompressed 1: Compressed. */
#define DCREG_OVERLAY_CONFIG_COMPRESSED                                    27:27
#define DCREG_OVERLAY_CONFIG_COMPRESSED_End                                   27
#define DCREG_OVERLAY_CONFIG_COMPRESSED_Start                                 27
#define DCREG_OVERLAY_CONFIG_COMPRESSED_Type                                 U01
#define   DCREG_OVERLAY_CONFIG_COMPRESSED_UNCOMPRESSED                       0x0
#define   DCREG_OVERLAY_CONFIG_COMPRESSED_COMPRESSED                         0x1

/* De-Gamma switch.  */
#define DCREG_OVERLAY_CONFIG_DE_GAMMA                                      28:28
#define DCREG_OVERLAY_CONFIG_DE_GAMMA_End                                     28
#define DCREG_OVERLAY_CONFIG_DE_GAMMA_Start                                   28
#define DCREG_OVERLAY_CONFIG_DE_GAMMA_Type                                   U01
#define   DCREG_OVERLAY_CONFIG_DE_GAMMA_DISABLED                             0x0
#define   DCREG_OVERLAY_CONFIG_DE_GAMMA_ENABLED                              0x1

/* RGB2RGB switch.  */
#define DCREG_OVERLAY_CONFIG_RGB_TO_RGB                                    29:29
#define DCREG_OVERLAY_CONFIG_RGB_TO_RGB_End                                   29
#define DCREG_OVERLAY_CONFIG_RGB_TO_RGB_Start                                 29
#define DCREG_OVERLAY_CONFIG_RGB_TO_RGB_Type                                 U01
#define   DCREG_OVERLAY_CONFIG_RGB_TO_RGB_DISABLED                           0x0
#define   DCREG_OVERLAY_CONFIG_RGB_TO_RGB_ENABLED                            0x1

/* Enable or disable YUV clamp for YUV2RGB.  */
#define DCREG_OVERLAY_CONFIG_YUV_CLAMP                                     30:30
#define DCREG_OVERLAY_CONFIG_YUV_CLAMP_End                                    30
#define DCREG_OVERLAY_CONFIG_YUV_CLAMP_Start                                  30
#define DCREG_OVERLAY_CONFIG_YUV_CLAMP_Type                                  U01
#define   DCREG_OVERLAY_CONFIG_YUV_CLAMP_DISABLED                            0x0
#define   DCREG_OVERLAY_CONFIG_YUV_CLAMP_ENABLED                             0x1

/* Register dcregOverlayAlphaBlendConfig (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Overlay alpha blending configure. NOTE: This register is double buffered. */

#define dcregOverlayAlphaBlendConfigRegAddrs                              0x0560
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_Address                         0x01580
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_MSB                                  15
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_LSB                                   4
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_BLK                                   4
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_Count                                16
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_FieldMask                    0x0000FFFB
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_ReadMask                     0x0000FFFB
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_WriteMask                    0x0000FFFB
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_ResetValue                   0x00000000

#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE                      0:0
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE_End                    0
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE_Start                  0
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE_Type                 U01
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE_NORMAL             0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_MODE_INVERSED           0x1

#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DISABLE_ALPHA_BLEND                 1:1
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DISABLE_ALPHA_BLEND_End               1
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DISABLE_ALPHA_BLEND_Start             1
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DISABLE_ALPHA_BLEND_Type            U01
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DISABLE_ALPHA_BLEND_DISABLED      0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DISABLE_ALPHA_BLEND_ENABLED       0x1

#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE               4:3
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_End             4
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_Start           3
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_Type          U02
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_NORMAL      0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_GLOBAL      0x1
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_GLOBAL_ALPHA_MODE_SCALED      0x2

#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE                   7:5
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_End                 7
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_Start               5
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_Type              U03
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_ZERO            0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_ONE             0x1
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_NORMAL          0x2
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_INVERSED        0x3
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_COLOR           0x4
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_COLOR_INVERSED  0x5
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_SATURATED_ALPHA 0x6
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_BLENDING_MODE_SATURATED_DEST_ALPHA 0x7

/* Src Blending factor is calculated from Src alpha. */
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR                    8:8
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_End                  8
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_Start                8
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_Type               U01
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_DISABLED         0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_SRC_ALPHA_FACTOR_ENABLED          0x1

#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE                      9:9
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE_End                    9
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE_Start                  9
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE_Type                 U01
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE_NORMAL             0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_MODE_INVERSED           0x1

#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE             11:10
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_End            11
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_Start          10
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_Type          U02
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_NORMAL      0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_GLOBAL      0x1
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_GLOBAL_ALPHA_MODE_SCALED      0x2

#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE                 14:12
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_End                14
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_Start              12
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_Type              U03
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_ZERO            0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_ONE             0x1
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_NORMAL          0x2
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_INVERSED        0x3
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_COLOR           0x4
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_COLOR_INVERSED  0x5
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_SATURATED_ALPHA 0x6
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_BLENDING_MODE_SATURATED_DEST_ALPHA 0x7

/* Dst Blending factor is calculated from Dst alpha. */
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR                  15:15
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_End                 15
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_Start               15
#define DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_Type               U01
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_DISABLED         0x0
#define   DCREG_OVERLAY_ALPHA_BLEND_CONFIG_DST_ALPHA_FACTOR_ENABLED          0x1

/* Register dcregOverlayAddress (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Starting address of the overlay. NOTE: This register is double buffered. */

#define dcregOverlayAddressRegAddrs                                       0x0570
#define DCREG_OVERLAY_ADDRESS_Address                                    0x015C0
#define DCREG_OVERLAY_ADDRESS_MSB                                             15
#define DCREG_OVERLAY_ADDRESS_LSB                                              4
#define DCREG_OVERLAY_ADDRESS_BLK                                              4
#define DCREG_OVERLAY_ADDRESS_Count                                           16
#define DCREG_OVERLAY_ADDRESS_FieldMask                               0xFFFFFFFF
#define DCREG_OVERLAY_ADDRESS_ReadMask                                0xFFFFFFFF
#define DCREG_OVERLAY_ADDRESS_WriteMask                               0xFFFFFFFF
#define DCREG_OVERLAY_ADDRESS_ResetValue                              0x00000000

#define DCREG_OVERLAY_ADDRESS_ADDRESS                                       31:0
#define DCREG_OVERLAY_ADDRESS_ADDRESS_End                                     31
#define DCREG_OVERLAY_ADDRESS_ADDRESS_Start                                    0
#define DCREG_OVERLAY_ADDRESS_ADDRESS_Type                                   U32

/* Register dcregOverlayStride (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Stride of the overlay in bytes. NOTE: This register is double buffered. */

#define dcregOverlayStrideRegAddrs                                        0x0580
#define DCREG_OVERLAY_STRIDE_Address                                     0x01600
#define DCREG_OVERLAY_STRIDE_MSB                                              15
#define DCREG_OVERLAY_STRIDE_LSB                                               4
#define DCREG_OVERLAY_STRIDE_BLK                                               4
#define DCREG_OVERLAY_STRIDE_Count                                            16
#define DCREG_OVERLAY_STRIDE_FieldMask                                0x0003FFFF
#define DCREG_OVERLAY_STRIDE_ReadMask                                 0x0003FFFF
#define DCREG_OVERLAY_STRIDE_WriteMask                                0x0003FFFF
#define DCREG_OVERLAY_STRIDE_ResetValue                               0x00000000

/* Number of bytes from start of one line to next line. */
#define DCREG_OVERLAY_STRIDE_STRIDE                                         17:0
#define DCREG_OVERLAY_STRIDE_STRIDE_End                                       17
#define DCREG_OVERLAY_STRIDE_STRIDE_Start                                      0
#define DCREG_OVERLAY_STRIDE_STRIDE_Type                                     U18

/* Register dcregOverlayTL (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Top left coordinate of panel pixel where the overlay should start.  Be     **
** aware there is no panning inside the overlay. NOTE: This register is       **
** double buffered.                                                           */

#define dcregOverlayTLRegAddrs                                            0x0590
#define DCREG_OVERLAY_TL_Address                                         0x01640
#define DCREG_OVERLAY_TL_MSB                                                  15
#define DCREG_OVERLAY_TL_LSB                                                   4
#define DCREG_OVERLAY_TL_BLK                                                   4
#define DCREG_OVERLAY_TL_Count                                                16
#define DCREG_OVERLAY_TL_FieldMask                                    0x3FFFFFFF
#define DCREG_OVERLAY_TL_ReadMask                                     0x3FFFFFFF
#define DCREG_OVERLAY_TL_WriteMask                                    0x3FFFFFFF
#define DCREG_OVERLAY_TL_ResetValue                                   0x00000000

/* Left boundary of overlay window. */
#define DCREG_OVERLAY_TL_X                                                  14:0
#define DCREG_OVERLAY_TL_X_End                                                14
#define DCREG_OVERLAY_TL_X_Start                                               0
#define DCREG_OVERLAY_TL_X_Type                                              U15

/* Top boundary of overlay window. */
#define DCREG_OVERLAY_TL_Y                                                 29:15
#define DCREG_OVERLAY_TL_Y_End                                                29
#define DCREG_OVERLAY_TL_Y_Start                                              15
#define DCREG_OVERLAY_TL_Y_Type                                              U15

/* Register dcregOverlayBR (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Bottom right coordinate of the panel pixel where the overlay should end.   **
** The border is inclusive. NOTE: This register is double buffered.           */

#define dcregOverlayBRRegAddrs                                            0x05A0
#define DCREG_OVERLAY_BR_Address                                         0x01680
#define DCREG_OVERLAY_BR_MSB                                                  15
#define DCREG_OVERLAY_BR_LSB                                                   4
#define DCREG_OVERLAY_BR_BLK                                                   4
#define DCREG_OVERLAY_BR_Count                                                16
#define DCREG_OVERLAY_BR_FieldMask                                    0x3FFFFFFF
#define DCREG_OVERLAY_BR_ReadMask                                     0x3FFFFFFF
#define DCREG_OVERLAY_BR_WriteMask                                    0x3FFFFFFF
#define DCREG_OVERLAY_BR_ResetValue                                   0x00000000

/* Right boundary of overlay. */
#define DCREG_OVERLAY_BR_X                                                  14:0
#define DCREG_OVERLAY_BR_X_End                                                14
#define DCREG_OVERLAY_BR_X_Start                                               0
#define DCREG_OVERLAY_BR_X_Type                                              U15

/* Bottom boundary of overlay. */
#define DCREG_OVERLAY_BR_Y                                                 29:15
#define DCREG_OVERLAY_BR_Y_End                                                29
#define DCREG_OVERLAY_BR_Y_Start                                              15
#define DCREG_OVERLAY_BR_Y_Type                                              U15

/* Register dcregOverlaySrcGlobalColor (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Color value used when alpha blending process is configured to use global   **
** color for source.  NOTE: This register is double buffered.                 */

#define dcregOverlaySrcGlobalColorRegAddrs                                0x05B0
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_Address                           0x016C0
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_MSB                                    15
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_LSB                                     4
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_BLK                                     4
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_Count                                  16
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_ResetValue                     0x00000000

#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_BLUE                                  7:0
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_BLUE_End                                7
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_BLUE_Start                              0
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_BLUE_Type                             U08

#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_GREEN                                15:8
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_GREEN_End                              15
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_GREEN_Start                             8
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_GREEN_Type                            U08

#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_RED                                 23:16
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_RED_End                                23
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_RED_Start                              16
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_RED_Type                              U08

#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_ALPHA                               31:24
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_ALPHA_End                              31
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_ALPHA_Start                            24
#define DCREG_OVERLAY_SRC_GLOBAL_COLOR_ALPHA_Type                            U08

/* Register dcregOverlayDstGlobalColor (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Color value used when alpha blending process is configured to use global   **
** color for destination.  NOTE: This register is double buffered.            */

#define dcregOverlayDstGlobalColorRegAddrs                                0x05C0
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_Address                           0x01700
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_MSB                                    15
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_LSB                                     4
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_BLK                                     4
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_Count                                  16
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_ResetValue                     0x00000000

#define DCREG_OVERLAY_DST_GLOBAL_COLOR_BLUE                                  7:0
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_BLUE_End                                7
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_BLUE_Start                              0
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_BLUE_Type                             U08

#define DCREG_OVERLAY_DST_GLOBAL_COLOR_GREEN                                15:8
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_GREEN_End                              15
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_GREEN_Start                             8
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_GREEN_Type                            U08

#define DCREG_OVERLAY_DST_GLOBAL_COLOR_RED                                 23:16
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_RED_End                                23
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_RED_Start                              16
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_RED_Type                              U08

#define DCREG_OVERLAY_DST_GLOBAL_COLOR_ALPHA                               31:24
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_ALPHA_End                              31
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_ALPHA_Start                            24
#define DCREG_OVERLAY_DST_GLOBAL_COLOR_ALPHA_Type                            U08

/* Register dcregOverlayColorKey (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Start of color key range for overlay. NOTE: This register is double        **
** buffered.                                                                  */

#define dcregOverlayColorKeyRegAddrs                                      0x05D0
#define DCREG_OVERLAY_COLOR_KEY_Address                                  0x01740
#define DCREG_OVERLAY_COLOR_KEY_MSB                                           15
#define DCREG_OVERLAY_COLOR_KEY_LSB                                            4
#define DCREG_OVERLAY_COLOR_KEY_BLK                                            4
#define DCREG_OVERLAY_COLOR_KEY_Count                                         16
#define DCREG_OVERLAY_COLOR_KEY_FieldMask                             0xFFFFFFFF
#define DCREG_OVERLAY_COLOR_KEY_ReadMask                              0xFFFFFFFF
#define DCREG_OVERLAY_COLOR_KEY_WriteMask                             0xFFFFFFFF
#define DCREG_OVERLAY_COLOR_KEY_ResetValue                            0x00000000

#define DCREG_OVERLAY_COLOR_KEY_BLUE                                         7:0
#define DCREG_OVERLAY_COLOR_KEY_BLUE_End                                       7
#define DCREG_OVERLAY_COLOR_KEY_BLUE_Start                                     0
#define DCREG_OVERLAY_COLOR_KEY_BLUE_Type                                    U08

#define DCREG_OVERLAY_COLOR_KEY_GREEN                                       15:8
#define DCREG_OVERLAY_COLOR_KEY_GREEN_End                                     15
#define DCREG_OVERLAY_COLOR_KEY_GREEN_Start                                    8
#define DCREG_OVERLAY_COLOR_KEY_GREEN_Type                                   U08

#define DCREG_OVERLAY_COLOR_KEY_RED                                        23:16
#define DCREG_OVERLAY_COLOR_KEY_RED_End                                       23
#define DCREG_OVERLAY_COLOR_KEY_RED_Start                                     16
#define DCREG_OVERLAY_COLOR_KEY_RED_Type                                     U08

#define DCREG_OVERLAY_COLOR_KEY_ALPHA                                      31:24
#define DCREG_OVERLAY_COLOR_KEY_ALPHA_End                                     31
#define DCREG_OVERLAY_COLOR_KEY_ALPHA_Start                                   24
#define DCREG_OVERLAY_COLOR_KEY_ALPHA_Type                                   U08

/* Register dcregOverlayColorKeyHigh (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* End of color key range for overlay. NOTE: This register is double          **
** buffered.                                                                  */

#define dcregOverlayColorKeyHighRegAddrs                                  0x05E0
#define DCREG_OVERLAY_COLOR_KEY_HIGH_Address                             0x01780
#define DCREG_OVERLAY_COLOR_KEY_HIGH_MSB                                      15
#define DCREG_OVERLAY_COLOR_KEY_HIGH_LSB                                       4
#define DCREG_OVERLAY_COLOR_KEY_HIGH_BLK                                       4
#define DCREG_OVERLAY_COLOR_KEY_HIGH_Count                                    16
#define DCREG_OVERLAY_COLOR_KEY_HIGH_FieldMask                        0xFFFFFFFF
#define DCREG_OVERLAY_COLOR_KEY_HIGH_ReadMask                         0xFFFFFFFF
#define DCREG_OVERLAY_COLOR_KEY_HIGH_WriteMask                        0xFFFFFFFF
#define DCREG_OVERLAY_COLOR_KEY_HIGH_ResetValue                       0x00000000

#define DCREG_OVERLAY_COLOR_KEY_HIGH_BLUE                                    7:0
#define DCREG_OVERLAY_COLOR_KEY_HIGH_BLUE_End                                  7
#define DCREG_OVERLAY_COLOR_KEY_HIGH_BLUE_Start                                0
#define DCREG_OVERLAY_COLOR_KEY_HIGH_BLUE_Type                               U08

#define DCREG_OVERLAY_COLOR_KEY_HIGH_GREEN                                  15:8
#define DCREG_OVERLAY_COLOR_KEY_HIGH_GREEN_End                                15
#define DCREG_OVERLAY_COLOR_KEY_HIGH_GREEN_Start                               8
#define DCREG_OVERLAY_COLOR_KEY_HIGH_GREEN_Type                              U08

#define DCREG_OVERLAY_COLOR_KEY_HIGH_RED                                   23:16
#define DCREG_OVERLAY_COLOR_KEY_HIGH_RED_End                                  23
#define DCREG_OVERLAY_COLOR_KEY_HIGH_RED_Start                                16
#define DCREG_OVERLAY_COLOR_KEY_HIGH_RED_Type                                U08

#define DCREG_OVERLAY_COLOR_KEY_HIGH_ALPHA                                 31:24
#define DCREG_OVERLAY_COLOR_KEY_HIGH_ALPHA_End                                31
#define DCREG_OVERLAY_COLOR_KEY_HIGH_ALPHA_Start                              24
#define DCREG_OVERLAY_COLOR_KEY_HIGH_ALPHA_Type                              U08

/* Register dcregOverlaySize (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Window size of overlay buffer in memory in pixels. If the overlay is       **
** rotated, this size may be different from size of overlay displayed.  NOTE: **
** This register is double buffered.                                          */

#define dcregOverlaySizeRegAddrs                                          0x05F0
#define DCREG_OVERLAY_SIZE_Address                                       0x017C0
#define DCREG_OVERLAY_SIZE_MSB                                                15
#define DCREG_OVERLAY_SIZE_LSB                                                 4
#define DCREG_OVERLAY_SIZE_BLK                                                 4
#define DCREG_OVERLAY_SIZE_Count                                              16
#define DCREG_OVERLAY_SIZE_FieldMask                                  0x3FFFFFFF
#define DCREG_OVERLAY_SIZE_ReadMask                                   0x3FFFFFFF
#define DCREG_OVERLAY_SIZE_WriteMask                                  0x3FFFFFFF
#define DCREG_OVERLAY_SIZE_ResetValue                                 0x00000000

#define DCREG_OVERLAY_SIZE_WIDTH                                            14:0
#define DCREG_OVERLAY_SIZE_WIDTH_End                                          14
#define DCREG_OVERLAY_SIZE_WIDTH_Start                                         0
#define DCREG_OVERLAY_SIZE_WIDTH_Type                                        U15

#define DCREG_OVERLAY_SIZE_HEIGHT                                          29:15
#define DCREG_OVERLAY_SIZE_HEIGHT_End                                         29
#define DCREG_OVERLAY_SIZE_HEIGHT_Start                                       15
#define DCREG_OVERLAY_SIZE_HEIGHT_Type                                       U15

/* Register dcregFrameBufferUStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Stride of the second planar of frame buffer if exist. NOTE: This register  **
** is double buffered.                                                        */

#define dcregFrameBufferUStrideRegAddrs                                   0x0600
#define DCREG_FRAME_BUFFER_USTRIDE_Address                               0x01800
#define DCREG_FRAME_BUFFER_USTRIDE_MSB                                        15
#define DCREG_FRAME_BUFFER_USTRIDE_LSB                                         1
#define DCREG_FRAME_BUFFER_USTRIDE_BLK                                         1
#define DCREG_FRAME_BUFFER_USTRIDE_Count                                       2
#define DCREG_FRAME_BUFFER_USTRIDE_FieldMask                          0x0003FFFF
#define DCREG_FRAME_BUFFER_USTRIDE_ReadMask                           0x0003FFFF
#define DCREG_FRAME_BUFFER_USTRIDE_WriteMask                          0x0003FFFF
#define DCREG_FRAME_BUFFER_USTRIDE_ResetValue                         0x00000000

/* Number of bytes from start of one line to next line. */
#define DCREG_FRAME_BUFFER_USTRIDE_STRIDE                                   17:0
#define DCREG_FRAME_BUFFER_USTRIDE_STRIDE_End                                 17
#define DCREG_FRAME_BUFFER_USTRIDE_STRIDE_Start                                0
#define DCREG_FRAME_BUFFER_USTRIDE_STRIDE_Type                               U18

/* Register dcregFrameBufferVStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Stride of the third planar of frame buffer if exist. NOTE: This register   **
** is double buffered.                                                        */

#define dcregFrameBufferVStrideRegAddrs                                   0x0602
#define DCREG_FRAME_BUFFER_VSTRIDE_Address                               0x01808
#define DCREG_FRAME_BUFFER_VSTRIDE_MSB                                        15
#define DCREG_FRAME_BUFFER_VSTRIDE_LSB                                         1
#define DCREG_FRAME_BUFFER_VSTRIDE_BLK                                         1
#define DCREG_FRAME_BUFFER_VSTRIDE_Count                                       2
#define DCREG_FRAME_BUFFER_VSTRIDE_FieldMask                          0x0003FFFF
#define DCREG_FRAME_BUFFER_VSTRIDE_ReadMask                           0x0003FFFF
#define DCREG_FRAME_BUFFER_VSTRIDE_WriteMask                          0x0003FFFF
#define DCREG_FRAME_BUFFER_VSTRIDE_ResetValue                         0x00000000

/* Number of bytes from start of one line to next line. */
#define DCREG_FRAME_BUFFER_VSTRIDE_STRIDE                                   17:0
#define DCREG_FRAME_BUFFER_VSTRIDE_STRIDE_End                                 17
#define DCREG_FRAME_BUFFER_VSTRIDE_STRIDE_Start                                0
#define DCREG_FRAME_BUFFER_VSTRIDE_STRIDE_Type                               U18

/* Register dcregFrameBufferSize (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Window size of frame buffer in memory in pixels. If frame buffer is        **
** rotated or scaled, this size may be different from size of display window. **
**  NOTE: This register is double buffered.                                   */

#define dcregFrameBufferSizeRegAddrs                                      0x0604
#define DCREG_FRAME_BUFFER_SIZE_Address                                  0x01810
#define DCREG_FRAME_BUFFER_SIZE_MSB                                           15
#define DCREG_FRAME_BUFFER_SIZE_LSB                                            1
#define DCREG_FRAME_BUFFER_SIZE_BLK                                            1
#define DCREG_FRAME_BUFFER_SIZE_Count                                          2
#define DCREG_FRAME_BUFFER_SIZE_FieldMask                             0x3FFFFFFF
#define DCREG_FRAME_BUFFER_SIZE_ReadMask                              0x3FFFFFFF
#define DCREG_FRAME_BUFFER_SIZE_WriteMask                             0x3FFFFFFF
#define DCREG_FRAME_BUFFER_SIZE_ResetValue                            0x00000000

#define DCREG_FRAME_BUFFER_SIZE_WIDTH                                       14:0
#define DCREG_FRAME_BUFFER_SIZE_WIDTH_End                                     14
#define DCREG_FRAME_BUFFER_SIZE_WIDTH_Start                                    0
#define DCREG_FRAME_BUFFER_SIZE_WIDTH_Type                                   U15

#define DCREG_FRAME_BUFFER_SIZE_HEIGHT                                     29:15
#define DCREG_FRAME_BUFFER_SIZE_HEIGHT_End                                    29
#define DCREG_FRAME_BUFFER_SIZE_HEIGHT_Start                                  15
#define DCREG_FRAME_BUFFER_SIZE_HEIGHT_Type                                  U15

/* Register dcregIndexColorTableIndex (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Index into index color table.  See dcregIndexColorTableData                **
** for more information.$                                                     */

#define dcregIndexColorTableIndexRegAddrs                                 0x0606
#define DCREG_INDEX_COLOR_TABLE_INDEX_Address                            0x01818
#define DCREG_INDEX_COLOR_TABLE_INDEX_MSB                                     15
#define DCREG_INDEX_COLOR_TABLE_INDEX_LSB                                      1
#define DCREG_INDEX_COLOR_TABLE_INDEX_BLK                                      1
#define DCREG_INDEX_COLOR_TABLE_INDEX_Count                                    2
#define DCREG_INDEX_COLOR_TABLE_INDEX_FieldMask                       0x000000FF
#define DCREG_INDEX_COLOR_TABLE_INDEX_ReadMask                        0x00000000
#define DCREG_INDEX_COLOR_TABLE_INDEX_WriteMask                       0x000000FF
#define DCREG_INDEX_COLOR_TABLE_INDEX_ResetValue                      0x00000000

/* Index into index color table. */
#define DCREG_INDEX_COLOR_TABLE_INDEX_INDEX                                  7:0
#define DCREG_INDEX_COLOR_TABLE_INDEX_INDEX_End                                7
#define DCREG_INDEX_COLOR_TABLE_INDEX_INDEX_Start                              0
#define DCREG_INDEX_COLOR_TABLE_INDEX_INDEX_Type                             U08

/* Register dcregIndexColorTableData (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Translation values for the index color table.  When this register          **
** gets written, the data gets stored in the index color table at the         **
** index specified by the dcregIndexColorTableIndex register.  After the      **
** register is written, the index gets incremented.                           **
** NOTE: This register is double buffered.                                    */

#define dcregIndexColorTableDataRegAddrs                                  0x0608
#define DCREG_INDEX_COLOR_TABLE_DATA_Address                             0x01820
#define DCREG_INDEX_COLOR_TABLE_DATA_MSB                                      15
#define DCREG_INDEX_COLOR_TABLE_DATA_LSB                                       1
#define DCREG_INDEX_COLOR_TABLE_DATA_BLK                                       1
#define DCREG_INDEX_COLOR_TABLE_DATA_Count                                     2
#define DCREG_INDEX_COLOR_TABLE_DATA_FieldMask                        0xFFFFFFFF
#define DCREG_INDEX_COLOR_TABLE_DATA_ReadMask                         0x00000000
#define DCREG_INDEX_COLOR_TABLE_DATA_WriteMask                        0xFFFFFFFF
#define DCREG_INDEX_COLOR_TABLE_DATA_ResetValue                       0x00000000

#define DCREG_INDEX_COLOR_TABLE_DATA_BLUE                                    7:0
#define DCREG_INDEX_COLOR_TABLE_DATA_BLUE_End                                  7
#define DCREG_INDEX_COLOR_TABLE_DATA_BLUE_Start                                0
#define DCREG_INDEX_COLOR_TABLE_DATA_BLUE_Type                               U08

#define DCREG_INDEX_COLOR_TABLE_DATA_GREEN                                  15:8
#define DCREG_INDEX_COLOR_TABLE_DATA_GREEN_End                                15
#define DCREG_INDEX_COLOR_TABLE_DATA_GREEN_Start                               8
#define DCREG_INDEX_COLOR_TABLE_DATA_GREEN_Type                              U08

#define DCREG_INDEX_COLOR_TABLE_DATA_RED                                   23:16
#define DCREG_INDEX_COLOR_TABLE_DATA_RED_End                                  23
#define DCREG_INDEX_COLOR_TABLE_DATA_RED_Start                                16
#define DCREG_INDEX_COLOR_TABLE_DATA_RED_Type                                U08

#define DCREG_INDEX_COLOR_TABLE_DATA_ALPHA                                 31:24
#define DCREG_INDEX_COLOR_TABLE_DATA_ALPHA_End                                31
#define DCREG_INDEX_COLOR_TABLE_DATA_ALPHA_Start                              24
#define DCREG_INDEX_COLOR_TABLE_DATA_ALPHA_Type                              U08

/* Register dcregFrameBufferScaleFactorX (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Horizontal scale factor used to scale frame buffer NOTE: This register is  **
** double buffered.                                                           */

#define dcregFrameBufferScaleFactorXRegAddrs                              0x060A
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_Address                        0x01828
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_MSB                                 15
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_LSB                                  1
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_BLK                                  1
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_Count                                2
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_FieldMask                   0x7FFFFFFF
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_ReadMask                    0x7FFFFFFF
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_WriteMask                   0x7FFFFFFF
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_ResetValue                  0x00000000

#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_X                                 30:0
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_X_End                               30
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_X_Start                              0
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_X_X_Type                             U31

/* Register dcregFrameBufferScaleFactorY (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Vertical scale factor used to scale frame buffer NOTE: This register is    **
** double buffered.                                                           */

#define dcregFrameBufferScaleFactorYRegAddrs                              0x060C
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_Address                        0x01830
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_MSB                                 15
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_LSB                                  1
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_BLK                                  1
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_Count                                2
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_FieldMask                   0x7FFFFFFF
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_ReadMask                    0x7FFFFFFF
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_WriteMask                   0x7FFFFFFF
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_ResetValue                  0x00000000

#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_Y                                 30:0
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_Y_End                               30
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_Y_Start                              0
#define DCREG_FRAME_BUFFER_SCALE_FACTOR_Y_Y_Type                             U31

/* Register dcregHoriFilterKernelIndex (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Index into horizontal filter kernel table.  See dcregHoriFilterKernel. for **
** more information.                                                          */

#define dcregHoriFilterKernelIndexRegAddrs                                0x060E
#define DCREG_HORI_FILTER_KERNEL_INDEX_Address                           0x01838
#define DCREG_HORI_FILTER_KERNEL_INDEX_MSB                                    15
#define DCREG_HORI_FILTER_KERNEL_INDEX_LSB                                     1
#define DCREG_HORI_FILTER_KERNEL_INDEX_BLK                                     1
#define DCREG_HORI_FILTER_KERNEL_INDEX_Count                                   2
#define DCREG_HORI_FILTER_KERNEL_INDEX_FieldMask                      0x000000FF
#define DCREG_HORI_FILTER_KERNEL_INDEX_ReadMask                       0x00000000
#define DCREG_HORI_FILTER_KERNEL_INDEX_WriteMask                      0x000000FF
#define DCREG_HORI_FILTER_KERNEL_INDEX_ResetValue                     0x00000000

#define DCREG_HORI_FILTER_KERNEL_INDEX_INDEX                                 7:0
#define DCREG_HORI_FILTER_KERNEL_INDEX_INDEX_End                               7
#define DCREG_HORI_FILTER_KERNEL_INDEX_INDEX_Start                             0
#define DCREG_HORI_FILTER_KERNEL_INDEX_INDEX_Type                            U08

/* Register dcregOverlayUPlanarAddress (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Address of the second planar of the overlay if exist. NOTE: This register  **
** is double buffered.                                                        */

#define dcregOverlayUPlanarAddressRegAddrs                                0x0610
#define DCREG_OVERLAY_UPLANAR_ADDRESS_Address                            0x01840
#define DCREG_OVERLAY_UPLANAR_ADDRESS_MSB                                     15
#define DCREG_OVERLAY_UPLANAR_ADDRESS_LSB                                      4
#define DCREG_OVERLAY_UPLANAR_ADDRESS_BLK                                      4
#define DCREG_OVERLAY_UPLANAR_ADDRESS_Count                                   16
#define DCREG_OVERLAY_UPLANAR_ADDRESS_FieldMask                       0xFFFFFFFF
#define DCREG_OVERLAY_UPLANAR_ADDRESS_ReadMask                        0xFFFFFFFF
#define DCREG_OVERLAY_UPLANAR_ADDRESS_WriteMask                       0xFFFFFFFF
#define DCREG_OVERLAY_UPLANAR_ADDRESS_ResetValue                      0x00000000

#define DCREG_OVERLAY_UPLANAR_ADDRESS_ADDRESS                               31:0
#define DCREG_OVERLAY_UPLANAR_ADDRESS_ADDRESS_End                             31
#define DCREG_OVERLAY_UPLANAR_ADDRESS_ADDRESS_Start                            0
#define DCREG_OVERLAY_UPLANAR_ADDRESS_ADDRESS_Type                           U32

/* Register dcregOverlayVPlanarAddress (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Address of the third planar of the overlay if exist. NOTE: This register   **
** is double buffered.                                                        */

#define dcregOverlayVPlanarAddressRegAddrs                                0x0620
#define DCREG_OVERLAY_VPLANAR_ADDRESS_Address                            0x01880
#define DCREG_OVERLAY_VPLANAR_ADDRESS_MSB                                     15
#define DCREG_OVERLAY_VPLANAR_ADDRESS_LSB                                      4
#define DCREG_OVERLAY_VPLANAR_ADDRESS_BLK                                      4
#define DCREG_OVERLAY_VPLANAR_ADDRESS_Count                                   16
#define DCREG_OVERLAY_VPLANAR_ADDRESS_FieldMask                       0xFFFFFFFF
#define DCREG_OVERLAY_VPLANAR_ADDRESS_ReadMask                        0xFFFFFFFF
#define DCREG_OVERLAY_VPLANAR_ADDRESS_WriteMask                       0xFFFFFFFF
#define DCREG_OVERLAY_VPLANAR_ADDRESS_ResetValue                      0x00000000

#define DCREG_OVERLAY_VPLANAR_ADDRESS_ADDRESS                               31:0
#define DCREG_OVERLAY_VPLANAR_ADDRESS_ADDRESS_End                             31
#define DCREG_OVERLAY_VPLANAR_ADDRESS_ADDRESS_Start                            0
#define DCREG_OVERLAY_VPLANAR_ADDRESS_ADDRESS_Type                           U32

/* Register dcregOverlayUStride (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Stride of the second planar of the overlay if exist. NOTE: This register   **
** is double buffered.                                                        */

#define dcregOverlayUStrideRegAddrs                                       0x0630
#define DCREG_OVERLAY_USTRIDE_Address                                    0x018C0
#define DCREG_OVERLAY_USTRIDE_MSB                                             15
#define DCREG_OVERLAY_USTRIDE_LSB                                              4
#define DCREG_OVERLAY_USTRIDE_BLK                                              4
#define DCREG_OVERLAY_USTRIDE_Count                                           16
#define DCREG_OVERLAY_USTRIDE_FieldMask                               0x0003FFFF
#define DCREG_OVERLAY_USTRIDE_ReadMask                                0x0003FFFF
#define DCREG_OVERLAY_USTRIDE_WriteMask                               0x0003FFFF
#define DCREG_OVERLAY_USTRIDE_ResetValue                              0x00000000

/* Number of bytes from start of one line to next line. */
#define DCREG_OVERLAY_USTRIDE_STRIDE                                        17:0
#define DCREG_OVERLAY_USTRIDE_STRIDE_End                                      17
#define DCREG_OVERLAY_USTRIDE_STRIDE_Start                                     0
#define DCREG_OVERLAY_USTRIDE_STRIDE_Type                                    U18

/* Register dcregOverlayVStride (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Stride of the third planar of the overlay if exist. NOTE: This register is **
** double buffered.                                                           */

#define dcregOverlayVStrideRegAddrs                                       0x0640
#define DCREG_OVERLAY_VSTRIDE_Address                                    0x01900
#define DCREG_OVERLAY_VSTRIDE_MSB                                             15
#define DCREG_OVERLAY_VSTRIDE_LSB                                              4
#define DCREG_OVERLAY_VSTRIDE_BLK                                              4
#define DCREG_OVERLAY_VSTRIDE_Count                                           16
#define DCREG_OVERLAY_VSTRIDE_FieldMask                               0x0003FFFF
#define DCREG_OVERLAY_VSTRIDE_ReadMask                                0x0003FFFF
#define DCREG_OVERLAY_VSTRIDE_WriteMask                               0x0003FFFF
#define DCREG_OVERLAY_VSTRIDE_ResetValue                              0x00000000

/* Number of bytes from start of one line to next line. */
#define DCREG_OVERLAY_VSTRIDE_STRIDE                                        17:0
#define DCREG_OVERLAY_VSTRIDE_STRIDE_End                                      17
#define DCREG_OVERLAY_VSTRIDE_STRIDE_Start                                     0
#define DCREG_OVERLAY_VSTRIDE_STRIDE_Type                                    U18

/* Register dcregOverlayClearValue (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Clear value used when dcregOverlayConfig.Clear is enabled, format is       **
** A8R8G8B8.  NOTE: This register is double buffered.                         */

#define dcregOverlayClearValueRegAddrs                                    0x0650
#define DCREG_OVERLAY_CLEAR_VALUE_Address                                0x01940
#define DCREG_OVERLAY_CLEAR_VALUE_MSB                                         15
#define DCREG_OVERLAY_CLEAR_VALUE_LSB                                          4
#define DCREG_OVERLAY_CLEAR_VALUE_BLK                                          4
#define DCREG_OVERLAY_CLEAR_VALUE_Count                                       16
#define DCREG_OVERLAY_CLEAR_VALUE_FieldMask                           0xFFFFFFFF
#define DCREG_OVERLAY_CLEAR_VALUE_ReadMask                            0xFFFFFFFF
#define DCREG_OVERLAY_CLEAR_VALUE_WriteMask                           0xFFFFFFFF
#define DCREG_OVERLAY_CLEAR_VALUE_ResetValue                          0x00000000

#define DCREG_OVERLAY_CLEAR_VALUE_BLUE                                       7:0
#define DCREG_OVERLAY_CLEAR_VALUE_BLUE_End                                     7
#define DCREG_OVERLAY_CLEAR_VALUE_BLUE_Start                                   0
#define DCREG_OVERLAY_CLEAR_VALUE_BLUE_Type                                  U08

#define DCREG_OVERLAY_CLEAR_VALUE_GREEN                                     15:8
#define DCREG_OVERLAY_CLEAR_VALUE_GREEN_End                                   15
#define DCREG_OVERLAY_CLEAR_VALUE_GREEN_Start                                  8
#define DCREG_OVERLAY_CLEAR_VALUE_GREEN_Type                                 U08

#define DCREG_OVERLAY_CLEAR_VALUE_RED                                      23:16
#define DCREG_OVERLAY_CLEAR_VALUE_RED_End                                     23
#define DCREG_OVERLAY_CLEAR_VALUE_RED_Start                                   16
#define DCREG_OVERLAY_CLEAR_VALUE_RED_Type                                   U08

#define DCREG_OVERLAY_CLEAR_VALUE_ALPHA                                    31:24
#define DCREG_OVERLAY_CLEAR_VALUE_ALPHA_End                                   31
#define DCREG_OVERLAY_CLEAR_VALUE_ALPHA_Start                                 24
#define DCREG_OVERLAY_CLEAR_VALUE_ALPHA_Type                                 U08

/* Register dcregOverlayIndexColorTableIndex (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Index into overlay index color table.  See dcregOverlayIndexColorTableData **
** for more information.$                                                     */

#define dcregOverlayIndexColorTableIndexRegAddrs                          0x0660
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_Address                    0x01980
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_MSB                             15
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_LSB                              4
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_BLK                              4
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_Count                           16
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_FieldMask               0x000000FF
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_ReadMask                0x00000000
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_WriteMask               0x000000FF
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_ResetValue              0x00000000

/* Index into index color table. */
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_INDEX                          7:0
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_INDEX_End                        7
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_INDEX_Start                      0
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_INDEX_INDEX_Type                     U08

/* Register dcregOverlayIndexColorTableData (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Translation values for the index color table of the overlay.  When this    **
** register                                                                   **
** gets written, the data gets stored in the index color table at the         **
** index specified by the dcregOverlayIndexColorTableIndex register.  After   **
** the                                                                        **
** register is written, the index gets incremented.                           **
** NOTE: This register is double buffered.                                    */

#define dcregOverlayIndexColorTableDataRegAddrs                           0x0670
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_Address                     0x019C0
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_MSB                              15
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_LSB                               4
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_BLK                               4
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_Count                            16
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_FieldMask                0xFFFFFFFF
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_ReadMask                 0x00000000
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_WriteMask                0xFFFFFFFF
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_ResetValue               0x00000000

#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_BLUE                            7:0
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_BLUE_End                          7
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_BLUE_Start                        0
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_BLUE_Type                       U08

#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_GREEN                          15:8
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_GREEN_End                        15
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_GREEN_Start                       8
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_GREEN_Type                      U08

#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_RED                           23:16
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_RED_End                          23
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_RED_Start                        16
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_RED_Type                        U08

#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_ALPHA                         31:24
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_ALPHA_End                        31
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_ALPHA_Start                      24
#define DCREG_OVERLAY_INDEX_COLOR_TABLE_DATA_ALPHA_Type                      U08

/* Register dcregHoriFilterKernel (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* One Pass filter Filter blit hori coefficient table. NOTE: This register is **
** double buffered.                                                           */

#define dcregHoriFilterKernelRegAddrs                                     0x0680
#define DCREG_HORI_FILTER_KERNEL_Address                                 0x01A00
#define DCREG_HORI_FILTER_KERNEL_MSB                                          15
#define DCREG_HORI_FILTER_KERNEL_LSB                                           1
#define DCREG_HORI_FILTER_KERNEL_BLK                                           1
#define DCREG_HORI_FILTER_KERNEL_Count                                         2
#define DCREG_HORI_FILTER_KERNEL_FieldMask                            0xFFFFFFFF
#define DCREG_HORI_FILTER_KERNEL_ReadMask                             0x00000000
#define DCREG_HORI_FILTER_KERNEL_WriteMask                            0xFFFFFFFF
#define DCREG_HORI_FILTER_KERNEL_ResetValue                           0x00000000

#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT0                               15:0
#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT0_End                             15
#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT0_Start                            0
#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT0_Type                           U16

#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT1                              31:16
#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT1_End                             31
#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT1_Start                           16
#define DCREG_HORI_FILTER_KERNEL_COEFFICIENT1_Type                           U16

/* Register dcregVertiFilterKernelIndex (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Index into vertical filter kernel table.  See dcregVertiFilterKernel. for  **
** more information.                                                          */

#define dcregVertiFilterKernelIndexRegAddrs                               0x0682
#define DCREG_VERTI_FILTER_KERNEL_INDEX_Address                          0x01A08
#define DCREG_VERTI_FILTER_KERNEL_INDEX_MSB                                   15
#define DCREG_VERTI_FILTER_KERNEL_INDEX_LSB                                    1
#define DCREG_VERTI_FILTER_KERNEL_INDEX_BLK                                    1
#define DCREG_VERTI_FILTER_KERNEL_INDEX_Count                                  2
#define DCREG_VERTI_FILTER_KERNEL_INDEX_FieldMask                     0x000000FF
#define DCREG_VERTI_FILTER_KERNEL_INDEX_ReadMask                      0x00000000
#define DCREG_VERTI_FILTER_KERNEL_INDEX_WriteMask                     0x000000FF
#define DCREG_VERTI_FILTER_KERNEL_INDEX_ResetValue                    0x00000000

#define DCREG_VERTI_FILTER_KERNEL_INDEX_INDEX                                7:0
#define DCREG_VERTI_FILTER_KERNEL_INDEX_INDEX_End                              7
#define DCREG_VERTI_FILTER_KERNEL_INDEX_INDEX_Start                            0
#define DCREG_VERTI_FILTER_KERNEL_INDEX_INDEX_Type                           U08

/* Register dcregVertiFilterKernel (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* One Pass Filter blit vertical coefficient table. NOTE: This register is    **
** double buffered.                                                           */

#define dcregVertiFilterKernelRegAddrs                                    0x0684
#define DCREG_VERTI_FILTER_KERNEL_Address                                0x01A10
#define DCREG_VERTI_FILTER_KERNEL_MSB                                         15
#define DCREG_VERTI_FILTER_KERNEL_LSB                                          1
#define DCREG_VERTI_FILTER_KERNEL_BLK                                          1
#define DCREG_VERTI_FILTER_KERNEL_Count                                        2
#define DCREG_VERTI_FILTER_KERNEL_FieldMask                           0xFFFFFFFF
#define DCREG_VERTI_FILTER_KERNEL_ReadMask                            0x00000000
#define DCREG_VERTI_FILTER_KERNEL_WriteMask                           0xFFFFFFFF
#define DCREG_VERTI_FILTER_KERNEL_ResetValue                          0x00000000

#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT0                              15:0
#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT0_End                            15
#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT0_Start                           0
#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT0_Type                          U16

#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT1                             31:16
#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT1_End                            31
#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT1_Start                          16
#define DCREG_VERTI_FILTER_KERNEL_COEFFICIENT1_Type                          U16

/* Register dcregFrameBufferClearValue (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Clear value used when dcregFrameBufferConfig.Clear is enabled, format is   **
** A8R8G8B8. NOTE: This register is double buffered.                          */

#define dcregFrameBufferClearValueRegAddrs                                0x0686
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_Address                           0x01A18
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_MSB                                    15
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_LSB                                     1
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_BLK                                     1
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_Count                                   2
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_FieldMask                      0xFFFFFFFF
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_ReadMask                       0xFFFFFFFF
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_WriteMask                      0xFFFFFFFF
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_ResetValue                     0x00000000

#define DCREG_FRAME_BUFFER_CLEAR_VALUE_BLUE                                  7:0
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_BLUE_End                                7
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_BLUE_Start                              0
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_BLUE_Type                             U08

#define DCREG_FRAME_BUFFER_CLEAR_VALUE_GREEN                                15:8
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_GREEN_End                              15
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_GREEN_Start                             8
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_GREEN_Type                            U08

#define DCREG_FRAME_BUFFER_CLEAR_VALUE_RED                                 23:16
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_RED_End                                23
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_RED_Start                              16
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_RED_Type                              U08

#define DCREG_FRAME_BUFFER_CLEAR_VALUE_ALPHA                               31:24
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_ALPHA_End                              31
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_ALPHA_Start                            24
#define DCREG_FRAME_BUFFER_CLEAR_VALUE_ALPHA_Type                            U08

/* Register dcregFrameBufferInitialOffset (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Initial offset used when fetching frame buffer source during scaling NOTE: **
** This register is double buffered.                                          */

#define dcregFrameBufferInitialOffsetRegAddrs                             0x0688
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_Address                        0x01A20
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_MSB                                 15
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_LSB                                  1
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_BLK                                  1
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_Count                                2
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_FieldMask                   0xFFFFFFFF
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_ReadMask                    0xFFFFFFFF
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_WriteMask                   0xFFFFFFFF
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_ResetValue                  0x00000000

#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_X                                 15:0
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_X_End                               15
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_X_Start                              0
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_X_Type                             U16

#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_Y                                31:16
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_Y_End                               31
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_Y_Start                             16
#define DCREG_FRAME_BUFFER_INITIAL_OFFSET_Y_Type                             U16

/* Register dcregModuleClockGatingControl (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Module level clock gating control for video and overlay */

#define dcregModuleClockGatingControlRegAddrs                             0x068A
#define DCREG_MODULE_CLOCK_GATING_CONTROL_Address                        0x01A28
#define DCREG_MODULE_CLOCK_GATING_CONTROL_MSB                                 15
#define DCREG_MODULE_CLOCK_GATING_CONTROL_LSB                                  1
#define DCREG_MODULE_CLOCK_GATING_CONTROL_BLK                                  1
#define DCREG_MODULE_CLOCK_GATING_CONTROL_Count                                2
#define DCREG_MODULE_CLOCK_GATING_CONTROL_FieldMask                   0x00000FFF
#define DCREG_MODULE_CLOCK_GATING_CONTROL_ReadMask                    0x00000FFF
#define DCREG_MODULE_CLOCK_GATING_CONTROL_WriteMask                   0x00000FFF
#define DCREG_MODULE_CLOCK_GATING_CONTROL_ResetValue                  0x00000FFF

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_VIDEO  0:0
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_VIDEO_End 0
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_VIDEO_Start 0
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_VIDEO_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0 1:1
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0_End 1
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0_Start 1
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY1 2:2
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY1_End 2
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY1_Start 2
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY1_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY2 3:3
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY2_End 3
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY2_Start 3
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY2_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY3 4:4
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY3_End 4
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY3_Start 4
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY3_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY4 5:5
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY4_End 5
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY4_Start 5
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY4_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY5 6:6
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY5_End 6
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY5_Start 6
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY5_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY6 7:7
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY6_End 7
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY6_Start 7
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY6_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY7 8:8
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY7_End 8
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY7_Start 8
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY7_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_WB_FIFO 9:9
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_WB_FIFO_End 9
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_WB_FIFO_Start 9
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_WB_FIFO_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_SCALER 10:10
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_SCALER_End 10
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_SCALER_Start 10
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_SCALER_Type U01

#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0_SCALER 11:11
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0_SCALER_End 11
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0_SCALER_Start 11
#define DCREG_MODULE_CLOCK_GATING_CONTROL_DISABLE_MODULE_CLOCK_GATING_OVERLAY0_SCALER_Type U01

/* Register dcregLatencyCounter (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregLatencyCounterRegAddrs                                       0x068C
#define DCREG_LATENCY_COUNTER_Address                                    0x01A30
#define DCREG_LATENCY_COUNTER_MSB                                             15
#define DCREG_LATENCY_COUNTER_LSB                                              1
#define DCREG_LATENCY_COUNTER_BLK                                              1
#define DCREG_LATENCY_COUNTER_Count                                            2
#define DCREG_LATENCY_COUNTER_FieldMask                               0xFFFFFFFF
#define DCREG_LATENCY_COUNTER_ReadMask                                0xFFFFFFFF
#define DCREG_LATENCY_COUNTER_WriteMask                               0xFFFFFFFF
#define DCREG_LATENCY_COUNTER_ResetValue                              0x00000000

/* Latency counter value which is used to judge whether latency is low or     **
** high.                                                                      */
#define DCREG_LATENCY_COUNTER_COUNTER                                       31:0
#define DCREG_LATENCY_COUNTER_COUNTER_End                                     31
#define DCREG_LATENCY_COUNTER_COUNTER_Start                                    0
#define DCREG_LATENCY_COUNTER_COUNTER_Type                                   U32

/* Register dcregQos (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregQosRegAddrs                                                  0x068E
#define DCREG_QOS_Address                                                0x01A38
#define DCREG_QOS_MSB                                                         15
#define DCREG_QOS_LSB                                                          1
#define DCREG_QOS_BLK                                                          1
#define DCREG_QOS_Count                                                        2
#define DCREG_QOS_FieldMask                                           0x000000FF
#define DCREG_QOS_ReadMask                                            0x000000FF
#define DCREG_QOS_WriteMask                                           0x000000FF
#define DCREG_QOS_ResetValue                                          0x00000000

/* If latency low, will send low value. */
#define DCREG_QOS_LOW                                                        3:0
#define DCREG_QOS_LOW_End                                                      3
#define DCREG_QOS_LOW_Start                                                    0
#define DCREG_QOS_LOW_Type                                                   U04

/* If latency high, will send high value. */
#define DCREG_QOS_HIGH                                                       7:4
#define DCREG_QOS_HIGH_End                                                     7
#define DCREG_QOS_HIGH_Start                                                   4
#define DCREG_QOS_HIGH_Type                                                  U04

/* Register dcregOverlayScaleFactorX (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Horizontal scale factor used to scale the overlay NOTE: This register is   **
** double buffered.                                                           */

#define dcregOverlayScaleFactorXRegAddrs                                  0x0690
#define DCREG_OVERLAY_SCALE_FACTOR_X_Address                             0x01A40
#define DCREG_OVERLAY_SCALE_FACTOR_X_MSB                                      15
#define DCREG_OVERLAY_SCALE_FACTOR_X_LSB                                       4
#define DCREG_OVERLAY_SCALE_FACTOR_X_BLK                                       4
#define DCREG_OVERLAY_SCALE_FACTOR_X_Count                                    16
#define DCREG_OVERLAY_SCALE_FACTOR_X_FieldMask                        0x7FFFFFFF
#define DCREG_OVERLAY_SCALE_FACTOR_X_ReadMask                         0x7FFFFFFF
#define DCREG_OVERLAY_SCALE_FACTOR_X_WriteMask                        0x7FFFFFFF
#define DCREG_OVERLAY_SCALE_FACTOR_X_ResetValue                       0x00000000

#define DCREG_OVERLAY_SCALE_FACTOR_X_X                                      30:0
#define DCREG_OVERLAY_SCALE_FACTOR_X_X_End                                    30
#define DCREG_OVERLAY_SCALE_FACTOR_X_X_Start                                   0
#define DCREG_OVERLAY_SCALE_FACTOR_X_X_Type                                  U31

/* Register dcregOverlayScaleFactorY (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Vertical scale factor used to scale the overlay NOTE: This register is     **
** double buffered.                                                           */

#define dcregOverlayScaleFactorYRegAddrs                                  0x06A0
#define DCREG_OVERLAY_SCALE_FACTOR_Y_Address                             0x01A80
#define DCREG_OVERLAY_SCALE_FACTOR_Y_MSB                                      15
#define DCREG_OVERLAY_SCALE_FACTOR_Y_LSB                                       4
#define DCREG_OVERLAY_SCALE_FACTOR_Y_BLK                                       4
#define DCREG_OVERLAY_SCALE_FACTOR_Y_Count                                    16
#define DCREG_OVERLAY_SCALE_FACTOR_Y_FieldMask                        0x7FFFFFFF
#define DCREG_OVERLAY_SCALE_FACTOR_Y_ReadMask                         0x7FFFFFFF
#define DCREG_OVERLAY_SCALE_FACTOR_Y_WriteMask                        0x7FFFFFFF
#define DCREG_OVERLAY_SCALE_FACTOR_Y_ResetValue                       0x00000000

#define DCREG_OVERLAY_SCALE_FACTOR_Y_Y                                      30:0
#define DCREG_OVERLAY_SCALE_FACTOR_Y_Y_End                                    30
#define DCREG_OVERLAY_SCALE_FACTOR_Y_Y_Start                                   0
#define DCREG_OVERLAY_SCALE_FACTOR_Y_Y_Type                                  U31

/* Register dcregOverlayHoriFilterKernelIndex (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Index into horizontal filter kernel table.  See                            **
** dcregOverlayHoriFilterKernel. for more information.                        */

#define dcregOverlayHoriFilterKernelIndexRegAddrs                         0x06B0
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_Address                   0x01AC0
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_MSB                            15
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_LSB                             4
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_BLK                             4
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_Count                          16
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_FieldMask              0x000000FF
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_ReadMask               0x00000000
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_WriteMask              0x000000FF
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_ResetValue             0x00000000

#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_INDEX                         7:0
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_INDEX_End                       7
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_INDEX_Start                     0
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_INDEX_INDEX_Type                    U08

/* Register dcregOverlayHoriFilterKernel (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* One Pass filter Filter hori coefficient table. NOTE: This register is      **
** double buffered.                                                           */

#define dcregOverlayHoriFilterKernelRegAddrs                              0x06C0
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_Address                         0x01B00
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_MSB                                  15
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_LSB                                   4
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_BLK                                   4
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_Count                                16
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_FieldMask                    0xFFFFFFFF
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_ReadMask                     0x00000000
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_WriteMask                    0xFFFFFFFF
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_ResetValue                   0x00000000

#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT0                       15:0
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT0_End                     15
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT0_Start                    0
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT0_Type                   U16

#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT1                      31:16
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT1_End                     31
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT1_Start                   16
#define DCREG_OVERLAY_HORI_FILTER_KERNEL_COEFFICIENT1_Type                   U16

/* Register dcregOverlayVertiFilterKernelIndex (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Index into overlay vertical filter kernel table.  See                      **
** dcregOverlayVertiFilterKernel. for more information.                       */

#define dcregOverlayVertiFilterKernelIndexRegAddrs                        0x06D0
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_Address                  0x01B40
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_MSB                           15
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_LSB                            4
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_BLK                            4
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_Count                         16
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_FieldMask             0x000000FF
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_ReadMask              0x00000000
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_WriteMask             0x000000FF
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_ResetValue            0x00000000

#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_INDEX                        7:0
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_INDEX_End                      7
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_INDEX_Start                    0
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_INDEX_INDEX_Type                   U08

/* Register dcregOverlayVertiFilterKernel (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* One Pass Filter vertical coefficient table. NOTE: This register is double  **
** buffered.                                                                  */

#define dcregOverlayVertiFilterKernelRegAddrs                             0x06E0
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_Address                        0x01B80
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_MSB                                 15
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_LSB                                  4
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_BLK                                  4
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_Count                               16
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_FieldMask                   0xFFFFFFFF
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_ReadMask                    0x00000000
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_WriteMask                   0xFFFFFFFF
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_ResetValue                  0x00000000

#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT0                      15:0
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT0_End                    15
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT0_Start                   0
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT0_Type                  U16

#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT1                     31:16
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT1_End                    31
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT1_Start                  16
#define DCREG_OVERLAY_VERTI_FILTER_KERNEL_COEFFICIENT1_Type                  U16

/* Register dcregOverlayInitialOffset (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Inital offset used when fetching source during scaling NOTE: This register **
** is double buffered.                                                        */

#define dcregOverlayInitialOffsetRegAddrs                                 0x06F0
#define DCREG_OVERLAY_INITIAL_OFFSET_Address                             0x01BC0
#define DCREG_OVERLAY_INITIAL_OFFSET_MSB                                      15
#define DCREG_OVERLAY_INITIAL_OFFSET_LSB                                       4
#define DCREG_OVERLAY_INITIAL_OFFSET_BLK                                       4
#define DCREG_OVERLAY_INITIAL_OFFSET_Count                                    16
#define DCREG_OVERLAY_INITIAL_OFFSET_FieldMask                        0xFFFFFFFF
#define DCREG_OVERLAY_INITIAL_OFFSET_ReadMask                         0xFFFFFFFF
#define DCREG_OVERLAY_INITIAL_OFFSET_WriteMask                        0xFFFFFFFF
#define DCREG_OVERLAY_INITIAL_OFFSET_ResetValue                       0x00000000

#define DCREG_OVERLAY_INITIAL_OFFSET_X                                      15:0
#define DCREG_OVERLAY_INITIAL_OFFSET_X_End                                    15
#define DCREG_OVERLAY_INITIAL_OFFSET_X_Start                                   0
#define DCREG_OVERLAY_INITIAL_OFFSET_X_Type                                  U16

#define DCREG_OVERLAY_INITIAL_OFFSET_Y                                     31:16
#define DCREG_OVERLAY_INITIAL_OFFSET_Y_End                                    31
#define DCREG_OVERLAY_INITIAL_OFFSET_Y_Start                                  16
#define DCREG_OVERLAY_INITIAL_OFFSET_Y_Type                                  U16

/* Register dcregOverlayScaleConfig (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Overlay scaling tap configure. NOTE: This register is double buffered. */

#define dcregOverlayScaleConfigRegAddrs                                   0x0700
#define DCREG_OVERLAY_SCALE_CONFIG_Address                               0x01C00
#define DCREG_OVERLAY_SCALE_CONFIG_MSB                                        15
#define DCREG_OVERLAY_SCALE_CONFIG_LSB                                         4
#define DCREG_OVERLAY_SCALE_CONFIG_BLK                                         4
#define DCREG_OVERLAY_SCALE_CONFIG_Count                                      16
#define DCREG_OVERLAY_SCALE_CONFIG_FieldMask                          0x000001FF
#define DCREG_OVERLAY_SCALE_CONFIG_ReadMask                           0x000001FF
#define DCREG_OVERLAY_SCALE_CONFIG_WriteMask                          0x000001FF
#define DCREG_OVERLAY_SCALE_CONFIG_ResetValue                         0x00000000

#define DCREG_OVERLAY_SCALE_CONFIG_VERTICAL_FILTER_TAP                       3:0
#define DCREG_OVERLAY_SCALE_CONFIG_VERTICAL_FILTER_TAP_End                     3
#define DCREG_OVERLAY_SCALE_CONFIG_VERTICAL_FILTER_TAP_Start                   0
#define DCREG_OVERLAY_SCALE_CONFIG_VERTICAL_FILTER_TAP_Type                  U04

#define DCREG_OVERLAY_SCALE_CONFIG_HORIZONTAL_FILTER_TAP                     7:4
#define DCREG_OVERLAY_SCALE_CONFIG_HORIZONTAL_FILTER_TAP_End                   7
#define DCREG_OVERLAY_SCALE_CONFIG_HORIZONTAL_FILTER_TAP_Start                 4
#define DCREG_OVERLAY_SCALE_CONFIG_HORIZONTAL_FILTER_TAP_Type                U04

#define DCREG_OVERLAY_SCALE_CONFIG_SCALE                                     8:8
#define DCREG_OVERLAY_SCALE_CONFIG_SCALE_End                                   8
#define DCREG_OVERLAY_SCALE_CONFIG_SCALE_Start                                 8
#define DCREG_OVERLAY_SCALE_CONFIG_SCALE_Type                                U01
#define   DCREG_OVERLAY_SCALE_CONFIG_SCALE_DISABLED                          0x0
#define   DCREG_OVERLAY_SCALE_CONFIG_SCALE_ENABLED                           0x1

/* Register dcregMpuIntfCmd (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfCmdRegAddrs                                           0x0710
#define DCREG_MPU_INTF_CMD_Address                                       0x01C40
#define DCREG_MPU_INTF_CMD_MSB                                                15
#define DCREG_MPU_INTF_CMD_LSB                                                 1
#define DCREG_MPU_INTF_CMD_BLK                                                 1
#define DCREG_MPU_INTF_CMD_Count                                               2
#define DCREG_MPU_INTF_CMD_FieldMask                                  0xE700FFFF
#define DCREG_MPU_INTF_CMD_ReadMask                                   0xC700FFFF
#define DCREG_MPU_INTF_CMD_WriteMask                                  0xE700FFFF
#define DCREG_MPU_INTF_CMD_ResetValue                                 0x00000000

/* Regsiter address or register configured parameter. */
#define DCREG_MPU_INTF_CMD_REG_DATA                                         15:0
#define DCREG_MPU_INTF_CMD_REG_DATA_End                                       15
#define DCREG_MPU_INTF_CMD_REG_DATA_Start                                      0
#define DCREG_MPU_INTF_CMD_REG_DATA_Type                                     U16

/* 0 is invalid value, one read operation returns 1 data at least, max value  **
** is 5, it's designed to prepare 5 registers to save returned data.          */
#define DCREG_MPU_INTF_CMD_READ_COUNT                                      26:24
#define DCREG_MPU_INTF_CMD_READ_COUNT_End                                     26
#define DCREG_MPU_INTF_CMD_READ_COUNT_Start                                   24
#define DCREG_MPU_INTF_CMD_READ_COUNT_Type                                   U03

/* HW generates a pulse while writing this field to 1, then HW starts to send **
** display data to LCD.                                                       */
#define DCREG_MPU_INTF_CMD_START                                           29:29
#define DCREG_MPU_INTF_CMD_START_End                                          29
#define DCREG_MPU_INTF_CMD_START_Start                                        29
#define DCREG_MPU_INTF_CMD_START_Type                                        U01

/* 0: Not a valid command, HW ignore CMD/REGDATA/READCOUNT field. Usage: only **
** want to trigger HW, don't configure other field. 1: Write register by MPU  **
** interface. 2: Write register parameter by MPU interface. 3: Read register  **
** by MPU interface.                                                          */
#define DCREG_MPU_INTF_CMD_CMD                                             31:30
#define DCREG_MPU_INTF_CMD_CMD_End                                            31
#define DCREG_MPU_INTF_CMD_CMD_Start                                          30
#define DCREG_MPU_INTF_CMD_CMD_Type                                          U02

/* Register dcregMpuIntfReadPara0 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfReadPara0RegAddrs                                     0x0712
#define DCREG_MPU_INTF_READ_PARA0_Address                                0x01C48
#define DCREG_MPU_INTF_READ_PARA0_MSB                                         15
#define DCREG_MPU_INTF_READ_PARA0_LSB                                          1
#define DCREG_MPU_INTF_READ_PARA0_BLK                                          1
#define DCREG_MPU_INTF_READ_PARA0_Count                                        2
#define DCREG_MPU_INTF_READ_PARA0_FieldMask                           0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA0_ReadMask                            0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA0_WriteMask                           0x00000000
#define DCREG_MPU_INTF_READ_PARA0_ResetValue                          0x00000000

/* Read register returned parameter0. */
#define DCREG_MPU_INTF_READ_PARA0_DATA                                      17:0
#define DCREG_MPU_INTF_READ_PARA0_DATA_End                                    17
#define DCREG_MPU_INTF_READ_PARA0_DATA_Start                                   0
#define DCREG_MPU_INTF_READ_PARA0_DATA_Type                                  U18

/* Register dcregMpuIntfReadPara1 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfReadPara1RegAddrs                                     0x0714
#define DCREG_MPU_INTF_READ_PARA1_Address                                0x01C50
#define DCREG_MPU_INTF_READ_PARA1_MSB                                         15
#define DCREG_MPU_INTF_READ_PARA1_LSB                                          1
#define DCREG_MPU_INTF_READ_PARA1_BLK                                          1
#define DCREG_MPU_INTF_READ_PARA1_Count                                        2
#define DCREG_MPU_INTF_READ_PARA1_FieldMask                           0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA1_ReadMask                            0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA1_WriteMask                           0x00000000
#define DCREG_MPU_INTF_READ_PARA1_ResetValue                          0x00000000

/* Read register returned parameter1. */
#define DCREG_MPU_INTF_READ_PARA1_DATA                                      17:0
#define DCREG_MPU_INTF_READ_PARA1_DATA_End                                    17
#define DCREG_MPU_INTF_READ_PARA1_DATA_Start                                   0
#define DCREG_MPU_INTF_READ_PARA1_DATA_Type                                  U18

/* Register dcregMpuIntfReadPara2 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfReadPara2RegAddrs                                     0x0716
#define DCREG_MPU_INTF_READ_PARA2_Address                                0x01C58
#define DCREG_MPU_INTF_READ_PARA2_MSB                                         15
#define DCREG_MPU_INTF_READ_PARA2_LSB                                          1
#define DCREG_MPU_INTF_READ_PARA2_BLK                                          1
#define DCREG_MPU_INTF_READ_PARA2_Count                                        2
#define DCREG_MPU_INTF_READ_PARA2_FieldMask                           0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA2_ReadMask                            0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA2_WriteMask                           0x00000000
#define DCREG_MPU_INTF_READ_PARA2_ResetValue                          0x00000000

/* Read register returned parameter2. */
#define DCREG_MPU_INTF_READ_PARA2_DATA                                      17:0
#define DCREG_MPU_INTF_READ_PARA2_DATA_End                                    17
#define DCREG_MPU_INTF_READ_PARA2_DATA_Start                                   0
#define DCREG_MPU_INTF_READ_PARA2_DATA_Type                                  U18

/* Register dcregMpuIntfReadPara3 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfReadPara3RegAddrs                                     0x0718
#define DCREG_MPU_INTF_READ_PARA3_Address                                0x01C60
#define DCREG_MPU_INTF_READ_PARA3_MSB                                         15
#define DCREG_MPU_INTF_READ_PARA3_LSB                                          1
#define DCREG_MPU_INTF_READ_PARA3_BLK                                          1
#define DCREG_MPU_INTF_READ_PARA3_Count                                        2
#define DCREG_MPU_INTF_READ_PARA3_FieldMask                           0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA3_ReadMask                            0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA3_WriteMask                           0x00000000
#define DCREG_MPU_INTF_READ_PARA3_ResetValue                          0x00000000

/* Read register returned parameter3. */
#define DCREG_MPU_INTF_READ_PARA3_DATA                                      17:0
#define DCREG_MPU_INTF_READ_PARA3_DATA_End                                    17
#define DCREG_MPU_INTF_READ_PARA3_DATA_Start                                   0
#define DCREG_MPU_INTF_READ_PARA3_DATA_Type                                  U18

/* Register dcregMpuIntfReadPara4 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfReadPara4RegAddrs                                     0x071A
#define DCREG_MPU_INTF_READ_PARA4_Address                                0x01C68
#define DCREG_MPU_INTF_READ_PARA4_MSB                                         15
#define DCREG_MPU_INTF_READ_PARA4_LSB                                          1
#define DCREG_MPU_INTF_READ_PARA4_BLK                                          1
#define DCREG_MPU_INTF_READ_PARA4_Count                                        2
#define DCREG_MPU_INTF_READ_PARA4_FieldMask                           0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA4_ReadMask                            0x0003FFFF
#define DCREG_MPU_INTF_READ_PARA4_WriteMask                           0x00000000
#define DCREG_MPU_INTF_READ_PARA4_ResetValue                          0x00000000

/* Read register returned parameter4. */
#define DCREG_MPU_INTF_READ_PARA4_DATA                                      17:0
#define DCREG_MPU_INTF_READ_PARA4_DATA_End                                    17
#define DCREG_MPU_INTF_READ_PARA4_DATA_Start                                   0
#define DCREG_MPU_INTF_READ_PARA4_DATA_Type                                  U18

/* Register dcregMpuIntfReadStatus (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfReadStatusRegAddrs                                    0x071C
#define DCREG_MPU_INTF_READ_STATUS_Address                               0x01C70
#define DCREG_MPU_INTF_READ_STATUS_MSB                                        15
#define DCREG_MPU_INTF_READ_STATUS_LSB                                         1
#define DCREG_MPU_INTF_READ_STATUS_BLK                                         1
#define DCREG_MPU_INTF_READ_STATUS_Count                                       2
#define DCREG_MPU_INTF_READ_STATUS_FieldMask                          0x00000001
#define DCREG_MPU_INTF_READ_STATUS_ReadMask                           0x00000001
#define DCREG_MPU_INTF_READ_STATUS_WriteMask                          0x00000000
#define DCREG_MPU_INTF_READ_STATUS_ResetValue                         0x00000000

/* 0: The returned parameters of all read registers are not ready. 1: The     **
** returned parameters of all read registers are ready.                       */
#define DCREG_MPU_INTF_READ_STATUS_DATA                                      0:0
#define DCREG_MPU_INTF_READ_STATUS_DATA_End                                    0
#define DCREG_MPU_INTF_READ_STATUS_DATA_Start                                  0
#define DCREG_MPU_INTF_READ_STATUS_DATA_Type                                 U01
#define   DCREG_MPU_INTF_READ_STATUS_DATA_UNREADY                            0x0
#define   DCREG_MPU_INTF_READ_STATUS_DATA_READY                              0x1

/* Register dcregMpuIntfConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfConfigRegAddrs                                        0x071E
#define DCREG_MPU_INTF_CONFIG_Address                                    0x01C78
#define DCREG_MPU_INTF_CONFIG_MSB                                             15
#define DCREG_MPU_INTF_CONFIG_LSB                                              1
#define DCREG_MPU_INTF_CONFIG_BLK                                              1
#define DCREG_MPU_INTF_CONFIG_Count                                            2
#define DCREG_MPU_INTF_CONFIG_FieldMask                               0x00001FFF
#define DCREG_MPU_INTF_CONFIG_ReadMask                                0x000017FF
#define DCREG_MPU_INTF_CONFIG_WriteMask                               0x00001FFF
#define DCREG_MPU_INTF_CONFIG_ResetValue                              0x00000000

/* 0: 8 bit register mode, only dcregMpuIntfCmd[7:0] are valid, each time     **
** only send 8 bit valid data to LCD by MPU Interface. 1: 16 bit register     **
** mode, only dcregMpuIntfCmd[15:0] are valid, each time only send 16 bit     **
** valid data to LCD by MPU Interface.                                        */
#define DCREG_MPU_INTF_CONFIG_REGISTER_MODE                                  0:0
#define DCREG_MPU_INTF_CONFIG_REGISTER_MODE_End                                0
#define DCREG_MPU_INTF_CONFIG_REGISTER_MODE_Start                              0
#define DCREG_MPU_INTF_CONFIG_REGISTER_MODE_Type                             U01
#define   DCREG_MPU_INTF_CONFIG_REGISTER_MODE_MODE_8_BIT                     0x0
#define   DCREG_MPU_INTF_CONFIG_REGISTER_MODE_MODE_16_BIT                    0x1

/* 0: MPU 8 bit interface. 1: MPU 9 bit interface. 2: MPU 16 bit interface.   **
** 3: MPU 18 bit interface.                                                   */
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH                                 2:1
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH_End                               2
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH_Start                             1
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH_Type                            U02
#define   DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH_WIDTH_8_BIT                   0x0
#define   DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH_WIDTH_9_BIT                   0x1
#define   DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH_WIDTH_16_BIT                  0x2
#define   DCREG_MPU_INTF_CONFIG_DATA_BUS_WIDTH_WIDTH_18_BIT                  0x3

/* 0: Mode0: 8-bit:DB[7:0] 9-bit:DB[8:0] 16-bit:DB[15:0] 18-bit:DB[17:0] 1:   **
** Mode1: 8-bit:DB[17:10] 9-bit:DB[17:9] 16-bit:{DB[17:10],DB[8:1]}           **
** 18-bit:DB[17:0]                                                            */
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_MODE                                  3:3
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_MODE_End                                3
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_MODE_Start                              3
#define DCREG_MPU_INTF_CONFIG_DATA_BUS_MODE_Type                             U01
#define   DCREG_MPU_INTF_CONFIG_DATA_BUS_MODE_MODE0                          0x0
#define   DCREG_MPU_INTF_CONFIG_DATA_BUS_MODE_MODE1                          0x1

/* 0: I80 system interface. 1: M68 system interface. */
#define DCREG_MPU_INTF_CONFIG_INTERFACE_MODE                                 4:4
#define DCREG_MPU_INTF_CONFIG_INTERFACE_MODE_End                               4
#define DCREG_MPU_INTF_CONFIG_INTERFACE_MODE_Start                             4
#define DCREG_MPU_INTF_CONFIG_INTERFACE_MODE_Type                            U01
#define   DCREG_MPU_INTF_CONFIG_INTERFACE_MODE_I80                           0x0
#define   DCREG_MPU_INTF_CONFIG_INTERFACE_MODE_M68                           0x1

/* 0: Disable VSYNC interface. 1: Enable VSYNC interface. */
#define DCREG_MPU_INTF_CONFIG_ENABLE_VSYNC                                   5:5
#define DCREG_MPU_INTF_CONFIG_ENABLE_VSYNC_End                                 5
#define DCREG_MPU_INTF_CONFIG_ENABLE_VSYNC_Start                               5
#define DCREG_MPU_INTF_CONFIG_ENABLE_VSYNC_Type                              U01
#define   DCREG_MPU_INTF_CONFIG_ENABLE_VSYNC_DISABLE                         0x0
#define   DCREG_MPU_INTF_CONFIG_ENABLE_VSYNC_ENABLE                          0x1

/* 0: VSync signal is negative value. 1: VSync signal is positive value. */
#define DCREG_MPU_INTF_CONFIG_VSYNC_POLARITY                                 6:6
#define DCREG_MPU_INTF_CONFIG_VSYNC_POLARITY_End                               6
#define DCREG_MPU_INTF_CONFIG_VSYNC_POLARITY_Start                             6
#define DCREG_MPU_INTF_CONFIG_VSYNC_POLARITY_Type                            U01
#define   DCREG_MPU_INTF_CONFIG_VSYNC_POLARITY_NEGATIVE                      0x0
#define   DCREG_MPU_INTF_CONFIG_VSYNC_POLARITY_POSITIVE                      0x1

/* 0: Disable TE interface. 1: Enable TE interface. */
#define DCREG_MPU_INTF_CONFIG_ENABLE_TE                                      7:7
#define DCREG_MPU_INTF_CONFIG_ENABLE_TE_End                                    7
#define DCREG_MPU_INTF_CONFIG_ENABLE_TE_Start                                  7
#define DCREG_MPU_INTF_CONFIG_ENABLE_TE_Type                                 U01
#define   DCREG_MPU_INTF_CONFIG_ENABLE_TE_DISABLE                            0x0
#define   DCREG_MPU_INTF_CONFIG_ENABLE_TE_ENABLE                             0x1

/* 0: TE signal is positive value. 1: TE signal is negative value. */
#define DCREG_MPU_INTF_CONFIG_TE_POLARITY                                    8:8
#define DCREG_MPU_INTF_CONFIG_TE_POLARITY_End                                  8
#define DCREG_MPU_INTF_CONFIG_TE_POLARITY_Start                                8
#define DCREG_MPU_INTF_CONFIG_TE_POLARITY_Type                               U01
#define   DCREG_MPU_INTF_CONFIG_TE_POLARITY_POSITIVE                         0x0
#define   DCREG_MPU_INTF_CONFIG_TE_POLARITY_NEGATIVE                         0x1

/* 0: 0-command, 1-data. 1: 1-command, 0-data. */
#define DCREG_MPU_INTF_CONFIG_DCX_POLARITY                                   9:9
#define DCREG_MPU_INTF_CONFIG_DCX_POLARITY_End                                 9
#define DCREG_MPU_INTF_CONFIG_DCX_POLARITY_Start                               9
#define DCREG_MPU_INTF_CONFIG_DCX_POLARITY_Type                              U01
#define   DCREG_MPU_INTF_CONFIG_DCX_POLARITY_MODE0                           0x0
#define   DCREG_MPU_INTF_CONFIG_DCX_POLARITY_MODE1                           0x1

/* 16 bit data bus, 24 bit data output. 0: 2pixels per 3 transfers. 1: 1pixel **
** per 2 transfers.                                                           */
#define DCREG_MPU_INTF_CONFIG_DATA_MODE24_BIT                              10:10
#define DCREG_MPU_INTF_CONFIG_DATA_MODE24_BIT_End                             10
#define DCREG_MPU_INTF_CONFIG_DATA_MODE24_BIT_Start                           10
#define DCREG_MPU_INTF_CONFIG_DATA_MODE24_BIT_Type                           U01
#define   DCREG_MPU_INTF_CONFIG_DATA_MODE24_BIT_MODE0                        0x0
#define   DCREG_MPU_INTF_CONFIG_DATA_MODE24_BIT_MODE1                        0x1

/* Write 1 to this field, HW will reset the MPU interface. */
#define DCREG_MPU_INTF_CONFIG_INTERFACE_RESET                              11:11
#define DCREG_MPU_INTF_CONFIG_INTERFACE_RESET_End                             11
#define DCREG_MPU_INTF_CONFIG_INTERFACE_RESET_Start                           11
#define DCREG_MPU_INTF_CONFIG_INTERFACE_RESET_Type                           U01

/* O:Disable MPU interface. 1:Enable MPU interface. */
#define DCREG_MPU_INTF_CONFIG_ENABLE_MPU_INTF                              12:12
#define DCREG_MPU_INTF_CONFIG_ENABLE_MPU_INTF_End                             12
#define DCREG_MPU_INTF_CONFIG_ENABLE_MPU_INTF_Start                           12
#define DCREG_MPU_INTF_CONFIG_ENABLE_MPU_INTF_Type                           U01
#define   DCREG_MPU_INTF_CONFIG_ENABLE_MPU_INTF_DISABLE                      0x0
#define   DCREG_MPU_INTF_CONFIG_ENABLE_MPU_INTF_ENABLE                       0x1

/* Register dcregMpuIntfFrame (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfFrameRegAddrs                                         0x0720
#define DCREG_MPU_INTF_FRAME_Address                                     0x01C80
#define DCREG_MPU_INTF_FRAME_MSB                                              15
#define DCREG_MPU_INTF_FRAME_LSB                                               1
#define DCREG_MPU_INTF_FRAME_BLK                                               1
#define DCREG_MPU_INTF_FRAME_Count                                             2
#define DCREG_MPU_INTF_FRAME_FieldMask                                0x0000000F
#define DCREG_MPU_INTF_FRAME_ReadMask                                 0x0000000F
#define DCREG_MPU_INTF_FRAME_WriteMask                                0x0000000F
#define DCREG_MPU_INTF_FRAME_ResetValue                               0x00000000

/* 0: When using VSync interface/TE mode, if this bit is 0, this frame data   **
** won't be updated. 1: When using VSync interface/TE mode, if this bit is 1, **
** this frame data will be updated.                                           */
#define DCREG_MPU_INTF_FRAME_FRAME_UPDATE                                    0:0
#define DCREG_MPU_INTF_FRAME_FRAME_UPDATE_End                                  0
#define DCREG_MPU_INTF_FRAME_FRAME_UPDATE_Start                                0
#define DCREG_MPU_INTF_FRAME_FRAME_UPDATE_Type                               U01
#define   DCREG_MPU_INTF_FRAME_FRAME_UPDATE_NO                               0x0
#define   DCREG_MPU_INTF_FRAME_FRAME_UPDATE_YES                              0x1

/* 0: 16 bit format R5G6B5. 1: 18 bit format R6G6B6. 2: 24 bit format R8G8B8. **
** 3: Reserved.                                                               */
#define DCREG_MPU_INTF_FRAME_DATA_FORMAT                                     2:1
#define DCREG_MPU_INTF_FRAME_DATA_FORMAT_End                                   2
#define DCREG_MPU_INTF_FRAME_DATA_FORMAT_Start                                 1
#define DCREG_MPU_INTF_FRAME_DATA_FORMAT_Type                                U02
#define   DCREG_MPU_INTF_FRAME_DATA_FORMAT_R5G6B5                            0x0
#define   DCREG_MPU_INTF_FRAME_DATA_FORMAT_R6G6B6                            0x1
#define   DCREG_MPU_INTF_FRAME_DATA_FORMAT_R8G8B8                            0x2

/* 0: Disable MPU data write back. 1: Enable MPU interface data write back. */
#define DCREG_MPU_INTF_FRAME_MPU_WRITE_BACK                                  3:3
#define DCREG_MPU_INTF_FRAME_MPU_WRITE_BACK_End                                3
#define DCREG_MPU_INTF_FRAME_MPU_WRITE_BACK_Start                              3
#define DCREG_MPU_INTF_FRAME_MPU_WRITE_BACK_Type                             U01
#define   DCREG_MPU_INTF_FRAME_MPU_WRITE_BACK_DISABLE                        0x0
#define   DCREG_MPU_INTF_FRAME_MPU_WRITE_BACK_ENABLE                         0x1

/* Register dcregMpuIntfACWrI80 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfACWrI80RegAddrs                                       0x0722
#define DCREG_MPU_INTF_AC_WR_I80_Address                                 0x01C88
#define DCREG_MPU_INTF_AC_WR_I80_MSB                                          15
#define DCREG_MPU_INTF_AC_WR_I80_LSB                                           1
#define DCREG_MPU_INTF_AC_WR_I80_BLK                                           1
#define DCREG_MPU_INTF_AC_WR_I80_Count                                         2
#define DCREG_MPU_INTF_AC_WR_I80_FieldMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_WR_I80_ReadMask                             0x3FFFFFFF
#define DCREG_MPU_INTF_AC_WR_I80_WriteMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_WR_I80_ResetValue                           0x00000000

/* I80 system write period cycle number. Minimun number is 3. Default number  **
** is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                  */
#define DCREG_MPU_INTF_AC_WR_I80_WR_PERIOD_I80                               9:0
#define DCREG_MPU_INTF_AC_WR_I80_WR_PERIOD_I80_End                             9
#define DCREG_MPU_INTF_AC_WR_I80_WR_PERIOD_I80_Start                           0
#define DCREG_MPU_INTF_AC_WR_I80_WR_PERIOD_I80_Type                          U10

/* I80 system WRX assert cycle number. Minimun number is 1. Default number is **
** 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                     */
#define DCREG_MPU_INTF_AC_WR_I80_WRX_ASSERT                                19:10
#define DCREG_MPU_INTF_AC_WR_I80_WRX_ASSERT_End                               19
#define DCREG_MPU_INTF_AC_WR_I80_WRX_ASSERT_Start                             10
#define DCREG_MPU_INTF_AC_WR_I80_WRX_ASSERT_Type                             U10

/* I80 system WRX de-assert cycle number. Minimun number is 1. Default number **
** is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                  */
#define DCREG_MPU_INTF_AC_WR_I80_WRX_DE_ASSERT                             29:20
#define DCREG_MPU_INTF_AC_WR_I80_WRX_DE_ASSERT_End                            29
#define DCREG_MPU_INTF_AC_WR_I80_WRX_DE_ASSERT_Start                          20
#define DCREG_MPU_INTF_AC_WR_I80_WRX_DE_ASSERT_Type                          U10

/* Register dcregMpuIntfACRdI80 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfACRdI80RegAddrs                                       0x0724
#define DCREG_MPU_INTF_AC_RD_I80_Address                                 0x01C90
#define DCREG_MPU_INTF_AC_RD_I80_MSB                                          15
#define DCREG_MPU_INTF_AC_RD_I80_LSB                                           1
#define DCREG_MPU_INTF_AC_RD_I80_BLK                                           1
#define DCREG_MPU_INTF_AC_RD_I80_Count                                         2
#define DCREG_MPU_INTF_AC_RD_I80_FieldMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_RD_I80_ReadMask                             0x3FFFFFFF
#define DCREG_MPU_INTF_AC_RD_I80_WriteMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_RD_I80_ResetValue                           0x00000000

/* I80 system read period cycle number. Minimun number is 3. Default number   **
** is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                  */
#define DCREG_MPU_INTF_AC_RD_I80_RD_PERIOD_I80                               9:0
#define DCREG_MPU_INTF_AC_RD_I80_RD_PERIOD_I80_End                             9
#define DCREG_MPU_INTF_AC_RD_I80_RD_PERIOD_I80_Start                           0
#define DCREG_MPU_INTF_AC_RD_I80_RD_PERIOD_I80_Type                          U10

/* I80 system RDX assert cycle number. Minimun number is 1. Default number is **
** 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                     */
#define DCREG_MPU_INTF_AC_RD_I80_RDX_ASSERT                                19:10
#define DCREG_MPU_INTF_AC_RD_I80_RDX_ASSERT_End                               19
#define DCREG_MPU_INTF_AC_RD_I80_RDX_ASSERT_Start                             10
#define DCREG_MPU_INTF_AC_RD_I80_RDX_ASSERT_Type                             U10

/* I80 system RDX de-assert cycle number. Minimun number is 1. Default number **
** is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                  */
#define DCREG_MPU_INTF_AC_RD_I80_RDX_DE_ASSERT                             29:20
#define DCREG_MPU_INTF_AC_RD_I80_RDX_DE_ASSERT_End                            29
#define DCREG_MPU_INTF_AC_RD_I80_RDX_DE_ASSERT_Start                          20
#define DCREG_MPU_INTF_AC_RD_I80_RDX_DE_ASSERT_Type                          U10

/* Register dcregMpuIntfACWrM68 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfACWrM68RegAddrs                                       0x0726
#define DCREG_MPU_INTF_AC_WR_M68_Address                                 0x01C98
#define DCREG_MPU_INTF_AC_WR_M68_MSB                                          15
#define DCREG_MPU_INTF_AC_WR_M68_LSB                                           1
#define DCREG_MPU_INTF_AC_WR_M68_BLK                                           1
#define DCREG_MPU_INTF_AC_WR_M68_Count                                         2
#define DCREG_MPU_INTF_AC_WR_M68_FieldMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_WR_M68_ReadMask                             0x3FFFFFFF
#define DCREG_MPU_INTF_AC_WR_M68_WriteMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_WR_M68_ResetValue                           0x00000000

/* M68 system write period cycle number. Minimun number is 3. Default number  **
** is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                  */
#define DCREG_MPU_INTF_AC_WR_M68_WR_PERIOD_M68                               9:0
#define DCREG_MPU_INTF_AC_WR_M68_WR_PERIOD_M68_End                             9
#define DCREG_MPU_INTF_AC_WR_M68_WR_PERIOD_M68_Start                           0
#define DCREG_MPU_INTF_AC_WR_M68_WR_PERIOD_M68_Type                          U10

/* M68 system write E assert cycle number. Minimun number is 1. Default       **
** number is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.           */
#define DCREG_MPU_INTF_AC_WR_M68_WR_EASSERT                                19:10
#define DCREG_MPU_INTF_AC_WR_M68_WR_EASSERT_End                               19
#define DCREG_MPU_INTF_AC_WR_M68_WR_EASSERT_Start                             10
#define DCREG_MPU_INTF_AC_WR_M68_WR_EASSERT_Type                             U10

/* M68 system write E de-assert cycle number. Minimun number is 1. Default    **
** number is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.           */
#define DCREG_MPU_INTF_AC_WR_M68_WR_EDE_ASSERT                             29:20
#define DCREG_MPU_INTF_AC_WR_M68_WR_EDE_ASSERT_End                            29
#define DCREG_MPU_INTF_AC_WR_M68_WR_EDE_ASSERT_Start                          20
#define DCREG_MPU_INTF_AC_WR_M68_WR_EDE_ASSERT_Type                          U10

/* Register dcregMpuIntfACRdM68 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfACRdM68RegAddrs                                       0x0728
#define DCREG_MPU_INTF_AC_RD_M68_Address                                 0x01CA0
#define DCREG_MPU_INTF_AC_RD_M68_MSB                                          15
#define DCREG_MPU_INTF_AC_RD_M68_LSB                                           1
#define DCREG_MPU_INTF_AC_RD_M68_BLK                                           1
#define DCREG_MPU_INTF_AC_RD_M68_Count                                         2
#define DCREG_MPU_INTF_AC_RD_M68_FieldMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_RD_M68_ReadMask                             0x3FFFFFFF
#define DCREG_MPU_INTF_AC_RD_M68_WriteMask                            0x3FFFFFFF
#define DCREG_MPU_INTF_AC_RD_M68_ResetValue                           0x00000000

/* M68 system read period cycle number. Minimun number is 3. Default number   **
** is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                  */
#define DCREG_MPU_INTF_AC_RD_M68_RD_PERIOD_M68                               9:0
#define DCREG_MPU_INTF_AC_RD_M68_RD_PERIOD_M68_End                             9
#define DCREG_MPU_INTF_AC_RD_M68_RD_PERIOD_M68_Start                           0
#define DCREG_MPU_INTF_AC_RD_M68_RD_PERIOD_M68_Type                          U10

/* M68 system read E assert cycle number. Minimun number is 1. Default number **
** is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.                  */
#define DCREG_MPU_INTF_AC_RD_M68_RD_EASSERT                                19:10
#define DCREG_MPU_INTF_AC_RD_M68_RD_EASSERT_End                               19
#define DCREG_MPU_INTF_AC_RD_M68_RD_EASSERT_Start                             10
#define DCREG_MPU_INTF_AC_RD_M68_RD_EASSERT_Type                             U10

/* M68 system read E de-assert cycle number. Minimun number is 1. Default     **
** number is 0, 0 means 1024 cycle number. Cycle unit is pixel clk.           */
#define DCREG_MPU_INTF_AC_RD_M68_RD_EDE_ASSERT                             29:20
#define DCREG_MPU_INTF_AC_RD_M68_RD_EDE_ASSERT_End                            29
#define DCREG_MPU_INTF_AC_RD_M68_RD_EDE_ASSERT_Start                          20
#define DCREG_MPU_INTF_AC_RD_M68_RD_EDE_ASSERT_Type                          U10

/* Register dcregMpuIntfACVsyncCSX (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregMpuIntfACVsyncCSXRegAddrs                                    0x072A
#define DCREG_MPU_INTF_AC_VSYNC_CSX_Address                              0x01CA8
#define DCREG_MPU_INTF_AC_VSYNC_CSX_MSB                                       15
#define DCREG_MPU_INTF_AC_VSYNC_CSX_LSB                                        1
#define DCREG_MPU_INTF_AC_VSYNC_CSX_BLK                                        1
#define DCREG_MPU_INTF_AC_VSYNC_CSX_Count                                      2
#define DCREG_MPU_INTF_AC_VSYNC_CSX_FieldMask                         0x000003FF
#define DCREG_MPU_INTF_AC_VSYNC_CSX_ReadMask                          0x000003FF
#define DCREG_MPU_INTF_AC_VSYNC_CSX_WriteMask                         0x000003FF
#define DCREG_MPU_INTF_AC_VSYNC_CSX_ResetValue                        0x00000000

/* Add delay between VSYNC-high and CSX-low. This field means how many        **
** transfer-one-pixel period.                                                 */
#define DCREG_MPU_INTF_AC_VSYNC_CSX_CSX_ASSERT                               9:0
#define DCREG_MPU_INTF_AC_VSYNC_CSX_CSX_ASSERT_End                             9
#define DCREG_MPU_INTF_AC_VSYNC_CSX_CSX_ASSERT_Start                           0
#define DCREG_MPU_INTF_AC_VSYNC_CSX_CSX_ASSERT_Type                          U10

/* Register dcregFrameBufferROIOrigin (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Framebuffer Region of Interest Origin Register.  NOTE: This register is    **
** double buffered.                                                           */

#define dcregFrameBufferROIOriginRegAddrs                                 0x072C
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_Address                            0x01CB0
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_MSB                                     15
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_LSB                                      1
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_BLK                                      1
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_Count                                    2
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_FieldMask                       0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_ReadMask                        0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_WriteMask                       0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_ResetValue                      0x00000000

/* OSD/Rectangle start point X coordinate. */
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_X                                     15:0
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_X_End                                   15
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_X_Start                                  0
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_X_Type                                 U16

/* OSD/Rectangle start point Y coordinate. */
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_Y                                    31:16
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_Y_End                                   31
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_Y_Start                                 16
#define DCREG_FRAME_BUFFER_ROI_ORIGIN_Y_Type                                 U16

/* Register dcregFrameBufferROISize (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Framebuffer Region of Interest Size Register.  NOTE: This register is      **
** double buffered.                                                           */

#define dcregFrameBufferROISizeRegAddrs                                   0x072E
#define DCREG_FRAME_BUFFER_ROI_SIZE_Address                              0x01CB8
#define DCREG_FRAME_BUFFER_ROI_SIZE_MSB                                       15
#define DCREG_FRAME_BUFFER_ROI_SIZE_LSB                                        1
#define DCREG_FRAME_BUFFER_ROI_SIZE_BLK                                        1
#define DCREG_FRAME_BUFFER_ROI_SIZE_Count                                      2
#define DCREG_FRAME_BUFFER_ROI_SIZE_FieldMask                         0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ROI_SIZE_ReadMask                          0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ROI_SIZE_WriteMask                         0xFFFFFFFF
#define DCREG_FRAME_BUFFER_ROI_SIZE_ResetValue                        0x00000000

/* OSD/Rectangle width. */
#define DCREG_FRAME_BUFFER_ROI_SIZE_WIDTH                                   15:0
#define DCREG_FRAME_BUFFER_ROI_SIZE_WIDTH_End                                 15
#define DCREG_FRAME_BUFFER_ROI_SIZE_WIDTH_Start                                0
#define DCREG_FRAME_BUFFER_ROI_SIZE_WIDTH_Type                               U16

/* OSD/Rectangle height. */
#define DCREG_FRAME_BUFFER_ROI_SIZE_HEIGHT                                 31:16
#define DCREG_FRAME_BUFFER_ROI_SIZE_HEIGHT_End                                31
#define DCREG_FRAME_BUFFER_ROI_SIZE_HEIGHT_Start                              16
#define DCREG_FRAME_BUFFER_ROI_SIZE_HEIGHT_Type                              U16

/* Register dcregFrameBufferConfigEx (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Framebuffer Configuration Extensions Register.  NOTE: This register is     **
** double buffered.                                                           */

#define dcregFrameBufferConfigExRegAddrs                                  0x0730
#define DCREG_FRAME_BUFFER_CONFIG_EX_Address                             0x01CC0
#define DCREG_FRAME_BUFFER_CONFIG_EX_MSB                                      15
#define DCREG_FRAME_BUFFER_CONFIG_EX_LSB                                       1
#define DCREG_FRAME_BUFFER_CONFIG_EX_BLK                                       1
#define DCREG_FRAME_BUFFER_CONFIG_EX_Count                                     2
#define DCREG_FRAME_BUFFER_CONFIG_EX_FieldMask                        0x00000FFF
#define DCREG_FRAME_BUFFER_CONFIG_EX_ReadMask                         0x00000FFF
#define DCREG_FRAME_BUFFER_CONFIG_EX_WriteMask                        0x00000FFF
#define DCREG_FRAME_BUFFER_CONFIG_EX_ResetValue                       0x00000000

/* 0:Disable ROI. 1:Enable ROI. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_ROI                              0:0
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_ROI_End                            0
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_ROI_Start                          0
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_ROI_Type                         U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_ROI_DISABLE                    0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_ROI_ENABLE                     0x1

/* Indicate whether it's compressed by dec. 0: Uncompressed 1: Compressed. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_COMPRESSED                              1:1
#define DCREG_FRAME_BUFFER_CONFIG_EX_COMPRESSED_End                            1
#define DCREG_FRAME_BUFFER_CONFIG_EX_COMPRESSED_Start                          1
#define DCREG_FRAME_BUFFER_CONFIG_EX_COMPRESSED_Type                         U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_COMPRESSED_UNCOMPRESSED               0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_COMPRESSED_COMPRESSED                 0x1

/* 0: Disable 1: Enable 3D-LUT. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_THREED_LUT                       2:2
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_THREED_LUT_End                     2
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_THREED_LUT_Start                   2
#define DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_THREED_LUT_Type                  U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_THREED_LUT_DISABLE             0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_ENABLE_THREED_LUT_ENABLE              0x1

/* 0: Disable 1: Enable MultiSync. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_MULTI_SYNC                              3:3
#define DCREG_FRAME_BUFFER_CONFIG_EX_MULTI_SYNC_End                            3
#define DCREG_FRAME_BUFFER_CONFIG_EX_MULTI_SYNC_Start                          3
#define DCREG_FRAME_BUFFER_CONFIG_EX_MULTI_SYNC_Type                         U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_MULTI_SYNC_DISABLE                    0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_MULTI_SYNC_ENABLE                     0x1

/* 0: Disable 1: Enable Master. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_MASTER                                  4:4
#define DCREG_FRAME_BUFFER_CONFIG_EX_MASTER_End                                4
#define DCREG_FRAME_BUFFER_CONFIG_EX_MASTER_Start                              4
#define DCREG_FRAME_BUFFER_CONFIG_EX_MASTER_Type                             U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_MASTER_DISABLE                        0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_MASTER_ENABLE                         0x1

/* When DeGamma is enabled, the R, G, and B channels will be  routed through  **
** the DeGamma LUT to perform de-gamma correction.  NOTE: This field is       **
** double buffered.                                                           */
#define DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA                                5:5
#define DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA_End                              5
#define DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA_Start                            5
#define DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA_Type                           U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA_DISABLED                     0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_DE_GAMMA_ENABLED               (0x1 << 5)

/* RGB2RGB switch. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_RGB                              6:6
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_RGB_End                            6
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_RGB_Start                          6
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_RGB_Type                         U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_RGB_DISABLED                   0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_RGB_ENABLED                    0x1

/* RGB2YUV switch. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_YUV                              7:7
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_YUV_End                            7
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_YUV_Start                          7
#define DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_YUV_Type                         U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_YUV_DISABLED                   0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_RGB_TO_YUV_ENABLED                    0x1

/* Enable or disable YUV clamp for YUV2RGB. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_YUV_CLAMP                               8:8
#define DCREG_FRAME_BUFFER_CONFIG_EX_YUV_CLAMP_End                             8
#define DCREG_FRAME_BUFFER_CONFIG_EX_YUV_CLAMP_Start                           8
#define DCREG_FRAME_BUFFER_CONFIG_EX_YUV_CLAMP_Type                          U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_YUV_CLAMP_DISABLED                    0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_YUV_CLAMP_ENABLED                     0x1

/* 0: Disable 1: Enable 3D-LUT enlarge. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_ENLARGE                      9:9
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_ENLARGE_End                    9
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_ENLARGE_Start                  9
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_ENLARGE_Type                 U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_ENLARGE_DISABLE            0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_ENLARGE_ENABLE             0x1

/* 0: Disable 1: Enable 3D-LUT before DeGamma. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_BEFORE_DEGAMMA             10:10
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_BEFORE_DEGAMMA_End            10
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_BEFORE_DEGAMMA_Start          10
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_BEFORE_DEGAMMA_Type          U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_BEFORE_DEGAMMA_DISABLE     0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_BEFORE_DEGAMMA_ENABLE      0x1

/* 0: Video 1: Overlay. */
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_USED_BY                    11:11
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_USED_BY_End                   11
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_USED_BY_Start                 11
#define DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_USED_BY_Type                 U01
#define   DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_USED_BY_VIDEO              0x0
#define   DCREG_FRAME_BUFFER_CONFIG_EX_THREED_LUT_USED_BY_OVERLAY            0x1

/* Register dcregReadOT **
** ~~~~~~~~~~~~~~~~~~~~ */
#define dcregReadOTRegAddrs                                               0x0732
#define DCREG_READ_OT_Address                                            0x01CC8
#define DCREG_READ_OT_MSB                                                     15
#define DCREG_READ_OT_LSB                                                      0
#define DCREG_READ_OT_BLK                                                      0
#define DCREG_READ_OT_Count                                                    1
#define DCREG_READ_OT_FieldMask                                       0x0000007F
#define DCREG_READ_OT_ReadMask                                        0x0000007F
#define DCREG_READ_OT_WriteMask                                       0x0000007F
#define DCREG_READ_OT_ResetValue                                      0x00000000

/* Read outstanding request number. */
#define DCREG_READ_OT_VALUE                                                  6:0
#define DCREG_READ_OT_VALUE_End                                                6
#define DCREG_READ_OT_VALUE_Start                                              0
#define DCREG_READ_OT_VALUE_Type                                             U07

/* Register dcregDpConfig (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The configuration register for DP output NOTE: This register is double     **
** buffered.                                                                  */

#define dcregDpConfigRegAddrs                                             0x0734
#define DCREG_DP_CONFIG_Address                                          0x01CD0
#define DCREG_DP_CONFIG_MSB                                                   15
#define DCREG_DP_CONFIG_LSB                                                    1
#define DCREG_DP_CONFIG_BLK                                                    1
#define DCREG_DP_CONFIG_Count                                                  2
#define DCREG_DP_CONFIG_FieldMask                                     0x000000FF
#define DCREG_DP_CONFIG_ReadMask                                      0x000000FF
#define DCREG_DP_CONFIG_WriteMask                                     0x000000FF
#define DCREG_DP_CONFIG_ResetValue                                    0x00000000

/* DP interface data format Refer to DP spec.'Interface color coding' for     **
** detailed                                                                   */
#define DCREG_DP_CONFIG_DP_DATA_FORMAT                                       2:0
#define DCREG_DP_CONFIG_DP_DATA_FORMAT_End                                     2
#define DCREG_DP_CONFIG_DP_DATA_FORMAT_Start                                   0
#define DCREG_DP_CONFIG_DP_DATA_FORMAT_Type                                  U03
#define   DCREG_DP_CONFIG_DP_DATA_FORMAT_RGB565                              0x0
#define   DCREG_DP_CONFIG_DP_DATA_FORMAT_RGB666                              0x1
#define   DCREG_DP_CONFIG_DP_DATA_FORMAT_RGB888                              0x2
#define   DCREG_DP_CONFIG_DP_DATA_FORMAT_RGB101010                           0x3

/* Output bus select */
#define DCREG_DP_CONFIG_BUS_OUTPUT_SEL                                       3:3
#define DCREG_DP_CONFIG_BUS_OUTPUT_SEL_End                                     3
#define DCREG_DP_CONFIG_BUS_OUTPUT_SEL_Start                                   3
#define DCREG_DP_CONFIG_BUS_OUTPUT_SEL_Type                                  U01
#define   DCREG_DP_CONFIG_BUS_OUTPUT_SEL_DPI                                 0x0
#define   DCREG_DP_CONFIG_BUS_OUTPUT_SEL_DP                                  0x1

/* DP interface YUV data format. Refer to the DP spec.'Interface color        **
** coding' for details.                                                       */
#define DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT                                   7:4
#define DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_End                                 7
#define DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_Start                               4
#define DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_Type                              U04
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV420BIT8CFG1                  0x0
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV420BIT8CFG2                  0x1
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV422BIT8CFG1                  0x2
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV422BIT8CFG2                  0x3
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV444BIT8CFG1                  0x4
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV444BIT8CFG2                  0x5
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV420BIT10CFG1                 0x6
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV420BIT10CFG2                 0x7
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV422BIT10CFG1                 0x8
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV422BIT10CFG2                 0x9
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV444BIT10CFG1                 0xA
#define   DCREG_DP_CONFIG_DP_DATA_YUV_FORMAT_YUV444BIT10CFG2                 0xB

/* Register dcregThreedLutIndex (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The lookup table index for 3D-LUT. */

#define dcregThreedLutIndexRegAddrs                                       0x0736
#define DCREG_THREED_LUT_INDEX_Address                                   0x01CD8
#define DCREG_THREED_LUT_INDEX_MSB                                            15
#define DCREG_THREED_LUT_INDEX_LSB                                             1
#define DCREG_THREED_LUT_INDEX_BLK                                             1
#define DCREG_THREED_LUT_INDEX_Count                                           2
#define DCREG_THREED_LUT_INDEX_FieldMask                              0x001F1F1F
#define DCREG_THREED_LUT_INDEX_ReadMask                               0x001F1F1F
#define DCREG_THREED_LUT_INDEX_WriteMask                              0x001F1F1F
#define DCREG_THREED_LUT_INDEX_ResetValue                             0x00000000

#define DCREG_THREED_LUT_INDEX_B                                             4:0
#define DCREG_THREED_LUT_INDEX_B_End                                           4
#define DCREG_THREED_LUT_INDEX_B_Start                                         0
#define DCREG_THREED_LUT_INDEX_B_Type                                        U05

#define DCREG_THREED_LUT_INDEX_G                                            12:8
#define DCREG_THREED_LUT_INDEX_G_End                                          12
#define DCREG_THREED_LUT_INDEX_G_Start                                         8
#define DCREG_THREED_LUT_INDEX_G_Type                                        U05

#define DCREG_THREED_LUT_INDEX_R                                           20:16
#define DCREG_THREED_LUT_INDEX_R_End                                          20
#define DCREG_THREED_LUT_INDEX_R_Start                                        16
#define DCREG_THREED_LUT_INDEX_R_Type                                        U05

/* Register dcregThreedLutData (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The lookup table data for 3D-LUT. */

#define dcregThreedLutDataRegAddrs                                        0x0738
#define DCREG_THREED_LUT_DATA_Address                                    0x01CE0
#define DCREG_THREED_LUT_DATA_MSB                                             15
#define DCREG_THREED_LUT_DATA_LSB                                              1
#define DCREG_THREED_LUT_DATA_BLK                                              1
#define DCREG_THREED_LUT_DATA_Count                                            2
#define DCREG_THREED_LUT_DATA_FieldMask                               0x3FFFFFFF
#define DCREG_THREED_LUT_DATA_ReadMask                                0x3FFFFFFF
#define DCREG_THREED_LUT_DATA_WriteMask                               0x3FFFFFFF
#define DCREG_THREED_LUT_DATA_ResetValue                              0x00000000

#define DCREG_THREED_LUT_DATA_B                                              9:0
#define DCREG_THREED_LUT_DATA_B_End                                            9
#define DCREG_THREED_LUT_DATA_B_Start                                          0
#define DCREG_THREED_LUT_DATA_B_Type                                         U10

#define DCREG_THREED_LUT_DATA_G                                            19:10
#define DCREG_THREED_LUT_DATA_G_End                                           19
#define DCREG_THREED_LUT_DATA_G_Start                                         10
#define DCREG_THREED_LUT_DATA_G_Type                                         U10

#define DCREG_THREED_LUT_DATA_R                                            29:20
#define DCREG_THREED_LUT_DATA_R_End                                           29
#define DCREG_THREED_LUT_DATA_R_Start                                         20
#define DCREG_THREED_LUT_DATA_R_Type                                         U10

/* Register dcregFrameBufferWaterMark (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The water mark for frame buffer. For 32bpp, the pixel water mark max value **
** is 24x1024. For 16bpp, the pixel water mark max value is 48x1024. If the   **
** water mark is setted as 0, then it will not work.                          */

#define dcregFrameBufferWaterMarkRegAddrs                                 0x073A
#define DCREG_FRAME_BUFFER_WATER_MARK_Address                            0x01CE8
#define DCREG_FRAME_BUFFER_WATER_MARK_MSB                                     15
#define DCREG_FRAME_BUFFER_WATER_MARK_LSB                                      1
#define DCREG_FRAME_BUFFER_WATER_MARK_BLK                                      1
#define DCREG_FRAME_BUFFER_WATER_MARK_Count                                    2
#define DCREG_FRAME_BUFFER_WATER_MARK_FieldMask                       0x000FFFFF
#define DCREG_FRAME_BUFFER_WATER_MARK_ReadMask                        0x000FFFFF
#define DCREG_FRAME_BUFFER_WATER_MARK_WriteMask                       0x000FFFFF
#define DCREG_FRAME_BUFFER_WATER_MARK_ResetValue                      0x00000000

#define DCREG_FRAME_BUFFER_WATER_MARK_VALUE                                 19:0
#define DCREG_FRAME_BUFFER_WATER_MARK_VALUE_End                               19
#define DCREG_FRAME_BUFFER_WATER_MARK_VALUE_Start                              0
#define DCREG_FRAME_BUFFER_WATER_MARK_VALUE_Type                             U20

/* Register dcregGammaExIndex (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Gamma Indext Table Register.  Index into gamma table.  See dcregGammaData  **
** for more  information.                                                     */

#define dcregGammaExIndexRegAddrs                                         0x073C
#define DCREG_GAMMA_EX_INDEX_Address                                     0x01CF0
#define DCREG_GAMMA_EX_INDEX_MSB                                              15
#define DCREG_GAMMA_EX_INDEX_LSB                                               1
#define DCREG_GAMMA_EX_INDEX_BLK                                               1
#define DCREG_GAMMA_EX_INDEX_Count                                             2
#define DCREG_GAMMA_EX_INDEX_FieldMask                                0x000001FF
#define DCREG_GAMMA_EX_INDEX_ReadMask                                 0x00000000
#define DCREG_GAMMA_EX_INDEX_WriteMask                                0x000001FF
#define DCREG_GAMMA_EX_INDEX_ResetValue                               0x00000000

/* Index into gamma table.  */
#define DCREG_GAMMA_EX_INDEX_INDEX                                           8:0
#define DCREG_GAMMA_EX_INDEX_INDEX_End                                         8
#define DCREG_GAMMA_EX_INDEX_INDEX_Start                                       0
#define DCREG_GAMMA_EX_INDEX_INDEX_Type                                      U09

/* Register dcregGammaExData (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Gamma Data Translation Register.  Translation values for the gamma table.  **
** When this register  gets written, the data gets stored in the gamma table  **
** at the  index specified by the dcregGammaIndex register.  After the        **
** register is written, the index gets incremented.                           */

#define dcregGammaExDataRegAddrs                                          0x073E
#define DCREG_GAMMA_EX_DATA_Address                                      0x01CF8
#define DCREG_GAMMA_EX_DATA_MSB                                               15
#define DCREG_GAMMA_EX_DATA_LSB                                                1
#define DCREG_GAMMA_EX_DATA_BLK                                                1
#define DCREG_GAMMA_EX_DATA_Count                                              2
#define DCREG_GAMMA_EX_DATA_FieldMask                                 0x0FFF0FFF
#define DCREG_GAMMA_EX_DATA_ReadMask                                  0x00000000
#define DCREG_GAMMA_EX_DATA_WriteMask                                 0x0FFF0FFF
#define DCREG_GAMMA_EX_DATA_ResetValue                                0x00000000

/* Blue translation value.  */
#define DCREG_GAMMA_EX_DATA_BLUE                                            11:0
#define DCREG_GAMMA_EX_DATA_BLUE_End                                          11
#define DCREG_GAMMA_EX_DATA_BLUE_Start                                         0
#define DCREG_GAMMA_EX_DATA_BLUE_Type                                        U12

/* Green translation value.  */
#define DCREG_GAMMA_EX_DATA_GREEN                                          27:16
#define DCREG_GAMMA_EX_DATA_GREEN_End                                         27
#define DCREG_GAMMA_EX_DATA_GREEN_Start                                       16
#define DCREG_GAMMA_EX_DATA_GREEN_Type                                       U12

/* Register dcregOverlayROIOrigin (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregOverlayROIOriginRegAddrs                                     0x0740
#define DCREG_OVERLAY_ROI_ORIGIN_Address                                 0x01D00
#define DCREG_OVERLAY_ROI_ORIGIN_MSB                                          15
#define DCREG_OVERLAY_ROI_ORIGIN_LSB                                           4
#define DCREG_OVERLAY_ROI_ORIGIN_BLK                                           4
#define DCREG_OVERLAY_ROI_ORIGIN_Count                                        16
#define DCREG_OVERLAY_ROI_ORIGIN_FieldMask                            0xFFFFFFFF
#define DCREG_OVERLAY_ROI_ORIGIN_ReadMask                             0xFFFFFFFF
#define DCREG_OVERLAY_ROI_ORIGIN_WriteMask                            0xFFFFFFFF
#define DCREG_OVERLAY_ROI_ORIGIN_ResetValue                           0x00000000

/* OSD/Rectangle start point X coordinate. */
#define DCREG_OVERLAY_ROI_ORIGIN_X                                          15:0
#define DCREG_OVERLAY_ROI_ORIGIN_X_End                                        15
#define DCREG_OVERLAY_ROI_ORIGIN_X_Start                                       0
#define DCREG_OVERLAY_ROI_ORIGIN_X_Type                                      U16

/* OSD/Rectangle start point Y coordinate. */
#define DCREG_OVERLAY_ROI_ORIGIN_Y                                         31:16
#define DCREG_OVERLAY_ROI_ORIGIN_Y_End                                        31
#define DCREG_OVERLAY_ROI_ORIGIN_Y_Start                                      16
#define DCREG_OVERLAY_ROI_ORIGIN_Y_Type                                      U16

/* Register dcregOverlayROISize (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define dcregOverlayROISizeRegAddrs                                       0x0750
#define DCREG_OVERLAY_ROI_SIZE_Address                                   0x01D40
#define DCREG_OVERLAY_ROI_SIZE_MSB                                            15
#define DCREG_OVERLAY_ROI_SIZE_LSB                                             4
#define DCREG_OVERLAY_ROI_SIZE_BLK                                             4
#define DCREG_OVERLAY_ROI_SIZE_Count                                          16
#define DCREG_OVERLAY_ROI_SIZE_FieldMask                              0xFFFFFFFF
#define DCREG_OVERLAY_ROI_SIZE_ReadMask                               0xFFFFFFFF
#define DCREG_OVERLAY_ROI_SIZE_WriteMask                              0xFFFFFFFF
#define DCREG_OVERLAY_ROI_SIZE_ResetValue                             0x00000000

/* OSD/Rectangle width. */
#define DCREG_OVERLAY_ROI_SIZE_WIDTH                                        15:0
#define DCREG_OVERLAY_ROI_SIZE_WIDTH_End                                      15
#define DCREG_OVERLAY_ROI_SIZE_WIDTH_Start                                     0
#define DCREG_OVERLAY_ROI_SIZE_WIDTH_Type                                    U16

/* OSD/Rectangle height. */
#define DCREG_OVERLAY_ROI_SIZE_HEIGHT                                      31:16
#define DCREG_OVERLAY_ROI_SIZE_HEIGHT_End                                     31
#define DCREG_OVERLAY_ROI_SIZE_HEIGHT_Start                                   16
#define DCREG_OVERLAY_ROI_SIZE_HEIGHT_Type                                   U16

/* Register dcregGammaExOneData (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Gamma Data Translation Register.  Translation values for the gamma table.  **
** When this register  gets written, the data gets stored in the gamma table  **
** at the  index specified by the dcregGammaIndex register.  After the        **
** register is written, the index gets incremented.                           */

#define dcregGammaExOneDataRegAddrs                                       0x0760
#define DCREG_GAMMA_EX_ONE_DATA_Address                                  0x01D80
#define DCREG_GAMMA_EX_ONE_DATA_MSB                                           15
#define DCREG_GAMMA_EX_ONE_DATA_LSB                                            1
#define DCREG_GAMMA_EX_ONE_DATA_BLK                                            1
#define DCREG_GAMMA_EX_ONE_DATA_Count                                          2
#define DCREG_GAMMA_EX_ONE_DATA_FieldMask                             0x00000FFF
#define DCREG_GAMMA_EX_ONE_DATA_ReadMask                              0x00000000
#define DCREG_GAMMA_EX_ONE_DATA_WriteMask                             0x00000FFF
#define DCREG_GAMMA_EX_ONE_DATA_ResetValue                            0x00000000

/* Red translation value.  */
#define DCREG_GAMMA_EX_ONE_DATA_RED                                         11:0
#define DCREG_GAMMA_EX_ONE_DATA_RED_End                                       11
#define DCREG_GAMMA_EX_ONE_DATA_RED_Start                                      0
#define DCREG_GAMMA_EX_ONE_DATA_RED_Type                                     U12

/* Register dcregDeGammaIndex (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DeGamma Indext Table Register.  Index into de-gamma table.  See            **
** dcregDeGammaData for more  information.                                    */

#define dcregDeGammaIndexRegAddrs                                         0x0762
#define DCREG_DE_GAMMA_INDEX_Address                                     0x01D88
#define DCREG_DE_GAMMA_INDEX_MSB                                              15
#define DCREG_DE_GAMMA_INDEX_LSB                                               1
#define DCREG_DE_GAMMA_INDEX_BLK                                               1
#define DCREG_DE_GAMMA_INDEX_Count                                             2
#define DCREG_DE_GAMMA_INDEX_FieldMask                                0x000001FF
#define DCREG_DE_GAMMA_INDEX_ReadMask                                 0x00000000
#define DCREG_DE_GAMMA_INDEX_WriteMask                                0x000001FF
#define DCREG_DE_GAMMA_INDEX_ResetValue                               0x00000000

/* Index into gamma table.  */
#define DCREG_DE_GAMMA_INDEX_INDEX                                           8:0
#define DCREG_DE_GAMMA_INDEX_INDEX_End                                         8
#define DCREG_DE_GAMMA_INDEX_INDEX_Start                                       0
#define DCREG_DE_GAMMA_INDEX_INDEX_Type                                      U09

/* Register dcregDeGammaData (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DeGamma Data Translation Register.  Translation values for the de-gamma    **
** table.  When this register  gets written, the data gets stored in the      **
** de-gamma table at the  index specified by the dcregDeGammaIndex register.  **
** After the  register is written, the index gets incremented.                */

#define dcregDeGammaDataRegAddrs                                          0x0764
#define DCREG_DE_GAMMA_DATA_Address                                      0x01D90
#define DCREG_DE_GAMMA_DATA_MSB                                               15
#define DCREG_DE_GAMMA_DATA_LSB                                                1
#define DCREG_DE_GAMMA_DATA_BLK                                                1
#define DCREG_DE_GAMMA_DATA_Count                                              2
#define DCREG_DE_GAMMA_DATA_FieldMask                                 0x3FFFFFFF
#define DCREG_DE_GAMMA_DATA_ReadMask                                  0x00000000
#define DCREG_DE_GAMMA_DATA_WriteMask                                 0x3FFFFFFF
#define DCREG_DE_GAMMA_DATA_ResetValue                                0x00000000

/* Blue translation value.  */
#define DCREG_DE_GAMMA_DATA_BLUE                                            14:0
#define DCREG_DE_GAMMA_DATA_BLUE_End                                          14
#define DCREG_DE_GAMMA_DATA_BLUE_Start                                         0
#define DCREG_DE_GAMMA_DATA_BLUE_Type                                        U15

/* Green translation value.  */
#define DCREG_DE_GAMMA_DATA_GREEN                                          29:15
#define DCREG_DE_GAMMA_DATA_GREEN_End                                         29
#define DCREG_DE_GAMMA_DATA_GREEN_Start                                       15
#define DCREG_DE_GAMMA_DATA_GREEN_Type                                       U15

/* Register dcregDeGammaExData (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* DeGamma Data Translation Register.  Translation values for the de-gamma    **
** table.  When this register  gets written, the data gets stored in the      **
** de-gamma table at the  index specified by the dcregDeGammaIndex register.  **
** After the  register is written, the index gets incremented.                */

#define dcregDeGammaExDataRegAddrs                                        0x0766
#define DCREG_DE_GAMMA_EX_DATA_Address                                   0x01D98
#define DCREG_DE_GAMMA_EX_DATA_MSB                                            15
#define DCREG_DE_GAMMA_EX_DATA_LSB                                             1
#define DCREG_DE_GAMMA_EX_DATA_BLK                                             1
#define DCREG_DE_GAMMA_EX_DATA_Count                                           2
#define DCREG_DE_GAMMA_EX_DATA_FieldMask                              0x00007FFF
#define DCREG_DE_GAMMA_EX_DATA_ReadMask                               0x00000000
#define DCREG_DE_GAMMA_EX_DATA_WriteMask                              0x00007FFF
#define DCREG_DE_GAMMA_EX_DATA_ResetValue                             0x00000000

/* Red translation value.  */
#define DCREG_DE_GAMMA_EX_DATA_RED                                          14:0
#define DCREG_DE_GAMMA_EX_DATA_RED_End                                        14
#define DCREG_DE_GAMMA_EX_DATA_RED_Start                                       0
#define DCREG_DE_GAMMA_EX_DATA_RED_Type                                      U15

/* Register dcregFrameYUVToRGBCoef0 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoef0RegAddrs                                   0x0768
#define DCREG_FRAME_YUV_TO_RGB_COEF0_Address                             0x01DA0
#define DCREG_FRAME_YUV_TO_RGB_COEF0_MSB                                      15
#define DCREG_FRAME_YUV_TO_RGB_COEF0_LSB                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF0_BLK                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF0_Count                                     2
#define DCREG_FRAME_YUV_TO_RGB_COEF0_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF0_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF0_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF0_ResetValue                       0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT0                           15:0
#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT0_End                         15
#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT0_Start                        0
#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT0_Type                       U16

#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT1                          31:16
#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT1_End                         31
#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT1_Start                       16
#define DCREG_FRAME_YUV_TO_RGB_COEF0_COEFFICIENT1_Type                       U16

/* Register dcregFrameYUVToRGBCoef1 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoef1RegAddrs                                   0x076A
#define DCREG_FRAME_YUV_TO_RGB_COEF1_Address                             0x01DA8
#define DCREG_FRAME_YUV_TO_RGB_COEF1_MSB                                      15
#define DCREG_FRAME_YUV_TO_RGB_COEF1_LSB                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF1_BLK                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF1_Count                                     2
#define DCREG_FRAME_YUV_TO_RGB_COEF1_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF1_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF1_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF1_ResetValue                       0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT2                           15:0
#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT2_End                         15
#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT2_Start                        0
#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT2_Type                       U16

#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT3                          31:16
#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT3_End                         31
#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT3_Start                       16
#define DCREG_FRAME_YUV_TO_RGB_COEF1_COEFFICIENT3_Type                       U16

/* Register dcregFrameYUVToRGBCoef2 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoef2RegAddrs                                   0x076C
#define DCREG_FRAME_YUV_TO_RGB_COEF2_Address                             0x01DB0
#define DCREG_FRAME_YUV_TO_RGB_COEF2_MSB                                      15
#define DCREG_FRAME_YUV_TO_RGB_COEF2_LSB                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF2_BLK                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF2_Count                                     2
#define DCREG_FRAME_YUV_TO_RGB_COEF2_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF2_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF2_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF2_ResetValue                       0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT4                           15:0
#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT4_End                         15
#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT4_Start                        0
#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT4_Type                       U16

#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT5                          31:16
#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT5_End                         31
#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT5_Start                       16
#define DCREG_FRAME_YUV_TO_RGB_COEF2_COEFFICIENT5_Type                       U16

/* Register dcregFrameYUVToRGBCoef3 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoef3RegAddrs                                   0x076E
#define DCREG_FRAME_YUV_TO_RGB_COEF3_Address                             0x01DB8
#define DCREG_FRAME_YUV_TO_RGB_COEF3_MSB                                      15
#define DCREG_FRAME_YUV_TO_RGB_COEF3_LSB                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF3_BLK                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF3_Count                                     2
#define DCREG_FRAME_YUV_TO_RGB_COEF3_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF3_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF3_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF3_ResetValue                       0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT6                           15:0
#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT6_End                         15
#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT6_Start                        0
#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT6_Type                       U16

#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT7                          31:16
#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT7_End                         31
#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT7_Start                       16
#define DCREG_FRAME_YUV_TO_RGB_COEF3_COEFFICIENT7_Type                       U16

/* Register dcregOverlayWaterMark (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The water mark for overlay. */

#define dcregOverlayWaterMarkRegAddrs                                     0x0770
#define DCREG_OVERLAY_WATER_MARK_Address                                 0x01DC0
#define DCREG_OVERLAY_WATER_MARK_MSB                                          15
#define DCREG_OVERLAY_WATER_MARK_LSB                                           4
#define DCREG_OVERLAY_WATER_MARK_BLK                                           4
#define DCREG_OVERLAY_WATER_MARK_Count                                        16
#define DCREG_OVERLAY_WATER_MARK_FieldMask                            0x000FFFFF
#define DCREG_OVERLAY_WATER_MARK_ReadMask                             0x000FFFFF
#define DCREG_OVERLAY_WATER_MARK_WriteMask                            0x000FFFFF
#define DCREG_OVERLAY_WATER_MARK_ResetValue                           0x00000000

#define DCREG_OVERLAY_WATER_MARK_VALUE                                      19:0
#define DCREG_OVERLAY_WATER_MARK_VALUE_End                                    19
#define DCREG_OVERLAY_WATER_MARK_VALUE_Start                                   0
#define DCREG_OVERLAY_WATER_MARK_VALUE_Type                                  U20

/* Register dcregFrameYUVToRGBCoef4 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoef4RegAddrs                                   0x0780
#define DCREG_FRAME_YUV_TO_RGB_COEF4_Address                             0x01E00
#define DCREG_FRAME_YUV_TO_RGB_COEF4_MSB                                      15
#define DCREG_FRAME_YUV_TO_RGB_COEF4_LSB                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF4_BLK                                       1
#define DCREG_FRAME_YUV_TO_RGB_COEF4_Count                                     2
#define DCREG_FRAME_YUV_TO_RGB_COEF4_FieldMask                        0x0000FFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF4_ReadMask                         0x0000FFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF4_WriteMask                        0x0000FFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF4_ResetValue                       0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF4_COEFFICIENT8                           15:0
#define DCREG_FRAME_YUV_TO_RGB_COEF4_COEFFICIENT8_End                         15
#define DCREG_FRAME_YUV_TO_RGB_COEF4_COEFFICIENT8_Start                        0
#define DCREG_FRAME_YUV_TO_RGB_COEF4_COEFFICIENT8_Type                       U16

/* Register dcregFrameYUVToRGBCoefD0 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoefD0RegAddrs                                  0x0782
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_Address                           0x01E08
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_MSB                                    15
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_LSB                                     1
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_BLK                                     1
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_Count                                   2
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_FieldMask                      0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_ReadMask                       0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_WriteMask                      0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_ResetValue                     0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_COEFFICIENT                          25:0
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_COEFFICIENT_End                        25
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_COEFFICIENT_Start                       0
#define DCREG_FRAME_YUV_TO_RGB_COEF_D0_COEFFICIENT_Type                      U26

/* Register dcregFrameYUVToRGBCoefD1 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoefD1RegAddrs                                  0x0784
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_Address                           0x01E10
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_MSB                                    15
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_LSB                                     1
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_BLK                                     1
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_Count                                   2
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_FieldMask                      0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_ReadMask                       0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_WriteMask                      0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_ResetValue                     0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_COEFFICIENT                          25:0
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_COEFFICIENT_End                        25
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_COEFFICIENT_Start                       0
#define DCREG_FRAME_YUV_TO_RGB_COEF_D1_COEFFICIENT_Type                      U26

/* Register dcregFrameYUVToRGBCoefD2 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregFrameYUVToRGBCoefD2RegAddrs                                  0x0786
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_Address                           0x01E18
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_MSB                                    15
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_LSB                                     1
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_BLK                                     1
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_Count                                   2
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_FieldMask                      0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_ReadMask                       0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_WriteMask                      0x03FFFFFF
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_ResetValue                     0x00000000

#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_COEFFICIENT                          25:0
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_COEFFICIENT_End                        25
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_COEFFICIENT_Start                       0
#define DCREG_FRAME_YUV_TO_RGB_COEF_D2_COEFFICIENT_Type                      U26

/* Register dcregFrameRGBToRGBCoef0 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregFrameRGBToRGBCoef0RegAddrs                                   0x0788
#define DCREG_FRAME_RGB_TO_RGB_COEF0_Address                             0x01E20
#define DCREG_FRAME_RGB_TO_RGB_COEF0_MSB                                      15
#define DCREG_FRAME_RGB_TO_RGB_COEF0_LSB                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF0_BLK                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF0_Count                                     2
#define DCREG_FRAME_RGB_TO_RGB_COEF0_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF0_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF0_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF0_ResetValue                       0x00000000

#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT0                           15:0
#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT0_End                         15
#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT0_Start                        0
#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT0_Type                       U16

#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT1                          31:16
#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT1_End                         31
#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT1_Start                       16
#define DCREG_FRAME_RGB_TO_RGB_COEF0_COEFFICIENT1_Type                       U16

/* Register dcregFrameRGBToRGBCoef1 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregFrameRGBToRGBCoef1RegAddrs                                   0x078A
#define DCREG_FRAME_RGB_TO_RGB_COEF1_Address                             0x01E28
#define DCREG_FRAME_RGB_TO_RGB_COEF1_MSB                                      15
#define DCREG_FRAME_RGB_TO_RGB_COEF1_LSB                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF1_BLK                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF1_Count                                     2
#define DCREG_FRAME_RGB_TO_RGB_COEF1_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF1_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF1_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF1_ResetValue                       0x00000000

#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT2                           15:0
#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT2_End                         15
#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT2_Start                        0
#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT2_Type                       U16

#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT3                          31:16
#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT3_End                         31
#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT3_Start                       16
#define DCREG_FRAME_RGB_TO_RGB_COEF1_COEFFICIENT3_Type                       U16

/* Register dcregFrameRGBToRGBCoef2 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregFrameRGBToRGBCoef2RegAddrs                                   0x078C
#define DCREG_FRAME_RGB_TO_RGB_COEF2_Address                             0x01E30
#define DCREG_FRAME_RGB_TO_RGB_COEF2_MSB                                      15
#define DCREG_FRAME_RGB_TO_RGB_COEF2_LSB                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF2_BLK                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF2_Count                                     2
#define DCREG_FRAME_RGB_TO_RGB_COEF2_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF2_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF2_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF2_ResetValue                       0x00000000

#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT4                           15:0
#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT4_End                         15
#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT4_Start                        0
#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT4_Type                       U16

#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT5                          31:16
#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT5_End                         31
#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT5_Start                       16
#define DCREG_FRAME_RGB_TO_RGB_COEF2_COEFFICIENT5_Type                       U16

/* Register dcregFrameRGBToRGBCoef3 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregFrameRGBToRGBCoef3RegAddrs                                   0x078E
#define DCREG_FRAME_RGB_TO_RGB_COEF3_Address                             0x01E38
#define DCREG_FRAME_RGB_TO_RGB_COEF3_MSB                                      15
#define DCREG_FRAME_RGB_TO_RGB_COEF3_LSB                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF3_BLK                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF3_Count                                     2
#define DCREG_FRAME_RGB_TO_RGB_COEF3_FieldMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF3_ReadMask                         0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF3_WriteMask                        0xFFFFFFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF3_ResetValue                       0x00000000

#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT6                           15:0
#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT6_End                         15
#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT6_Start                        0
#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT6_Type                       U16

#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT7                          31:16
#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT7_End                         31
#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT7_Start                       16
#define DCREG_FRAME_RGB_TO_RGB_COEF3_COEFFICIENT7_Type                       U16

/* Register dcregFrameRGBToRGBCoef4 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregFrameRGBToRGBCoef4RegAddrs                                   0x0790
#define DCREG_FRAME_RGB_TO_RGB_COEF4_Address                             0x01E40
#define DCREG_FRAME_RGB_TO_RGB_COEF4_MSB                                      15
#define DCREG_FRAME_RGB_TO_RGB_COEF4_LSB                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF4_BLK                                       1
#define DCREG_FRAME_RGB_TO_RGB_COEF4_Count                                     2
#define DCREG_FRAME_RGB_TO_RGB_COEF4_FieldMask                        0x0000FFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF4_ReadMask                         0x0000FFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF4_WriteMask                        0x0000FFFF
#define DCREG_FRAME_RGB_TO_RGB_COEF4_ResetValue                       0x00000000

#define DCREG_FRAME_RGB_TO_RGB_COEF4_COEFFICIENT8                           15:0
#define DCREG_FRAME_RGB_TO_RGB_COEF4_COEFFICIENT8_End                         15
#define DCREG_FRAME_RGB_TO_RGB_COEF4_COEFFICIENT8_Start                        0
#define DCREG_FRAME_RGB_TO_RGB_COEF4_COEFFICIENT8_Type                       U16

/* Register dcregRGBToYUVCoef0 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoef0RegAddrs                                        0x0792
#define DCREG_RGB_TO_YUV_COEF0_Address                                   0x01E48
#define DCREG_RGB_TO_YUV_COEF0_MSB                                            15
#define DCREG_RGB_TO_YUV_COEF0_LSB                                             1
#define DCREG_RGB_TO_YUV_COEF0_BLK                                             1
#define DCREG_RGB_TO_YUV_COEF0_Count                                           2
#define DCREG_RGB_TO_YUV_COEF0_FieldMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF0_ReadMask                               0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF0_WriteMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF0_ResetValue                             0x00000000

#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT0                                 15:0
#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT0_End                               15
#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT0_Start                              0
#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT0_Type                             U16

#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT1                                31:16
#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT1_End                               31
#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT1_Start                             16
#define DCREG_RGB_TO_YUV_COEF0_COEFFICIENT1_Type                             U16

/* Register dcregRGBToYUVCoef1 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoef1RegAddrs                                        0x0794
#define DCREG_RGB_TO_YUV_COEF1_Address                                   0x01E50
#define DCREG_RGB_TO_YUV_COEF1_MSB                                            15
#define DCREG_RGB_TO_YUV_COEF1_LSB                                             1
#define DCREG_RGB_TO_YUV_COEF1_BLK                                             1
#define DCREG_RGB_TO_YUV_COEF1_Count                                           2
#define DCREG_RGB_TO_YUV_COEF1_FieldMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF1_ReadMask                               0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF1_WriteMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF1_ResetValue                             0x00000000

#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT2                                 15:0
#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT2_End                               15
#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT2_Start                              0
#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT2_Type                             U16

#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT3                                31:16
#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT3_End                               31
#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT3_Start                             16
#define DCREG_RGB_TO_YUV_COEF1_COEFFICIENT3_Type                             U16

/* Register dcregRGBToYUVCoef2 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoef2RegAddrs                                        0x0796
#define DCREG_RGB_TO_YUV_COEF2_Address                                   0x01E58
#define DCREG_RGB_TO_YUV_COEF2_MSB                                            15
#define DCREG_RGB_TO_YUV_COEF2_LSB                                             1
#define DCREG_RGB_TO_YUV_COEF2_BLK                                             1
#define DCREG_RGB_TO_YUV_COEF2_Count                                           2
#define DCREG_RGB_TO_YUV_COEF2_FieldMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF2_ReadMask                               0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF2_WriteMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF2_ResetValue                             0x00000000

#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT4                                 15:0
#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT4_End                               15
#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT4_Start                              0
#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT4_Type                             U16

#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT5                                31:16
#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT5_End                               31
#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT5_Start                             16
#define DCREG_RGB_TO_YUV_COEF2_COEFFICIENT5_Type                             U16

/* Register dcregRGBToYUVCoef3 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoef3RegAddrs                                        0x0798
#define DCREG_RGB_TO_YUV_COEF3_Address                                   0x01E60
#define DCREG_RGB_TO_YUV_COEF3_MSB                                            15
#define DCREG_RGB_TO_YUV_COEF3_LSB                                             1
#define DCREG_RGB_TO_YUV_COEF3_BLK                                             1
#define DCREG_RGB_TO_YUV_COEF3_Count                                           2
#define DCREG_RGB_TO_YUV_COEF3_FieldMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF3_ReadMask                               0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF3_WriteMask                              0xFFFFFFFF
#define DCREG_RGB_TO_YUV_COEF3_ResetValue                             0x00000000

#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT6                                 15:0
#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT6_End                               15
#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT6_Start                              0
#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT6_Type                             U16

#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT7                                31:16
#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT7_End                               31
#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT7_Start                             16
#define DCREG_RGB_TO_YUV_COEF3_COEFFICIENT7_Type                             U16

/* Register dcregRGBToYUVCoef4 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoef4RegAddrs                                        0x079A
#define DCREG_RGB_TO_YUV_COEF4_Address                                   0x01E68
#define DCREG_RGB_TO_YUV_COEF4_MSB                                            15
#define DCREG_RGB_TO_YUV_COEF4_LSB                                             1
#define DCREG_RGB_TO_YUV_COEF4_BLK                                             1
#define DCREG_RGB_TO_YUV_COEF4_Count                                           2
#define DCREG_RGB_TO_YUV_COEF4_FieldMask                              0x0000FFFF
#define DCREG_RGB_TO_YUV_COEF4_ReadMask                               0x0000FFFF
#define DCREG_RGB_TO_YUV_COEF4_WriteMask                              0x0000FFFF
#define DCREG_RGB_TO_YUV_COEF4_ResetValue                             0x00000000

#define DCREG_RGB_TO_YUV_COEF4_COEFFICIENT8                                 15:0
#define DCREG_RGB_TO_YUV_COEF4_COEFFICIENT8_End                               15
#define DCREG_RGB_TO_YUV_COEF4_COEFFICIENT8_Start                              0
#define DCREG_RGB_TO_YUV_COEF4_COEFFICIENT8_Type                             U16

/* Register dcregRGBToYUVCoefD0 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoefD0RegAddrs                                       0x079C
#define DCREG_RGB_TO_YUV_COEF_D0_Address                                 0x01E70
#define DCREG_RGB_TO_YUV_COEF_D0_MSB                                          15
#define DCREG_RGB_TO_YUV_COEF_D0_LSB                                           1
#define DCREG_RGB_TO_YUV_COEF_D0_BLK                                           1
#define DCREG_RGB_TO_YUV_COEF_D0_Count                                         2
#define DCREG_RGB_TO_YUV_COEF_D0_FieldMask                            0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D0_ReadMask                             0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D0_WriteMask                            0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D0_ResetValue                           0x00000000

#define DCREG_RGB_TO_YUV_COEF_D0_COEFFICIENT                                 9:0
#define DCREG_RGB_TO_YUV_COEF_D0_COEFFICIENT_End                               9
#define DCREG_RGB_TO_YUV_COEF_D0_COEFFICIENT_Start                             0
#define DCREG_RGB_TO_YUV_COEF_D0_COEFFICIENT_Type                            U10

/* Register dcregRGBToYUVCoefD1 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoefD1RegAddrs                                       0x079E
#define DCREG_RGB_TO_YUV_COEF_D1_Address                                 0x01E78
#define DCREG_RGB_TO_YUV_COEF_D1_MSB                                          15
#define DCREG_RGB_TO_YUV_COEF_D1_LSB                                           1
#define DCREG_RGB_TO_YUV_COEF_D1_BLK                                           1
#define DCREG_RGB_TO_YUV_COEF_D1_Count                                         2
#define DCREG_RGB_TO_YUV_COEF_D1_FieldMask                            0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D1_ReadMask                             0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D1_WriteMask                            0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D1_ResetValue                           0x00000000

#define DCREG_RGB_TO_YUV_COEF_D1_COEFFICIENT                                 9:0
#define DCREG_RGB_TO_YUV_COEF_D1_COEFFICIENT_End                               9
#define DCREG_RGB_TO_YUV_COEF_D1_COEFFICIENT_Start                             0
#define DCREG_RGB_TO_YUV_COEF_D1_COEFFICIENT_Type                            U10

/* Register dcregRGBToYUVCoefD2 (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB2YUV Coef */

#define dcregRGBToYUVCoefD2RegAddrs                                       0x07A0
#define DCREG_RGB_TO_YUV_COEF_D2_Address                                 0x01E80
#define DCREG_RGB_TO_YUV_COEF_D2_MSB                                          15
#define DCREG_RGB_TO_YUV_COEF_D2_LSB                                           1
#define DCREG_RGB_TO_YUV_COEF_D2_BLK                                           1
#define DCREG_RGB_TO_YUV_COEF_D2_Count                                         2
#define DCREG_RGB_TO_YUV_COEF_D2_FieldMask                            0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D2_ReadMask                             0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D2_WriteMask                            0x000003FF
#define DCREG_RGB_TO_YUV_COEF_D2_ResetValue                           0x00000000

#define DCREG_RGB_TO_YUV_COEF_D2_COEFFICIENT                                 9:0
#define DCREG_RGB_TO_YUV_COEF_D2_COEFFICIENT_End                               9
#define DCREG_RGB_TO_YUV_COEF_D2_COEFFICIENT_Start                             0
#define DCREG_RGB_TO_YUV_COEF_D2_COEFFICIENT_Type                            U10

/* Register dcregFrameYClampBound (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined Y clamp value for YUV2RGB. */

#define dcregFrameYClampBoundRegAddrs                                     0x07A2
#define DCREG_FRAME_YCLAMP_BOUND_Address                                 0x01E88
#define DCREG_FRAME_YCLAMP_BOUND_MSB                                          15
#define DCREG_FRAME_YCLAMP_BOUND_LSB                                           1
#define DCREG_FRAME_YCLAMP_BOUND_BLK                                           1
#define DCREG_FRAME_YCLAMP_BOUND_Count                                         2
#define DCREG_FRAME_YCLAMP_BOUND_FieldMask                            0x03FF03FF
#define DCREG_FRAME_YCLAMP_BOUND_ReadMask                             0x03FF03FF
#define DCREG_FRAME_YCLAMP_BOUND_WriteMask                            0x03FF03FF
#define DCREG_FRAME_YCLAMP_BOUND_ResetValue                           0x00000000

/* The min value. */
#define DCREG_FRAME_YCLAMP_BOUND_LVALUE                                      9:0
#define DCREG_FRAME_YCLAMP_BOUND_LVALUE_End                                    9
#define DCREG_FRAME_YCLAMP_BOUND_LVALUE_Start                                  0
#define DCREG_FRAME_YCLAMP_BOUND_LVALUE_Type                                 U10

/* The max value. */
#define DCREG_FRAME_YCLAMP_BOUND_HVALUE                                    25:16
#define DCREG_FRAME_YCLAMP_BOUND_HVALUE_End                                   25
#define DCREG_FRAME_YCLAMP_BOUND_HVALUE_Start                                 16
#define DCREG_FRAME_YCLAMP_BOUND_HVALUE_Type                                 U10

/* Register dcregFrameUVClampBound (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined UV clamp value for YUV2RGB. */

#define dcregFrameUVClampBoundRegAddrs                                    0x07A4
#define DCREG_FRAME_UV_CLAMP_BOUND_Address                               0x01E90
#define DCREG_FRAME_UV_CLAMP_BOUND_MSB                                        15
#define DCREG_FRAME_UV_CLAMP_BOUND_LSB                                         1
#define DCREG_FRAME_UV_CLAMP_BOUND_BLK                                         1
#define DCREG_FRAME_UV_CLAMP_BOUND_Count                                       2
#define DCREG_FRAME_UV_CLAMP_BOUND_FieldMask                          0x03FF03FF
#define DCREG_FRAME_UV_CLAMP_BOUND_ReadMask                           0x03FF03FF
#define DCREG_FRAME_UV_CLAMP_BOUND_WriteMask                          0x03FF03FF
#define DCREG_FRAME_UV_CLAMP_BOUND_ResetValue                         0x00000000

/* The min value. */
#define DCREG_FRAME_UV_CLAMP_BOUND_LVALUE                                    9:0
#define DCREG_FRAME_UV_CLAMP_BOUND_LVALUE_End                                  9
#define DCREG_FRAME_UV_CLAMP_BOUND_LVALUE_Start                                0
#define DCREG_FRAME_UV_CLAMP_BOUND_LVALUE_Type                               U10

/* The max value. */
#define DCREG_FRAME_UV_CLAMP_BOUND_HVALUE                                  25:16
#define DCREG_FRAME_UV_CLAMP_BOUND_HVALUE_End                                 25
#define DCREG_FRAME_UV_CLAMP_BOUND_HVALUE_Start                               16
#define DCREG_FRAME_UV_CLAMP_BOUND_HVALUE_Type                               U10

/* Register gcregMMUPrefetch (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Enable or disable MMU prefetch. Used in MMU Slave TLB prefetch. */

#define gcregMMUPrefetchRegAddrs                                          0x07A6
#define GCREG_MMU_PREFETCH_Address                                       0x01E98
#define GCREG_MMU_PREFETCH_MSB                                                15
#define GCREG_MMU_PREFETCH_LSB                                                 1
#define GCREG_MMU_PREFETCH_BLK                                                 1
#define GCREG_MMU_PREFETCH_Count                                               2
#define GCREG_MMU_PREFETCH_FieldMask                                  0x00000003
#define GCREG_MMU_PREFETCH_ReadMask                                   0x00000003
#define GCREG_MMU_PREFETCH_WriteMask                                  0x00000003
#define GCREG_MMU_PREFETCH_ResetValue                                 0x00000000

/* Prefetch config. */
#define GCREG_MMU_PREFETCH_CONFIG                                            0:0
#define GCREG_MMU_PREFETCH_CONFIG_End                                          0
#define GCREG_MMU_PREFETCH_CONFIG_Start                                        0
#define GCREG_MMU_PREFETCH_CONFIG_Type                                       U01
#define   GCREG_MMU_PREFETCH_CONFIG_DISABLE                                  0x0
#define   GCREG_MMU_PREFETCH_CONFIG_ENABLE                                   0x1

/* Setted up from SW or HW. */
#define GCREG_MMU_PREFETCH_SET_UP                                            1:1
#define GCREG_MMU_PREFETCH_SET_UP_End                                          1
#define GCREG_MMU_PREFETCH_SET_UP_Start                                        1
#define GCREG_MMU_PREFETCH_SET_UP_Type                                       U01
#define   GCREG_MMU_PREFETCH_SET_UP_FROM_HW                                  0x0
#define   GCREG_MMU_PREFETCH_SET_UP_FROM_SW                                  0x1

/* Register gcregFrameYOrArgbSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregFrameYOrArgbSTLBStartAddrRegAddrs                            0x07A8
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_Address                     0x01EA0
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_MSB                              15
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_LSB                               1
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_BLK                               1
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_Count                             2
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_FieldMask                0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_ReadMask                 0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_WriteMask                0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_ResetValue               0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_VALUE                          31:0
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_VALUE_End                        31
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_VALUE_Start                       0
#define GCREG_FRAME_YOR_ARGB_STLB_START_ADDR_VALUE_Type                      U32

/* Register gcregFrameYOrArgbSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregFrameYOrArgbSTLBEndAddrRegAddrs                              0x07AA
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_Address                       0x01EA8
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_MSB                                15
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_LSB                                 1
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_BLK                                 1
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_Count                               2
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_FieldMask                  0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_ReadMask                   0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_WriteMask                  0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_ResetValue                 0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_VALUE                            31:0
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_VALUE_End                          31
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_VALUE_Start                         0
#define GCREG_FRAME_YOR_ARGB_STLB_END_ADDR_VALUE_Type                        U32

/* Register gcregFrameYOrArgbSTLBStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The slave cache line size. Used in MMU Slave TLB prefetch. */

#define gcregFrameYOrArgbSTLBStrideRegAddrs                               0x07AC
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_Address                         0x01EB0
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_MSB                                  15
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_LSB                                   1
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_BLK                                   1
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_Count                                 2
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_FieldMask                    0x0000FFFF
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_ReadMask                     0x0000FFFF
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_WriteMask                    0x0000FFFF
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_ResetValue                   0x00000000

/* The slave cache line size in horizontal direction. */
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_VALUE                              15:0
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_VALUE_End                            15
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_VALUE_Start                           0
#define GCREG_FRAME_YOR_ARGB_STLB_STRIDE_VALUE_Type                          U16

/* Register gcregFrameYOrArgbSTLBMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in MMU Slave TLB prefetch. */

#define gcregFrameYOrArgbSTLBMaxNumRegAddrs                               0x07AE
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_Address                        0x01EB8
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_MSB                                 15
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_LSB                                  1
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_BLK                                  1
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_Count                                2
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_FieldMask                   0x00FF00FF
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_ReadMask                    0x00FF00FF
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_WriteMask                   0x00FF00FF
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_ResetValue                  0x00000000

/* Slave TLB horizontal max request number. */
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL                         7:0
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL_End                       7
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL_Start                     0
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL_Type                    U08

/* Slave TLB vertical max request number. */
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_VERTICAL                         23:16
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_VERTICAL_End                        23
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_VERTICAL_Start                      16
#define GCREG_FRAME_YOR_ARGB_STLB_MAX_NUM_VERTICAL_Type                      U08

/* Register dcregOverlayYUVToRGBCoef0 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoef0RegAddrs                                 0x07B0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_Address                           0x01EC0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_MSB                                    15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_LSB                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_BLK                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_Count                                  16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_ResetValue                     0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT0                         15:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT0_End                       15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT0_Start                      0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT0_Type                     U16

#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT1                        31:16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT1_End                       31
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT1_Start                     16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF0_COEFFICIENT1_Type                     U16

/* Register dcregOverlayYUVToRGBCoef1 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoef1RegAddrs                                 0x07C0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_Address                           0x01F00
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_MSB                                    15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_LSB                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_BLK                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_Count                                  16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_ResetValue                     0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT2                         15:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT2_End                       15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT2_Start                      0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT2_Type                     U16

#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT3                        31:16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT3_End                       31
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT3_Start                     16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF1_COEFFICIENT3_Type                     U16

/* Register dcregOverlayYUVToRGBCoef2 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoef2RegAddrs                                 0x07D0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_Address                           0x01F40
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_MSB                                    15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_LSB                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_BLK                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_Count                                  16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_ResetValue                     0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT4                         15:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT4_End                       15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT4_Start                      0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT4_Type                     U16

#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT5                        31:16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT5_End                       31
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT5_Start                     16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF2_COEFFICIENT5_Type                     U16

/* Register dcregOverlayYUVToRGBCoef3 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoef3RegAddrs                                 0x07E0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_Address                           0x01F80
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_MSB                                    15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_LSB                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_BLK                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_Count                                  16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_ResetValue                     0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT6                         15:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT6_End                       15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT6_Start                      0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT6_Type                     U16

#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT7                        31:16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT7_End                       31
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT7_Start                     16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF3_COEFFICIENT7_Type                     U16

/* Register dcregOverlayYUVToRGBCoef4 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoef4RegAddrs                                 0x07F0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_Address                           0x01FC0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_MSB                                    15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_LSB                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_BLK                                     4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_Count                                  16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_FieldMask                      0x0000FFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_ReadMask                       0x0000FFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_WriteMask                      0x0000FFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_ResetValue                     0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_COEFFICIENT8                         15:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_COEFFICIENT8_End                       15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_COEFFICIENT8_Start                      0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF4_COEFFICIENT8_Type                     U16

/* Register dcregOverlayYUVToRGBCoefD0 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoefD0RegAddrs                                0x0800
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_Address                         0x02000
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_MSB                                  15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_LSB                                   4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_BLK                                   4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_Count                                16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_FieldMask                    0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_ReadMask                     0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_WriteMask                    0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_ResetValue                   0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_COEFFICIENT                        25:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_COEFFICIENT_End                      25
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_COEFFICIENT_Start                     0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D0_COEFFICIENT_Type                    U26

/* Register dcregOverlayYUVToRGBCoefD1 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoefD1RegAddrs                                0x0810
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_Address                         0x02040
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_MSB                                  15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_LSB                                   4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_BLK                                   4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_Count                                16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_FieldMask                    0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_ReadMask                     0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_WriteMask                    0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_ResetValue                   0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_COEFFICIENT                        25:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_COEFFICIENT_End                      25
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_COEFFICIENT_Start                     0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D1_COEFFICIENT_Type                    U26

/* Register dcregOverlayYUVToRGBCoefD2 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined YUV2RGB Coef */

#define dcregOverlayYUVToRGBCoefD2RegAddrs                                0x0820
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_Address                         0x02080
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_MSB                                  15
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_LSB                                   4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_BLK                                   4
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_Count                                16
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_FieldMask                    0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_ReadMask                     0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_WriteMask                    0x03FFFFFF
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_ResetValue                   0x00000000

#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_COEFFICIENT                        25:0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_COEFFICIENT_End                      25
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_COEFFICIENT_Start                     0
#define DCREG_OVERLAY_YUV_TO_RGB_COEF_D2_COEFFICIENT_Type                    U26

/* Register dcregOverlayRGBToRGBCoef0 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregOverlayRGBToRGBCoef0RegAddrs                                 0x0830
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_Address                           0x020C0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_MSB                                    15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_LSB                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_BLK                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_Count                                  16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_ResetValue                     0x00000000

#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT0                         15:0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT0_End                       15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT0_Start                      0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT0_Type                     U16

#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT1                        31:16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT1_End                       31
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT1_Start                     16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF0_COEFFICIENT1_Type                     U16

/* Register dcregOverlayRGBToRGBCoef1 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregOverlayRGBToRGBCoef1RegAddrs                                 0x0840
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_Address                           0x02100
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_MSB                                    15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_LSB                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_BLK                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_Count                                  16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_ResetValue                     0x00000000

#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT2                         15:0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT2_End                       15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT2_Start                      0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT2_Type                     U16

#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT3                        31:16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT3_End                       31
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT3_Start                     16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF1_COEFFICIENT3_Type                     U16

/* Register dcregOverlayRGBToRGBCoef2 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregOverlayRGBToRGBCoef2RegAddrs                                 0x0850
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_Address                           0x02140
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_MSB                                    15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_LSB                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_BLK                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_Count                                  16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_ResetValue                     0x00000000

#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT4                         15:0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT4_End                       15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT4_Start                      0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT4_Type                     U16

#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT5                        31:16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT5_End                       31
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT5_Start                     16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF2_COEFFICIENT5_Type                     U16

/* Register dcregOverlayRGBToRGBCoef3 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregOverlayRGBToRGBCoef3RegAddrs                                 0x0860
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_Address                           0x02180
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_MSB                                    15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_LSB                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_BLK                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_Count                                  16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_FieldMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_ReadMask                       0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_WriteMask                      0xFFFFFFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_ResetValue                     0x00000000

#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT6                         15:0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT6_End                       15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT6_Start                      0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT6_Type                     U16

#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT7                        31:16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT7_End                       31
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT7_Start                     16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF3_COEFFICIENT7_Type                     U16

/* Register dcregOverlayRGBToRGBCoef4 (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined RGB709ToRGB2020 or RGB2020ToRGB709 Coef. */

#define dcregOverlayRGBToRGBCoef4RegAddrs                                 0x0870
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_Address                           0x021C0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_MSB                                    15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_LSB                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_BLK                                     4
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_Count                                  16
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_FieldMask                      0x0000FFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_ReadMask                       0x0000FFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_WriteMask                      0x0000FFFF
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_ResetValue                     0x00000000

#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_COEFFICIENT8                         15:0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_COEFFICIENT8_End                       15
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_COEFFICIENT8_Start                      0
#define DCREG_OVERLAY_RGB_TO_RGB_COEF4_COEFFICIENT8_Type                     U16

/* Register dcregOverlayDeGammaIndex (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Overlay DeGamma Indext Table Register.  Index into de-gamma table.  See    **
** dcregDeGammaData for more  information.                                    */

#define dcregOverlayDeGammaIndexRegAddrs                                  0x0880
#define DCREG_OVERLAY_DE_GAMMA_INDEX_Address                             0x02200
#define DCREG_OVERLAY_DE_GAMMA_INDEX_MSB                                      15
#define DCREG_OVERLAY_DE_GAMMA_INDEX_LSB                                       4
#define DCREG_OVERLAY_DE_GAMMA_INDEX_BLK                                       4
#define DCREG_OVERLAY_DE_GAMMA_INDEX_Count                                    16
#define DCREG_OVERLAY_DE_GAMMA_INDEX_FieldMask                        0x000001FF
#define DCREG_OVERLAY_DE_GAMMA_INDEX_ReadMask                         0x00000000
#define DCREG_OVERLAY_DE_GAMMA_INDEX_WriteMask                        0x000001FF
#define DCREG_OVERLAY_DE_GAMMA_INDEX_ResetValue                       0x00000000

/* Index into gamma table.  */
#define DCREG_OVERLAY_DE_GAMMA_INDEX_INDEX                                   8:0
#define DCREG_OVERLAY_DE_GAMMA_INDEX_INDEX_End                                 8
#define DCREG_OVERLAY_DE_GAMMA_INDEX_INDEX_Start                               0
#define DCREG_OVERLAY_DE_GAMMA_INDEX_INDEX_Type                              U09

/* Register dcregOverlayDeGammaData (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Overlay DeGamma Data Translation Register.  Translation values for the     **
** de-gamma table.  When this register  gets written, the data gets stored in **
** the de-gamma table at the  index specified by the dcregDeGammaIndex        **
** register.  After the  register is written, the index gets incremented.     */

#define dcregOverlayDeGammaDataRegAddrs                                   0x0890
#define DCREG_OVERLAY_DE_GAMMA_DATA_Address                              0x02240
#define DCREG_OVERLAY_DE_GAMMA_DATA_MSB                                       15
#define DCREG_OVERLAY_DE_GAMMA_DATA_LSB                                        4
#define DCREG_OVERLAY_DE_GAMMA_DATA_BLK                                        4
#define DCREG_OVERLAY_DE_GAMMA_DATA_Count                                     16
#define DCREG_OVERLAY_DE_GAMMA_DATA_FieldMask                         0x7FFF7FFF
#define DCREG_OVERLAY_DE_GAMMA_DATA_ReadMask                          0x00000000
#define DCREG_OVERLAY_DE_GAMMA_DATA_WriteMask                         0x7FFF7FFF
#define DCREG_OVERLAY_DE_GAMMA_DATA_ResetValue                        0x00000000

/* Blue translation value.  */
#define DCREG_OVERLAY_DE_GAMMA_DATA_BLUE                                    14:0
#define DCREG_OVERLAY_DE_GAMMA_DATA_BLUE_End                                  14
#define DCREG_OVERLAY_DE_GAMMA_DATA_BLUE_Start                                 0
#define DCREG_OVERLAY_DE_GAMMA_DATA_BLUE_Type                                U15

/* Green translation value.  */
#define DCREG_OVERLAY_DE_GAMMA_DATA_GREEN                                  30:16
#define DCREG_OVERLAY_DE_GAMMA_DATA_GREEN_End                                 30
#define DCREG_OVERLAY_DE_GAMMA_DATA_GREEN_Start                               16
#define DCREG_OVERLAY_DE_GAMMA_DATA_GREEN_Type                               U15

/* Register dcregOverlayDeGammaExData (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Overlay DeGamma Data Translation Register.  Translation values for the     **
** de-gamma table.  When this register  gets written, the data gets stored in **
** the de-gamma table at the  index specified by the dcregDeGammaIndex        **
** register.  After the  register is written, the index gets incremented.     */

#define dcregOverlayDeGammaExDataRegAddrs                                 0x08A0
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_Address                           0x02280
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_MSB                                    15
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_LSB                                     4
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_BLK                                     4
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_Count                                  16
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_FieldMask                      0x00007FFF
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_ReadMask                       0x00000000
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_WriteMask                      0x00007FFF
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_ResetValue                     0x00000000

/* Red translation value.  */
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_RED                                  14:0
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_RED_End                                14
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_RED_Start                               0
#define DCREG_OVERLAY_DE_GAMMA_EX_DATA_RED_Type                              U15

/* Register dcregOverlayYClampBound (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined Y clamp value for YUV2RGB. */

#define dcregOverlayYClampBoundRegAddrs                                   0x08B0
#define DCREG_OVERLAY_YCLAMP_BOUND_Address                               0x022C0
#define DCREG_OVERLAY_YCLAMP_BOUND_MSB                                        15
#define DCREG_OVERLAY_YCLAMP_BOUND_LSB                                         4
#define DCREG_OVERLAY_YCLAMP_BOUND_BLK                                         4
#define DCREG_OVERLAY_YCLAMP_BOUND_Count                                      16
#define DCREG_OVERLAY_YCLAMP_BOUND_FieldMask                          0x03FF03FF
#define DCREG_OVERLAY_YCLAMP_BOUND_ReadMask                           0x03FF03FF
#define DCREG_OVERLAY_YCLAMP_BOUND_WriteMask                          0x03FF03FF
#define DCREG_OVERLAY_YCLAMP_BOUND_ResetValue                         0x00000000

/* The min value. */
#define DCREG_OVERLAY_YCLAMP_BOUND_LVALUE                                    9:0
#define DCREG_OVERLAY_YCLAMP_BOUND_LVALUE_End                                  9
#define DCREG_OVERLAY_YCLAMP_BOUND_LVALUE_Start                                0
#define DCREG_OVERLAY_YCLAMP_BOUND_LVALUE_Type                               U10

/* The max value. */
#define DCREG_OVERLAY_YCLAMP_BOUND_HVALUE                                  25:16
#define DCREG_OVERLAY_YCLAMP_BOUND_HVALUE_End                                 25
#define DCREG_OVERLAY_YCLAMP_BOUND_HVALUE_Start                               16
#define DCREG_OVERLAY_YCLAMP_BOUND_HVALUE_Type                               U10

/* Register dcregOverlayUVClampBound (16 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* user defined UV clamp value for YUV2RGB. */

#define dcregOverlayUVClampBoundRegAddrs                                  0x08C0
#define DCREG_OVERLAY_UV_CLAMP_BOUND_Address                             0x02300
#define DCREG_OVERLAY_UV_CLAMP_BOUND_MSB                                      15
#define DCREG_OVERLAY_UV_CLAMP_BOUND_LSB                                       4
#define DCREG_OVERLAY_UV_CLAMP_BOUND_BLK                                       4
#define DCREG_OVERLAY_UV_CLAMP_BOUND_Count                                    16
#define DCREG_OVERLAY_UV_CLAMP_BOUND_FieldMask                        0x03FF03FF
#define DCREG_OVERLAY_UV_CLAMP_BOUND_ReadMask                         0x03FF03FF
#define DCREG_OVERLAY_UV_CLAMP_BOUND_WriteMask                        0x03FF03FF
#define DCREG_OVERLAY_UV_CLAMP_BOUND_ResetValue                       0x00000000

/* The min value. */
#define DCREG_OVERLAY_UV_CLAMP_BOUND_LVALUE                                  9:0
#define DCREG_OVERLAY_UV_CLAMP_BOUND_LVALUE_End                                9
#define DCREG_OVERLAY_UV_CLAMP_BOUND_LVALUE_Start                              0
#define DCREG_OVERLAY_UV_CLAMP_BOUND_LVALUE_Type                             U10

/* The max value. */
#define DCREG_OVERLAY_UV_CLAMP_BOUND_HVALUE                                25:16
#define DCREG_OVERLAY_UV_CLAMP_BOUND_HVALUE_End                               25
#define DCREG_OVERLAY_UV_CLAMP_BOUND_HVALUE_Start                             16
#define DCREG_OVERLAY_UV_CLAMP_BOUND_HVALUE_Type                             U10

/* Register gcregFrameUvSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregFrameUvSTLBStartAddrRegAddrs                                 0x08D0
#define GCREG_FRAME_UV_STLB_START_ADDR_Address                           0x02340
#define GCREG_FRAME_UV_STLB_START_ADDR_MSB                                    15
#define GCREG_FRAME_UV_STLB_START_ADDR_LSB                                     1
#define GCREG_FRAME_UV_STLB_START_ADDR_BLK                                     1
#define GCREG_FRAME_UV_STLB_START_ADDR_Count                                   2
#define GCREG_FRAME_UV_STLB_START_ADDR_FieldMask                      0xFFFFFFFF
#define GCREG_FRAME_UV_STLB_START_ADDR_ReadMask                       0xFFFFFFFF
#define GCREG_FRAME_UV_STLB_START_ADDR_WriteMask                      0xFFFFFFFF
#define GCREG_FRAME_UV_STLB_START_ADDR_ResetValue                     0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_FRAME_UV_STLB_START_ADDR_VALUE                                31:0
#define GCREG_FRAME_UV_STLB_START_ADDR_VALUE_End                              31
#define GCREG_FRAME_UV_STLB_START_ADDR_VALUE_Start                             0
#define GCREG_FRAME_UV_STLB_START_ADDR_VALUE_Type                            U32

/* Register gcregFrameUvSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregFrameUvSTLBEndAddrRegAddrs                                   0x08D2
#define GCREG_FRAME_UV_STLB_END_ADDR_Address                             0x02348
#define GCREG_FRAME_UV_STLB_END_ADDR_MSB                                      15
#define GCREG_FRAME_UV_STLB_END_ADDR_LSB                                       1
#define GCREG_FRAME_UV_STLB_END_ADDR_BLK                                       1
#define GCREG_FRAME_UV_STLB_END_ADDR_Count                                     2
#define GCREG_FRAME_UV_STLB_END_ADDR_FieldMask                        0xFFFFFFFF
#define GCREG_FRAME_UV_STLB_END_ADDR_ReadMask                         0xFFFFFFFF
#define GCREG_FRAME_UV_STLB_END_ADDR_WriteMask                        0xFFFFFFFF
#define GCREG_FRAME_UV_STLB_END_ADDR_ResetValue                       0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_FRAME_UV_STLB_END_ADDR_VALUE                                  31:0
#define GCREG_FRAME_UV_STLB_END_ADDR_VALUE_End                                31
#define GCREG_FRAME_UV_STLB_END_ADDR_VALUE_Start                               0
#define GCREG_FRAME_UV_STLB_END_ADDR_VALUE_Type                              U32

/* Register gcregFrameUvSTLBStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The slave cache line size. Used in MMU Slave TLB prefetch. */

#define gcregFrameUvSTLBStrideRegAddrs                                    0x08D4
#define GCREG_FRAME_UV_STLB_STRIDE_Address                               0x02350
#define GCREG_FRAME_UV_STLB_STRIDE_MSB                                        15
#define GCREG_FRAME_UV_STLB_STRIDE_LSB                                         1
#define GCREG_FRAME_UV_STLB_STRIDE_BLK                                         1
#define GCREG_FRAME_UV_STLB_STRIDE_Count                                       2
#define GCREG_FRAME_UV_STLB_STRIDE_FieldMask                          0x0000FFFF
#define GCREG_FRAME_UV_STLB_STRIDE_ReadMask                           0x0000FFFF
#define GCREG_FRAME_UV_STLB_STRIDE_WriteMask                          0x0000FFFF
#define GCREG_FRAME_UV_STLB_STRIDE_ResetValue                         0x00000000

/* The slave cache line size in horizontal direction. */
#define GCREG_FRAME_UV_STLB_STRIDE_VALUE                                    15:0
#define GCREG_FRAME_UV_STLB_STRIDE_VALUE_End                                  15
#define GCREG_FRAME_UV_STLB_STRIDE_VALUE_Start                                 0
#define GCREG_FRAME_UV_STLB_STRIDE_VALUE_Type                                U16

/* Register gcregFrameUvSTLBMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in MMU Slave TLB prefetch. */

#define gcregFrameUvSTLBMaxNumRegAddrs                                    0x08D6
#define GCREG_FRAME_UV_STLB_MAX_NUM_Address                              0x02358
#define GCREG_FRAME_UV_STLB_MAX_NUM_MSB                                       15
#define GCREG_FRAME_UV_STLB_MAX_NUM_LSB                                        1
#define GCREG_FRAME_UV_STLB_MAX_NUM_BLK                                        1
#define GCREG_FRAME_UV_STLB_MAX_NUM_Count                                      2
#define GCREG_FRAME_UV_STLB_MAX_NUM_FieldMask                         0x00FF00FF
#define GCREG_FRAME_UV_STLB_MAX_NUM_ReadMask                          0x00FF00FF
#define GCREG_FRAME_UV_STLB_MAX_NUM_WriteMask                         0x00FF00FF
#define GCREG_FRAME_UV_STLB_MAX_NUM_ResetValue                        0x00000000

/* Slave TLB horizontal max request number. */
#define GCREG_FRAME_UV_STLB_MAX_NUM_HORIZONTAL                               7:0
#define GCREG_FRAME_UV_STLB_MAX_NUM_HORIZONTAL_End                             7
#define GCREG_FRAME_UV_STLB_MAX_NUM_HORIZONTAL_Start                           0
#define GCREG_FRAME_UV_STLB_MAX_NUM_HORIZONTAL_Type                          U08

/* Slave TLB vertical max request number. */
#define GCREG_FRAME_UV_STLB_MAX_NUM_VERTICAL                               23:16
#define GCREG_FRAME_UV_STLB_MAX_NUM_VERTICAL_End                              23
#define GCREG_FRAME_UV_STLB_MAX_NUM_VERTICAL_Start                            16
#define GCREG_FRAME_UV_STLB_MAX_NUM_VERTICAL_Type                            U08

/* Register gcregFrameVSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregFrameVSTLBStartAddrRegAddrs                                  0x08D8
#define GCREG_FRAME_VSTLB_START_ADDR_Address                             0x02360
#define GCREG_FRAME_VSTLB_START_ADDR_MSB                                      15
#define GCREG_FRAME_VSTLB_START_ADDR_LSB                                       1
#define GCREG_FRAME_VSTLB_START_ADDR_BLK                                       1
#define GCREG_FRAME_VSTLB_START_ADDR_Count                                     2
#define GCREG_FRAME_VSTLB_START_ADDR_FieldMask                        0xFFFFFFFF
#define GCREG_FRAME_VSTLB_START_ADDR_ReadMask                         0xFFFFFFFF
#define GCREG_FRAME_VSTLB_START_ADDR_WriteMask                        0xFFFFFFFF
#define GCREG_FRAME_VSTLB_START_ADDR_ResetValue                       0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_FRAME_VSTLB_START_ADDR_VALUE                                  31:0
#define GCREG_FRAME_VSTLB_START_ADDR_VALUE_End                                31
#define GCREG_FRAME_VSTLB_START_ADDR_VALUE_Start                               0
#define GCREG_FRAME_VSTLB_START_ADDR_VALUE_Type                              U32

/* Register gcregFrameVSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregFrameVSTLBEndAddrRegAddrs                                    0x08DA
#define GCREG_FRAME_VSTLB_END_ADDR_Address                               0x02368
#define GCREG_FRAME_VSTLB_END_ADDR_MSB                                        15
#define GCREG_FRAME_VSTLB_END_ADDR_LSB                                         1
#define GCREG_FRAME_VSTLB_END_ADDR_BLK                                         1
#define GCREG_FRAME_VSTLB_END_ADDR_Count                                       2
#define GCREG_FRAME_VSTLB_END_ADDR_FieldMask                          0xFFFFFFFF
#define GCREG_FRAME_VSTLB_END_ADDR_ReadMask                           0xFFFFFFFF
#define GCREG_FRAME_VSTLB_END_ADDR_WriteMask                          0xFFFFFFFF
#define GCREG_FRAME_VSTLB_END_ADDR_ResetValue                         0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_FRAME_VSTLB_END_ADDR_VALUE                                    31:0
#define GCREG_FRAME_VSTLB_END_ADDR_VALUE_End                                  31
#define GCREG_FRAME_VSTLB_END_ADDR_VALUE_Start                                 0
#define GCREG_FRAME_VSTLB_END_ADDR_VALUE_Type                                U32

/* Register gcregFrameVSTLBStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The slave cache line size. Used in MMU Slave TLB prefetch. */

#define gcregFrameVSTLBStrideRegAddrs                                     0x08DC
#define GCREG_FRAME_VSTLB_STRIDE_Address                                 0x02370
#define GCREG_FRAME_VSTLB_STRIDE_MSB                                          15
#define GCREG_FRAME_VSTLB_STRIDE_LSB                                           1
#define GCREG_FRAME_VSTLB_STRIDE_BLK                                           1
#define GCREG_FRAME_VSTLB_STRIDE_Count                                         2
#define GCREG_FRAME_VSTLB_STRIDE_FieldMask                            0x0000FFFF
#define GCREG_FRAME_VSTLB_STRIDE_ReadMask                             0x0000FFFF
#define GCREG_FRAME_VSTLB_STRIDE_WriteMask                            0x0000FFFF
#define GCREG_FRAME_VSTLB_STRIDE_ResetValue                           0x00000000

/* The slave cache line size in horizontal direction. */
#define GCREG_FRAME_VSTLB_STRIDE_VALUE                                      15:0
#define GCREG_FRAME_VSTLB_STRIDE_VALUE_End                                    15
#define GCREG_FRAME_VSTLB_STRIDE_VALUE_Start                                   0
#define GCREG_FRAME_VSTLB_STRIDE_VALUE_Type                                  U16

/* Register gcregFrameVSTLBMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in MMU Slave TLB prefetch. */

#define gcregFrameVSTLBMaxNumRegAddrs                                     0x08DE
#define GCREG_FRAME_VSTLB_MAX_NUM_Address                                0x02378
#define GCREG_FRAME_VSTLB_MAX_NUM_MSB                                         15
#define GCREG_FRAME_VSTLB_MAX_NUM_LSB                                          1
#define GCREG_FRAME_VSTLB_MAX_NUM_BLK                                          1
#define GCREG_FRAME_VSTLB_MAX_NUM_Count                                        2
#define GCREG_FRAME_VSTLB_MAX_NUM_FieldMask                           0x00FF00FF
#define GCREG_FRAME_VSTLB_MAX_NUM_ReadMask                            0x00FF00FF
#define GCREG_FRAME_VSTLB_MAX_NUM_WriteMask                           0x00FF00FF
#define GCREG_FRAME_VSTLB_MAX_NUM_ResetValue                          0x00000000

/* Slave TLB horizontal max request number. */
#define GCREG_FRAME_VSTLB_MAX_NUM_HORIZONTAL                                 7:0
#define GCREG_FRAME_VSTLB_MAX_NUM_HORIZONTAL_End                               7
#define GCREG_FRAME_VSTLB_MAX_NUM_HORIZONTAL_Start                             0
#define GCREG_FRAME_VSTLB_MAX_NUM_HORIZONTAL_Type                            U08

/* Slave TLB vertical max request number. */
#define GCREG_FRAME_VSTLB_MAX_NUM_VERTICAL                                 23:16
#define GCREG_FRAME_VSTLB_MAX_NUM_VERTICAL_End                                23
#define GCREG_FRAME_VSTLB_MAX_NUM_VERTICAL_Start                              16
#define GCREG_FRAME_VSTLB_MAX_NUM_VERTICAL_Type                              U08

/* Register gcregOverlayYOrArgbSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayYOrArgbSTLBStartAddrRegAddrs                          0x08E0
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_Address                   0x02380
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_MSB                            15
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_LSB                             1
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_BLK                             1
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_Count                           2
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_FieldMask              0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_ReadMask               0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_WriteMask              0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_ResetValue             0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_VALUE                        31:0
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_VALUE_End                      31
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_VALUE_Start                     0
#define GCREG_OVERLAY_YOR_ARGB_STLB_START_ADDR_VALUE_Type                    U32

/* Register gcregOverlayYOrArgbSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayYOrArgbSTLBEndAddrRegAddrs                            0x08E2
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_Address                     0x02388
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_MSB                              15
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_LSB                               1
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_BLK                               1
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_Count                             2
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_FieldMask                0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_ReadMask                 0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_WriteMask                0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_ResetValue               0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_VALUE                          31:0
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_VALUE_End                        31
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_VALUE_Start                       0
#define GCREG_OVERLAY_YOR_ARGB_STLB_END_ADDR_VALUE_Type                      U32

/* Register gcregOverlayYOrArgbSTLBStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The slave cache line size. Used in MMU Slave TLB prefetch. */

#define gcregOverlayYOrArgbSTLBStrideRegAddrs                             0x08E4
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_Address                       0x02390
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_MSB                                15
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_LSB                                 1
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_BLK                                 1
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_Count                               2
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_FieldMask                  0x0000FFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_ReadMask                   0x0000FFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_WriteMask                  0x0000FFFF
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_ResetValue                 0x00000000

/* The slave cache line size in horizontal direction. */
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_VALUE                            15:0
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_VALUE_End                          15
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_VALUE_Start                         0
#define GCREG_OVERLAY_YOR_ARGB_STLB_STRIDE_VALUE_Type                        U16

/* Register gcregOverlayYOrArgbSTLBMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in MMU Slave TLB prefetch. */

#define gcregOverlayYOrArgbSTLBMaxNumRegAddrs                             0x08E6
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_Address                      0x02398
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_MSB                               15
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_LSB                                1
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_BLK                                1
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_Count                              2
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_FieldMask                 0x00FF00FF
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_ReadMask                  0x00FF00FF
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_WriteMask                 0x00FF00FF
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_ResetValue                0x00000000

/* Slave TLB horizontal max request number. */
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL                       7:0
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL_End                     7
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL_Start                   0
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_HORIZONTAL_Type                  U08

/* Slave TLB vertical max request number. */
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_VERTICAL                       23:16
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_VERTICAL_End                      23
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_VERTICAL_Start                    16
#define GCREG_OVERLAY_YOR_ARGB_STLB_MAX_NUM_VERTICAL_Type                    U08

/* Register gcregOverlayUvSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayUvSTLBStartAddrRegAddrs                               0x08E8
#define GCREG_OVERLAY_UV_STLB_START_ADDR_Address                         0x023A0
#define GCREG_OVERLAY_UV_STLB_START_ADDR_MSB                                  15
#define GCREG_OVERLAY_UV_STLB_START_ADDR_LSB                                   1
#define GCREG_OVERLAY_UV_STLB_START_ADDR_BLK                                   1
#define GCREG_OVERLAY_UV_STLB_START_ADDR_Count                                 2
#define GCREG_OVERLAY_UV_STLB_START_ADDR_FieldMask                    0xFFFFFFFF
#define GCREG_OVERLAY_UV_STLB_START_ADDR_ReadMask                     0xFFFFFFFF
#define GCREG_OVERLAY_UV_STLB_START_ADDR_WriteMask                    0xFFFFFFFF
#define GCREG_OVERLAY_UV_STLB_START_ADDR_ResetValue                   0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_OVERLAY_UV_STLB_START_ADDR_VALUE                              31:0
#define GCREG_OVERLAY_UV_STLB_START_ADDR_VALUE_End                            31
#define GCREG_OVERLAY_UV_STLB_START_ADDR_VALUE_Start                           0
#define GCREG_OVERLAY_UV_STLB_START_ADDR_VALUE_Type                          U32

/* Register gcregOverlayUvSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayUvSTLBEndAddrRegAddrs                                 0x08EA
#define GCREG_OVERLAY_UV_STLB_END_ADDR_Address                           0x023A8
#define GCREG_OVERLAY_UV_STLB_END_ADDR_MSB                                    15
#define GCREG_OVERLAY_UV_STLB_END_ADDR_LSB                                     1
#define GCREG_OVERLAY_UV_STLB_END_ADDR_BLK                                     1
#define GCREG_OVERLAY_UV_STLB_END_ADDR_Count                                   2
#define GCREG_OVERLAY_UV_STLB_END_ADDR_FieldMask                      0xFFFFFFFF
#define GCREG_OVERLAY_UV_STLB_END_ADDR_ReadMask                       0xFFFFFFFF
#define GCREG_OVERLAY_UV_STLB_END_ADDR_WriteMask                      0xFFFFFFFF
#define GCREG_OVERLAY_UV_STLB_END_ADDR_ResetValue                     0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_OVERLAY_UV_STLB_END_ADDR_VALUE                                31:0
#define GCREG_OVERLAY_UV_STLB_END_ADDR_VALUE_End                              31
#define GCREG_OVERLAY_UV_STLB_END_ADDR_VALUE_Start                             0
#define GCREG_OVERLAY_UV_STLB_END_ADDR_VALUE_Type                            U32

/* Register gcregOverlayUvSTLBStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The slave cache line size. Used in MMU Slave TLB prefetch. */

#define gcregOverlayUvSTLBStrideRegAddrs                                  0x08EC
#define GCREG_OVERLAY_UV_STLB_STRIDE_Address                             0x023B0
#define GCREG_OVERLAY_UV_STLB_STRIDE_MSB                                      15
#define GCREG_OVERLAY_UV_STLB_STRIDE_LSB                                       1
#define GCREG_OVERLAY_UV_STLB_STRIDE_BLK                                       1
#define GCREG_OVERLAY_UV_STLB_STRIDE_Count                                     2
#define GCREG_OVERLAY_UV_STLB_STRIDE_FieldMask                        0x0000FFFF
#define GCREG_OVERLAY_UV_STLB_STRIDE_ReadMask                         0x0000FFFF
#define GCREG_OVERLAY_UV_STLB_STRIDE_WriteMask                        0x0000FFFF
#define GCREG_OVERLAY_UV_STLB_STRIDE_ResetValue                       0x00000000

/* The slave cache line size in horizontal direction. */
#define GCREG_OVERLAY_UV_STLB_STRIDE_VALUE                                  15:0
#define GCREG_OVERLAY_UV_STLB_STRIDE_VALUE_End                                15
#define GCREG_OVERLAY_UV_STLB_STRIDE_VALUE_Start                               0
#define GCREG_OVERLAY_UV_STLB_STRIDE_VALUE_Type                              U16

/* Register gcregOverlayUvSTLBMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in MMU Slave TLB prefetch. */

#define gcregOverlayUvSTLBMaxNumRegAddrs                                  0x08EE
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_Address                            0x023B8
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_MSB                                     15
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_LSB                                      1
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_BLK                                      1
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_Count                                    2
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_FieldMask                       0x00FF00FF
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_ReadMask                        0x00FF00FF
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_WriteMask                       0x00FF00FF
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_ResetValue                      0x00000000

/* Slave TLB horizontal max request number. */
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_HORIZONTAL                             7:0
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_HORIZONTAL_End                           7
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_HORIZONTAL_Start                         0
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_HORIZONTAL_Type                        U08

/* Slave TLB vertical max request number. */
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_VERTICAL                             23:16
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_VERTICAL_End                            23
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_VERTICAL_Start                          16
#define GCREG_OVERLAY_UV_STLB_MAX_NUM_VERTICAL_Type                          U08

/* Register gcregOverlayVSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayVSTLBStartAddrRegAddrs                                0x08F0
#define GCREG_OVERLAY_VSTLB_START_ADDR_Address                           0x023C0
#define GCREG_OVERLAY_VSTLB_START_ADDR_MSB                                    15
#define GCREG_OVERLAY_VSTLB_START_ADDR_LSB                                     1
#define GCREG_OVERLAY_VSTLB_START_ADDR_BLK                                     1
#define GCREG_OVERLAY_VSTLB_START_ADDR_Count                                   2
#define GCREG_OVERLAY_VSTLB_START_ADDR_FieldMask                      0xFFFFFFFF
#define GCREG_OVERLAY_VSTLB_START_ADDR_ReadMask                       0xFFFFFFFF
#define GCREG_OVERLAY_VSTLB_START_ADDR_WriteMask                      0xFFFFFFFF
#define GCREG_OVERLAY_VSTLB_START_ADDR_ResetValue                     0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_OVERLAY_VSTLB_START_ADDR_VALUE                                31:0
#define GCREG_OVERLAY_VSTLB_START_ADDR_VALUE_End                              31
#define GCREG_OVERLAY_VSTLB_START_ADDR_VALUE_Start                             0
#define GCREG_OVERLAY_VSTLB_START_ADDR_VALUE_Type                            U32

/* Register gcregOverlayVSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayVSTLBEndAddrRegAddrs                                  0x08F2
#define GCREG_OVERLAY_VSTLB_END_ADDR_Address                             0x023C8
#define GCREG_OVERLAY_VSTLB_END_ADDR_MSB                                      15
#define GCREG_OVERLAY_VSTLB_END_ADDR_LSB                                       1
#define GCREG_OVERLAY_VSTLB_END_ADDR_BLK                                       1
#define GCREG_OVERLAY_VSTLB_END_ADDR_Count                                     2
#define GCREG_OVERLAY_VSTLB_END_ADDR_FieldMask                        0xFFFFFFFF
#define GCREG_OVERLAY_VSTLB_END_ADDR_ReadMask                         0xFFFFFFFF
#define GCREG_OVERLAY_VSTLB_END_ADDR_WriteMask                        0xFFFFFFFF
#define GCREG_OVERLAY_VSTLB_END_ADDR_ResetValue                       0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_OVERLAY_VSTLB_END_ADDR_VALUE                                  31:0
#define GCREG_OVERLAY_VSTLB_END_ADDR_VALUE_End                                31
#define GCREG_OVERLAY_VSTLB_END_ADDR_VALUE_Start                               0
#define GCREG_OVERLAY_VSTLB_END_ADDR_VALUE_Type                              U32

/* Register gcregOverlayVSTLBStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The slave cache line size. Used in MMU Slave TLB prefetch. */

#define gcregOverlayVSTLBStrideRegAddrs                                   0x08F4
#define GCREG_OVERLAY_VSTLB_STRIDE_Address                               0x023D0
#define GCREG_OVERLAY_VSTLB_STRIDE_MSB                                        15
#define GCREG_OVERLAY_VSTLB_STRIDE_LSB                                         1
#define GCREG_OVERLAY_VSTLB_STRIDE_BLK                                         1
#define GCREG_OVERLAY_VSTLB_STRIDE_Count                                       2
#define GCREG_OVERLAY_VSTLB_STRIDE_FieldMask                          0x0000FFFF
#define GCREG_OVERLAY_VSTLB_STRIDE_ReadMask                           0x0000FFFF
#define GCREG_OVERLAY_VSTLB_STRIDE_WriteMask                          0x0000FFFF
#define GCREG_OVERLAY_VSTLB_STRIDE_ResetValue                         0x00000000

/* The slave cache line size in horizontal direction. */
#define GCREG_OVERLAY_VSTLB_STRIDE_VALUE                                    15:0
#define GCREG_OVERLAY_VSTLB_STRIDE_VALUE_End                                  15
#define GCREG_OVERLAY_VSTLB_STRIDE_VALUE_Start                                 0
#define GCREG_OVERLAY_VSTLB_STRIDE_VALUE_Type                                U16

/* Register gcregOverlayVSTLBMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in MMU Slave TLB prefetch. */

#define gcregOverlayVSTLBMaxNumRegAddrs                                   0x08F6
#define GCREG_OVERLAY_VSTLB_MAX_NUM_Address                              0x023D8
#define GCREG_OVERLAY_VSTLB_MAX_NUM_MSB                                       15
#define GCREG_OVERLAY_VSTLB_MAX_NUM_LSB                                        1
#define GCREG_OVERLAY_VSTLB_MAX_NUM_BLK                                        1
#define GCREG_OVERLAY_VSTLB_MAX_NUM_Count                                      2
#define GCREG_OVERLAY_VSTLB_MAX_NUM_FieldMask                         0x00FF00FF
#define GCREG_OVERLAY_VSTLB_MAX_NUM_ReadMask                          0x00FF00FF
#define GCREG_OVERLAY_VSTLB_MAX_NUM_WriteMask                         0x00FF00FF
#define GCREG_OVERLAY_VSTLB_MAX_NUM_ResetValue                        0x00000000

/* Slave TLB horizontal max request number. */
#define GCREG_OVERLAY_VSTLB_MAX_NUM_HORIZONTAL                               7:0
#define GCREG_OVERLAY_VSTLB_MAX_NUM_HORIZONTAL_End                             7
#define GCREG_OVERLAY_VSTLB_MAX_NUM_HORIZONTAL_Start                           0
#define GCREG_OVERLAY_VSTLB_MAX_NUM_HORIZONTAL_Type                          U08

/* Slave TLB vertical max request number. */
#define GCREG_OVERLAY_VSTLB_MAX_NUM_VERTICAL                               23:16
#define GCREG_OVERLAY_VSTLB_MAX_NUM_VERTICAL_End                              23
#define GCREG_OVERLAY_VSTLB_MAX_NUM_VERTICAL_Start                            16
#define GCREG_OVERLAY_VSTLB_MAX_NUM_VERTICAL_Type                            U08

/* Register gcregCursorSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregCursorSTLBStartAddrRegAddrs                                  0x08F8
#define GCREG_CURSOR_STLB_START_ADDR_Address                             0x023E0
#define GCREG_CURSOR_STLB_START_ADDR_MSB                                      15
#define GCREG_CURSOR_STLB_START_ADDR_LSB                                       1
#define GCREG_CURSOR_STLB_START_ADDR_BLK                                       1
#define GCREG_CURSOR_STLB_START_ADDR_Count                                     2
#define GCREG_CURSOR_STLB_START_ADDR_FieldMask                        0xFFFFFFFF
#define GCREG_CURSOR_STLB_START_ADDR_ReadMask                         0xFFFFFFFF
#define GCREG_CURSOR_STLB_START_ADDR_WriteMask                        0xFFFFFFFF
#define GCREG_CURSOR_STLB_START_ADDR_ResetValue                       0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_CURSOR_STLB_START_ADDR_VALUE                                  31:0
#define GCREG_CURSOR_STLB_START_ADDR_VALUE_End                                31
#define GCREG_CURSOR_STLB_START_ADDR_VALUE_Start                               0
#define GCREG_CURSOR_STLB_START_ADDR_VALUE_Type                              U32

/* Register gcregCursorSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregCursorSTLBEndAddrRegAddrs                                    0x08FA
#define GCREG_CURSOR_STLB_END_ADDR_Address                               0x023E8
#define GCREG_CURSOR_STLB_END_ADDR_MSB                                        15
#define GCREG_CURSOR_STLB_END_ADDR_LSB                                         1
#define GCREG_CURSOR_STLB_END_ADDR_BLK                                         1
#define GCREG_CURSOR_STLB_END_ADDR_Count                                       2
#define GCREG_CURSOR_STLB_END_ADDR_FieldMask                          0xFFFFFFFF
#define GCREG_CURSOR_STLB_END_ADDR_ReadMask                           0xFFFFFFFF
#define GCREG_CURSOR_STLB_END_ADDR_WriteMask                          0xFFFFFFFF
#define GCREG_CURSOR_STLB_END_ADDR_ResetValue                         0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_CURSOR_STLB_END_ADDR_VALUE                                    31:0
#define GCREG_CURSOR_STLB_END_ADDR_VALUE_End                                  31
#define GCREG_CURSOR_STLB_END_ADDR_VALUE_Start                                 0
#define GCREG_CURSOR_STLB_END_ADDR_VALUE_Type                                U32

/* Register gcregFrameYOrArgbTscSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregFrameYOrArgbTscSTLBStartAddrRegAddrs                         0x08FC
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_Address                 0x023F0
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_MSB                          15
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_LSB                           1
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_BLK                           1
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_Count                         2
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_FieldMask            0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_ReadMask             0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_WriteMask            0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_ResetValue           0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_VALUE                      31:0
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_VALUE_End                    31
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_VALUE_Start                   0
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_START_ADDR_VALUE_Type                  U32

/* Register gcregFrameYOrArgbTscSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregFrameYOrArgbTscSTLBEndAddrRegAddrs                           0x08FE
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_Address                   0x023F8
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_MSB                            15
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_LSB                             1
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_BLK                             1
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_Count                           2
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_FieldMask              0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_ReadMask               0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_WriteMask              0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_ResetValue             0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_VALUE                        31:0
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_VALUE_End                      31
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_VALUE_Start                     0
#define GCREG_FRAME_YOR_ARGB_TSC_STLB_END_ADDR_VALUE_Type                    U32

/* Register gcregFrameUvTscSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregFrameUvTscSTLBStartAddrRegAddrs                              0x0900
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_Address                       0x02400
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_MSB                                15
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_LSB                                 1
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_BLK                                 1
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_Count                               2
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_FieldMask                  0xFFFFFFFF
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_ReadMask                   0xFFFFFFFF
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_WriteMask                  0xFFFFFFFF
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_ResetValue                 0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_VALUE                            31:0
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_VALUE_End                          31
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_VALUE_Start                         0
#define GCREG_FRAME_UV_TSC_STLB_START_ADDR_VALUE_Type                        U32

/* Register gcregFrameUvTscSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregFrameUvTscSTLBEndAddrRegAddrs                                0x0902
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_Address                         0x02408
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_MSB                                  15
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_LSB                                   1
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_BLK                                   1
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_Count                                 2
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_FieldMask                    0xFFFFFFFF
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_ReadMask                     0xFFFFFFFF
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_WriteMask                    0xFFFFFFFF
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_ResetValue                   0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_VALUE                              31:0
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_VALUE_End                            31
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_VALUE_Start                           0
#define GCREG_FRAME_UV_TSC_STLB_END_ADDR_VALUE_Type                          U32

/* Register gcregOverlayYOrArgbTscSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayYOrArgbTscSTLBStartAddrRegAddrs                       0x0904
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_Address               0x02410
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_MSB                        15
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_LSB                         1
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_BLK                         1
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_Count                       2
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_FieldMask          0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_ReadMask           0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_WriteMask          0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_ResetValue         0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_VALUE                    31:0
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_VALUE_End                  31
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_VALUE_Start                 0
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_START_ADDR_VALUE_Type                U32

/* Register gcregOverlayYOrArgbTscSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayYOrArgbTscSTLBEndAddrRegAddrs                         0x0906
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_Address                 0x02418
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_MSB                          15
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_LSB                           1
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_BLK                           1
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_Count                         2
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_FieldMask            0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_ReadMask             0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_WriteMask            0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_ResetValue           0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_VALUE                      31:0
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_VALUE_End                    31
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_VALUE_Start                   0
#define GCREG_OVERLAY_YOR_ARGB_TSC_STLB_END_ADDR_VALUE_Type                  U32

/* Register gcregOverlayUvTscSTLBStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry start address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayUvTscSTLBStartAddrRegAddrs                            0x0908
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_Address                     0x02420
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_MSB                              15
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_LSB                               1
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_BLK                               1
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_Count                             2
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_FieldMask                0xFFFFFFFF
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_ReadMask                 0xFFFFFFFF
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_WriteMask                0xFFFFFFFF
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_ResetValue               0x00000000

/* Slave Entry start address: STLB base + STLB index. */
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_VALUE                          31:0
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_VALUE_End                        31
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_VALUE_Start                       0
#define GCREG_OVERLAY_UV_TSC_STLB_START_ADDR_VALUE_Type                      U32

/* Register gcregOverlayUvTscSTLBEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Slave Entry end address. Used in MMU Slave TLB prefetch. */

#define gcregOverlayUvTscSTLBEndAddrRegAddrs                              0x090A
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_Address                       0x02428
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_MSB                                15
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_LSB                                 1
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_BLK                                 1
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_Count                               2
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_FieldMask                  0xFFFFFFFF
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_ReadMask                   0xFFFFFFFF
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_WriteMask                  0xFFFFFFFF
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_ResetValue                 0x00000000

/* Slave Entry end address: STLB end + STLB index. */
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_VALUE                            31:0
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_VALUE_End                          31
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_VALUE_Start                         0
#define GCREG_OVERLAY_UV_TSC_STLB_END_ADDR_VALUE_Type                        U32

/* Register gcregTSCPrefetch (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Enable or disable TSC prefetch. Used in DEC Tile Status prefetch. */

#define gcregTSCPrefetchRegAddrs                                          0x090C
#define GCREG_TSC_PREFETCH_Address                                       0x02430
#define GCREG_TSC_PREFETCH_MSB                                                15
#define GCREG_TSC_PREFETCH_LSB                                                 1
#define GCREG_TSC_PREFETCH_BLK                                                 1
#define GCREG_TSC_PREFETCH_Count                                               2
#define GCREG_TSC_PREFETCH_FieldMask                                  0x00000003
#define GCREG_TSC_PREFETCH_ReadMask                                   0x00000003
#define GCREG_TSC_PREFETCH_WriteMask                                  0x00000003
#define GCREG_TSC_PREFETCH_ResetValue                                 0x00000000

/* Prefetch config. */
#define GCREG_TSC_PREFETCH_CONFIG                                            0:0
#define GCREG_TSC_PREFETCH_CONFIG_End                                          0
#define GCREG_TSC_PREFETCH_CONFIG_Start                                        0
#define GCREG_TSC_PREFETCH_CONFIG_Type                                       U01
#define   GCREG_TSC_PREFETCH_CONFIG_DISABLE                                  0x0
#define   GCREG_TSC_PREFETCH_CONFIG_ENABLE                                   0x1

/* Setted up from SW or HW. */
#define GCREG_TSC_PREFETCH_SET_UP                                            1:1
#define GCREG_TSC_PREFETCH_SET_UP_End                                          1
#define GCREG_TSC_PREFETCH_SET_UP_Start                                        1
#define GCREG_TSC_PREFETCH_SET_UP_Type                                       U01
#define   GCREG_TSC_PREFETCH_SET_UP_FROM_HW                                  0x0
#define   GCREG_TSC_PREFETCH_SET_UP_FROM_SW                                  0x1

/* Register gcregFrameYOrArgbDecTscStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC start address. Used in DEC Tile Status prefetch. */

#define gcregFrameYOrArgbDecTscStartAddrRegAddrs                          0x090E
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_Address                  0x02438
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_MSB                           15
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_LSB                            1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_BLK                            1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_Count                          2
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_FieldMask             0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_ReadMask              0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_WriteMask             0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_ResetValue            0x00000000

/* The tile status cache virtual end address. */
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_VALUE                       31:0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_VALUE_End                     31
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_VALUE_Start                    0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_START_ADDR_VALUE_Type                   U32

/* Register gcregFrameYOrArgbDecTscEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC end address. Used in DEC Tile Status prefetch. */

#define gcregFrameYOrArgbDecTscEndAddrRegAddrs                            0x0910
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_Address                    0x02440
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_MSB                             15
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_LSB                              1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_BLK                              1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_Count                            2
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_FieldMask               0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_ReadMask                0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_WriteMask               0xFFFFFFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_ResetValue              0x00000000

/* The tile status cache virtual end address. */
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_VALUE                         31:0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_VALUE_End                       31
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_VALUE_Start                      0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_END_ADDR_VALUE_Type                     U32

/* Register gcregFrameYOrArgbDecTscStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The tile status cache line size. Used in DEC Tile Status prefetch. */

#define gcregFrameYOrArgbDecTscStrideRegAddrs                             0x0912
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_Address                      0x02448
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_MSB                               15
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_LSB                                1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_BLK                                1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_Count                              2
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_FieldMask                 0x0000FFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_ReadMask                  0x0000FFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_WriteMask                 0x0000FFFF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_ResetValue                0x00000000

/* The tile status cache line size in horizontal direction. */
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_VALUE                           15:0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_VALUE_End                         15
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_VALUE_Start                        0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_STRIDE_VALUE_Type                       U16

/* Register gcregFrameYOrArgbDecTscMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in DEC Tile Status prefetch. */

#define gcregFrameYOrArgbDecTscMaxNumRegAddrs                             0x0914
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_Address                     0x02450
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_MSB                              15
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_LSB                               1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_BLK                               1
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_Count                             2
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_FieldMask                0x03FF03FF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_ReadMask                 0x03FF03FF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_WriteMask                0x03FF03FF
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_ResetValue               0x00000000

/* TSC horizontal max request number. */
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL                      9:0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL_End                    9
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL_Start                  0
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL_Type                 U10

/* TSC vertical max request number. */
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL                      25:16
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL_End                     25
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL_Start                   16
#define GCREG_FRAME_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL_Type                   U10

/* Register gcregFrameUvDecTscStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC start address. Used in DEC Tile Status prefetch. */

#define gcregFrameUvDecTscStartAddrRegAddrs                               0x0916
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_Address                        0x02458
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_MSB                                 15
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_LSB                                  1
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_BLK                                  1
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_Count                                2
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_FieldMask                   0xFFFFFFFF
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_ReadMask                    0xFFFFFFFF
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_WriteMask                   0xFFFFFFFF
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_ResetValue                  0x00000000

/* The tile status cache virtual end address. */
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_VALUE                             31:0
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_VALUE_End                           31
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_VALUE_Start                          0
#define GCREG_FRAME_UV_DEC_TSC_START_ADDR_VALUE_Type                         U32

/* Register gcregFrameUvDecTscEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC end address. Used in DEC Tile Status prefetch. */

#define gcregFrameUvDecTscEndAddrRegAddrs                                 0x0918
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_Address                          0x02460
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_MSB                                   15
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_LSB                                    1
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_BLK                                    1
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_Count                                  2
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_FieldMask                     0xFFFFFFFF
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_ReadMask                      0xFFFFFFFF
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_WriteMask                     0xFFFFFFFF
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_ResetValue                    0x00000000

/* The tile status cache virtual end address. */
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_VALUE                               31:0
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_VALUE_End                             31
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_VALUE_Start                            0
#define GCREG_FRAME_UV_DEC_TSC_END_ADDR_VALUE_Type                           U32

/* Register gcregFrameUvDecTscStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The tile status cache line size. Used in DEC Tile Status prefetch. */

#define gcregFrameUvDecTscStrideRegAddrs                                  0x091A
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_Address                            0x02468
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_MSB                                     15
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_LSB                                      1
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_BLK                                      1
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_Count                                    2
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_FieldMask                       0x0000FFFF
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_ReadMask                        0x0000FFFF
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_WriteMask                       0x0000FFFF
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_ResetValue                      0x00000000

/* The tile status cache line size in horizontal direction. */
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_VALUE                                 15:0
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_VALUE_End                               15
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_VALUE_Start                              0
#define GCREG_FRAME_UV_DEC_TSC_STRIDE_VALUE_Type                             U16

/* Register gcregFrameUvDecTscMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in DEC Tile Status prefetch. */

#define gcregFrameUvDecTscMaxNumRegAddrs                                  0x091C
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_Address                           0x02470
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_MSB                                    15
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_LSB                                     1
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_BLK                                     1
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_Count                                   2
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_FieldMask                      0x03FF03FF
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_ReadMask                       0x03FF03FF
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_WriteMask                      0x03FF03FF
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_ResetValue                     0x00000000

/* TSC horizontal max request number. */
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_HORIZONTAL                            9:0
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_HORIZONTAL_End                          9
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_HORIZONTAL_Start                        0
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_HORIZONTAL_Type                       U10

/* TSC vertical max request number. */
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_VERTICAL                            25:16
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_VERTICAL_End                           25
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_VERTICAL_Start                         16
#define GCREG_FRAME_UV_DEC_TSC_MAX_NUM_VERTICAL_Type                         U10

/* Register gcregOverlayYOrArgbDecTscStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC start address. Used in DEC Tile Status prefetch. */

#define gcregOverlayYOrArgbDecTscStartAddrRegAddrs                        0x091E
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_Address                0x02478
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_MSB                         15
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_LSB                          1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_BLK                          1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_Count                        2
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_FieldMask           0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_ReadMask            0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_WriteMask           0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_ResetValue          0x00000000

/* The tile status cache virtual start address. */
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_VALUE                     31:0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_VALUE_End                   31
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_VALUE_Start                  0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_START_ADDR_VALUE_Type                 U32

/* Register gcregOverlayYOrArgbDecTscEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC end address. Used in DEC Tile Status prefetch. */

#define gcregOverlayYOrArgbDecTscEndAddrRegAddrs                          0x0920
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_Address                  0x02480
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_MSB                           15
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_LSB                            1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_BLK                            1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_Count                          2
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_FieldMask             0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_ReadMask              0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_WriteMask             0xFFFFFFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_ResetValue            0x00000000

/* The tile status cache virtual end address. */
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_VALUE                       31:0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_VALUE_End                     31
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_VALUE_Start                    0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_END_ADDR_VALUE_Type                   U32

/* Register gcregOverlayYOrArgbDecTscStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The tile status cache line size. Used in DEC Tile Status prefetch. */

#define gcregOverlayYOrArgbDecTscStrideRegAddrs                           0x0922
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_Address                    0x02488
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_MSB                             15
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_LSB                              1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_BLK                              1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_Count                            2
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_FieldMask               0x0000FFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_ReadMask                0x0000FFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_WriteMask               0x0000FFFF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_ResetValue              0x00000000

/* The tile status cache line size in horizontal direction. */
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_VALUE                         15:0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_VALUE_End                       15
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_VALUE_Start                      0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_STRIDE_VALUE_Type                     U16

/* Register gcregOverlayYOrArgbDecTscMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in DEC Tile Status prefetch. */

#define gcregOverlayYOrArgbDecTscMaxNumRegAddrs                           0x0924
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_Address                   0x02490
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_MSB                            15
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_LSB                             1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_BLK                             1
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_Count                           2
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_FieldMask              0x03FF03FF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_ReadMask               0x03FF03FF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_WriteMask              0x03FF03FF
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_ResetValue             0x00000000

/* TSC horizontal max request number. */
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL                    9:0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL_End                  9
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL_Start                0
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_HORIZONTAL_Type               U10

/* TSC vertical max request number. */
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL                    25:16
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL_End                   25
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL_Start                 16
#define GCREG_OVERLAY_YOR_ARGB_DEC_TSC_MAX_NUM_VERTICAL_Type                 U10

/* Register gcregOverlayUvDecTscStartAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC start address. Used in DEC Tile Status prefetch. */

#define gcregOverlayUvDecTscStartAddrRegAddrs                             0x0926
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_Address                      0x02498
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_MSB                               15
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_LSB                                1
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_BLK                                1
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_Count                              2
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_FieldMask                 0xFFFFFFFF
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_ReadMask                  0xFFFFFFFF
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_WriteMask                 0xFFFFFFFF
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_ResetValue                0x00000000

/* The tile status cache virtual start address. */
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_VALUE                           31:0
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_VALUE_End                         31
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_VALUE_Start                        0
#define GCREG_OVERLAY_UV_DEC_TSC_START_ADDR_VALUE_Type                       U32

/* Register gcregOverlayUvDecTscEndAddr (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* TSC end address. Used in DEC Tile Status prefetch. */

#define gcregOverlayUvDecTscEndAddrRegAddrs                               0x0928
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_Address                        0x024A0
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_MSB                                 15
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_LSB                                  1
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_BLK                                  1
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_Count                                2
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_FieldMask                   0xFFFFFFFF
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_ReadMask                    0xFFFFFFFF
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_WriteMask                   0xFFFFFFFF
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_ResetValue                  0x00000000

/* The tile status cache virtual end address. */
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_VALUE                             31:0
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_VALUE_End                           31
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_VALUE_Start                          0
#define GCREG_OVERLAY_UV_DEC_TSC_END_ADDR_VALUE_Type                         U32

/* Register gcregOverlayUvDecTscStride (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The tile status cache line size. Used in DEC Tile Status prefetch. */

#define gcregOverlayUvDecTscStrideRegAddrs                                0x092A
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_Address                          0x024A8
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_MSB                                   15
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_LSB                                    1
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_BLK                                    1
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_Count                                  2
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_FieldMask                     0x0000FFFF
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_ReadMask                      0x0000FFFF
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_WriteMask                     0x0000FFFF
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_ResetValue                    0x00000000

/* The tile status cache line size in horizontal direction. */
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_VALUE                               15:0
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_VALUE_End                             15
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_VALUE_Start                            0
#define GCREG_OVERLAY_UV_DEC_TSC_STRIDE_VALUE_Type                           U16

/* Register gcregOverlayUvDecTscMaxNum (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The request max number. Used in DEC Tile Status prefetch. */

#define gcregOverlayUvDecTscMaxNumRegAddrs                                0x092C
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_Address                         0x024B0
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_MSB                                  15
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_LSB                                   1
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_BLK                                   1
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_Count                                 2
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_FieldMask                    0x03FF03FF
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_ReadMask                     0x03FF03FF
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_WriteMask                    0x03FF03FF
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_ResetValue                   0x00000000

/* TSC horizontal max request number. */
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_HORIZONTAL                          9:0
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_HORIZONTAL_End                        9
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_HORIZONTAL_Start                      0
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_HORIZONTAL_Type                     U10

/* TSC vertical max request number. */
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_VERTICAL                          25:16
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_VERTICAL_End                         25
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_VERTICAL_Start                       16
#define GCREG_OVERLAY_UV_DEC_TSC_MAX_NUM_VERTICAL_Type                       U10

/* Register dcregThreedLutScale (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The scale value for 3D-LUT. */

#define dcregThreedLutScaleRegAddrs                                       0x092E
#define DCREG_THREED_LUT_SCALE_Address                                   0x024B8
#define DCREG_THREED_LUT_SCALE_MSB                                            15
#define DCREG_THREED_LUT_SCALE_LSB                                             1
#define DCREG_THREED_LUT_SCALE_BLK                                             1
#define DCREG_THREED_LUT_SCALE_Count                                           2
#define DCREG_THREED_LUT_SCALE_FieldMask                              0x3FFFFFFF
#define DCREG_THREED_LUT_SCALE_ReadMask                               0x3FFFFFFF
#define DCREG_THREED_LUT_SCALE_WriteMask                              0x3FFFFFFF
#define DCREG_THREED_LUT_SCALE_ResetValue                             0x10040100

#define DCREG_THREED_LUT_SCALE_B                                             9:0
#define DCREG_THREED_LUT_SCALE_B_End                                           9
#define DCREG_THREED_LUT_SCALE_B_Start                                         0
#define DCREG_THREED_LUT_SCALE_B_Type                                        U10

#define DCREG_THREED_LUT_SCALE_G                                           19:10
#define DCREG_THREED_LUT_SCALE_G_End                                          19
#define DCREG_THREED_LUT_SCALE_G_Start                                        10
#define DCREG_THREED_LUT_SCALE_G_Type                                        U10

#define DCREG_THREED_LUT_SCALE_R                                           29:20
#define DCREG_THREED_LUT_SCALE_R_End                                          29
#define DCREG_THREED_LUT_SCALE_R_Start                                        20
#define DCREG_THREED_LUT_SCALE_R_Type                                        U10

/* Register dcregThreedLutOffset (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The lookup table data for 3D-LUT. */

#define dcregThreedLutOffsetRegAddrs                                      0x0930
#define DCREG_THREED_LUT_OFFSET_Address                                  0x024C0
#define DCREG_THREED_LUT_OFFSET_MSB                                           15
#define DCREG_THREED_LUT_OFFSET_LSB                                            1
#define DCREG_THREED_LUT_OFFSET_BLK                                            1
#define DCREG_THREED_LUT_OFFSET_Count                                          2
#define DCREG_THREED_LUT_OFFSET_FieldMask                             0x0FFF0FFF
#define DCREG_THREED_LUT_OFFSET_ReadMask                              0x0FFF0FFF
#define DCREG_THREED_LUT_OFFSET_WriteMask                             0x0FFF0FFF
#define DCREG_THREED_LUT_OFFSET_ResetValue                            0x00000000

#define DCREG_THREED_LUT_OFFSET_B                                           11:0
#define DCREG_THREED_LUT_OFFSET_B_End                                         11
#define DCREG_THREED_LUT_OFFSET_B_Start                                        0
#define DCREG_THREED_LUT_OFFSET_B_Type                                       U12

#define DCREG_THREED_LUT_OFFSET_G                                          27:16
#define DCREG_THREED_LUT_OFFSET_G_End                                         27
#define DCREG_THREED_LUT_OFFSET_G_Start                                       16
#define DCREG_THREED_LUT_OFFSET_G_Type                                       U12

/* Register dcregThreedLutOffsetEx (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The lookup table data for 3D-LUT. */

#define dcregThreedLutOffsetExRegAddrs                                    0x0932
#define DCREG_THREED_LUT_OFFSET_EX_Address                               0x024C8
#define DCREG_THREED_LUT_OFFSET_EX_MSB                                        15
#define DCREG_THREED_LUT_OFFSET_EX_LSB                                         1
#define DCREG_THREED_LUT_OFFSET_EX_BLK                                         1
#define DCREG_THREED_LUT_OFFSET_EX_Count                                       2
#define DCREG_THREED_LUT_OFFSET_EX_FieldMask                          0x00000FFF
#define DCREG_THREED_LUT_OFFSET_EX_ReadMask                           0x00000FFF
#define DCREG_THREED_LUT_OFFSET_EX_WriteMask                          0x00000FFF
#define DCREG_THREED_LUT_OFFSET_EX_ResetValue                         0x00000000

#define DCREG_THREED_LUT_OFFSET_EX_R                                        11:0
#define DCREG_THREED_LUT_OFFSET_EX_R_End                                      11
#define DCREG_THREED_LUT_OFFSET_EX_R_Start                                     0
#define DCREG_THREED_LUT_OFFSET_EX_R_Type                                    U12

/* Register dcregThreedLutEnlarge (2 in total) **
** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* The enlarge value for 3D LUT.  */

#define dcregThreedLutEnlargeRegAddrs                                     0x0934
#define DCREG_THREED_LUT_ENLARGE_Address                                 0x024D0
#define DCREG_THREED_LUT_ENLARGE_MSB                                          15
#define DCREG_THREED_LUT_ENLARGE_LSB                                           1
#define DCREG_THREED_LUT_ENLARGE_BLK                                           1
#define DCREG_THREED_LUT_ENLARGE_Count                                         2
#define DCREG_THREED_LUT_ENLARGE_FieldMask                            0x0000000F
#define DCREG_THREED_LUT_ENLARGE_ReadMask                             0x0000000F
#define DCREG_THREED_LUT_ENLARGE_WriteMask                            0x0000000F
#define DCREG_THREED_LUT_ENLARGE_ResetValue                           0x00000000

/* Enlarge value */
#define DCREG_THREED_LUT_ENLARGE_VALUE                                       3:0
#define DCREG_THREED_LUT_ENLARGE_VALUE_End                                     3
#define DCREG_THREED_LUT_ENLARGE_VALUE_Start                                   0
#define DCREG_THREED_LUT_ENLARGE_VALUE_Type                                  U04

gctUINT eswin_dc_read_reg(void *regs, gctUINT addr);
gctVOID eswin_dc_write_reg(void *regs, gctUINT addr, gctUINT data);

/* dc clock Operation */
gctVOID eswin_dc_reset(struct dc8000_dc *dc);

/* dc memory operation */
gctVOID eswin_hw_mmu_enable(struct dc8000_dc *dc, gctBOOL enable);

/* dc dec operation */
gctVOID eswin_hw_dec_disable(struct dc8000_dc *dc, gctBOOL disable);
gctVOID eswin_hw_dec_inter_enable(struct dc8000_dc *dc, gctUINT inter);
gctVOID eswin_hw_dec_inter_enable_ex(struct dc8000_dc *dc, gctUINT inter);
gctVOID eswin_hw_dec_inter_enable_ex2(struct dc8000_dc *dc, gctUINT inter);
gctVOID eswin_hw_dec_control(struct dc8000_dc *dc, gctUINT control);
gctVOID eswin_hw_dec_control_ex(struct dc8000_dc *dc, gctUINT control);
gctVOID eswin_hw_dec_control_ex2(struct dc8000_dc *dc, gctUINT control);

/* dc Framebuffer Operation */
gctVOID eswin_hw_set_framebuffer_address(struct dc8000_dc *dc, gctUINT addr);
gctVOID eswin_hw_set_framebuffer_stride(struct dc8000_dc *dc, gctUINT stride);
gctVOID eswin_hw_set_framebuffer_bgcolor(struct dc8000_dc *dc, gctUINT color);
gctVOID eswin_hw_set_framebuffer_colorkey(struct dc8000_dc *dc, gctUINT colorKey);
gctVOID eswin_hw_set_framebuffer_colorkeyhigh(struct dc8000_dc *dc, gctUINT colorKey);
gctVOID eswin_hw_set_framebuffer_config(struct dc8000_dc *dc, gctUINT config);
gctUINT eswin_hw_get_framebuffer_config(struct dc8000_dc *dc);
gctVOID eswin_hw_set_framebuffer_scale_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_framebuffer_colortable_index(struct dc8000_dc *dc, gctUINT index);
gctVOID eswin_hw_set_framebuffer_colortable_data(struct dc8000_dc *dc, gctUINT data);
gctVOID eswin_hw_set_framebuffer_hor_index(struct dc8000_dc *dc, gctUINT index);
gctVOID eswin_hw_set_framebuffer_ver_index(struct dc8000_dc *dc, gctUINT index);
gctVOID eswin_hw_set_framebuffer_hor_kernel(struct dc8000_dc *dc, gctUINT data);
gctVOID eswin_hw_set_framebuffer_ver_kernel(struct dc8000_dc *dc, gctUINT data);
gctVOID eswin_hw_set_framebuffer_size(struct dc8000_dc *dc, gctUINT size);
gctVOID eswin_hw_set_framebuffer_scale_factorX(struct dc8000_dc *dc, gctUINT x);
gctVOID eswin_hw_set_framebuffer_scale_factorY(struct dc8000_dc *dc, gctUINT y);
gctVOID eswin_hw_set_framebuffer_clear(struct dc8000_dc *dc, gctUINT value);
gctVOID eswin_hw_set_framebuffer_initial_offset(struct dc8000_dc *dc, gctUINT offset);
gctVOID eswin_hw_framebuffer_gamma_enable(struct dc8000_dc *dc, gctBOOL enable);
gctVOID eswin_hw_framebuffer_degamma_enable(struct dc8000_dc *dc, gctBOOL enable);
gctVOID eswin_hw_framebuffer_water_mark(struct dc8000_dc *dc, gctUINT mark);
gctVOID eswin_hw_set_framebuffer_config_ex(struct dc8000_dc *dc, gctUINT config);

/* dc overlay Operation */
gctVOID eswin_hw_overlay_enable(struct dc8000_dc *dc, gctBOOL enable);
gctVOID eswin_hw_set_overlay_address(struct dc8000_dc *dc, gctUINT addr);
gctVOID eswin_hw_set_overlay_stride(struct dc8000_dc *dc, gctUINT stride);
gctVOID eswin_hw_set_overlay_size(struct dc8000_dc *dc, gctUINT size);
gctVOID eswin_hw_set_overlay_tl(struct dc8000_dc *dc, gctUINT tl);
gctVOID eswin_hw_set_overlay_br(struct dc8000_dc *dc, gctUINT br);
gctVOID eswin_hw_set_overlay_scale_factorX(struct dc8000_dc *dc, gctUINT x);
gctVOID eswin_hw_set_overlay_scale_factorY(struct dc8000_dc *dc, gctUINT y);
gctVOID eswin_hw_set_overlay_src_global_color(struct dc8000_dc *dc, gctUINT color);
gctVOID eswin_hw_set_overlay_dst_global_color(struct dc8000_dc *dc, gctUINT color);
gctVOID eswin_hw_set_overlay_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_overlay_alpha_blend_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_overlay_scale_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_overlay_colortable_index(struct dc8000_dc *dc, gctUINT index);
gctVOID eswin_hw_set_overlay_colortable_data(struct dc8000_dc *dc, gctUINT data);
gctVOID eswin_hw_set_overlay_hor_index(struct dc8000_dc *dc, gctUINT index);
gctVOID eswin_hw_set_overlay_ver_index(struct dc8000_dc *dc, gctUINT index);
gctVOID eswin_hw_set_overlay_hor_kernel(struct dc8000_dc *dc, gctUINT data);
gctVOID eswin_hw_set_overlay_ver_kernel(struct dc8000_dc *dc, gctUINT data);
gctVOID eswin_hw_set_overlay_initial_offset(struct dc8000_dc *dc, gctUINT offset);
gctVOID eswin_hw_set_overlay_colorkey(struct dc8000_dc *dc, gctUINT colorKey);
gctVOID eswin_hw_set_overlay_colorkeyhigh(struct dc8000_dc *dc, gctUINT colorKey);
gctVOID eswin_hw_overlay_water_mark(struct dc8000_dc *dc, gctUINT mark);

/* dc cursor Operation */
gctVOID eswin_hw_set_cursor_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_cursor_module_clock_gating(struct dc8000_dc *dc, gctUINT control);

/* dc writeback Operation */
gctVOID eswin_hw_set_mem_dest_addr(struct dc8000_dc *dc, gctUINT addr);
gctVOID eswin_hw_set_panel_dest_addr(struct dc8000_dc *dc, gctUINT addr);
gctVOID eswin_hw_set_dest_stride(struct dc8000_dc *dc, gctUINT stride);
gctVOID eswin_hw_set_dest_config(struct dc8000_dc *dc, gctUINT config);

/* dc display Operation */
gctVOID eswin_hw_set_display_dither_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_panel_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_display_intr_enable(struct dc8000_dc *dc, gctUINT enable);
gctVOID eswin_hw_set_general_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_dpi_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_dp_config(struct dc8000_dc *dc, gctUINT config);
gctVOID eswin_hw_set_hdispay(struct dc8000_dc *dc, gctUINT hdispay);
gctVOID eswin_hw_set_vdispay(struct dc8000_dc *dc, gctUINT vdispay);
gctVOID eswin_hw_set_hsync(struct dc8000_dc *dc, gctUINT hsync);
gctVOID eswin_hw_set_vsync(struct dc8000_dc *dc, gctUINT vsync);
gctVOID eswin_hw_dither_enable(struct dc8000_dc *dc, gctBOOL enable, gctUINT low, gctUINT high);
gctVOID eswin_hw_set_interrupt_enable(struct dc8000_dc *dc, gctBOOL enable);
gctUINT eswin_hw_get_interrupt_enable(struct dc8000_dc *dc);
gctVOID eswin_hw_set_module_clock_gating(struct dc8000_dc *dc, gctUINT control);
gctVOID eswin_hw_set_read_ot(struct dc8000_dc *dc, gctUINT ot);
gctVOID eswin_hw_set_three_lut_enlarge(struct dc8000_dc *dc, gctUINT enlarge);
gctVOID eswin_hw_set_tsc_prefetch(struct dc8000_dc *dc, gctUINT prefetch);
gctVOID eswin_hw_set_display_dither_tablelow(struct dc8000_dc *dc, gctUINT table);
gctVOID eswin_hw_set_display_dither_tablehigh(struct dc8000_dc *dc, gctUINT table);
void eswin_syscrg_config(int pclk);

#endif /* _ESWIN_DC_REG_H */
