include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	gstreamer.defs				\
	gstreamer-types.defs

EXTRA_DIST = $(defs_DATA)

gstreamer:
	../../../h2def.py --types \
	   /usr/include/gstreamer-0.10/gst/gst*.h > gstreamer-types.defs
	../../../h2def.py --procs \
	   --with-header='(include type-ignores) (include "gstreamer-types.defs") (include overrides)' \
	   /usr/include/gstreamer-0.10/gst/gst*.h > gstreamer.defs
