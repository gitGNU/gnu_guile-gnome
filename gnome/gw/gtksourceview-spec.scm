;; guile-gnome
;; Copyright (C) 2003,2004 Andy Wingo <wingo at pobox dot com>

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
;;g-wrap specification for libgtksourceview.
;;
;;; Code:

(define-module (gnome gw gtksourceview-spec)
  :use-module (oop goops)
  :use-module (g-wrap)
  :use-module (gnome gw gobject-spec)
  :use-module (gnome gw gtk-spec)
  :use-module (gnome gobject gw-spec-utils)
  :use-module (gnome gobject defs-support))

(define-class <gnome-source-view-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-gtksourceview
  #:dependencies '(standard gnome-glib gnome-gobject
                   gnome-atk gnome-gdk gnome-pango gnome-gtk))

(define-method (initialize (ws <gnome-source-view-wrapset>) initargs)
  (next-method ws (append '(#:module (gnome gw gtksourceview)) initargs))


  (add-item! ws (make <client-item>))

  (load-defs ws "gnome/defs/gtksourceview.defs"))

(define-class <client-item> (<gw-item>))

(define *common-includes*
  '("#include <gtksourceview/gtksourceview.h>\n"
    "#include <gtksourceview/gtksourceview-typebuiltins.h>\n"
    "#include <gtksourceview/gtksourceprintjob.h>\n"
    "#include <gtksourceview/gtksourcetag.h>\n"
    "#include <gtksourceview/gtksourcelanguagesmanager.h>\n"))
         
(define-method (global-declarations-cg (self <gnome-source-view-wrapset>))
  (list (next-method)
        *common-includes*
        "#include \"source-view-support.h\"\n"))

(define-method (global-declarations-cg (self <gnome-source-view-wrapset>)
                                       (item <client-item>))
  *common-includes*)

