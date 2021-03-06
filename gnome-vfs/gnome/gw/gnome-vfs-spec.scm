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
;;g-wrap specification for gnome-vfs.
;;
;;; Code:

(define-module (gnome gw gnome-vfs-spec)
  #:use-module (oop goops)
  #:use-module (gnome gw support g-wrap)
  #:use-module (gnome gw gobject-spec)
  #:use-module (gnome gw support defs)
  #:use-module (gnome gw support gobject))

(define-class <gnome-vfs-wrapset> (<gobject-wrapset-base>)
  #:id 'gnome-gnome-vfs
  #:dependencies '(standard gnome-glib gnome-gobject))

(define-method (initialize (ws <gnome-vfs-wrapset>) initargs)
  (next-method ws (cons #:module (cons '(gnome gw gnome-vfs) initargs)))
  
  (add-type! ws (make <gnome-vfs-result-type>
                  #:gtype-id "GNOME_VFS_TYPE_VFS_RESULT"
                  #:ctype "GnomeVFSResult"
                  #:c-type-name "GnomeVFSResult"
                  #:c-const-type-name "GnomeVFSResult"
                  #:ffspec 'uint
                  #:wrapped "Custom"))
  (add-type-alias! ws "GnomeVFSResult" '<gnome-vfs-result>)

  (wrap-refcounted-pointer! ws "GnomeVFSURI"
                            "gnome_vfs_uri_ref"
                            "gnome_vfs_uri_unref")

  (wrap-refcounted-pointer! ws "GnomeVFSFileInfo"
                            "gnome_vfs_file_info_ref"
                            "gnome_vfs_file_info_unref")

  (wrap-freeable-pointer! ws "GnomeVFSMimeApplication"
                          "gnome_vfs_mime_application_free")

  ;; these are platform-dependent -- FIXME.
  (add-type-alias! ws "GnomeVFSFileSize" 'unsigned-int64)
  (add-type-alias! ws "GnomeVFSFileOffset" 'int64)
  (load-defs-with-overrides ws "gnome/defs/gnome-vfs.defs"))

(define-method (global-declarations-cg (self <gnome-vfs-wrapset>))
  (list (next-method)
        "#include <libgnomevfs/gnome-vfs.h>\n"
        "#include <libgnomevfs/gnome-vfs-enum-types.h>\n"
        "#include <libgnomevfs/gnome-vfs-mime.h>\n"
        "#include <libgnomevfs/gnome-vfs-mime-handlers.h>\n"
        "#include <libgnomevfs/gnome-vfs-application-registry.h>\n"
        "#include \"gnome-vfs-port.h\"\n"
        "#include \"gnome-vfs-support.h\"\n"))

(define-method (initializations-cg (self <gnome-vfs-wrapset>) err)
  (list
   (next-method)
   "gnome_vfs_init ();\n"
   "scm_init_gnome_vfs_ports ();\n"
   "g_type_class_ref (GNOME_VFS_TYPE_VFS_RESULT);\n"))

;; if a GnomeVFSResult return value is not GNOME_VFS_OK, throw an error.
(define-class <gnome-vfs-result-type> (<gobject-classed-type>))

(define-method (make-typespec (type <gnome-vfs-result-type>) (options <list>))
  (next-method type (cons 'caller-owned options)))

(define-method (unwrap-value-cg (type <gnome-vfs-result-type>)
                                (value <gw-value>)
                                status-var)
  (let ((c-var (var value))
        (scm-var (scm-var value))
        (gtype-id (gtype-id type)))
    (list
     "if (scm_c_gvalue_holds (" scm-var ", " gtype-id "))\n"
     "  " c-var " = g_value_get_enum (scm_c_gvalue_peek_value (" scm-var "));\n"
     "else {\n"
     "  GValue tmp = {0,};\n"
     "  g_value_init (&tmp, " gtype-id ");\n"
     "  scm_c_gvalue_set (&tmp, " scm-var ");\n"
     "  " c-var " = g_value_get_enum (&tmp);\n"
     "}\n")))

(define-method (wrap-value-cg (type <gnome-vfs-result-type>)
                                    (result <gw-value>)
                                    status-var)
  (let ((c-var (var result))
        (scm-var (scm-var result)))
    (list
     "if (" c-var " == GNOME_VFS_OK)\n"
     "  " scm-var " = SCM_UNSPECIFIED;\n"
     "else\n"
     "  RESULT_ERROR(" c-var ");\n")))
