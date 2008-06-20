include $(top_srcdir)/common.mk

guilegwmodule_DATA = clutter-spec.scm clutter.scm

EXTRA_DIST = clutter-spec.scm

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

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

CLEANFILES = $(wildcard gnome-*.log) \
	     $(wildcard guile-gnome-gw-clutter.*) clutter.scm

BUILT_SOURCES = guile-gnome-gw-clutter.c $(DOT_X_FILES)

