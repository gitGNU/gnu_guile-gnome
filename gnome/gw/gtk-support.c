/* guile-gnome
 * Copyright (C) 2003 Andy Wingo <wingo at pobox dot com>
 *
 * gtk-support.c: Customizations for guile-gtk
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
 *
 * Based on work Copyright (C) 2003 James Henstridge <james@daa.com.au>
 */

#include <string.h>
#include "gtk-support.h"


/* python-gtk defines a nice series of classes to access rows as elements of a
   sequence and to access their fields via a sequence interface. I wish we could
   do that in guile. Food for thought. */

SCM
guile_gtk_tree_path_to_scm (GtkTreePath *path)
{
    gint len, i, *indices;
    SCM ret;

    len = gtk_tree_path_get_depth(path);
    indices = gtk_tree_path_get_indices(path);
    ret = SCM_EOL;
    for (i = len - 1; i >= 0; i--)
	ret = scm_cons (SCM_MAKINUM (indices[i]), ret);
    return ret;
}

GtkTreePath*
guile_gtk_scm_to_tree_path (SCM scm)
#define FUNC_NAME "guile-gtk-scm-to-tree-path"
{
    if (SCM_STRINGP (scm)) {
	return gtk_tree_path_new_from_string (SCM_STRING_CHARS (scm));
    } else if (SCM_INUMP (scm)) {
        GtkTreePath *ret = gtk_tree_path_new ();
        gtk_tree_path_append_index (ret, SCM_INUM (scm));
        return ret;
    } else if (SCM_EQ_P (scm, SCM_EOL)) {
        return NULL;
    } else if (SCM_NFALSEP (scm_list_p (scm))) {
	GtkTreePath *path = gtk_tree_path_new ();
        
        for (; !SCM_EQ_P (scm, SCM_EOL); scm = SCM_CDR (scm))
            gtk_tree_path_append_index (path, SCM_NUM2INT (0, SCM_CAR (scm)));

	return path;
    }
    return NULL;
}
#undef FUNC_NAME

#define GET_ACTION_STR(var,n)\
do {\
    if (n < l) {\
        SCM x = scm_list_ref (entry, SCM_MAKINUM (n));\
        SCM_ASSERT (SCM_FALSEP (x) || SCM_STRINGP (x), entry,\
                    2, FUNC_NAME);\
        var = SCM_FALSEP (x) ? NULL : SCM_STRING_CHARS (x);\
    } else {\
        var = NULL;\
    }\
}while (0)
#define GET_ACTION_BOOL(var,n)\
do {\
    if (n < l) {\
        var = SCM_NFALSEP (scm_list_ref (entry, SCM_MAKINUM (n)));\
    } else {\
        var = FALSE;\
    }\
}while (0)
#define GET_ACTION_INUM(var,n)\
do {\
    if (n < l) {\
        SCM x = scm_list_ref (entry, SCM_MAKINUM (n));\
        SCM_ASSERT (SCM_INUMP (x), entry,\
                    2, FUNC_NAME);\
        var = SCM_INUM (x);\
    } else {\
        var = FALSE;\
    }\
}while (0)

void
_wrap_gtk_action_group_add_actions (GtkActionGroup *action_group,
                                    SCM entries)
#define FUNC_NAME "gtk-action-group-add-actions"
{
    static SCM connect = SCM_BOOL_F;
    SCM entry, x;
    GtkAction *action;
    char *name, *stock_id, *label, *accelerator, *tooltip;
    gint l;

    SCM_VALIDATE_NONEMPTYLIST (1, entries);

    if (SCM_FALSEP (connect))
        connect = SCM_VARIABLE_REF (scm_c_module_lookup
                                    (scm_c_resolve_module ("gnome gobject"),
                                     "gtype-instance-signal-connect"));

    for (; SCM_NNULLP (entries); entries = SCM_CDR (entries)) {
        entry = SCM_CAR (entries);
        SCM_VALIDATE_NONEMPTYLIST (1, entry);
        l = scm_ilength (entry);
        SCM_ASSERT (l > 0 && l <= 6, entry, 2, FUNC_NAME);
		
        GET_ACTION_STR (name, 0);
        SCM_ASSERT (name != NULL, entry, 2, FUNC_NAME);
        GET_ACTION_STR (stock_id, 1);
        GET_ACTION_STR (label, 2);
        GET_ACTION_STR (accelerator, 3);
        GET_ACTION_STR (tooltip, 4);

        action = g_object_new (GTK_TYPE_ACTION,
                               "name", name, "label", label, "tooltip", tooltip,
                               "stock_id", stock_id, NULL);

        if (5 < l && SCM_NFALSEP ((x = scm_list_ref (entry, SCM_MAKINUM (5)))))
            scm_call_3 (connect,
                        scm_c_gtype_instance_to_scm ((GTypeInstance*)action),
                        scm_str2symbol ("activate"), x);

        gtk_action_group_add_action_with_accel (action_group, action, accelerator);
        g_object_unref (action);
    }
}
#undef FUNC_NAME

void
_wrap_gtk_action_group_add_toggle_actions (GtkActionGroup *action_group,
                                           SCM entries)
#define FUNC_NAME "gtk-action-group-add-toggle-actions"
{
    static SCM connect = SCM_BOOL_F;
    SCM entry, x;
    GtkToggleAction *action;
    char *name, *stock_id, *label, *accelerator, *tooltip;
    gboolean is_active;
    gint l;

    SCM_VALIDATE_NONEMPTYLIST (1, entries);

    if (SCM_FALSEP (connect))
        connect = SCM_VARIABLE_REF (scm_c_module_lookup
                                    (scm_c_resolve_module ("gnome gobject"),
                                     "gtype-instance-signal-connect"));

    for (; SCM_NNULLP (entries); entries = SCM_CDR (entries)) {
        entry = SCM_CAR (entries);
        SCM_VALIDATE_NONEMPTYLIST (1, entry);
        l = scm_ilength (entry);
        SCM_ASSERT (l > 0 && l <= 7, entry, 2, FUNC_NAME);
		
        GET_ACTION_STR (name, 0);
        SCM_ASSERT (name != NULL, entry, 2, FUNC_NAME);
        GET_ACTION_STR (stock_id, 1);
        GET_ACTION_STR (label, 2);
        GET_ACTION_STR (accelerator, 3);
        GET_ACTION_STR (tooltip, 4);

        action = g_object_new (GTK_TYPE_TOGGLE_ACTION,
                               "name", name, "label", label, "tooltip", tooltip,
                               "stock_id", stock_id, NULL);

        if (5 < l && SCM_NFALSEP ((x = scm_list_ref (entry, SCM_MAKINUM (5)))))
            scm_call_3 (connect,
                        scm_c_gtype_instance_to_scm ((GTypeInstance*)action),
                        scm_str2symbol ("activate"), x);

        GET_ACTION_BOOL (is_active, 6);
        gtk_toggle_action_set_active (action, is_active);

        gtk_action_group_add_action_with_accel (action_group, GTK_ACTION (action), accelerator);
        g_object_unref (action);
    }
}
#undef FUNC_NAME

static void
action_group_radio_actions_callback(GtkAction *action,
                                    GtkRadioAction *current,
                                    gpointer user_data)
{
    SCM proc;

    proc = SCM_PACK (GPOINTER_TO_INT (user_data));

    scm_call_2 (proc,
                scm_c_gtype_instance_to_scm ((GTypeInstance*) action),
                scm_c_gtype_instance_to_scm ((GTypeInstance*) current));
	
}

void
_wrap_gtk_action_group_add_radio_actions (GtkActionGroup *action_group,
                                          SCM entries,
                                          gint value,
                                          SCM on_change)
#define FUNC_NAME "gtk-action-group-add-radio-actions"
{
    GtkRadioActionEntry *raes;
    SCM entry;    
    gint len, i, l;

    SCM_VALIDATE_NONEMPTYLIST (1, entries);
    SCM_VALIDATE_PROC (4, on_change);

    len = scm_ilength (entries);
    raes = g_new0 (GtkRadioActionEntry, len);

    for (i=0; SCM_NNULLP (entries); entries = SCM_CDR (entries), i++) {
        entry = SCM_CAR (entries);
        SCM_VALIDATE_NONEMPTYLIST (1, entry);
        l = scm_ilength (entry);
        SCM_ASSERT (l == 6, entry, 2, FUNC_NAME);
		
        GET_ACTION_STR (raes[i].name, 0);
        SCM_ASSERT (raes[i].name != NULL, entry, 2, FUNC_NAME);
        GET_ACTION_STR (raes[i].stock_id, 1);
        GET_ACTION_STR (raes[i].label, 2);
        GET_ACTION_STR (raes[i].accelerator, 3);
        GET_ACTION_STR (raes[i].tooltip, 4);
        GET_ACTION_INUM (raes[i].value, 5);
    }

    gtk_action_group_add_radio_actions (action_group, raes, len, value,
                                        G_CALLBACK (action_group_radio_actions_callback),
                                        GINT_TO_POINTER (SCM_UNPACK (on_change)));

    g_free (raes);
}
#undef FUNC_NAME

void
_wrap_gtk_clipboard_set_text (GtkClipboard *clipboard, const gchar *text)
{
    gtk_clipboard_set_text (clipboard, text, strlen (text));
}

gint
_wrap_gtk_editable_insert_text (GtkEditable *editable, const gchar *text, gint pos)
{
    gtk_editable_insert_text (editable, text, strlen (text), &pos);
    return pos;
}

GtkWidget*
_wrap_gtk_dialog_get_vbox (GtkDialog *dialog)
{
    return dialog->vbox;
}

GtkWidget*
_wrap_gtk_dialog_get_action_area (GtkDialog *dialog)
{
    return dialog->action_area;
}

GtkWidget*
_wrap_gtk_color_selection_dialog_get_colorsel (GtkColorSelectionDialog *dialog)
{
    return dialog->colorsel;
}

GtkWidget*
_wrap_gtk_color_selection_dialog_get_ok_button (GtkColorSelectionDialog *dialog)
{
    return dialog->ok_button;
}

GtkWidget*
_wrap_gtk_color_selection_dialog_get_cancel_button (GtkColorSelectionDialog *dialog)
{
    return dialog->cancel_button;
}

GtkWidget*
_wrap_gtk_color_selection_dialog_get_help_button (GtkColorSelectionDialog *dialog)
{
    return dialog->help_button;
}

GtkWidget*
_wrap_gtk_file_selection_get_ok_button (GtkFileSelection* selection)
{
    return selection->ok_button;
}

GtkWidget*
_wrap_gtk_file_selection_get_cancel_button (GtkFileSelection* selection)
{
    return selection->cancel_button;
}

GtkListStore*
_wrap_gtk_list_store_new (SCM col_types) 
#define FUNC_NAME "gtk-list-store-new"
{
    GType *col_gtypes;
    gint len, i;
    GtkListStore *ret;
    
    SCM_VALIDATE_NONEMPTYLIST (1, col_types);
    len = scm_ilength (col_types);
    col_gtypes = g_new (GType, len);

    for (i=0; i<len; i++) {
        SCM v = SCM_CAR (col_types);
        if (SCM_GTYPEP (v))
            col_gtypes[i] = (GType)SCM_SMOB_DATA (v);
        else if (SCM_GTYPE_CLASSP (v))
            col_gtypes[i] = (GType)SCM_SMOB_DATA (scm_slot_ref
                                                  (v, scm_str2symbol ("gtype")));
        else {
            g_free (col_gtypes);
            scm_wrong_type_arg (FUNC_NAME, 1, v);
        }
        col_types = SCM_CDR (col_types);
    }
    
    ret = gtk_list_store_newv (len, col_gtypes);
    g_free (col_gtypes);
    return ret;
}
#undef FUNC_NAME

void
_wrap_gtk_list_store_set_value (GtkListStore *store, GtkTreeIter *iter,
                                gint column, SCM scm) 
{
    GValue *value;
    GType type;

    SCM_ASSERT (column < gtk_tree_model_get_n_columns (GTK_TREE_MODEL (store)),
                SCM_MAKINUM (column), 3, "gtk-list-store-set-value");
    type = gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), column);
    value = scm_c_scm_to_gvalue (type, scm);

    gtk_list_store_set_value (store, iter, column, value);
    g_value_unset (value);
    g_free (value);
}

GtkTreeIter*
_wrap_gtk_list_store_remove (GtkListStore *store, GtkTreeIter *iter) 
{
    if (!gtk_list_store_remove (store, iter))
        return NULL;
    return gtk_tree_iter_copy (iter);
}

GtkTreeIter*
_wrap_gtk_list_store_insert (GtkListStore *store, gint position)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_list_store_insert (store, new, position);
    return new;
}

GtkTreeIter*
_wrap_gtk_list_store_insert_before (GtkListStore *store, GtkTreeIter *sibling)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_list_store_insert_before (store, new, sibling);
    return new;
}

GtkTreeIter*
_wrap_gtk_list_store_insert_after (GtkListStore *store, GtkTreeIter *sibling)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_list_store_insert_after (store, new, sibling);
    return new;
}

GtkTreeIter*
_wrap_gtk_list_store_prepend (GtkListStore *store)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_list_store_prepend (store, new);
    return new;
}

GtkTreeIter*
_wrap_gtk_list_store_append (GtkListStore *store)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_list_store_append (store, new);
    return new;
}

SCM
_wrap_gtk_message_dialog_new (GtkWindow* parent, GtkDialogFlags flags, GtkMessageType type,
                              GtkButtonsType buttons, const gchar *text)
{
    SCM ret;
    GtkWidget *w = gtk_message_dialog_new (parent, flags, type, buttons, "%s", text);
    g_object_ref (w); /* the initial ref belongs to GTK+ */
    SCM_NEWSMOB2 (ret, scm_tc16_gtype_instance, w, NULL);
    return ret;
}

gchar*
_gtk_selection_data_get_as_string (GtkSelectionData *data) 
{
    return g_strndup (data->data, data->length);
}

void
_wrap_gtk_stock_add (SCM items)
#define FUNC_NAME "gtk-stock-add"
{
    int len, i;
    GtkStockItem *stockitems;

    SCM_ASSERT (SCM_NIMP (items) && SCM_CONSP (items) && SCM_NNULLP (items),
                items, 1, FUNC_NAME);

    len = scm_ilength (items);

    stockitems = g_new0 (GtkStockItem, len);
	
    for (i = 0; i < len ; i++) {
        SCM item = SCM_CAR(items);

        SCM_ASSERT (SCM_NIMP (item) && SCM_CONSP (item) &&
                    scm_ilength(item) == 5 &&
                    SCM_STRINGP (SCM_CAR (item)) &&
                    SCM_STRINGP (SCM_CADR (item)) &&
                    SCM_INUMP   (SCM_CADDR (item)) &&
                    SCM_INUMP   (SCM_CADDDR (item)),
                    item, 1, FUNC_NAME);

        stockitems[i].stock_id = SCM_STRING_CHARS (SCM_CAR (item));
        stockitems[i].label    = SCM_STRING_CHARS (SCM_CADR (item));
        stockitems[i].modifier = SCM_INUM (SCM_CADDR (item));
        stockitems[i].keyval   = (guint) SCM_INUM (SCM_CADDDR (item));
        stockitems[i].translation_domain =
            SCM_STRINGP (SCM_CADDDR (SCM_CDR (item))) ?
            SCM_STRING_CHARS (SCM_CADDDR (SCM_CDR (item))) : NULL;
		
        items = SCM_CDR(items);
    }

    gtk_stock_add (stockitems, len);
    g_free(stockitems);
}
#undef FUNC_NAME

SCM
_wrap_gtk_stock_lookup (const gchar *stock_id)
#define FUNC_NAME "gtk-stock-add"
{
    GtkStockItem item;
	
    if (gtk_stock_lookup (stock_id, &item)) {
        return SCM_LIST5 (scm_makfrom0str (item.stock_id),
                          scm_makfrom0str (item.label),
                          SCM_MAKINUM (item.modifier),
                          SCM_MAKINUM (item.keyval),
                          scm_makfrom0str (item.translation_domain));
    }

    return SCM_BOOL_F;

}
#undef FUNC_NAME

void
_wrap_gtk_text_buffer_set_text (GtkTextBuffer *buf, SCM stext)
#define FUNC_NAME "gtk-text-buffer-set-text"
{
    SCM_VALIDATE_STRING (2, stext);
    gtk_text_buffer_set_text (buf, SCM_STRING_CHARS (stext), SCM_STRING_LENGTH (stext));
}
#undef FUNC_NAME

void
_wrap_gtk_text_buffer_insert (GtkTextBuffer *buf, GtkTextIter* iter, SCM stext)
#define FUNC_NAME "gtk-text-buffer-insert"
{
    SCM_VALIDATE_STRING (3, stext);
    gtk_text_buffer_insert (buf, iter, SCM_STRING_CHARS (stext), SCM_STRING_LENGTH (stext));
}
#undef FUNC_NAME

void
_wrap_gtk_text_buffer_insert_at_cursor (GtkTextBuffer *buf, SCM stext)
#define FUNC_NAME "gtk-text-buffer-insert-at-cursor"
{
    SCM_VALIDATE_STRING (2, stext);
    gtk_text_buffer_insert_at_cursor (buf, SCM_STRING_CHARS (stext), SCM_STRING_LENGTH (stext));
}
#undef FUNC_NAME

gboolean
_wrap_gtk_text_buffer_insert_interactive (GtkTextBuffer *buf, GtkTextIter* iter,
                                                   SCM stext, gboolean default_editable)
#define FUNC_NAME "gtk-text-buffer-insert-interactive"
{
    SCM_VALIDATE_STRING (3, stext);
    return gtk_text_buffer_insert_interactive (buf, iter, SCM_STRING_CHARS (stext),
                                               SCM_STRING_LENGTH (stext), default_editable);
}
#undef FUNC_NAME

gboolean
_wrap_gtk_text_buffer_insert_interactive_at_cursor (GtkTextBuffer *buf, SCM stext,
                                                    gboolean default_editable)
#define FUNC_NAME "gtk-text-buffer-insert-interactive-at-cursor"
{
    SCM_VALIDATE_STRING (2, stext);
    return gtk_text_buffer_insert_interactive_at_cursor (buf, SCM_STRING_CHARS (stext),
                                                         SCM_STRING_LENGTH (stext),
                                                         default_editable);
}
#undef FUNC_NAME

void
_wrap_gtk_text_buffer_insert_with_tags (GtkTextBuffer *buf, GtkTextIter* iter,
                                        SCM stext, GList* tag_list)
#define FUNC_NAME "gtk-text-buffer-insert-with-tags"
{
    GtkTextIter start;
    GList *walk;
    gint start_offset;
    
    /* based on gtktextbuffer.c (LGPL) */

    SCM_VALIDATE_STRING (3, stext);
    start_offset = gtk_text_iter_get_offset (iter);
    gtk_text_buffer_insert (buf, iter, SCM_STRING_CHARS (stext),
                            SCM_STRING_LENGTH (stext));
    gtk_text_buffer_get_iter_at_offset (buf, &start, start_offset);
    
    for (walk = tag_list; walk; walk = walk->next)
        gtk_text_buffer_apply_tag (buf, (GtkTextTag*)walk->data, &start, iter);
    g_list_free (tag_list);
}
#undef FUNC_NAME

void
_wrap_gtk_text_buffer_insert_with_tags_by_name (GtkTextBuffer *buf, GtkTextIter* iter,
                                                SCM stext, GList* tag_list)
#define FUNC_NAME "gtk-text-buffer-insert-with-tags-by-name"
{
    GtkTextIter start;
    GList *walk;
    gint start_offset;
    
    /* based on gtktextbuffer.c (LGPL) */

    SCM_VALIDATE_STRING (3, stext);
    start_offset = gtk_text_iter_get_offset (iter);
    gtk_text_buffer_insert (buf, iter, SCM_STRING_CHARS (stext),
                            SCM_STRING_LENGTH (stext));
    gtk_text_buffer_get_iter_at_offset (buf, &start, start_offset);
    
    for (walk = tag_list; walk; walk = walk->next)
        gtk_text_buffer_apply_tag (buf,
                                   gtk_text_tag_table_lookup (buf->tag_table,
                                                              (gchar*)walk->data),
                                   &start, iter);
    g_list_free (tag_list);
}
#undef FUNC_NAME

GtkTextIter*
_wrap_gtk_text_buffer_get_iter_at_line_offset (GtkTextBuffer *buf, gint line_number,
                                               gint char_offset)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_iter_at_line_offset (buf, iter, line_number, char_offset);
    return iter;
}

GtkTextIter*
_wrap_gtk_text_buffer_get_iter_at_line_index (GtkTextBuffer *buf, gint line_number,
                                              gint byte_index)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_iter_at_line_index (buf, iter, line_number, byte_index);
    return iter;
}

GtkTextIter*
_wrap_gtk_text_buffer_get_iter_at_offset (GtkTextBuffer *buf, gint char_offset)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_iter_at_offset (buf, iter, char_offset);
    return iter;
}

GtkTextIter*
_wrap_gtk_text_buffer_get_iter_at_line (GtkTextBuffer *buf, gint line_number)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_iter_at_line (buf, iter, line_number);
    return iter;
}

GtkTextIter*
_wrap_gtk_text_buffer_get_start_iter (GtkTextBuffer *buf)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_start_iter (buf, iter);
    return iter;
}

GtkTextIter*
_wrap_gtk_text_buffer_get_end_iter (GtkTextBuffer *buf)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_end_iter (buf, iter);
    return iter;
}

SCM
_wrap_gtk_text_buffer_get_bounds (GtkTextBuffer *buf)
{
    GtkTextIter *start, *end;
    SCM sstart, send;
    
    start = g_new0 (GtkTextIter, 1);
    end = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_bounds (buf, start, end);
    sstart = scm_c_make_gvalue (GTK_TYPE_TEXT_ITER);
    send = scm_c_make_gvalue (GTK_TYPE_TEXT_ITER);
    g_value_set_boxed_take_ownership ((GValue*)SCM_SMOB_DATA (sstart), start);
    g_value_set_boxed_take_ownership ((GValue*)SCM_SMOB_DATA (send), end);
    return scm_values (SCM_LIST2 (sstart, send));
}

SCM
_wrap_gtk_text_buffer_get_selection_bounds (GtkTextBuffer *buf)
{
    GtkTextIter *start, *end;
    SCM sstart, send;
    
    start = g_new0 (GtkTextIter, 1);
    end = g_new0 (GtkTextIter, 1);
    if (gtk_text_buffer_get_selection_bounds (buf, start, end)) {
        sstart = scm_c_make_gvalue (GTK_TYPE_TEXT_ITER);
        send = scm_c_make_gvalue (GTK_TYPE_TEXT_ITER);
        g_value_set_boxed_take_ownership ((GValue*)SCM_SMOB_DATA (sstart), start);
        g_value_set_boxed_take_ownership ((GValue*)SCM_SMOB_DATA (send), end);
        return scm_values (SCM_LIST2 (sstart, send));
    }
    g_free (start);
    g_free (end);
    return scm_values (SCM_LIST2 (SCM_BOOL_F, SCM_BOOL_F));
}

GtkTextIter*
_wrap_gtk_text_buffer_get_iter_at_mark (GtkTextBuffer *buf, GtkTextMark* mark)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_iter_at_mark (buf, iter, mark);
    return iter;
}

GtkTextIter*
_wrap_gtk_text_buffer_get_iter_at_child_anchor (GtkTextBuffer *buf,
                                                GtkTextChildAnchor* anchor)
{
    GtkTextIter* iter = g_new0 (GtkTextIter, 1);
    gtk_text_buffer_get_iter_at_child_anchor (buf, iter, anchor);
    return iter;
}

GtkTreeIter*
_wrap_gtk_tree_model_get_iter (GtkTreeModel *model, GtkTreePath *path)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    
    if (gtk_tree_model_get_iter (model, new, path))
        return new;
    g_free (new);
    return NULL;
}

GtkTreeIter*
_wrap_gtk_tree_model_get_iter_first (GtkTreeModel *model) 
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    
    if (gtk_tree_model_get_iter_first (model, new))
        return new;
    g_free (new);
    return NULL;
}

SCM
_wrap_gtk_tree_model_get_value (GtkTreeModel *model, GtkTreeIter *iter, gint column) 
{
    GValue value = { 0, };
    
    gtk_tree_model_get_value (model, iter, column, &value);

    return scm_c_gvalue_to_scm (&value);
}

GtkTreeIter*
_wrap_gtk_tree_model_iter_next (GtkTreeModel *model, GtkTreeIter *iter) 
{
    GtkTreeIter *new = gtk_tree_iter_copy (iter);

    if (gtk_tree_model_iter_next (model, new))
        return new;
    g_free (new);
    return NULL;
}
    
GList*
_wrap_gtk_tree_model_iter_children (GtkTreeModel *model, GtkTreeIter *iter)
{
    GList *list = NULL;
    GtkTreeIter *prev = g_new0 (GtkTreeIter, 1);
    
    if (gtk_tree_model_iter_children (model, prev, iter)) {
        while (1) {
            GtkTreeIter *new = g_new0 (GtkTreeIter, 1);

            list = g_list_prepend (list, prev);
            *new = *prev;

            if (!gtk_tree_model_iter_next (model, new)) {
                g_free (new);
                return g_list_reverse (list);
            }
            prev = new;
        }
    }
    g_free (prev);
    return NULL;
}

GtkTreeIter*
_wrap_gtk_tree_model_iter_parent (GtkTreeModel *model, GtkTreeIter *iter)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    
    if (gtk_tree_model_iter_parent (model, new, iter))
        return new;
    g_free (new);
    return NULL;
}

GtkTreeIter*
_wrap_gtk_tree_model_iter_nth_child (GtkTreeModel *model, GtkTreeIter *iter, gint n)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    
    if (gtk_tree_model_iter_nth_child (model, new, iter, n))
        return new;
    g_free (new);
    return NULL;
}

SCM
_wrap_gtk_tree_selection_get_selected (GtkTreeSelection *selection)
{
    GtkTreeIter *iter = g_new0 (GtkTreeIter, 1);
    GtkTreeModel *model = NULL;
    
    if (gtk_tree_selection_get_selected (selection, &model, iter)) {
        SCM smodel, siter;
        g_object_ref (model);
        smodel = scm_c_gtype_instance_to_scm ((GTypeInstance*)model);
        siter = scm_c_make_gvalue (GTK_TYPE_TREE_ITER);
        g_value_set_boxed_take_ownership ((GValue*)SCM_SMOB_DATA (siter), iter);
        return scm_values (SCM_LIST2 (smodel, siter));
    }
    return scm_values (SCM_LIST2 (SCM_BOOL_F, SCM_BOOL_F));
}

GtkTreeStore*
_wrap_gtk_tree_store_new (SCM col_types) 
#define FUNC_NAME "gtk-tree-store-new"
{
    GType *col_gtypes;
    gint len, i;
    GtkTreeStore *ret;
    
    SCM_VALIDATE_NONEMPTYLIST (1, col_types);
    len = scm_ilength (col_types);
    col_gtypes = g_new (GType, len);

    for (i=0; i<len; i++) {
        SCM v = SCM_CAR (col_types);
        if (SCM_GTYPEP (v))
            col_gtypes[i] = (GType)SCM_SMOB_DATA (v);
        else if (SCM_GTYPE_CLASSP (v))
            col_gtypes[i] = (GType)SCM_SMOB_DATA (scm_slot_ref
                                                  (v, scm_str2symbol ("gtype")));
        else {
            g_free (col_gtypes);
            scm_wrong_type_arg (FUNC_NAME, 1, v);
        }
        col_types = SCM_CDR (col_types);
    }
    
    ret = gtk_tree_store_newv (len, col_gtypes);
    g_free (col_gtypes);
    return ret;
}
#undef FUNC_NAME

void
_wrap_gtk_tree_store_set_value (GtkTreeStore *store, GtkTreeIter *iter,
                                gint column, SCM scm) 
{
    GValue *value;
    GType type;

    SCM_ASSERT (column < gtk_tree_model_get_n_columns (GTK_TREE_MODEL (store)),
                SCM_MAKINUM (column), 3, "gtk-tree-store-set-value");
    type = gtk_tree_model_get_column_type (GTK_TREE_MODEL (store), column);
    value = scm_c_scm_to_gvalue (type, scm);

    gtk_tree_store_set_value (store, iter, column, value);
    g_value_unset (value);
    g_free (value);
}

GtkTreeIter*
_wrap_gtk_tree_store_remove (GtkTreeStore *store, GtkTreeIter *iter) 
{
    if (!gtk_tree_store_remove (store, iter))
        return NULL;
    return gtk_tree_iter_copy (iter);
}

GtkTreeIter*
_wrap_gtk_tree_store_insert (GtkTreeStore *store, GtkTreeIter *parent, gint position)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_tree_store_insert (store, new, parent, position);
    return new;
}

GtkTreeIter*
_wrap_gtk_tree_store_insert_before (GtkTreeStore *store, GtkTreeIter *parent,
                                    GtkTreeIter *sibling)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_tree_store_insert_before (store, new, parent, sibling);
    return new;
}

GtkTreeIter*
_wrap_gtk_tree_store_insert_after (GtkTreeStore *store, GtkTreeIter *parent,
                                   GtkTreeIter *sibling)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_tree_store_insert_after (store, new, parent, sibling);
    return new;
}

GtkTreeIter*
_wrap_gtk_tree_store_prepend (GtkTreeStore *store, GtkTreeIter *parent)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_tree_store_prepend (store, new, parent);
    return new;
}

GtkTreeIter*
_wrap_gtk_tree_store_append (GtkTreeStore *store, GtkTreeIter *parent)
{
    GtkTreeIter *new = g_new0 (GtkTreeIter, 1);
    gtk_tree_store_append (store, new, parent);
    return new;
}

static void
cell_data_func (GtkTreeViewColumn *tree_column, GtkCellRenderer *cell,
                GtkTreeModel *tree_model, GtkTreeIter *iter, gpointer data)
{
    SCM proc, scolumn, scell, smodel, siter;
    proc = SCM_PACK (GPOINTER_TO_INT (data));
    scolumn = scm_c_gtype_instance_to_scm ((GTypeInstance*)tree_column);
    scell = scm_c_gtype_instance_to_scm ((GTypeInstance*)cell);
    smodel = scm_c_gtype_instance_to_scm ((GTypeInstance*)tree_model);
    siter = scm_c_dup_gboxed_to_scm (GTK_TYPE_TREE_ITER, iter);
    
    scm_call_4 (proc, scolumn, scell, smodel, siter);
}

void
_wrap_gtk_tree_view_column_set_cell_data_func (GtkTreeViewColumn *tree_column,
                                               GtkCellRenderer *cell_renderer,
                                               SCM proc) 
{
    gtk_tree_view_column_set_cell_data_func
        (tree_column, cell_renderer, cell_data_func,
         GINT_TO_POINTER (SCM_UNPACK (scm_gc_protect_object (proc))),
         (GtkDestroyNotify)scm_gc_unprotect_object);
}

void
_wrap_gtk_drag_dest_set (GtkWidget *widget, GtkDestDefaults flags,
                         const GList* types, GdkDragAction actions)
{
    const GList *l;
    gint i, n;
    GtkTargetEntry* entries;

    n = g_list_length ((GList*)types);
    entries = g_new0 (GtkTargetEntry, n);
    for (i=0, l=types; i<n; i++, l=l->next)
        entries[i].target = (gchar*)l->data;

    gtk_drag_dest_set (widget, flags, entries, n, actions);
}

GdkGC*
gtk_style_get_fg_gc (GtkStyle *style, GtkStateType state)
{
    return style->fg_gc[state];
}

GdkGC*
gtk_style_get_bg_gc (GtkStyle *style, GtkStateType state)
{
    return style->bg_gc[state];
}

GdkGC*
gtk_style_get_white_gc (GtkStyle *style)
{
    return style->white_gc;
}

GdkGC*
gtk_style_get_black_gc (GtkStyle *style)
{
    return style->black_gc;
}

GdkWindow*
gtk_widget_get_window (GtkWidget *widget)
{
    return widget->window;
}

GdkRectangle*
gtk_widget_get_allocation (GtkWidget *widget)
{
    GdkRectangle *ret = g_new (GdkRectangle, 1);
    *ret = widget->allocation;
    return ret;
}
