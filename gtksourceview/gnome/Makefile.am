include $(top_srcdir)/common.mk

SUBDIRS = defs overrides

guilemodule_DATA = 

if HAVE_GTKSOURCEVIEW
guilemodule_DATA += gtksourceview.scm
SUBDIRS += gw
endif

EXTRA_DIST = gtksourceview.scm
DIST_SUBDIRS = defs gw overrides
