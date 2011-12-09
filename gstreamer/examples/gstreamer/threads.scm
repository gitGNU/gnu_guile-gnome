#! /usr/bin/guile -s
!#
;; guile-gnome
;; Copyright (C) 2003 Free Software Foundation, Inc.

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
