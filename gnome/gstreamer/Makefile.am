AM_CFLAGS = $(WARN_CFLAGS)

guilegstreamermoduledir = $(datadir)/guile/gnome/gstreamer

guilegstreamermodule_DATA = \
	gw-gstreamer-spec.scm gw-gstreamer.scm \
	generics.scm config.scm

EXTRA_DIST = $(guilegstreamermodule_DATA) config.scm.in

# There is one shared library for each wrapset. Each is produced by
# g-wrap, but also may contain some hand-coded routines.

lib_LTLIBRARIES = \
	libguile-gnome-gw-gstreamer.la

########################################################################
## gstreamer
libguile_gnome_gw_gstreamer_la_SOURCES = guile-gnome-gw-gstreamer.c	\
	guile-gnome-gw-gstreamer.h gstreamer-support.c gstreamer-support.h

libguile_gnome_gw_gstreamer_la_CFLAGS = -I\
	$(AM_CFLAGS) $(GST_CFLAGS) $(GUILE_GOBJECT_CFLAGS) $(GUILE_CFLAGS) \
	$(G_WRAP_CFLAGS) -DGSTREAMER_DISABLE_DEPRECATED

libguile_gnome_gw_gstreamer_la_LIBADD = $(GST_LIBS) $(GUILE_LIBS) \
	$(GUILE_GOBJECT_LIBS) $(G_WRAP_LINK_ARGS)

libguile_gnome_gw_gstreamer_la_LDFLAGS = \
	-export-dynamic

gw-gstreamer.scm guile-gnome-gw-gstreamer.c guile-gnome-gw-gstreamer.h: \
	gw-gstreamer-spec.scm $(top_srcdir)/gnome/defs/gstreamer.defs \
	$(top_srcdir)/gnome/defs/gstreamer-overrides.defs
	guile -c \
	  "(set! %load-path (cons \"${G_WRAP_MODULE_DIR}\" %load-path)) \
	   (set! %load-path (cons \"${top_srcdir}\" %load-path)) \
	   (primitive-load \"$(srcdir)/gw-gstreamer-spec.scm\") \
	   (gw:generate-wrapset \"guile-gnome-gw-gstreamer\")"
	mv guile-gnome-gw-gstreamer.scm gw-gstreamer.scm

BUILT_SOURCES = \
	guile-gnome-gw-gstreamer.h guile-gnome-gw-gstreamer.c

CLEANFILES = $(BUILT_SOURCES) gw-gstreamer.scm guile-gnome-gw-glib.log	\
	guile-gnome-gw-gstreamer.log

DISTCLEANFILES = $(BUILT_SOURCES) guile-gnome-gw-gstreamer.html

dist-hook:
	for file in $(DISTCLEANFILES); do rm -f $(distdir)/$$file; done
