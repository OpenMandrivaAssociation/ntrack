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
#include "ntrackmacros.h"

#ifndef __NTRACKIDPOOL_H__
#define __NTRACKIDPOOL_H__

NTRACK_BEGIN_DECLS

typedef struct _ntrack_id_pool ntrack_id_pool_t;

ntrack_id_pool_t* ntrack_id_pool_new ();
long ntrack_id_pool_acquire (ntrack_id_pool_t *self);
void ntrack_id_pool_release (ntrack_id_pool_t *self, long id);
void ntrack_id_pool_free (ntrack_id_pool_t *self);

NTRACK_END_DECLS

#endif /* __NTRACKIDPOOL_H__ */
