/*
 * ArcPro MMORPG Server
 * Copyright (c) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (c) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
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

#ifndef RWLOCK_H
#define RWLOCK_H

#include "Mutex.h"

class RWLock
{
	public:
		ARCPRO_INLINE void AcquireReadLock()
		{
			_lock.Acquire();
		}

		ARCPRO_INLINE void ReleaseReadLock()
		{
			_lock.Release();
		}

		ARCPRO_INLINE void AcquireWriteLock()
		{
			_lock.Acquire();
		}

		ARCPRO_INLINE void ReleaseWriteLock()
		{
			_lock.Release();
		}

	private:
		Mutex _lock;
};

#endif
