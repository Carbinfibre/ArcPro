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


#ifndef SOCKETMGR_H_WIN32
#define SOCKETMGR_H_WIN32

#ifdef CONFIG_USE_IOCP

class Socket;
class SERVER_DECL SocketMgr : public Singleton<SocketMgr>
{
	public:
		SocketMgr();
		~SocketMgr();

		ARCPRO_INLINE HANDLE GetCompletionPort() { return m_completionPort; }
		void SpawnWorkerThreads();
		void CloseAll();
		void ShowStatus();
		void AddSocket(Socket* s)
		{
			socketLock.Acquire();
			_sockets.insert(s);
			++socket_count;
			socketLock.Release();
		}

		void RemoveSocket(Socket* s)
		{
			socketLock.Acquire();
			_sockets.erase(s);
			--socket_count;
			socketLock.Release();
		}

		void ShutdownThreads();
		long threadcount;

	private:
		HANDLE m_completionPort;
		set<Socket*> _sockets;
		Mutex socketLock;
		Arcpro::Threading::AtomicCounter socket_count;
};

#define sSocketMgr SocketMgr::getSingleton()

typedef void(*OperationHandler)(Socket* s, uint32 len);

class SocketWorkerThread : public ThreadBase
{
	public:
		bool run();
};

void SERVER_DECL HandleReadComplete(Socket* s, uint32 len);
void SERVER_DECL HandleWriteComplete(Socket* s, uint32 len);
void SERVER_DECL HandleShutdown(Socket* s, uint32 len);

static OperationHandler ophandlers[NUM_SOCKET_IO_EVENTS] =
{
	&HandleReadComplete,
	&HandleWriteComplete,
	&HandleShutdown
};

#endif
#endif