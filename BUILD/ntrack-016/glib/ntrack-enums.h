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

#include "ntrackbase.h"

#ifndef __NTRACK_ENUMS_H__
#define __NTRACK_ENUMS_H__

typedef enum /*<prefix=NTRACK_GLIB_STATE>*/
{
        NTRACK_GLIB_STATE_UNSET = NTRACK_STATE_UNSET, 
        NTRACK_GLIB_STATE_ONLINE = NTRACK_STATE_ONLINE,
        NTRACK_GLIB_STATE_OFFLINE = NTRACK_STATE_OFFLINE,
        NTRACK_GLIB_STATE_BLOCKED = NTRACK_STATE_BLOCKED,
        NTRACK_GLIB_STATE_UNKNOWN = NTRACK_STATE_UNKNOWN
} NTrackGlibState;

typedef enum /*<prefix=NTRACK_GLIB_EVENT>*/
{
        NTRACK_GLIB_EVENT_UNSET = NTRACK_EVENT_UNSET,
        NTRACK_GLIB_EVENT_CONNECT = NTRACK_EVENT_CONNECT,
        NTRACK_GLIB_EVENT_DISCONNECT = NTRACK_EVENT_DISCONNECT,
        NTRACK_GLIB_EVENT_RECONNECT = NTRACK_EVENT_RECONNECT
} NTrackGlibEvent;

#endif

