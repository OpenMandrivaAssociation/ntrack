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
#include "ntrackmonitor.h"

#include <QObject>
#include <QSocketNotifier>

#ifndef __QNTRACK_H__
#define __QNTRACK_H__

/* define Qt style type names for ntrack event and state types */
typedef ntrack_state_t QNTrackState;
typedef ntrack_event_t QNTrackEvent;

class QNtrack : public QObject
{
	Q_OBJECT
public:
	static void init(int *argc, char ***argv);
	static QNtrack* instance();
	QNTrackState networkState();

public slots:
	void socketActivated(int socket);

signals:
	void monitorEvent (QNTrackEvent event);
	void stateChanged (QNTrackState oldState, QNTrackState newState);

private:
	QNtrack();
	~QNtrack();

	void processMonitorEvent (QNTrackEvent event);

	static void monitorCallback (ntrack_monitor_t        *monitor,
	                             ntrack_event_t  event,
	                             ntrackpointer           user_data);

	QNTrackState state;
	ntrack_monitor_t *monitor;
	QSocketNotifier *notifier;
};

class QNtrackListener : public QObject
{
	Q_OBJECT
public:
	QNtrackListener() {}
	virtual ~QNtrackListener() {}

public slots:
	virtual void monitorEvent (QNTrackEvent /* event */) {}
	virtual void stateChanged (QNTrackState /* oldState */, QNTrackState /* newState */) {}

};

#endif /* __QNTRACK_H__ */

