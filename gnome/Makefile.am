include $(top_srcdir)/common.mk

SUBDIRS = gstreamer overrides
DIST_SUBDIRS = gw gstreamer overrides

guilemodule_DATA = 

if HAVE_GSTREAMER
guilemodule_DATA += gstreamer.scm
SUBDIRS += gw
endif

EXTRA_DIST = gstreamer.scm
