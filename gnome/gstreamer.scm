;; guile-gnome
;; Copyright (C) 2003,2004,2007 Andy Wingo <wingo at pobox dot com>

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
;;GStreamer bindings for Guile.
;;
;;; Code:

(define-module (gnome gstreamer)
  :use-module (oop goops)
  :use-module (gnome gobject)
  :use-module (gnome glib)
  :use-module (gnome gw gstreamer)
  :use-module (gnome gw support modules))

(define-method (initialize (instance <gst-mini-object>) initargs)
  (cond
   ((get-keyword #:%real-instance initargs #f)
    => (lambda (gtype-instance)
         (slot-set! instance 'gtype-instance gtype-instance)))
   (else
    ;; nothing for next-method to do
    (let* ((class (class-of instance))
           (type (gtype-class->type class)))
      (or (null? initargs)
          (gruntime-error "Unexpected initargs: ~a" initargs))
      (gst-mini-object-primitive-create class type instance)))))

(define (construct-levels-list specifier)
  (or (and (list? specifier) (and-map list? specifier))
      (error "invalid debug specifier"))
  (let ((categories (map (lambda (c)
                           (cons
                            (string->symbol
                             (gst-debug-category-get-name c))
                            c))
                         (gst-debug-get-all-categories))))
    (map (lambda (s)
           (cond
            ((eq? (car s) '*all*)
             (list '*all* #f (cadr s) (gst-debug-get-default-threshold)))
            ((assq-ref categories (car s))
             => (lambda (c) (list (car s) c (cadr s)
                                  (gst-debug-category-get-threshold c))))
            (else
             (error "unknown debugging category" s))))
         specifier)))

(define-macro (with-gst-debug specifier . body)
  (let* ((levels (construct-levels-list specifier))
         (default-level (and=> (assq '*all* levels)
                               (lambda (x) `(quote ,x)))))
    `(dynamic-wind
         (lambda () ;; pre
           (if ,default-level
               (gst-debug-set-default-threshold (caddr ,default-level)))
           (for-each
            (lambda (l)
              (if (not (eq? (car l) '*all*))
                  (gst-debug-category-set-threshold (cadr l) (caddr l))))
            ',levels))
         
         (lambda () ,@body)
         
         (lambda () ;; post
           (if ,default-level
               (gst-debug-set-default-threshold (cadddr ,default-level)))
           (for-each
            (lambda (l)
              (if (not (eq? (car l) '*all*))
                  (gst-debug-category-set-threshold (cadr l) (cadddr l))))
            ',levels)))))
(set-object-property! with-gst-debug 'documentation
                      "Usage:
@example
(with-gst-debug ((@var{category1} #var{level1}) (@var{category2} @var{level2}) ...)
  @var{body}
@end example

Execute @var{body} with GStreamer debugging. The debugging thresholds
for categories named @var{category1}, @var{category2}, etc. will be set
to the specified values during the dynamic extent of @var{body}. As a
special case, @code{*all*} is interpreted to be the default threshold.")
(export with-gst-debug)

(define (make-value kind arg)
  (cond
   ((memv kind (list <gint> <gfloat> <gdouble> <gboolean> <gchararray>
                     <gst-fourcc> <gst-int-range> <gst-double-range>
                     <gst-fraction> <gst-fraction-range>))
    (scm->gvalue (gtype-class->type kind) arg))
   ((memv kind (list <gst-value-list> <gst-value-array>))
    (scm->gvalue (gtype-class->type kind)
                 (map (lambda (x) (make-value (car arg) x)) (cdr arg))))
   (else
    (error "Unknown structure field type: " kind))))
    
(define-public (gst-structure-new name props)
  (let ((struct (gst-structure-empty-new name)))
    (for-each
     (lambda (args)
       (apply
        (lambda (symname class value)
          (gst-structure-set-value
           struct
           (symbol->string symname)
           (make-value class value)))
        args))
     props)
    struct))

(define-public (gst-caps-new mime-type props)
  (let ((caps (gst-caps-new-empty)))
    (gst-caps-append-structure
     caps
     (gst-structure-new mime-type props))
    caps))

(define-method (add (bin <gst-bin>) . args)
  (for-each (lambda (element) (gst-bin-add bin element)) args))

(define-method (link (e1 <gst-element>) (e2 <gst-element>) . rest)
  (if (gst-element-link e1 e2)
      (if (not (null? rest))
          (apply link (cons e2 rest))
          #t)
      #f))

(define-method (remove (bin <gst-bin>) . args)
  (for-each (lambda (element) (gst-bin-remove bin element)) args))

(define-method (unlink (e1 <gst-element>) (e2 <gst-element>) . rest)
  (if (gst-element-unlink e1 e2)
      (if (not (null? rest))
          (apply unlink (cons e2 rest))
          #t)
      #f))

(define-method (unlink (pad <gst-pad>))
  (let ((peer (get-peer pad)))
    (if peer (unlink pad peer))))

(define scheme-elements (make-hash-table))

(define-public (register-new-element class name)
  (hash-create-handle! scheme-elements name class))

(define-method (make (name <string>) . args)
  ;; fixme: actually apply the args...
  (let ((factory (gst-element-factory-find name)))
    (cond
     (factory
      (gst-element-factory-create
       (gst-plugin-feature-load factory) #f))
     ((hash-ref scheme-elements name #f)
      (let ((element (make (hash-ref scheme-elements name #f))))
        (set-name element #f)
        element))
     (else
      (gruntime-error "Could not find an element factory named ~A" name)))))

;; manually wrap the class methods
;;
;; can't especify the metaclass, as that doesn't work for classes that
;; already have children
(define <gobject-class> (class-of <gobject>))
(define-method (add-pad-template (class <gobject-class>) templ)
  (gst-element-class-add-pad-template class templ))
(export add-pad-template)

(define-method (get-pad-template-list (class <gobject-class>))
  (gst-element-class-get-pad-template-list class))

(define-method (get-pad-template (class <gobject-class>) name)
  (gst-element-class-get-pad-template class name))

(define-method (set-details (class <gobject-class>) details)
  (gst-element-class-set-details class details))

;; selects a pad with name_template NAME out of list PAD-LIST
(define-method (get-pad-template (pad-list <list>) name)
  (cond
   ((null? pad-list)
    #f)
   ((equal? (get-name-template (car pad-list)) name)
    (car pad-list))
   (else
    (get-pad-template (cdr pad-list) name))))

;; re-export everything you need to have a nice gst session...

(re-export-modules (gnome glib)
                   (gnome gobject)
                   (gnome gobject generics)
                   (gnome gw gstreamer)
                   (oop goops))
