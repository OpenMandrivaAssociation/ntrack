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

#ifndef __NTRACK_MACROS_H__
#define __NTRACK_MACROS_H__

#ifdef  __cplusplus
#define NTRACK_BEGIN_DECLS  extern "C" {
#define NTRACK_END_DECLS    }
#else
#define NTRACK_BEGIN_DECLS
#define NTRACK_END_DECLS
#endif


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#endif /* __NTRACK_MACROS_H__ */
