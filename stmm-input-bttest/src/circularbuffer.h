/*
 * Copyright © 2017  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   circularbuffer.h
 */

#ifndef _BT_CIRCULAR_BUFFER_H_
#define _BT_CIRCULAR_BUFFER_H_

#include <vector>
#include <cassert>
#include <iostream>

namespace stmi
{

template <class T>
class CircularBuffer {
public:
	inline CircularBuffer(int32_t nSize);

	inline void clear();
	inline bool isEmpty() const;
	inline bool isFull() const;
	inline T read();
	inline void write(const T& oT);

private:
	CircularBuffer() = delete;

	int32_t m_nCount;
	int32_t m_nReadIdx;
	int32_t m_nWriteIdx;
	std::vector<T> m_aT;
};

template <class T>
inline CircularBuffer<T>::CircularBuffer(int32_t nSize)
: m_nCount(0)
, m_nReadIdx(0)
, m_nWriteIdx(0)
{
	assert(nSize > 0);
	m_aT.resize(nSize);
}

template <class T>
inline void CircularBuffer<T>::clear()
{
	m_nCount = 0;
	m_nReadIdx = 0;
	m_nWriteIdx = 0;
}

template <class T>
inline bool CircularBuffer<T>::isEmpty() const
{
	return (m_nCount == 0);
}
template <class T>
inline bool CircularBuffer<T>::isFull() const
{
	return (m_nCount == static_cast<int32_t>(m_aT.size()));
}

template <class T>
inline T CircularBuffer<T>::read()
{
	assert(!isEmpty());
	const int32_t nReadIdx = m_nReadIdx;
	++m_nReadIdx;
	if (m_nReadIdx == static_cast<int32_t>(m_aT.size())) {
		m_nReadIdx = 0;
	}
	--m_nCount;
	return m_aT[nReadIdx];
}

template <class T>
inline void CircularBuffer<T>::write(const T& oT)
{
	assert(!isFull());
	m_aT[m_nWriteIdx] = oT;
	++m_nWriteIdx;
	if (m_nWriteIdx == static_cast<int32_t>(m_aT.size())) {
		m_nWriteIdx = 0;
	}
	++m_nCount;
}

} // namespace stmi

#endif	/* _BT_CIRCULAR_BUFFER_H_ */

