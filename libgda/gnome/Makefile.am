include $(top_srcdir)/common.mk

SUBDIRS = defs overrides

guilemodule_DATA = 

if HAVE_LIBGDA
guilemodule_DATA += gda.scm
SUBDIRS += gw
endif

EXTRA_DIST = gda.scm
DIST_SUBDIRS = defs gw overrides
