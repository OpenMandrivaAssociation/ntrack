dnl
dnl  Copyright (C) 2009-2011  Alexander Sack <asac@jwsdot.com>
dnl
dnl  This file is part of:
dnl      ntrack - Network Status Tracking for Desktop Applications
dnl               http://launchpad.net/ntrack
dnl
dnl   ntrack is free software: you can redistribute it and/or modify
dnl   it under the terms of the GNU Lesser General Public License as
dnl   published by the Free Software Foundation, either version 3 of
dnl   the License, or (at your option) any later version.
dnl
dnl   ntrack is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU Lesser General Public License for more details.
dnl
dnl   You should have received a copy of the GNU Lesser General Public
dnl   License along with ntrack.  If not, see <http://www.gnu.org/licenses/>.
dnl

dnl Process this file with autoconf to produce a configure script.

AC_INIT(common/ntrack.c)

AC_CONFIG_MACRO_DIR(m4)

dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(ntrack, 016)

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AC_CONFIG_HEADERS([config.h])

dnl enable maintainer mode by default
AM_MAINTAINER_MODE([enable])

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_CXX

dnl enable libtool support
AC_PROG_LIBTOOL

dnl pkg-config support
PKG_PROG_PKG_CONFIG

dnl check for python
AM_PATH_PYTHON(2.3.5)

PYTHON_INCLUDES="-isystem/usr/include/python$PYTHON_VERSION"
AC_SUBST(PYTHON_INCLUDES)

AC_DEFINE([AUTHOR], ["Alexander Sack <asac@jwsdot.com>"], [author name for build])
AC_DEFINE([COPYRIGHT], ["Copyright (C) 2009-2011"], [copyright info for build])
AC_DEFINE([LICENSE], ["GPLv3+ or LGPLv3+"], [license info for build])

dnl check for developer build
developer_build=no
AC_ARG_ENABLE([developer-build],
              [AS_HELP_STRING([--enable-developer-build], [enable developer build which tries to find modules in various source directories])],
              [AC_DEFINE(
                 [DEVELOPER_BUILD],
                 [1],
                 [enable developer build. this allows you to run and test binaries properly without make install])])

AC_ARG_ENABLE([backend],
            [AS_HELP_STRING([--enable-backend=@<:@ARGS@:>@],
              [select one or more backends from: libnl and rtnetlink @<:@default=libnl@:>@])],
            ,
            [enable_backend=libnl])

AS_IF([test x$enable_backend = xyes],
      [enable_backend=libnl])

enable_backend=`echo $enable_backend | tr ',' ' '`

for backend in $enable_backend; do
	case $backend in
	libnl)
		enable_module_libnl=yes
                ;;
	rtnetlink)
		enable_module_rtnetlink=yes
                ;;
	*)
		AC_MSG_ERROR([unknown backend: $backend])
                ;;
	esac
done

AS_IF([test x$enable_module_libnl = xyes],
              [AC_DEFINE(
                 [MODULE_LIBNL],
                 [1],
                 [enable module libnl])])
AS_IF([test x$enable_module_rtnetlink = xyes],
              [AC_DEFINE(
                 [MODULE_RTNETLINK],
                 [1],
                 [enable module rtnetlink])])

dnl Checks for libraries.

AC_ARG_WITH([glib2],
            [AS_HELP_STRING([--with-glib2],
              [support glib2 binding @<:@default=check@:>@])],
            [],
            [with_glib2=check])

AC_ARG_WITH([gobject],
            [AS_HELP_STRING([--with-gobject],
              [support gobject binding @<:@default=check@:>@])],
            [],
            [with_gobject=check])

AC_ARG_WITH([pygobject],
            [AS_HELP_STRING([--with-pygobject],
              [support gobject binding @<:@default=check@:>@])],
            [],
            [with_pygobject=check])

AC_ARG_WITH([qt4],
            [AS_HELP_STRING([--with-qt4],
              [support qt4 binding @<:@default=check@:>@])],
            [],
            [with_qt4=check])

##
# parameter consistency check

AS_IF([test "x$with_gobject" = xyes -a "x$with_glib2" = "xno"],
      [AC_MSG_ERROR([--with-gobject is impossible --without-glib2])])

AS_IF([test "x$with_pygobject" = xyes -a "x$with_glib2" = "xno"],
      [AC_MSG_ERROR([--with-pygobject is impossible --without-glib2])])

AS_IF([test "x$with_pygobject" = xyes -a "x$with_gobject" = "xno"],
      [AC_MSG_ERROR([--with-pygobject is impossible --without-gobject])])

##
# gather c and ld flags

ac_have_glib2=check
AS_IF([test "x$with_glib2" != xno],
        [PKG_CHECK_MODULES(GLIB, glib-2.0, ac_have_glib2=1, ac_have_glib2=0)
         AC_SUBST(GLIB_CFLAGS)
         AC_SUBST(GLIB_LIBS)])
AM_CONDITIONAL(HAVE_GLIB2, test $ac_have_glib2 = 1)

ac_have_gobject=check
if test $ac_have_glib2 = 1; then
  AS_IF([test "x$with_gobject" != xno],
          [PKG_CHECK_MODULES(GOBJECT, gobject-2.0, ac_have_gobject=1, ac_have_gobject=0)
           AC_SUBST(GOBJECT_CFLAGS)
           AC_SUBST(GOBJECT_LIBS)])
fi
AM_CONDITIONAL(HAVE_GOBJECT, test $ac_have_gobject = 1)

ac_have_pygobject=check
if test $ac_have_gobject = 1; then
  AS_IF([test "x$with_pygobject" != xno],
          [PKG_CHECK_MODULES(PYGOBJECT, pygobject-2.0, ac_have_pygobject=1, [PKG_CHECK_MODULES(PYGOBJECT, pygtk-2.0, ac_have_pygobject=1, ac_have_pygobject=0)])
           AC_SUBST(PYGOBJECT_CFLAGS)
           AC_SUBST(PYGOBJECT_LIBS)])
fi

if test $ac_have_pygobject = 1; then
  if test -f /usr/share/pygobject/2.0/codegen/h2def.pyc; then
    H2DEF="python /usr/share/pygobject/2.0/codegen/h2def.pyc"
    PYG_CODEGEN=pygobject-codegen-2.0
  elif test -f /usr/share/pygobject/2.0/codegen/h2def.py; then
    H2DEF="python /usr/share/pygobject/2.0/codegen/h2def.py"
    PYG_CODEGEN=pygobject-codegen-2.0
  elif test -f /usr/share/pygtk/2.0/codegen/h2def.pyc; then
    H2DEF="python /usr/share/pygtk/2.0/codegen/h2def.pyc"
    PYG_CODEGEN=pygtk-codegen-2.0
  elif test -f /usr/share/pygtk/2.0/codegen/h2def.py; then
    H2DEF="python /usr/share/pygtk/2.0/codegen/h2def.py"
    PYG_CODEGEN=pygtk-codegen-2.0
  else
    AC_ERROR(no h2def found)
  fi
fi

AC_SUBST(H2DEF)
AC_SUBST(PYG_CODEGEN)

AM_CONDITIONAL(HAVE_PYGOBJECT, test $ac_have_pygobject = 1)

ac_have_qt4=check
AS_IF([test "x$with_qt4" != xno],
        [PKG_CHECK_MODULES(QTCORE, QtCore, ac_have_qt4=1, ac_have_qt4=0)
         AC_SUBST(QTCORE_CFLAGS)
         AC_SUBST(QTCORE_LIBS)
         AC_CHECK_PROGS(MOC_QT4_BIN, moc-qt4 moc, no)
         AS_IF([test "x$ac_have_qt4" = x1 -a "x$MOC_QT4_BIN" = xno],
               AC_MSG_ERROR([use --without-qt4 or make moc-qt4 or moc command available in PATH]))])

AM_CONDITIONAL(HAVE_QT4, test $ac_have_qt4 = 1)

AS_IF([test x$enable_module_libnl = xyes ],
[
  # lets hope libnl doesnt break anything so LIBNL3_X can be for all libnl 3.x
  # versions > 3.1 that have libnl-route-3.0 pc file and we finally get out of this
  # libnl madness
  PKG_CHECK_MODULES(LIBNL3_X, libnl-route-3.0, ac_have_libnl3_X=1, ac_have_libnl3_X=0)
  AC_SUBST(LIBNL3_X_CFLAGS)
  AC_SUBST(LIBNL3_X_LIBS)

  PKG_CHECK_MODULES(LIBNL3_1, libnl-route-3.1, ac_have_libnl3_1=1, ac_have_libnl3_1=0)
  AC_SUBST(LIBNL3_1_CFLAGS)
  AC_SUBST(LIBNL3_1_LIBS)

  PKG_CHECK_MODULES(LIBNL3, libnl-3.0 = 3.0, ac_have_libnl3=1, ac_have_libnl3=0)
  AS_IF([test x$ac_have_libnl3 = x1],
	[LIBNL3_LIBS="$LIBNL3_LIBS -lnl-route"])
  AC_SUBST(LIBNL3_CFLAGS)
  AC_SUBST(LIBNL3_LIBS)

  PKG_CHECK_MODULES(LIBNL2, libnl-2.0, ac_have_libnl2=1, ac_have_libnl2=0)
  AC_SUBST(LIBNL2_CFLAGS)
  AC_SUBST(LIBNL2_LIBS)

  PKG_CHECK_MODULES(LIBNL1, libnl-1, ac_have_libnl1=1, ac_have_libnl1=0)
  AC_SUBST(LIBNL1_CFLAGS)
  AC_SUBST(LIBNL1_LIBS)
])

AM_CONDITIONAL(HAVE_LIBNL1, test x$ac_have_libnl1 = x1)
AM_CONDITIONAL(HAVE_LIBNL2, test x$ac_have_libnl2 = x1)
AM_CONDITIONAL(HAVE_LIBNL3, test x$ac_have_libnl3 = x1)
AM_CONDITIONAL(HAVE_LIBNL3_1, test x$ac_have_libnl3_1 = x1)
AM_CONDITIONAL(HAVE_LIBNL3_X, test x$ac_have_libnl3_X = x1)

ac_have_rtnetlink=0
AS_IF([test x$enable_module_rtnetlink = xyes ],
  [AC_CHECK_HEADER(linux/netlink.h, ac_have_rtnetlink=1, ac_have_rtnetlink=0,
                   [#include <sys/socket.h>]
                  )
  ]
)
AM_CONDITIONAL(HAVE_MODULE_RTNETLINK, test $ac_have_rtnetlink = 1)

# consistency checks
if ! test "$ac_have_libnl1" = "1" -o "$ac_have_libnl2" = 1 -o "$ac_have_libnl3" = 1 -o "$ac_have_libnl3_1" = 1 -o "$ac_have_libnl3_X" = 1 -o "$ac_have_rtnetlink" = 1; then
	AC_ERROR([neither rtnetlink nor libnl backend are enabled])
fi

AS_IF([test "$ac_have_libnl3_1" = "1" -o "$ac_have_libnl3_X" = 1 ],
      [AC_MSG_WARN([
------------
	libnl-3.1 (and later) official release keeps a dangling reference when
        recycling a cache manager.

        This will cause ntrack unit tests to fail. If you see segfaults in any
        recycle test when running make check, ensure that you have the following
        patches applied to your libnl:
	 - ntrack/3rd-party/libnl-3.1/0001-avoid-dangling-co_major_cache-reference-to-NL_AUTO_P.patch
------------
])])


AS_IF([test x$with_glib2 = xyes -a $ac_have_glib2 != 1],
  [AC_MSG_ERROR([with-glib2 set, but build dependencies not fulfilled, you need glib-2.0 .pc file in PKG_CONFIG_PATH])]
)

AC_OUTPUT(
 Makefile
 common/Makefile common/test/Makefile
 modules/Makefile
 glib/Makefile glib/test/Makefile
 gobject/Makefile gobject/test/Makefile
 qt4/Makefile qt4/test/Makefile
 common/libntrack.pc
 glib/libntrack-glib.pc
 gobject/libntrack-gobject.pc
 qt4/libntrack-qt4.pc
)

