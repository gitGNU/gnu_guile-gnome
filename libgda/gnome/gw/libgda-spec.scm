;; guile-gnome
;; Copyright (C) 2003,2004 Andreas Rottmann <a.rottman at gmx.at>

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

(define-module (gnome gw libgda-spec)
  #:use-module (oop goops)
  #:use-module (g-wrap)
  #:use-module (g-wrap guile)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject)
  #:use-module (gnome gw gobject-spec))


(define-class <gnome-gda-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-libgda #:dependencies '(standard gnome-gobject gnome-glib))

(define-method (global-declarations-cg (self <gobject-wrapset-base>))
  (list (next-method)
        "#include \"gda-support.h\"\n"))

(define-method (initializations-cg (self <gobject-wrapset-base>) err)
   (list (next-method)
         "scm_init_gnome_gda_support ();\n"
         "scm_c_eval_string (\"(use-modules (oop goops))"
         " (define-class <gda-primitive-class> (<class>))"
         " (define-method (compute-slots (class <gda-primitive-class>))"
         "   (cons '(gtype-primitive) (next-method)))"
         " (export <gda-primitive-class>)"
         "\");\n"))
  
(define-method (initialize (ws <gnome-gda-wrapset>) initargs)
  (next-method ws (append '(#:module (gnome gw libgda)) initargs))
  
  (add-type! ws (make <gda-classed-type>
                  #:ctype "GdaValue"
                  #:gtype-id "GDA_TYPE_VALUE"))
  (add-type! ws (make <gda-classed-type>
                  #:ctype "GdaCommand"
                  #:gtype-id "GDA_TYPE_COMMAND"))
  
  (for-each
   (lambda (elt)
     (add-type-alias! ws (car elt) (cadr elt)))
   '(("GdaValue*" <gda-value>)
     ("GdaCommand*" <gda-command>)))
  
  (load-defs-with-overrides ws "gnome/defs/libgda.defs"))

(define-class <gda-classed-type> (<gobject-classed-pointer-type>))

(define-method (initialize (type <gda-classed-type>))
  (next-method type (append '(#:define-class? #f #:wrapped  "Custom")
                            initargs)))

(define-method (initializations-cg (wrapset <gnome-gda-wrapset>)
                                   (type <gda-classed-type>)
                                   status-var)
  (let ((class (symbol->string (class-name type))))
    (list
     (next-method)
     (inline-scheme
      wrapset
      `(define-class ,(class-name type) ()
         #:gtype ,(string->symbol
                   (string-append
                    "gtype:" (substring class 1 (- (string-length class) 1))))
         #:metaclass <gda-primitive-class>)
      `(export ,(class-name type))))))

(define-method (unwrap-value-cg (type <gda-classed-type>)
                                (value <gw-value>)
                                status-var)
  (let ((scm-var (scm-var value))
        (c-var (var value))
        (class (class-name type)))
    (list
     "if (SCM_NFALSEP (scm_call_2 (SCM_VARIABLE_REF (scm_c_lookup (\"is-a?\")),\n"
     "                                  " scm-var ",\n"
     "                                  SCM_VARIABLE_REF (scm_c_lookup (\"" class "\")))))\n"
     "  " c-var " = (" (c-type-name type) ") g_value_get_boxed ((GValue*)SCM_SMOB_DATA (scm_slot_ref (" scm-var ", scm_str2symbol (\"gtype-primitive\"))));\n"
     "else " `(gw:error ,status-var type ,scm-var))))

(define-method (wrap-value-cg (type <gda-classed-type>)
                              (value <gw-value>)
                              status-var)
  (let ((scm-var (scm-var value))
        (c-var (var value)))
    (list
     scm-var " = scm_c_make_gtype_primitive_instance (\""
     (class-name type) "\", " (gtype-id type) ", (void *)" c-var ");\n")))

