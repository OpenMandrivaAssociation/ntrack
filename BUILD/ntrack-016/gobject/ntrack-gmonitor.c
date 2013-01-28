/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* 
 * Copyright (C) 2009  Alexander Sack <asac@jwsdot.com>
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

#include "ntrackbase.h"
#include "ntrack-gsource.h"
#include "ntrack-gmonitor.h"
#include "ntrack-gmarshal.h"
#include "ntrack-enumtypes.h"

enum {
	_EVENT = 0,
	_STATE_CHANGED,
	_LAST
};

typedef struct _NTrackGMonitorPrivate
{
	NTrackGlibState state;
	guint gsource_id;
} NTrackGMonitorPrivate;

#define N_TRACK_GMONITOR_PRIVATE(s) G_TYPE_INSTANCE_GET_PRIVATE(s,N_TYPE_TRACK_GMONITOR,NTrackGMonitorPrivate)

static int signals [_LAST];

G_DEFINE_TYPE (NTrackGMonitor, ntrack_g_monitor, G_TYPE_OBJECT)

static union {
  gpointer ptr;
  NTrackGMonitor *obj;
} _singleton;
static int initialized = 0;

static GObject *
ntrack_g_monitor_constructor (GType                  gtype,
                              guint                  n_properties,
                              GObjectConstructParam *properties)
{
	if (!initialized) {
		GObjectClass *parent_class;  
		parent_class = G_OBJECT_CLASS (ntrack_g_monitor_parent_class);
		_singleton.obj = N_TRACK_GMONITOR (parent_class->constructor (gtype, n_properties, properties));
		initialized = 1;
	} else {
		g_object_ref (_singleton.obj);
	}

	return G_OBJECT (_singleton.obj);
}

static void
ntrack_g_monitor_finalize (GObject *self)
{
	NTrackGMonitor *monitor = N_TRACK_GMONITOR (self);
	NTrackGMonitorPrivate *priv = N_TRACK_GMONITOR_PRIVATE (self);
	g_assert (monitor);

	if (priv->gsource_id)
		g_source_remove (priv->gsource_id);

	_singleton.ptr = NULL;
	initialized = 0;
}

static gboolean
_event_callback (NTrackGlibEvent event, gpointer user_data)
{
	NTrackGlibState oldstate;
	NTrackGMonitor *self = N_TRACK_GMONITOR (user_data);
	NTrackGMonitorPrivate *priv = N_TRACK_GMONITOR_PRIVATE (self);
	g_assert (self);

	/* update state; keep oldstate to condition state signal emit */
	oldstate = priv->state;
	priv->state = ntrack_glib_get_state ();

	/* dispatch all events */
	g_signal_emit (self, signals[_EVENT], 0, event, NULL);

	/* emit state changes only */
	if (oldstate != priv->state)
		g_signal_emit (self, signals[_STATE_CHANGED], 0, oldstate, priv->state, NULL);

	return TRUE;
}

static void
ntrack_g_monitor_init (NTrackGMonitor *self)
{
	NTrackGMonitorPrivate *priv = N_TRACK_GMONITOR_PRIVATE (self);
	g_assert (self);

	priv->gsource_id = ntrack_g_source_add (_event_callback, self);
	g_assert (priv->gsource_id);

	priv->state = ntrack_glib_get_state ();
}

static void
ntrack_g_monitor_class_init (NTrackGMonitorClass *klass)
{
	GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

	gobject_class->constructor = ntrack_g_monitor_constructor;
	gobject_class->finalize = ntrack_g_monitor_finalize;

	g_type_class_add_private (klass, sizeof (NTrackGMonitorPrivate));

	signals [_EVENT] =
	  g_signal_new ("ntrack-event",
	                G_OBJECT_CLASS_TYPE (gobject_class),
	                G_SIGNAL_RUN_FIRST,
	                G_STRUCT_OFFSET (NTrackGMonitorClass, event),
	                NULL, NULL,
	                _ntrack_cclosure_marshal_VOID__ENUM,
	                G_TYPE_NONE, 1, n_track_glib_event_get_type());

	signals [_STATE_CHANGED] =
	  g_signal_new ("ntrack-state-changed",
	                G_OBJECT_CLASS_TYPE (gobject_class),
	                G_SIGNAL_RUN_FIRST,
	                G_STRUCT_OFFSET (NTrackGMonitorClass, state_changed),
	                NULL, NULL,
	                _ntrack_cclosure_marshal_VOID__ENUM_ENUM,
	                G_TYPE_NONE, 2, n_track_glib_state_get_type(), n_track_glib_event_get_type());
}


NTrackGMonitor* ntrack_g_monitor_get ()
{
	_singleton.obj = g_object_new (N_TYPE_TRACK_GMONITOR, NULL);
	return _singleton.obj;
}


