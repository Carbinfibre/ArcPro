/*
 * ArcPro MMORPG Server
 * Copyright (c) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (c) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (c) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef CREFCOUNTER_HPP
#define CREFCOUNTER_HPP

namespace Arcpro
{
	namespace Shared
	{
		/////////////////////////////////////////////////////////////////////
		//class CRefCounter
		//  Reference Counter class.
		//  Reference counting starts with 1 reference, on instantiation
		//
		//
		/////////////////////////////////////////////////////////////////////
		class SERVER_DECL CRefCounter
		{

			public:
				CRefCounter() { Counter.SetVal(1); }


				virtual ~CRefCounter() {}


				////////////////////////////////////////////////////////////////
				//void AddRef()
				//  Increases the reference count by 1
				//
				//Parameters
				//  None
				//
				//Return Value
				//  None
				//
				//
				////////////////////////////////////////////////////////////////
				void AddRef() { ++Counter; }


				////////////////////////////////////////////////////////////////
				//void DecRef()
				//  Decreases the reference count by 1. When it reaches 0,
				//  the object is deleted
				//
				//Parameters
				//  None
				//
				//Return Value
				//  None
				//
				//
				////////////////////////////////////////////////////////////////
				void DecRef()
				{
					if(--Counter == 0)
						delete this;
				}


			private:
				Arcpro::Threading::AtomicCounter Counter;

		};
	}
}

#endif
