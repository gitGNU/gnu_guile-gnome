include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	panel-applet.defs			\
	panel-applet-types.defs

EXTRA_DIST = $(defs_DATA)

panel-applet:
	../../../h2def.py --types \
	   /usr/include/panel-2.0/panel-*.h > panel-applet-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "panel-applet-types.defs") (include overrides)' \
	   /usr/include/panel-2.0/panel-*.h > panel-applet.defs
