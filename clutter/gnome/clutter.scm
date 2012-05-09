;; guile-gnome
;; Copyright (C) 2008, 2012 Free Software Foundation, Inc.

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
;;Clutter bindings.
;;
;;Example code goes here.
;;
;;; Code:

(define-module (gnome clutter)
  #:use-module (oop goops)
  #:use-module (gnome gobject)
  #:use-module (gnome gw clutter)
  #:use-module (gnome gw support modules)
  #:export (clutter-interval-new
            clutter-interval-set-initial
            clutter-interval-set-final
            clutter-interval-set-interval
            clutter-interval-get-initial
            clutter-interval-get-final
            clutter-interval-get-interval))

(re-export-modules (gnome gw clutter))

(define (clutter-interval-new type from to)
  (clutter-interval-new-with-values type
                                    (make type #:value from)
                                    (make type #:value to)))

(define (clutter-interval-set-initial interval val)
  (let ((type (clutter-interval-get-value-type interval)))
    (clutter-interval-set-initial-value interval (make type #:value val))))

(define (clutter-interval-set-final interval val)
  (let ((type (clutter-interval-get-value-type interval)))
    (clutter-interval-set-final-value interval (make type #:value val))))

(define (clutter-interval-set-interval interval initial final)
  (clutter-interval-set-initial interval initial)
  (clutter-interval-set-final interval final))

(define (clutter-interval-get-initial interval)
  (gvalue->scm (clutter-interval-get-initial-value interval)))

(define (clutter-interval-get-final interval)
  (gvalue->scm (clutter-interval-get-final-value interval)))

(define (clutter-interval-get-interval interval)
  (values (clutter-interval-get-initial interval)
          (clutter-interval-get-final interval)))
