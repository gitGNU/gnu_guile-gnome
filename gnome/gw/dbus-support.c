/* guile-gnome
 * Copyright (C) 2003,2004 Andy Wingo <wingo at pobox dot com>
 *
 * dbus-support.c: Support routines for the dbus wrapper
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

#include "dbus-support.h"

SCM
dbus_message_iter_get (DBusMessageIter *iter)
#define FUNC_NAME "dbus-message-iter-get"
{
    switch (dbus_message_iter_get_arg_type (iter)) {
    case DBUS_TYPE_INVALID:
        scm_misc_error (FUNC_NAME, "Invalid DBus message iter (message has no args?)", SCM_EOL);
        return SCM_UNSPECIFIED; /* shouldn't be reached */
        
    case DBUS_TYPE_NIL:
        return scm_str2symbol ("nil");

    case DBUS_TYPE_BYTE:
        return SCM_MAKINUM (dbus_message_iter_get_byte (iter));

    case DBUS_TYPE_BOOLEAN:
        return dbus_message_iter_get_boolean (iter) ? SCM_BOOL_T : SCM_BOOL_F;

    case DBUS_TYPE_INT32:
        return scm_int2num (dbus_message_iter_get_int32 (iter));

    case DBUS_TYPE_UINT32:
        return scm_uint2num (dbus_message_iter_get_uint32 (iter));

    case DBUS_TYPE_INT64:
        return scm_long_long2num (dbus_message_iter_get_int64 (iter));

    case DBUS_TYPE_UINT64:
        return scm_ulong_long2num (dbus_message_iter_get_uint64 (iter));

    case DBUS_TYPE_DOUBLE:
        return scm_double2num (dbus_message_iter_get_double (iter));

    case DBUS_TYPE_STRING:
        {
            char *ret = dbus_message_iter_get_string (iter);
            if (!ret)
                scm_misc_error (FUNC_NAME, "Internal DBus error 1", SCM_EOL);
            return scm_take0str (ret);
        }

    case DBUS_TYPE_CUSTOM:
        {
            char *name = NULL;
            unsigned char *bytes = NULL;
            int len = 0;
            SCM ret;
            
            if (!dbus_message_iter_get_custom (iter, &name, &bytes, &len))
                scm_misc_error (FUNC_NAME, "Internal DBus error 2", SCM_EOL);
            ret = scm_make_uve (len, SCM_CHAR (0));
            SCM_SET_UVECTOR_BASE (ret, bytes);
            return scm_cons (scm_take0str (name), ret);
        }
            
    case DBUS_TYPE_ARRAY:
        switch (dbus_message_iter_get_array_type (iter)) {
        case DBUS_TYPE_INVALID:
            scm_misc_error (FUNC_NAME, "Invalid DBus message iter", SCM_EOL);
            return SCM_UNSPECIFIED; /* shouldn't be reached */
        
        case DBUS_TYPE_BYTE:
        {
            unsigned char *bytes = NULL;
            int len = 0;
            SCM ret;
            
            if (!dbus_message_iter_get_byte_array (iter, &bytes, &len))
                scm_misc_error (FUNC_NAME, "Internal DBus error A1", SCM_EOL);
            ret = scm_make_uve (len, SCM_CHAR (0));
            SCM_SET_UVECTOR_BASE (ret, bytes);
            /* is that all? need to register the memory? dunno.. */
            return ret;
        }

        case DBUS_TYPE_INT32:
        {
            dbus_int32_t *data = NULL;
            int len = 0;
            SCM ret;
            
            if (!dbus_message_iter_get_int32_array (iter, &data, &len))
                scm_misc_error (FUNC_NAME, "Internal DBus error A2", SCM_EOL);
            /* this prototype is for longs, probably will break on 64 bit! */
            ret = scm_make_uve (len, SCM_MAKINUM (-1));
            SCM_SET_UVECTOR_BASE (ret, data);
            /* is that all? need to register the memory? dunno.. */
            return ret;
        }

        case DBUS_TYPE_UINT32:
        {
            dbus_uint32_t *data = NULL;
            int len = 0;
            SCM ret;
            
            if (!dbus_message_iter_get_uint32_array (iter, &data, &len))
                scm_misc_error (FUNC_NAME, "Internal DBus error A3", SCM_EOL);
            /* this prototype is for longs, probably will break on 64 bit! */
            ret = scm_make_uve (len, SCM_MAKINUM (1));
            SCM_SET_UVECTOR_BASE (ret, data);
            /* is that all? need to register the memory? dunno.. */
            return ret;
        }

        case DBUS_TYPE_INT64:
        {
            dbus_int64_t *data = NULL;
            int len = 0;
            SCM ret;
            
            if (!dbus_message_iter_get_int64_array (iter, &data, &len))
                scm_misc_error (FUNC_NAME, "Internal DBus error A4", SCM_EOL);
            ret = scm_make_uve (len, scm_list_2 (scm_sym_quote, SCM_MAKINUM (-1)));
            SCM_SET_UVECTOR_BASE (ret, data);
            /* is that all? need to register the memory? dunno.. */
            return ret;
        }

        case DBUS_TYPE_UINT64:
        {
            dbus_uint64_t *data = NULL;
            int len = 0;
            SCM ret;
            
            if (!dbus_message_iter_get_uint64_array (iter, &data, &len))
                scm_misc_error (FUNC_NAME, "Internal DBus error A5", SCM_EOL);
            ret = scm_make_uve (len, scm_list_2 (scm_sym_quote, SCM_MAKINUM (1)));
            SCM_SET_UVECTOR_BASE (ret, data);
            /* is that all? need to register the memory? dunno.. */
            return ret;
        }

        case DBUS_TYPE_DOUBLE:
            {
            double *data = NULL;
            int len = 0;
            SCM ret;
            
            if (!dbus_message_iter_get_double_array (iter, &data, &len))
                scm_misc_error (FUNC_NAME, "Internal DBus error A6", SCM_EOL);
            ret = scm_make_uve (len, scm_double2num (1./3.));
            
            SCM_SET_UVECTOR_BASE (ret, data);
            /* is that all? need to register the memory? dunno.. */
            return ret;
        }
                
        default:
            {
                DBusMessageIter aiter;
                SCM ret = SCM_EOL;
                int type = DBUS_TYPE_INVALID;
                
                dbus_message_iter_init_array_iterator (iter, &aiter, &type);
                if (type == DBUS_TYPE_INVALID)
                    scm_misc_error (FUNC_NAME, "Internal DBus error A7", SCM_EOL);
                if (type != dbus_message_iter_get_arg_type (&aiter))
                    return SCM_EOL; /* empty array */
                
                while (1) {
                    ret = scm_cons (dbus_message_iter_get (&aiter), ret);
                    if (dbus_message_iter_has_next (&aiter)) {
                        dbus_message_iter_next (&aiter);
                    } else {
                        return scm_reverse_x (ret, SCM_EOL);
                    }
                }
            }
        }
        
    case DBUS_TYPE_DICT:
        {
            DBusMessageIter diter;
            SCM ret = SCM_EOL;
                
            dbus_message_iter_init_dict_iterator (iter, &diter);
            if (dbus_message_iter_get_arg_type (&diter) == DBUS_TYPE_INVALID)
                return SCM_EOL; /* empty dict */
                
            while (1) {
                char *key = dbus_message_iter_get_dict_key (&diter);
                if (!key)
                    scm_misc_error (FUNC_NAME, "Internal DBus error A8", SCM_EOL);
                ret = scm_cons (scm_cons (scm_take0str (key),
                                          dbus_message_iter_get (&diter)),
                                ret);
                if (dbus_message_iter_has_next (&diter)) {
                    dbus_message_iter_next (&diter);
                } else {
                    return scm_reverse_x (ret, SCM_EOL);
                }
            }
        }
        
    case DBUS_TYPE_OBJECT_PATH:
        {
            char **split;
            SCM ret = SCM_EOL;
            char *str = dbus_message_iter_get_string (iter);
            
            if (!str)
                scm_misc_error (FUNC_NAME, "Internal DBus error A9", SCM_EOL);
            split = g_strsplit (str, ".", 0);
            free (str);
            str = *split;
            while (str) {
                ret = scm_cons (scm_makfrom0str (str), ret);
                str++;
            }
            g_strfreev (split);
            return scm_reverse_x (ret, SCM_EOL);
        }
            
    default:
        return scm_str2symbol ("unknown");
    }
}
#undef FUNC_NAME

/* for a while... */
#define FUNC_NAME "dbus-message-iter-append"
static void
detect_dbus_type (SCM arg, int *dtype, int *array_dtype)
{
    /* the default, we detect the type based on the argument */
    if (SCM_FALSEP (arg) || SCM_EQ_P (arg, SCM_BOOL_T)) {
        *dtype = DBUS_TYPE_BOOLEAN;
    } else if (SCM_SYMBOLP (arg)) {
        if (SCM_EQ_P (arg, scm_str2symbol ("nil")))
            *dtype = DBUS_TYPE_NIL;
        else
            scm_misc_error (FUNC_NAME, "Invalid arg: ~A", scm_list_1 (arg));
    } else if (SCM_INUMP (arg)) {
        *dtype = DBUS_TYPE_INT32;
    } else if (SCM_NFALSEP (scm_exact_p (arg))) {
        if (SCM_NFALSEP (scm_leq_p (arg, scm_uint2num (G_MAXINT))))
            *dtype = DBUS_TYPE_INT32;
        else if (SCM_NFALSEP (scm_leq_p (arg, scm_long_long2num (G_MAXINT64))))
            *dtype = DBUS_TYPE_INT64;
        else
            scm_misc_error (FUNC_NAME, "Invalid arg: ~A", scm_list_1 (arg));
    } else if (SCM_NFALSEP (scm_inexact_p (arg))) {
        *dtype = DBUS_TYPE_DOUBLE;
    } else if (SCM_STRINGP (arg)) {
        *dtype = DBUS_TYPE_STRING;
    } else if (SCM_ARRAYP (arg)) {
        *dtype = DBUS_TYPE_ARRAY;
        switch (scm_uniform_element_size (arg)) {
        case 1:
            *array_dtype = DBUS_TYPE_ARRAY; break;
        case 4:
            *array_dtype = DBUS_TYPE_INT32; break;
        case 8:
            if (SCM_TYP7 (arg) == scm_tc7_dvect)
                *array_dtype = DBUS_TYPE_DOUBLE;
            else
                *array_dtype = DBUS_TYPE_INT64;
            break;
        default:
            scm_misc_error (FUNC_NAME, "Invalid arg: ~A", scm_list_1 (arg));
        }
    } else if (SCM_NFALSEP (scm_list_p (arg))) {
        SCM walk;
        int all_symbols = TRUE;
        int all_pairs = FALSE;
            
        if (SCM_EQ_P (arg, SCM_EOL))
            scm_misc_error (FUNC_NAME, "Invalid arg: ~A", scm_list_1 (arg));
            
        for (walk = arg; !SCM_EQ_P (walk, SCM_EOL); walk = SCM_CDR (walk)) {
            if (!SCM_SYMBOLP (SCM_CAR (walk))) {
                all_symbols = FALSE;
                break;
            }
        }
            
        if (!all_symbols) {
            all_pairs = TRUE;
            for (walk = arg; !SCM_EQ_P (walk, SCM_EOL); walk = SCM_CDR (walk)) {
                if (SCM_FALSEP (scm_pair_p (SCM_CAR (walk)))) {
                    all_pairs = FALSE;
                    break;
                }
            }
        }
            
        /* three possibilities: */
        if (all_symbols) {
            /* it's an object path */
            *dtype = DBUS_TYPE_OBJECT_PATH;
        } else if (all_pairs) {
            /* it's an alist, which we wrap as a dict */
            *dtype = DBUS_TYPE_DICT;
        } else {
            /* it's an array of something. we require a type descriptor for
               generic arrays. */
            scm_misc_error (FUNC_NAME, "Array needs type descriptor: ~A", scm_list_1 (arg));
        }
    } else {
        scm_misc_error (FUNC_NAME, "Invalid arg: ~A", scm_list_1 (arg));
    }
}

#define CHECKR(_d,x) \
do { \
  if (!(x)) scm_misc_error (FUNC_NAME, "Invalid arg for type: ~A ~A", scm_list_2 (arg, type)); \
  *dtype = DBUS_TYPE_##_d; \
  return; \
} while (0)

#define ACHECKR(_d, _a_dtype,x) \
do { \
  if (!(x)) scm_misc_error (FUNC_NAME, "Invalid arg for type: ~A ~A", scm_list_2 (arg, type)); \
  *dtype = DBUS_TYPE_##_d; \
  *array_dtype = DBUS_TYPE_##_a_dtype; \
  return; \
} while (0)

void
validate_dbus_type (SCM arg, SCM type, int *dtype, int *array_dtype)
{
    if (SCM_SYMBOLP (type)) {
        if (SCM_EQ_P (type, scm_str2symbol ("nil")))
            CHECKR (NIL, TRUE);
        else if (SCM_EQ_P (type, scm_str2symbol ("byte")))
            CHECKR (BYTE,
                    SCM_INUMP (arg) && SCM_INUM (arg) < 256 && SCM_INUM (arg) >= 0);
        else if (SCM_EQ_P (type, scm_str2symbol ("boolean")))
            CHECKR (BOOLEAN,
                    TRUE);
        else if (SCM_EQ_P (type, scm_str2symbol ("int32")))
            CHECKR (INT32,
                    SCM_INUMP (arg)
                    || SCM_NFALSEP (scm_leq_p (scm_abs (arg), scm_int2num (2147483647L))));
        else if (SCM_EQ_P (type, scm_str2symbol ("uint32")))
            CHECKR (UINT32,
                    SCM_INUMP (arg)
                    || (SCM_NFALSEP (scm_positive_p (arg))
                        && SCM_NFALSEP (scm_leq_p (arg, scm_uint2num (G_MAXUINT)))));
        else if (SCM_EQ_P (type, scm_str2symbol ("int64")))
            CHECKR (INT64,
                    SCM_INUMP (arg)
                    || SCM_NFALSEP (scm_leq_p (scm_abs (arg), scm_long_long2num (G_MAXINT64))));
        else if (SCM_EQ_P (type, scm_str2symbol ("uint64")))
            CHECKR (UINT64,
                    SCM_INUMP (arg)
                    || (SCM_NFALSEP (scm_positive_p (arg))
                        && SCM_NFALSEP (scm_leq_p (arg, scm_ulong_long2num (G_MAXUINT64)))));
        else if (SCM_EQ_P (type, scm_str2symbol ("double")))
            CHECKR (DOUBLE,
                    SCM_NFALSEP (scm_inexact_p (arg)));
        else if (SCM_EQ_P (type, scm_str2symbol ("string")))
            CHECKR (CUSTOM,
                    SCM_STRINGP (arg));
        else if (SCM_EQ_P (type, scm_str2symbol ("custom")))
            CHECKR (STRING,
                    SCM_NFALSEP (scm_pair_p (arg))
                    && SCM_SYMBOLP (SCM_CAR (arg))
                    && SCM_ARRAYP (SCM_CDR (arg))
                    && (scm_uniform_element_size (SCM_CDR (arg)) == 1));
        else if (SCM_EQ_P (type, scm_str2symbol ("object-path"))) {
            SCM walk;
            for (walk = arg; SCM_NNULLP (walk); walk = SCM_CDR (walk))
                if (!SCM_SYMBOLP (SCM_CAR (walk)))
                    CHECKR (OBJECT_PATH, FALSE);
            CHECKR (OBJECT_PATH, TRUE);
        }
    } else if (SCM_NFALSEP (scm_list_p (type))) {
        if (SCM_SYMBOLP (SCM_CAR (type))
            && SCM_EQ_P (SCM_CAR (type), scm_str2symbol ("dict"))) {
            /* a dict, formatted as an alist, where the value of the key is the
             * type */
            SCM walk;
            for (walk = SCM_CDR (type); SCM_NNULLP (walk); walk = SCM_CDR (walk)) {
                SCM pair = scm_assq (SCM_CAAR (walk), arg);
                int sub_type;
                if (SCM_FALSEP (pair))
                    CHECKR (DICT, FALSE);
                validate_dbus_type (SCM_CDR (pair), SCM_CDAR (walk), &sub_type, NULL);
            }
            CHECKR (DICT, TRUE);
        } else if (SCM_NULLP (SCM_CDR (type))) {
            /* an array */
            SCM atype = SCM_CAR (type);
            
            if (SCM_SYMBOLP (atype)) {
                /* first, check for "immediate" arrays */
                if (SCM_EQ_P (atype, scm_str2symbol ("byte")))
                    ACHECKR (ARRAY, BYTE,
                             SCM_ARRAYP (arg) && scm_uniform_element_size (arg) == 1);
                else if (SCM_EQ_P (atype, scm_str2symbol ("int32")))
                    ACHECKR (ARRAY, INT32,
                             SCM_ARRAYP (arg) && scm_uniform_element_size (arg) == 4);
                else if (SCM_EQ_P (atype, scm_str2symbol ("uint32")))
                    ACHECKR (ARRAY, UINT32,
                             SCM_ARRAYP (arg) && scm_uniform_element_size (arg) == 4);
                else if (SCM_EQ_P (atype, scm_str2symbol ("int64")))
                    ACHECKR (ARRAY, INT64,
                             SCM_ARRAYP (arg) && scm_uniform_element_size (arg) == 8);
                else if (SCM_EQ_P (atype, scm_str2symbol ("uint64")))
                    ACHECKR (ARRAY, UINT64,
                             SCM_ARRAYP (arg) && scm_uniform_element_size (arg) == 8);
                else if (SCM_EQ_P (atype, scm_str2symbol ("double")))
                    ACHECKR (ARRAY, DOUBLE,
                             SCM_ARRAYP (arg) && scm_uniform_element_size (arg) == 8);
                /* fixme: be more specific than element size */
            }
            /* atype could still be a symbol */
            
            {
                SCM walk;
                int array_subtype = DBUS_TYPE_INVALID;
                
                for (walk = arg; SCM_NNULLP (walk); walk = SCM_CDR (walk))
                    validate_dbus_type (SCM_CAR (walk), atype, array_dtype, &array_subtype);
            }
            CHECKR (ARRAY, TRUE);
        } else {
            scm_misc_error (FUNC_NAME, "Invalid type descriptor: ~A", scm_list_1 (type));
        }
    }
    scm_misc_error (FUNC_NAME, "Invalid type descriptor: ~A", scm_list_1 (type));
}

dbus_bool_t
dbus_message_iter_append (DBusMessageIter *iter, SCM arg, SCM type)
{
    int dtype = DBUS_TYPE_INVALID, array_dtype = DBUS_TYPE_INVALID;
    
    if (SCM_FALSEP (type))
        detect_dbus_type (arg, &dtype, &array_dtype);
    else
        validate_dbus_type (arg, type, &dtype, &array_dtype);

    switch (dtype) {
    case DBUS_TYPE_INVALID:
        scm_misc_error (FUNC_NAME, "Internal error", SCM_EOL);
        return FALSE; /* shouldn't be reached */
        
    case DBUS_TYPE_NIL:
        return dbus_message_iter_append_nil (iter);

    case DBUS_TYPE_BYTE:
        return dbus_message_iter_append_byte (iter, SCM_INUM (arg));

    case DBUS_TYPE_BOOLEAN:
        return dbus_message_iter_append_boolean (iter, SCM_NFALSEP (arg));

    case DBUS_TYPE_INT32:
        return dbus_message_iter_append_int32 (iter,
                                               scm_num2int (arg, 2, FUNC_NAME));
    case DBUS_TYPE_UINT32:
        return dbus_message_iter_append_uint32 (iter,
                                                scm_num2uint (arg, 2, FUNC_NAME));

    case DBUS_TYPE_INT64:
        return dbus_message_iter_append_int64 (iter,
                                               scm_num2long_long (arg, 2, FUNC_NAME));

    case DBUS_TYPE_UINT64:
        return dbus_message_iter_append_uint64 (iter,
                                                scm_num2ulong_long (arg, 2, FUNC_NAME));

    case DBUS_TYPE_DOUBLE:
        return dbus_message_iter_append_double (iter,
                                                scm_num2double (arg, 2, FUNC_NAME));

    case DBUS_TYPE_STRING:
        return dbus_message_iter_append_string (iter,
                                                SCM_STRING_CHARS (arg));
    case DBUS_TYPE_CUSTOM:
        return dbus_message_iter_append_custom (iter,
                                                SCM_SYMBOL_CHARS (SCM_CAR (arg)), /* ? */
                                                SCM_UVECTOR_BASE (SCM_CDR (arg)),
                                                SCM_UVECTOR_LENGTH (SCM_CDR (arg)));
            
    case DBUS_TYPE_ARRAY:
        {
            switch (array_dtype) {
            case DBUS_TYPE_BYTE:
                return dbus_message_iter_append_byte_array 
                    (iter, SCM_UVECTOR_BASE (arg), SCM_UVECTOR_LENGTH (arg));
            case DBUS_TYPE_INT32:
                return dbus_message_iter_append_int32_array 
                    (iter, SCM_UVECTOR_BASE (arg), SCM_UVECTOR_LENGTH (arg));
            case DBUS_TYPE_UINT32:
                return dbus_message_iter_append_uint32_array 
                    (iter, SCM_UVECTOR_BASE (arg), SCM_UVECTOR_LENGTH (arg));
            case DBUS_TYPE_INT64:
                return dbus_message_iter_append_int64_array 
                    (iter, SCM_UVECTOR_BASE (arg), SCM_UVECTOR_LENGTH (arg));
            case DBUS_TYPE_UINT64:
                return dbus_message_iter_append_uint64_array 
                    (iter, SCM_UVECTOR_BASE (arg), SCM_UVECTOR_LENGTH (arg));
            case DBUS_TYPE_DOUBLE:
                return dbus_message_iter_append_double_array 
                    (iter, SCM_UVECTOR_BASE (arg), SCM_UVECTOR_LENGTH (arg));
            default:
                {
                    SCM walk;
                    DBusMessageIter aiter;
                
                    if (!dbus_message_iter_append_array (iter, &aiter, array_dtype))
                        return FALSE;
                
                    for (walk = arg; SCM_NNULLP (walk); walk = SCM_CDR (walk))
                        if (!dbus_message_iter_append
                            (&aiter, SCM_CAR (walk),
                             SCM_FALSEP (type) ? SCM_BOOL_F : SCM_CAR (type)))
                            return FALSE;
                    return TRUE;
                }
            }
        }
        
    case DBUS_TYPE_DICT:
        {
            DBusMessageIter diter;
            SCM walk;

            if (!dbus_message_iter_append_dict (iter, &diter))
                return FALSE;
            
            for (walk = arg; SCM_NNULLP (walk); walk = SCM_CDR (walk)) {
                SCM pair = SCM_NFALSEP (type) ? scm_assq (SCM_CAAR (walk), type) : SCM_BOOL_F;
                if (!dbus_message_iter_append_dict_key (&diter,
                                                        SCM_SYMBOL_CHARS (SCM_CAAR (walk))))
                    return FALSE;
                if (!dbus_message_iter_append (&diter, SCM_CDR (arg),
                                               SCM_FALSEP (pair) ? SCM_BOOL_F : SCM_CDR (pair)))
                    return FALSE;
            }
            return TRUE;
        }
        
    case DBUS_TYPE_OBJECT_PATH:
        scm_misc_error (FUNC_NAME, "I'm lazy", SCM_EOL);
        return FALSE;
            
    default:
        scm_misc_error (FUNC_NAME, "Unsupported operation: ~A ~A", scm_list_2 (arg, type));
        return FALSE;
    }
}
#undef FUNC_NAME

SCM
_wrap_dbus_connection_get_unix_user (DBusConnection *connection) 
{
    unsigned long uid;
    if (dbus_connection_get_unix_user (connection, &uid))
        return scm_ulong2num (uid);
    else
        return SCM_BOOL_F;
}

DBusMessageIter*
dbus_message_get_iter (DBusMessage *message) 
{
    DBusMessageIter *ret = g_new (DBusMessageIter, 1);
    dbus_message_iter_init (message, ret);
    return ret;
}

DBusMessageIter*
dbus_message_get_append_iter (DBusMessage *message) 
{
    DBusMessageIter *ret = g_new (DBusMessageIter, 1);
    dbus_message_append_iter_init (message, ret);
    return ret;
}
