include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	libecal.defs				\
	libecal-types.defs			\
	libedataserver.defs			\
	libedataserver-types.defs		\
	evolution-data-server.defs

EXTRA_DIST = $(defs_DATA)

libecal:
	../../../h2def.py --types --c-enums \
	   /usr/include/evolution-data-server-1.0/libecal/e-*.h > libecal-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "libecal-types.defs") (include overrides-ecal)' \
	   /usr/include/evolution-data-server-1.0/libecal/e-*.h > libecal.defs

libedataserver:
	../../../h2def.py --types \
	   /usr/include/evolution-data-server-1.0/libedataserver/e-source*.h > libedataserver-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "libedataserver-types.defs") (include overrides-eds)' \
	   /usr/include/evolution-data-server-1.0/libedataserver/e-source*.h > libedataserver.defs
