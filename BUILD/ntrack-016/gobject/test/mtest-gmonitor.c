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
#include <ntrackutil.h>

#include "ntrack-gsource.h"
#include "ntrack-gmonitor.h"

static gint main_result = 0;
static GMainLoop *main_loop;

static void
_ntrack_event_callback (NTrackGMonitor *self,
                        NTrackGlibEvent event,
                        gpointer data)
{
	g_print ("_ntrack_event_callback: %s (%d)\n", ntrack_util_event_to_ascii (event), event);
}

static void
_ntrack_state_changed_callback (NTrackGMonitor *self,
                                NTrackGlibState oldstate,
                                NTrackGlibState newstate,
                                gpointer data)
{
	g_print ("_ntrack_state_changed_callback: %s (%d) -> %s (%d)\n",
	         ntrack_util_state_to_ascii (oldstate), oldstate,
	         ntrack_util_state_to_ascii (newstate), newstate);
}

int main (int argc, char **argv)
{
	NTrackGMonitor *monitor;
	GMainContext *mainctx = g_main_context_default ();

	ntrack_init (&argc, &argv);
	/* we need gobject, so we need type system */
	g_type_init ();

	/* check that new works at all (first ref) */
	monitor = ntrack_g_monitor_get ();
	if (!monitor)
		return 1;

	g_print ("%s", ntrack_info_human_banner ());

	/* test if _new still works (second ref) */
	if (!ntrack_g_monitor_get ())
		return 2;

	/* test if singleton works (third ref) */
	if (ntrack_g_monitor_get () != monitor)
		return 3;

	/* we have 3 refs here, so unref once and create again should be
	 * stil the same */
	g_object_unref (monitor);
	if (ntrack_g_monitor_get () != monitor)
		return 4;

	/* we have 3 refs here, so unref three times and create again should be
         * a new instance */
	g_object_unref (monitor);
	g_object_unref (monitor);
	g_object_unref (monitor);
	if (ntrack_g_monitor_get () == monitor)
		return 5;

	/* let's get a fresh copy for the event tests */
	monitor = ntrack_g_monitor_get ();
	g_object_unref (monitor);

	g_signal_connect (monitor, "ntrack-event", (GCallback) _ntrack_event_callback, NULL);
	g_signal_connect (monitor, "ntrack-state-changed", (GCallback) _ntrack_state_changed_callback, NULL);

	/* now main loop ... */
	main_loop = g_main_loop_new (mainctx, FALSE);

	g_print ("Initial state: %s\n", ntrack_util_state_to_ascii (ntrack_glib_get_state()));
	g_print ("Manual action needed:\n");
	g_print ("\t... please turn on/off your network ...\n");

	/* and go ... */
	g_main_loop_run (main_loop);

	/* exit with loop result */
	return main_result;
}
