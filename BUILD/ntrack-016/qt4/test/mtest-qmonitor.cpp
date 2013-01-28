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
#include <iostream>
#include <QEventLoop>
#include <QCoreApplication>

#include <ntrackutil.h>

#include "QNtrack.h"

class Test : public QNtrackListener
{
public:
	void stateChanged (QNTrackState oldState, QNTrackState newState) {
		std::cout <<
		  "\t=> state changed: " << 
		  ntrack_util_state_to_ascii (oldState) << 
		  " -> " <<
		  ntrack_util_state_to_ascii (newState) <<
		  std::endl;
	}
};

int main(int argc, char **argv)
{
  // We must always have an application
  QCoreApplication a(argc, argv);
  Test *test = new Test();

  QNtrack::init(&argc, &argv);

  QNtrack *monitor = QNtrack::instance();

  std::cout << ntrack_info_human_banner () << std::endl;

  QObject::connect(monitor, SIGNAL (stateChanged (QNTrackState, QNTrackState)),
                   test, SLOT (stateChanged (QNTrackState, QNTrackState)));

  std::cout << "initial state: " << ntrack_util_state_to_ascii (monitor->networkState()) << std::endl;

  std::cout << "\tmanual action needed:" << std::endl;
  std::cout << "\t... please turn on/off your network ..." << std::endl;

  return a.exec();              // And run!
} 
