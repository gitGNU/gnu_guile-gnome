#! /usr/bin/guile -s
!#
;; guile-gnome
;; Copyright (C) 2003,2004 Free Software Foundation, Inc.

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
