include $(top_srcdir)/common.mk

SUBDIRS = overrides defs
guilemodule_DATA =

if HAVE_CLUTTER
SUBDIRS += gw
guilemodule_DATA += clutter.scm
endif

if HAVE_CLUTTER_GLX
guilemodule_DATA += clutter-glx.scm
endif

EXTRA_DIST = clutter.scm clutter-glx.scm
DIST_SUBDIRS = gw overrides
