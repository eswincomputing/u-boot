/*
 * Copyright © 1997-2003 by The XFree86 Project, Inc.
 * Copyright © 2007 Dave Airlie
 * Copyright © 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 * Copyright 2005-2006 Luc Verhaegen
 * Copyright (c) 2001, Andy Ritger  aritger@nvidia.com
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
 *
 * Except as contained in this notice, the name of the copyright holder(s)
 * and author(s) shall not be used in advertising or otherwise to promote
 * the sale, use or other dealings in this Software without prior written
 * authorization from the copyright holder(s) and author(s).
 */

#include <linux/ctype.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <drm_modes.h>
#include "eswin_modes.h"
#include "drm_mode.h"

/**
 * drm_mode_create - create a new display mode
 *
 * Create a new, cleared drm_display_mode.
 *
 * Returns:
 * Pointer to new mode on success, NULL on error.
 */
struct drm_display_mode *drm_mode_create(void)
{
    struct drm_display_mode *nmode;

    nmode = malloc(sizeof(struct drm_display_mode));
    memset(nmode, 0, sizeof(struct drm_display_mode));
    if (!nmode)
        return NULL;

    return nmode;
}

/**
 * drm_mode_destroy - remove a mode
 * @mode: mode to remove
 *
 */
void drm_mode_destroy(struct drm_display_mode *mode)
{
    if (!mode)
        return;

    free(mode);
}

/**
 * drm_cvt_mode -create a modeline based on the CVT algorithm
 * @hdisplay: hdisplay size
 * @vdisplay: vdisplay size
 * @vrefresh: vrefresh rate
 * @reduced: whether to use reduced blanking
 * @interlaced: whether to compute an interlaced mode
 * @margins: whether to add margins (borders)
 *
 * This function is called to generate the modeline based on CVT algorithm
 * according to the hdisplay, vdisplay, vrefresh.
 * It is based from the VESA(TM) Coordinated Video Timing Generator by
 * Graham Loveridge April 9, 2003 available at
 * http://www.elo.utfsm.cl/~elo212/docs/CVTd6r1.xls
 *
 * And it is copied from xf86CVTmode in xserver/hw/xfree86/modes/xf86cvt.c.
 * What I have done is to translate it by using integer calculation.
 *
 * Returns:
 * The modeline based on the CVT algorithm stored in a drm_display_mode object.
 * The display mode object is allocated with drm_mode_create(). Returns NULL
 * when no mode could be allocated.
 */
struct drm_display_mode *drm_cvt_mode(int hdisplay, int vdisplay, int vrefresh,
                      bool reduced, bool interlaced,
                      bool margins)
{
#define HV_FACTOR           1000
    /* 1) top/bottom margin size (% of height) - default: 1.8, */
#define CVT_MARGIN_PERCENTAGE       18
    /* 2) character cell horizontal granularity (pixels) - default 8 */
#define CVT_H_GRANULARITY       8
    /* 3) Minimum vertical porch (lines) - default 3 */
#define CVT_MIN_V_PORCH         3
    /* 4) Minimum number of vertical back porch lines - default 6 */
#define CVT_MIN_V_BPORCH        6
    /* Pixel Clock step (kHz) */
#define CVT_CLOCK_STEP          250
    struct drm_display_mode *drm_mode;
    unsigned int vfieldrate, hperiod;
    int hdisplay_rnd, hmargin, vdisplay_rnd, vmargin, vsync;
    int interlace;

    /* allocate the drm_display_mode structure. If failure, we will
     * return directly
     */
    drm_mode = drm_mode_create();
    if (!drm_mode)
        return NULL;

    /* the CVT default refresh rate is 60Hz */
    if (!vrefresh)
        vrefresh = 60;

    /* the required field fresh rate */
    if (interlaced)
        vfieldrate = vrefresh * 2;
    else
        vfieldrate = vrefresh;

    /* horizontal pixels */
    hdisplay_rnd = hdisplay - (hdisplay % CVT_H_GRANULARITY);

    /* determine the left&right borders */
    hmargin = 0;
    if (margins) {
        hmargin = hdisplay_rnd * CVT_MARGIN_PERCENTAGE / 1000;
        hmargin -= hmargin % CVT_H_GRANULARITY;
    }
    /* find the total active pixels */
    drm_mode->hdisplay = hdisplay_rnd + 2 * hmargin;

    /* find the number of lines per field */
    if (interlaced)
        vdisplay_rnd = vdisplay / 2;
    else
        vdisplay_rnd = vdisplay;

    /* find the top & bottom borders */
    vmargin = 0;
    if (margins)
        vmargin = vdisplay_rnd * CVT_MARGIN_PERCENTAGE / 1000;

    drm_mode->vdisplay = vdisplay + 2 * vmargin;

    /* Interlaced */
    if (interlaced)
        interlace = 1;
    else
        interlace = 0;

    /* Determine VSync Width from aspect ratio */
    if (!(vdisplay % 3) && ((vdisplay * 4 / 3) == hdisplay))
        vsync = 4;
    else if (!(vdisplay % 9) && ((vdisplay * 16 / 9) == hdisplay))
        vsync = 5;
    else if (!(vdisplay % 10) && ((vdisplay * 16 / 10) == hdisplay))
        vsync = 6;
    else if (!(vdisplay % 4) && ((vdisplay * 5 / 4) == hdisplay))
        vsync = 7;
    else if (!(vdisplay % 9) && ((vdisplay * 15 / 9) == hdisplay))
        vsync = 7;
    else /* custom */
        vsync = 10;

    if (!reduced) {
        /* simplify the GTF calculation */
        /* 4) Minimum time of vertical sync + back porch interval
         * default 550.0
         */
        int tmp1, tmp2;
#define CVT_MIN_VSYNC_BP    550
        /* 3) Nominal HSync width (% of line period) - default 8 */
#define CVT_HSYNC_PERCENTAGE    8
        unsigned int hblank_percentage;
        int vsyncandback_porch, hblank;

        /* estimated the horizontal period */
        tmp1 = HV_FACTOR * 1000000  -
                CVT_MIN_VSYNC_BP * HV_FACTOR * vfieldrate;
        tmp2 = (vdisplay_rnd + 2 * vmargin + CVT_MIN_V_PORCH) * 2 +
                interlace;
        hperiod = tmp1 * 2 / (tmp2 * vfieldrate);

        tmp1 = CVT_MIN_VSYNC_BP * HV_FACTOR / hperiod + 1;
        /* 9. Find number of lines in sync + backporch */
        if (tmp1 < (vsync + CVT_MIN_V_PORCH))
            vsyncandback_porch = vsync + CVT_MIN_V_PORCH;
        else
            vsyncandback_porch = tmp1;
        /* 10. Find number of lines in back porch
         *      vback_porch = vsyncandback_porch - vsync;
         */
        drm_mode->vtotal = vdisplay_rnd + 2 * vmargin +
                vsyncandback_porch + CVT_MIN_V_PORCH;
        /* 5) Definition of Horizontal blanking time limitation */
        /* Gradient (%/kHz) - default 600 */
#define CVT_M_FACTOR    600
        /* Offset (%) - default 40 */
#define CVT_C_FACTOR    40
        /* Blanking time scaling factor - default 128 */
#define CVT_K_FACTOR    128
        /* Scaling factor weighting - default 20 */
#define CVT_J_FACTOR    20
#define CVT_M_PRIME (CVT_M_FACTOR * CVT_K_FACTOR / 256)
#define CVT_C_PRIME ((CVT_C_FACTOR - CVT_J_FACTOR) * CVT_K_FACTOR / 256 + \
             CVT_J_FACTOR)
        /* 12. Find ideal blanking duty cycle from formula */
        hblank_percentage = CVT_C_PRIME * HV_FACTOR - CVT_M_PRIME *
                    hperiod / 1000;
        /* 13. Blanking time */
        if (hblank_percentage < 20 * HV_FACTOR)
            hblank_percentage = 20 * HV_FACTOR;
        hblank = drm_mode->hdisplay * hblank_percentage /
             (100 * HV_FACTOR - hblank_percentage);
        hblank -= hblank % (2 * CVT_H_GRANULARITY);
        /* 14. find the total pixels per line */
        drm_mode->htotal = drm_mode->hdisplay + hblank;
        drm_mode->hsync_end = drm_mode->hdisplay + hblank / 2;
        drm_mode->hsync_start = drm_mode->hsync_end -
            (drm_mode->htotal * CVT_HSYNC_PERCENTAGE) / 100;
        drm_mode->hsync_start += CVT_H_GRANULARITY -
            drm_mode->hsync_start % CVT_H_GRANULARITY;
        /* fill the Vsync values */
        drm_mode->vsync_start = drm_mode->vdisplay + CVT_MIN_V_PORCH;
        drm_mode->vsync_end = drm_mode->vsync_start + vsync;
    } else {
        /* Reduced blanking */
        /* Minimum vertical blanking interval time - default 460 */
#define CVT_RB_MIN_VBLANK   460
        /* Fixed number of clocks for horizontal sync */
#define CVT_RB_H_SYNC       32
        /* Fixed number of clocks for horizontal blanking */
#define CVT_RB_H_BLANK      160
        /* Fixed number of lines for vertical front porch - default 3*/
#define CVT_RB_VFPORCH      3
        int vbilines;
        int tmp1, tmp2;
        /* 8. Estimate Horizontal period. */
        tmp1 = HV_FACTOR * 1000000 -
            CVT_RB_MIN_VBLANK * HV_FACTOR * vfieldrate;
        tmp2 = vdisplay_rnd + 2 * vmargin;
        hperiod = tmp1 / (tmp2 * vfieldrate);
        /* 9. Find number of lines in vertical blanking */
        vbilines = CVT_RB_MIN_VBLANK * HV_FACTOR / hperiod + 1;
        /* 10. Check if vertical blanking is sufficient */
        if (vbilines < (CVT_RB_VFPORCH + vsync + CVT_MIN_V_BPORCH))
            vbilines = CVT_RB_VFPORCH + vsync + CVT_MIN_V_BPORCH;
        /* 11. Find total number of lines in vertical field */
        drm_mode->vtotal = vdisplay_rnd + 2 * vmargin + vbilines;
        /* 12. Find total number of pixels in a line */
        drm_mode->htotal = drm_mode->hdisplay + CVT_RB_H_BLANK;
        /* Fill in HSync values */
        drm_mode->hsync_end = drm_mode->hdisplay + CVT_RB_H_BLANK / 2;
        drm_mode->hsync_start = drm_mode->hsync_end - CVT_RB_H_SYNC;
        /* Fill in VSync values */
        drm_mode->vsync_start = drm_mode->vdisplay + CVT_RB_VFPORCH;
        drm_mode->vsync_end = drm_mode->vsync_start + vsync;
    }
    /* 15/13. Find pixel clock frequency (kHz for xf86) */
    drm_mode->clock = drm_mode->htotal * HV_FACTOR * 1000 / hperiod;
    drm_mode->clock -= drm_mode->clock % CVT_CLOCK_STEP;
    /* 18/16. Find actual vertical frame frequency */
    /* ignore - just set the mode flag for interlaced */
    if (interlaced) {
        drm_mode->vtotal *= 2;
        drm_mode->flags |= DRM_MODE_FLAG_INTERLACE;
    }

    if (reduced)
        drm_mode->flags |= (DRM_MODE_FLAG_PHSYNC |
                    DRM_MODE_FLAG_NVSYNC);
    else
        drm_mode->flags |= (DRM_MODE_FLAG_PVSYNC |
                    DRM_MODE_FLAG_NHSYNC);

    return drm_mode;
}

/**
 * drm_mode_equal_no_clocks_no_stereo - test modes for equality
 * @mode1: first mode
 * @mode2: second mode
 *
 * Check to see if @mode1 and @mode2 are equivalent, but
 * don't check the pixel clocks nor the stereo layout.
 *
 * Returns:
 * True if the modes are equal, false otherwise.
 */

bool drm_mode_equal_no_clocks_no_stereo(const struct drm_display_mode *mode1,
                    const struct drm_display_mode *mode2)
{
    unsigned int flags_mask =
        ~(DRM_MODE_FLAG_3D_MASK | DRM_MODE_FLAG_420_MASK);

    if (mode1->hdisplay == mode2->hdisplay &&
        mode1->hsync_start == mode2->hsync_start &&
        mode1->hsync_end == mode2->hsync_end &&
        mode1->htotal == mode2->htotal &&
        mode1->vdisplay == mode2->vdisplay &&
        mode1->vsync_start == mode2->vsync_start &&
        mode1->vsync_end == mode2->vsync_end &&
        mode1->vtotal == mode2->vtotal &&
        mode1->vscan == mode2->vscan &&
        (mode1->flags & flags_mask) == (mode2->flags & flags_mask))
        return true;

    return false;
}

/**
 * drm_mode_equal_no_clocks - test modes for equality
 * @mode1: first mode
 * @mode2: second mode
 *
 * Check to see if @mode1 and @mode2 are equivalent, but
 * don't check the pixel clocks.
 *
 * Returns:
 * True if the modes are equal, false otherwise.
 */
bool drm_mode_equal_no_clocks(const struct drm_display_mode *mode1,
                     const struct drm_display_mode *mode2)
{
    if ((mode1->flags & DRM_MODE_FLAG_3D_MASK) !=
        (mode2->flags & DRM_MODE_FLAG_3D_MASK))
        return false;

    return drm_mode_equal_no_clocks_no_stereo(mode1, mode2);
}

struct drm_display_mode *
drm_gtf_mode_complex(int hdisplay, int vdisplay,
             int vrefresh, bool interlaced, int margins,
             int GTF_M, int GTF_2C, int GTF_K, int GTF_2J)
{   /* 1) top/bottom margin size (% of height) - default: 1.8, */
#define GTF_MARGIN_PERCENTAGE       18
    /* 2) character cell horizontal granularity (pixels) - default 8 */
#define GTF_CELL_GRAN           8
    /* 3) Minimum vertical porch (lines) - default 3 */
#define GTF_MIN_V_PORCH         1
    /* width of vsync in lines */
#define V_SYNC_RQD          3
    /* width of hsync as % of total line */
#define H_SYNC_PERCENT          8
    /* min time of vsync + back porch (microsec) */
#define MIN_VSYNC_PLUS_BP       550
    /* C' and M' are part of the Blanking Duty Cycle computation */
#define GTF_C_PRIME ((((GTF_2C - GTF_2J) * GTF_K / 256) + GTF_2J) / 2)
#define GTF_M_PRIME (GTF_K * GTF_M / 256)
    struct drm_display_mode *drm_mode;
    unsigned int hdisplay_rnd, vdisplay_rnd, vfieldrate_rqd;
    int top_margin, bottom_margin;
    int interlace;
    unsigned int hfreq_est;
    int vsync_plus_bp;
    unsigned int vtotal_lines;
    int left_margin, right_margin;
    unsigned int total_active_pixels, ideal_duty_cycle;
    unsigned int hblank, total_pixels, pixel_freq;
    int hsync, hfront_porch, vodd_front_porch_lines;
    unsigned int tmp1, tmp2;

    drm_mode = drm_mode_create();
    if (!drm_mode)
        return NULL;

    /* 1. In order to give correct results, the number of horizontal
     * pixels requested is first processed to ensure that it is divisible
     * by the character size, by rounding it to the nearest character
     * cell boundary:
     */
    hdisplay_rnd = (hdisplay + GTF_CELL_GRAN / 2) / GTF_CELL_GRAN;
    hdisplay_rnd = hdisplay_rnd * GTF_CELL_GRAN;

    /* 2. If interlace is requested, the number of vertical lines assumed
     * by the calculation must be halved, as the computation calculates
     * the number of vertical lines per field.
     */
    if (interlaced)
        vdisplay_rnd = vdisplay / 2;
    else
        vdisplay_rnd = vdisplay;

    /* 3. Find the frame rate required: */
    if (interlaced)
        vfieldrate_rqd = vrefresh * 2;
    else
        vfieldrate_rqd = vrefresh;

    /* 4. Find number of lines in Top margin: */
    top_margin = 0;
    if (margins)
        top_margin = (vdisplay_rnd * GTF_MARGIN_PERCENTAGE + 500) /
                1000;
    /* 5. Find number of lines in bottom margin: */
    bottom_margin = top_margin;

    /* 6. If interlace is required, then set variable interlace: */
    if (interlaced)
        interlace = 1;
    else
        interlace = 0;

    /* 7. Estimate the Horizontal frequency */
    {
        tmp1 = (1000000  - MIN_VSYNC_PLUS_BP * vfieldrate_rqd) / 500;
        tmp2 = (vdisplay_rnd + 2 * top_margin + GTF_MIN_V_PORCH) *
                2 + interlace;
        hfreq_est = (tmp2 * 1000 * vfieldrate_rqd) / tmp1;
    }

    /* 8. Find the number of lines in V sync + back porch */
    /* [V SYNC+BP] = RINT(([MIN VSYNC+BP] * hfreq_est / 1000000)) */
    vsync_plus_bp = MIN_VSYNC_PLUS_BP * hfreq_est / 1000;
    vsync_plus_bp = (vsync_plus_bp + 500) / 1000;
    /*  9. Find the number of lines in V back porch alone:
     *  vback_porch = vsync_plus_bp - V_SYNC_RQD;
     */
    /*  10. Find the total number of lines in Vertical field period: */
    vtotal_lines = vdisplay_rnd + top_margin + bottom_margin +
            vsync_plus_bp + GTF_MIN_V_PORCH;
    /*  11. Estimate the Vertical field frequency:
     *  vfieldrate_est = hfreq_est / vtotal_lines;
     */

    /*  12. Find the actual horizontal period:
     *  hperiod = 1000000 / (vfieldrate_rqd * vtotal_lines);
     */
    /*  13. Find the actual Vertical field frequency:
     *  vfield_rate = hfreq_est / vtotal_lines;
     */
    /*  14. Find the Vertical frame frequency:
     *  if (interlaced)
     *      vframe_rate = vfield_rate / 2;
     *  else
     *      vframe_rate = vfield_rate;
     */
    /*  15. Find number of pixels in left margin: */
    if (margins)
        left_margin = (hdisplay_rnd * GTF_MARGIN_PERCENTAGE + 500) /
                1000;
    else
        left_margin = 0;

    /* 16.Find number of pixels in right margin: */
    right_margin = left_margin;
    /* 17.Find total number of active pixels in image and left and right */
    total_active_pixels = hdisplay_rnd + left_margin + right_margin;
    /* 18.Find the ideal blanking duty cycle from blanking duty cycle */
    ideal_duty_cycle = GTF_C_PRIME * 1000 -
                (GTF_M_PRIME * 1000000 / hfreq_est);
    /* 19.Find the number of pixels in the blanking time to the nearest
     * double character cell:
     */
    hblank = total_active_pixels * ideal_duty_cycle /
            (100000 - ideal_duty_cycle);
    hblank = (hblank + GTF_CELL_GRAN) / (2 * GTF_CELL_GRAN);
    hblank = hblank * 2 * GTF_CELL_GRAN;
    /* 20.Find total number of pixels: */
    total_pixels = total_active_pixels + hblank;
    /* 21.Find pixel clock frequency: */
    pixel_freq = total_pixels * hfreq_est / 1000;
    /* Stage 1 computations are now complete; I should really pass
     * the results to another function and do the Stage 2 computations,
     * but I only need a few more values so I'll just append the
     * computations here for now
     */

    /* 17. Find the number of pixels in the horizontal sync period: */
    hsync = H_SYNC_PERCENT * total_pixels / 100;
    hsync = (hsync + GTF_CELL_GRAN / 2) / GTF_CELL_GRAN;
    hsync = hsync * GTF_CELL_GRAN;
    /* 18. Find the number of pixels in horizontal front porch period */
    hfront_porch = hblank / 2 - hsync;
    /*  36. Find the number of lines in the odd front porch period: */
    vodd_front_porch_lines = GTF_MIN_V_PORCH;

    /* finally, pack the results in the mode struct */
    drm_mode->hdisplay = hdisplay_rnd;
    drm_mode->hsync_start = hdisplay_rnd + hfront_porch;
    drm_mode->hsync_end = drm_mode->hsync_start + hsync;
    drm_mode->htotal = total_pixels;
    drm_mode->vdisplay = vdisplay_rnd;
    drm_mode->vsync_start = vdisplay_rnd + vodd_front_porch_lines;
    drm_mode->vsync_end = drm_mode->vsync_start + V_SYNC_RQD;
    drm_mode->vtotal = vtotal_lines;

    drm_mode->clock = pixel_freq;

    if (interlaced) {
        drm_mode->vtotal *= 2;
        drm_mode->flags |= DRM_MODE_FLAG_INTERLACE;
    }

    if (GTF_M == 600 && GTF_2C == 80 && GTF_K == 128 && GTF_2J == 40)
        drm_mode->flags = DRM_MODE_FLAG_NHSYNC | DRM_MODE_FLAG_PVSYNC;
    else
        drm_mode->flags = DRM_MODE_FLAG_PHSYNC | DRM_MODE_FLAG_NVSYNC;

    return drm_mode;
}

/**
 * drm_gtf_mode - create the mode based on the GTF algorithm
 * @hdisplay: hdisplay size
 * @vdisplay: vdisplay size
 * @vrefresh: vrefresh rate.
 * @interlaced: whether to compute an interlaced mode
 * @margins: desired margin (borders) size
 *
 * return the mode based on GTF algorithm
 *
 * This function is to create the mode based on the GTF algorithm.
 * Generalized Timing Formula is derived from:
 *  GTF Spreadsheet by Andy Morrish (1/5/97)
 *  available at http://www.vesa.org
 *
 * And it is copied from the file of xserver/hw/xfree86/modes/xf86gtf.c.
 * What I have done is to translate it by using integer calculation.
 * I also refer to the function of fb_get_mode in the file of
 * drivers/video/fbmon.c
 *
 * Standard GTF parameters:
 * M = 600
 * C = 40
 * K = 128
 * J = 20
 *
 * Returns:
 * The modeline based on the GTF algorithm stored in a drm_display_mode object.
 * The display mode object is allocated with drm_mode_create(). Returns NULL
 * when no mode could be allocated.
 */
struct drm_display_mode *
drm_gtf_mode(int hdisplay, int vdisplay, int vrefresh,
         bool interlaced, int margins)
{
    return drm_gtf_mode_complex(hdisplay, vdisplay, vrefresh,
                    interlaced, margins,
                    600, 40 * 2, 128, 20 * 2);
}