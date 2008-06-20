/* guile-gnome
 * Copyright (C) 2008 Free Software Foundation, Inc.
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
    ClutterKnot *ret = g_new0 (ClutterKnot, 1);
    
    ret->x = scm_to_int (scm_car (scm));
    ret->y = scm_to_int (scm_cdr (scm));
    return ret;
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
    ClutterColor *ret = g_new0 (ClutterColor, 1);
    
    ret->red = scm_to_uint8 (scm_car (scm));
    ret->green = scm_to_uint8 (scm_cadr (scm));
    ret->blue = scm_to_uint8 (scm_caddr (scm));
    ret->alpha = scm_to_uint8 (scm_cadddr (scm));

    return ret;
}

SCM
scm_clutter_units_to_scm (ClutterUnit u)
{
    return scm_from_double (CLUTTER_UNITS_TO_FLOAT (u));
}

ClutterUnit
scm_scm_to_clutter_units (SCM scm)
{
    return CLUTTER_UNITS_FROM_FLOAT (scm_to_double (scm));
}

SCM
scm_clutter_actor_box_to_scm (ClutterActorBox *a)
{
    return scm_list_4 (scm_clutter_units_to_scm (a->x1),
                       scm_clutter_units_to_scm (a->y1),
                       scm_clutter_units_to_scm (a->x2),
                       scm_clutter_units_to_scm (a->y2));
}

ClutterActorBox*
scm_scm_to_clutter_actor_box (SCM scm)
{
    ClutterActorBox *ret = g_new0 (ClutterActorBox, 1);
    
    ret->x1 = scm_scm_to_clutter_units (scm_car (scm));
    ret->y1 = scm_scm_to_clutter_units (scm_cadr (scm));
    ret->x2 = scm_scm_to_clutter_units (scm_caddr (scm));
    ret->y2 = scm_scm_to_clutter_units (scm_cadddr (scm));

    return ret;
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
    ClutterGeometry *ret = g_new0 (ClutterGeometry, 1);
    
    ret->x = scm_to_int (scm_car (scm));
    ret->y = scm_to_int (scm_cadr (scm));
    ret->width = scm_to_uint (scm_caddr (scm));
    ret->height = scm_to_uint (scm_cadddr (scm));

    return ret;
}

SCM
scm_clutter_vertex_to_scm (ClutterVertex *x)
{
    return scm_list_3 (scm_clutter_units_to_scm (x->x),
                       scm_clutter_units_to_scm (x->y),
                       scm_clutter_units_to_scm (x->z));
}

ClutterVertex*
scm_scm_to_clutter_vertex (SCM scm)
{
    ClutterVertex *ret = g_new0 (ClutterVertex, 1);
    
    ret->x = scm_scm_to_clutter_units (scm_car (scm));
    ret->y = scm_scm_to_clutter_units (scm_cadr (scm));
    ret->z = scm_scm_to_clutter_units (scm_caddr (scm));

    return ret;
}

struct StockAlphaFunc {
    const char *name;
    ClutterAlphaFunc func;
};

const struct StockAlphaFunc stock_alpha_funcs[] = {
    { "ramp-inc", clutter_ramp_inc_func },
    { "ramp-dec", clutter_ramp_dec_func },
    { "ramp", clutter_ramp_func },
    { "sine", clutter_sine_func },
    { "sine-inc", clutter_sine_inc_func },
    { "sine-dec", clutter_sine_dec_func },
    { "sine-half", clutter_sine_half_func },
    { "square", clutter_square_func },
    { "smoothstep-inc", clutter_smoothstep_inc_func },
    { "smoothstep-dec", clutter_smoothstep_dec_func },
    { "exp-inc", clutter_exp_inc_func },
    { "exp-dec", clutter_exp_dec_func },
    { NULL, NULL },
};
            
void
clutter_alpha_set_stock_func (ClutterAlpha *a, const char *name)
{
    const struct StockAlphaFunc *p;
    
    for (p = stock_alpha_funcs; p->name; p++)
        if (strcmp (name, p->name) == 0) {
            clutter_alpha_set_func (a, p->func, NULL, NULL);
            return;
        }
    
    GRUNTIME_ERROR("unknown stock alpha func: ~a", 
                   "clutter-alpha-set-stock-func",
                   scm_from_locale_string (name));
}
