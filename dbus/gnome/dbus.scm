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
;;DBus bindings for Guile.
;;
;;DBus is a @code{freedesktop.org} project to create system-wide and
;;inter-application message buses. It is typically used as a simple IPC
;;system.
;;
;;Among the types that DBus defines is the @dfn{message}, the unit of
;;communication between subscribers to the bus. A message has a number
;;of pre-set fields and, at the end, a list of typed arguments. When you
;;work with this argument list, you need to first get an @dfn{iter}.
;;These can be retrieved with @code{dbus-message-get-iter} or
;;@code{dbus-message-get-append-iter}.
;;
;;Guile-DBus provides some higher-level functions for appending and
;;retrieving arguments:
;;
;;@defun dbus-message-iter-get iter
;;Reads an argument pointed to by the current iter. The various types
;;will be retrieved as follows:
;;@table @code
;;@item NIL
;;The symbol, @code{nil}.
;;@item BOOLEAN
;;As a boolean, @code{#t} or @code{#f}.
;;@item BYTE
;;@itemx INT32
;;@itemx UINT32
;;@itemx INT64
;;@itemx UINT64
;;As an exact number.
;;@item DOUBLE
;;As an inexact number.
;;@item STRING
;;As a string.
;;@item CUSTOM
;;As a pair. The @sc{car} will be a symbol containing the name of the
;;argument, and the @sc{cdr} will contain a uniform byte vector of the
;;contents.
;;@item ARRAY
;;The type of the return value depends on the subtype of the array.
;;@table @asis
;;@item @code{BYTE}
;;@itemx @code{INT32}
;;@itemx @code{UINT32}
;;@itemx @code{INT64}
;;@itemx @code{UINT64}
;;@itemx @code{DOUBLE}
;;As a uniform vector of the proper type.
;;@item anything else
;;As a list. The elements will be made with
;;@code{dbus-message-iter-get}, which makes this a recursive type
;;definition.
;;@end table
;;@item DICT
;;As a pair. The @sc{car} will be a symbol containing the key for the
;;dict entry, and the @sc{cdr} will be as returned by
;;@code{dbus-message-iter-get}.
;;@item OBJECT-PATH
;;As a list. For example, "org.freedesktop" => '(org freedesktop).
;;@end table
;;@end defun
;;
;;@defun dbus-message-iter-append iter arg [type=#f]
;;Appends an arg to the message. If @var{type} is not given, or is
;;@code{#f}, the type of the DBus argument will be deduced from
;;@var{arg}. If @var{type} is given, it should be in the following
;;format:
;;@table @asis
;;@item a symbol
;;Must be one of @code{nil}, @code{boolean}, @code{byte},
;;@code{int32}, @code{uint32}, @code{int64}, @code{uint64},
;;@code{double}, @code{string}, @code{custom}, or @code{object-path}.
;;@var{arg} should be given as dbus-message-iter-get would return it.
;;@item a list of one element
;;Indicates an array. If the single element of @var{type} is one of
;;@code{byte}, @code{int32}, @code{uint32}, @code{int64}, @code{uint64}, 
;;or @code{double}, @var{arg} should be a uniform vector of the
;;appropriate type. Otherwise, @var{arg} should be a list, the members
;;of which conform to the type indicated by the single element of
;;@var{type}. Note that this is recursive: @code{'((int32))} is an array
;;of arrays of int32, etc.
;;@item a list headed by the symbol, @code{dict}
;;Indicates a dict. @var{arg} should be an alist containing the dict
;;entries. The @sc{cdr} of @var{type}, if present, is an alist
;;containing type specifications for the dict entries, e.g.
;;@code{'(dict (arg1 . int32) (arg2 . string))} specifies a dict for
;;which @code{arg1} is an int32 and @code{arg2} is a string.
;;@end table
;;@end defun
;;; Code:

(define-module (gnome dbus)
  #:use-module (oop goops)
  #:use-module (gnome gw dbus)
  #:use-module (gnome gw support modules)
  #:export (<dbus-service>
            dbus-connection-get-service
            <dbus-remote-object>
            get-object
            invoke))

(re-export-modules (gnome gw dbus))

;; a convenience class for a service, following the model of the python
;; dbus bindings
(define-class <dbus-service> ()
  (connection #:init-value #f #:init-keyword #:connection)
  (service-name #:init-value #f #:init-keyword #:service-name))

;; should be a method, but have to fix classes of boxed types first
(define (dbus-connection-get-service connection service-name)
  (make <dbus-service>
    #:connection connection
    #:service-name service-name))

(define-class <dbus-remote-object> ()
  (connection #:init-value #f #:init-keyword #:connection)
  (service-name #:init-value #f #:init-keyword #:service-name)
  (object-path #:init-value #f #:init-keyword #:object-path)
  (interface #:init-value #f #:init-keyword #:interface))
  
(define-method (get-object (service <dbus-service>) path interface)
  (make <dbus-remote-object>
    #:connection (slot-ref service 'connection)
    #:service-name (slot-ref service 'service-name)
    #:object-path path
    #:interface interface))

(define-method (invoke (object <dbus-remote-object>) method-name . args)
  (define (prop x) (slot-ref object x))
  (let* ((message (dbus-message-new-method-call
                   (prop 'service-name) (prop 'object-path)
                   (prop 'interface) (symbol->string method-name)))
         (iter (dbus-message-get-append-iter message)))
    (for-each
     (lambda (arg) (dbus-message-iter-append iter arg))
     args)

    (catch 'dbus-error
           (lambda ()
             (let* ((reply (dbus-connection-send-with-reply-and-block
                            (prop 'connection) message 5000))
                    (iter (dbus-message-get-iter reply)))
               (let lp ((vals '()))
                 (let ((val (dbus-message-iter-get iter)))
                   (cond ((dbus-message-iter-next iter)
                          (lp (cons val vals)))
                         ((null? vals) val)
                         (else
                          (apply values (reverse! (cons val vals)))))))))

           (lambda (key name message)
             (if (not (string=? name "org.freedesktop.DBus.NoReply"))
                 (throw key name message))))))
