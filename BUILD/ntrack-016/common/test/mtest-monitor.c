/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* 
 * Copyright (C) 2009-2011  Alexander Sack <asac@jwsdot.com>
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
#include "ntrack.h"

#include <stdio.h>
#include <sys/select.h>

void monitor_callback (ntrack_monitor_t *monitor, ntrack_event_t event, ntrackpointer user_data)
{
	ntrack_state_t state = ntrack_monitor_get_state (monitor);

	printf ("ntrack monitor callback:\n");
	printf ("\tevent: %s\n", ntrack_util_event_to_ascii (event));
	printf ("\tstate: %s\n", ntrack_util_state_to_ascii (state));
}

int main (int argc, char **argv)
{
	ntrack_monitor_t *monitor;
	ntrackpointer handle;
	int sr = 0;
	fd_set fds;
	const struct timeval tv = { 2, 0 };
	int *rfds;
	ntrack_state_t initial_state;

	ntrack_init (&argc, &argv);

	monitor = ntrack_monitor_get();
	handle = ntrack_monitor_register (monitor, monitor_callback, 0);
	initial_state = ntrack_monitor_get_state (monitor);

	printf("%s\n", ntrack_info_human_banner());

	if (!handle)
		return 10;

	printf ("Initial monitor state: %s\n", ntrack_util_state_to_ascii (initial_state));
	if (NTRACK_STATE_UNKNOWN == initial_state)
		return 11;

	rfds = ntrack_monitor_get_rfds (monitor);

	if (!rfds)
		return 1;

	if (!*rfds)
		return 2;

	printf ("Manual action needed:\n");
	printf ("\t... please turn on/off your network ...\n");

redo:
	/* our mini main loop until we get any update */
	while (sr == 0) {
		struct timeval tv_tmp = tv;
		FD_ZERO (&fds);
		FD_SET (*rfds, &fds);
		sr = select (*rfds + 1, &fds, 0, 0, &tv_tmp);
	}

	if (!FD_ISSET (*rfds, &fds))
		return 3;

	sr = ntrack_monitor_process_data (monitor, rfds);
	if (sr == 0)
		goto redo;

	if (sr < 0)
		return 5;

	sr = 0;
	goto redo;

	return 0;
}
