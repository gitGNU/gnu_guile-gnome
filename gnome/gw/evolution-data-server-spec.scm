;; guile-gnome
;; Copyright (C) 2004 Free Software Foundation, Inc.

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
;;g-wrap specification for evolution-data-server.
;;
;;; Code:

(define-module (gnome gw evolution-data-server-spec)
  #:use-module (oop goops)
  #:use-module (g-wrap)
  #:use-module (g-wrap guile)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject))

(define-class <evolution-data-server-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-evolution-data-server
  #:dependencies '(standard gnome-glib gnome-gobject))

(define-method (initialize (ws <evolution-data-server-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw evolution-data-server) initargs)))
  
  (add-type! ws (make <ical-time-type>
                  #:gtype-id "(icaltimetype_get_gtype ())" 
                  #:ctype "IcalTimeType" ;; only used to get the scm name
                  #:c-type-name "struct icaltimetype*"
                  #:c-const-type-name "const struct icaltimetype*"
                  #:ffspec 'pointer
                  #:wrapped "Custom"))
  (add-type-alias! ws "struct-icaltimetype*" '<ical-time-type>)
  (add-type-rule! ws '(("struct-icaltimetype**" "*")) '(<ical-time-type> out caller-owned))
  
  (add-type! ws (make <ical-geo-type>
                  #:ctype "IcalGeoType" ;; only used to get the scm name
                  #:c-type-name "struct icalgeotype*"
                  #:c-const-type-name "const struct icalgeotype*"
                  #:ffspec 'pointer
                  #:wrapped "Custom"))
  (add-type-alias! ws "struct-icalgeotype*" '<ical-geo-type>)
  (add-type-rule! ws '(("struct-icalgeotype**" "*")) '(<ical-geo-type> out caller-owned))
  
  (add-type! ws (make <e-cal-component-text-type>
                  #:ctype "ECalComponentText" ;; only used to get the scm name
                  #:c-type-name "ECalComponentText*"
                  #:c-const-type-name "ECalComponentText*"
                  #:ffspec 'pointer
                  #:wrapped "Custom"))
  (add-type-alias! ws "ECalComponentText*" '<e-cal-component-text>)

  (add-type! ws (make <e-cal-component-date-time-type>
                  #:ctype "ECalComponentDateTime" ;; only used to get the scm name
                  #:c-type-name "ECalComponentDateTime*"
                  #:c-const-type-name "ECalComponentDateTime*"
                  #:ffspec 'pointer
                  #:wrapped "Custom"))
  (add-type-alias! ws "ECalComponentDateTime*" '<e-cal-component-date-time>)

  ;; don't seem to pick up their 'const qualifiers, grr.
  ;;(add-type-rule! ws '(("const-gchar**" "*")) '(mchars out callee-owned))
  ;;(add-type-rule! ws '(("const-char**" "*")) '(mchars out callee-owned const))

  (load-defs-with-overrides ws "gnome/defs/evolution-data-server.defs"))

(define-method (global-declarations-cg (self <evolution-data-server-wrapset>))
  (list (next-method)
        "#include <libecal/e-cal.h>\n"
        "#include <libedataserver/e-source-list.h>\n"
        "#include \"evolution-data-server-support.h\"\n"))

;; icaltimetype
(define-class <ical-time-type> (<gobject-type-base>))
(define-method (unwrap-value-cg (type <ical-time-type>)
                                (value <gw-value>)
                                status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list c-var " = scm_date_to_icaltimetype (" scm-var ");\n")))
(define-method (wrap-value-cg (type <ical-time-type>)
                              (value <gw-value>)
                              status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list scm-var " = scm_icaltimetype_to_date (" c-var ");\n")))

;; icalgeotype
(define-class <ical-geo-type> (<gobject-type-base>))
(define-method (unwrap-value-cg (type <ical-geo-type>)
                                (value <gw-value>)
                                status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list
     "if (SCM_FALSEP (" scm-var ")) {\n"
     "  " c-var " = NULL;\n"
     "} else {\n"
     "  " c-var " = malloc (sizeof (struct icalgeotype));\n"
     "  " c-var "->lat = scm_num2float (SCM_CAR (" scm-var "), 0, \"ical-geo-type\");\n"
     "  " c-var "->lon = scm_num2float (SCM_CDR (" scm-var "), 0, \"ical-geo-type\");\n"
     "}\n")))
(define-method (wrap-value-cg (type <ical-geo-type>)
                              (value <gw-value>)
                              status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list
     "if (" c-var ")\n"
     "  " scm-var " = scm_cons (scm_float2num (" c-var "->lat), scm_float2num (" c-var "->lon));\n"
     "else\n"
     "  " scm-var " = SCM_BOOL_F;\n")))
(define-method (destroy-value-cg (type <ical-geo-type>)
                                 (value <gw-value>)
                                 status-var)
  (list "if (" (var value) ") free (" (var value) ");\n"))

;; ecalcomponenttext
(define-class <e-cal-component-text-type> (<gobject-type-base>))
(define-method (unwrap-value-cg (type <e-cal-component-text-type>)
                                (value <gw-value>)
                                status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list
     c-var " = g_new0 (ECalComponentText,1);\n"
     "{\n"
     "  SCM tmp;\n"
     "  tmp = SCM_CAR (" scm-var ");\n"
     "  " c-var "->value = SCM_FALSEP (tmp) ? NULL : strdup (SCM_STRING_CHARS (tmp));\n"
     "  tmp = SCM_CDR (" scm-var ");\n"
     "  " c-var "->altrep = SCM_FALSEP (tmp) ? NULL : strdup (SCM_STRING_CHARS (tmp));\n"
     "}\n")))
(define-method (wrap-value-cg (type <e-cal-component-text-type>)
                              (value <gw-value>)
                              status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list
     scm-var " = scm_cons (" c-var "->value ? scm_makfrom0str (" c-var "->value) : SCM_BOOL_F,\n"
     "            " c-var "->altrep ? scm_makfrom0str (" c-var "->altrep) : SCM_BOOL_F);\n")))
(define-method (destroy-value-cg (type <e-cal-component-text-type>)
                                 (value <gw-value>)
                                 status-var)
  (list "if (" (var value) ") {\n"
        "  free (" (var value) ");\n"
        "}\n"))

;; ecalcomponenttime
(define-class <e-cal-component-date-time-type> (<gobject-type-base>))
(define-method (unwrap-value-cg (type <e-cal-component-date-time-type>)
                                (value <gw-value>)
                                status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list
     c-var " = g_new0 (ECalComponentDateTime,1);\n"
     "{\n"
     "  SCM tmp;\n"
     "  tmp = SCM_CAR (" scm-var ");\n"
     "  " c-var "->value = SCM_FALSEP (tmp) ? NULL : scm_date_to_icaltimetype (tmp);\n"
     "  tmp = SCM_CDR (" scm-var ");\n"
     "  " c-var "->tzid = SCM_FALSEP (tmp) ? NULL : strdup (SCM_STRING_CHARS (tmp));\n"
     "}\n")))
(define-method (wrap-value-cg (type <e-cal-component-date-time-type>)
                              (value <gw-value>)
                              status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value)))
    (list
     scm-var " = scm_cons (" c-var "->value ? scm_icaltimetype_to_date (" c-var "->value) : SCM_BOOL_F,\n"
     "            " c-var "->tzid ? scm_makfrom0str (" c-var "->tzid) : SCM_BOOL_F);\n")))
(define-method (destroy-value-cg (type <e-cal-component-date-time-type>)
                                 (value <gw-value>)
                                 status-var)
  (list "if (" (var value) ") {\n"
        "  if (" (var value) "->value) free (" (var value) "->value);\n"
        "  free (" (var value) ");\n"
        "}\n"))
