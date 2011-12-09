;; guile-gnome
;; Copyright (C) 2008 Free Software Foundation, Inc.

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
;;g-wrap specification for clutter-glx.
;;
;;; Code:

(define-module (gnome gw clutter-glx-spec)
  #:use-module ((srfi srfi-1) :select (lset-difference))
  #:use-module (oop goops)
  #:use-module (gnome gw support g-wrap)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject)
  #:use-module (gnome gw clutter-spec))

(define-class <clutter-glx-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-clutter-glx
  #:dependencies '(gnome-clutter))

(define-method (initialize (ws <clutter-glx-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw clutter-glx) initargs)))
  
  (add-type-alias! ws "Window" 'unsigned-int32)
  (add-type-alias! ws "Pixmap" 'unsigned-int32)
  
  (load-defs-with-overrides ws "gnome/defs/clutter-x11.defs")
  (load-defs-with-overrides ws "gnome/defs/clutter-glx.defs"))

(define-method (global-declarations-cg (self <clutter-glx-wrapset>))
  (list (next-method)
        "#include \"clutter-support.h\"\n"
        "#include <clutter/x11/clutter-x11.h>\n"
        "#include <clutter/glx/clutter-glx.h>\n"))
