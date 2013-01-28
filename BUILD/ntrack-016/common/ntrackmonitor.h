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

#include "ntrackmacros.h"
#include "ntrackbase.h"

#ifndef __NTRACKMONITOR_H__
#define __NTRACKMONITOR_H__

NTRACK_BEGIN_DECLS

typedef struct _ntrack_monitor ntrack_monitor_t;

typedef void (*ntrack_monitor_callback_f) (ntrack_monitor_t        *monitor,
                                           ntrack_event_t  event,
                                           ntrackpointer           user_data);

ntrack_monitor_t *ntrack_monitor_get ();

void ntrack_monitor_put (ntrack_monitor_t *monitor);

ntrackpointer ntrack_monitor_register (ntrack_monitor_t *self,
                                       ntrack_monitor_callback_f callback,
                                       ntrackpointer data);

ntrackpointer ntrack_monitor_register_full (ntrack_monitor_t *self,
                                            ntrack_monitor_callback_f callback,
                                            ntrackpointer data,
                                            ntrack_destroy_f func);

int ntrack_monitor_remove (ntrack_monitor_t *self,
                           ntrackpointer handle);

int* ntrack_monitor_get_rfds (ntrack_monitor_t *self);

int ntrack_monitor_process_data (ntrack_monitor_t *self, int* fds);

ntrack_state_t ntrack_monitor_get_state (ntrack_monitor_t *self);

NTRACK_END_DECLS

#endif /* __NTRACKMONITOR_H__ */
