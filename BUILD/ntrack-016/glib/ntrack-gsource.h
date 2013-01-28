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
#include <glib.h>

#include "ntrackbase.h"
#include "ntrack-enums.h"

#ifndef __NTRACK_GLIB_H__
#define __NTRACK_GLIB_H__

G_BEGIN_DECLS

typedef gboolean (*NTrackGlibSourceFunc) (NTrackGlibEvent event, gpointer user_data);

GSource* ntrack_g_source_new ();

guint ntrack_g_source_add (NTrackGlibSourceFunc func, gpointer data);

guint ntrack_g_source_add_full (gint priority,
                                NTrackGlibSourceFunc func,
                                gpointer data,
                                GDestroyNotify notify);

NTrackGlibState ntrack_glib_get_state ();

G_END_DECLS

#endif /* __NTRACK_GLIB_H__ */
