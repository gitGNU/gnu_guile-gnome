include $(top_srcdir)/common.mk

SUBDIRS = gnome examples 

EXTRA_DIST = ChangeLog NEWS AUTHORS README $(wildcard *.ac) VERSION

if HAVE_EVOLUTION_DATA_SERVER
all-local: $(pcifiles) $(pcufiles)
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = $(pcifiles)
endif
CLEANFILES = $(pcifiles) $(pcufiles)
