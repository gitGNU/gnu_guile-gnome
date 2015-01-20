/* guile-gnome
 * Copyright (C) 2008,2012,2014 Free Software Foundation, Inc.
 *
 * clutter-support.c: Support routines for the clutter wrapper
 *
 * This program is free software; you can redistribute it and/or    
 * modify it under the terms of the GNU General Public License as   
 * published by the Free Software Foundation; either version 2 of   
 * the License, or (at your option) any later version.              
 *                                                                  
 * This program is distributed in the hope that it will be useful,  
 * but WITHOUT ANY WARRANTY; without even the implied warranty of   
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    
 * GNU General Public License for more details.                     
 *                                                                  
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, contact:
 *
 * Free Software Foundation           Voice:  +1-617-542-5942
 * 59 Temple Place - Suite 330        Fax:    +1-617-542-2652
 * Boston, MA  02111-1307,  USA       gnu@gnu.org
 */

#include <string.h>

#include "clutter-support.h"
#include "guile-gnome-gobject.h"

#define GRUNTIME_ERROR(format, func_name, args...) \
  scm_error_scm (scm_str2symbol ("gruntime-error"), scm_makfrom0str (func_name), \
                 scm_simple_format (SCM_BOOL_F, scm_makfrom0str (format), \
                                    scm_list_n (args, SCM_UNDEFINED)), \
                 SCM_EOL, SCM_EOL)

SCM
scm_clutter_knot_to_scm (ClutterKnot *k)
{
    return scm_cons (scm_from_int (k->x), scm_from_int (k->y));
}

ClutterKnot*
scm_scm_to_clutter_knot (SCM scm)
{
    ClutterKnot ret;
    ret.x = scm_to_int (scm_car (scm));
    ret.y = scm_to_int (scm_cdr (scm));
    return clutter_knot_copy (&ret);
}

SCM
scm_clutter_path_node_to_scm (ClutterPathNode *pn)
{
    SCM tail = SCM_EOL;
    SCM head;

    head = scm_c_make_gvalue (CLUTTER_TYPE_PATH_NODE);
    g_value_set_enum (scm_c_gvalue_peek_value (head), pn->type);

    switch (pn->type) {
    case CLUTTER_PATH_CURVE_TO:
    case CLUTTER_PATH_REL_CURVE_TO:
    default:
      tail = scm_cons (scm_clutter_knot_to_scm (&pn->points[2]), tail);
    case CLUTTER_PATH_LINE_TO:
    case CLUTTER_PATH_REL_LINE_TO:
      tail = scm_cons (scm_clutter_knot_to_scm (&pn->points[1]), tail);
    case CLUTTER_PATH_MOVE_TO:
    case CLUTTER_PATH_REL_MOVE_TO:
      tail = scm_cons (scm_clutter_knot_to_scm (&pn->points[0]), tail);
    case CLUTTER_PATH_CLOSE:
      break;
    }

    return scm_cons (head, tail);
}

ClutterPathNode*
scm_scm_to_clutter_path_node (SCM scm)
{
    ClutterPathNode ret;
    ClutterPathNodeType type;

    if (scm_c_gvalue_holds (scm_car (scm), CLUTTER_TYPE_PATH_NODE))
        type = g_value_get_enum (scm_c_gvalue_peek_value (scm_car (scm)));
    else {
        GValue newval = {0,};
        g_value_init (&newval, CLUTTER_TYPE_PATH_NODE);
        scm_c_gvalue_set (&newval, scm_car (scm));
        type = g_value_get_enum (&newval);
    }

    switch (type) {
    case CLUTTER_PATH_CURVE_TO:
    case CLUTTER_PATH_REL_CURVE_TO:
    default:
        ret.points[2].x = scm_to_int (scm_car (scm_cadddr (scm)));
        ret.points[2].y = scm_to_int (scm_cdr (scm_cadddr (scm)));
    case CLUTTER_PATH_LINE_TO:
    case CLUTTER_PATH_REL_LINE_TO:
        ret.points[1].x = scm_to_int (scm_car (scm_caddr (scm)));
        ret.points[1].y = scm_to_int (scm_cdr (scm_caddr (scm)));
    case CLUTTER_PATH_MOVE_TO:
    case CLUTTER_PATH_REL_MOVE_TO:
        ret.points[0].x = scm_to_int (scm_car (scm_cadr (scm)));
        ret.points[0].y = scm_to_int (scm_cdr (scm_cadr (scm)));
    case CLUTTER_PATH_CLOSE:
        break;
    }
      
    ret.type = type;
    return clutter_path_node_copy (&ret);
}

SCM
scm_clutter_color_to_scm (ClutterColor *c)
{
    return scm_list_4 (scm_from_uint8 (c->red),
                       scm_from_uint8 (c->green),
                       scm_from_uint8 (c->blue),
                       scm_from_uint8 (c->alpha));
}

ClutterColor*
scm_scm_to_clutter_color (SCM scm)
{
    ClutterColor ret;
    
    ret.red = scm_to_uint8 (scm_car (scm));
    ret.green = scm_to_uint8 (scm_cadr (scm));
    ret.blue = scm_to_uint8 (scm_caddr (scm));
    ret.alpha = scm_to_uint8 (scm_cadddr (scm));

    return clutter_color_copy (&ret);
}

SCM
scm_clutter_margin_to_scm (ClutterMargin *c)
{
    return scm_list_4 (scm_from_double (c->left),
                       scm_from_double (c->right),
                       scm_from_double (c->top),
                       scm_from_double (c->bottom));
}

ClutterMargin*
scm_scm_to_clutter_margin (SCM scm)
{
    ClutterMargin ret;
    
    ret.left = scm_to_double (scm_car (scm));
    ret.right = scm_to_double (scm_cadr (scm));
    ret.top = scm_to_double (scm_caddr (scm));
    ret.bottom = scm_to_double (scm_cadddr (scm));

    return clutter_margin_copy (&ret);
}

SCM
wrap_clutter_actor_get_margin (ClutterActor* actor)
{
    ClutterMargin margin;
    clutter_actor_get_margin (actor, &margin);
    return scm_clutter_margin_to_scm (&margin);
}

SCM
scm_clutter_actor_box_to_scm (ClutterActorBox *a)
{
    return scm_list_4 (scm_from_double (a->x1),
                       scm_from_double (a->y1),
                       scm_from_double (a->x2),
                       scm_from_double (a->y2));
}

ClutterActorBox*
scm_scm_to_clutter_actor_box (SCM scm)
{
    ClutterActorBox ret;
    ret.x1 = scm_to_double (scm_car (scm));
    ret.y1 = scm_to_double (scm_cadr (scm));
    ret.x2 = scm_to_double (scm_caddr (scm));
    ret.y2 = scm_to_double (scm_cadddr (scm));
    return clutter_actor_box_copy (&ret);
}

SCM
scm_clutter_geometry_to_scm (ClutterGeometry *g)
{
    return scm_list_4 (scm_from_int (g->x),
                       scm_from_int (g->y),
                       scm_from_uint (g->width),
                       scm_from_uint (g->height));
}

ClutterGeometry*
scm_scm_to_clutter_geometry (SCM scm)
{
    ClutterGeometry ret;
    ret.x = scm_to_int (scm_car (scm));
    ret.y = scm_to_int (scm_cadr (scm));
    ret.width = scm_to_uint (scm_caddr (scm));
    ret.height = scm_to_uint (scm_cadddr (scm));
    return g_boxed_copy (clutter_geometry_get_type (), &ret);
}

SCM
scm_clutter_vertex_to_scm (ClutterVertex *x)
{
    return scm_list_3 (scm_from_double (x->x),
                       scm_from_double (x->y),
                       scm_from_double (x->z));
}

ClutterVertex*
scm_scm_to_clutter_vertex (SCM scm)
{
    ClutterVertex ret;
    ret.x = scm_to_double (scm_car (scm));
    ret.y = scm_to_double (scm_cadr (scm));
    ret.z = scm_to_double (scm_caddr (scm));
    return clutter_vertex_copy (&ret);
}

SCM
scm_clutter_perspective_to_scm (ClutterPerspective *x)
{
    return scm_list_4 (scm_from_double (x->fovy),
                       scm_from_double (x->aspect),
                       scm_from_double (x->z_near),
                       scm_from_double (x->z_far));
}

ClutterPerspective*
scm_scm_to_clutter_perspective (SCM scm)
{
    ClutterPerspective ret;
    ret.fovy = scm_to_double (scm_car (scm));
    ret.aspect = scm_to_double (scm_cadr (scm));
    ret.z_near = scm_to_double (scm_caddr (scm));
    ret.z_far = scm_to_double (scm_cadddr (scm));
    return g_boxed_copy (CLUTTER_TYPE_PERSPECTIVE, &ret);
}

SCM
wrap_clutter_stage_get_perspective (ClutterStage* stage)
{
    ClutterPerspective p;
    clutter_stage_get_perspective (stage, &p);
    return scm_clutter_perspective_to_scm (&p);
}

SCM
_wrap_clutter_color_from_string (const char *name)
{
    ClutterColor parsed;
    if (clutter_color_from_string (&parsed, name))
        return scm_clutter_color_to_scm (&parsed);
    else
        return SCM_BOOL_F;
}

SCM
wrap_clutter_actor_get_background_color (ClutterActor* actor)
{
    ClutterColor color;
    clutter_actor_get_background_color (actor, &color);
    return scm_clutter_color_to_scm (&color);
}

SCM
wrap_clutter_text_get_color (ClutterText* text)
{
    ClutterColor color;
    clutter_text_get_color (text, &color);
    return scm_clutter_color_to_scm (&color);
}

ClutterUnits*
wrap_clutter_units_from_pixels (gint px)
{
  ClutterUnits units;
  clutter_units_from_pixels (&units, px);
  return clutter_units_copy (&units);
}

ClutterUnits*
wrap_clutter_units_from_em (gfloat em)
{
  ClutterUnits units;
  clutter_units_from_em (&units, em);
  return clutter_units_copy (&units);
}

ClutterUnits*
wrap_clutter_units_from_em_for_font (const gchar *font_name, gfloat em)
{
  ClutterUnits units;
  clutter_units_from_em_for_font (&units, font_name, em);
  return clutter_units_copy (&units);
}

ClutterUnits*
wrap_clutter_units_from_mm (gfloat mm)
{
  ClutterUnits units;
  clutter_units_from_mm (&units, mm);
  return clutter_units_copy (&units);
}

ClutterUnits*
wrap_clutter_units_from_pt (gfloat pt)
{
  ClutterUnits units;
  clutter_units_from_pt (&units, pt);
  return clutter_units_copy (&units);
}

ClutterUnits*
wrap_clutter_units_from_string (const gchar *str)
{
  ClutterUnits units;
  clutter_units_from_string (&units, str);
  return clutter_units_copy (&units);
}


/* Since 1.12 */

SCM
scm_clutter_point_to_scm (ClutterPoint *c)
{
    return scm_list_2 (scm_from_double (c->x),
                       scm_from_double (c->y));
}

ClutterPoint*
scm_scm_to_clutter_point (SCM scm)
{
    ClutterPoint ret;

    ret.x = scm_to_double (scm_car (scm));
    ret.y = scm_to_double (scm_cadr (scm));

    return clutter_point_copy (&ret);
}

SCM
scm_clutter_size_to_scm (ClutterSize *c)
{
    return scm_list_2 (scm_from_double (c->width),
                       scm_from_double (c->height));
}

ClutterSize*
scm_scm_to_clutter_size (SCM scm)
{
    ClutterSize ret;

    ret.width = scm_to_double (scm_car (scm));
    ret.height = scm_to_double (scm_cadr (scm));

    return clutter_size_copy (&ret);
}

SCM
scm_clutter_rect_to_scm (ClutterRect *rect)
{
  return scm_list_4 (scm_from_double (rect->origin.x),
		     scm_from_double (rect->origin.y),
		     scm_from_double (rect->size.width),
		     scm_from_double (rect->size.height));
}

ClutterRect*
scm_scm_to_clutter_rect (SCM scm)
{
  ClutterRect ret;

  ret.origin.x = scm_to_double (scm_car (scm));
  ret.origin.y = scm_to_double (scm_cadr (scm));
  ret.size.width = scm_to_double (scm_caddr (scm));
  ret.size.height = scm_to_double (scm_cadddr (scm));

  return clutter_rect_copy (&ret);
}

SCM
scm_clutter_matrix_to_scm (ClutterMatrix *m)
{
  return scm_list_n (scm_from_double (m->xx), /* column 0 */
		     scm_from_double (m->yx),
		     scm_from_double (m->zx),
		     scm_from_double (m->wx),
		     scm_from_double (m->xy), /* column 1 */
		     scm_from_double (m->yy),
		     scm_from_double (m->zy),
		     scm_from_double (m->wy),
		     scm_from_double (m->xz), /* column 2 */
		     scm_from_double (m->yz),
		     scm_from_double (m->zz),
		     scm_from_double (m->wz),
		     scm_from_double (m->xw), /* column 3 */
		     scm_from_double (m->yw),
		     scm_from_double (m->zw),
		     scm_from_double (m->ww),
		     SCM_UNDEFINED);
}

ClutterMatrix*
scm_scm_to_clutter_matrix (SCM scm)
{
  ClutterMatrix ret;

  ret.xx = scm_to_double (scm_list_ref (scm, scm_from_int (0)));
  ret.yx = scm_to_double (scm_list_ref (scm, scm_from_int (1)));
  ret.zx = scm_to_double (scm_list_ref (scm, scm_from_int (2)));
  ret.wx = scm_to_double (scm_list_ref (scm, scm_from_int (3)));
  ret.xy = scm_to_double (scm_list_ref (scm, scm_from_int (4)));
  ret.yy = scm_to_double (scm_list_ref (scm, scm_from_int (5)));
  ret.zy = scm_to_double (scm_list_ref (scm, scm_from_int (6)));
  ret.wy = scm_to_double (scm_list_ref (scm, scm_from_int (7)));
  ret.xz = scm_to_double (scm_list_ref (scm, scm_from_int (8)));
  ret.yz = scm_to_double (scm_list_ref (scm, scm_from_int (9)));
  ret.zz = scm_to_double (scm_list_ref (scm, scm_from_int (10)));
  ret.wz = scm_to_double (scm_list_ref (scm, scm_from_int (11)));
  ret.xw = scm_to_double (scm_list_ref (scm, scm_from_int (12)));
  ret.yw = scm_to_double (scm_list_ref (scm, scm_from_int (13)));
  ret.zw = scm_to_double (scm_list_ref (scm, scm_from_int (14)));
  ret.ww = scm_to_double (scm_list_ref (scm, scm_from_int (15)));

  return g_boxed_copy (clutter_matrix_get_type (), &ret);
}

gboolean
_wrap_clutter_image_set_data (ClutterImage *image,
			      guchar *data,
			      CoglPixelFormat pixel_format,
			      guint width,
			      guint height,
			      guint row_stride,
			      GError **error)
{
  return clutter_image_set_data (image,
				 (const guint8 *)data,
				 pixel_format, width, height, row_stride, error);
}
