
####
#### Copyright (C) 2007, 2015 Free Software Foundation, Inc.

#### This file is part of Guile-Clutter.
#### A Guile-Clutter of Really Important Procedures.

#### Guile-Clutter is free software: you can redistribute it and/or
#### modify it under the terms of the GNU General Public License as
#### published by the Free Software Foundation, either version 3 of
#### the License, or (at your option) any later version.

#### Guile-Clutter is distributed in the hope that it will be useful, but
#### WITHOUT ANY WARRANTY; without even the implied warranty of
#### MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#### General Public License for more details.

#### You should have received a copy of the GNU General Public License
#### along with Guile-Clutter.  If not, see <http://www.gnu.org/licenses/>.
####


include $(top_srcdir)/common.mk

defsdir = $(guilemoduledir)/defs

defs_DATA = 					\
	clutter.defs				\
	clutter-types.defs			\
	clutter-x11.defs			\
	clutter-x11-types.defs			\
	clutter-glx.defs			\
	clutter-glx-types.defs			\
	cogl-types.defs

EXTRA_DIST = $(defs_DATA)

CLUTTER_PREFIX=/opt/clutter
CLUTTER_INC_DIR=$(CLUTTER_PREFIX)/include/clutter-$(CLUTTER_MAJORMINOR)/clutter

# We include deprecated header files here so that we continue to bind
# functions that we deprecated in the current stable release.  When the
# next stable release comes out, there will be new compile warnings, and
# those warnings should be added to the clutter.defs file in ../overrides/.

# we keep copy of the below target in case we need to run it in the
# future, but so far we only need COGL_TYPE_PIXEL_FORMAT, which is in
# cogl-types.defs already.
#	../../../h2def.py --types ../gw/cogl-support.h > cogl-types.defs
clutter:
	../../../h2def.py --types \
	   $(CLUTTER_INC_DIR)/*.h $(CLUTTER_INC_DIR)/deprecated/*.h > clutter-types.defs
	../../../h2def.py --procs \
	   --with-header='(include type-ignores) (include "cogl-types.defs") (include "clutter-types.defs") (include overrides)' \
	   $(CLUTTER_INC_DIR)/*.h $(CLUTTER_INC_DIR)/deprecated/*.h > clutter.defs

clutter-x11:
	../../../h2def.py --types  --type-re='s/^CLUTTER_X11/CLUTTER_X11_TYPE/'\
	   $(CLUTTER_INC_DIR)/x11/*.h > clutter-x11-types.defs
	../../../h2def.py --procs \
	   --with-header='(include type-ignores) (include "clutter-x11-types.defs") (include overrides)' \
	   $(CLUTTER_INC_DIR)/x11/*.h > clutter-x11.defs

clutter-glx:
	../../../h2def.py --types  --type-re='s/^CLUTTER_GLX/CLUTTER_GLX_TYPE/'\
	   $(CLUTTER_INC_DIR)/glx/*.h > clutter-glx-types.defs
	../../../h2def.py --procs \
	   --with-header='(include "clutter-glx-types.defs") (include overrides)' \
	   $(CLUTTER_INC_DIR)/glx/*.h > clutter-glx.defs
