include $(top_srcdir)/common.mk

SUBDIRS =

guilemodule_DATA = 

if HAVE_WNCK
guilemodule_DATA += wnck.scm
SUBDIRS += gw
endif

EXTRA_DIST = wnck.scm
DIST_SUBDIRS = gw
