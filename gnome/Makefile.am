include $(top_srcdir)/common.mk

SUBDIRS = overrides
guilemodule_DATA = dbus.scm

if HAVE_DBUS
SUBDIRS += gw
guilemodule_DATA += dbus.scm
endif

EXTRA_DIST = dbus.scm
DIST_SUBDIRS = gw overrides
