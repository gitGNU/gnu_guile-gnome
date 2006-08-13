include $(top_srcdir)/common.mk

SUBDIRS = defs overrides

guilemodule_DATA = 

if HAVE_GTKSOURCEVIEW
guilemodule_DATA += source-view.scm
SUBDIRS += gw
endif

EXTRA_DIST = source-view.scm
DIST_SUBDIRS = defs gw overrides
