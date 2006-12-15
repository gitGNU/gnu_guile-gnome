;; guile-gnome
;; Copyright (C) 2003,2004 Andy Wingo <wingo at pobox dot com>

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
;;g-wrap specification for GStreamer.
;;
;;; Code:

(define-module (gnome gw gstreamer-spec)
  #:use-module (oop goops)
  #:use-module (gnome gw support g-wrap)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw support gobject)
  #:use-module (gnome gw support defs))

(define-class <gstreamer-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-gstreamer
  #:dependencies '(standard gnome-glib gnome-gobject))

(define-method (global-declarations-cg (self <gstreamer-wrapset>))
  (list (next-method)
        "#include <gst/gst.h>\n"
        "#include \"gstreamer-support.h\"\n"))
  
(define-method (initializations-cg (self <gstreamer-wrapset>) err)
  (list (next-method)
        "scm_init_gstreamer ();\n"))

(define-method (initialize (ws <gstreamer-wrapset>) initargs)

  (next-method ws (append '(#:module (gnome gw gstreamer)) initargs))

  (add-type-alias! ws "GstClockTime" 'unsigned-long-long)
  (add-type-alias! ws "GstClockTimeDiff" 'unsigned-long-long)
  
  (wrap-gobject-class! ws #:ctype "GstElementClass" #:gtype-id "GST_TYPE_ELEMENT")

  (wrap-custom-gvalue! "GstFourcc" "GST_TYPE_FOURCC"
                       "scm_from_gst_fourcc" "scm_to_gst_fraction")
  (wrap-custom-gvalue! "GstFraction" "GST_TYPE_FRACTION"
                       "scm_from_gst_fraction" "scm_to_gst_fraction")

  (wrap-custom-gvalue! "GstIntRange" "GST_TYPE_INT_RANGE"
                       "scm_from_gst_int_range" "scm_to_gst_int_range")
  (wrap-custom-gvalue! "GstDoubleRange" "GST_TYPE_DOUBLE_RANGE"
                       "scm_from_gst_double_range" "scm_to_gst_double_range")
  (wrap-custom-gvalue! "GstFractionRange" "GST_TYPE_FRACTION_RANGE"
                       "scm_from_gst_fraction_range" "scm_to_gst_fraction_range")

  (wrap-custom-gvalue! "GstValueList" "GST_TYPE_LIST"
                       "scm_from_gst_list" "scm_to_gst_list")
  (wrap-custom-gvalue! "GstValueArray" "GST_TYPE_ARRAY"
                       "scm_from_gst_array" "scm_to_gst_array")

  (load-defs-with-overrides ws "gnome/defs/gstreamer.defs"))
