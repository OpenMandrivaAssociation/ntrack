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
#include "ntracklist.h"

static void
iter_func (ntrackpointer data, ntrackpointer user_data);

int main (int argc, char **argv)
{
	ntrack_list_t *list = 0;
	int itercount = 0;

	ntrack_init (&argc, &argv);

	if (ntrack_list_size (list))
		return 1;

	ntrack_list_foreach (list, iter_func, &itercount);
	if (itercount != ntrack_list_size (list))
		return 21;

	list = ntrack_list_append (list, (void*) 1);

	if (ntrack_list_size (list) != 1)
		return 2;

	if (!list)
		return 2;

	itercount = 0;
	ntrack_list_foreach (list, iter_func, &itercount);
	if (itercount != ntrack_list_size (list))
		return 22;

	list = ntrack_list_append (list, (void*) 2);

	if (!list)
		return 3;

	if (ntrack_list_size (list) != 2)
		return 3;

	itercount = 0;
	ntrack_list_foreach (list, iter_func, &itercount);
	if (itercount != ntrack_list_size (list))
		return 23;

	list = ntrack_list_remove (list, list);

	if (ntrack_list_size (list) != 1)
		return 3;

	itercount = 0;
	ntrack_list_foreach (list, iter_func, &itercount);
	if (itercount != ntrack_list_size (list))
		return 23;

	list = ntrack_list_remove (list, list);

	if (list)
		return 4;

	if (ntrack_list_size (list))
		return 5;

	itercount = 0;
	ntrack_list_foreach (list, iter_func, &itercount);
	if (itercount != ntrack_list_size (list))
		return 25;

	list = ntrack_list_insert_sorted (list, (void*) 1, ntrack_compare_direct);

	if (ntrack_list_size (list) != 1)
		return 6;

	itercount = 0;
	ntrack_list_foreach (list, iter_func, &itercount);
	if (itercount != ntrack_list_size (list))
		return 26;

	list = ntrack_list_insert_sorted (list, (void*) 3, ntrack_compare_direct);
	
	if (ntrack_list_size (list) != 2)
		return 7;

	if (((long) list->data) != 1)
		return 8;

	if ((long) list->next->data != 3)
		return 9;

	list = ntrack_list_insert_sorted (list, (void*) 2, ntrack_compare_direct);

	if ((long) list->data != 1 && (long) list->next->data != 2)
		return 10;

	itercount = 0;
	ntrack_list_foreach (list, iter_func, &itercount);
	if (itercount != ntrack_list_size (list))
		return 21;

	return 0;
}

static void
iter_func (ntrackpointer data, ntrackpointer user_data){
	int *i = user_data;
	(*i)++;
}

