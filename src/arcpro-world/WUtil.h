/*
 * ArcPro MMORPG Server
 * Copyright (C) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (C) 2005-2007 Ascent Team <http://www.ascentemu.com/>
 * Copyright (C) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
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

#ifndef ARCWORLDUTILS__H
#define ARCWORLDUTILS__H

namespace Arcpro
{

	// Common Arcpro world stuff
	class SERVER_DECL Util
	{
		public:

			static uint32 GUID_LOPART(uint64 GUID);
			static uint32 GUID_HIPART(uint64 GUID);
			static uint32 GET_CREATURE_ENTRY_FROM_GUID(uint64 guid);
			static void ArcproAssert(bool condition);
			static uint64 MAKE_PET_GUID(uint32 entry, uint32 lowGUID);
			static uint64 MAKE_ITEM_GUID(uint32 lowGUID);
			static uint32 MAKE_UNIT_ACTION_BUTTON( uint32 spell, uint32 unk );
			static uint32 MAKE_GAME_TIME();
	};
}
#define ARCPRO_ASSERT( EXPR ) Arcpro::Util::ArcproAssert( EXPR ); ANALYSIS_ASSUME( EXPR )

#endif
