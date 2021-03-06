/* guile-gnome
 * Copyright (C) 2004, 2010 Free Software Foundation, Inc.
 *
 * gconf-support.c: Support routines for the gconf wrapper
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

#include "guile-support.h"
#include "gconf-support.h"
#include "guile-gnome-gobject.h"

#define GRUNTIME_ERROR(format, func_name, args...) \
  scm_error_scm (scm_from_locale_symbol ("gruntime-error"), scm_from_locale_string (func_name), \
                 scm_simple_format (SCM_BOOL_F, scm_from_locale_string (format), \
                                    scm_list_n (args, SCM_UNDEFINED)), \
                 SCM_EOL, SCM_EOL)

GType
_gconf_schema_get_type (void)
{
    static GType t = 0;
    if (!t)
        t = g_boxed_type_register_static ("GConfSchema",
                                          (GBoxedCopyFunc)gconf_schema_copy,
                                          (GBoxedFreeFunc)gconf_schema_free);
    return t;
}

GType
_gconf_value_get_type (void)
{
    static GType t = 0;
    if (!t)
        t = g_boxed_type_register_static ("GConfValue",
                                          (GBoxedCopyFunc)gconf_value_copy,
                                          (GBoxedFreeFunc)gconf_value_free);
    return t;
}

SCM
scm_c_gconf_value_to_scm (const GConfValue *value)
{
    if (!value)
        scm_throw (scm_from_locale_symbol ("value-unset"), SCM_EOL);
    
    switch (value->type) {
    case GCONF_VALUE_STRING:
        return scm_from_locale_string (gconf_value_get_string (value));
    case GCONF_VALUE_INT:
        return scm_from_int (gconf_value_get_int (value));
    case GCONF_VALUE_FLOAT:
        return scm_from_double (gconf_value_get_float (value));
    case GCONF_VALUE_BOOL:
        return SCM_BOOL (gconf_value_get_bool (value));
    case GCONF_VALUE_SCHEMA:
        return scm_c_gvalue_new_from_boxed (_GCONF_TYPE_SCHEMA, 
                                            gconf_value_get_schema (value));
    case GCONF_VALUE_LIST: {
        SCM ret = SCM_EOL;
        GSList *head, *walk;
        GConfValueType t;

        t = gconf_value_get_list_type (value);
        head = gconf_value_get_list (value);
        for (walk = head; walk; walk = walk->next) {
            switch (t) {
            case GCONF_VALUE_STRING:
                ret = scm_cons (scm_from_locale_string ((char*)walk->data), ret);
                g_free (walk->data);
                break;
            case GCONF_VALUE_INT:
                ret = scm_cons (scm_from_int (GPOINTER_TO_INT (walk->data)), ret);
                break;
            case GCONF_VALUE_FLOAT:
                ret = scm_cons (scm_from_double (*(float*)walk->data), ret);
                g_free (walk->data);
                break;
            case GCONF_VALUE_BOOL:
                ret = scm_cons (SCM_BOOL (walk->data), ret);
                break;
            case GCONF_VALUE_SCHEMA:
                ret = scm_cons (scm_c_gvalue_new_from_boxed (_GCONF_TYPE_SCHEMA,
                                                             walk->data),
                                ret);
                break;
            default:
                scm_throw (scm_from_locale_symbol ("unknown-value"),
                           SCM_LIST1 (scm_from_int (t)));
            }
        }
        g_slist_free (head);
        return scm_reverse_x (ret, SCM_UNBOUND);
    }
    case GCONF_VALUE_PAIR:
        return scm_cons (scm_c_gconf_value_to_scm (gconf_value_get_car (value)),
                         scm_c_gconf_value_to_scm (gconf_value_get_cdr (value)));
    default:
        scm_throw (scm_from_locale_symbol ("unknown-value"),
                   SCM_LIST1 (scm_from_int (value->type)));
    }
    return SCM_BOOL_F; /* shouldn't get here */
}

#define FUNC_NAME "scm->gconf-value"
static GConfValueType
sniff_list_type (SCM l) 
{
    SCM walk;
    SCM value;

    /* since we checked with pair_p below, we know there's at least one
     * element */
    value = SCM_CAR (l);

    if (SCM_FALSEP (value) || SCM_EQ_P (value, SCM_BOOL_T)) {
        for (walk=l; SCM_NNULLP (walk); walk = SCM_CDR (walk)) {
            value = SCM_CAR (walk);
            if (!(SCM_FALSEP (value) || SCM_EQ_P (value, SCM_BOOL_T)))
                scm_misc_error (FUNC_NAME, "Invalid subelement", scm_list_1 (value));
        }
        return GCONF_VALUE_BOOL;
    }
    if (scm_is_signed_integer (value, SCM_T_INT32_MIN, SCM_T_INT32_MAX)) {
        for (walk=l; SCM_NNULLP (walk); walk = SCM_CDR (walk)) {
            value = SCM_CAR (walk);
            if (!(scm_is_signed_integer (value, SCM_T_INT32_MIN, SCM_T_INT32_MAX)))
                scm_misc_error (FUNC_NAME, "Invalid subelement", scm_list_1 (value));
        }
        return GCONF_VALUE_INT;
    }
    if (SCM_NFALSEP (scm_inexact_p (value))) {
        for (walk=l; SCM_NNULLP (walk); walk = SCM_CDR (walk)) {
            value = SCM_CAR (walk);
            if (!(SCM_NFALSEP (scm_inexact_p (value))))
                scm_misc_error (FUNC_NAME, "Invalid subelement", scm_list_1 (value));
        }
        return GCONF_VALUE_FLOAT;
    }
    if (scm_is_string (value)) {
        for (walk=l; SCM_NNULLP (walk); walk = SCM_CDR (walk)) {
            value = SCM_CAR (walk);
            if (!(scm_is_string (value)))
                scm_misc_error (FUNC_NAME, "Invalid subelement", scm_list_1 (value));
        }
        return GCONF_VALUE_STRING;
    }
    if (SCM_GVALUEP (value)) {
        GValue *tmp; /* never allocated */
        SCM_VALIDATE_GVALUE_TYPE_COPY (1, value, _GCONF_TYPE_SCHEMA, tmp);
        for (walk=l; SCM_NNULLP (walk); walk = SCM_CDR (walk)) {
            value = SCM_CAR (walk);
            SCM_VALIDATE_GVALUE_TYPE_COPY (1, value, _GCONF_TYPE_SCHEMA, tmp);
        }
        return GCONF_VALUE_SCHEMA;
    }
    scm_misc_error (FUNC_NAME, "Invalid list", scm_list_1 (l));
    return 0;
}

GConfValue*
scm_c_scm_to_gconf_value (SCM value)
{
    GConfValue *ret = NULL;
    
    if (SCM_FALSEP (value) || SCM_EQ_P (value, SCM_BOOL_T)) {
        ret = gconf_value_new (GCONF_VALUE_BOOL);
        gconf_value_set_bool (ret, SCM_NFALSEP (value));
    } else if (scm_is_signed_integer (value, SCM_T_INT32_MIN, SCM_T_INT32_MAX)) {
        ret = gconf_value_new (GCONF_VALUE_INT);
        gconf_value_set_int (ret, scm_to_int (value));
    } else if (SCM_NFALSEP (scm_exact_p (value))) {
        if (SCM_NFALSEP (scm_leq_p (value, scm_from_uint (G_MAXINT)))) {
            ret = gconf_value_new (GCONF_VALUE_INT);
            gconf_value_set_int (ret, scm_to_int (value));
        } else {
            scm_misc_error (FUNC_NAME, "Invalid value: ~A", scm_list_1 (value));
        }
    } else if (SCM_NFALSEP (scm_inexact_p (value))) {
        ret = gconf_value_new (GCONF_VALUE_FLOAT);
        gconf_value_set_float (ret, scm_to_double (value));
    } else if (scm_is_string (value)) {
        char *chars;
        ret = gconf_value_new (GCONF_VALUE_STRING);
        chars = scm_to_locale_string (value);
        gconf_value_set_string (ret, chars);
        free (chars);
    } else if (scm_pair_p (value)) {
        if (scm_pair_p (SCM_CDR (value))) {
            GConfValueType t = sniff_list_type (value);
            SCM walk;
            GSList *l = NULL;
            
            for (walk = value; SCM_NNULLP (walk); walk = SCM_CDR (walk))
                l = g_slist_prepend (l, scm_c_scm_to_gconf_value (SCM_CAR (walk)));
                
            ret = gconf_value_new (GCONF_VALUE_LIST);
            gconf_value_set_list_type (ret, t);
            l = g_slist_reverse (l);
            gconf_value_set_list (ret, l);
            g_slist_free (l);
        } else {
            ret = gconf_value_new (GCONF_VALUE_PAIR);
            gconf_value_set_car (ret, scm_c_scm_to_gconf_value (SCM_CAR (value)));
            gconf_value_set_cdr (ret, scm_c_scm_to_gconf_value (SCM_CDR (value)));
        }
    } else {
        scm_misc_error (FUNC_NAME, "Invalid value: ~A", scm_list_1 (value));
    }

    return ret;
}
#undef FUNC_NAME

static void
with_notify_proc (GConfClient *client, guint cnxn_id, GConfEntry *entry,
             gpointer user_data) 
{
    SCM sclient, key, val, proc;
    
    proc = GPOINTER_TO_SCM (user_data);
    sclient = scm_c_gtype_instance_to_scm ((GTypeInstance*)client);
    key = scm_from_locale_symbol (gconf_entry_get_key (entry));
    val = scm_c_gconf_value_to_scm (gconf_entry_get_value (entry));

    scm_call_4 (proc, sclient, scm_from_uint (cnxn_id), key, val);
}

static void
notify_proc (GConfClient *client, guint cnxn_id, GConfEntry *entry,
             gpointer user_data) 
{
    scm_dynwind_guile_v__p_u_p_p (scm_with_guile, with_notify_proc,
                                  client, cnxn_id, entry, user_data);
}

guint
_wrap_gconf_client_notify_add (GConfClient *client, const gchar *namespace_section,
                               SCM proc, GError **err) 
{
    return gconf_client_notify_add
        (client, namespace_section, notify_proc,
         SCM_TO_GPOINTER (scm_gc_protect_object (proc)),
         (GFreeFunc)scm_gc_unprotect_object, err);
}

