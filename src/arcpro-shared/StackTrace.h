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

#ifndef _DEV_STACKTRACE_H
#define _DEV_STACKTRACE_H

class MapFile;

/** Stack tracing utility. */
class StackTrace
{
	public:
		/**
		 * Prints formatted call stack to the user buffer.
		 * Always terminates the user buffer with 0.
		 *
		 * @param map Array of pointers to map files.
		 * @param maps Number of map files.
		 * @param initLevel Number of functions to skip before starting the tracing.
		 * @param maxDepth Maximum number of levels in the stack trace.
		 * @param buffer [out] Output buffer for the formatted stack trace.
		 * @param bufferSize Size of the output buffer.
		 * @return Needed buffer size.
		 */
		static int	printStackTrace(MapFile** map, int maps,
		                            int initLevel, int maxDepth,
		                            char* buffer, int bufferSize);
};

#endif // _DEV_STACKTRACE_H
