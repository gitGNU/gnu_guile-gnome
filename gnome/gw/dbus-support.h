/* guile-gnome
 * Copyright (C) 2003 Andy Wingo <wingo at pobox dot com>
 *
 * dbus-support.h:
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

#include <dbus/dbus.h>
#include <dbus/dbus-glib.h>
#include <libguile.h>

SCM _wrap_dbus_connection_get_unix_user (DBusConnection *connection);

SCM dbus_message_iter_get (DBusMessageIter *iter);
dbus_bool_t dbus_message_iter_append (DBusMessageIter *iter, SCM arg, SCM type);

DBusMessageIter* dbus_message_get_iter (DBusMessage *message);
DBusMessageIter* dbus_message_get_append_iter (DBusMessage *message);


