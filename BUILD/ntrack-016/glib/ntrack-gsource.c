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
#include "ntrack.h"

/* *
 * * NTrackGlibSource implementation for flexible multi-fds main loop integration
 * */

struct __NTrackGlibSource
{
	GSource parent;
	ntrack_list_t *poll_fds;
	ntrack_list_t *up_event_queue;
	ntrackpointer monitor_handle;
	ntrack_monitor_t* monitor;
};

static gboolean
__ntrack_g_source_prepare (GSource    *source,
                          gint       *timeout)
{
	/* don't register for any timeouts; this will
	 * keep this source silence when there are
	 * no fds polls */
	*timeout = -1;

	/* we are a pure fd source; means: always FALSE
	 * see glib docs for GSourceFuncs in general and
	 * GSourceFunc::prepare */
	return FALSE;
}

static gboolean 
__ntrack_g_source_check (GSource *source)
{
	struct __NTrackGlibSource *self = (struct __NTrackGlibSource*) source;
	ntrack_list_t *i = self->poll_fds;
	gint *rfds = self->poll_fds ? g_new0(gint, *(self->poll_fds->len)) : NULL;
	gint rfds_count = 0;

	/* if there are no fds for polling, just return FALSE */
	if (!i)
		return FALSE;

	/* until we reach the start of list/circle again, we will add all fds */
	do {
		GPollFD *poll_fd = i->data;
		if ((poll_fd->revents & G_IO_IN) != 0) {
			rfds[rfds_count] = poll_fd->fd;
			rfds_count++;
		}
		i = i->next;
	} while (i != self->poll_fds);

	if (rfds_count > 0) {
		ntrack_monitor_process_data (self->monitor, rfds);
		g_free (rfds);
	}
	return !!self->up_event_queue;
}

static gboolean
__ntrack_g_source_dispatch (GSource *source,
                           GSourceFunc callback,
                           gpointer user_data)
{
	NTrackGlibSourceFunc func = (NTrackGlibSourceFunc) callback;
	struct __NTrackGlibSource *self = (struct __NTrackGlibSource*) source;

	/* dispatch all events accumulated during _source_check */
	while (self->up_event_queue) {
		func ((ntrack_event_t) self->up_event_queue->data, user_data);
		self->up_event_queue = ntrack_list_remove (self->up_event_queue,
		                                           self->up_event_queue);
	}

	/* this is a meta source, that never should
	 * remove it self; because we use timeout=-1 above
	 * this source will not be called if no pollids
	 * are registered, so keeping it active should
	 * be of minimal maintenance overhead. */
	return TRUE;
}

static void
__ntrack_g_source_remove_one_poll_fd (struct __NTrackGlibSource* source,
                                     GPollFD *poll_fd)
{
	g_assert (poll_fd);

	g_warning ("dropping poll id: %d", (gint) poll_fd->fd);

	source->poll_fds = ntrack_list_remove (source->poll_fds,
	                                       source->poll_fds);

	g_source_remove_poll ((GSource*) source, poll_fd);

	g_free (poll_fd);
}

static void
__ntrack_g_source_finalize (GSource *source)
{
	struct __NTrackGlibSource* self = (struct __NTrackGlibSource*) source;
	while (self->poll_fds) {
		GPollFD *poll_fd = self->poll_fds->data;
		self->poll_fds = ntrack_list_remove (self->poll_fds,
		                                     self->poll_fds);
		g_free (poll_fd);
	}

	if (self->monitor) {
		ntrack_monitor_remove (self->monitor, self->monitor_handle);
		ntrack_monitor_put (self->monitor);
		self->monitor = NULL;
	}
}

static void
__ntrack_g_source_add_one_poll_fd (struct __NTrackGlibSource* source,
                                  int fd,
                                  GIOCondition condition)
{
	GPollFD *poll_fd = g_new0 (GPollFD, 1);

	poll_fd->fd = fd;
	poll_fd->events  = condition;
	poll_fd->revents = 0;

	/* register and remember */
	g_source_add_poll ((GSource*) source, poll_fd);
	source->poll_fds = ntrack_list_append (source->poll_fds, poll_fd);
}

static void
__ntrack_g_source_update_poll_fds (struct __NTrackGlibSource* source, int *rfds, int *wfds)
{
	while (source->poll_fds)
	{
		__ntrack_g_source_remove_one_poll_fd (source, source->poll_fds->data);

	}

	/* create all initial polls for read-fds and register/remember them */
	while (*rfds) {
		__ntrack_g_source_add_one_poll_fd (source,
		                               *rfds,
		                               G_IO_IN | G_IO_HUP | G_IO_ERR);
		rfds++;
	}

	/* create all initial polls for write-fds and register/remember them */
	while (*wfds) {
		__ntrack_g_source_add_one_poll_fd (source,
		                               *wfds,
		                               G_IO_OUT | G_IO_ERR);
		wfds++;
	}
}

static GSourceFuncs
__ntrack_g_source_funcs = {
	/* .prepare  = */ __ntrack_g_source_prepare,
	/* .check    = */ __ntrack_g_source_check,
	/* .dispatch = */ __ntrack_g_source_dispatch,
	/* .finalize = */ __ntrack_g_source_finalize,
	/* Note: closures are gobject world, so keep them NULL */
	/* .closure_callback = */ NULL,
	/* .closure_marshal = */ NULL
};

static void
__ntrack_monitor_callback (ntrack_monitor_t *monitor,
                           ntrack_event_t event,
                           ntrackpointer user_data)
{
	struct __NTrackGlibSource *source = user_data;
	source->up_event_queue = ntrack_list_append (source->up_event_queue, GUINT_TO_POINTER (event));
}


GSource*
ntrack_g_source_new ()
{
	GSourceFuncs *source_funcs;
	struct __NTrackGlibSource *source;
	ntrack_monitor_t *monitor = ntrack_monitor_get ();
	gint* rfds = ntrack_monitor_get_rfds (monitor);
	int wfds[] = { 0 };

	source_funcs = g_new0 (GSourceFuncs, 1);
	*source_funcs = __ntrack_g_source_funcs;

	source = (struct __NTrackGlibSource *)
		g_source_new (source_funcs, sizeof (struct __NTrackGlibSource));
	source->monitor = monitor;

	source->monitor_handle = ntrack_monitor_register (monitor,
                                                          __ntrack_monitor_callback,
                                                          source);

	__ntrack_g_source_update_poll_fds (source, rfds, wfds);

	g_free(rfds);

	return (GSource*) source;
}

guint
ntrack_g_source_add (NTrackGlibSourceFunc func, gpointer data)
{

	return ntrack_g_source_add_full (G_PRIORITY_DEFAULT,
	                                func,
	                                data,
	                                NULL);
}

guint
ntrack_g_source_add_full (gint priority,
                          NTrackGlibSourceFunc func,
                          gpointer data,
                          GDestroyNotify notify)
{
	struct __NTrackGlibSource *source = (struct __NTrackGlibSource *)
		ntrack_g_source_new ();
	guint sid;

	g_source_set_priority ((GSource*) source, priority);
	g_source_set_callback ((GSource*) source,
	                       (GSourceFunc) func,
	                       data,
	                       notify);

#ifdef THREAD_EXPERIMENT
	sid = g_source_attach ((GSource*) source, g_main_context_get_thread_default ());
#else
	sid = g_source_attach ((GSource*) source, g_main_context_default ());
#endif
	g_source_unref ((GSource*) source);
	return sid;
}

NTrackGlibState
ntrack_glib_get_state ()
{
	NTrackGlibState state;
	ntrack_monitor_t *monitor;

	monitor = ntrack_monitor_get();
	state = ntrack_monitor_get_state (monitor);
	ntrack_monitor_put(monitor);

	return state;
}

