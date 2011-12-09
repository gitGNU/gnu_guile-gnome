;; guile-gnome
;; Copyright (C) 2004 Free Software Foundation, Inc.

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

;;; Commentary:
;;
;;g-wrap specification for panel-applet.
;;
;;; Code:

(define-module (gnome gw panel-applet-spec)
  #:use-module (oop goops)
  #:use-module (g-wrap)
  #:use-module (g-wrap guile)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw gtk-spec)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject))

(define-class <panel-applet-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-panel-applet
  #:dependencies '(standard gnome-glib gnome-gobject gnome-gtk))

(define-method (initialize (ws <panel-applet-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw panel-applet) initargs)))
  
  (load-defs-with-overrides ws "gnome/defs/panel-applet.defs"))

(define-method (global-declarations-cg (self <panel-applet-wrapset>))
  (list (next-method)
        "#include <panel-applet.h>\n"
        "#include <panel-applet-enums.h>\n"
        "#include <panel-applet-gconf.h>\n"
        "#include \"panel-applet-support.h\"\n"))
