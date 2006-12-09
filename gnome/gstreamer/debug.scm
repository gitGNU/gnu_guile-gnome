(define-module (gnome gstreamer debug)
  :use-module (gnome gstreamer)
  :use-module (gnome gobject)
  :export     (debug-caps
               debug-notify-handler
               debug-deep-notify-handler))

(define (debug-caps caps)
  (let loop ((i 0))
    (if (< i (gst-caps-get-size caps))
        (let ((struct (gst-caps-get-structure caps i)))
          (format #t "mime-type: ~A\n" (gst-structure-get-name struct))
          (gst-structure-foreach
           struct
           (lambda (k v)
             (format #t "  ~a: ~a ~a\n" k (gvalue->type v) (gvalue->scm v))))
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
