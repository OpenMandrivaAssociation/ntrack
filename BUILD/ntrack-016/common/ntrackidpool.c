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
#include "ntrackidpool.h"
#include "ntracklist.h"

#include <assert.h>
#include <stdlib.h>

#define NTRACK_POOL_GEN_SIZE 128

struct _ntrack_id_pool {
	ntrack_list_t *pool;
	long size;
};

ntrack_id_pool_t*
ntrack_id_pool_new ()
{
	ntrack_id_pool_t *self = calloc (sizeof (ntrack_id_pool_t), 1);
	self->size = 0;
	self->pool = NULL;
	return self;
}

long
ntrack_id_pool_acquire (ntrack_id_pool_t *self)
{
	long rval;
	if (!ntrack_list_size (self->pool)) {
		int c = NTRACK_POOL_GEN_SIZE;
		while (c) {
			self->pool = ntrack_list_insert_sorted_unique (self->pool,
			                                               (void*) self->size,
			                                               FALSE,
			                                               ntrack_compare_direct);
			self->size++;
			c--;
		}
	}
	rval = (long) self->pool->data;
	self->pool = ntrack_list_remove (self->pool, self->pool);
	return rval;
}

void
ntrack_id_pool_release (ntrack_id_pool_t *self, long id)
{
	assert (id < self->size);
	self->pool = ntrack_list_insert_sorted_unique (self->pool,
	                                               (void*) id,
	                                               FALSE,
	                                               ntrack_compare_direct);
}

void
ntrack_id_pool_free (ntrack_id_pool_t *self)
{
	while (self->pool)
		self->pool = ntrack_list_remove (self->pool, self->pool);
	free (self);
}

