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
;;g-wrap specification for dbus.
;;
;;; Code:

(define-module (gnome gw dbus-spec)
  #:use-module (oop goops)
  #:use-module (g-wrap)
  #:use-module (g-wrap guile)
  #:use-module (gnome gw glib-spec)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject))

(define-class <dbus-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-dbus
  #:dependencies '(standard gnome-glib gnome-gobject))

(define-method (initialize (ws <dbus-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw dbus) initargs)))
  
  (for-each
   (lambda (pair) (add-type-alias! ws (car pair) (cadr pair)))
   '(("dbus_uint16_t" unsigned-int16)
     ("dbus_uint32_t" unsigned-int32)
     ("dbus_uint64_t" unsigned-int64)
     ("dbus_int16_t" int16)
     ("dbus_int32_t" int32)
     ("dbus_unichar_t" int32)
     ("dbus_int64_t" int64)
     ("dbus_bool_t" bool)
     ("ssize_t" int) ; fixme: system-dependant
     ("size_t" unsigned-int))) ; fixme: system-dependant

  ;; g-wrap rocks!
  (add-type! ws (make <dbus-error-type> #:name '<dbus-error>))
  (add-type-alias! ws "DBusError*" '<dbus-error>)

  ;; copying from glib.scm
  (add-type-rule! ws '(("dbus_int32_t*" "*")) '(int32 out))
  (add-type-rule! ws '(("dbus_uint32_t*" "*")) '(unsigned-int32 out))
  ;; (add-type-rule! ws '(("DBusPendingCall**" "*")) '(<d-bus-pending-call*> out))

  (load-defs-with-overrides ws "gnome/defs/dbus.defs"))

(define-method (global-declarations-cg (self <dbus-wrapset>))
  (list (next-method)
        "#include <dbus/dbus.h>\n"
        "#include <dbus/dbus-glib.h>\n"
        "#include \"dbus-support.h\"\n"))

;;; DBusError

(define-class <dbus-error-type> (<gw-type>))

(define-method (initialize (self <dbus-error-type>) initargs)
  (next-method self (append '(#:arguments-visible? #f) initargs)))
    
(define-method (c-type-name (type <dbus-error-type>))
  "DBusError")

(define-method (check-typespec-options (type <dbus-error-type>) (options <list>))
  (let ((remainder options))
    (set! remainder (delq 'caller-owned remainder))
    (set! remainder (delq 'null-ok remainder))
    (if (not (null? remainder))
        (raise (condition
                (&gw:bad-typespec (type type) (options options)))))))
    
(define-method (destroy-value-cg (t <dbus-error-type>)
                                  (value <gw-value>)
                                  status-var)
  (list "if (dbus_error_is_set (& " (var value) "))\n" ;; paranoia
        "  dbus_error_free (&" (var value) ");\n"))

(define-method (pre-call-arg-cg (t <dbus-error-type>)
                                (value <gw-value>)
                                status-var)
  (list "dbus_error_init (&" (var value) ");\n"))


(define-method (call-arg-cg (t <dbus-error-type>) (value <gw-value>))
  (list "&" (var value)))

(define-method (post-call-arg-cg (t <dbus-error-type>)
                                 (value <gw-value>)
                                 status-var)
  (let* ((c-name (var value))
         (typespec (typespec value)))
    (list
     "if (dbus_error_is_set (&" c-name ")) {\n" 
     "  SCM dberror = scm_list_2(scm_makfrom0str(" c-name ".name), scm_makfrom0str(" c-name ".message));\n"
     (destroy-value-cg t value status-var)
     "  scm_throw(scm_str2symbol(\"dbus-error\"), dberror);\n"
     "}\n")))
