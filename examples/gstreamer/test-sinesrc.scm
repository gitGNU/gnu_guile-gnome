(use-modules (gnome gstreamer) (examples gstreamer sinesrc))

;; (gst-debug-set-default-threshold 5)

(let ((src (make "scm-sinesrc"))
      (sink (make "osssink"))
      (pipe (make "pipeline")))
  (add pipe src sink)
  (link src sink)
  (set-state pipe 'playing)
  (while (iterate pipe) #t))
