// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2024 Beijing ESWIN Computing Technology Co., Ltd.
 *
 */

#ifndef _ESWIN_DC_TYPE_H_
#define _ESWIN_DC_TYPE_H_

/* Chip models. */
typedef enum _dcCHIPMODEL
{
    dc0000  = 0x0000,
    dc8000  = 0x8000,
    dculta_l  = 0x0000,
}dcCHIPMODEL;

 /*
  * Chip feature.
  */
typedef enum _gceFEATURE
{
    gcvFEATURE_DC_SOURCE_ROTATION,
    gcvFEATURE_DC_TILED,
    gcvFEATURE_DC_YUV_L1,
    gcvFEATURE_DC_OVERLAY_SCALING,
    gcvFEATURE_DC_D30_OUTPUT,
    gcvFEATURE_DC_MMU,
    gcvFEATURE_DC_COMPRESSION,
    gcvFEATURE_DC_QOS,
    gcvFEATURE_DC_HORFILTER_5,
    gcvFEATURE_DC_10BIT,    /*Fix 10-bit NV12/ NV16 format cross 4K byte issue. */
    gcvFEATURE_DC_FIX_HWCURSOR,   /* Fix HW cursor issue, cursor can cross original framebuffer boundary. */
    gcvFEATURE_DC_ROI, /* Support ROI feature */
    gcvFEATURE_DC_DP, /* Support DP output type */
    gcvFEATURE_DC_DP_YUV, /* Support DP YUV output format */
    gcvFEATURE_DC_NEW_GAMMA, /* Support new gamma, expand channel bits */
    gcvFEATURE_DC_CURSOR_SIZE_CONFIG, /* Support configurable cursor size */
    gcvFEATURE_DC_CSC_MOUDLE, /* Support programmable csc moudle */
    gcvFEATURE_DC_TSC_PREFETCH, /* Support TSC prefetch */
    gcvFEATURE_DC_MMU_PREFETCH, /* Support MMU prefetch */
    gcvFEATURE_DC_FORMAT_OPT, /* remove NV12_10bit/NV16_10bit/YV12/Index/TileY */
    gcvFEATURE_DC_DEC400_VERISION, /* Support different dec400 version */
    /* Insert features above this comment only. */
    gcvFEATURE_COUNT    /* Not a feature. */
}gceFEATURE;

 /*
    DPI display size.
 */
typedef enum _eswin_display_size_type
{
    eswin_display_640_480,
    eswin_display_720_480,
    eswin_display_800_480_60,
    eswin_display_1024_600_60,
    eswin_display_1280_720,
    eswin_display_1920_1080,
    eswin_display_3840_2160_30,
    eswin_display_3840_2160,
    eswin_display_4096_2160_60,

}eswin_display_size_type;

 /*
  * Frame buffer format and output format type.
  */
typedef enum _eswin_format_type {
    /* Unknown */
    eswinUNKNOWN,

    /* General */
    eswinARGB1555,
    eswinARGB4444,
    eswinRGB565,
    eswinARGB8888,
    eswinNV12,
    eswinYUY2,

    /* DBI */
    eswinD8R3G3B2,
    eswinD8R4G4B4,
    eswinD8R5G6B5,
    eswinD8R6G6B6,
    eswinD8R8G8B8,
    eswinD9R6G6B6,
    eswinD16R3G3B2,
    eswinD16R4G4B4,
    eswinD16R5G6B5,
    eswinD16R6G6B6OP1,
    eswinD16R6G6B6OP2,
    eswinD16R8G8B8OP1,
    eswinD16R8G8B8OP2,

    /* DPI */
    eswinD16CFG1,
    eswinD16CFG2,
    eswinD16CFG3,
    eswinD18CFG1,
    eswinD18CFG2,
    eswinD24    ,

    /* DP */
    eswinDPRGB565 = 0x200,
    eswinDPRGB666,
    eswinDPRGB888,
    eswinDPRGB101010,

    /* DP YUV */
    eswinDPYUV420B8CFG1= 0x300,
    eswinDPYUV420B8CFG2,
    eswinDPYUV422B8CFG1,
    eswinDPYUV422B8CFG2,
    eswinDPYUV444B8CFG1,
    eswinDPYUV444B8CFG2,
    eswinDPYUV420B10CFG1,
    eswinDPYUV420B10CFG2,
    eswinDPYUV422B10CFG1,
    eswinDPYUV422B10CFG2,
    eswinDPYUV444B10CFG1,
    eswinDPYUV444B10CFG2,
}eswin_format_type;

 /*
  * Frame buffer mode.
  * Used in eswin_conf_framebuffer_set_config()
  */
typedef enum _eswin_tiling_type {
    eswinLINEAR,
    eswinTILED,
}eswin_tiling_type;

 /*
  * Output type selection.
  * Used in eswin_conf_output_select()
  */
typedef enum _eswin_output_type {
    eswinDBI,
    eswinDPI,
    eswinDP,
}eswin_output_type;

 /*
  * DBI type.
  * Used in eswin_conf_output_dbi_set_config()
  */
typedef enum _eswin_dbi_type {
    eswinDBI_AFIXED,
    eswinDBI_ACLOCK,
    eswinDBI_B,
}eswin_dbi_type;

 /*
  * DBI command flag
  * Used in eswin_conf_output_dbi_set_command()
  */
typedef enum _eswin_dbi_command_type {
    eswinDBI_COMMAND_ADDRESS,
    eswinDBI_COMMAND_MEM,
    eswinDBI_COMMAND_DATA,
}eswin_dbi_command_type;

 /*
  * Cursor type.
  * Used in eswin_conf_cursor_set_type()
  */
typedef enum _eswin_cursor_type {
    eswinCURSOR_ARGB,
    eswinCURSOR_MASK,
}eswin_cursor_type;

 /*
  * Cursor size type.
  * Used in eswin_conf_cursor_set_size()
  */
typedef enum _eswin_cursor_size_type {
    eswinCURSOR_32x32,
    eswinCURSOR_64x64,
    eswinCURSOR_128x128,
    eswinCURSOR_256x256,
}eswin_cursor_size_type;

/*
  * csc moulde switch type.
  */
typedef enum _eswin_csc_moudle {
    /* Unsupport programmable csc moudle */
    eswinCSC_NONE,

    eswinCSC_RGB2RGB,
    eswinCSC_YUV2RGB,
    eswinCSC_RGB2YUV,
}eswin_csc_moudle;

/*
 * The enlarge value for 3D LUT.
 */
typedef enum _eswin_lut_enlarge {
    eswinLUT_ENLARGE0,
    eswinLUT_ENLARGE9,
    eswinLUT_ENLARGE10,
}eswin_lut_enlarge;

/*
 * Core type
 */
typedef enum _eswin_core_type {
    HW_VC8000D = 0,
    HW_VC8000DJ,
    HW_BIGOCEAN,
    HW_VCMD,
    HW_MMU,
    HW_DEC400,
    HW_L2CACHE,
    HW_SHAPER,
    HW_NOC,
    HW_AXIFE,
    HW_DC8000,
    HW_CORE_MAX    /* max number of cores supported */
}eswin_core_type;

/************************************************************************/
typedef enum _eswin_status_type {
    eswinSTATUS_HEAP_CORRUPTED = -7,
    eswinSTATUS_OUT_OF_RESOURCES = -6,
    eswinSTATUS_TIMEOUT = -5,
    eswinSTATUS_NOT_SUPPORT = -4,
    eswinSTATUS_OOM = -3,
    eswinSTATUS_FAILED = -2,
    eswinSTATUS_INVALID_ARGUMENTS = -1,

    eswinSTATUS_OK = 0,
}eswin_status_type, eswinSTATUS;

typedef enum _eswin_file_type {
    eswinFILE_BINARY = 0x001,

    eswinFILE_READ   = 0x010,
    eswinFILE_WRITE  = 0x100,

    eswinFILE_READB  = eswinFILE_READ  | eswinFILE_BINARY,
    eswinFILE_WRITEB = eswinFILE_WRITE | eswinFILE_BINARY,
}eswin_file_type;

typedef enum _eswin_test_type {
    eswinTEST_NORMAL          = 0x0,
    eswinTEST_MULTIFRAME      = 0x1,
    eswinTEST_VIDEOSTRESS     = 0x2,
    eswinTEST_OVERLAYSTRESS   = 0x3,
}eswin_test_type;

typedef enum _eswin_mmu_mode {
    eswinMMU_1KMODE_4KPAGE,
    eswinMMU_1KMODE_64KPAGE,
    eswinMMU_1KMODE_1MPAGE,
    eswinMMU_1KMODE_16MPAGE,
    eswinMMU_4KMODE_4KPAGE,
    eswinMMU_4KMODE_64KPAGE
}eswin_mmu_mode;

typedef enum _eswin_pool_type {
    gcvPOOL_CONTIGUOUS,
    gcvPOOL_NON_CONTIGUOUS,
    gcvPOOL_DEFAULT,
}gcvPOOL;

typedef int                     gctBOOL;
typedef int                     gctINT;
typedef unsigned int            gctUINT;
typedef unsigned char           gctUINT8;
typedef gctUINT8 *              gctUINT8_PTR;
typedef signed short            gctINT16;
typedef unsigned short          gctUINT16;
typedef gctUINT16 *             gctUINT16_PTR;
typedef signed int              gctINT32;
typedef unsigned int            gctUINT32;
typedef unsigned long long      gctUINT64;
typedef gctUINT32 *             gctUINT32_PTR;
typedef unsigned long           gctSIZE_T;
typedef char                    gctCHAR;
typedef float                   gctFLOAT;
typedef double                  gctDOUBLE;

typedef void                    gctVOID;
typedef void *                  gctPOINTER;

typedef struct _eswin_interface_query_chip_identity * eswin_interface_query_chip_identity_ptr;
typedef struct _eswin_interface_query_chip_identity {
    /* Chip model. */
    dcCHIPMODEL chipModel;
    /* Revision value.*/
    gctUINT32 chipRevision;
    /* Revision value.*/
    gctUINT32 chipPatchRevision;
    /* Chip date. */
    gctUINT32 chipInfo;
    /* Product ID */
    gctUINT32 productID;
    /* ECO ID. */
    gctUINT32 ecoID;
    /* Customer ID. */
    gctUINT32 customerID;
    /* Product Data */
    gctUINT32 productDate;
    /* Product time */
    gctUINT32 productTime;

}eswin_interface_query_chip_identity;

typedef enum _eswinFLATMAP_FLAG
{
    eswinFLATMAP_DIRECT,
    eswinFLATMAP_SHIFT,
}eswinFLATMAP_FLAG;

typedef struct _eswinFLAT_MAPPING_RANGE
{
    gctUINT64 start;
    gctUINT64 end;
    gctUINT32 size;
    eswinFLATMAP_FLAG flag;
}eswinFLAT_MAPPING_RANGE;

#endif
