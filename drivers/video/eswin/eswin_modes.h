/*
 * Copyright © 2006 Keith Packard
 * Copyright © 2007-2008 Dave Airlie
 * Copyright © 2007-2008 Intel Corporation
 *   Jesse Barnes <jesse.barnes@intel.com>
 * Copyright © 2014 Intel Corporation
 *   Daniel Vetter <daniel.vetter@ffwll.ch>
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
#ifndef __ES_MODES_H__
#define __ES_MODES_H__

#define DRM_MODE(t, c, hd, hss, hse, ht, vd, vss, vse, vt, vs, f) \
    .clock = (c), .type = (t),\
    .hdisplay = (hd), .hsync_start = (hss), .hsync_end = (hse), \
    .htotal = (ht), .vdisplay = (vd), \
    .vsync_start = (vss), .vsync_end = (vse), .vtotal = (vt), \
    .vscan = (vs), .flags = (f)

struct drm_display_mode *drm_mode_create(void);
void drm_mode_destroy(struct drm_display_mode *mode);
struct drm_display_mode *drm_cvt_mode(int hdisplay, int vdisplay, int vrefresh,
                      bool reduced, bool interlaced,
                      bool margins);
bool drm_mode_equal_no_clocks_no_stereo(const struct drm_display_mode *mode1,
                    const struct drm_display_mode *mode2);
bool drm_mode_equal_no_clocks(const struct drm_display_mode *mode1,
                     const struct drm_display_mode *mode2);
struct drm_display_mode *drm_gtf_mode_complex(int hdisplay, int vdisplay,
             int vrefresh, bool interlaced, int margins,
             int GTF_M, int GTF_2C, int GTF_K, int GTF_2J);
struct drm_display_mode *drm_gtf_mode(int hdisplay, int vdisplay, int vrefresh,
             bool interlaced, int margins);

#endif