(use-modules (gnome gstreamer))

(let ((p (gst-element-factory-make "pipeline" #f))
      (src (gst-element-factory-make "fakesrc" #f))
      (sink (gst-element-factory-make "fakesink" #f)))

  (set-chain-function (get-pad sink "sink")
                      (lambda (pad buf) (display "Hello!\n")))

  (add p src sink)
  (link src sink)

  (set-state p 'playing)

  (while (gst-bin-iterate p) #t))
