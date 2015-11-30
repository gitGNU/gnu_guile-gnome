dnl -*- Autoconf -*-
dnl
dnl   configuration script for Guile-Clutter
dnl   Process this file with autoconf to produce configure.
dnl

define(GUILE_CLUTTER_CONFIGURE_COPYRIGHT,[[

Copyright (C) 2011,2012,2014,2015  Free Software Foundation, Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this program.  If not, see
<http://www.gnu.org/licenses/>.

]])

AC_PREREQ(2.61)

AC_INIT([guile-clutter],
  [1.14.7-dev],
  [guile-gtk-general at gnu.org])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AC_COPYRIGHT(GUILE_CLUTTER_CONFIGURE_COPYRIGHT)
AM_CONFIG_HEADER(config.h)

dnl Use `serial-tests' so the output `check-guile' is not hidden
dnl (`parallel-tests' is the default in Automake 1.13.)
dnl `serial-tests' was introduced in Automake 1.12.
AM_INIT_AUTOMAKE(1.12 -Wno-portability -Wno-syntax serial-tests)

# Note:
# This is _not_ the guile-clutter api version,
# but the guile-gnome api version.
API_VERSION=2
AC_SUBST(API_VERSION)

AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
LT_INIT([disable-static])

AC_SUBST(AG_PKG_CONFIG_PATH, [$pkg_config_path])
AG_PACKAGES="clutter"
AC_SUBST(AG_PACKAGES)

MK=""
AC_SUBST(MK)

WARN_CFLAGS=-Wall
AC_ARG_ENABLE([Werror],
  AC_HELP_STRING([--enable-Werror],[Stop the build on warnings]),
  [WARN_CFLAGS="-Wall -Werror"], [])
AC_SUBST(WARN_CFLAGS)

DISABLE_DEPRECATED=false
AC_ARG_ENABLE([deprecated],
AC_HELP_STRING([--disable-deprecated],
  [Disable wrapping of functions deprecated upstream]),
  [DISABLE_DEPRECATED=true],
  [DISABLE_DEPRECATED=false])

GUILE_PKG([2.2, 2.0])
GUILE_PROGS([2.0.11])
GUILE_GLOBAL_SITE_DIR
GUILE_SITE_DIR
GUILE_SITE_CCACHE_DIR

PKG_CHECK_MODULES(G_WRAP, g-wrap-2.0-guile >= 1.9.15)
AC_SUBST(G_WRAP_MODULE_DIR,
  `${PKG_CONFIG} --variable=module_directory g-wrap-2.0-guile`)
AC_SUBST(G_WRAP_LIB_DIR,
  `echo $G_WRAP_MODULE_DIR | sed -e 's|share/guile|lib|'`)


# we need guile-cairo >= 1.10.0 but it hasn't been released yet, so we
# need to trust the user having installed guile-cairo from the source
# PKG_CHECK_MODULES(GUILE_CAIRO, guile-cairo >= 1.10.0)
PKG_CHECK_MODULES(GUILE_CAIRO, guile-cairo)

GUILE_GNOME_PROGS([2.16.3])
GUILE_GNOME_FLAGS
GUILE_GNOME_INST_DIR
GUILE_GNOME_LIBS_PATH

dnl some modules guile must find [using %load-path], sounds too much?
dnl once a user pretended he 'just installed' g-wrap, but guile still
dnl could not find g-wrap's modules.  With these additional tests,
dnl such a situation will now fail at configure time, saving us some
dnl useful time!
GUILE_MODULE_REQUIRED([gnome-2])
GUILE_MODULE_REQUIRED([g-wrap])
GUILE_MODULE_REQUIRED([g-wrap guile])
GUILE_MODULE_REQUIRED([g-wrap c-codegen])
GUILE_MODULE_REQUIRED([g-wrap scm-codegen])
dnl guile-gnome is a set of wrappers, we need to check that the user
dnl has wrapped the gnome libraries guile-clutter needs.  strictly
dnl speaking, the list below is 'too much' because some guile-gnome
dnl modules are always part of guile-gnome, but hey, no arm and nice
dnl to see all the checks.
GUILE_GNOME_MODULE_REQUIRED([gnome gobject])
GUILE_GNOME_MODULE_REQUIRED([gnome gw support modules])
GUILE_GNOME_MODULE_REQUIRED([gnome gw support g-wrap])
GUILE_GNOME_MODULE_REQUIRED([gnome gw glib])
GUILE_GNOME_MODULE_REQUIRED([gnome gw gobject])
GUILE_GNOME_MODULE_REQUIRED([gnome gw gobject-spec])
GUILE_GNOME_MODULE_REQUIRED([gnome gw gdk]) ## gdk-pixbuf
GUILE_GNOME_MODULE_REQUIRED([gnome gw cairo])
GUILE_GNOME_MODULE_REQUIRED([gnome gw cairo-spec])
GUILE_GNOME_MODULE_REQUIRED([gnome gw atk])
GUILE_GNOME_MODULE_REQUIRED([gnome gw atk-spec])
GUILE_GNOME_MODULE_REQUIRED([gnome gw pango])
GUILE_GNOME_MODULE_REQUIRED([gnome gw pango-spec])

COGL_MAJORMINOR=1.0
COGL_MINIMUM_VERSION=1.12.2
AC_SUBST(COGL_MAJORMINOR)

PKG_CHECK_MODULES(COGL,
  cogl-$COGL_MAJORMINOR >= $COGL_MINIMUM_VERSION)

CLUTTER_MAJORMINOR=1.0
CLUTTER_MINIMUM_VERSION=1.12.2
AC_SUBST(CLUTTER_MAJORMINOR)

PKG_CHECK_MODULES(CLUTTER,
  clutter-$CLUTTER_MAJORMINOR >= $CLUTTER_MINIMUM_VERSION)

PKG_CHECK_MODULES(CLUTTER_GLX,
  clutter-glx-$CLUTTER_MAJORMINOR,
  HAVE_CLUTTER_GLX=true,
  HAVE_CLUTTER_GLX=false)
AM_CONDITIONAL(HAVE_CLUTTER_GLX, $HAVE_CLUTTER_GLX)

AC_CONFIG_FILES(dev-environ, [chmod +x ./dev-environ])
AC_CONFIG_FILES(Makefile
	clutter/Makefile
	clutter/gnome/gw/Makefile
	clutter/gnome/Makefile
	clutter/gnome/overrides/Makefile
	clutter/gnome/defs/Makefile
	meta/guile-clutter-1.0.pc
	doc/Makefile
	doc/clutter-glx/Makefile
	doc/clutter/Makefile
	tests/Makefile)
AC_OUTPUT
