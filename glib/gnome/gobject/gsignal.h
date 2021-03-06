/* guile-gnome
 * Copyright (C) 2003,2008 Andy Wingo <wingo at pobox dot com>
 *
 * gsignal.h: Support for GSignal
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

#ifndef __GUILE_GNOME_GOBJECT_GSIGNAL_H__
#define __GUILE_GNOME_GOBJECT_GSIGNAL_H__

#include <guile-gnome-gobject/gclosure.h>

G_BEGIN_DECLS

SCM scm_gtype_get_signals (SCM type);
SCM scm_gsignal_create (SCM signal, SCM closure);
SCM scm_gsignal_handler_block (SCM instance, SCM handler_id);
SCM scm_gsignal_handler_unblock (SCM instance, SCM handler_id);
SCM scm_gsignal_handler_disconnect (SCM instance, SCM handler_id);
SCM scm_gsignal_handler_connected_p (SCM instance, SCM handler_id);

G_END_DECLS

#endif
