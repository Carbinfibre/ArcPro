/*
 * ArcPro MMORPG Server
 * Copyright (C) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (C) 2007-2010 Burlex <burlex@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include "../Log.h"
#include "CircularBuffer.h"
#include "SocketDefines.h"
#include "SocketOps.h"
#include "Socket.h"

#ifdef CONFIG_USE_IOCP
#include "SocketMgrWin32.h"
#include "ListenSocketWin32.h"
#endif

#ifdef CONFIG_USE_EPOLL
#include "SocketMgrLinux.h"
#include "ListenSocketLinux.h"
#endif

#ifdef CONFIG_USE_KQUEUE
#include "SocketMgrFreeBSD.h"
#include "ListenSocketFreeBSD.h"
#endif

#endif
