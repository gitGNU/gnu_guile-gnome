include $(top_srcdir)/common.mk

guilegwmodule_DATA = evolution-data-server-spec.scm evolution-data-server.scm

EXTRA_DIST = evolution-data-server-spec.scm

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

guilegnomelib_LTLIBRARIES = libgw-guile-gnome-evolution-data-server.la

########################################################################
## evolution-data-server
nodist_libgw_guile_gnome_evolution_data_server_la_SOURCES = guile-gnome-gw-evolution-data-server.c
libgw_guile_gnome_evolution_data_server_la_SOURCES = \
	evolution-data-server-support.c evolution-data-server-support.h \
	ical-support.c ical-support.h

libgw_guile_gnome_evolution_data_server_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(EVOLUTION_DATA_SERVER_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS)

libgw_guile_gnome_evolution_data_server_la_LIBADD = $(EVOLUTION_DATA_SERVER_LIBS) \
	$(GUILE_LIBS) $(G_WRAP_LIBS) $(GUILE_GLIB_LIBS)

libgw_guile_gnome_evolution_data_server_la_LDFLAGS = -module

CLEANFILES = $(wildcard gnome-*.log) \
	     $(wildcard guile-gnome-gw-evolution-data-server.*) evolution-data-server.scm

BUILT_SOURCES = guile-gnome-gw-evolution-data-server.c $(DOT_X_FILES)

