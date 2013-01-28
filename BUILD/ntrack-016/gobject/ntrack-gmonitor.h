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
#include <glib-object.h>

#include <ntrack-glib.h>

#ifndef __NTRACK_GMONITOR_H__
#define __NTRACK_GMONITOR_H__

G_BEGIN_DECLS

#define N_TYPE_TRACK_GMONITOR                  (ntrack_g_monitor_get_type ())
#define N_TRACK_GMONITOR(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), N_TYPE_TRACK_GMONITOR, NTrackGMonitor))
#define N_TRACK_IS_GMONITOR(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), N_TYPE_TRACK_GMONITOR))
#define N_TRACK_GMONITOR_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), N_TYPE_TRACK_GMONITOR, NTrackGMonitorClass))
#define N_TRACK_IS_GMONITOR_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), N_TYPE_TRACK_GMONITOR))
#define N_TRACK_GMONITOR_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), N_TYPE_TRACK_GMONITOR, NTrackGMonitorClass))

typedef struct _NTrackGMonitor        NTrackGMonitor;
typedef struct _NTrackGMonitorClass   NTrackGMonitorClass;

GType
ntrack_g_monitor_get_type ();

/**
 * get singleton instance for NTrackGMonitor; remember to release the 
 * ref when not used anymore.
 */
NTrackGMonitor* ntrack_g_monitor_get ();

struct _NTrackGMonitor
{
	GObject parent_instance;
};

struct _NTrackGMonitorClass
{
	GObjectClass parent_class;

	/* signal callback members */
	void (* event)           (NTrackGMonitor *monitor,
	                          ntrack_event_t event);

	void (* state_changed)   (NTrackGMonitor *monitor,
	                          ntrack_state_t old_state,
	                          ntrack_state_t new_state);
};

G_END_DECLS

#endif

