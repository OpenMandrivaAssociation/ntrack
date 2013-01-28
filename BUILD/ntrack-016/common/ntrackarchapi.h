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
#include "ntrackmonitor.h"

#ifndef __NTRACK_ARCH_API_H__
#define __NTRACK_ARCH_API_H__

NTRACK_BEGIN_DECLS

#define NTRACK_MODULE_DEFINE(name,longname,function_table) \
	struct _ntrack_module ntrack_module = { \
		{ name, longname, "<module>" }, \
		function_table \
	};

struct _ntrack_module_info
{
	const char *name;
	const char *longname;
	const char *location;
};

struct _ntrack_module_functions
{
	ntrack_monitor_t* (*new) (ntrack_monitor_callback_f callback, ntrackpointer user_data);
	int* (*get_rfds) (ntrack_monitor_t *self);
	int (*process_data) (ntrackpointer self, int* fds);
	void (*free) (ntrackpointer self);
};

struct _ntrack_module
{
	struct _ntrack_module_info parent;
	struct _ntrack_module_functions *ftbl;
};

struct _ntrack_monitor
{
	ntrack_state_t state;
	struct _listener_pack **packs;
	int pack_len;
	int pack_size;
};

struct _ntrack_monitor_arch
{
	ntrack_monitor_t parent;

	ntrackpointer data;

	ntrack_monitor_callback_f cb;
	ntrackpointer cb_user_data;
};

NTRACK_END_DECLS

#endif
