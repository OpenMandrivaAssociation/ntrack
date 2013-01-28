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
#include "QNtrack.h"
#include "ntrackmonitor.h"

#include <iostream>

void
QNtrack::init(int *argc, char ***argv)
{
	ntrack_init (argc, argv);
}

QNtrack*
QNtrack::instance()
{
	static QNtrack* singleton = 0;

	if (!singleton)
		singleton = new QNtrack();

	return singleton;
}

QNtrack::QNtrack()
{
	int *rfds;
	this->monitor = ntrack_monitor_get();
	rfds = ntrack_monitor_get_rfds (this->monitor);
	this->state = ntrack_monitor_get_state (this->monitor);
	this->notifier = new QSocketNotifier (rfds[0], QSocketNotifier::Read);
	QObject::connect(this->notifier, SIGNAL(activated(int)), this, SLOT(socketActivated(int)));
	ntrack_monitor_register (this->monitor, QNtrack::monitorCallback, this);
}

QNtrack::~QNtrack()
{
	ntrack_monitor_put (this->monitor);
	this->monitor = 0;
}

QNTrackState
QNtrack::networkState()
{
	return this->state;
}

void
QNtrack::socketActivated (int socket)
{
	int rfds[] = { socket, 0 };
	ntrack_monitor_process_data (this->monitor, rfds);
}

void QNtrack::processMonitorEvent (QNTrackEvent event)
{
	QNTrackState oldState = this->state;
	this->state = ntrack_monitor_get_state (this->monitor);
	if (oldState != this->state) {
		emit stateChanged (oldState , this->state);
	}

	emit monitorEvent (event);
}

void
QNtrack::monitorCallback (ntrack_monitor_t        *monitor,
                          ntrack_event_t  event,
                          ntrackpointer           user_data)
{
	QNtrack *self = (QNtrack*) user_data;
	self->processMonitorEvent(event);
}

