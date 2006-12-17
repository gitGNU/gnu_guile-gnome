#! /bin/sh
# -*- scheme -*-
exec guile-gnome-0 -e main -s $0 "$@"
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


(use-modules (gnome gstreamer))

(define (main args)
  (if (not (eq? (length args) 2))
      (begin (format #t "usage: ~A VORBISFILE" (car (program-arguments)))
             (exit 1)))

  (let ((pipeline (make "pipeline"))
        (filesrc (make "filesrc"))
        (oggdemux (make "oggdemux"))
        (vorbisdec (make "vorbisdec"))
        (audioconvert (make "audioconvert"))
        (alsasink (make "alsasink")))

    (define (pad-added element pad)
      (link (pk pad) (get-pad vorbisdec "sink")))

    (add pipeline filesrc oggdemux vorbisdec audioconvert alsasink)
    (link filesrc oggdemux)
    (link vorbisdec audioconvert alsasink)

    (set filesrc 'location (cadr args))
    (connect oggdemux 'pad-added pad-added)

    (set-state pipeline 'playing)
    (poll (get-bus pipeline) '(eos error) gparameter:uint64-max)
    (set-state pipeline 'null)))

