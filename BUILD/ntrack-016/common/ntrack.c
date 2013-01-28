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

#include "ntrackbase.h"
#include "ntrackarch.h"
#include "ntrackarchapi.h"

#include <dirent.h>
#include <dlfcn.h>
#include <error.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef MODULES_DIR
#define MODULES_DIR "/usr/lib/ntrack/modules/"
#endif

static char buf[4][PATH_MAX] = {
	MODULES_DIR,
	"",
	"",
	""
};

static ntrack_monitor_t* _builtin_noop_monitor_new (ntrack_monitor_callback_f callback, ntrackpointer user_data);
static int* _builtin_noop_monitor_get_rfds (ntrack_monitor_t *self);
static int _builtin_noop_monitor_process_data (ntrackpointer self, int* fds);
static void _builtin_noop_monitor_free (ntrackpointer self);

struct _ntrack_module_functions builtin_noop_monitor_funcs =
{
	_builtin_noop_monitor_new,
	_builtin_noop_monitor_get_rfds,
	_builtin_noop_monitor_process_data,
	_builtin_noop_monitor_free
};

struct _ntrack_module builtin_noop_monitor_module = {
	{"builtin-noop-monitor", "Built-in noop monitor (always online)", "<built-in>"},
	&builtin_noop_monitor_funcs
};

void
ntrack_init (int *argc, char ***argv)
{
	char *dirn = dirname ((*argv)[0]);
	int i=0;
#ifdef DEVELOPER_BUILD
	sprintf (buf[i++], "%s/../../modules/.libs/", dirn);
	sprintf (buf[i++], "%s/../../../modules/.libs/", dirn);
#endif
	dirn = MODULES_DIR;
	sprintf (buf[i++], "%s", dirn);
	buf[i++][0] = 0;
}

static struct _ntrack_module module;
static int ftbl_initialized = 0;
static char *module_info_human_banner = 0;

const char*
ntrack_info_human_banner ()
{
	if (!ftbl_initialized)
		return 0;
	return module_info_human_banner;
}


ntrack_monitor_t*
ntrack_arch_new (ntrack_monitor_callback_f callback, ntrackpointer user_data)
{
	DIR *dir;
	struct dirent *dirent;
	void *dlhandle;
	const char *dirpath;
	ntrack_monitor_t *self = 0;
	int i;

	if (ftbl_initialized)
		return (module.ftbl->new) (callback, user_data);

	for (i=0; !self && buf[i] && buf[i][0]; i++) {
		dir = opendir (buf[i]);
		dirpath = buf[i];
		
		while (!self && dir && (dirent = readdir (dir))) {
			char *filepath = malloc (sizeof(char) *
			                         (strlen(dirent->d_name) + strlen(dirpath) + 1));
			sprintf (filepath, "%s%s", dirpath, dirent->d_name);
			dlhandle = dlopen (filepath, RTLD_LAZY | RTLD_LOCAL);
			if (!dlhandle) {
				goto next;
			}

			module = *( (struct _ntrack_module*) dlsym (dlhandle, "ntrack_module"));

			if (module.ftbl)
			{
				self = (module.ftbl->new) (callback, user_data);
				if (self) {
					module.parent.location = filepath;
					ftbl_initialized = 1;
					break;
				}
			}
			memset (&module, '\0', sizeof (struct _ntrack_module));
			dlclose (dlhandle);

		next:
			free(filepath);
		}
		if (dir)
			closedir (dir);
	}

	if (!self) {
		printf ("no backend found: using built-in noop monitor (always online).\n");
		module = builtin_noop_monitor_module;
		self = (module.ftbl->new) (callback, user_data);
		ftbl_initialized = 1;
	}

	#define _ntrack_banner_format "----\n-  ntrack %s - https://launchpad.net/ntrack\n-  %s  %s\n-  with %s (%s)\n----\n"

	module_info_human_banner = malloc (sizeof(char) * (1
	                                   + strlen (VERSION)
	                                   + strlen (COPYRIGHT)
	                                   + strlen (AUTHOR)
	                                   + strlen (module.parent.longname)
	                                   + strlen (module.parent.location)
	                                   + strlen(_ntrack_banner_format)));
	sprintf (module_info_human_banner, _ntrack_banner_format,
	                      VERSION,
	                      COPYRIGHT,
	                      AUTHOR,
	                      module.parent.longname,
	                      module.parent.location);

	#undef _ntrack_banner_format

	return self;
}

int*
ntrack_arch_get_rfds (ntrack_monitor_t *self)
{
	if (module.ftbl->get_rfds)
		return (module.ftbl->get_rfds) (self);
	else
		printf ("no rfds func found, but called\n");

	return 0;
}

int
ntrack_arch_process_data (ntrackpointer self, int* fds)
{
	if (module.ftbl->process_data)
		return (module.ftbl->process_data) (self, fds);
	else
		printf ("no process_data_func found, but called\n");

	return 0;
}

void
ntrack_arch_free (ntrackpointer self)
{
	if (module.ftbl->free)
		(module.ftbl->free) (self);
	else
		printf ("no process_data_func found, but called\n");
	return;
}

static ntrack_monitor_t*
_builtin_noop_monitor_new (ntrack_monitor_callback_f callback, ntrackpointer user_data)
{
	struct _ntrack_monitor_arch *self;
	int rv;

	self = calloc (sizeof (struct _ntrack_monitor_arch), 1);
	self->data = calloc (sizeof (int), 3);

	rv = pipe ((int*) self->data);
	if (rv)
		goto err;

	((int*)self->data)[2] = ((int*)self->data)[1];
	((int*)self->data)[1] = 0;

	((ntrack_monitor_t*)self)->state = NTRACK_STATE_ONLINE;

	return (ntrack_monitor_t*) self;
err:
	free (self);
	return 0;
}

static int*
_builtin_noop_monitor_get_rfds (ntrack_monitor_t *self)
{
	int* rfds = malloc (sizeof(int) * 2);
	return memcpy (rfds, ((struct _ntrack_monitor_arch*) self)->data, sizeof (int) * 2);
}

static int _builtin_noop_monitor_process_data (ntrackpointer self, int* fds)
{
	return 0;
}

static void _builtin_noop_monitor_free (ntrackpointer self)
{
	struct _ntrack_monitor_arch *monitor = (struct _ntrack_monitor_arch*) self;
	int *fdes = (int*) monitor->data;
	close (fdes[0]);
	close (fdes[2]);
	free (monitor->data);
	free (monitor);
}

