#! /usr/bin/guile -s
!#
;; guile-gnome
;; Copyright (C) 2003,2004 Free Software Foundation, Inc.

;; This program is free software; you can redistribute it and/or    
;; modify it under the terms of the GNU General Public License as   
;; published by the Free Software Foundation; either version 2 of   
;; the License, or (at your option) any later version.              
;;                                                                  
;; This program is distributed in the hope that it will be useful,  
;; but WITHOUT ANY WARRANTY; without even the implied warranty of   
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    
;; GNU General Public License for more details.                     
;;                                                                  
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, contact:
;;
;; Free Software Foundation           Voice:  +1-617-542-5942
;; 59 Temple Place - Suite 330        Fax:    +1-617-542-2652
;; Boston, MA  02111-1307,  USA       gnu@gnu.org


;; Demonstrates how to construct caps with scheme.

(use-modules (gnome-2)
             (gnome gobject)
             (gnome gstreamer debug) ; debug-caps
             (gnome gstreamer)) ; for some ungodly reason this must be
			        ; after debug -- don't know why yet

(display "\nCaps of audioconvert, instantiated from scheme:\n\n")

(debug-caps
 (gst-caps-new
  "audio/x-raw-int"
  `((channels   ,<gst-int-range>  (1 . 2))
    (rate       ,<gst-int-range>  (8000 . 192000))
    (endianness ,<gst-value-list> (,<gint> 1234 4321))
    (signed     ,<gst-value-list> (,<gboolean> #f #t))
    (depth      ,<gst-int-range>  (1 . 32))
    (signed     ,<gst-value-list> (,<gint> 8 16 24 32)))))

(newline)

;; look ma, no segfault
