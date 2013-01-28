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
#include "ntrackcompare.h"

#ifndef __NTRACKLIST_H__
#define __NTRACKLIST_H__
 
NTRACK_BEGIN_DECLS
 
typedef struct _ntrack_list ntrack_list_t;
typedef void ntrack_list_cb_f (ntrackpointer data, ntrackpointer user_data);

ntrack_list_t* ntrack_list_insert_sorted (ntrack_list_t *list,
                                          ntrackpointer data,
                                          ntrack_compare_f sort_func);
ntrack_list_t* ntrack_list_insert_sorted_unique (ntrack_list_t *list,
                                                 ntrackpointer data,
                                                 int unique,
                                                 ntrack_compare_f func);
ntrack_list_t* ntrack_list_append (ntrack_list_t *list, ntrackpointer data);
ntrack_list_t* ntrack_list_remove (ntrack_list_t *list, ntrack_list_t *elem);
int ntrack_list_size (ntrack_list_t *list);
void ntrack_list_foreach (ntrack_list_t *list, ntrack_list_cb_f func, ntrackpointer user_data);
void ntrack_list_free (ntrack_list_t *list);

struct _ntrack_list {
	ntrack_list_t *prev;
	ntrack_list_t *next;

	ntrackpointer data;
	int *len;
};

NTRACK_END_DECLS

#endif /* __NTRACKLIST_H__ */
