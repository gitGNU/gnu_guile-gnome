
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

guilegwmodule_DATA =		\
	clutter-spec.scm	\
	clutter.scm

EXTRA_DIST = \
	clutter-spec.scm	\
	clutter-glx-spec.scm

lib_LTLIBRARIES = \
	libgw-guile-gnome-clutter.la


###
### Clutter
###

nodist_libgw_guile_gnome_clutter_la_SOURCES = \
	guile-gnome-gw-clutter.c

libgw_guile_gnome_clutter_la_SOURCES =	\
	clutter-support.c		\
	clutter-support.h

libgw_guile_gnome_clutter_la_CFLAGS =				\
	$(GUILE_GNOME_CFLAGS)					\
	$(AM_CFLAGS) $(CLUTTER_CFLAGS) $(GUILE_CFLAGS)		\
	$(G_WRAP_CFLAGS) $(GUILE_CAIRO_CFLAGS)

libgw_guile_gnome_clutter_la_LIBADD = $(CLUTTER_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LIBS) $(GUILE_CAIRO_LIBS) $(GUILE_GNOME_LIBS)

libgw_guile_gnome_clutter_la_LDFLAGS = -module


###
### Clutter-Glx
###

nodist_libgw_guile_gnome_clutter_glx_la_SOURCES = \
	guile-gnome-gw-clutter-glx.c
#libgw_guile_gnome_clutter_glx_la_SOURCES = clutter-support.c clutter-support.h

libgw_guile_gnome_clutter_glx_la_CFLAGS =			\
	$(GUILE_GNOME_CFLAGS)					\
	$(AM_CFLAGS) $(CLUTTER_GLX_CFLAGS) $(GUILE_CFLAGS)	\
	$(G_WRAP_CFLAGS) $(GUILE_CAIRO_CFLAGS)

libgw_guile_gnome_clutter_glx_la_LIBADD = \
	$(CLUTTER_GLX_LIBS) $(GUILE_LIBS) \
	$(G_WRAP_LIBS) $(GUILE_CAIRO_LIBS) $(GUILE_GNOME_LIBS)

libgw_guile_gnome_clutter_glx_la_LDFLAGS = -module

CLEANFILES = 					\
	$(wildcard gnome-*.log)			\
	$(wildcard guile-gnome-gw-clutter*)	\
	clutter.scm				\
	clutter-glx.scm

BUILT_SOURCES = \
	guile-gnome-gw-clutter.c \
	guile-gnome-gw-clutter-glx.c

if HAVE_CLUTTER_GLX
lib_LTLIBRARIES +=		\
	libgw-guile-gnome-clutter-glx.la
guilegwmodule_DATA +=		\
	clutter-glx-spec.scm	\
	clutter-glx.scm
endif
