include $(top_srcdir)/common.mk

SUBDIRS = defs overrides
guilemodule_DATA =

if HAVE_EVOLUTION_DATA_SERVER
SUBDIRS += gw
guilemodule_DATA += evolution-data-server.scm
endif

EXTRA_DIST = evolution-data-server.scm
DIST_SUBDIRS = defs gw overrides
