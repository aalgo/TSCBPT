/*
 * BinaryFileIterator.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 29/03/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef BINARYFILEITERATOR_HPP_
#define BINARYFILEITERATOR_HPP_

#include <iostream>

namespace tscbpt
{

template <typename T>
struct BinaryFileIterator: public std::iterator<std::input_iterator_tag, void, void, void, void>
{
	typedef T			value_type;

	BinaryFileIterator(std::istream& str) :
		m_str(&str), _swap_endian(false) {
		m_str->read(reinterpret_cast<char*> (&current), sizeof(T));
//		m_str->peek(); // Needed to set stream flags properly
	}

	BinaryFileIterator(std::istream& str, bool swap) :
		m_str(&str), _swap_endian(swap) {
		m_str->read(reinterpret_cast<char*> (&current), sizeof(T));
		if(_swap_endian) current = swap_endian(current);
	}

	BinaryFileIterator() :
		m_str(NULL), _swap_endian(false) {
	}

	BinaryFileIterator& operator++() {
		m_str->read(reinterpret_cast<char*> (&current), sizeof(T));
		if(_swap_endian) current = swap_endian(current);
//		m_str->peek(); // Needed to set stream flags properly
		return *this;
	}
//	BinaryFileIterator& operator++(int) {
//		m_str->read(reinterpret_cast<char*> (&current), sizeof(T));
//		if(_swap_endian) current = swap_endian(current);
////		m_str->peek(); // Needed to set stream flags properly
//		return *this;
//	}

	T& operator*() {
		if(!m_str)
			__throw_ios_failure("ERROR: operator* over an end BinaryFileIterator");

		return current;
	}

	// If either iterator has a NULL pointer then it is the end() of stream iterator.
	// Input iterators are only equal if they have read past the end of stream.
	bool operator!=(BinaryFileIterator const& rhs) const {
		bool lhsPastEnd = (m_str == NULL) || (!m_str->good());
		bool rhsPastEnd = (rhs.m_str == NULL) || (!rhs.m_str->good());
		return !(lhsPastEnd && rhsPastEnd);
	}

	bool operator==(BinaryFileIterator const& rhs) const {
		return !operator!=(rhs);
	}

private:
	std::istream* m_str;
	T current;
	bool _swap_endian;

	template<typename Type>
	Type swap_endian(Type value) {
		char * data = reinterpret_cast<char*>(&value);
		const size_t data_size = sizeof(Type);
		for (size_t i = 0; i < sizeof(Type) / 2; ++i) {
			unsigned char tmp = data[i];
			data[i] = data[data_size - 1 - i];
			data[data_size - 1 - i] = tmp;
		}
		return value;
	}

};

}

#endif /* BINARYFILEITERATOR_HPP_ */
