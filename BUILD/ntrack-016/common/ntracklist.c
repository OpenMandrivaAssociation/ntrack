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
#include "stdlib.h"


ntrack_list_t*
ntrack_list_append (ntrack_list_t *list, ntrackpointer data)
{
	ntrack_list_t *new_list = calloc (sizeof (ntrack_list_t), 1);
	ntrack_list_t *end_save = NULL;
	new_list->data = data;
	if(!list) {
		new_list->len = malloc (sizeof (int));
		*new_list->len = 1;
		new_list->next = new_list->prev = new_list;
		return new_list;
	}
	*list->len = *list->len + 1;
	new_list->len = list->len;
	end_save = list->prev;
	end_save->next = new_list;
	new_list->next = list;
	new_list->prev = end_save;
	list->prev = new_list;
	return list;
}

ntrack_list_t*
ntrack_list_insert_sorted (ntrack_list_t *list,
                           ntrackpointer data,
                           ntrack_compare_f func)
{
	return ntrack_list_insert_sorted_unique (list, data, TRUE, func);
}

ntrack_list_t*
ntrack_list_insert_sorted_unique (ntrack_list_t *list,
                                  ntrackpointer data,
                                  int unique,
                                  ntrack_compare_f func)
{
	ntrack_list_t *new_list = calloc (sizeof (ntrack_list_t), 1);
	ntrack_list_t *i = list;
	new_list->data = data;
	if(!list) {
		new_list->len = malloc (sizeof (int));
		*new_list->len = 1;
		new_list->next = new_list->prev = new_list;
		return new_list;
	}

	*list->len = *list->len + 1;
	new_list->len = list->len;

	do {
		int cmp = func(i->data, data);
		if ((unique && cmp >= 0) || cmp > 0)
			break;

		/* if no dupes allowed, cleanup constructed particals
		 * and return
		 */
		if (cmp == 0) {
			*list->len = *list->len - 1;
			free (new_list);
			return list;
		}
		i = i->next;
	} while (i != list);

	/* first link new_list */
	new_list->next = i;
	new_list->prev = i->prev;
	/* now fix i->prev */
	i->prev = new_list;
	/* finally the old i->prev needs a fixed next link to new_list */
	new_list->prev->next = new_list;

	/* special case if the insertion point is previous list start
         * if thats the case it means we either append or prepend; in
         * prepend case the new_list becomes the new head; otherwise
         * all is just fine and we return the normal list */
	if ( i == list  && func(i->data, data) >= 0)
		return new_list;

	return list;
}


ntrack_list_t*
ntrack_list_remove (ntrack_list_t *list, ntrack_list_t *elem)
{
	*elem->len = *elem->len - 1;

	/* we remove the element by linking prev to next */
	elem->prev->next = elem->next;
	elem->next->prev = elem->prev;
	/* if elem was the list start, move list start one to right */
	if (list == elem)
		list = list->next;

	/* if list is NULL here, it means list is dead, free counter */
	if (!*elem->len) {
		free (elem->len);
		list = NULL;
	}

	/* always free element for now */
	free (elem);

	return list;
}

int
ntrack_list_size (ntrack_list_t *list)
{
	if (!list)
		return 0;
	return *list->len;
}

void
ntrack_list_foreach (ntrack_list_t *list,
                     ntrack_list_cb_f func,
                     ntrackpointer user_data)
{
	ntrack_list_t *iter = list;
	while (iter && iter->next != list) {
		(*func) (iter->data, user_data);
		iter = iter->next;
	}
	if (iter) {
		(*func) (iter->data, user_data);
	}
}

void ntrack_list_free (ntrack_list_t *list)
{
	ntrack_list_t* i = list;
	while (i)
		i = ntrack_list_remove (i, i);

}

