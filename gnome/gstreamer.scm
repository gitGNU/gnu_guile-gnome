(define-module (gnome gstreamer))

;; 'link' is so important to gstreamer that I think it can shadow
;; (guile)'s definition. That's my opinion, though.

(module-define! the-root-module 'oldlink link)
(module-remove! the-root-module 'link)

(display "(gnome gstreamer): [")

(display "goops ")
(use-modules (oop goops))
(display "gobject ")
(use-modules (gnome gobject))
(display "glib ")
(use-modules (gnome glib))
(display "gstreamer ")
(use-modules (gnome gw gstreamer))
(display "support")
(use-modules (gnome gobject gw-utils)
             (gnome gstreamer config))

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

(define gtype:gst-list (gtype-from-name "GstValueList"))
(define gtype:gst-fourcc (gtype-from-name "GstFourcc"))
(define gtype:gst-int-range (gtype-from-name "GstIntRange"))
(define gtype:gst-double-range (gtype-from-name "GstDoubleRange"))
;;(gtype->class gtype:gst-list)
;;(gtype->class gtype:gst-fourcc)
;;(gtype->class gtype:gst-int-range)
;;(gtype->class gtype:gst-double-range)

(define simple-types-alist `((int . ,gtype:gint)
                             (float . ,gtype:gfloat)
                             (double . ,gtype:gdouble)
                             (boolean . ,gtype:gboolean)
                             (string . ,gtype:gchararray)))
(define custom-types-alist `((fourcc . ,gtype:gst-fourcc)
                             (int-range . ,gtype:gst-int-range)
                             (double-range . ,gtype:gst-double-range)))
(define (make-value cdr-prop)
  (let ((kind (car cdr-prop))
        (args (cdr cdr-prop)))
    (cond
     ((assq kind simple-types-alist)
      => (lambda (pair) (apply scm->gvalue (cdr pair) args)))
     ((assq kind custom-types-alist)
      => (lambda (pair)
           (let* ((type (cdr pair))
                  (ret (gtype-primitive-create-basic-instance type)))
             (case kind
               ((fourcc)
                (apply gst-value-set-fourcc ret args))
               ((int-range)
                (apply gst-value-set-int-range ret args))
               ((double-range)
                (apply gst-value-set-double-range ret args)))
             ret)))
     ((list? kind)
      ;; The whole thing is a list
      (let ((ret (gtype-primitive-create-basic-instance gtype:gst-list)))
        (for-each
         (lambda (arg)
           (gst-value-list-append-value ret (make-value arg)))
         cdr-prop)
        ret))
     (else
      (error "Unknown structure field type: " cdr-prop)))))
    
(define-public (gst-structure-new name . props)
  (let ((struct (gst-structure-empty-new name)))
    (for-each
     (lambda (prop)
       (gst-structure-set-value struct (car prop) (make-value (cdr prop))))
     props)
    struct))

(define-public (gst-caps-new mime-type . props)
  (let ((caps (gst-caps-new-empty)))
    (gst-caps-append-structure
     caps
     (apply gst-structure-new mime-type props))
    caps))

(define-public (gst-structure->spec structure)
  (define (find-kind type)
    (let loop ((pairs (append simple-types-alist
                              custom-types-alist)))
      (cond
       ((null? pairs) 'unknown)
       ((eq? (cdar pairs) type) (caar pairs))
       (else (loop (cdr pairs))))))
  (define (gst-value-list-map proc val)
    (let loop ((i 0) (ret '()))
      (if (< i (gst-value-list-get-size val))
          (loop (1+ i) (cons (process-value (gst-value-list-get-value val i)) ret))
          (reverse ret))))
  (define (process-value val)
    (let* ((type (gvalue->type val))
           (kind (find-kind type)))
       (if (eq? type gtype:gst-list)
           (gst-value-list-map process-value val)
           (cons
            kind
            (case kind
              ((int-range) (list (gst-value-get-int-range-min val)
                                 (gst-value-get-int-range-max val)))
              ((double-range) (list (gst-value-get-double-range-min val)
                                    (gst-value-get-double-range-max val)))
              ((fourcc) (list (gst-value-get-fourcc)))
              (else (list (gvalue->scm val))))))))
  ;; Hack!
  (let ((ret '()))
    (gst-structure-foreach
     structure
     (lambda (name val)
       (let* ((type (gvalue->type val))
              (kind (find-kind type)))
         (set! ret (cons (cons name (process-value val))
                         ret)))))
    (reverse ret)))

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

(define scheme-elements (make-hash-table 31))

(define-public (register-new-element class name)
  (hash-create-handle! scheme-elements name class))

(define-method (make (name <string>) . args)
  ;; fixme: actually apply the args...
  (let ((factory (gst-element-factory-find name)))
    (cond
     (factory
      (gst-plugin-feature-ensure-loaded factory)
      (gst-element-factory-create factory #f))
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

(display "]\n")
