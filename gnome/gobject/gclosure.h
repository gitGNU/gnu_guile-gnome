/* guile-gnome
 * Copyright (C) 2003 Andy Wingo <wingo at pobox dot com>
 *
 * gclosure.h: Support for GClosure
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

#ifndef __GUILE_GNOME_GOBJECT_CLOSURES_H__
#define __GUILE_GNOME_GOBJECT_CLOSURES_H__

#include <guile-gnome-gobject/gvalue.h>

G_BEGIN_DECLS

extern SCM scm_class_gclosure;

#define SCM_GCLOSUREP(x) SCM_HACKY_IS_A_P (x, scm_class_gclosure)

/* Although closures are stored as GValues, they cannot be manipulated with the
   gvalue-primitive API. */
SCM scm_gclosure_primitive_new (SCM func);
SCM scm_gclosure_primitive_invoke (SCM instance, SCM return_type, SCM args);

G_END_DECLS

#endif /* __GUILE_GNOME_GOBJECT_CLOSURES_H__ */
