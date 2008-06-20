include $(top_srcdir)/common.mk

SUBDIRS = overrides
guilemodule_DATA =

if HAVE_CLUTTER
SUBDIRS += gw
guilemodule_DATA += clutter.scm
endif

EXTRA_DIST = clutter.scm
DIST_SUBDIRS = gw overrides
