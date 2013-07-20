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

#include "../Common.h"

namespace Arcpro
{
	namespace Threading
	{

		float AtomicFloat::SetVal(float NewValue)
		{
			unsigned long iv = 0;
			float ret = 0.0f;

			iv = *(reinterpret_cast< unsigned long* >(&NewValue));
			ret = *(reinterpret_cast< float* >(Value.SetVal(iv)));

			return ret;
		}


		float AtomicFloat::GetVal()
		{
			float val = 0.0f;

			val = *(reinterpret_cast< float* >(Value.GetVal()));

			return val;
		}
	}
}