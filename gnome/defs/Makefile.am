include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	libwnck.defs				\
	libwnck-types.defs

EXTRA_DIST = $(defs_DATA) common-ignores.defs

libwnck:
	../../../h2def.py --c-enums --types \
	   $(DEFS_HEADER_DIR)/libwnck-1.0/libwnck/*.h > libwnck-types.defs
	../../../h2def.py --procs --no-methods \
	   --with-header='(include "common-ignores")' \
	   $(DEFS_HEADER_DIR)/libwnck-1.0/libwnck/*.h > libwnck.defs
