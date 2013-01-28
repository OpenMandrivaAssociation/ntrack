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

#ifndef __NTRACKUTIL_H__
#define __NTRACKUTIL_H__

NTRACK_BEGIN_DECLS

const static inline char*
ntrack_util_state_to_ascii (ntrack_state_t state)
{
	switch (state) {
	case NTRACK_STATE_UNSET:
		return "NTRACK_STATE_UNSET";
	case NTRACK_STATE_ONLINE:
		return "NTRACK_STATE_ONLINE";
	case NTRACK_STATE_OFFLINE:
		return "NTRACK_STATE_OFFLINE";
	case NTRACK_STATE_BLOCKED:
		return "NTRACK_STATE_BLOCKED";
	case NTRACK_STATE_UNKNOWN:
		return "NTRACK_STATE_UNKNOWN";
	default:
		return "NTRACK_STATE_(UNDEFINED)";
	}
}

const static inline char*
ntrack_util_event_to_ascii (ntrack_event_t event)
{
	switch (event) {
	case NTRACK_EVENT_UNSET:
		return "NTRACK_EVENT_UNSET";
	case NTRACK_EVENT_CONNECT:
		return "NTRACK_EVENT_ONLINE";
	case NTRACK_EVENT_DISCONNECT:
		return "NTRACK_EVENT_OFFLINE";
	case NTRACK_EVENT_RECONNECT:
		return "NTRACK_EVENT_RECONNECT";
	default:
		return "NTRACK_EVENT_(UNDEFINED)";
	}
}

NTRACK_END_DECLS

#endif /* __NTRACKUTIL_H__ */
