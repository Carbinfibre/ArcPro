/*
 * ArcPro MMORPG Server
 * Copyright (C) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (C) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
 * Copyright (C) 2005-2011 MaNGoS <http://getmangos.com/>
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

#ifndef VMAPEXPORT_H
#define VMAPEXPORT_H

enum ModelFlags
{
	MOD_M2 = 1,
	MOD_WORLDSPAWN = 1<<1,
    MOD_HAS_BOUND = 1<<2
};

extern const char * szWorkDirWmo;
extern const char * szRawVMAPMagic;                         // vmap magic string for extracted raw vmap data

#endif
