include $(top_srcdir)/common.mk

SUBDIRS = overrides defs gw
guilemodule_DATA = clutter.scm

if HAVE_CLUTTER_GLX
guilemodule_DATA += clutter-glx.scm
endif

EXTRA_DIST = clutter.scm clutter-glx.scm
DIST_SUBDIRS = overrides defs gw
