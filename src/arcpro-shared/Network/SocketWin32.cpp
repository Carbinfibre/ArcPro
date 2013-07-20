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
#ifdef CONFIG_USE_IOCP

void Socket::WriteCallback()
{
	if(IsDeleted() || !IsConnected())
		return;

	//printf("\nSocket::Writecallback(): sendsize : %u\n", this->m_writeByteCount);
	// We don't want any writes going on while this is happening.
	m_writeMutex.Acquire();
	if(writeBuffer.GetContiguiousBytes())
	{
		DWORD w_length = 0;
		DWORD flags = 0;

		// attempt to push all the data out in a non-blocking fashion.
		WSABUF buf;
		buf.len = (u_long)writeBuffer.GetContiguiousBytes();
		buf.buf = (char*)writeBuffer.GetBufferStart();

		/*OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_EVENT_WRITE_END);
		int r = WSASend(m_fd, &buf, 1, &w_length, flags, &ov->m_overlap, 0);
		if(r == SOCKET_ERROR)
		{
			if(WSAGetLastError() != WSA_IO_PENDING)
			{
				DecSendLock();
				Disconnect();
			}
		}*/

		m_writeEvent.Mark();
		m_writeEvent.Reset(SOCKET_IO_EVENT_WRITE_END);
		int r = WSASend(m_fd, &buf, 1, &w_length, flags, &m_writeEvent.m_overlap, 0);
		if(r == SOCKET_ERROR)
		{
			int wsaerror = WSAGetLastError();

			if(wsaerror != WSA_IO_PENDING)
			{
				sLog.outError("WSAGetLastError() = %d on socket %u", wsaerror, m_fd);

				m_writeEvent.Unmark();
				DecSendLock();
				Disconnect();
			}
		}
		m_BytesSent += w_length;
	}
	else
	{
		// Write operation is completed.
		DecSendLock();
	}
	m_writeMutex.Release();
}

void Socket::SetupReadEvent()
{
	if(IsDeleted() || !IsConnected())
		return;

	m_readMutex.Acquire();
	DWORD r_length = 0;
	DWORD flags = 0;
	WSABUF buf;
	buf.len = (u_long)readBuffer.GetSpace();
	buf.buf = (char*)readBuffer.GetBuffer();

	// event that will trigger after data is receieved
	/*OverlappedStruct * ov = new OverlappedStruct(SOCKET_IO_EVENT_READ_COMPLETE);

	if(WSARecv(m_fd, &buf, 1, &r_length, &flags, &ov->m_overlap, 0) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
			Disconnect();
	}*/

	m_readEvent.Mark();
	m_readEvent.Reset(SOCKET_IO_EVENT_READ_COMPLETE);
	if(WSARecv(m_fd, &buf, 1, &r_length, &flags, &m_readEvent.m_overlap, 0) == SOCKET_ERROR)
	{
		if(WSAGetLastError() != WSA_IO_PENDING)
		{
			m_readEvent.Unmark();
			Disconnect();
		}
	}
	m_BytesRecieved += r_length;
	//m_readEvent = ov;
	m_readMutex.Release();
}

void Socket::ReadCallback(uint32 len)
{
	readBuffer.IncrementWritten(len);
	OnRead();
	SetupReadEvent();
}

void Socket::AssignToCompletionPort()
{
	/*HANDLE h = */CreateIoCompletionPort((HANDLE)m_fd, m_completionPort, (ULONG_PTR)this, 0);
	//__asm int 3;
}

void Socket::BurstPush()
{
	if(AcquireSendLock())
		WriteCallback();
}

#endif
