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
#include "ntrackmonitor.h"
#include "ntrackarchapi.h"
#include "ntrackarch.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PACK_BLOCK_SIZE 256

static ntrack_monitor_t* __new ();
static void __free (ntrack_monitor_t* monitor);
static ntrack_monitor_t *global_monitor = 0;
static int refc = 0;

struct _listener_pack {
	ntrack_monitor_callback_f  callback;
	ntrackpointer              user_data;
	ntrack_destroy_f           destroy;
};

ntrack_monitor_t*
ntrack_monitor_get ()
{
	if (!global_monitor) {
		global_monitor = __new ();
	}
	refc++;
	return global_monitor;
}

void
ntrack_monitor_put (ntrack_monitor_t *monitor)
{
	if (!monitor)
		return;

	assert (monitor == global_monitor);

	--refc;
	if (refc == 0) {
		__free (monitor);
		global_monitor = 0;
	}
	if (refc < 0)
		refc = 0;
}

ntrackpointer
ntrack_monitor_register (ntrack_monitor_t *self,
                         ntrack_monitor_callback_f callback,
                         ntrackpointer data)
{
	int i, p = -1;
	if (self->pack_size <= self->pack_len + 1) {
		struct _listener_pack **sav = self->packs;
		self->pack_size += PACK_BLOCK_SIZE;
		self->packs = calloc (sizeof (struct _listener_pack*), self->pack_size);
		memcpy (self->packs, sav, self->pack_size - PACK_BLOCK_SIZE);
		free(sav);
	}

	for (i = 0; i < self->pack_size && p == -1; i++) {
		if(!self->packs[i])
			p=i;
	}

	/* its a bug if no slot is found */
	assert (p >= 0);

	self->pack_len++;

	self->packs[p] = calloc (sizeof (struct _listener_pack), 1);
	assert (self->packs[p]);

	self->packs[p]->callback = callback;
	self->packs[p]->user_data = data;

	return self->packs[p];
}

ntrackpointer
ntrack_monitor_register_full (ntrack_monitor_t *self,
                              ntrack_monitor_callback_f callback,
                              ntrackpointer data,
                              ntrack_destroy_f func)
{
	struct _listener_pack *pack = ntrack_monitor_register (self, callback, data);
	if (pack)
		pack->destroy = func;
	return pack;
}

int
ntrack_monitor_remove (ntrack_monitor_t *self,
                       ntrackpointer ntrack_handle)
{
	int i;
	struct _listener_pack *pack;
	if (!ntrack_handle)
		return 0;

	for (i = 0; i < self->pack_size; i++) {
		if (self->packs[i] == ntrack_handle)
			break;
	}
	if (i >= self->pack_size)
		return 0;

	pack = self->packs[i];
	if (pack->destroy)
		pack->destroy(pack->user_data);
	self->packs[i] = 0;
	free (pack);
	return 1;
}

int*
ntrack_monitor_get_rfds (ntrack_monitor_t *self)
{
	return ntrack_arch_get_rfds (self);
}

int
ntrack_monitor_process_data (ntrack_monitor_t *self, int* fds)
{
	return ntrack_arch_process_data (self, fds);
}

ntrack_state_t
ntrack_monitor_get_state (ntrack_monitor_t *self)
{
	return self->state;
}

static void
dispatch_monitor_event (ntrack_monitor_t *self,
                        ntrack_event_t event)
{
	int i;

	switch (event) {
	case NTRACK_EVENT_CONNECT:
		self->state = NTRACK_STATE_ONLINE;
		break;
	case NTRACK_EVENT_DISCONNECT:
		self->state = NTRACK_STATE_OFFLINE;
		break;
	case NTRACK_EVENT_RECONNECT:
		self->state = NTRACK_STATE_ONLINE;
		break;
	default:
		self->state = NTRACK_STATE_UNKNOWN;
	}

	for (i = 0; i < self->pack_len; i++) {
		struct _listener_pack *pack = self->packs[i];
		pack->callback (self, event, pack->user_data);
	}
}

static void
process_monitor_arch_event (ntrack_monitor_t *self,
                            ntrack_event_t event,
                            ntrackpointer user_data)
{

	switch (event) {
	case NTRACK_EVENT_CONNECT:
		dispatch_monitor_event (self, NTRACK_EVENT_CONNECT);
		break;
	case NTRACK_EVENT_DISCONNECT:
		dispatch_monitor_event (self, NTRACK_EVENT_DISCONNECT);
		break;
	case NTRACK_EVENT_RECONNECT:
		dispatch_monitor_event (self, NTRACK_EVENT_RECONNECT);
		break;
	default:
		printf ("Unknown ntrack_event_t: %d\n", event);
		assert (FALSE);
	}
}

/* private/static funcs */
static ntrack_monitor_t*
__new ()
{
	/*delegate to arch specific implementation */
	ntrack_monitor_t *arch = ntrack_arch_new (process_monitor_arch_event, NULL);
	return arch;
}

static void
__free (ntrack_monitor_t* self)
{
	int i;

	for (i = 0; i < self->pack_size; i++) {
		if (!self->packs[i])
			continue;
		if (!ntrack_monitor_remove (self, self->packs[i]))
			printf ("ntrack_monitor_remove failed for self->packs[i]: %p\n", (ntrackpointer) self->packs[i]);
	}

	self->pack_size = 0;
	self->pack_len = 0;
	free (self->packs);

	ntrack_arch_free (self);
}

