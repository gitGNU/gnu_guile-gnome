/* guile-gnome
 * Copyright (C) 2004 Free Software Foundation, Inc.
 *
 * ical-support.c: Support routines for the evolution-data-server wrapper
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

#include <libical/ical.h>
#include <guile-gnome-gobject.h>

SCM
scm_c_module_ref (SCM module, const char *sym)
{
    return SCM_VARIABLE_REF (scm_c_module_lookup (module, sym));
}

static struct icaltimetype*
copy_icaltimetype (const struct icaltimetype *t)
{
    return (struct icaltimetype*)scm_must_strndup ((const gchar*)t,
                                                   sizeof(struct icaltimetype));
}

GType
icaltimetype_get_gtype (void)
{
    static GType t = 0;

    /* fixme: change to scm_gc_free once we switch to guile 1.7 -- right now
       it's a macro */
    if (!t)
        t = g_boxed_type_register_static ("guileicaltimetype",
                                          (GBoxedCopyFunc)copy_icaltimetype,
                                          (GBoxedFreeFunc)free);
    return t;
}

struct icaltimetype*
scm_date_to_icaltimetype (SCM date)
{
    struct icaltimetype *ret;
    static SCM date_to_string = SCM_BOOL_F;

    if (SCM_FALSEP (date_to_string))
        date_to_string = scm_c_module_ref (scm_c_resolve_module ("srfi srfi-19"),
                                           "date->string");
    
    /* fix me ... */
    ret = scm_must_malloc (sizeof(struct icaltimetype), "date->icaltimetype");
    *ret = icaltime_from_string (SCM_STRING_CHARS (scm_call_1 (date_to_string, date)));
    return ret;
}

SCM
scm_icaltimetype_to_date (struct icaltimetype *t)
{
    static SCM make_date = SCM_BOOL_F;
    int d; /* 1->daylight savings, 0->otherwise */

    if (SCM_FALSEP (make_date))
        make_date = scm_c_module_ref (scm_c_resolve_module ("srfi srfi-19"),
                                      "make-date");
    
    return scm_apply_0 (make_date,
                        scm_list_n (SCM_MAKINUM (0), /* nano */
                                    SCM_MAKINUM (t->second),
                                    SCM_MAKINUM (t->minute),
                                    SCM_MAKINUM (t->hour),
                                    SCM_MAKINUM (t->day),
                                    SCM_MAKINUM (t->month),
                                    SCM_MAKINUM (t->year),
                                    SCM_MAKINUM (icaltimezone_get_utc_offset
                                                 ((icaltimezone*)t->zone, t, &d)),
                                    SCM_UNDEFINED));
}
