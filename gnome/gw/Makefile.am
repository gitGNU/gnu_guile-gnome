include $(top_srcdir)/common.mk

DOT_X_FILES = gda-support.x
DOT_DOC_FILES = gda-support.doc

GUILE_SNARF_CFLAGS += $(GUILE_GLIB_CFLAGS) $(LIBGDA_CFLAGS)

guilegwmodule_DATA = libgda-spec.scm libgda.scm

EXTRA_DIST = libgda-spec.scm

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

lib_LTLIBRARIES = libgw-guile-gnome-libgda.la

########################################################################
## gda
libgw_guile_gnome_libgda_la_SOURCES = \
	gda-support.c gda-support.h
nodist_libgw_guile_gnome_libgda_la_SOURCES = \
	guile-gnome-gw-libgda.c

libgw_guile_gnome_libgda_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(LIBGDA_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS)

libgw_guile_gnome_libgda_la_LIBADD = $(LIBGDA_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LINK_ARGS) $(GUILE_GLIB_LIBS)

libgw_guile_gnome_libgda_la_LDFLAGS = -module

CLEANFILES = $(DOT_X_FILES) $(wildcard gnome-*.log) \
	     $(wildcard guile-gnome-gw-libgda.*) libgda.scm

BUILT_SOURCES = guile-gnome-gw-libgda.c $(DOT_X_FILES)

