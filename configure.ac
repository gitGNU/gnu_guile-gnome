define(GUILE_CLUTTER_CONFIGURE_COPYRIGHT,[[

Copyright (C) 2011,2012,2014  Free Software Foundation, Inc.

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

AC_PREREQ(2.52)
AC_INIT(guile-clutter, 1.12.2)
AC_CONFIG_AUX_DIR([build-aux])
AC_COPYRIGHT(GUILE_CLUTTER_CONFIGURE_COPYRIGHT)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(-Wno-portability -Wno-syntax)

AC_SUBST(VERSION,1.12.2)

# Meaning of the API version
# --------------------------
#
# If 0, guile-gnome is unstable, and the API might change anytime.
# Otherwise, guile-gnome is stable. Future incompatible releases will
# bump this number so as to allow parallel, incompatible versions to
# coexist.
API_VERSION=2
AC_SUBST(API_VERSION)

AC_DISABLE_STATIC

AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

AC_SUBST(AG_PKG_CONFIG_PATH, [$pkg_config_path])
AG_PACKAGES="clutter"
AC_SUBST(AG_PACKAGES)

if test "x$prefix" = xNONE; then
   AC_MSG_ERROR([No explicit prefix given.

Guile-GNOME requires you to explicitly enter in a prefix when
configuring. This is because the default prefix, /usr/local, is not in
the default guile load path. What you probably want to do is to run

  $0 --prefix=/usr

so that guile can find the guile-gnome modules when they are installed.

If you choose to install to a prefix that is not in guile's load path,
you will need to modify GUILE_LOAD_PATH and LD_LIBRARY_PATH every time
you run guile. (Which is a pain.)])
fi

MK=""; AC_SUBST(MK)
WARN_CFLAGS=-Wall
AC_ARG_ENABLE([Werror], AC_HELP_STRING([--enable-Werror],[Stop the build on warnings]),
        [WARN_CFLAGS="-Wall -Werror"], [])
AC_SUBST(WARN_CFLAGS)

DISABLE_DEPRECATED=false
AC_ARG_ENABLE([deprecated],
AC_HELP_STRING([--disable-deprecated],[Disable wrapping of functions deprecated upstream]),
        DISABLE_DEPRECATED=true, DISABLE_DEPRECATED=false)


#
# Check for Guile
#
GUILE_EFFECTIVE_VERSION=no
all_versions="2.2 2.0"
for v in $all_versions; do
  if test $GUILE_EFFECTIVE_VERSION = no; then
    AC_MSG_NOTICE([checking for Guile $v.x])
    PKG_CHECK_MODULES(GUILE, guile-$v, [GUILE_EFFECTIVE_VERSION=$v], [true])
  fi
done

AC_SUBST(GUILE_EFFECTIVE_VERSION)

if test $GUILE_EFFECTIVE_VERSION = no; then
  AC_MSG_ERROR([Failed to find Guile.  Is it installed?])
else
  AC_MSG_NOTICE([found Guile $GUILE_EFFECTIVE_VERSION.x])
fi

# Check for g-wrap

PKG_CHECK_MODULES(G_WRAP, g-wrap-2.0-guile >= 1.9.8)
AC_SUBST(G_WRAP_CFLAGS)
AC_SUBST(G_WRAP_LIBS)
AC_SUBST(G_WRAP_MODULE_DIR, `${PKG_CONFIG} --variable=module_directory g-wrap-2.0-guile`)
AC_SUBST(G_WRAP_LIB_DIR, `echo $G_WRAP_MODULE_DIR | sed -e 's|share/guile|lib|'`)

# Check for Guile-GNOME

PKG_CHECK_MODULES(GUILE_GLIB, guile-gnome-glib-2)
AC_SUBST(GUILE_GLIB_CFLAGS)
AC_SUBST(GUILE_GLIB_LIBS)

# Check for clutter

CLUTTER_MAJORMINOR=1.0
CLUTTER_MINIMUM_VERSION=1.12
AC_SUBST(CLUTTER_MAJORMINOR)

PKG_CHECK_MODULES(CLUTTER, clutter-$CLUTTER_MAJORMINOR > $CLUTTER_MINIMUM_VERSION guile-cairo)
AC_SUBST(CLUTTER_CFLAGS)
AC_SUBST(CLUTTER_LIBS)

PKG_CHECK_MODULES(CLUTTER_GLX, clutter-glx-$CLUTTER_MAJORMINOR,
                  HAVE_CLUTTER_GLX=true, HAVE_CLUTTER_GLX=false)
AC_SUBST(CLUTTER_GLX_CFLAGS)
AC_SUBST(CLUTTER_GLX_LIBS)
AM_CONDITIONAL(HAVE_CLUTTER_GLX, $HAVE_CLUTTER_GLX)

AC_CONFIG_FILES(dev-environ, [chmod +x ./dev-environ])
AC_CONFIG_FILES(
Makefile
clutter/Makefile
clutter/gnome/gw/Makefile
clutter/gnome/Makefile
clutter/gnome/overrides/Makefile
clutter/gnome/defs/Makefile
doc/Makefile
doc/clutter-glx/Makefile
doc/clutter/Makefile
tests/Makefile
)
AC_OUTPUT
