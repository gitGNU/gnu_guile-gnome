include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	gtksourceview.defs			\
	gtksourceview-types.defs

EXTRA_DIST = $(defs_DATA)

gtksourceview:
	../../../h2def.py --types \
	   /usr/include/gtksourceview-1.0/gtksourceview/*.h > gtksourceview-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "gtksourceview-types.defs") (include overrides)' \
	   /usr/include/gtksourceview-1.0/gtksourceview/*.h > gtksourceview.defs
