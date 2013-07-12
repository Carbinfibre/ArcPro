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

#ifndef _DEV_MAPFILE_H
#define _DEV_MAPFILE_H


class MapFileEntry;


/**
 * Linker generated module map file parser.
 */
class MapFile
{
	public:
		/** Error code. */
		enum ErrorType
		{
		    /** No error. */
		    ERROR_NONE,
		    /** File open failed. */
		    ERROR_OPEN,
		    /** File reading failed. */
		    ERROR_READ,
		    /** Syntax error. */
		    ERROR_PARSE
		};

		/** Reads a map file. */
		explicit MapFile(const char* filename);

		///
		~MapFile();

		/** Returns preferred load address. */
		long				loadAddress() const;

		/** Returns ith entry from the map file. */
		const MapFileEntry	& getEntry(int i) const;

		/** Returns ith segment from the map file. */
		const MapFileEntry	& getSegment(int i) const;

		/** Returns number of segments in the map file. */
		int					segments() const;

		/** Returns number of entries in the map file. */
		int					entries() const;

		/** Returns error code or 0 (ERROR_NONE) if no error. */
		ErrorType			error() const;

		/** Returns line number of last successful read character. */
		int					line() const;

		/**
		 * Finds entry which contains specified address.
		 * @return Entry index or -1 if not found.
		 */
		int					findEntry(long addr) const;

		/**
		 * Returns current module name, with map extension.
		 * The output buffer is always 0-terminated.
		 */
		static void			getModuleMapFilename(char* buffer, int bufferSize);

	private:
		class MapFileImpl;
		MapFileImpl* m_this;

		MapFile(const MapFile &);
		MapFile & operator=(const MapFile &);
};

#endif // _DEV_MAPFILE_H
