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
