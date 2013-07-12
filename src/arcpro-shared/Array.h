/*
 * ArcPro MMORPG Server
 * Copyright (C) 2011-2013 ArcPro Speculation <http://arcpro.sexyi.am/>
 * Copyright (C) 2008-2013 ArcEmu Team <http://www.arcemu.org/>
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

#ifndef _DEV_ARRAY_H
#define _DEV_ARRAY_H

/** Very simple dynamic array. */
template <class T> class Array
{
	public:
		/** Creates an empty array. */
		Array() :
			m_data(0), m_len(0), m_cap(0)
		{
		}

		/** Creates an array of specified size. */
		explicit Array(int size) :
			m_data(0), m_len(0), m_cap(0)
		{
			setSize(size);
		}

		///
		~Array()
		{
			delete[] m_data;
		}

		/** Appends an item at the end of the array. */
		void add(const T & item)
		{
			if(m_len + 1 > m_cap)
				setCapacity(m_len + 1);
			m_data[m_len++] = item;
		}

		/** Resizes the array. */
		void setSize(int size)
		{
			if(size > m_cap)
				setCapacity(size);
			m_len = size;
		}

		/** Returns ith item. */
		T & operator[](int i)
		{
			return m_data[i];
		}

		/** Returns pointer to the first element in the vector. */
		T* begin()
		{
			return m_data;
		}

		/** Returns pointer to one beyond the last element in the vector. */
		T* end()
		{
			return m_data + m_len;
		}

		/** Returns number of items in the array. */
		int size() const
		{
			return m_len;
		}

		/** Returns ith item. */
		const T & operator[](int i) const
		{
			return m_data[i];
		}

		/** Returns pointer to the first element in the vector. */
		const T* begin() const
		{
			return m_data;
		}

		/** Returns pointer to one beyond the last element in the vector. */
		const T* end() const
		{
			return m_data + m_len;
		}

	private:
		T*		m_data;
		int		m_len;
		int		m_cap;

		void setCapacity(int cap)
		{
			++cap;
			if(cap < 8)
				cap = 8;
			else if(cap < m_cap * 2)
				cap = m_cap * 2;
			m_cap = cap;

			T* data = new T[cap];
			for(int i = 0 ; i < m_len ; ++i)
				data[i] = m_data[i];
			delete[] m_data;
			m_data = data;
		}
};

#endif // _DEV_ARRAY_H
