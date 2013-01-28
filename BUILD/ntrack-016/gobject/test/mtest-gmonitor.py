#!/usr/bin/python
#
#  Copyright (C) 2009,2010  Alexander Sack <asac@jwsdot.com>
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
import glib
import gobject
import pyntrack
import sys

monitor=None

def monitor_event(object, event):
	print "event:", event
	print "state:", pyntrack.ntrack_glib_get_state ()

# check that we get an exception if run with wrong arguments
ok=False
try:
	pyntrack.ntrack_init ()
except Exception as e:
	ok=True
if not ok:
	print "no exception with wrong argument ntrack_init"
	sys.exit(1)

# check that we get an exception if run with wrong arguments
ok=False
try:
	pyntrack.ntrack_init ("String")
except pyntrack.NtrackException as e:
	ok=True
if not ok:
	print "no exception with string argument ntrack_init"
	sys.exit(1)

# init properly - should work
pyntrack.ntrack_init (sys.argv)

# get a monitor
monitor = pyntrack.ntrack_g_monitor_get()
# register ntrack-event
monitor.connect("ntrack-event", monitor_event)
# get initial state
print "initial state: ", pyntrack.ntrack_glib_get_state ()

# lets go and track from here onwards
glib.MainLoop().run()

