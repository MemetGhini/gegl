/* This file is an image processing operation for GEGL
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (C) 2017 Ell
 *
 */

#include "config.h"
#include <glib/gi18n-lib.h>
#include <math.h>

#ifdef GEGL_PROPERTIES

property_double (x_period, _("X Period"), 256.0)
  description (_("Period for X axis"))
  value_range (0.0001, G_MAXDOUBLE)
  ui_range    (0.0001, 1000.0)
  ui_meta     ("unit", "pixel-distance")
  ui_meta     ("axis", "x")

property_double (y_period, _("Y Period"), 256.0)
  description (_("Period for Y axis"))
  value_range (0.0001, G_MAXDOUBLE)
  ui_range    (0.0001, 1000.0)
  ui_meta     ("unit", "pixel-distance")
  ui_meta     ("axis", "y")

property_double (x_amplitude, _("X Amplitude"), 0.25)
  description (_("Amplitude for X axis"))
  value_range (0.0, G_MAXDOUBLE)
  ui_range    (0.0, 1.0)
  ui_meta     ("axis", "x")

property_double (y_amplitude, _("Y Amplitude"), 0.25)
  description (_("Amplitude for Y axis"))
  value_range (0.0, G_MAXDOUBLE)
  ui_range    (0.0, 1.0)
  ui_meta     ("axis", "y")

property_double (x_phase, _("X Phase"), 0.0)
  description (_("Phase for X axis"))
  value_range (-G_MAXDOUBLE, G_MAXDOUBLE)
  ui_range    (-10000.0, 10000.0)
  ui_meta     ("unit", "pixel-distance")
  ui_meta     ("axis", "x")

property_double (y_phase, _("Y Phase"), 0.0)
  description (_("Phase for Y axis"))
  value_range (-G_MAXDOUBLE, G_MAXDOUBLE)
  ui_range    (-10000.0, 10000.0)
  ui_meta     ("unit", "pixel-distance")
  ui_meta     ("axis", "y")

property_double (angle, _("Angle"), 90.0)
  description(_("Axis separation angle"))
  value_range (0.0, 360.0)
  ui_meta     ("unit", "degree")

property_double (offset, _("Offset"), 0.5)
  description(_("Value offset"))
  value_range (-G_MAXDOUBLE, G_MAXDOUBLE)
  ui_range    (0.0, 1.0)

property_double (exponent, _("Exponent"), 1.0)
  description(_("Value exponent"))
  value_range (0.0, G_MAXDOUBLE)
  ui_range    (0.0, 10.0)

property_double (x_offset, _("X Offset"), 0.0)
  description (_("Offset for X axis"))
  value_range (-G_MAXDOUBLE, G_MAXDOUBLE)
  ui_range    (-10000.0, 10000.0)
  ui_meta     ("unit", "pixel-coordinate")
  ui_meta     ("axis", "x")

property_double (y_offset, _("Y Offset"), 0.0)
  description (_("Offset for Y axis"))
  value_range (-G_MAXDOUBLE, G_MAXDOUBLE)
  ui_range    (-10000.0, 10000.0)
  ui_meta     ("unit", "pixel-coordinate")
  ui_meta     ("axis", "y")

property_double (rotation, _("Rotation"), 0.0)
  description(_("Pattern rotation angle"))
  value_range (0.0, 360.0)
  ui_meta     ("unit", "degree")

property_int (supersampling, _("Supersampling"), 0)
  description(_("Supersampling factor"))
  value_range (0, 4)

#else

#define GEGL_OP_POINT_RENDER
#define GEGL_OP_NAME     linear_sinusoid
#define GEGL_OP_C_SOURCE linear-sinusoid.c

#include "gegl-op.h"
#include <math.h>

static void
prepare (GeglOperation *operation)
{
  gegl_operation_set_format (operation, "output", babl_format ("Y' float"));
}

static GeglRectangle
get_bounding_box (GeglOperation *operation)
{
  return gegl_rectangle_infinite_plane ();
}

static gboolean
process (GeglOperation       *operation,
         void                *out_buf,
         glong                n_pixels,
         const GeglRectangle *roi,
         gint                 level)
{
  GeglProperties *o = GEGL_PROPERTIES (operation);
  gdouble         scale;
  gdouble         x_scale, y_scale;
  gdouble         x_angle, y_angle;
  gint            i, j;
  gdouble         x0, y0;
  gdouble         x, y;
  gdouble         i_dx, i_dy;
  gdouble         j_dx, j_dy;
  gint            supersampling_size;
  gdouble         supersampling_scale;
  gdouble         supersampling_scale2;
  gint            m, n;
  gdouble         u0, v0;
  gdouble         u, v;
  gdouble         m_du, m_dv;
  gdouble         n_du, n_dv;
  gfloat         *result = out_buf;

  scale = 1.0 / (1 << level);

  x_scale = 2 * G_PI * scale / o->x_period;
  y_scale = 2 * G_PI * scale / o->y_period;
  
  x_angle = -G_PI *  o->rotation             / 180.0;
  y_angle = -G_PI * (o->rotation + o->angle) / 180.0;

  i_dx = cos (x_angle) * x_scale;
  i_dy = cos (y_angle) * y_scale;

  j_dx = sin (x_angle) * x_scale;
  j_dy = sin (y_angle) * y_scale;

  x0 = o->x_phase * scale            +
       (roi->x - o->x_offset) * i_dx +
       (roi->y - o->y_offset) * j_dx;
  y0 = o->y_phase * scale            +
       (roi->x - o->x_offset) * i_dy +
       (roi->y - o->y_offset) * j_dy;

  if (o->supersampling)
    {
      gdouble offset;

      supersampling_size   = 1 << o->supersampling;
      supersampling_scale  = 1.0 / supersampling_size;
      supersampling_scale2 = supersampling_scale * supersampling_scale;

      m_du = supersampling_scale * i_dx;
      m_dv = supersampling_scale * i_dy;

      n_du = supersampling_scale * j_dx;
      n_dv = supersampling_scale * j_dy;

      offset  = (gdouble) (supersampling_size - 1) / supersampling_size;
      x0     -= offset * (i_dx + j_dx);
      y0     -= offset * (i_dy + j_dy);
    }

  for (j = 0; j < roi->height; j++)
    {
      x = x0;
      y = y0;

      for (i = 0; i < roi->width; i++)
        {
          gdouble z;

          if (! o->supersampling)
            {
              z = o->offset                -
                  o->x_amplitude * cos (x) -
                  o->y_amplitude * cos (y);
              z = pow (z, o->exponent);
            }
          else
            {
              z = 0.0;

              u0 = x;
              v0 = y;

              for (n = 0; n < supersampling_size; n++)
                {
                  u = u0;
                  v = v0;

                  for (m = 0; m < supersampling_size; m++)
                    {
                      gdouble w;

                      w = o->offset                -
                          o->x_amplitude * cos (u) -
                          o->y_amplitude * cos (v);
                      w = pow (w, o->exponent);

                      z += w;

                      u += m_du;
                      v += m_dv;
                    }

                  u0 += n_du;
                  v0 += n_dv;
                }

              z *= supersampling_scale2;
            }

          *result++ = z;

          x += i_dx;
          y += i_dy;
        }

      x0 += j_dx;
      y0 += j_dy;
    }

  return TRUE;
}

static void
gegl_op_class_init (GeglOpClass *klass)
{
  GeglOperationClass            *operation_class;
  GeglOperationPointRenderClass *point_render_class;

  operation_class    = GEGL_OPERATION_CLASS (klass);
  point_render_class = GEGL_OPERATION_POINT_RENDER_CLASS (klass);

  point_render_class->process = process;

  operation_class->get_bounding_box = get_bounding_box;
  operation_class->prepare = prepare;
  operation_class->opencl_support = FALSE;

  gegl_operation_class_set_keys (operation_class,
    "name",               "gegl:linear-sinusoid",
    "title",              _("Linear Sinusoid"),
    "categories",         "render",
    "position-dependent", "true",
    "license",            "GPL3+",
    "description",        _("Generate a linear sinusoid pattern"),
    NULL);
}

#endif