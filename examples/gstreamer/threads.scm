#! /usr/bin/guile -s
!#
;; A pipeline with threads that fails.
;;
;; Andy Wingo, 12 May 2003

(use-modules (gnome gstreamer debug)
             (gnome gstreamer))

(let ((pipeline (make "pipeline"))
      (thread (make "thread"))
      (fakesrc (make "fakesrc"))
      (queue (make "queue"))
      (fakesink (make "fakesink"))
      (main-loop (g-main-loop-new #f #f)))

  (add pipeline fakesrc thread)
  (add thread queue fakesink)
  (link fakesrc queue fakesink)

  (connect pipeline 'deep-notify debug-deep-notify-handler)
  (connect fakesink 'eos (lambda (p) 
                          (display "fakesink: EOS event received\n")
                          (gst-main-quit)))

  (set-state pipeline 'playing)

  (attach-iterator pipeline)

  (g-main-loop-run main-loop))
