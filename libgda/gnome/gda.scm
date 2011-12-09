;; guile-gnome
;; Copyright (C) 2003,2004 Andreas Rottmann <a.rottmann at gmx.at>

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
;;GDA bindings.
;;
;;; Code:

(define-module (gnome gda)
  #:use-module (oop goops)
  #:use-module (gnome gobject)
  #:use-module (gnome gw support modules)
  #:use-module (gnome gw libgda)
  
  #:export (value))

(define-method (make-instance (class <gda-primitive-class>) . initargs)
  (let ((instance (allocate-instance class initargs))
        (primitive (get-keyword #:%primitive-instance initargs #f)))
    (if primitive
        (slot-set! instance 'gtype-primitive primitive)
        (initialize instance initargs))
    instance))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; GdaValue
;;;
(define-method (initialize (value <gda-value>) initargs)
  (let ((init-value (get-keyword #:value initargs *unspecified*)))
    (if (unspecified? init-value)
        (gruntime-error "Missing #:value argument"))
    (slot-set! value 'gtype-primitive (gda-value-primitive-new init-value))))

(define-accessor value)

(define-method (value (v <gda-value>))
  (gda-value-primitive-get (slot-ref v 'gtype-primitive)))

(define-method ((setter value) (v <gda-value>) newval)
  (gda-value-primitive-set! (slot-ref v 'gtype-primitive) newval))

(define-method (write (v <gda-value>) port)
  (display "#<gda-value " port)
  (write (value v) port)
  (display #\> port))


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;; GdaCommand
;;;
(define-method (write (command <gda-command>) port)
  (display "#<gda-command " port)
  (display (genum->symbol (get-command-type command)) port)
  (display #\space port)
  (write (get-text command) port)
  (display #\> port))

(define-method (initialize (command <gda-command>) initargs)
  (let ((type (get-keyword #:type initargs *unspecified*))
        (text (get-keyword #:text initargs *unspecified*))
        (options (get-keyword #:options initargs '())))
    (if (unspecified? type)
        (gruntime-error "Missing #:type argument"))
    (if (unspecified? text)
        (gruntime-error "Missing #:text argument"))
    ;; WART: This creates a uneeded instance
    (slot-set! command 'gtype-primitive
               (slot-ref (gda-command-new text type options)
                         'gtype-primitive))))

(re-export-modules (oop goops)
                   (gnome gw libgda))
