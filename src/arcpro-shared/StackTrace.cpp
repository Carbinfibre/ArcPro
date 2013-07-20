/*
 * ArcPro MMORPG Server
 * Copyright (c) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (c) 2001 Jani Kajala
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

#include "StackTrace.h"
#include "MapFile.h"
#include "MapFileEntry.h"
#include <cstring>
#include <cstdio>

//-----------------------------------------------------------------------------

#define MAX_DEPTH 32

static long getCaller(int index)
{
#if defined(_DEBUG) && defined(_MSC_VER) && defined(_M_IX86)

	long caller = 0;
	__asm
	{
		mov ebx, ebp
		mov ecx, index
		inc ecx
		xor eax, eax
		StackTrace_getCaller_next:
		mov eax, [ebx+4]
		mov ebx, [ebx]
		dec ecx
		jnz StackTrace_getCaller_next
		mov caller, eax
	}
	return caller;

#else

	return 0;

#endif
}

int StackTrace::printStackTrace(MapFile** map, int maps,
                                int initLevel, int maxDepth,
                                char* buffer, int bufferSize)
{
	if(maxDepth > MAX_DEPTH)
		maxDepth = MAX_DEPTH;

	// list callers
	long callersAddr[MAX_DEPTH];
	int callers = 0;
	int i;
	for(i = initLevel ; i < maxDepth ; ++i)
	{
		long addr = getCaller(i);
		callersAddr[callers++] = addr;

		// end tracing here if the entry is not in a map file
		int entry = -1;
		for(int j = 0 ; j < maps ; ++j)
		{
			entry = map[j]->findEntry(addr);
			if(-1 != entry)
				break;
		}
		if(-1 == entry)
			break;
	}

	// output call stack
	if(bufferSize > 0)
		*buffer = 0;
	int needed = 0;
	for(i = initLevel ; i < callers ; ++i)
	{
		long addr = callersAddr[callers - i - 1];

		// find entry info
		int entry = -1;
		const MapFile* entryMap = NULL;
		for(int j = 0 ; j < maps ; ++j)
		{
			entry = map[j]->findEntry(addr);
			if(-1 != entry)
			{
				entryMap = map[j];
				break;
			}
		}

		// format entry to tempory buf
		char buf[MapFileEntry::MAX_NAME + MAX_DEPTH + 20];	// name + margin + hex number
		buf[0] = 0;
		for(int k = initLevel - 1 ; k < i ; ++k)
			strcat(buf, " ");
		if(!entryMap)
			sprintf(buf + strlen(buf), "0x%x\n", addr);
		else
			sprintf(buf + strlen(buf), "%s (%x)\n", entryMap->getEntry(entry).name(), addr);

		// append temporary buf to output buffer if space left
		needed += (int)strlen(buf);
		if(needed < bufferSize)
			strcat(buffer, buf);
	}

	// terminate output buffer
	if(needed < bufferSize)
		buffer[needed] = 0;
	else if(bufferSize > 0)
		buffer[bufferSize - 1] = 0;
	return needed;
}
