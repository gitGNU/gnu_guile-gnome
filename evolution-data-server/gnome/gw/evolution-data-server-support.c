/* guile-gnome
 * Copyright (C) 2004 Free Software Foundation, Inc.
 *
 * evolution-data-server-support.c: Support routines for the evolution-data-server wrapper
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

#include "evolution-data-server-support.h"
#include "guile-gnome-gobject.h"
#include <string.h>

#define GRUNTIME_ERROR(format, func_name, args...) \
  scm_error_scm (scm_str2symbol ("gruntime-error"), scm_makfrom0str (func_name), \
                 scm_simple_format (SCM_BOOL_F, scm_makfrom0str (format), \
                                    scm_list_n (args, SCM_UNDEFINED)), \
                 SCM_EOL, SCM_EOL)

static char *
c_auth_proc (ECal *ecal, const char *prompt, const char *key, gpointer user_data)
{
    SCM proc = SCM_PACK (GPOINTER_TO_INT (user_data));
    return strdup (SCM_STRING_CHARS
                   (scm_call_3 (proc,
                                scm_c_gtype_instance_to_scm ((GTypeInstance*)ecal),
                                scm_makfrom0str (prompt),
                                scm_makfrom0str (key))));
}

ECal*
wrap_e_cal_open_default (ECalSourceType type, SCM auth_proc, GError **error) 
{
    ECal *ret = NULL;
    if (!e_cal_open_default (&ret, type, c_auth_proc, GINT_TO_POINTER (SCM_UNPACK (auth_proc)),
                             error))
        return NULL;
    return ret;
}

/* there must be an easier way... */
static time_t
srfi_date_to_time_t (SCM date)
#define FUNC_NAME "%date->time_t"
{
    static SCM srfi_19_module = SCM_BOOL_F;
    static SCM date_second, date_minute, date_hour, date_day, date_month, date_year;
    struct tm t;
    time_t ret;

    if (SCM_FALSEP (date))
        return 0;

    memset (&t, 0, sizeof(t));

    if (SCM_FALSEP (srfi_19_module)) {
        srfi_19_module = scm_c_resolve_module ("srfi srfi-19");
        date_second = scm_c_module_ref (srfi_19_module, "date-second");
        date_minute = scm_c_module_ref (srfi_19_module, "date-minute");
        date_hour = scm_c_module_ref (srfi_19_module, "date-hour");
        date_day = scm_c_module_ref (srfi_19_module, "date-day");
        date_month  = scm_c_module_ref (srfi_19_module, "date-month");
        date_year = scm_c_module_ref (srfi_19_module, "date-year");
    }

    t.tm_sec = scm_num2int (scm_call_1 (date_second, date), 0, FUNC_NAME);
    t.tm_min = scm_num2int (scm_call_1 (date_minute, date), 0, FUNC_NAME);
    t.tm_hour = scm_num2int (scm_call_1 (date_hour, date), 0, FUNC_NAME);
    t.tm_mday = scm_num2int (scm_call_1 (date_day, date), 0, FUNC_NAME);
    /* srfi months are 1-offset */
    t.tm_mon = scm_num2int (scm_call_1 (date_month, date), 0, FUNC_NAME) - 1;
    /* tm year as from 1900 */
    t.tm_year = scm_num2int (scm_call_1 (date_year, date), 0, FUNC_NAME) - 1900;
    t.tm_isdst = -1; /* dunno */
    ret = mktime (&t);
    if (ret == -1)
        GRUNTIME_ERROR ("Bad date: ~A", FUNC_NAME, date);
    return ret;
}
#undef FUNC_NAME

static SCM
time_t_to_srfi_date (time_t t)
{
    static SCM make_date = SCM_BOOL_F;
    struct tm tmp;

    if (t == 0)
        return SCM_BOOL_F;

    if (SCM_FALSEP (make_date))
        make_date = scm_c_module_ref (scm_c_resolve_module ("srfi srfi-19"),
                                      "make-date");
    gmtime_r (&t, &tmp);

    return scm_apply_0 (make_date,
                        scm_list_n (SCM_MAKINUM (0), /* nano */
                                    SCM_MAKINUM (tmp.tm_sec),
                                    SCM_MAKINUM (tmp.tm_min),
                                    SCM_MAKINUM (tmp.tm_hour),
                                    SCM_MAKINUM (tmp.tm_mday),
                                    SCM_MAKINUM (tmp.tm_mon+1), /* tm is 0-offset */
                                    SCM_MAKINUM (tmp.tm_year+1900),
                                    SCM_MAKINUM (0),
                                    SCM_UNDEFINED)); /* punt on the tz */
}

typedef struct {
    ECalComponent *comp;
    time_t instance_start;
    time_t instance_end;
    gpointer data;
} recur_data;

static gboolean
async_recur_instance_fn (gpointer rdata)
{
    ECalComponent *comp = ((recur_data*)rdata)->comp;
    time_t instance_start = ((recur_data*)rdata)->instance_start;
    time_t instance_end = ((recur_data*)rdata)->instance_end;
    SCM proc = SCM_PACK (GPOINTER_TO_INT (((recur_data*)rdata)->data));;
    
    scm_call_3 (proc,
                scm_c_gtype_instance_to_scm ((GTypeInstance*)comp),
                time_t_to_srfi_date (instance_start),
                time_t_to_srfi_date (instance_end));
    g_object_unref (comp);
    g_free (rdata);
    return FALSE;
}

static gboolean
recur_instance_fn (ECalComponent *comp, time_t instance_start, time_t instance_end,
                   gpointer data)
{
    recur_data *rdata = g_new0 (recur_data, 1);
    g_object_ref (comp);
    rdata->comp = comp;
    rdata->instance_start = instance_start;
    rdata->instance_end = instance_end;
    rdata->data = data; /* protected, because it's on the stack (?) */
    
    g_print ("here!\n");
    g_idle_add (async_recur_instance_fn, rdata);
    return TRUE;
}

void
wrap_e_cal_generate_instances (ECal *ecal, SCM start, SCM end, SCM instance_proc)
{
    e_cal_generate_instances (ecal, srfi_date_to_time_t (start),
                              srfi_date_to_time_t (end), recur_instance_fn,
                              GINT_TO_POINTER (SCM_UNPACK (instance_proc)));
}

#define DEFINE_GETTER(field,type)\
  PROTO_GETTER (field, type) \
  {\
    type tmp;\
    e_cal_component_get_##field (comp, &tmp);\
    return tmp;\
  }

DEFINE_GETTER (categories_list, GSList*);
DEFINE_GETTER (comment_list, GSList*);
DEFINE_GETTER (contact_list, GSList*);
DEFINE_GETTER (description_list, GSList*);

DEFINE_GETTER (uid, const char*);
DEFINE_GETTER (categories, const char*);
DEFINE_GETTER (url, const char*);
DEFINE_GETTER (location, const char*);

#define DEFINE_MALLOC_GETTER(field,type)\
  PROTO_GETTER (field, type*) \
  {\
    type* tmp = g_new0 (type, 1);\
    e_cal_component_get_##field (comp, tmp);\
    return tmp;\
  }

DEFINE_MALLOC_GETTER (dtend, ECalComponentDateTime);
DEFINE_MALLOC_GETTER (dtstamp, struct icaltimetype);
DEFINE_MALLOC_GETTER (dtstart, ECalComponentDateTime);
DEFINE_MALLOC_GETTER (due, ECalComponentDateTime);
DEFINE_MALLOC_GETTER (summary, ECalComponentText);
