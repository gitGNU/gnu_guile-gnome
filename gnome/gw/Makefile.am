include $(top_srcdir)/common.mk

guilegwmodule_DATA = libwnck-spec.scm libwnck.scm

EXTRA_DIST = libwnck-spec.scm

CLEANFILES = $(wildcard gnome-*.log)

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

lib_LTLIBRARIES = libgw-guile-gnome-libwnck.la

########################################################################
## libwnck
libgw_guile_gnome_libwnck_la_SOURCES = guile-gnome-gw-libwnck.c 

libgw_guile_gnome_libwnck_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(WNCK_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS) -DGTK_DISABLE_DEPRECATED

libgw_guile_gnome_libwnck_la_LIBADD = $(WNCK_LIBS) $(GUILE_GLIB_LIBS) \
	$(GUILE_LIBS) $(G_WRAP_LINK_ARGS) 

libgw_guile_gnome_libwnck_la_LDFLAGS = -module

CLEANFILES += $(wildcard guile-gnome-gw-libwnck.*) libwnck.scm

BUILT_SOURCES = guile-gnome-gw-libwnck.c

