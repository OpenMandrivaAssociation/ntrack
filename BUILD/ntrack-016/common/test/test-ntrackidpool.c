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
#include "ntrack.h"
#include "ntrackidpool.h"

#include <stdio.h>

int main (int argc, char **argv)
{
	int id1, id2, id3, id4;
	int tmp;
	ntrack_id_pool_t *pool;

	ntrack_init (&argc, &argv);

	pool = ntrack_id_pool_new ();
	id1 = ntrack_id_pool_acquire (pool);
	if (id1 != 0) return (1);
	id2 = ntrack_id_pool_acquire (pool);
	if (id2 != 1) return (2);
	id3 = ntrack_id_pool_acquire (pool);
	if (id3 != 2) return (3);
	printf ("id1: %d, %d, %d\n", id1, id2, id3);

	tmp = id1;
	ntrack_id_pool_release (pool, id1);
	ntrack_id_pool_release (pool, id2);
	ntrack_id_pool_release (pool, id3);
	id1 = ntrack_id_pool_acquire (pool);
	if (id1 != 0) return (4);
	id2 = ntrack_id_pool_acquire (pool);
	if (id2 != 1) return (5);
	id3 = ntrack_id_pool_acquire (pool);
	if (id3 != 2) return (6);
	id4 = ntrack_id_pool_acquire (pool);
	if (id4 != 3) return (7);

	ntrack_id_pool_release (pool, id2);
	ntrack_id_pool_release (pool, id4);
	id2 = ntrack_id_pool_acquire (pool);
	if (id2 != 1) return (8);
	id4 = ntrack_id_pool_acquire (pool);
	if (id4 != 3) return (9);

	/* releasing two times must not add more ids */
	ntrack_id_pool_release (pool, id2);
	ntrack_id_pool_release (pool, id1);
	ntrack_id_pool_release (pool, id1);
	id1 = ntrack_id_pool_acquire (pool);
	id2 = ntrack_id_pool_acquire (pool);
	if (id1 != 0) return (10);
	if (id2 != 1) return (11);

	if (id1 != tmp)
		return 70;

	ntrack_id_pool_free (pool);

	return 0;
}

