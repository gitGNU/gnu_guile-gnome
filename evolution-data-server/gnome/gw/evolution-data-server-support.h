/* guile-gnome
 * Copyright (C) 2004 Free Software Foundation, Inc.
 *
 * evolution-data-server-support.h:
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

#include <libecal/e-cal.h>
#include "ical-support.h"
#include <libedataserver/e-source-list.h>
#include <libguile.h>
#include <guile-gnome-gobject.h>

ECal* wrap_e_cal_open_default (ECalSourceType type, SCM auth_proc, GError **error);
void wrap_e_cal_generate_instances (ECal *ecal, SCM start, SCM end, SCM instance_proc);

#define PROTO_GETTER(field,type)\
  type wrap_e_cal_component_get_##field (ECalComponent *comp)
#define PROTO_SETTER(field,type)\
  void wrap_e_cal_component_set_##field (ECalComponent *comp, type val)
#define PROTO(field,type)\
  PROTO_GETTER (field, type);\
  PROTO_SETTER (field, type);
#define PROTO_FULL(field,g_type,s_type)\
  PROTO_GETTER (field, g_type);\
  PROTO_SETTER (field, s_type);

PROTO_GETTER (categories_list, GSList*);
PROTO_GETTER (comment_list, GSList*);
PROTO_GETTER (contact_list, GSList*);
PROTO_GETTER (description_list, GSList*);

PROTO_GETTER (uid, const char*);
PROTO_GETTER (categories, const char*);
PROTO_GETTER (dtend, ECalComponentDateTime*);
PROTO_GETTER (dtstamp, struct icaltimetype*);
PROTO_GETTER (dtstart, ECalComponentDateTime*);
PROTO_GETTER (due, ECalComponentDateTime*);
PROTO_GETTER (summary, ECalComponentText*);
PROTO_GETTER (url, const char*);
PROTO_GETTER (location, const char*);
