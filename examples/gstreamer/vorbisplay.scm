#! /usr/bin/guile -s
!#
;; This file is intended to be analogous to vorbisplay.py in the
;; gst-python distribution.
;;
;; Andy Wingo, 12 May 2003

(use-modules (gnome gstreamer debug) ; debug-notify-handler
             (gnome gstreamer)) ; for some ungodly reason this must be
			        ; after debug -- don't know why yet

(if (not (eq? (length (program-arguments)) 2))
    (begin (format #t "usage: ~A VORBISFILE" (car (program-arguments)))
           (exit 1)))

;;(gst-debug-set-threshold 4)

(let ((pipeline (make "pipeline"))
      (filesrc (make "filesrc"))
      (vorbisfile (make "vorbisfile"))
      (osssink (make "osssink"))
      (main-loop (g-main-loop-new #f #f)))

  (add pipeline filesrc vorbisfile osssink)
  (link filesrc vorbisfile osssink)

  (set filesrc 'location (cadr (program-arguments)))
  (connect vorbisfile 'notify debug-notify-handler)
  (connect osssink 'eos (lambda (p) 
                          (display "osssink: EOS event received\n")
                          (g-main-loop-quit main-loop)))

  ;; just a demo of how to do things at certain times in the stream
  (gst-clock-id-wait-async
   (new-single-shot-id (get-clock osssink) 5000000000)
   (lambda (time) (format #t "OSS clock has passed five seconds -- current time ~A\n"
                          time) #t))

  (set-state pipeline 'playing)

  (attach-iterator pipeline)

  (g-main-loop-run main-loop))
