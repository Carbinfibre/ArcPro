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

#include "Common.h"
#include "printStackTrace.h"
#include "StackTrace.h"
#include "MapFile.h"
#include <cstdio>
#include <cstring>

/**
 * Prints stack trace to user defined buffer.
 * Always terminates the buffer with 0.
 */
extern SERVER_DECL void printStackTrace(char* buffer, int bufferSize)
{
#if defined(WIN32) && defined(_DEBUG)
	// find out map file name
	char modname[500];
	MapFile::getModuleMapFilename(modname, sizeof(modname));

	// parse map file
	char buf[600];
	MapFile map(modname);
	switch(map.error())
	{
		case MapFile::ERROR_OPEN:
			sprintf(buf, "Failed to open map file %s\n", modname);
			break;
		case MapFile::ERROR_READ:
			sprintf(buf, "Error while reading map file %s(%i)\n", modname, map.line());
			break;
		case MapFile::ERROR_PARSE:
			sprintf(buf, "Parse error in map file %s(%i)\n", modname, map.line());
			break;
		default:
			break;
	}

	// print stack trace to buffer
	if(!map.error())
	{
		MapFile* maps[] = {&map};
		StackTrace::printStackTrace(maps, 1, 1, 16, buf, sizeof(buf));
	}

	// copy to user buffer
	if(bufferSize > 0)
	{
		strncpy(buffer, buf, bufferSize);
		buffer[bufferSize - 1] = 0;
	}
#endif
}

/**
 * Prints stack trace to stdout
 */
extern SERVER_DECL void printStackTrace()
{
#if defined(WIN32) && defined(_DEBUG)
	char buffer[6400];
	printStackTrace(&buffer[0], 6400);
	printf("%s", buffer);
#endif
}

/**
 * Used for assertions
 */
extern SERVER_DECL void arcAssertFailed(const char* fname, int line, const char* expr)
{
	printf("Assertion Failed: (%s)\n", expr);
	printf("Location: %s(%i)\n", fname, line);
	//printf( "Expression: %s\n", expr );
#if defined(WIN32) && defined(_DEBUG)
	printf("Stack trace:\n");
	printStackTrace();
#endif
}