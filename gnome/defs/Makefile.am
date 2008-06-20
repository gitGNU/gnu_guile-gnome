include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	clutter.defs				\
	clutter-types.defs			\
	clutter-x11.defs			\
	clutter-x11-types.defs			\
	clutter-glx.defs			\
	clutter-glx-types.defs

EXTRA_DIST = $(defs_DATA)

CLUTTER_PREFIX=/opt/clutter
CLUTTER_INC_DIR=$(CLUTTER_PREFIX)/include/clutter-$(CLUTTER_MAJORMINOR)/clutter
clutter:
	../../../h2def.py --types \
	   $(shell ls $(CLUTTER_INC_DIR)/clutter-*.h | grep -v -- -fixed) > clutter-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "clutter-types.defs") (include overrides)' \
	   $(shell ls $(CLUTTER_INC_DIR)/clutter-*.h | grep -v -- -fixed)> clutter.defs

clutter-x11:
	../../../h2def.py --types \
	   $(CLUTTER_INC_DIR)/x11/*.h > clutter-x11-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "clutter-x11-types.defs") (include overrides)' \
	   $(CLUTTER_INC_DIR)/x11/*.h > clutter-x11.defs

clutter-glx:
	../../../h2def.py --types \
	   $(CLUTTER_INC_DIR)/glx/*.h > clutter-glx-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "clutter-glx-types.defs") (include overrides)' \
	   $(CLUTTER_INC_DIR)/glx/*.h > clutter-glx.defs
