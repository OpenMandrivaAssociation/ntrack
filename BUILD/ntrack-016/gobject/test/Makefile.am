#
#  Copyright (C) 2009-2011  Alexander Sack <asac@jwsdot.com>
#
#  This file is part of:
#      ntrack - Network Status Tracking for Desktop Applications
#               http://launchpad.net/ntrack
#
#   ntrack is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   ntrack is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with ntrack.  If not, see <http://www.gnu.org/licenses/>.
#

EXTRA_DIST =

test_libs = \
	$(GOBJECT_LIBS) \
	../libntrack-gobject.la \
	../../glib/libntrack-glib.la \
	../../common/libntrack.la

AM_CFLAGS = \
	-Wall \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../common \
	-I$(srcdir)/../../glib \
	$(GOBJECT_CFLAGS)

if HAVE_PYGOBJECT
pyPROGS = mtest-gmonitor-py
endif

check_PROGRAMS = \
	test-gmonitor-recycle \
	mtest-gmonitor \
	$(pyPROGS)

mtest_gmonitor_py_SOURCES = \
	mtest-gmonitor.py

.libs/mtest-gmonitor.py:: $(srcdir)/mtest-gmonitor.py
	mkdir -p $(builddir).libs/
	cp -f $(srcdir)/mtest-gmonitor.py .libs/mtest-gmonitor.py

mtest-gmonitor-py:: $(top_srcdir)/gobject/generic-py-wrapper .libs/mtest-gmonitor.py
	cp $(top_srcdir)/gobject/generic-py-wrapper mtest-gmonitor-py

test_gmonitor_recycle_SOURCES = test-gmonitor-recycle.c
test_gmonitor_recycle_LDADD = $(test_libs)

mtest_gmonitor_SOURCES = mtest-gmonitor.c
mtest_gmonitor_LDADD = $(test_libs)

EXTRA_DIST += \
	mtest-gmonitor.py

TESTS = \
	test-gmonitor-recycle

