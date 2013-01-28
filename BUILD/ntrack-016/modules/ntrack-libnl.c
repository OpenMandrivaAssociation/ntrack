/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* 
 * Copyright (C) 2009-2011  Alexander Sack <asac@jwsdot.com>
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

#include "config.h"

#include "ntrackarchapi.h"
#include "ntracklist.h"

#include <error.h>
#include <assert.h>

#include <arpa/inet.h>
#include <netinet/in.h>

/* import netlink stuff required */
#include <netlink/netlink.h>
#include <netlink/object.h>
#include <netlink/object-api.h>
#include <netlink/route/addr.h>
#include <netlink/route/link.h>
#include <netlink/route/rtnl.h>
#include <netlink/route/route.h>

#include <linux/if.h>

#ifndef ROUTE_ATTR_OIF
#define ROUTE_ATTR_OIF       0x000400
#endif
#ifndef ROUTE_ATTR_GATEWAY
#define ROUTE_ATTR_GATEWAY   0x000800
#endif

#ifdef HAVE_LIBNL1
typedef struct nl_handle NTRACK_nl_handle;

#define NL_CACHE_MNGR_ALLOC(handle,mngr_o) \
	mngr_o = nl_cache_mngr_alloc (handle, \
                                      NETLINK_ROUTE, \
                                      NL_AUTO_PROVIDE)

#define NL_CACHE_MNGR_ADD(mngr,name,cache_o) \
	cache_o = nl_cache_mngr_add (mngr, \
	                             name, \
                                     0)
#define NL_PRINT_ERROR(text,err) \
	printf ("%s/%s\n", text, nl_geterror())

#define RTNL_ROUTE_GET_PRIO rtnl_route_get_prio

#elif defined(HAVE_LIBNL2) || defined (HAVE_LIBNL3) || defined (HAVE_LIBNL3_1) || defined (HAVE_LIBNL3_X)
typedef struct nl_sock NTRACK_nl_handle;

#define NL_CACHE_MNGR_ALLOC(handle,mngr_o) \
	nl_cache_mngr_alloc (handle, NETLINK_ROUTE, NL_AUTO_PROVIDE, &mngr_o)

#define NL_CACHE_MNGR_ADD(mngr,name,cache_o) \
	nl_cache_mngr_add (mngr, \
	                   name, \
	                   0, \
	                   0, \
	                   &cache_o)
#define NL_PRINT_ERROR(text,err) \
	printf ("%s/%s\n", text, nl_geterror(err))

#define RTNL_ROUTE_GET_PRIO rtnl_route_get_priority

#else
#error no LIBNLx found
#endif /* HAVE_LIBNL1 */

struct _nl_info
{
	NTRACK_nl_handle *handle;
	NTRACK_nl_handle *smart_update_handle;

	struct nl_cache_mngr *update_cache_mngr;
	struct nl_cache_mngr *smart_update_cache_mngr;

	struct nl_cache *route_cache;
	struct nl_cache *addr_cache;
	struct nl_cache *link_cache;
	struct nl_cache *neigh_cache;

	struct rtnl_route *topmost_route;
	int update_run;
};

static void
setup_main_handle (struct _nl_info *nl_info);

static void
setup_smart_handle (struct _nl_info *nl_info);

static int
smart_cache_update (ntrack_monitor_t *self, ntrackpointer user_data);

static int
update_caches (struct _nl_info *nl_info);

static void
update_connectivity (ntrack_monitor_t *self, struct _nl_info *nl_info);

static int
cmp_rtnl_route_metric (ntrackpointer first, ntrackpointer second);

static void
op_default_route_appeared (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info);

static void
op_default_route_disappeared (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info);

static void
op_default_route_changed (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info);


ntrack_monitor_t*
_ntrack_arch_new (ntrack_monitor_callback_f callback, ntrackpointer user_data)
{
	struct _ntrack_monitor_arch *arch = calloc(sizeof(struct _ntrack_monitor_arch), 1);
	struct _nl_info *nl_info = calloc (sizeof (struct _nl_info), 1);

	/* setup ntrack_monitor_t part */
	((ntrack_monitor_t *)arch)->state = NTRACK_STATE_UNKNOWN;

	/* setup struct ntrack_monitor_arch part */
	arch->data = nl_info;
	arch->cb = callback;
	arch->cb_user_data = user_data;

#ifdef HAVE_LIBNL1
	nl_info->handle = nl_handle_alloc();
	nl_info->smart_update_handle = nl_handle_alloc();
#elif defined(HAVE_LIBNL2) || defined(HAVE_LIBNL3) || defined (HAVE_LIBNL3_1) || defined (HAVE_LIBNL3_X)
	nl_info->handle = nl_socket_alloc();
	nl_info->smart_update_handle = nl_socket_alloc();
#else
#error "needs libnl-1 or -2.0"
#endif
	if (!nl_info->handle)
		error (1, 255, "OOM (nl_handle_alloc)");

	if (!nl_info->smart_update_handle)
		error (1, 257, "OOM (nl_handle_alloc)");

	setup_main_handle (nl_info);
	setup_smart_handle (nl_info);

	/* set smart update handle to nonblocking */
	if (nl_socket_set_nonblocking (nl_info->smart_update_handle))
		error (1,260,"cannot set smart handle to nonblocking");

	update_caches (nl_info);
	update_connectivity ((ntrack_monitor_t *) arch, nl_info);

	return (ntrack_monitor_t*) arch;
}

int*
_ntrack_arch_get_rfds (ntrack_monitor_t *self)
{
	struct _ntrack_monitor_arch *arch = (struct _ntrack_monitor_arch*) self;
	int* fds = calloc (sizeof(int), 2);
	struct _nl_info *info = arch->data;
	/*
	 * just the smart_update_handle is used to get smart
	 * updates hint from the application mainloop.
	 */
	fds[0] = nl_cache_mngr_get_fd (info->smart_update_cache_mngr);
	return fds;
}

int
_ntrack_arch_process_data (ntrackpointer self, int* fds)
{
	struct _ntrack_monitor_arch *arch = self;
	struct _nl_info *nl_info = arch->data;
	/*
	 * libnl is too broken to rely on caches ... so just consume
	 * all data from the fd until libnl is fixed
	 */
	int rval = nl_cache_mngr_data_ready (nl_info->smart_update_cache_mngr);

	/*
	 * if all looks good and there was relevant data for the managed
	 * caches then we attempt a smart cache update. This will flush
	 * our caches and reinterpret the new state and emit events
	 * as appropriate.
	 */
	if(rval)
		if (!smart_cache_update (self, nl_info))
			return -1;
	return rval;
}

void
_ntrack_arch_free (ntrackpointer self)
{
	struct _ntrack_monitor_arch *arch = self;
	struct _nl_info *nl_info = arch->data;

	if (nl_info->topmost_route)
		rtnl_route_put(nl_info->topmost_route);
	if (nl_info->smart_update_cache_mngr)
		nl_cache_mngr_free (nl_info->smart_update_cache_mngr);
	if (nl_info->update_cache_mngr)
		nl_cache_mngr_free (nl_info->update_cache_mngr);
	if (nl_info->smart_update_handle)
		nl_close (nl_info->smart_update_handle);
	if (nl_info->handle)
		nl_close (nl_info->handle);

	free (nl_info);
	free (self);
}

static void
setup_main_handle (struct _nl_info *nl_info)
{
	NL_CACHE_MNGR_ALLOC (nl_info->handle, nl_info->update_cache_mngr);

	if (!nl_info->addr_cache) {
		NL_CACHE_MNGR_ADD (nl_info->update_cache_mngr, "route/addr", nl_info->addr_cache);
		if (!nl_info->addr_cache)
			error (1, 366, "cache mngr add fail.");
	}

	if (!nl_info->link_cache) {
		NL_CACHE_MNGR_ADD (nl_info->update_cache_mngr, "route/link", nl_info->link_cache);
		if (!nl_info->link_cache)
			error (1, 367, "cache mngr add fail.");
	}

	if (!nl_info->route_cache) {
		NL_CACHE_MNGR_ADD (nl_info->update_cache_mngr, "route/route", nl_info->route_cache);
		if (!nl_info->route_cache)
			error (1, 368, "cache mngr add fail.");
	}

	if (!nl_info->neigh_cache) {
		NL_CACHE_MNGR_ADD (nl_info->update_cache_mngr, "route/neigh", nl_info->neigh_cache);
		if (!nl_info->neigh_cache)
			error (1, 369, "cache mngr add fail.");
	}
}

static void
setup_smart_handle (struct _nl_info *nl_info)
{
	struct nl_cache *tmp_cache = 0;
	NL_CACHE_MNGR_ALLOC (nl_info->smart_update_handle, nl_info->smart_update_cache_mngr);

	if (!nl_info->smart_update_cache_mngr)
		error (1, 265, "cache mngr alloc failed");

	/* add caches for events we are interested in. We
	 * don't need to remember them as we are solely
	 * interested on whether something of relevance for
	 * us changed or not. If thats the case we will
	 * explicitly refill the primary caches and calculate
	 * network status changes.
	 */
	
	NL_CACHE_MNGR_ADD (nl_info->smart_update_cache_mngr, "route/addr", tmp_cache);

	if (!tmp_cache)
		error (1, 266, "cache mngr add fail.");

	tmp_cache = 0;
	NL_CACHE_MNGR_ADD (nl_info->smart_update_cache_mngr, "route/link", tmp_cache);

	if (!tmp_cache)
		error (1, 267, "cache mngr add fail.");

	tmp_cache = 0;
	NL_CACHE_MNGR_ADD (nl_info->smart_update_cache_mngr, "route/route", tmp_cache);

	if (!tmp_cache)
		error (1, 268, "cache mngr add fail.");

	tmp_cache = 0;
	NL_CACHE_MNGR_ADD (nl_info->smart_update_cache_mngr, "route/neigh", tmp_cache);

	if (!tmp_cache)
		error (1, 269, "cache mngr add fail.");
}


static int
smart_cache_update (ntrack_monitor_t *self, ntrackpointer user_data)
{
	struct _nl_info *nl_info = user_data;

	update_caches(nl_info);
	update_connectivity (self, nl_info);

	return TRUE;
}

static int
update_caches (struct _nl_info *nl_info)
{
	assert (nl_info->route_cache);
	assert (nl_info->link_cache);
	assert (nl_info->addr_cache);

 	nl_cache_mngr_data_ready (nl_info->update_cache_mngr);

	return TRUE;
}

static int
cmp_rtnl_route_metric (ntrackpointer first, ntrackpointer second)
{
	struct rtnl_route *first_route = first;
	struct rtnl_route *second_route = second;
	int metric1, metric2;

	if (first_route == NULL || second_route == NULL)
		return 0;

	metric1 = RTNL_ROUTE_GET_PRIO (first_route);
	metric2 = RTNL_ROUTE_GET_PRIO (second_route);

	if (metric1 < metric2)
		return -1;
	else if (metric1 > metric2)
		return 1;
	return 0;
}

static void
op_default_route_appeared (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info)
{
	if (self->cb)
		self->cb ((ntrack_monitor_t*) self, NTRACK_EVENT_CONNECT, self->cb_user_data);
}

static void
op_default_route_disappeared (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info)
{
	if (self->cb)
		self->cb ((ntrack_monitor_t*) self, NTRACK_EVENT_DISCONNECT, self->cb_user_data);
}

static void
op_default_route_changed (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info)
{
	if (self->cb)
		self->cb ((ntrack_monitor_t*) self, NTRACK_EVENT_RECONNECT, self->cb_user_data);
}

static int
nl_link_index_cmp (ntrackpointer a, ntrackpointer b)
{
	struct rtnl_link *link_a = a;
	struct rtnl_link *link_b = b;
	int ifindex_a = rtnl_link_get_ifindex (link_a);
	int ifindex_b = rtnl_link_get_ifindex (link_b);

	if (ifindex_a < ifindex_b)
		return -1;
	if (ifindex_a > ifindex_b)
		return 1;
	return 0;
}

static void
get_nl_if_status (ntrack_monitor_t *self, struct _nl_info *nl_info, int ifindex, struct in_addr *in, struct in6_addr *in6)
{
	struct rtnl_addr *addr, *addr6;
	struct nl_object *iter;

	addr = addr6 = NULL;
	iter = nl_cache_get_first (nl_info->addr_cache);
	while (iter) {
		struct rtnl_addr *iter_addr;
		iter_addr = (struct rtnl_addr *) iter;
		if (rtnl_addr_get_ifindex (iter_addr) == ifindex && rtnl_addr_get_family (iter_addr) == AF_INET) {
			addr = iter_addr;
			nl_object_get (iter);
			break;
		} else if (rtnl_addr_get_ifindex (iter_addr) == ifindex && rtnl_addr_get_family (iter_addr) == AF_INET6) {
			addr6 = iter_addr;
			nl_object_get (iter);
			break;
		}
		iter = nl_cache_get_next (iter);
	}

	if (addr) {
		if (in) *in = *((struct in_addr*) nl_addr_get_binary_addr (rtnl_addr_get_local (addr)));
	} else if(addr6) {
		if (in6) *in6 = *((struct in6_addr*) nl_addr_get_binary_addr (rtnl_addr_get_local (addr)));
	}

	nl_object_put ((struct nl_object*) addr);
	nl_object_put ((struct nl_object*) addr6);
}

static struct rtnl_link*
get_nl_link_by_index (ntrack_monitor_t *self, ntrack_list_t *linklist, int iindex)
{
	ntrack_list_t *i = linklist;
	struct rtnl_link *result = NULL;
	while (i) {
		struct rtnl_link *link = i->data;
		if (rtnl_link_get_ifindex(link) == iindex) {
			result = link;
			break;
		}
		if (rtnl_link_get_ifindex(link) > iindex)
			break;
		i = i->next;

		if (i == linklist)
			break;
	}
	return result;
}

static void
update_connectivity (ntrack_monitor_t *self, struct _nl_info *nl_info)
{
	struct rtnl_route *topmost_route = NULL;
	struct rtnl_route *filter_route = rtnl_route_alloc ();
	struct nl_addr *dst_filter = nl_addr_alloc (0);
	struct nl_object *iter;
	ntrack_list_t *link_list = NULL;

	/* fill link info */
	iter = nl_cache_get_first (nl_info->link_cache);
	while (iter) {
		link_list = ntrack_list_insert_sorted (link_list, iter, nl_link_index_cmp);
		nl_object_get (iter);
		iter = nl_cache_get_next (iter);
	}

	/* get topmost route with link that has is up and has carrier */
	nl_addr_set_family (dst_filter, AF_INET);
	nl_addr_set_binary_addr (dst_filter, "\0", 0);
	nl_addr_set_prefixlen (dst_filter, 0);
	rtnl_route_set_dst (filter_route, dst_filter);

	iter = nl_cache_get_first (nl_info->route_cache);
	while (iter) {
		struct rtnl_link *iter_link;
		int route_oif = 0;
		struct in_addr in = { 0 };
#if defined(HAVE_LIBNL2) || defined(HAVE_LIBNL3) || defined (HAVE_LIBNL3_1) || defined (HAVE_LIBNL3_X)
		struct rtnl_nexthop *nexth=0;
		int nhops=0;
#endif
		if (!nl_object_match_filter (iter, OBJ_CAST(filter_route)))
			goto next;

#if defined(HAVE_LIBNL2) || defined(HAVE_LIBNL3) || defined (HAVE_LIBNL3_1) || defined (HAVE_LIBNL3_X)
		nhops = rtnl_route_get_nnexthops ((struct rtnl_route*) iter);
		if (nhops > 0)
			nexth = rtnl_route_nexthop_n ((struct rtnl_route*) iter, 0);
			
		route_oif = rtnl_route_nh_get_ifindex (nexth);
#elif defined(HAVE_LIBNL1)
		route_oif = rtnl_route_get_oif ((struct rtnl_route*) iter);
#else
#error "no libnl"
#endif
		iter_link = get_nl_link_by_index (self, link_list, route_oif);

		/* if we dont have a link for the current oif,
		 * this is not a valid topmost_route */
		if (!iter_link)
			goto next;

		get_nl_if_status (self, nl_info, route_oif, &in, NULL);

		/* interfaces without address are not ready */
		if (in.s_addr == INADDR_ANY)
			goto next;

		/* if new route is lower than the current topmost, continue */
		if (topmost_route && cmp_rtnl_route_metric (topmost_route, iter) <= 0)
			goto next;

		/* if the link of the potential new topmost route is up,
		   we replace the current topmost_route */
		if (rtnl_link_get_flags (iter_link) & IFF_LOWER_UP) {
			/* release ref to current last topmost_route */
			if (topmost_route)
				nl_object_put (OBJ_CAST(topmost_route));
			/* remember current topmost_route candidate */
			topmost_route = (struct rtnl_route*) iter;
			nl_object_get (OBJ_CAST (topmost_route));
		}
	  next:
		iter = nl_cache_get_next (iter);
	}

	/* cleanup the link references */
	ntrack_list_foreach (link_list, (ntrack_list_cb_f*) nl_object_put, NULL);
	ntrack_list_free (link_list);

	nl_addr_put (dst_filter);
	nl_object_put (OBJ_CAST (filter_route));

	if (!nl_info->topmost_route && topmost_route) {
		nl_info->topmost_route = topmost_route;
		op_default_route_appeared ((struct _ntrack_monitor_arch*) self, nl_info);
	} else if (nl_info->topmost_route && !topmost_route) {
		nl_object_put (OBJ_CAST(nl_info->topmost_route));
		nl_info->topmost_route = NULL;
		op_default_route_disappeared ((struct _ntrack_monitor_arch*) self, nl_info);
	} else if (nl_info->topmost_route && topmost_route) {
		int diff_bits;
		if ((diff_bits = (route_obj_ops.oo_id_attrs | ROUTE_ATTR_OIF
		                  | ROUTE_ATTR_GATEWAY) &
		                  nl_object_diff (OBJ_CAST (nl_info->topmost_route),
		                                  OBJ_CAST (topmost_route)))) {
			char diff_str[4096];
			char attr1[4096];
			char attr2[4096];
			nl_object_attrs2str (OBJ_CAST (nl_info->topmost_route),
			                     diff_bits,
			                     diff_str,
			                     4096);
			nl_object_attr_list (OBJ_CAST (nl_info->topmost_route),
			                     attr1,
			                     4096);
			nl_object_attr_list (OBJ_CAST (topmost_route),
			                     attr2,
			                     4096);

			printf ("DIFF_BITS for changed topmost route: %s\n", diff_str);
			printf ("ATTR1 : %s\n", attr1);
			printf ("ATTR2 : %s\n", attr2);

			nl_object_put (OBJ_CAST(nl_info->topmost_route));
			nl_info->topmost_route = topmost_route;
			op_default_route_changed ((struct _ntrack_monitor_arch*) self, nl_info);
		}
	} else {
		/* on first run of update_connectivity we emit a disappeared signal if no
		 * new topmost_route was found; in this way monitors can update their initial
		 * state to OFFLINE */
		if (!nl_info->update_run)
			op_default_route_disappeared ((struct _ntrack_monitor_arch*) self, nl_info);
	}
	/* set to update_run mode so we dont signal disappear if no topmost_route
	 * exists and nothing changed */
	if (!nl_info->update_run)
		nl_info->update_run = TRUE;
}

struct _ntrack_module_functions module_funcs = {
	_ntrack_arch_new,
	_ntrack_arch_get_rfds,
	_ntrack_arch_process_data,
	_ntrack_arch_free
};

#ifdef HAVE_LIBNL1
NTRACK_MODULE_DEFINE("libnl1", "libnl v1", &module_funcs)
#elif defined(HAVE_LIBNL2) 
NTRACK_MODULE_DEFINE("libnl2", "libnl v2", &module_funcs)
#elif defined(HAVE_LIBNL3) 
NTRACK_MODULE_DEFINE("libnl3", "libnl v3", &module_funcs)
#elif defined(HAVE_LIBNL3_1) 
NTRACK_MODULE_DEFINE("libnl3.1", "libnl v3.1", &module_funcs)
#elif defined(HAVE_LIBNL3_X) 
NTRACK_MODULE_DEFINE("libnl3.x", "libnl v3.x", &module_funcs)
#endif

