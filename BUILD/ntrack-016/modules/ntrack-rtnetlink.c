/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/*
 * Copyright (C) 2011  Alexander Sack <asac@jwsdot.com>
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

/* std libc headers */
#include <assert.h>
#include <error.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* kernel and system headers */
#include <arpa/inet.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/if.h>
#include <net/if_arp.h>

#define __RTA(r) ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(*r))))

#if 0
#define D(...) printf (__VA_ARGS__)
#else
#define D(...) ;
#endif

struct _rtlink
{
	struct ifinfomsg msg;
	char *ifla_name;
};

struct _rtaddr
{
	struct ifaddrmsg msg;

	struct sockaddr_storage *address; /* IFA_ADDRESS */
	struct sockaddr_storage *local;   /* IFA_LOCAL */
	char *label; /* IFA_LABEL */
};

struct _rtroute
{
	struct rtmsg msg;

	int iif_index;
	int oif_index;
	union {
		struct in_addr in4;
		struct in6_addr in6;
	} src_addr, dst_addr;
	int priority;
};

struct _nl_info
{
	int sockfd;
	int topmostr;
};

/* some awful defines - XXX: do better here */
#define __LINKS_LEN 256
#define __LINKS_SIZE (__LINKS_LEN * sizeof (struct _rtlink))
static struct _rtlink links[__LINKS_LEN];
static struct _rtlink prev_links[__LINKS_LEN];
static int links_flushnext = 1;
static int links_count = 0;
static int prev_links_count = 0;
#define __ADD_LINK(a,b) (a[links_count++] = b)
#define __START_LINK_FLUSH() links_flushnext = 1
#define __GET_LINK(a) links[a]
#define __HAS_LINK(a) (a < links_count)

#define __ADDRS_LEN 256
#define __ADDRS_SIZE (__ADDRS_LEN * sizeof (struct _rtaddr))
static struct _rtaddr addrs[__ADDRS_LEN];
static struct _rtaddr prev_addrs[__ADDRS_LEN];
static int addrs_flushnext = 1;
static int addrs_count = 0;
static int prev_addrs_count = 0;
#define __ADD_ADDR(a,b) (a[addrs_count++] = b)
#define __START_ADDR_FLUSH() addrs_flushnext = 1
#define __GET_ADDR(a) addrs[a]
#define __HAS_ADDR(a) (a < addrs_count)

#define __ROUTES_LEN 256
#define __ROUTES_SIZE (__ROUTES_LEN * sizeof (struct _rtroute))
static struct _rtroute routes[__ROUTES_LEN];
static struct _rtroute prev_routes[__ROUTES_LEN];
static int routes_flushnext = 1;
static int routes_count = 0;
static int prev_routes_count = 0;
#define __ADD_ROUTE(a,b) (a[routes_count++] = b)
#define __START_ROUTE_FLUSH() routes_flushnext = 1
#define __GET_ROUTE(a) (routes[a])
#define __HAS_ROUTE(a) (a < routes_count)


static int
spin_miniloop (ntrack_monitor_t *self)
{
	int sr = 0;
	fd_set fds;
	int *rfds;
	const struct timeval tv = { 0, 200 };

	rfds = ntrack_monitor_get_rfds (self);

	/* our mini main loop until we get any update */
	while (sr == 0) {
		struct timeval tv_tmp = tv;
		FD_ZERO (&fds);
		FD_SET (*rfds, &fds);
		sr = select (*rfds + 1, &fds, 0, 0, &tv_tmp);
	}

	free (rfds);

	return sr;
}

static int msg_seq = 0;
static int
flush_links (struct _nl_info *nl_info)
{
	size_t rv_s;
	struct {
		struct nlmsghdr nh;
		struct ifinfomsg msg;
		char attrbuf [128];
	} req;

	__START_LINK_FLUSH();

	memset (&req, 0, sizeof (req));

	req.nh.nlmsg_len = sizeof (req);
	req.nh.nlmsg_type = RTM_GETLINK;
	req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	req.nh.nlmsg_pid = getpid();
	req.nh.nlmsg_seq = ++msg_seq;
	req.msg.ifi_family = ARPHRD_ETHER;

	rv_s = send(nl_info->sockfd, &req, sizeof (req), 0);

	if (rv_s < 0)
		goto err;

	return rv_s;
 err:
	return rv_s;

}

static int
flush_addrs (struct _nl_info *nl_info)
{
	size_t rv_s;
	struct {
		struct nlmsghdr nh;
		struct ifaddrmsg msg;
		char attrbuf [128];
	} req;

	__START_ADDR_FLUSH();

	memset (&req, 0, sizeof (req));

	req.nh.nlmsg_len = sizeof (req);
	req.nh.nlmsg_type = RTM_GETADDR;
	req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	req.nh.nlmsg_pid = getpid();
	req.nh.nlmsg_seq = ++msg_seq;

	rv_s = send(nl_info->sockfd, &req, sizeof (req), 0);

	if (rv_s < 0)
		goto err;

	return rv_s;
 err:
	return rv_s;
}

static int
flush_routes (struct _nl_info *nl_info)
{
	size_t rv_s;
	struct {
		struct nlmsghdr nh;
		struct rtmsg msg;
		char attrbuf [128];
	} req;

	__START_ROUTE_FLUSH();

	memset (&req, 0, sizeof (req));

	req.nh.nlmsg_len = sizeof (req);
	req.nh.nlmsg_type = RTM_GETROUTE;
	req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
	req.nh.nlmsg_pid = getpid();
	req.nh.nlmsg_seq = ++msg_seq;

	rv_s = send(nl_info->sockfd, &req, sizeof (req), 0);

	if (rv_s < 0)
		goto err;

	return rv_s;
 err:
	return rv_s;
}

static void
newlink (struct nlmsghdr *nh)
{
	struct _rtlink link;
	struct rtattr *attr;
	struct ifinfomsg *msg = (struct ifinfomsg *) NLMSG_DATA(nh);
	int bytes = NLMSG_PAYLOAD (nh, sizeof (*msg));
	int i;

	memset (&link, 0, sizeof (struct _rtlink));

	if (links_flushnext) {
		for (i=0; i < links_count; i++)
			free(links[i].ifla_name);

		memcpy (prev_links, links, __LINKS_SIZE );
		memset (links, 0, __LINKS_SIZE);
		prev_links_count = links_count;
		links_count = 0;
		links_flushnext = 0;
	}

	/* copy the ifinfomsg details in one shot */
	link.msg = *msg;

	/* copy only the attributes we are interested in */
	for (attr = IFLA_RTA(msg); RTA_OK(attr, bytes); attr = RTA_NEXT(attr, bytes)) {
		switch (attr->rta_type) {
		case IFLA_IFNAME:
			link.ifla_name = RTA_DATA(attr);
			link.ifla_name = strdup (link.ifla_name);
			break;
		default:
		    /* D ("IFLA_UNKNOWN(0x%x)\n", attr->rta_type); */
			break;
		}
	}

	__ADD_LINK (links, link);
}

static void
newaddr (struct nlmsghdr *nh)
{
	struct _rtaddr addr;
	struct rtattr *attr;
	struct ifaddrmsg *msg = (struct ifaddrmsg *) NLMSG_DATA(nh);
	int bytes = NLMSG_PAYLOAD (nh, sizeof (*msg));
	int i;

	memset (&addr, 0, sizeof (addr));

	if (addrs_flushnext) {
		for (i=0; i < addrs_count; i++)
			free(addrs[i].label);

		memcpy (prev_addrs, addrs, __ADDRS_SIZE );
		memset (addrs, 0, __ADDRS_SIZE);
		prev_addrs_count = addrs_count;
		addrs_count = 0;
		addrs_flushnext = 0;
	}

	/* copy the ifinfomsg details in one shot */
	addr.msg = *msg;

	/* copy only the attributes we are interested in */
	for (attr = __RTA(msg); RTA_OK(attr, bytes); attr = RTA_NEXT(attr, bytes)) {
		switch (attr->rta_type) {
		case IFA_ADDRESS:
			addr.address = RTA_DATA (attr);
			break;
		case IFA_LOCAL:
			addr.local = RTA_DATA (attr);
			break;
		case IFA_LABEL:
			addr.label = strdup (RTA_DATA (attr));
			break;
		default:
			break;
		}
	}

	__ADD_ADDR(addrs, addr);
}

static void
newroute (struct nlmsghdr *nh)
{
	struct _rtroute route;
	struct rtattr *attr;
	struct rtmsg *msg = (struct rtmsg *) NLMSG_DATA(nh);
	int bytes = NLMSG_PAYLOAD (nh, sizeof (*msg));

	memset (&route, 0, sizeof (route));

	if (routes_flushnext) {
		routes_flushnext = 0;
		memcpy (prev_routes, routes, __ROUTES_SIZE );
		memset (routes, 0, __ROUTES_SIZE);
		prev_routes_count = routes_count;
		routes_count = 0;
	}

	/* copy the ifinfomsg details in one shot */
	route.msg = *msg;

	/* copy only the attributes we are interested in */
	for (attr = __RTA(msg); RTA_OK(attr, bytes); attr = RTA_NEXT(attr, bytes)) {
		switch (attr->rta_type) {
		case RTA_IIF:
			route.iif_index = *((int*) RTA_DATA(attr));
			break;
		case RTA_OIF:
			route.oif_index = *((int*) RTA_DATA(attr));
			break;
		case RTA_SRC:
			if (route.msg.rtm_family == AF_INET)
				route.src_addr.in4 = *((struct in_addr*) RTA_DATA(attr));
			else if (route.msg.rtm_family == AF_INET6)
				route.src_addr.in6 = *((struct in6_addr*) RTA_DATA(attr));
			break;
		case RTA_DST:
			if (route.msg.rtm_family == AF_INET)
				route.dst_addr.in4 = *((struct in_addr*) RTA_DATA(attr));
			else if (route.msg.rtm_family == AF_INET6)
				route.dst_addr.in6 = *((struct in6_addr*) RTA_DATA(attr));
			break;
		case RTA_PRIORITY:
			route.priority = *((int*) RTA_DATA(attr));
			break;
		default:
			break;
		}
	}

	__ADD_ROUTE(routes, route);
}

static int
recv_msgs (struct _nl_info *nl_info)
{
	int len;
	char buf[4096];
	int incremental = 0;
	struct iovec iov;
	struct sockaddr_nl sa;
	struct msghdr msg;
	struct nlmsghdr *nh;
	struct nlmsgerr *err;

	iov.iov_base =  buf;
	iov.iov_len = sizeof(buf);

	msg.msg_name=(void *)&sa;
	msg.msg_namelen = sizeof(sa);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = 0;

	while ((len = recvmsg(nl_info->sockfd, &msg, O_NONBLOCK)) >0) {

		for (nh = (struct nlmsghdr *) buf;
			 NLMSG_OK (nh, len);
			 nh = NLMSG_NEXT (nh, len)) {

			if (!nh->nlmsg_seq)
				incremental = 1;

			switch (nh->nlmsg_type) {
				/* end of multipart */
			case NLMSG_DONE:
				goto done;
			case NLMSG_ERROR:
				D (">>>> ERROR\n");
				err = NLMSG_DATA (nh);
				goto err;
				break;
			case RTM_NEWLINK:
				if (!incremental) newlink (nh); else incremental++;
				break;
			case RTM_NEWADDR:
				if (!incremental) newaddr (nh); else incremental++;
				break;
			case RTM_NEWROUTE:
				if (!incremental) newroute (nh); else incremental++;
				break;
			case RTM_DELLINK:
			case RTM_DELADDR:
			case RTM_DELROUTE:
				incremental++;
				break;
			default:
				D ("OTHER msg type: %d\n", nh->nlmsg_type);
				break;
			}
			/* Continue with parsing payload. */
		}
	}
	return incremental;
 done:
	return 0;

 err:
	return err->error;
}

static int
reflush (ntrack_monitor_t *monitor, struct _nl_info *nl_info)
{
	int rv;

	/* initial flushing */
	rv = flush_links (nl_info);
	if (rv < 0)
		goto err;

	/* spin to get initial state */
	rv = spin_miniloop (monitor);
	while (rv >= 0) {
		rv = recv_msgs (nl_info);
		if (rv <= 0)
			break;
		rv = spin_miniloop (monitor);
	}
	if (rv < 0)
		goto err;	

	if (flush_addrs (nl_info) < 0)
		goto err;

	/* spin to get initial state */
	rv = spin_miniloop (monitor);
	while (rv >= 0) {
		rv = recv_msgs (nl_info);
		if (rv <= 0)
			break;
		rv = spin_miniloop (monitor);
	}
	if (rv < 0)
		goto err;

	if (flush_routes (nl_info) < 0)
		goto err;

	/* spin to get initial state */
	rv = spin_miniloop (monitor);
	while (rv >= 0) {
		rv = recv_msgs (nl_info);
		if (rv <= 0)
			break;
		rv = spin_miniloop (monitor);
	}
	if (rv < 0)
		goto err;

	return rv;
 err:
	printf ("error creating rtnetlink monitor: %s\n", strerror(-rv));
	return rv;
}

static void
sort_routes_by_prio ()
{
	int i;

	for (i = 0; i < routes_count; i++) {
		struct _rtroute irt = __GET_ROUTE (i);
		int j;
		for (j = i + 1; j < routes_count; j++) {
			struct _rtroute jrt = __GET_ROUTE (j);
			if (jrt.priority > irt.priority) {
				routes[i] = routes[j];
				routes[j] = irt;
				irt = jrt;
			}
		}
	}
}

static void
dump_addr (struct _rtaddr addr)
{
	D ("ADDR => ");
	switch (addr.msg.ifa_family) {
	case AF_INET:
		D ("AF_INET::");
		break;
	case AF_INET6:
		D ("AF_INET6::");
		break;
	default:
		D ("AF_%d::", addr.msg.ifa_family);
	}

	D ("label=%s::", addr.label);	
	D ("ifa_prefixlen=%d::", addr.msg.ifa_prefixlen);
	D ("ifa_flags=%d::", addr.msg.ifa_flags);
	D ("ifa_index=%d::", addr.msg.ifa_index);

	switch(addr.msg.ifa_scope) {
	case RT_SCOPE_UNIVERSE:
		D ("ifa_scope=RT_SCOPE_UNIVERSE::");
		break;
	case RT_SCOPE_SITE:
		D ("ifa_scope=RT_SCOPE_SITE::");
		break;
	case RT_SCOPE_LINK:
		D ("ifa_scope=RT_SCOPE_LINK::");
		break;
	case RT_SCOPE_HOST:
		D ("ifa_scope=RT_SCOPE_HOST::");
		break;
	case RT_SCOPE_NOWHERE:
		D ("ifa_scope=RT_SCOPE_NOWHERE::");
		break;
	default:
		D ("ifa_scope=RT_SCOPE_UNKNOWN(%d)::", addr.msg.ifa_scope);
		break;
	}
	D ("\n");
}

static void
dump_link (struct _rtlink link)
{
	D ("LINK => ");
	switch (link.msg.ifi_type) {
	case ARPHRD_ETHER:
		D ("ARPHRD_ETHER::");
		break;
	case ARPHRD_PPP:
		D ("ARPHRD_PPP::");
		break;
	case ARPHRD_LOOPBACK:
		D ("ARPHRD_LOOPBACK::");
		break;
	default:
		D ("ARPHRD_0x%x::", link.msg.ifi_type);
	}

	D ("ifla_name=%s::", link.ifla_name);
	D ("ifi_family=0x%x::", link.msg.ifi_family);
	D ("ifi_flags=0x%x::", link.msg.ifi_flags);
	D ("ifi_index=%d::", link.msg.ifi_index);
	D ("ifi_change=%d::", link.msg.ifi_change);

	D ("\n");
}

static void
dump_route (struct _rtroute route)
{
	char outbuf[512];
	if (1 || outbuf[0])
		;

	D ("ROUTE => ");
	switch (route.msg.rtm_family) {
	case AF_INET:
		D ("AF_INET::");
		break;
	case AF_INET6:
		D ("AF_INET6::");
		break;
	default:
		D ("AF_0x%x::", route.msg.rtm_family);
	}

	D ("rtm_table=");
	switch (route.msg.rtm_table) {
	case RT_TABLE_UNSPEC:
		D ("RT_TABLE_UNSPEC::");
		break;
	case RT_TABLE_DEFAULT:
		D ("RT_TABLE_DEFAULT::");
		break;
	case RT_TABLE_MAIN:
		D ("RT_TABLE_MAIN::");
		break;
	case RT_TABLE_LOCAL:
		D ("RT_TABLE_LOCAL");
		break;
	default:
		D ("RT_TABLE_UNKNOWN(%d)::",route.msg.rtm_table);		
		break;
	}

	D ("rtm_protocol=");
	switch (route.msg.rtm_protocol) {
	case RTPROT_UNSPEC:
		D ("RTPROT_UNSPEC::");
		break;
	case RTPROT_REDIRECT:
		D ("RTPROT_REDIRECT::");
		break;
	case RTPROT_KERNEL:
		D ("RTPROT_KERNEL::");
		break;
	case RTPROT_BOOT:
		D ("RTPROT_BOOT::");
		break;
	case RTPROT_STATIC:
		D ("RTPROT_STATIC::");
		break;
	default:
		D ("RTPROT_UNKNOWN(%d)::",route.msg.rtm_protocol);		
		break;
	}

	D ("rtm_scope=");
	switch (route.msg.rtm_scope) {
	case RT_SCOPE_UNIVERSE:
		D ("RT_SCOPE_UNIVERSE::");
		break;
	case RT_SCOPE_SITE:
		D ("RT_SCOPE_SITE::");
		break;
	case RT_SCOPE_LINK:
		D ("RT_SCOPE_LINK::");
		break;
	case RT_SCOPE_HOST:
		D ("RT_SCOPE_HOST::");
		break;
	case RT_SCOPE_NOWHERE:
		D ("RT_SCOPE_NOWHERE::");
		break;
	default:
		D ("RT_SCOPE_UNKNOWN(%d)::",route.msg.rtm_scope);		
		break;
	}

	D ("rtm_flags=");
	switch (route.msg.rtm_flags) {
	case RTM_F_NOTIFY:
		D ("RTM_F_NOTIFY::");
		break;
	case RTM_F_CLONED:
		D ("RTM_F_CLONED::");
		break;
	case RTM_F_EQUALIZE:
		D ("RTM_F_EQUALIZE::");
		break;
	default:
		D ("RTM_F_UNKNOWN(%d)::",route.msg.rtm_flags);		
		break;
	}

	D ("rtm_src_len=%d::", route.msg.rtm_src_len);
	D ("rtm_dst_len=%d::", route.msg.rtm_dst_len);
	D ("dst_address=%s::", inet_ntop (route.msg.rtm_family, &route.dst_addr, outbuf, sizeof (outbuf)));

	D ("priority=%d::", route.priority);
	D ("iif_index=%d::", route.iif_index);
	D ("oif_index=%d::", route.oif_index);
	
	D ("src_address=%s::", inet_ntop (route.msg.rtm_family, &route.src_addr, outbuf, sizeof (outbuf)));
	D ("dst_address=%s::", inet_ntop (route.msg.rtm_family, &route.dst_addr, outbuf, sizeof (outbuf)));

	D ("\n");
}

static void
op_default_route_appeared (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info)
{
	D ("op_default_route_appeared\n");
	if (self->cb)
		self->cb ((ntrack_monitor_t*) self, NTRACK_EVENT_CONNECT, self->cb_user_data);
}

static void
op_default_route_disappeared (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info)
{
	D ("op_default_route_disappeared\n");
	if (self->cb)
		self->cb ((ntrack_monitor_t*) self, NTRACK_EVENT_DISCONNECT, self->cb_user_data);
}

static void
op_default_route_changed (struct _ntrack_monitor_arch *self, struct _nl_info *nl_info)
{
	D ("op_default_route_changed\n");
	if (self->cb)
		self->cb ((ntrack_monitor_t*) self, NTRACK_EVENT_RECONNECT, self->cb_user_data);
}

static int
calc_connectivity (ntrack_monitor_t *monitor, struct _nl_info *nl_info)
{
	struct _rtroute topr4[__ROUTES_LEN];
	struct _rtaddr topa4[__ADDRS_LEN];
	struct _rtlink topl4[__LINKS_LEN];

	struct _rtroute topr6[__ROUTES_LEN];
	struct _rtaddr topa6[__ADDRS_LEN];
	struct _rtlink topl6[__LINKS_LEN];

	int ipstat4[__ROUTES_LEN];
	int ipstat6[__ROUTES_LEN];

	int i;

	int topr4_count = 0;
	int topa4_count = 0;
	int topl4_count = 0;

	int topr6_count = 0;
	int topa6_count = 0;
	int topl6_count = 0;


	memset (&topr4, 0, sizeof (topr4));
	memset (&topa4, 0, sizeof (topa4));
	memset (&topl4, 0, sizeof (topl4));

	memset (&topr6, 0, sizeof (topr6));
	memset (&topa6, 0, sizeof (topa6));
	memset (&topl6, 0, sizeof (topl6));

	sort_routes_by_prio ();

	i = 0;
	while (__HAS_ROUTE (i)) {
		struct _rtroute route;

		route = __GET_ROUTE (i++);
		dump_route (route);

		if (route.msg.rtm_table == RT_TABLE_MAIN &&
			route.msg.rtm_dst_len == 0 &&
			route.msg.rtm_family == AF_INET)
			topr4[topr4_count++] = route;

		if (route.msg.rtm_table == RT_TABLE_MAIN &&
			route.msg.rtm_dst_len == 0 &&
			route.msg.rtm_family == AF_INET6)
			topr6[topr6_count++] = route;
	}

	i = 0;
	while (__HAS_LINK (i)) {
		struct _rtlink link;
		int ii;
		link = __GET_LINK (i++);
		dump_link (link);
		for (ii = 0; ii < topr4_count; ii++) {
			if (link.msg.ifi_index == topr4[ii].oif_index) {
				topl4[topl4_count++] = link;
				break;
			}
		}
		for (ii = 0; ii < topr6_count; ii++) {
			if (link.msg.ifi_index == topr6[ii].oif_index) {
				topl6[topl6_count++] = link;
				break;
			}
		}
	}

	i = 0;
	while (__HAS_ADDR (i)) {
		struct _rtaddr addr;
		int ii;

		addr = __GET_ADDR (i++);
		dump_addr (addr);
		for (ii = 0; ii < topr4_count; ii++) {
			if (addr.msg.ifa_index == topr4[ii].oif_index) {
				topa4[topa4_count++] = addr;
				break;
			}
		}
		for (ii = 0; ii < topr6_count; ii++) {
			if (addr.msg.ifa_index == topr6[ii].oif_index) {
				topa6[topa6_count++] = addr;
				break;
			}
		}
	}

	for (i = 0; i < topr4_count; i++) {
		if ((topl4[i].msg.ifi_flags & IFF_LOWER_UP) &&
			(topa4[i].msg.ifa_index > 0)) {
			/* has interface and carrier, online */
			ipstat4[i] = NTRACK_STATE_ONLINE;
		}	
		if (!(topl4[i].msg.ifi_flags & IFF_LOWER_UP) &&
			(topa4[i].msg.ifa_index > 0)) {
			/* no carrier on link, blocked */
			ipstat4[i] = NTRACK_STATE_ONLINE;
		}	
		if (!(topl4[i].msg.ifi_flags & IFF_LOWER_UP) &&
			!(topl4[i].msg.ifi_flags & IFF_UP) &&
			(topa4[i].msg.ifa_index > 0)) {
			/* no link, offline */
			ipstat4[i] = NTRACK_STATE_OFFLINE;
		}	
	}

	ipstat6[0] = NTRACK_STATE_OFFLINE;
	for (i = 0; i < topr6_count; i++) {
		ipstat6[i] = NTRACK_STATE_UNKNOWN;

		if ((topl6[i].msg.ifi_flags & IFF_LOWER_UP) &&
			(topa6[i].msg.ifa_index > 0)) {
			/* has interface and carrier, online */
			ipstat6[i] = NTRACK_STATE_ONLINE;
		}	
		if (!(topl6[i].msg.ifi_flags & IFF_LOWER_UP) &&
			(topa6[i].msg.ifa_index > 0)) {
			/* no carrier on link, blocked */
			ipstat6[i] = NTRACK_STATE_BLOCKED;
		}	
		if (!(topl6[i].msg.ifi_flags & IFF_LOWER_UP) &&
			(topa6[i].msg.ifa_index == 0)) {
			/* no link, offline */
			ipstat6[i] = NTRACK_STATE_OFFLINE;
		}	
	}

	for (i = 0; i < topr4_count; i++) {
		if (ipstat4[i] != NTRACK_STATE_BLOCKED)
			break;
	}

	if (topr4_count > 0 && ipstat4[i] == NTRACK_STATE_UNKNOWN) {
		return -1;
	}

	D ("tmr_1: %d, i: %d, ipstat4: %d\n", nl_info->topmostr, i, ipstat4[i]);

	if (nl_info->topmostr < 0 && topr4_count > 0 && ipstat4[i] != NTRACK_STATE_OFFLINE) {
		nl_info->topmostr = topl4[i].msg.ifi_index;
		op_default_route_appeared ((struct _ntrack_monitor_arch *) monitor, nl_info);	 
	}

	if ((nl_info->topmostr == -2 || nl_info->topmostr >=0) && (topr4_count == 0 || ipstat4[i] == NTRACK_STATE_OFFLINE)) {
		nl_info->topmostr = -1;
		op_default_route_disappeared ((struct _ntrack_monitor_arch *) monitor, nl_info);	 
	}

	if (nl_info->topmostr >=0 && topr4_count > 0 && topl4[i].msg.ifi_index != nl_info->topmostr && ipstat4[i] != NTRACK_STATE_OFFLINE) {
		nl_info->topmostr = topl4[i].msg.ifi_index;
		op_default_route_changed ((struct _ntrack_monitor_arch *) monitor, nl_info);	 
	}

	D ("tmr_2: %d, i: %d, ipstat4: %d\n", nl_info->topmostr, i, ipstat4[i]);

	for (i = 0; i < topr6_count; i++) {
		if (ipstat6[i] == NTRACK_STATE_BLOCKED)
			continue;
	}

	D ("connectivity: %d = %d\n", ipstat4[i],topr4_count);
	 
	return topr4_count == 0 ? NTRACK_STATE_OFFLINE : ipstat4[nl_info->topmostr];
}

static int
setup_socket (struct _nl_info *nl_info)
{
	struct sockaddr_nl addr;
	int rv;

	nl_info->sockfd = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);

	if (nl_info->sockfd < 0)
		return nl_info->sockfd;

	rv = fcntl (nl_info->sockfd , F_GETFL);
	rv = fcntl (nl_info->sockfd, F_SETFL, rv | O_NONBLOCK);
	if (rv < 0)
		return rv;

	memset(&addr, 0, sizeof(addr));
	addr.nl_family = AF_NETLINK;
	addr.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE |
		RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;

	rv = bind(nl_info->sockfd, (struct sockaddr *) &addr, sizeof(addr));

	return rv;
}

static ntrack_monitor_t*
_ntrack_arch_new (ntrack_monitor_callback_f callback, ntrackpointer user_data)
{
	int rv;
	struct _ntrack_monitor_arch *arch = calloc(sizeof(struct _ntrack_monitor_arch), 1);
	struct _nl_info *nl_info = calloc (sizeof (struct _nl_info), 1);

	/* setup ntrack_monitor_t part */
	((ntrack_monitor_t *)arch)->state = NTRACK_STATE_UNKNOWN;

	/* setup struct ntrack_monitor_arch part */
	arch->data = nl_info;
	arch->cb = callback;
	arch->cb_user_data = user_data;

	nl_info->topmostr = -2;

	rv = setup_socket (nl_info);
	if (rv < 0)
		return NULL;

	rv = reflush ((ntrack_monitor_t*) arch, nl_info);

	if (rv < 0)
		goto err;

	calc_connectivity ((ntrack_monitor_t*) arch, nl_info);

	return (ntrack_monitor_t*) arch;

 err:
	printf ("error creating rtnetlink monitor: %s\n", strerror(-rv));
	return NULL;
}

static int*
_ntrack_arch_get_rfds (ntrack_monitor_t *self)
{
	struct _ntrack_monitor_arch *arch = (struct _ntrack_monitor_arch*) self;
	int* fds = calloc (sizeof(int), 2);
	struct _nl_info *info = arch->data;
	/*
	 * just the smart_update_handle is used to get smart
	 * updates hint from the application mainloop.
	 */
	fds[0] = info->sockfd;

	return fds;
}

static int
_ntrack_arch_process_data (ntrackpointer self, int* fds)
{
	int rv = -1;
	struct _ntrack_monitor_arch *arch = self;
	struct _nl_info *nl_info = arch->data;

	rv = recv_msgs (nl_info);

	if (rv < 0)
		goto err;

	rv = reflush ((ntrack_monitor_t*) arch, nl_info);

	if (rv < 0)
		goto err;

	calc_connectivity ((ntrack_monitor_t*) arch, nl_info);
 
 err:
	return rv;
}

static void
_ntrack_arch_free (ntrackpointer self)
{
	struct _ntrack_monitor_arch *arch = self;
	struct _nl_info *nl_info = arch->data;

	if (nl_info->sockfd)
		close (nl_info->sockfd);

	free (nl_info);
	free (self);
}

/* module struct definition */
struct _ntrack_module_functions module_funcs = {
	_ntrack_arch_new,
	_ntrack_arch_get_rfds,
	_ntrack_arch_process_data,
	_ntrack_arch_free
};

/* register module */
NTRACK_MODULE_DEFINE("rtnetlink", "rtnetlink ntrack module", &module_funcs)
