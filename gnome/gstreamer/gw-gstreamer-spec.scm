(define-module (gnome gstreamer gw-gstreamer-spec)
  :use-module (g-wrap)
  :use-module (gnome gobject gw-gobject-spec)
  :use-module (gnome gstreamer config)
  :use-module (gnome gobject defs-support)
  :use-module (gnome gobject gw-spec-utils))

(let ((ws (gw:new-wrapset "guile-gnome-gw-gstreamer")))

  (gw:wrapset-set-guile-module! ws '(gnome gstreamer gw-gstreamer))
  (gw:wrapset-depends-on ws "guile-gnome-gw-standard")
  (gw:wrapset-depends-on ws "guile-gnome-gw-glib")
  (gw:wrapset-depends-on ws "guile-gnome-gw-gobject")

  (gw:wrapset-add-cs-declarations!
   ws
   (lambda (wrapset client-wrapset)
     (if (not client-wrapset)
         '("#include <gst/gst.h>\n"
           "#include \"gstreamer-support.h\"\n")
         '("#include <gst/gst.h>\n"))))

  (gw:wrapset-add-cs-initializers!
   ws
   (lambda (wrapset client-wrapset status-var)
     (if (not client-wrapset)
         `("g_log_set_handler (\"GStreamer\", G_LOG_LEVEL_MASK | G_LOG_FLAG_FATAL\n"
           "                   | G_LOG_FLAG_RECURSION, guile_gobject_log_handler,\n"
           "                   NULL);\n"
           "/* because of gstreamer's cothreads, we have to disable stack checking */\n"
           ,(gw:inline-scheme '(debug-set! stack 0))
           "gst_init (NULL, NULL);\n")
         '())))

  (register-type "guile-gnome-gw-gstreamer" "GstClockTime" '<gw:unsigned-long-long>)
  (register-type "guile-gnome-gw-gstreamer" "GstClockTimeDiff" '<gw:unsigned-long-long>)
  (let ((type (gobject:gwrap-class ws "GstElementClass" "GST_TYPE_ELEMENT")))
    (register-type "guile-gnome-gw-gstreamer" "GstElementClass*" (gw:type-get-name type)))

  (load-defs ws "gnome/defs/gstreamer.defs"))

