/* guile-gnome
 * Copyright (C) 2003,2004,2010 Andy Wingo <wingo at pobox dot com>
 *
 * gdk-support.c: Support routines for the GDK wrapper
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

#include <libguile.h>
#include "guile-gnome-gobject.h"

#include "gdk-support.h"

#define VSET(scm, i, val) scm_vector_set_x (scm, scm_from_short (i), val)
SCM
gdk_event_to_vector (GdkEvent *event)
{
    switch (event->type) {
    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
        {
            GdkEventKey ekey = event->key;
            SCM ret;

            /* 8 fields (we ignore key.string and key.length) */
            ret = scm_c_make_vector (8, SCM_BOOL_F);
            
            VSET (ret, 0, scm_from_int (event->type));
            VSET (ret, 1, scm_c_gtype_instance_to_scm (ekey.window));
            VSET (ret, 2, ekey.send_event ? SCM_BOOL_T : SCM_BOOL_F);
            VSET (ret, 3, scm_from_ulong (ekey.time));
            VSET (ret, 4, scm_from_int (ekey.state));
            VSET (ret, 5, scm_from_int (ekey.keyval));
            VSET (ret, 6, scm_from_int (ekey.hardware_keycode));
            VSET (ret, 7, scm_from_int (ekey.group));
            return ret;
        }
    case GDK_BUTTON_PRESS:
    case GDK_2BUTTON_PRESS:
    case GDK_3BUTTON_PRESS:
    case GDK_BUTTON_RELEASE:
        {
            GdkEventButton ebutton = event->button;
            SCM ret;

            /* 11 fields (we ignore axes) */
            ret = scm_c_make_vector (11, SCM_BOOL_F);
            
            VSET (ret, 0, scm_from_int (event->type));
            VSET (ret, 1, scm_c_gtype_instance_to_scm (ebutton.window));
            VSET (ret, 2, ebutton.send_event ? SCM_BOOL_T : SCM_BOOL_F);
            VSET (ret, 3, scm_from_ulong (ebutton.time));
            VSET (ret, 4, scm_from_double (ebutton.x));
            VSET (ret, 5, scm_from_double (ebutton.y));
            VSET (ret, 6, scm_from_int (ebutton.state));
            VSET (ret, 7, scm_from_int (ebutton.button));
            VSET (ret, 8, scm_c_gtype_instance_to_scm (ebutton.device));
            VSET (ret, 9, scm_from_double (ebutton.x_root));
            VSET (ret, 10, scm_from_double (ebutton.y_root));
            return ret;
        }
    case GDK_ENTER_NOTIFY:
    case GDK_LEAVE_NOTIFY:
        {
            GdkEventCrossing ecrossing = event->crossing;
            SCM ret;

            /* 13 fields */
            ret = scm_c_make_vector (13, SCM_BOOL_F);

            VSET (ret, 0, scm_from_int (event->type));
            VSET (ret, 1, scm_c_gtype_instance_to_scm (ecrossing.window));
            VSET (ret, 2, ecrossing.send_event ? SCM_BOOL_T : SCM_BOOL_F);
	    /* subwindow may be NULL. --jcn */
	    if (ecrossing.subwindow)
	      VSET (ret, 3, scm_c_gtype_instance_to_scm (ecrossing.subwindow));
	    else
	      VSET (ret, 3, SCM_BOOL_F);
	      
            VSET (ret, 4, scm_from_ulong (ecrossing.time));
            VSET (ret, 5, scm_from_double (ecrossing.x));
            VSET (ret, 6, scm_from_double (ecrossing.y));
            VSET (ret, 7, scm_from_double (ecrossing.x_root));
            VSET (ret, 8, scm_from_double (ecrossing.y_root));
            VSET (ret, 9, scm_from_int (ecrossing.mode));
            VSET (ret, 10, scm_from_int (ecrossing.detail));
            VSET (ret, 11, SCM_BOOL (ecrossing.focus));
            VSET (ret, 12, scm_from_int (ecrossing.state));
            return ret;
        }
    case GDK_SELECTION_CLEAR:
    case GDK_SELECTION_NOTIFY:
    case GDK_SELECTION_REQUEST:
        {
            GdkEventSelection eselection = event->selection;
            SCM ret;

            /* 8 fields */
            ret = scm_c_make_vector (8, SCM_BOOL_F);
            
            VSET (ret, 0, scm_from_int (event->type));
            VSET (ret, 1, scm_c_gtype_instance_to_scm (eselection.window));
            VSET (ret, 2, eselection.send_event ? SCM_BOOL_T : SCM_BOOL_F);
            VSET (ret, 3, scm_take_locale_string (gdk_atom_name (eselection.selection)));
            VSET (ret, 4, scm_take_locale_string (gdk_atom_name (eselection.target)));
            VSET (ret, 5, scm_take_locale_string (gdk_atom_name (eselection.property)));
            VSET (ret, 6, scm_from_ulong (eselection.time));
            VSET (ret, 7, scm_from_int (eselection.requestor));

            return ret;
        }
    case GDK_MOTION_NOTIFY:
        {
            GdkEventMotion emotion = event->motion;
            SCM ret;

            /* 11 fields (we ignore axes) */
            ret = scm_c_make_vector (11, SCM_BOOL_F);
            
            VSET (ret, 0, scm_from_int (event->type));
            VSET (ret, 1, scm_c_gtype_instance_to_scm (emotion.window));
            VSET (ret, 2, emotion.send_event ? SCM_BOOL_T : SCM_BOOL_F);
            VSET (ret, 3, scm_from_ulong (emotion.time));
            VSET (ret, 4, scm_from_double (emotion.x));
            VSET (ret, 5, scm_from_double (emotion.y));
            VSET (ret, 6, scm_from_int (emotion.state));
            VSET (ret, 7, emotion.is_hint ? SCM_BOOL_T : SCM_BOOL_F);
            VSET (ret, 8, 
                            scm_c_gtype_instance_to_scm (emotion.device));
            VSET (ret, 9, scm_from_double (emotion.x_root));
            VSET (ret, 10, scm_from_double (emotion.y_root));
            return ret;
        }
    case GDK_WINDOW_STATE:
        {
            GdkEventWindowState ewinstate = event->window_state;
            SCM ret;

            /* 5 fields */
            ret = scm_c_make_vector (5, SCM_BOOL_F);

            VSET (ret, 0, scm_from_int (event->type));
            VSET (ret, 1, scm_c_gtype_instance_to_scm (ewinstate.window));
            VSET (ret, 2, ewinstate.send_event ? SCM_BOOL_T : SCM_BOOL_F);
            VSET (ret, 3, scm_from_int (ewinstate.changed_mask));
            VSET (ret, 4, scm_from_int (ewinstate.new_window_state));
            
            return ret;
        }
    case GDK_EXPOSE:
        {
            GdkEventExpose expose = event->expose;
            SCM ret;

            /* 6 fields */
            ret = scm_c_make_vector (6, SCM_BOOL_F);

            VSET (ret, 0, scm_from_int (event->type));
            VSET (ret, 1, scm_c_gtype_instance_to_scm (expose.window));
            VSET (ret, 2, SCM_BOOL (expose.send_event));
            VSET (ret, 3, scm_gdk_rectangle_to_scm (&expose.area));
            VSET (ret, 4, SCM_BOOL_F); /* FIXME: region */
            VSET (ret, 5, scm_from_int (expose.count));
            return ret;
        }
    default:
        g_print ("Conversions for events of type %d are not implemented.\n"
                 "How about doing it yourself?\n", event->type);
        return SCM_BOOL_F;
    }
}

SCM
scm_gdk_rectangle_to_scm (GdkRectangle *rect)
{
    SCM ret = scm_c_make_vector (4, SCM_BOOL_F);

    VSET (ret, 0, scm_from_int (rect->x));
    VSET (ret, 1, scm_from_int (rect->y));
    VSET (ret, 2, scm_from_int (rect->width));
    VSET (ret, 3, scm_from_int (rect->height));

    return ret;
}

static GdkRectangle*
gdk_rectangle_new (void)
{
  GdkRectangle tmp;
  return g_boxed_copy (GDK_TYPE_RECTANGLE, &tmp);
}

GdkRectangle*
scm_scm_to_gdk_rectangle (SCM scm)
#define FUNC_NAME "%scm->gdk-rectangle"
{
  GdkRectangle *ret = gdk_rectangle_new ();
    
#define GET_VINT(v,i)                                                   \
  scm_to_int (scm_vector_ref (v, scm_from_int(i)))

  ret->x = GET_VINT (scm, 0);
  ret->y = GET_VINT (scm, 1);
  ret->width = GET_VINT (scm, 2);
  ret->height = GET_VINT (scm, 3);

  return ret;
}
#undef FUNC_NAME

SCM
scm_gdk_color_to_scm (GdkColor *c)
{
    SCM ret = scm_c_make_vector (3, SCM_BOOL_F);

    VSET (ret, 0, scm_from_ushort (c->red));
    VSET (ret, 1, scm_from_ushort (c->green));
    VSET (ret, 2, scm_from_ushort (c->blue));

    return ret;
}

/*
 * Allocate a new GdkColor* which must be freed using
 * gdk_color_free().
 */
static GdkColor*
gdk_color_new (void)
{
  GdkColor tmp;
  return gdk_color_copy (&tmp);
}

GdkColor*
scm_scm_to_gdk_color (SCM scm)
#define FUNC_NAME "%scm->gdk-rectangle"
{
  GdkColor *ret = gdk_color_new();
    
  if (scm_is_string (scm)) {
    char *chars;
    gboolean success;

    chars = scm_to_locale_string (scm);
    success = gdk_color_parse (chars, ret);
    free (chars);
    if (success)
      return ret;
    /* FIXME: give a proper error */
  }
    
#define GET_VUSHORT(v,i)                                                \
  scm_to_ushort (scm_vector_ref (v, scm_from_int (i)))

  ret->red = GET_VUSHORT (scm, 0);
  ret->green = GET_VUSHORT (scm, 1);
  ret->blue = GET_VUSHORT (scm, 2);

  return ret;
}
#undef FUNC_NAME
