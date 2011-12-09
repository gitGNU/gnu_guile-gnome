;; guile-gnome
;; Copyright (C) 2001 Martin Baulig <martin@gnome.org>
;;               2003,2004 Andy Wingo <wingo at pobox dot com>

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
;;A CORBA wrapper for Guile.
;;
;;; Code:

(use-modules (gnome corba) (oop goops))
(corba-open-module "/gnome/head/INSTALL/lib/orbit/Foo_module")
;(define s (make-corba-struct TC:Foo:MyType))
(define a (make-corba-record-type TC:Foo:MyType))
(define b ((corba-record-constructor a) 7 49))
(define c (make-corba-record-type TC:ORBit:IInterface))
(define d ((corba-record-constructor-from-struct c) IF:Foo:Hello))
(define e (make-corba-struct TC:ORBit:IInterface 0 IF:Foo:Hello))
