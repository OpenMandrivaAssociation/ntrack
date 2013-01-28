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

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = common modules

DISTCHECK_CONFIGURE_FLAGS = --enable-developer-build

if HAVE_GLIB2
SUBDIRS += glib
endif

if HAVE_GOBJECT
SUBDIRS += gobject
endif

if HAVE_QT4
SUBDIRS +=qt4
endif

EXTRA_DIST = autogen.sh \
	3rd-party/libnl-3.1/COPYING \
	3rd-party/libnl-3.1/COPYING.libnl \
	3rd-party/libnl-3.1/0001-avoid-dangling-co_major_cache-reference-to-NL_AUTO_P.patch

dist_doc_DATA = AUTHORS NEWS README

dump-bzr-changelog:
	@bzr log --gnu-changelog

