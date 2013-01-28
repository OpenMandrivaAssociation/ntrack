/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* 
 * Copyright (C) 2009,2010  Alexander Sack <asac@jwsdot.com>
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
#include <ntrackmonitor.h>

int main (int argc, char **argv)
{
	ntrack_monitor_t *monitor = 0;

	ntrack_init (&argc, &argv);

	ntrack_monitor_put(monitor);

	monitor = ntrack_monitor_get();
	ntrack_monitor_put(monitor);

	monitor = ntrack_monitor_get ();
	ntrack_monitor_get();

	ntrack_monitor_put(monitor);
	ntrack_monitor_put(monitor);

	return 0;
}
