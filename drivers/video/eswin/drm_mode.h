/*
 * Copyright (c) 2007 Dave Airlie <airlied@linux.ie>
 * Copyright (c) 2007 Jakob Bornecrantz <wallbraker@gmail.com>
 * Copyright (c) 2008 Red Hat Inc.
 * Copyright (c) 2007-2008 Tungsten Graphics, Inc., Cedar Park, TX., USA
 * Copyright (c) 2007-2008 Intel Corporation
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
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */
#ifndef _DRM_MODE_H
#define _DRM_MODE_H

/* drm mode 4k and 3d */
#define DRM_MODE_FLAG_420_MASK                 (0x03 << 23)
#define  DRM_MODE_FLAG_420                     BIT(23)
#define  DRM_MODE_FLAG_420_ONLY                BIT(24)

#define DRM_MODE_FLAG_3D_MASK                  (0x1f << 14)
#define  DRM_MODE_FLAG_3D_NONE                 (0 << 14)
#define  DRM_MODE_FLAG_3D_FRAME_PACKING        BIT(14)
#define  DRM_MODE_FLAG_3D_FIELD_ALTERNATIVE    (2 << 14)
#define  DRM_MODE_FLAG_3D_LINE_ALTERNATIVE     (3 << 14)
#define  DRM_MODE_FLAG_3D_SIDE_BY_SIDE_FULL    (4 << 14)
#define  DRM_MODE_FLAG_3D_L_DEPTH              (5 << 14)
#define  DRM_MODE_FLAG_3D_L_DEPTH_GFX_GFX_DEPTH (6 << 14)
#define  DRM_MODE_FLAG_3D_TOP_AND_BOTTOM       (7 << 14)
#define  DRM_MODE_FLAG_3D_SIDE_BY_SIDE_HALF    (8 << 14)
#endif