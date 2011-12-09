;; guile-gnome
;; Copyright (C) 2004,2007 Jan Nieuwenhuizen <janneke@gnu.org>
;;                    	   Andy Wingo <wingo at pobox dot com>

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
;;g-wrap specification for libwnk.
;;
;;; Code:

(define-module (gnome gw wnck-spec)
  #:use-module (oop goops)
  #:use-module (gnome gw support g-wrap)
  #:use-module (gnome gw glib-spec)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw gdk-spec)
  #:use-module (gnome gobject gw-spec-utils)
  #:use-module (gnome gobject defs-support))

(define-class <wnck-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-wnck
  #:dependencies '(standard gnome-glib gnome-gobject gnome-gdk))

(define-method (global-declarations-cg (self <wnck-wrapset>))
  (list
   (next-method)
   "#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
#include <libwnck/class-group.h>
#include <libwnck/window-action-menu.h>
"))
  
(define-method (initialize (ws <wnck-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw wnck) initargs)))

  (load-defs-with-overrides ws "gnome/defs/libwnck.defs"))
