include $(top_srcdir)/common.mk

guilegwmodule_DATA = dbus-spec.scm dbus.scm

EXTRA_DIST = dbus-spec.scm

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

lib_LTLIBRARIES = libgw-guile-gnome-dbus.la

########################################################################
## dbus
nodist_libgw_guile_gnome_dbus_la_SOURCES = guile-gnome-gw-dbus.c
libgw_guile_gnome_dbus_la_SOURCES = dbus-support.c dbus-support.h

libgw_guile_gnome_dbus_la_CFLAGS = $(GUILE_GLIB_CFLAGS) \
	$(AM_CFLAGS) $(DBUS_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS) -DDBUS_API_SUBJECT_TO_CHANGE

libgw_guile_gnome_dbus_la_LIBADD = $(DBUS_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LINK_ARGS) $(GUILE_GLIB_LIBS)

libgw_guile_gnome_dbus_la_LDFLAGS = -module

CLEANFILES = $(wildcard gnome-*.log) \
	     $(wildcard guile-gnome-gw-dbus.*) dbus.scm

BUILT_SOURCES = guile-gnome-gw-dbus.c $(DOT_X_FILES)

