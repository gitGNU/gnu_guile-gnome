include $(top_srcdir)/common.mk

SUBDIRS = overrides
guilemodule_DATA =

if HAVE_PANEL_APPLET
SUBDIRS += gw
guilemodule_DATA += panel-applet.scm
endif

EXTRA_DIST = panel-applet.scm
DIST_SUBDIRS = gw overrides
