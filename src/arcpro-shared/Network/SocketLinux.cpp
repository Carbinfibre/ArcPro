/*
 * ArcPro MMORPG Server
 * Copyright (c) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (c) 2007-2010 Burlex <burlex@gmail.com>
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

#include "Network.h"
#ifdef CONFIG_USE_EPOLL

void Socket::PostEvent(uint32 events)
{
	int epoll_fd = sSocketMgr.GetEpollFd();

	struct epoll_event ev;
	memset(&ev, 0, sizeof(epoll_event));
	ev.data.fd = m_fd;
	ev.events = events | EPOLLET;			/* use edge-triggered instead of level-triggered because we're using nonblocking sockets */

	// post actual event
	if(epoll_ctl(epoll_fd, EPOLL_CTL_MOD, ev.data.fd, &ev))
		Log.Warning("epoll", "Could not post event on fd %u", m_fd);
}

void Socket::ReadCallback(uint32 len)
{
	if(IsDeleted() || !IsConnected())
		return;

	// We have to lock here.
	m_readMutex.Acquire();

	size_t space = readBuffer.GetSpace();
	int bytes = recv(m_fd, readBuffer.GetBuffer(), space, 0);
	if(bytes <= 0)
	{
		m_readMutex.Release();
		Disconnect();
		return;
	}
	else if(bytes > 0)
	{
		//m_readByteCount += bytes;
		readBuffer.IncrementWritten(bytes);
		// call virtual onread()
		OnRead();
	}
	m_BytesRecieved += bytes;

	m_readMutex.Release();
}

void Socket::WriteCallback()
{
	if(IsDeleted() || !IsConnected())
		return;

	// We should already be locked at this point, so try to push everything out.
	int bytes_written = send(m_fd, writeBuffer.GetBufferStart(), writeBuffer.GetContiguiousBytes(), 0);
	if(bytes_written < 0)
	{
		// error.
		Disconnect();
		return;
	}
	m_BytesSent += bytes_written;

	//RemoveWriteBufferBytes(bytes_written, false);
	writeBuffer.Remove(bytes_written);
}

void Socket::BurstPush()
{
	if(AcquireSendLock())
		PostEvent(EPOLLOUT);
}

#endif
