(define-module (gnome gstreamer debug)
  :use-module (gnome gstreamer)
  ;:use-module (gnome gstreamer config)
  :use-module (gnome gobject primitives)
  :export     (debug-caps
               debug-notify-handler
               debug-deep-notify-handler))

(define (display-prop prop level)
  (define (display-value name cdr-prop indent)
    (format #t "~A~A (~A) ~A\n"
            indent
            (if name (string-append name ":") "")
            (if (list? (car cdr-prop)) "list" (car cdr-prop))
            (cond
             ((list? (car cdr-prop)) "(")
             ((null? (cddr cdr-prop)) (cadr cdr-prop))
             (else (cdr cdr-prop))))
    (if (list? (car cdr-prop))
        (begin
          (for-each
           (lambda (cdr-prop)
             (display-value #f cdr-prop (string-append " " indent)))
           cdr-prop)
          (format #t "~A)\n" indent))))
  (display-value (car prop) (cdr prop)
                 (apply string-append (make-list level "    "))))

(define (debug-caps caps)
  (let loop ((i 0))
    (if (< i (gst-caps-get-size caps))
        (let ((struct (gst-caps-get-structure caps i)))
          (format #t "mime-type: ~A\n" (gst-structure-get-name struct))
          (for-each
           (lambda (prop)
             (display-prop prop 1))
           (gst-structure->spec struct))
          (loop (1+ i))))))

(define (debug-notify-handler sender param)
  (let* ((pspec-struct (gparam->pspec-struct param))
         (param-name (gparam-spec:name pspec-struct))
         (value-type (gparam-spec:value-type pspec-struct))
         (value (get sender param-name)))
    (format #t "~A::~A = " (get-name sender) param-name)
    (begin (display value) (newline))))

(define (debug-deep-notify-handler element original pspec)
  (display
   (string-join
    (append
     '("")
     (let loop ((parent element) (child original) (ret '()))
       (if (not (equal? (get-name parent) (get-name child)))
           (loop parent (get-parent child) (cons (get-name (get-parent child)) ret))
           ret))
     '(""))
    "/"))
  (debug-notify-handler original pspec))
