/*
 *  gstvaapitexture.c - VA texture abstraction
 *
 *  Copyright (C) 2010-2011 Splitted-Desktop Systems
 *    Author: Gwenole Beauchesne <gwenole.beauchesne@splitted-desktop.com>
 *  Copyright (C) 2012-2013 Intel Corporation
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

/**
 * SECTION:gstvaapitexture
 * @short_description: VA/GLX texture abstraction
 */

#include "sysdeps.h"
#include "gstvaapitexture.h"
#include "gstvaapitexture_priv.h"

#define DEBUG 1
#include "gstvaapidebug.h"

/* Ensure those symbols are actually defined in the resulting libraries */
#undef gst_vaapi_texture_ref
#undef gst_vaapi_texture_unref
#undef gst_vaapi_texture_replace

static void
gst_vaapi_texture_init (GstVaapiTexture * texture, guint texture_id,
    guint target, guint format, guint width, guint height)
{
  GST_VAAPI_OBJECT_ID (texture) = texture_id;
  texture->is_wrapped = texture_id != 0;
  texture->gl_target = target;
  texture->gl_format = format;
  texture->width = width;
  texture->height = height;
}

static inline gboolean
gst_vaapi_texture_allocate (GstVaapiTexture * texture)
{
  return GST_VAAPI_TEXTURE_GET_CLASS (texture)->allocate (texture);
}

GstVaapiTexture *
gst_vaapi_texture_new (const GstVaapiTextureClass * klass,
    GstVaapiDisplay * display, guint texture_id, guint target, guint format,
    guint width, guint height)
{
  GstVaapiTexture *texture;

  g_return_val_if_fail (target != 0, NULL);
  g_return_val_if_fail (format != 0, NULL);
  g_return_val_if_fail (width > 0, NULL);
  g_return_val_if_fail (height > 0, NULL);

  texture = gst_vaapi_object_new (GST_VAAPI_OBJECT_CLASS (klass), display);
  if (!texture)
    return NULL;

  gst_vaapi_texture_init (texture, texture_id, target, format, width, height);
  if (!gst_vaapi_texture_allocate (texture))
    goto error;
  return texture;

error:
  gst_vaapi_object_unref (texture);
  return NULL;
}

/**
 * gst_vaapi_texture_ref:
 * @texture: a #GstVaapiTexture
 *
 * Atomically increases the reference count of the given @texture by one.
 *
 * Returns: The same @texture argument
 */
GstVaapiTexture *
gst_vaapi_texture_ref (GstVaapiTexture * texture)
{
  return gst_vaapi_texture_ref_internal (texture);
}

/**
 * gst_vaapi_texture_unref:
 * @texture: a #GstVaapiTexture
 *
 * Atomically decreases the reference count of the @texture by one. If
 * the reference count reaches zero, the texture will be free'd.
 */
void
gst_vaapi_texture_unref (GstVaapiTexture * texture)
{
  gst_vaapi_texture_unref_internal (texture);
}

/**
 * gst_vaapi_texture_replace:
 * @old_texture_ptr: a pointer to a #GstVaapiTexture
 * @new_texture: a #GstVaapiTexture
 *
 * Atomically replaces the texture texture held in @old_texture_ptr
 * with @new_texture. This means that @old_texture_ptr shall reference
 * a valid texture. However, @new_texture can be NULL.
 */
void
gst_vaapi_texture_replace (GstVaapiTexture ** old_texture_ptr,
    GstVaapiTexture * new_texture)
{
  gst_vaapi_texture_replace_internal (old_texture_ptr, new_texture);
}

/**
 * gst_vaapi_texture_get_id:
 * @texture: a #GstVaapiTexture
 *
 * Returns the underlying texture id of the @texture.
 *
 * Return value: the underlying texture id of the @texture
 */
guint
gst_vaapi_texture_get_id (GstVaapiTexture * texture)
{
  g_return_val_if_fail (texture != NULL, 0);

  return GST_VAAPI_TEXTURE_ID (texture);
}

/**
 * gst_vaapi_texture_get_target:
 * @texture: a #GstVaapiTexture
 *
 * Returns the @texture target type
 *
 * Return value: the texture target
 */
guint
gst_vaapi_texture_get_target (GstVaapiTexture * texture)
{
  g_return_val_if_fail (texture != NULL, 0);

  return GST_VAAPI_TEXTURE_TARGET (texture);
}

/**
 * gst_vaapi_texture_get_format
 * @texture: a #GstVaapiTexture
 *
 * Returns the @texture format
 *
 * Return value: the texture format
 */
guint
gst_vaapi_texture_get_format (GstVaapiTexture * texture)
{
  g_return_val_if_fail (texture != NULL, 0);

  return GST_VAAPI_TEXTURE_FORMAT (texture);
}

/**
 * gst_vaapi_texture_get_width:
 * @texture: a #GstVaapiTexture
 *
 * Returns the @texture width.
 *
 * Return value: the texture width, in pixels
 */
guint
gst_vaapi_texture_get_width (GstVaapiTexture * texture)
{
  g_return_val_if_fail (texture != NULL, 0);

  return GST_VAAPI_TEXTURE_WIDTH (texture);
}

/**
 * gst_vaapi_texture_get_height:
 * @texture: a #GstVaapiTexture
 *
 * Returns the @texture height.
 *
 * Return value: the texture height, in pixels.
 */
guint
gst_vaapi_texture_get_height (GstVaapiTexture * texture)
{
  g_return_val_if_fail (texture != NULL, 0);

  return GST_VAAPI_TEXTURE_HEIGHT (texture);
}

/**
 * gst_vaapi_texture_get_size:
 * @texture: a #GstVaapiTexture
 * @width_ptr: return location for the width, or %NULL
 * @height_ptr: return location for the height, or %NULL
 *
 * Retrieves the dimensions of a #GstVaapiTexture.
 */
void
gst_vaapi_texture_get_size (GstVaapiTexture * texture,
    guint * width_ptr, guint * height_ptr)
{
  g_return_if_fail (texture != NULL);

  if (width_ptr)
    *width_ptr = GST_VAAPI_TEXTURE_WIDTH (texture);

  if (height_ptr)
    *height_ptr = GST_VAAPI_TEXTURE_HEIGHT (texture);
}

/**
 * gst_vaapi_texture_put_surface:
 * @texture: a #GstVaapiTexture
 * @surface: a #GstVaapiSurface
 * @flags: postprocessing flags. See #GstVaapiTextureRenderFlags
 *
 * Renders the @surface into the àtexture. The @flags specify how
 * de-interlacing (if needed), color space conversion, scaling and
 * other postprocessing transformations are performed.
 *
 * Return value: %TRUE on success
 */
gboolean
gst_vaapi_texture_put_surface (GstVaapiTexture * texture,
    GstVaapiSurface * surface, const GstVaapiRectangle * crop_rect, guint flags)
{
  const GstVaapiTextureClass *klass;
  GstVaapiRectangle rect;

  g_return_val_if_fail (texture != NULL, FALSE);
  g_return_val_if_fail (surface != NULL, FALSE);

  klass = GST_VAAPI_TEXTURE_GET_CLASS (texture);
  if (!klass)
    return FALSE;

  if (!crop_rect) {
    rect.x = 0;
    rect.y = 0;
    gst_vaapi_surface_get_size (surface, &rect.width, &rect.height);
    crop_rect = &rect;
  }
  return klass->put_surface (texture, surface, crop_rect, flags);
}
