dnl autoconf configuration file for guile-gstreamer
AC_INIT

dnl when going to/from release please set the nano (fourth number) right !
AS_VERSION(guile-gstreamer, GUILE_GSTREAMER_VERSION, 0, 0, 9, 2, 
           WARN_CFLAGS="-Wall",
           WARN_CFLAGS="-Wall -Werror")
PACKAGE=guile-gstreamer
AM_INIT_AUTOMAKE($PACKAGE,$VERSION)
AC_SUBST(WARN_CFLAGS)

AM_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_CONFIG_SRCDIR(gnome/gstreamer.scm)
AM_CONFIG_HEADER(config.h)

ACLOCAL="$ACLOCAL -I common/m4 $ACLOCAL_FLAGS"

AC_PROG_CC
AC_STDC_HEADERS

dnl modify pkg-config path
AC_ARG_WITH(pkg-config-path, 
   AC_HELP_STRING([--with-pkg-config-path],[colon-separated list of pkg-config(1) dirs]),
   [export PKG_CONFIG_PATH=${withval}])

dnl Now we're ready to ask for gstreamer libs and cflags
dnl And we can also ask for the right version of gstreamer
HAVE_GSTREAMER=no

dnl start with 0.7
GST_MAJORMINOR=0.7
PKG_CHECK_MODULES(GST, gstreamer-$GST_MAJORMINOR,
	HAVE_GSTREAMER=yes,HAVE_GSTREAMER=no)

dnl try 0.6
if test "x$HAVE_GSTREAMER" = "xno"; then
	GST_MAJORMINOR=0.6
	PKG_CHECK_MODULES(GST, gstreamer-$GST_MAJORMINOR,
		HAVE_GSTREAMER=yes,HAVE_GSTREAMER=no)
fi

dnl Give error and exit if we don't have gstreamer
if test "x$HAVE_GSTREAMER" = "xno"; then
	AC_MSG_ERROR(you need gstreamer development packages installed!)
fi

AC_SUBST(GST_LIBS)
AC_SUBST(GST_CFLAGS)
AC_SUBST(GST_MAJORMINOR)

#
# Check for g-wrap -- hopefully this will become a pkg-config check in the
# near future
#
AM_PATH_GWRAP(1.3.4, ,[AC_MSG_ERROR([

  g-wrap does not appear to be installed correctly, or is not new
  enough. We need version 1.3.4.

  If you need to install g-wrap, you can find it at
  http://www.gnucash.org/pub/g-wrap.
])])

AC_SUBST(G_WRAP_CFLAGS, `${G_WRAP_CONFIG} --c-compile-args guile`)
AC_SUBST(G_WRAP_LIBS, `${G_WRAP_CONFIG} --c-link-args guile`)
AC_SUBST(G_WRAP_MODULE_DIR, `${G_WRAP_CONFIG} --guile-module-directory`)
AC_SUBST(G_WRAP_LIB_DIR, `echo $G_WRAP_MODULE_DIR | sed -e 's|share/guile|lib|'`)

PKG_CHECK_MODULES(GUILE_GOBJECT, guile-gobject >= 0.5.3,, exit)
AC_SUBST(GUILE_GOBJECT_LIBS)
AC_SUBST(GUILE_GOBJECT_CFLAGS)

dnl check for GTK
PKG_CHECK_MODULES(GTK, gtk+-2.0 >= 2.2.0 gdk-2.0 >= 2.2.0, HAVE_GTK=true, HAVE_GTK=false)
AC_SUBST(HAVE_GTK)
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

AC_CHECK_PROG(HAVE_XMLTO, xmlto, true, false)
AC_CHECK_PROG(HAVE_XMLCATALOG, xmlcatalog, true, false)

AC_ARG_ENABLE(docs,
AC_HELP_STRING([--enable-docs],[enable building of documentation]),
[case "${enableval}" in
  yes)
    if test "x$HAVE_XMLTO" = "xtrue" ; then
      BUILD_DOCS=yes
    else
      AC_MSG_ERROR([you don't have xmlto, so don't use --enable-docs])
      BUILD_DOCS=no
    fi
    if test "x$HAVE_XMLCATALOG" = "xtrue" ; then
      BUILD_DOCS=yes
    else
      AC_MSG_ERROR([you don't have xmlcatalog, so don't use --enable-docs])
      BUILD_DOCS=no
    fi ;;
  no)  BUILD_DOCS=no ;;
  *) AC_MSG_ERROR(bad value ${enableval} for --enable-docs) ;;
esac],
[BUILD_DOCS=yes]) dnl Default value

AM_CONDITIONAL(BUILD_DOCS,          test "x$BUILD_DOCS" = "xyes")

AC_OUTPUT([
  Makefile 
  gnome/Makefile
  gnome/defs/Makefile
  gnome/gstreamer/config.scm
  gnome/gstreamer/Makefile
  examples/Makefile
  docs/Makefile
  docs/gst-guile.ent
  guile-gstreamer.pc
  guile-gstreamer-uninstalled.pc
])
