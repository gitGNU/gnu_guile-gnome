include $(top_srcdir)/common.mk

SUBDIRS = overrides

guilemodule_DATA = 

if HAVE_LIBGDA
guilemodule_DATA += gda.scm
SUBDIRS += gw
endif

EXTRA_DIST = gda.scm
DIST_SUBDIRS = gw overrides
