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

#include "ntrackmacros.h"

#ifndef __NTRACKBASE_H__
#define __NTRACKBASE_H__

NTRACK_BEGIN_DECLS

typedef enum {
	NTRACK_STATE_UNSET = 0,
	NTRACK_STATE_ONLINE = 1,
	NTRACK_STATE_OFFLINE = 2,
	NTRACK_STATE_BLOCKED = 3,
	NTRACK_STATE_UNKNOWN = 4
} ntrack_state_t;

typedef enum {
	NTRACK_EVENT_UNSET = 0,
	NTRACK_EVENT_CONNECT = 1,
	NTRACK_EVENT_DISCONNECT = 2,
	NTRACK_EVENT_RECONNECT = 3
} ntrack_event_t;

typedef void* ntrackpointer;

typedef void (*ntrack_destroy_f) (ntrackpointer to_destroy);

void ntrack_init (int *argc, char ***argv);

const char* ntrack_info_human_banner ();

NTRACK_END_DECLS

#endif /* __NTRACKBASE_H__ */
