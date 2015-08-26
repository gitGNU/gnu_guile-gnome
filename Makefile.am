
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

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=meta/guile-clutter-1.0.pc

SUBDIRS = \
	clutter \
	doc	\
	tests

EXTRA_DIST = \
	AUTHORS				\
	ChangeLog			\
	COPYING				\
	HACKING				\
	INSTALL				\
	LICENSE				\
	NEWS				\
	README				\
	dev-environ.in			\
	meta/guile-clutter-1.0.pc.in	\
	$(wildcard m4/*)		\
	$(wildcard examples/*)

CLEANFILES = \
	meta/guile-clutter-1.0.pc
# $(GOBJECTS)
# $(bin_SCRIPTS)

dist-gzip: distdir
	$(AMTAR) --format=ustar -chf - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

dist dist-all: distdir
	$(AMTAR) --format=ustar -chf - $(distdir) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	$(am__remove_distdir)

distclean-local:
	rm -f Makefile.in
	rm -f aclocal.m4
	rm -rf autom4te.cache
	rm -rf build-aux
	rm -f configure
	rm -f *~
	rm -f guile-clutter-*.tar.gz
	rm -f doc/Makefile.in
	rm -f doc/clutter/Makefile.in
	rm -f doc/clutter/stamp-vti
	rm -f doc/clutter/*.aux doc/clutter/*.cp doc/clutter/*.cps
	rm -f doc/clutter/*.fn doc/clutter/*.ky doc/clutter/*.log
	rm -f doc/clutter/*.pg doc/clutter/*.toc doc/clutter/*.tp
	rm -f doc/clutter/*.vr
	rm -f doc/clutter/guile-gnome-clutter.info*
	rm -f doc/clutter/guile-gnome-clutter.pdf
	rm -f doc/clutter-glx/Makefile.in
	rm -f doc/clutter-glx/stamp-vti
	rm -f doc/clutter-glx/*.aux doc/clutter-glx/*.cp doc/clutter-glx/*.cps
	rm -f doc/clutter-glx/*.fn doc/clutter-glx/*.ky doc/clutter-glx/*.log
	rm -f doc/clutter-glx/*.pg doc/clutter-glx/*.toc doc/clutter-glx/*.tp
	rm -f doc/clutter-glx/*.vr
	rm -f guile-gnome-clutter-glx.info*
	rm -f guile-gnome-clutter-glx.pdf
	rm -f clutter/*~
