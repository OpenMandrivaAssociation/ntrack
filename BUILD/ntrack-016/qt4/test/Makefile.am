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

test_libs = \
	$(QTCORE_LIBS) \
	../libntrack-qt4.la  \
	../../common/libntrack.la

AM_CXXFLAGS = \
	-Wall \
	-I$(srcdir)/.. \
	-I$(srcdir)/../../common \
	$(QTCORE_CFLAGS)

check_PROGRAMS = \
	mtest-qmonitor

mtest_qmonitor_SOURCES = mtest-qmonitor.cpp
mtest_qmonitor_LDADD = $(test_libs)

TESTS =

