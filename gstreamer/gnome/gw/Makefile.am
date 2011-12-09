include $(top_srcdir)/common.mk

guilegwmodule_DATA = gstreamer-spec.scm gstreamer.scm

EXTRA_DIST = gstreamer-spec.scm

CLEANFILES = $(wildcard gnome-*.log)

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

guilegnomelib_LTLIBRARIES = libgw-guile-gnome-gstreamer.la

########################################################################
## gstreamer
nodist_libgw_guile_gnome_gstreamer_la_SOURCES = guile-gnome-gw-gstreamer.c
libgw_guile_gnome_gstreamer_la_SOURCES = \
	gstreamer-support.c gstreamer-support.h

libgw_guile_gnome_gstreamer_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(GST_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS)
libgw_guile_gnome_gstreamer_la_LIBADD = $(GST_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LIBS) $(GUILE_GLIB_LIBS)
libgw_guile_gnome_gstreamer_la_LDFLAGS = -module

CLEANFILES += $(wildcard guile-gnome-gw-gstreamer.*) gstreamer.scm

BUILT_SOURCES = guile-gnome-gw-gstreamer.c
