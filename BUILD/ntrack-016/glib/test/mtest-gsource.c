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
#include "ntrack-gsource.h"
#include "ntrackutil.h"

static gint main_result = 0;
static GMainLoop *main_loop;

static gboolean
NTrackGlibSourceFunc_callback (NTrackGlibEvent event, gpointer user_data)
{
	static gint online_count = 0;
	ntrack_state_t state;

	switch (event) {
	case NTRACK_EVENT_CONNECT:
		g_message ("NTrackGlibEvent: CONNECT");
		online_count++;
		break;
	case NTRACK_EVENT_DISCONNECT:
		g_message ("NTrackGlibEvent: DISCONNECT");
		break;
	case NTRACK_EVENT_RECONNECT:
		g_message ("NTrackGlibEvent: RECONNECT");
		break;
	default:
		g_message ("NTrackGlibEvent: UNKOWN (%d)", event);
		main_result = 2;
		g_main_loop_quit (main_loop);
		break;
	}

	state = ntrack_glib_get_state ();

	switch (state) {
	case NTRACK_STATE_ONLINE:
		g_message ("ntrack_state_t: ONLINE");
		break;
	case NTRACK_STATE_OFFLINE:
		g_message ("ntrack_state_t: OFFLINE");
		break;
	default:
		g_message ("ntrack_state_t: UNKNOWN (%d)", state);
	}

	if (online_count > 1)
		g_main_loop_quit (main_loop);

	return TRUE;
}

int main (int argc, char **argv)
{
	guint sourceid;
	GMainContext *mainctx;

	ntrack_init (&argc, &argv);

	sourceid = ntrack_g_source_add (NTrackGlibSourceFunc_callback, NULL);

	g_message("\n%s", ntrack_info_human_banner());

	mainctx = g_main_context_default ();

	main_loop = g_main_loop_new (mainctx, FALSE);
	if (!sourceid)
		return 1;

	g_print ("Initial state: %s\n", ntrack_util_state_to_ascii (ntrack_glib_get_state()));
	g_message ("Manual action needed:");
	g_message ("\t... please turn on/off your network ...");

	g_main_loop_run (main_loop);
	return main_result;
}
