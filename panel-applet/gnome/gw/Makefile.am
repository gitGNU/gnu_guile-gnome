include $(top_srcdir)/common.mk

guilegwmodule_DATA = panel-applet-spec.scm panel-applet.scm

EXTRA_DIST = panel-applet-spec.scm

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

guilegnomelib_LTLIBRARIES = libgw-guile-gnome-panel-applet.la

########################################################################
## panel-applet
nodist_libgw_guile_gnome_panel_applet_la_SOURCES = guile-gnome-gw-panel-applet.c
libgw_guile_gnome_panel_applet_la_SOURCES = panel-applet-support.c panel-applet-support.h

libgw_guile_gnome_panel_applet_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(PANEL_APPLET_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS)

libgw_guile_gnome_panel_applet_la_LIBADD = $(PANEL_APPLET_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LIBS) $(GUILE_GLIB_LIBS)

libgw_guile_gnome_panel_applet_la_LDFLAGS = -module

CLEANFILES = $(wildcard gnome-*.log) \
	     $(wildcard guile-gnome-gw-panel-applet.*) panel-applet.scm

BUILT_SOURCES = guile-gnome-gw-panel-applet.c $(DOT_X_FILES)

