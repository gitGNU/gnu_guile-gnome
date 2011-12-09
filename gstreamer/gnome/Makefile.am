include $(top_srcdir)/common.mk

SUBDIRS = defs gstreamer overrides
DIST_SUBDIRS = defs gw gstreamer overrides

guilemodule_DATA = 

if HAVE_GSTREAMER
guilemodule_DATA += gstreamer.scm
SUBDIRS += gw
endif

EXTRA_DIST = gstreamer.scm
