#! /usr/bin/guile -s
!#
;; Demonstrates how to construct caps with scheme.
;;
;; Andy Wingo, 17 June 2003
;; Updated Jan 2004 for new caps system

(use-modules (gnome gstreamer debug) ; debug-caps
             (gnome gstreamer)) ; for some ungodly reason this must be
			        ; after debug -- don't know why yet

(display "\nCaps of audioconvert, instantiated from scheme:\n\n")

(debug-caps
 (gst-caps-new
  "audio/x-raw-int"
  '("channels" int-range 1 2)
  '("rate" int-range 8000 192000)
  '("endianness" (int 1234) (int 4321))
  '("signed" (boolean #f) (boolean #t))
  '("depth" int-range 1 32)
  '("width" (int 8) (int 16) (int 24) (int 32))))

(newline)

;; look ma, no segfault
