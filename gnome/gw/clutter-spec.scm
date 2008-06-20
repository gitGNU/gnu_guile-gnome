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
;;g-wrap specification for clutter.
;;
;;; Code:

(define-module (gnome gw clutter-spec)
  #:use-module ((srfi srfi-1) :select (lset-difference))
  #:use-module (oop goops)
  #:use-module (gnome gw support g-wrap)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject))

(define-class <clutter-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-clutter
  #:dependencies '(standard gnome-glib gnome-gobject))

(define-class <gw-guile-clutter-unit-type> (<gw-guile-simple-type>))

(define-method (check-typespec-options (type <gw-guile-clutter-unit-type>) (options <list>))
  (let ((remainder options))
    (define (del*! . syms)
      (set! remainder (lset-difference eq? remainder syms)))
    (del*! 'const 'out 'unspecialized)
    (apply del*! 'caller-owned 'callee-owned (slot-ref type 'allowed-options))
    (if (not (null? remainder))
        (raise-bad-typespec type options
                            "spurious options in RTI type: ~S" remainder))))

(define-method (initialize (ws <clutter-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw clutter) initargs)))
  
  (wrap-custom-boxed!
   "ClutterKnot" "CLUTTER_TYPE_KNOT"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_knot_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_knot (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterColor" "CLUTTER_TYPE_COLOR"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_color_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_color (" scm-var ");\n"))

  (add-type! ws (make <gw-guile-clutter-unit-type>
                  #:name '<clutter-unit>
                  #:c-type-name "ClutterUnit"
                  #:type-check #f
                  #:unwrap '(c-var " = scm_scm_to_clutter_units (" scm-var ");\n")
                  #:wrap '(scm-var " = scm_clutter_units_to_scm (" c-var ");\n")
                  #:allowed-options '(out)
                  #:ffspec 'int32))
  (add-type-alias! ws "ClutterUnit" '<clutter-unit>)
  (add-type-rule! ws "ClutterUnit*" '(<clutter-unit> out))
  
  (load-defs-with-overrides ws "gnome/defs/clutter.defs"))

(define-method (global-declarations-cg (self <clutter-wrapset>))
  (list (next-method)
        "#include <clutter/clutter.h>\n"
        "#include \"clutter-support.h\"\n"))
