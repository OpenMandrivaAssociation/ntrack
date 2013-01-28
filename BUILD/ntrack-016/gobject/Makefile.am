#
#  Copyright (C) 2009,2010  Alexander Sack <asac@jwsdot.com>
#
#  This file is part of:
#      ntrack - Network Status Tracking for Desktop Applications
#               http://launchpad.net/ntrack
#
#   ntrack is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Lesser General Public License as
#   published by the Free Software Foundation, either version 3 of
#   the License, or (at your option) any later version.
#
#   ntrack is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General Public
#   License along with ntrack.  If not, see <http://www.gnu.org/licenses/>.
#

SUBDIRS = test
CLEANFILES =
NTRACK_GOBJECT_LT_VERSION = 1:0:0

AM_CFLAGS = \
	-Wall \
	-I$(srcdir)/../common/ \
	-I$(srcdir)/../glib/


AM_LDFLAGS =

lib_LTLIBRARIES = libntrack-gobject.la

gobjectpkgincludedir = $(pkgincludedir)/gobject

libntrack_gobject_la_SOURCES = \
	ntrack-gmonitor.c ntrack-gmonitor.h \
	ntrack-enumtypes.c ntrack-enumtypes.h \
	ntrack-gmarshal.h

libntrack_gobject_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(GOBJECT_CFLAGS) \
	-Wno-error

libntrack_gobject_la_LIBADD = \
	$(GOBJECT_LIBS) \
	../glib/libntrack-glib.la

libntrack_gobject_la_LDFLAGS = \
	$(AM_LDFLAGS) \
	-export-symbols $(srcdir)/ntrack-gobject.symbols \
	-no-undefined \
	-version-info $(NTRACK_GOBJECT_LT_VERSION)

EXTRA_DIST = \
	libntrack-gobject.pc.in \
	ntrack-enumtypes.c.template ntrack-enumtypes.h.template \
	ntrack-gobject.symbols \
	ntrack-gmarshal.list

gobjectpkginclude_HEADERS = \
	ntrack-enumtypes.h \
	ntrack-gobject.h \
	ntrack-gmonitor.h \
	ntrack-gmarshal.h

BUILT_SOURCES = \
	ntrack-enumtypes.c \
	ntrack-enumtypes.h \
	ntrack-gmarshal.h

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = \
	libntrack-gobject.pc

ntrack-gmarshal.h: ntrack-gmarshal.list
	echo "/*** AUTO GENERATED - your changes will be overriden. check $@: rule in Makefile.am ***/" > $@
	echo "#pragma GCC diagnostic warning \"-pedantic\"" >> $@
	echo >> $@
	glib-genmarshal --header --body --prefix=_ntrack_cclosure_marshal $? >> $@

CLEANFILES += ntrack-gmarshal.h


ntrack-enumtypes.h: $(srcdir)/../glib/ntrack-enums.h ntrack-enumtypes.h.template
	( top_builddir=`cd $(top_builddir) && pwd`; \
	  cd $(srcdir) && glib-mkenums --template ntrack-enumtypes.h.template -- ../glib/ntrack-enums.h) | sed -e 's/@ENUMPREFIX@/N/' > \
	      ntrack-enumtypes.h.tmp && mv ntrack-enumtypes.h.tmp ntrack-enumtypes.h

CLEANFILES += ntrack-enumtypes.h


ntrack-enumtypes.c: $(srcdir)/../glib/ntrack-enums.h ntrack-enumtypes.c.template
	( top_builddir=`cd $(top_builddir) && pwd`; \
	  cd $(srcdir) && glib-mkenums --template ntrack-enumtypes.c.template -- ../glib/ntrack-enums.h) | sed -e 's/@ENUMPREFIX@/N/' > \
	      ntrack-enumtypes.c.tmp && mv ntrack-enumtypes.c.tmp ntrack-enumtypes.c

CLEANFILES += ntrack-enumtypes.c

# python bindings
if HAVE_PYGOBJECT

pyexec_LTLIBRARIES = pyntrack.la

pyntrack_la_SOURCES = \
	pyntrack-module.c \
	pyntrack-gmonitor.c \
	ntrack-enumtypes.h

pyntrack_la_CFLAGS = \
	$(AM_CFLAGS) \
	$(PYGOBJECT_CFLAGS) \
	$(PYTHON_INCLUDES)

pyntrack_la_LIBADD = \
	libntrack-gobject.la \
	$(GOBJECT_LIBS) \
	$(PYGOBJECT_LIBS)

pyntrack_la_LDFLAGS = \
	-avoid-version \
	-module

pyntrack-gmonitor.c: pyntrack-gmonitor.defs $(srcdir)/pyntrack-gmonitor.override
	$(PYG_CODEGEN) \
		--prefix pyntrack \
		--override $(srcdir)/pyntrack-gmonitor.override \
		pyntrack-gmonitor.defs > $@

CLEANFILES += pyntrack-gmonitor.c

pyntrack-gmonitor.defs: $(wildcard $(srcdir)/*.h) $(wildcard $(srcdir)/../common/*.h)
	$(H2DEF) -m NTrack $(srcdir)/*.h $(srcdir)/../glib/*.h $(srcdir)/../common/*.h > $@

CLEANFILES += pyntrack-gmonitor.defs

EXTRA_DIST += \
	pyntrack-gmonitor.defs \
	pyntrack-gmonitor.override \
	generic-py-wrapper
endif

