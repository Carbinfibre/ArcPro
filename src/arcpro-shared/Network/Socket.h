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

#ifndef SOCKET_H
#define SOCKET_H

#include "SocketDefines.h"

class SERVER_DECL Socket
{
	public:
		// Constructor. If fd = 0, it will be assigned
		Socket(SOCKET fd, uint32 sendbuffersize, uint32 recvbuffersize);

		// Destructor.
		virtual ~Socket();

		// Open a connection to another machine.
		bool Connect(const char* Address, uint32 Port);

		// Disconnect the socket.
		void Disconnect();

		// Accept from the already-set fd.
		void Accept(sockaddr_in* address);

		/* Implementable methods */

		// Called when data is received.
		virtual void OnRead() {}

		// Called when a connection is first successfully established.
		virtual void OnConnect() {}

		// Called when the socket is disconnected from the client (either forcibly or by the connection dropping)
		virtual void OnDisconnect() {}

		/* Sending Operations */

		// Locks sending mutex, adds bytes, unlocks mutex.
		bool Send(const uint8* Bytes, uint32 Size);

		// Burst system - Locks the sending mutex.
		ARCPRO_INLINE void BurstBegin() { m_writeMutex.Acquire(); }

		// Burst system - Adds bytes to output buffer.
		bool BurstSend(const uint8* Bytes, uint32 Size);

		// Burst system - Pushes event to queue - do at the end of write events.
		void BurstPush();

		// Burst system - Unlocks the sending mutex.
		ARCPRO_INLINE void BurstEnd() { m_writeMutex.Release(); }

		/* Client Operations */

		// Get the client's ip in numerical form.
		string GetRemoteIP();
		ARCPRO_INLINE uint32 GetRemotePort() { return ntohs(m_client.sin_port); }
		ARCPRO_INLINE SOCKET GetFd() { return m_fd; }

		/* Platform-specific methods */

		void SetupReadEvent();
		void ReadCallback(uint32 len);
		void WriteCallback();

		ARCPRO_INLINE bool IsDeleted()
		{
			return m_deleted.GetVal();
		}
		ARCPRO_INLINE bool IsConnected()
		{
			return m_connected.GetVal();
		}
		ARCPRO_INLINE sockaddr_in & GetRemoteStruct() { return m_client; }

		void Delete();

		ARCPRO_INLINE in_addr GetRemoteAddress() { return m_client.sin_addr; }


		CircularBuffer readBuffer;
		CircularBuffer writeBuffer;

	protected:

		// Called when connection is opened.
		void _OnConnect();

		SOCKET m_fd;

		Mutex m_writeMutex;
		Mutex m_readMutex;

		// we are connected? stop from posting events.
		Arcpro::Threading::AtomicBoolean m_connected;

		// We are deleted? Stop us from posting events.
		Arcpro::Threading::AtomicBoolean m_deleted;

		sockaddr_in m_client;

		unsigned long m_BytesSent;
		unsigned long m_BytesRecieved;

	public:
		// Atomic wrapper functions for increasing read/write locks
		ARCPRO_INLINE void IncSendLock() { ++m_writeLock; }
		ARCPRO_INLINE void DecSendLock() { --m_writeLock; }
		ARCPRO_INLINE bool AcquireSendLock()
		{
			if(m_writeLock.SetVal(1) != 0)
				return false;
			else
				return true;
		}

	private:
		// Write lock, stops multiple write events from being posted.
		Arcpro::Threading::AtomicCounter m_writeLock;

		/* Win32 - IOCP Specific Calls */
#ifdef CONFIG_USE_IOCP

	public:

		// Set completion port that this socket will be assigned to.
		ARCPRO_INLINE void SetCompletionPort(HANDLE cp) { m_completionPort = cp; }

		OverlappedStruct m_readEvent;
		OverlappedStruct m_writeEvent;

	private:
		// Completion port socket is assigned to
		HANDLE m_completionPort;

		// Assigns the socket to his completion port.
		void AssignToCompletionPort();

#endif

		/* Linux - EPOLL Specific Calls */
#ifdef CONFIG_USE_EPOLL
	public:
		// Posts a epoll event with the specifed arguments.
		void PostEvent(uint32 events);

		arcpro_INLINE bool HasSendLock()
		{
			bool res;
			res = (m_writeLock.GetVal() != 0);
			return res;
		}
#endif

		/* FreeBSD - kqueue specific calls */
#ifdef CONFIG_USE_KQUEUE
	public:
		// Posts a epoll event with the specifed arguments.
		void PostEvent(int events, bool oneshot);
		arcpro_INLINE bool HasSendLock()
		{
			bool res;
			res = (m_writeLock.GetVal() != 0);
			return res;
		}
#endif

	public:
		// Polls and resets the traffic data
		void PollTraffic(unsigned long* sent, unsigned long* recieved)
		{

			m_writeMutex.Acquire();
			*sent = m_BytesSent;
			*recieved = m_BytesRecieved;
			m_BytesSent = 0;
			m_BytesRecieved = 0;

			m_writeMutex.Release();
		}
};

/** Connect to a server.
* @param hostname Hostname or IP address to connect to
* @param port Port to connect to
* @return templated type if successful, otherwise null
*/
template<class T>
T* ConnectTCPSocket(const char* hostname, u_short port)
{
	sockaddr_in conn;
	hostent* host;

	/* resolve the peer */
	host = gethostbyname(hostname);

	if(!host)
		return NULL;

	/* copy into our address struct */
	memcpy(&conn.sin_addr, host->h_addr_list[0], sizeof(in_addr));
	conn.sin_family = AF_INET;
	conn.sin_port = ntohs(port);

	T* s = new T(0);
	if(!s->Connect(hostname, port))
	{
		s->Delete();
		return 0;
	}
	return s;
}

/* Socket Garbage Collector */
#define SOCKET_GC_TIMEOUT 15

class SocketGarbageCollector : public Singleton<SocketGarbageCollector>
{
		map<Socket*, time_t> deletionQueue;
		Mutex lock;
	public:
		~SocketGarbageCollector()
		{
			map<Socket*, time_t>::iterator i;
			for(i = deletionQueue.begin(); i != deletionQueue.end(); ++i)
				delete i->first;
		}

		void Update()
		{
			map<Socket*, time_t>::iterator i, i2;
			time_t t = UNIXTIME;
			lock.Acquire();
			for(i = deletionQueue.begin(); i != deletionQueue.end();)
			{
				i2 = i++;
				if(i2->second <= t)
				{
					delete i2->first;
					deletionQueue.erase(i2);
				}
			}
			lock.Release();
		}

		void QueueSocket(Socket* s)
		{
			lock.Acquire();
			deletionQueue.insert(map<Socket*, time_t>::value_type(s, UNIXTIME + SOCKET_GC_TIMEOUT));
			lock.Release();
		}
};

#define sSocketGarbageCollector SocketGarbageCollector::getSingleton()

#endif
