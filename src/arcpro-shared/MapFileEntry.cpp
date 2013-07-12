/*
 * ArcPro MMORPG Server
 * Copyright (C) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (C) 2001 Jani Kajala
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

#include "MapFileEntry.h"
#include <cstring>

MapFileEntry::MapFileEntry()
{
	m_sec = 0;
	m_addr = 0;
	m_len = 0;
	m_name[0] = 0;
}

MapFileEntry::MapFileEntry(long section, long offset, long length, const char* name)
{
	m_sec = section;
	m_addr = offset;
	m_len = length;

	strncpy(m_name, name, MAX_NAME);
	m_name[MAX_NAME] = 0;
}

long MapFileEntry::section() const
{
	return m_sec;
}

long MapFileEntry::offset() const
{
	return m_addr;
}

long MapFileEntry::length() const
{
	return m_len;
}

const char* MapFileEntry::name() const
{
	return m_name;
}

bool MapFileEntry::operator<(const MapFileEntry & other) const
{
	if(m_sec < other.m_sec)
		return true;
	if(m_sec > other.m_sec)
		return false;
	return m_addr < other.m_addr;
}
