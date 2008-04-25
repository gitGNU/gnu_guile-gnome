# guile-gnome
# Copyright (C) 2001,2003,2004 Free Software Foundation, Inc.

# This program is free software; you can redistribute it and/or    
# modify it under the terms of the GNU General Public License as   
# published by the Free Software Foundation; either version 2 of   
# the License, or (at your option) any later version.              
                                                                 
# This program is distributed in the hope that it will be useful,  
# but WITHOUT ANY WARRANTY; without even the implied warranty of   
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the    
# GNU General Public License for more details.                     
                                                                 
# You should have received a copy of the GNU General Public License
# along with this program; if not, contact:

# Free Software Foundation           Voice:  +1-617-542-5942
# 59 Temple Place - Suite 330        Fax:    +1-617-542-2652
# Boston, MA  02111-1307,  USA       gnu@gnu.org

pkgs_ordered=""
checks_catted=""

# Defines the release manager. Packages from someone else will carry a
# date and releaser stamp.
release_manager="Andy Wingo"

sort_pkgs()
{
  local packages="$@"
  ( 
    for pkg in $packages; do
      in_graph=0
      for check in $pkg/*-checks.ac; do
	if [ -f $check ]; then
	  dep_pkg=`echo $check | sed -e "s,^$pkg/\(.*\)-checks\.ac$,\1,"`
	  if [ -d $dep_pkg ]; then
            in_graph=1
	    echo "$pkg $dep_pkg"
	  else
            echo "$checks_catted" | grep -q $dep_pkg;
	    if test $? -eq 1; then
	      cat $check >> configure.ac
	      checks_catted="$checks_catted $dep_pkg"
	    fi
	  fi
        fi
      done
      if test $in_graph -eq 0; then
        echo "$pkg null"
      fi
    done 
  ) | tsort | tac | grep -v '^null$' | tr '\n' ' '
}

get_version()
{
    local packages="$@"
    version_pkgs=""

    for i in $packages; do
        if test -f $i/VERSION; then
            if test -z "$version_pkgs"; then
                version_pkgs=$i
            else
                version_pkgs="$i $version_pkgs"
            fi
        fi
    done

    case `echo "$version_pkgs" | wc -w` in
        0)  
	    version=`date +%Y%m%d`
            echo "+ warning: no subpackage has a VERSION"
            ;;
        1)  
            version=`head -n 1 $version_pkgs/VERSION`
            echo "+ using VERSION $version from $version_pkgs"
            ;;
        *)  
	    version=`date +%Y%m%d`
            echo "+ warning: multiple subpackages with VERSION: $version_pkgs"
            ;;
    esac
}

determine_package()
{
    local packages="$@"

    while read line; do
        if echo $line | egrep -q '^(#.*|[[:space:]]*)$'; then
            continue
        elif echo $line | egrep -qv '^[[:space:]]*[[:alnum:]_-]+:.*$'; then
            echo "Warning: bad PACKAGES statement: $line">&2
            continue
        fi
        package=`echo $line|sed -re 's/^[[:space:]]*([[:alnum:]_-]+):.*$/\\1/'`
        subpackages=`echo $line|sed -re 's/^[^:]*:(.*)$/\\1/'`
        subpackages=`for p in $subpackages; do echo $p; done | sort | xargs echo`
        if test "$packages" = "$subpackages"; then
            return
        fi
    done < PACKAGES
    echo "Warning: package set '$packages' does not form a source package; configuring as guile-gnome-custom"
    package=guile-gnome-custom
}
    
autogen_pkg()
{
    # Check and parse release ID
    # Figure out list of packages
    packages=""
    pkgs_path="."
    for dir in [a-z]*; do
	if [ -f $dir/package.ac ]; then
	    packages="$packages $dir"
	fi
    done
    if test -z "$packages"; then
        echo "No sub-packages present in source tree."
        echo
        echo "You probably just pulled this branch from bzr. Guile-GNOME can"
        echo "be configured to build many different packages, as listed in"
        echo "PACKAGES. To pull those packages from the net, try the scripts"
        echo "in ./scripts/."
        echo
        echo "For example, to check out the source packages for "
        echo "guile-gnome-platform, try:"
        echo
        echo "  ./scripts/get-packages guile-gnome-platform"
        echo
        exit 1
    fi
    packages=`for p in $packages; do echo $p; done | sort | xargs echo`
    determine_package $packages

    # versioning
    get_version $packages
    if bzr whoami | grep -qv "$release_manager"; then
        version="$version+unofficial";
    fi
    
    echo "+ configuring tree as $package, version $version"

    # prelude of configure.ac
    cat > configure.ac <<EOF
dnl -- WARNING: generated by $0, changes will be lost
AC_PREREQ(2.52)
AC_INIT($package, $version)
AC_CONFIG_SRCDIR(autogen-pkg.sh)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(-Wno-portability)

AC_SUBST(VERSION,$version)

# Meaning of the API version
# --------------------------
#
# If 0, guile-gnome is unstable, and the API might change anytime.
# Otherwise, guile-gnome is stable. Future incompatible releases will
# bump this number so as to allow parallel, incompatible versions to
# coexist.
API_VERSION=2
AC_SUBST(API_VERSION)

AM_MAINTAINER_MODE
AC_DISABLE_STATIC

AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

AC_SUBST(AG_PKG_CONFIG_PATH, [\$pkg_config_path])
AG_PACKAGES="$packages"
AC_SUBST(AG_PACKAGES)

if test "x\$prefix" = xNONE; then
   AC_MSG_ERROR([No explicit prefix given.

Guile-GNOME requires you to explicitly enter in a prefix when
configuring. This is because the default prefix, /usr/local, is not in
the default guile load path. What you probably want to do is to run

  \$0 --prefix=/usr

so that guile can find the guile-gnome modules when they are installed.

If you choose to install to a prefix that is not in guile's load path,
you will need to modify GUILE_LOAD_PATH and LD_LIBRARY_PATH every time
you run guile. (Which is a pain.)])
fi

MK=""; AC_SUBST(MK)
EOF

    cat >> configure.ac <<'EOF'
WARN_CFLAGS=-Wall
AC_ARG_ENABLE([Werror], AC_HELP_STRING([--disable-Werror],[Don't stop the build on errors]),
        [], WARN_CFLAGS="-Wall -Werror")
AC_SUBST(WARN_CFLAGS)

DEBUG_CFLAGS=
AC_ARG_ENABLE([debug], AC_HELP_STRING([--disable-debug],[Disable debugging information]),
        [], DEBUG_CFLAGS=-g)
AC_SUBST(DEBUG_CFLAGS)

DISABLE_DEPRECATED=false
AC_ARG_ENABLE([deprecated],
AC_HELP_STRING([--disable-deprecated],[Disable wrapping of functions deprecated upstream]),
        DISABLE_DEPRECATED=true, DISABLE_DEPRECATED=false)

# Optimize the DSO symbol hash table -- see ulrich drepper's paper,
# "how to write shared libraries"
GNULD_LDFLAGS=-Wl,-O1
AC_SUBST(GNULD_LDFLAGS)
AM_LDFLAGS='$(GNULD_LDFLAGS)'
AC_SUBST(AM_LDFLAGS)

#
# Check for Guile
#
AC_MSG_CHECKING(for Guile)
guile-config link > /dev/null || {
   echo "configure: cannot find guile-config; is Guile installed?" 1>&2
   exit 1
}
GUILE_VERSION=`guile-config info guileversion`
if test "$GUILE_VERSION" \< 1.8.0; then
   AC_MSG_ERROR([Guile 1.8.0 or newer is required, but you only have $GUILEVERSION.])
fi
GUILE_CFLAGS="`guile-config compile`"
GUILE_LIBS="`guile-config link`"
AC_SUBST(GUILE_CFLAGS)
AC_SUBST(GUILE_LIBS)
AC_MSG_RESULT(yes)

# Check for g-wrap

PKG_CHECK_MODULES(G_WRAP, g-wrap-2.0-guile >= 1.9.8)
AC_SUBST(G_WRAP_CFLAGS)
AC_SUBST(G_WRAP_LIBS)
AC_SUBST(G_WRAP_MODULE_DIR, `${PKG_CONFIG} --variable=module_directory g-wrap-2.0-guile`)
AC_SUBST(G_WRAP_LIB_DIR, `echo $G_WRAP_MODULE_DIR | sed -e 's|share/guile|lib|'`)

PACKAGES_TO_BUILD=""
PACKAGES_NOT_BUILT=""
record_check()
{
    local package=$1;
    local buildp=$2;
    case $buildp in
        yes|true)
            if test -z "$PACKAGES_TO_BUILD"; then
                PACKAGES_TO_BUILD="$package"
            else
                PACKAGES_TO_BUILD="$PACKAGES_TO_BUILD $package"
            fi
            ;;
        no|false)
            if test -z "$PACKAGES_NOT_BUILT"; then
                PACKAGES_NOT_BUILT="$package"
            else
                PACKAGES_NOT_BUILT="$PACKAGES_NOT_BUILT $package"
            fi
            ;;
        *)
            echo "bad record_check value: \"$buildp\" (for package \"$package\")"
            exit 1
            ;;
    esac
}

report_checks()
{
    if test -z "$PACKAGES_TO_BUILD"; then
        AC_MSG_ERROR([All wrappers failed their prerequisites.

These following wrappers cannot be built:
  $PACKAGES_NOT_BUILT

Check the README files in the above subdirectories, install the necessary
packages, and try again.
])
    elif test -z "$PACKAGES_NOT_BUILT"; then
        AC_MSG_NOTICE([
All available wrappers will be built:
  $PACKAGES_TO_BUILD

])
    else
        AC_MSG_NOTICE([Some packages will not be built.

These wrappers will be built:
  $PACKAGES_TO_BUILD

These wrappers failed their prerequisites and will NOT be built:
  $PACKAGES_NOT_BUILT

Check the README files in the above subdirectories for more information
on the prerequisites of a package.
])
    fi
}

# Per-package checks follow
EOF

    # package checks
    for pkg in $packages; do
      echo "# $pkg" >> configure.ac
      cat $pkg/package.ac >> configure.ac
    done
    
    echo "+ sorting package dependencies..."
    pkgs_ordered=`sort_pkgs $packages`
    echo "  $pkgs_ordered"
    
    # postlude
    (
        echo report_checks
	echo
	echo "AC_CONFIG_FILES(dev-environ, [chmod +x ./dev-environ])"
	echo "AC_CONFIG_FILES("
	echo "Makefile"
	find $packages -name Makefile.am | egrep -v '{arch}|\+\+|,,' | sed -e 's/\.am$//'
	for pkg in $packages; do
	  if [ -f $pkg/files.ac ]; then
	    while read file; do
	      echo "$pkg/$file"
	    done < $pkg/files.ac
	  fi
	done
	echo ")"
	echo "AC_OUTPUT" 
    ) >> configure.ac

    cat > Makefile.am <<EOF
# this file is generated by autogen-pkg.sh

SUBDIRS = $pkgs_ordered
EOF
    cat Makefile.am.bottom >> Makefile.am
}
