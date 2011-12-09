include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	libwnck.defs				\
	libwnck-types.defs

EXTRA_DIST = $(defs_DATA) common-ignores.defs

WNCK_DIR = /usr/include/libwnck-1.0/

WNCK_INCLS = application.h \
	class-group.h \
	libwnck.h \
	pager.h \
	screen.h \
	selector.h \
	tasklist.h \
	util.h \
	window-action-menu.h \
	window.h \
	workspace.h

libwnck:
	../../../h2def.py --types \
	   $(addprefix $(WNCK_DIR)/libwnck/,$(WNCK_INCLS)) > libwnck-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "common-ignores.defs") (include "libwnck-types.defs") (include overrides)' \
	   $(addprefix $(WNCK_DIR)/libwnck/,$(WNCK_INCLS)) > libwnck.defs
