/*
 * Copyright (c) 2016 Intel Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that copyright
 * notice and this permission notice appear in supporting documentation, and
 * that the name of the copyright holders not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The copyright holders make no representations
 * about the suitability of this software for any purpose.  It is provided "as
 * is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 */

#ifndef __DRM_CONNECTOR_H__
#define __DRM_CONNECTOR_H__

#include <linux/types.h>

/**
 * struct drm_scrambling: sink's scrambling support.
 */
struct drm_scrambling {
    /**
     * @supported: scrambling supported for rates > 340 Mhz.
     */
    bool supported;
    /**
     * @low_rates: scrambling supported for rates <= 340 Mhz.
     */
    bool low_rates;
};

/**
 * struct drm_scdc - Information about scdc capabilities of a HDMI 2.0 sink
 *
 * Provides SCDC register support and capabilities related information on a
 * HDMI 2.0 sink. In case of a HDMI 1.4 sink, all parameter must be 0.
 */

struct drm_scdc {
    /**
     * @supported: status control & data channel present.
     */
    bool supported;
    /**
     * @read_request: sink is capable of generating scdc read request.
     */
    bool read_request;
    /**
     * @scrambling: sink's scrambling capabilities
     */
    struct drm_scrambling scrambling;
};

/**
 * struct drm_hdmi_info - runtime information about the connected HDMI sink
 *
 * Describes if a given display supports advanced HDMI 2.0 features.
 * This information is available in CEA-861-F extension blocks (like HF-VSDB).
 */
struct drm_hdmi_info {
    struct drm_scdc scdc;

    /**
     * @y420_vdb_modes: bitmap of modes which can support ycbcr420
     * output only (not normal RGB/YCBCR444/422 outputs). There are total
     * 107 VICs defined by CEA-861-F spec, so the size is 128 bits to map
     * upto 128 VICs;
     */
    unsigned long y420_vdb_modes[BITS_TO_LONGS(128)];

    /**
     * @y420_cmdb_modes: bitmap of modes which can support ycbcr420
     * output also, along with normal HDMI outputs. There are total 107
     * VICs defined by CEA-861-F spec, so the size is 128 bits to map upto
     * 128 VICs;
     */
    unsigned long y420_cmdb_modes[BITS_TO_LONGS(128)];

    /** @y420_cmdb_map: bitmap of SVD index, to extraxt vcb modes */
    u64 y420_cmdb_map;

    /** @y420_dc_modes: bitmap of deep color support index */
    u8 y420_dc_modes;
};

enum subpixel_order {
    subpixelunknown = 0,
    subpixelhorizontalrgb,
    subpixelhorizontalbgr,
    subpixelverticalrgb,
    subpixelverticalbgr,
    subpixelnone,
};

#define DRM_COLOR_FORMAT_RGB444		(1<<0)
#define DRM_COLOR_FORMAT_YCRCB444	(1<<1)
#define DRM_COLOR_FORMAT_YCRCB422	(1<<2)
#define DRM_COLOR_FORMAT_YCRCB420	(1<<3)

/*
 * Describes a given display (e.g. CRT or flat panel) and its limitations.
 */
struct drm_display_info {
    char name[32];

    /* Physical size */
    unsigned int width_mm;
    unsigned int height_mm;

    /* Clock limits FIXME: storage format */
    unsigned int min_vfreq, max_vfreq;
    unsigned int min_hfreq, max_hfreq;
    unsigned int pixel_clock;
    unsigned int bpc;

    enum subpixel_order subpixel_order;
    u32 color_formats;

    const u32 *bus_formats;
    unsigned int num_bus_formats;

    /**
     * @max_tmds_clock: Maximum TMDS clock rate supported by the
     * sink in kHz. 0 means undefined.
     */
    int max_tmds_clock;

    /**
     * @dvi_dual: Dual-link DVI sink?
     */
    bool dvi_dual;

    /* Mask of supported hdmi deep color modes */
    u8 edid_hdmi_dc_modes;

    u8 cea_rev;

    /**
     * @hdmi: advance features of a HDMI sink.
     */
    struct drm_hdmi_info hdmi;
};

#endif