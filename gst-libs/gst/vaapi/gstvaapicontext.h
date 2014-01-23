/*
 *  gstvaapicontext.h - VA context abstraction (private)
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2011-2014 Intel Corporation
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@intel.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public License
 *  as published by the Free Software Foundation; either version 2.1
 *  of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free
 *  Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301 USA
 */

#ifndef GST_VAAPI_CONTEXT_H
#define GST_VAAPI_CONTEXT_H

#include <gst/video/video-overlay-composition.h>
#include "gstvaapiobject.h"
#include "gstvaapiobject_priv.h"
#include "gstvaapiprofile.h"
#include "gstvaapidisplay.h"
#include "gstvaapisurface.h"
#include "gstvaapivideopool.h"

G_BEGIN_DECLS

#define GST_VAAPI_CONTEXT(obj) \
  ((GstVaapiContext *) (obj))

typedef struct _GstVaapiContextInfo GstVaapiContextInfo;
typedef struct _GstVaapiContext GstVaapiContext;
typedef struct _GstVaapiContextClass GstVaapiContextClass;

/**
 * GstVaapiContextInfo:
 *
 * Structure holding VA context info like encoded size, decoder
 * profile and entry-point to use, and maximum number of reference
 * frames reported by the bitstream.
 *
 * Note: @rc_mode is only valid for VA context used for encoding,
 * i.e. if @entrypoint is set to @GST_VAAPI_ENTRYPOINT_SLICE_ENCODE.
 */
struct _GstVaapiContextInfo
{
  GstVaapiProfile profile;
  GstVaapiEntrypoint entrypoint;
  GstVaapiRateControl rc_mode;
  guint width;
  guint height;
  guint ref_frames;
};

/**
 * GstVaapiContext:
 *
 * A VA context wrapper.
 */
struct _GstVaapiContext
{
  /*< private >*/
  GstVaapiObject parent_instance;

  GstVaapiContextInfo info;
  VAProfile va_profile;
  VAEntrypoint va_entrypoint;
  VAConfigID va_config;
  GPtrArray *surfaces;
  GstVaapiVideoPool *surfaces_pool;
  GPtrArray *overlays[2];
  guint overlay_id;
};

/**
 * GstVaapiContextClass:
 *
 * A VA context wrapper class.
 */
struct _GstVaapiContextClass
{
  /*< private >*/
  GstVaapiObjectClass parent_class;
};

G_GNUC_INTERNAL
GstVaapiContext *
gst_vaapi_context_new (GstVaapiDisplay * display,
    const GstVaapiContextInfo * cip);

G_GNUC_INTERNAL
gboolean
gst_vaapi_context_reset (GstVaapiContext * context,
    const GstVaapiContextInfo * new_cip);

G_GNUC_INTERNAL
GstVaapiID
gst_vaapi_context_get_id (GstVaapiContext * context);

G_GNUC_INTERNAL
GstVaapiSurfaceProxy *
gst_vaapi_context_get_surface_proxy (GstVaapiContext * context);

G_GNUC_INTERNAL
guint
gst_vaapi_context_get_surface_count (GstVaapiContext * context);

G_GNUC_INTERNAL
gboolean
gst_vaapi_context_apply_composition (GstVaapiContext * context,
    GstVideoOverlayComposition * composition);

G_GNUC_INTERNAL
gboolean
gst_vaapi_context_get_attribute (GstVaapiContext * context,
    VAConfigAttribType type, guint * out_value_ptr);

G_END_DECLS

#endif /* GST_VAAPI_CONTEXT_H */
