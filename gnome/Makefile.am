include $(top_srcdir)/common.mk

SUBDIRS = defs gstreamer
DIST_SUBDIRS = gw defs gstreamer

guilemodule_DATA = 

if HAVE_GSTREAMER
guilemodule_DATA += gstreamer.scm
SUBDIRS += gw
endif

EXTRA_DIST = gstreamer.scm
