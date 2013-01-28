/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* 
 * Copyright (C) 2010  Alexander Sack <asac@jwsdot.com>
 *
 * This file is part of:
 *     ntrack - Network Status Tracking for Desktop Applications
 *              http://launchpad.net/ntrack
 *
 *  ntrack is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as
 *  published by the Free Software Foundation, either version 3 of
 *  the License, or (at your option) any later version.
 *
 *  ntrack is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with ntrack.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <ntrack.h>
#include <ntrack-gmonitor.h>

int main (int argc, char **argv)
{
	union {
		gpointer ptr;
		NTrackGMonitor *obj;
	} monitor;

	ntrack_init (&argc, &argv);

	g_type_init ();

	monitor.obj = ntrack_g_monitor_get ();
	g_object_add_weak_pointer (G_OBJECT (monitor.obj), &monitor.ptr);

	g_object_unref (monitor.obj);

	if (monitor.ptr != 0) {
		g_warning ("unreffing monitor does not reset weak pointer");
		return 1;
	}

	monitor.obj = ntrack_g_monitor_get ();
	g_object_add_weak_pointer (G_OBJECT (monitor.obj), &monitor.ptr);
	g_object_ref (monitor.obj);

	g_object_unref (monitor.obj);

	if (monitor.ptr == 0) {
		g_warning ("unreffing double reffed monitor did reset weak pointer");
		return 1;
	}

	g_object_unref (monitor.obj);

	if (monitor.ptr != 0) {
		g_warning ("unreffing monitor does not reset weak pointer");
		return 1;
	}

	return 0;
}
