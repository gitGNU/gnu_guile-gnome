include $(top_srcdir)/common.mk

guilegwmodule_DATA = clutter-spec.scm clutter.scm

EXTRA_DIST = clutter-spec.scm clutter-glx-spec.scm

lib_LTLIBRARIES = libgw-guile-gnome-clutter.la

########################################################################
## clutter
nodist_libgw_guile_gnome_clutter_la_SOURCES = guile-gnome-gw-clutter.c
libgw_guile_gnome_clutter_la_SOURCES = clutter-support.c clutter-support.h

libgw_guile_gnome_clutter_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(CLUTTER_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS)

libgw_guile_gnome_clutter_la_LIBADD = $(CLUTTER_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LIBS) $(GUILE_GLIB_LIBS)

libgw_guile_gnome_clutter_la_LDFLAGS = -module

########################################################################
## clutter
nodist_libgw_guile_gnome_clutter_glx_la_SOURCES = guile-gnome-gw-clutter-glx.c
#libgw_guile_gnome_clutter_glx_la_SOURCES = clutter-support.c clutter-support.h

libgw_guile_gnome_clutter_glx_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(CLUTTER_GLX_CFLAGS) $(GUILE_CFLAGS) $(G_WRAP_CFLAGS)

libgw_guile_gnome_clutter_glx_la_LIBADD = $(CLUTTER_GLX_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LIBS) $(GUILE_GLIB_LIBS)

libgw_guile_gnome_clutter_glx_la_LDFLAGS = -module

CLEANFILES = $(wildcard gnome-*.log) \
	     $(wildcard guile-gnome-gw-clutter*) clutter.scm clutter-glx.scm

BUILT_SOURCES = guile-gnome-gw-clutter.c guile-gnome-gw-clutter-glx.c

if HAVE_CLUTTER_GLX
lib_LTLIBRARIES += libgw-guile-gnome-clutter-glx.la
guilegwmodule_DATA += clutter-glx-spec.scm clutter-glx.scm
endif
