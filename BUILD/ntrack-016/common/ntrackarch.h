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
#include "ntrackmonitor.h"

#ifndef __NTRACK_ARCH_H__
#define __NTRACK_ARCH_H__

NTRACK_BEGIN_DECLS

#pragma GCC visibility push(hidden)

ntrack_monitor_t*
ntrack_arch_new (ntrack_monitor_callback_f callback, ntrackpointer user_data);

int*
ntrack_arch_get_rfds (ntrack_monitor_t *self);

int
ntrack_arch_process_data (ntrackpointer self, int* fds);

void
ntrack_arch_free (ntrackpointer self);

#pragma GCC visibility pop

NTRACK_END_DECLS

#endif /* __NTRACK_ARCH_H__ */

