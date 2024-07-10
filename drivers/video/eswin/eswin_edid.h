/*
 * Copyright © 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE COPYRIGHT HOLDER(S) OR AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __ES_EDID_H_
#define __ES_EDID_H_

#include <div64.h>
#include <linux/types.h>
#include <drm_modes.h>
#include <i2c.h>
#include "drm_mode.h"
#include "drm_displayid.h"
#include "drm_connector.h"

#define MODE_LEN        240

#define CEA_EXT         0x02
#define VTB_EXT         0x10
#define DI_EXT          0x40
#define LS_EXT          0x50
#define MI_EXT          0x60
#define DISPLAYID_EXT   0x70

#define DDC_SEGMENT_ADDR       0x30
#define DDC_ADDR               0x50
#define HDMI_EDID_BLOCK_SIZE   128
#define SCDC_I2C_SLAVE_ADDRESS 0x54

struct est_timings {
    u8 t1;
    u8 t2;
    u8 mfg_rsvd;
} __packed;

/* 00=16:10, 01=4:3, 10=5:4, 11=16:9 */
#define EDID_TIMING_ASPECT_SHIFT 6
#define EDID_TIMING_ASPECT_MASK  (0x3 << EDID_TIMING_ASPECT_SHIFT)

/* need to add 60 */
#define EDID_TIMING_VFREQ_SHIFT  0
#define EDID_TIMING_VFREQ_MASK   (0x3f << EDID_TIMING_VFREQ_SHIFT)

struct std_timing {
    u8 hsize; /* need to multiply by 8 then add 248 */
    u8 vfreq_aspect;
} __packed;

struct detailed_pixel_timing {
    u8 hactive_lo;
    u8 hblank_lo;
    u8 hactive_hblank_hi;
    u8 vactive_lo;
    u8 vblank_lo;
    u8 vactive_vblank_hi;
    u8 hsync_offset_lo;
    u8 hsync_pulse_width_lo;
    u8 vsync_offset_pulse_width_lo;
    u8 hsync_vsync_offset_pulse_width_hi;
    u8 width_mm_lo;
    u8 height_mm_lo;
    u8 width_height_mm_hi;
    u8 hborder;
    u8 vborder;
    u8 misc;
} __packed;

/* If it's not pixel timing, it'll be one of the below */
struct detailed_data_string {
    u8 str[13];
} __packed;

struct detailed_data_monitor_range {
    u8 min_vfreq;
    u8 max_vfreq;
    u8 min_hfreq_khz;
    u8 max_hfreq_khz;
    u8 pixel_clock_mhz; /* need to multiply by 10 */
    u8 flags;
    union {
        struct {
            u8 reserved;
            u8 hfreq_start_khz; /* need to multiply by 2 */
            u8 c; /* need to divide by 2 */
            __le16 m;
            u8 k;
            u8 j; /* need to divide by 2 */
        } __packed gtf2;
        struct {
            u8 version;
            u8 data1; /* high 6 bits: extra clock resolution */
            u8 data2; /* plus low 2 of above: max hactive */
            u8 supported_aspects;
            u8 flags; /* preferred aspect and blanking support */
            u8 supported_scalings;
            u8 preferred_refresh;
        } __packed cvt;
    } formula;
} __packed;

struct detailed_data_wpindex {
    u8 white_yx_lo; /* Lower 2 bits each */
    u8 white_x_hi;
    u8 white_y_hi;
    u8 gamma; /* need to divide by 100 then add 1 */
} __packed;

struct detailed_data_color_point {
    u8 windex1;
    u8 wpindex1[3];
    u8 windex2;
    u8 wpindex2[3];
} __packed;

struct cvt_timing {
    u8 code[3];
} __packed;

struct detailed_non_pixel {
    u8 pad1;
    u8 type; /* ff=serial, fe=string, fd=monitor range, fc=monitor name
          * fb=color point data, fa=standard timing data,
          * f9=undefined, f8=mfg. reserved
          */
    u8 pad2;
    union {
        struct detailed_data_string str;
        struct detailed_data_monitor_range range;
        struct detailed_data_wpindex color;
        struct std_timing timings[6];
        struct cvt_timing cvt[4];
    } data;
} __packed;

#define EDID_DETAIL_EST_TIMINGS     0xf7
#define EDID_DETAIL_CVT_3BYTE       0xf8
#define EDID_DETAIL_COLOR_MGMT_DATA 0xf9
#define EDID_DETAIL_STD_MODES       0xfa
#define EDID_DETAIL_MONITOR_CPDATA  0xfb
#define EDID_DETAIL_MONITOR_NAME    0xfc
#define EDID_DETAIL_MONITOR_RANGE   0xfd
#define EDID_DETAIL_MONITOR_STRING  0xfe
#define EDID_DETAIL_MONITOR_SERIAL  0xff

struct detailed_timing {
    __le16 pixel_clock; /* need to multiply by 10 KHz */
    union {
        struct detailed_pixel_timing pixel_data;
        struct detailed_non_pixel other_data;
    } data;
} __packed;

#define DRM_EDID_INPUT_SERRATION_VSYNC (1 << 0)
#define DRM_EDID_INPUT_SYNC_ON_GREEN   (1 << 1)
#define DRM_EDID_INPUT_COMPOSITE_SYNC  (1 << 2)
#define DRM_EDID_INPUT_SEPARATE_SYNCS  (1 << 3)
#define DRM_EDID_INPUT_BLANK_TO_BLACK  (1 << 4)
#define DRM_EDID_INPUT_VIDEO_LEVEL     (3 << 5)
#define DRM_EDID_INPUT_DIGITAL         (1 << 7)
#define DRM_EDID_DIGITAL_DEPTH_MASK    (7 << 4)
#define DRM_EDID_DIGITAL_DEPTH_UNDEF   (0 << 4)
#define DRM_EDID_DIGITAL_DEPTH_6       (1 << 4)
#define DRM_EDID_DIGITAL_DEPTH_8       (2 << 4)
#define DRM_EDID_DIGITAL_DEPTH_10      (3 << 4)
#define DRM_EDID_DIGITAL_DEPTH_12      (4 << 4)
#define DRM_EDID_DIGITAL_DEPTH_14      (5 << 4)
#define DRM_EDID_DIGITAL_DEPTH_16      (6 << 4)
#define DRM_EDID_DIGITAL_DEPTH_RSVD    (7 << 4)
#define DRM_EDID_DIGITAL_TYPE_UNDEF    (0)
#define DRM_EDID_DIGITAL_TYPE_DVI      (1)
#define DRM_EDID_DIGITAL_TYPE_HDMI_A   (2)
#define DRM_EDID_DIGITAL_TYPE_HDMI_B   (3)
#define DRM_EDID_DIGITAL_TYPE_MDDI     (4)
#define DRM_EDID_DIGITAL_TYPE_DP       (5)

#define DRM_EDID_FEATURE_DEFAULT_GTF      (1 << 0)
#define DRM_EDID_FEATURE_PREFERRED_TIMING (1 << 1)
#define DRM_EDID_FEATURE_STANDARD_COLOR   (1 << 2)
/* If analog */
/* 00=mono, 01=rgb, 10=non-rgb, 11=unknown */
#define DRM_EDID_FEATURE_DISPLAY_TYPE     (3 << 3)
/* If digital */
#define DRM_EDID_FEATURE_COLOR_MASK       (3 << 3)
#define DRM_EDID_FEATURE_RGB              (0 << 3)
#define DRM_EDID_FEATURE_RGB_YCRCB444     (1 << 3)
#define DRM_EDID_FEATURE_RGB_YCRCB422     (2 << 3)
/* both 4:4:4 and 4:2:2 */
#define DRM_EDID_FEATURE_RGB_YCRCB        (3 << 3)

#define DRM_EDID_FEATURE_PM_ACTIVE_OFF    (1 << 5)
#define DRM_EDID_FEATURE_PM_SUSPEND       (1 << 6)
#define DRM_EDID_FEATURE_PM_STANDBY       (1 << 7)

#define DRM_EDID_HDMI_DC_48               (1 << 6)
#define DRM_EDID_HDMI_DC_36               (1 << 5)
#define DRM_EDID_HDMI_DC_30               (1 << 4)
#define DRM_EDID_HDMI_DC_Y444             (1 << 3)

/* YCBCR 420 deep color modes */
#define DRM_EDID_YCBCR420_DC_48           (1 << 2)
#define DRM_EDID_YCBCR420_DC_36           (1 << 1)
#define DRM_EDID_YCBCR420_DC_30           (1 << 0)
#define DRM_EDID_YCBCR420_DC_MASK (DRM_EDID_YCBCR420_DC_48 | \
                    DRM_EDID_YCBCR420_DC_36 | \
                    DRM_EDID_YCBCR420_DC_30)

struct edid {
    u8 header[8];
    /* Vendor & product info */
    u8 mfg_id[2];
    u8 prod_code[2];
    u32 serial; /* FIXME: byte order */
    u8 mfg_week;
    u8 mfg_year;
    /* EDID version */
    u8 version;
    u8 revision;
    /* Display info: */
    u8 input;
    u8 width_cm;
    u8 height_cm;
    u8 gamma;
    u8 features;
    /* Color characteristics */
    u8 red_green_lo;
    u8 black_white_lo;
    u8 red_x;
    u8 red_y;
    u8 green_x;
    u8 green_y;
    u8 blue_x;
    u8 blue_y;
    u8 white_x;
    u8 white_y;
    /* Est. timings and mfg rsvd timings*/
    struct est_timings established_timings;
    /* Standard timings 1-8*/
    struct std_timing standard_timings[8];
    /* Detailing timings 1-4 */
    struct detailed_timing detailed_timings[4];
    /* Number of 128 byte ext. blocks */
    u8 extensions;
    /* Checksum */
    u8 checksum;
} __packed;

enum base_output_format {
    DRM_HDMI_OUTPUT_DEFAULT_RGB, /* default RGB */
    DRM_HDMI_OUTPUT_YCBCR444, /* YCBCR 444 */
    DRM_HDMI_OUTPUT_YCBCR422, /* YCBCR 422 */
    DRM_HDMI_OUTPUT_YCBCR420, /* YCBCR 420 */
    /* (YCbCr444 > YCbCr422 > YCbCr420 > RGB) */
    DRM_HDMI_OUTPUT_YCBCR_HQ,
    /* (YCbCr420 > YCbCr422 > YCbCr444 > RGB) */
    DRM_HDMI_OUTPUT_YCBCR_LQ,
    DRM_HDMI_OUTPUT_INVALID, /* Guess what ? */
};

enum  base_output_depth {
    AUTOMATIC = 0,
    DEPTH_24BIT = 8,
    DEPTH_30BIT = 10,
};

struct base_overscan {
    unsigned int maxvalue;
    unsigned short leftscale;
    unsigned short rightscale;
    unsigned short topscale;
    unsigned short bottomscale;
};

struct base_drm_display_mode {
    int clock;      /* in kHz */
    int hdisplay;
    int hsync_start;
    int hsync_end;
    int htotal;
    int vdisplay;
    int vsync_start;
    int vsync_end;
    int vtotal;
    int vrefresh;
    int vscan;
    unsigned int flags;
    int picture_aspect_ratio;
};

struct base_screen_info {
    int type;
    struct base_drm_display_mode mode;  /* 52 bytes */
    enum base_output_format  format;    /* 4 bytes */
    enum base_output_depth depth;       /* 4 bytes */
    unsigned int feature;           /* 4 bytes */
};

struct base_disp_info {
    struct base_screen_info screen_list[5];
    struct base_overscan scan;      /* 12 bytes */
};

struct drm_display_mode;
struct display_timing;

struct hdmi_edid_data {
    struct drm_display_mode *preferred_mode;
    int modes;
    struct drm_display_mode *mode_buf;
    struct drm_display_info display_info;
};

struct ddc_adapter {
    int (*ddc_xfer)(struct ddc_adapter *adap, struct i2c_msg *msgs,
            int num);
};

int edid_get_drm_mode(u8 *buf, int buf_size, struct drm_display_mode *mode,
              int *panel_bits_per_colourp);
int drm_add_edid_modes(struct hdmi_edid_data *data, u8 *edid);
bool drm_detect_hdmi_monitor(struct edid *edid);
bool drm_detect_monitor_audio(struct edid *edid);
int do_cea_modes(struct hdmi_edid_data *data, const u8 *db, u8 len);
int drm_do_get_edid(struct ddc_adapter *adap, u8 *edid);
enum hdmi_quantization_range
drm_default_rgb_quant_range(struct drm_display_mode *mode);
u8 drm_scdc_readb(struct ddc_adapter *adap, u8 offset,
          u8 *value);
u8 drm_scdc_writeb(struct ddc_adapter *adap, u8 offset,
           u8 value);
void drm_mode_sort(struct hdmi_edid_data *edid_data);
int drm_mode_prune_invalid(struct hdmi_edid_data *edid_data);
void drm_rk_filter_whitelist(struct hdmi_edid_data *edid_data);
void drm_rk_select_mode(struct hdmi_edid_data *edid_data,
            struct base_screen_info *screen_info);

#endif /* __EDID_H_ */
