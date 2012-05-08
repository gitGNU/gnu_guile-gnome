;; guile-gnome
;; Copyright (C) 2008,2012 Free Software Foundation, Inc.

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
;;g-wrap specification for clutter.
;;
;;; Code:

(define-module (gnome gw clutter-spec)
  #:use-module ((srfi srfi-1) :select (lset-difference))
  #:use-module (oop goops)
  #:use-module (gnome gw support g-wrap)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject)
  #:use-module (gnome gw cairo-spec)
  #:use-module (gnome gw atk-spec)
  #:use-module (gnome gw pango-spec))

(define-class <clutter-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-clutter
  #:dependencies '(standard gnome-glib gnome-gobject gnome-cairo gnome-atk gnome-pango))

(define-method (initializations-cg (self <clutter-wrapset>) err)
  (list
   (next-method)
   "clutter_init (NULL, NULL);\n"
   ;;; fixme: add sinkfunc for GInitiallyUnowned
   ))

(define-class <clutter-event-type> (<gobject-classed-pointer-type>))
(define-method (initialize (type <clutter-event-type>) initargs)
  (next-method type (cons #:gtype-id (cons "CLUTTER_TYPE_EVENT" initargs))))
(define-method (unwrap-value-cg (type <clutter-event-type>)
                                (value <gw-value>)
                                status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (unwrap-null-checked
     value status-var
     (list
      "if (scm_c_gvalue_holds (" scm-var ", CLUTTER_TYPE_EVENT))\n"
      "  " c-var " = scm_c_gvalue_peek_boxed (" scm-var ");\n"
      "else {\n"
      "  " c-var " = NULL;\n"
      `(gw:error ,status-var type ,(wrapped-var value))
      "}\n"))))
(define-method (wrap-value-cg (type <clutter-event-type>)
                              (value <gw-value>)
                              status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list
     "if (" c-var " == NULL) {\n"
     "  " scm-var " = SCM_BOOL_F;\n"
     "} else {\n"
     "  " scm-var " = scm_c_gvalue_new_from_boxed (CLUTTER_TYPE_EVENT, " c-var ");\n"
     "}\n")))

(define-method (initialize (ws <clutter-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw clutter) initargs)))
  
  (for-each
   (lambda (ctype)
     (let ((event (make <clutter-event-type>
                    #:ctype ctype
                    #:c-type-name (string-append ctype "*"))))
       (add-type! ws event)
       (add-type-alias! ws (string-append ctype "*") (name event))))
   '("ClutterEvent"
     "ClutterAnyEvent"
     "ClutterButtonEvent"
     "ClutterKeyEvent"
     "ClutterMotionEvent"
     "ClutterScrollEvent"
     "ClutterStageStateEvent"
     "ClutterCrossingEvent"))

  (add-type-rule! ws "ClutterBinAlignment*" '(<clutter-bin-alignment> out))
  (add-type-rule! ws "ClutterBoxAlignment*" '(<clutter-box-alignment> out))
  (add-type-rule! ws "ClutterTableAlignment*" '(<clutter-table-alignment> out))
  (add-type-rule! ws "ClutterModifierType*" '(<clutter-modifier-type> out))
  (add-type-rule! ws "ClutterSnapEdge*" '(<clutter-snap-edge> out))
  (add-type-rule! ws "ClutterScalingFilter*" '(<clutter-scaling-filter> out))

  (wrap-custom-boxed!
   "ClutterKnot" "CLUTTER_TYPE_KNOT"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_knot_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_knot (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterPathNode" "CLUTTER_TYPE_PATH_NODE"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_path_node_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_path_node (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterMargin" "CLUTTER_TYPE_MARGIN"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_margin_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_margin (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterColor" "CLUTTER_TYPE_COLOR"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_color_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_color (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterActorBox" "CLUTTER_TYPE_ACTOR_BOX"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_actor_box_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_actor_box (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterGeometry" "CLUTTER_TYPE_GEOMETRY"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_geometry_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_geometry (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterVertex" "CLUTTER_TYPE_VERTEX"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_vertex_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_vertex (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterFog" "CLUTTER_TYPE_FOG"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_fog_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_fog (" scm-var ");\n"))

  (wrap-custom-boxed!
   "ClutterPerspective" "CLUTTER_TYPE_PERSPECTIVE"
   ;; wrap
   (list scm-var " = " c-var " ? scm_clutter_perspective_to_scm (" c-var ") : SCM_BOOL_F;\n")
   ;; unwrap
   (list c-var " = scm_scm_to_clutter_perspective (" scm-var ");\n"))

  (wrap-opaque-pointer! ws "ClutterInputDevice*")

  (wrap-opaque-pointer! ws "ClutterEventSequence*")

  (load-defs-with-overrides ws "gnome/defs/clutter.defs"))

(define-method (global-declarations-cg (self <clutter-wrapset>))
  (list (next-method)
        "#include <clutter/clutter.h>\n"
        "#include \"clutter-support.h\"\n"))
