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
