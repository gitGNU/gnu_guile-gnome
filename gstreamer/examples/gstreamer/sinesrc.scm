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

;;; Commentary:
;;
;; A reimplementation of GStreamer's sine src in scheme. It's slow slow
;; slow -- my PIII/600 laptop can just barely do 44100 Hz in realtime.
;; This is really just a proof of concept.
;;
;;; Code:

(define-module (examples gstreamer sinesrc)
  :use-module (gnome gstreamer)
  :use-module (gnome gstreamer config)
  :use-module (gnome gobject primitives)
  :export (<scm-sinesrc>))

(read-set! keywords 'prefix) ;; :keywords rather than #:keywords

(define-class <scm-sinesrc> (<gst-element>)
  (sample-rate
   :param-spec `(,<gparam-int>
                 :nick "Sample rate" :blurb "Sample rate"
                 :minimum 4000 :maximum 48000 :default-value 22050
                 :flags (read write construct)))
  (table-size
   :param-spec `(,<gparam-int>
                 :nick "Table size" :blurb "Size of wavetable"
                 :minimum 1 :default-value 1024
                 :flags (read write construct)))
  (buffer-size
   :param-spec `(,<gparam-int>
                 :nick "Buffer size" :blurb "Buffer size (frames)"
                 :minimum 1 :default-value 1024
                 :flags (read write construct)))
  (frequency
   :param-spec `(,<gparam-float>
                 :nick "Frequency" :blurb "Frequency of sine source"
                 :minimum 0.0 :maximum 20000.0 :default-value 440.0
                 :flags (read write construct)))
  (volume
   :param-spec `(,<gparam-float>
                 :nick "Volume" :blurb "Volume of sine source"
                 :minimum 0.0 :maximum 1.0 :default-value 0.8
                 :flags (read write construct)))
  src-pad
  timestamp
  wavetable
  new-caps?
  table-pos)

(define uv-set! uniform-vector-set!)
(define uv-ref uniform-vector-ref)

(define (populate-sinetable this)
  (let* ((tsize (slot-ref this 'table-size))
         (t (make-uniform-vector tsize 1.0))
         (pi2scaled (/ (* 3.141592653589 2 ) tsize)))
    (let loop ((i 0))
      (uv-set! t i (sin (* i pi2scaled)))
      (if (eq? (1+ i) tsize)
          #t
          (loop (1+ i))))
    (slot-set! this 'wavetable t)))

(define (get-func pad)
  (let* ((this (get-parent pad))
         (data-len (slot-ref this 'buffer-size))
         (data (make-uniform-vector data-len 's)) ;; a vector of shorts
         (buffer (gst-buffer-new))
         (table-size (slot-ref this 'table-size))
         (wavetable (slot-ref this 'wavetable))
         (volume (slot-ref this 'volume))
         (table-step (* table-size (/ (slot-ref this 'frequency)
                                      (slot-ref this 'sample-rate)))))
    ;;      (set-timestamp buffer (slot-ref this 'timestamp))
    (gst-buffer-set-data buffer data)
    
    ;;      (set! (slot-ref this 'timestamp) (+ (slot-ref this 'timestamp) (* data-len gst:second)))
    
    (let ((lookup 0) (lookup-next 0) (f 0.0))
      (do ((i 0 (1+ i))
           (pos (slot-ref this 'table-pos) (+ pos table-step)))
          ((eq? i data-len) (slot-set! this 'table-pos pos))
        (set! lookup (modulo (inexact->exact (floor pos)) table-size))
        (set! lookup-next (modulo (1+ lookup) table-size))
        (set! f (- pos (floor pos)))
        
        ;; linear interpolation
        (uv-set! data i (inexact->exact
                         (* (+ (* f (uv-ref wavetable lookup-next))
                               (* (- 1 f) (uv-ref wavetable lookup)))
                            (* volume 32767))))))
    
    (if (slot-ref this 'new-caps?)
        (let ((caps (gst-caps-new
                     "audio/x-raw-int"
                     '("channels" int 1)
                     `("rate" int ,(slot-ref this 'sample-rate))
                     `("endianness" int ,gruntime:byte-order)
                     '("signed" boolean #t)
                     '("depth" int 16)
                     '("width" int 16))))
          (try-set-caps (slot-ref this 'src-pad) caps)
          (slot-set! this 'new-caps? #f)))
    
    buffer))

(define-method (gobject:set-property (this <scm-sinesrc>) (name <symbol>) value)
  (next-method)
  (case name
    ((sample-rate)
     (slot-set! this 'new-caps? #t))
    ((table-size)
     (populate-sinetable this))))

(define-method (initialize (this <scm-sinesrc>) initargs)
  (let* ((templates (get-pad-template-list <scm-sinesrc>))
         (src-template (get-pad-template templates "src"))
         (src (gst-pad-new-from-template src-template "src")))
    (slot-set! this 'src-pad src)
    (add-pad this src)
    (set-get-function src get-func)
    
    ;; no dparams (yet), sorry...
    ;; wavetable will be populated on construction automagically

    (slot-set! this 'table-pos 0.0)
    (slot-set! this 'timestamp 0)))

(add-pad-template
 <scm-sinesrc>
 (gst-pad-template-new
  "src" 'src 'always
  (gst-caps-new
   "audio/x-raw-int"
   '("channels" int 1)
   '("rate" int-range 8000 48000)
   `("endianness" int ,gruntime:byte-order)
   '("signed" boolean #t)
   '("depth" int 16)
   '("width" int 16))))

(register-new-element <scm-sinesrc> "scm-sinesrc")
