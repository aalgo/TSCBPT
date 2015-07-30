/*
 * SourceCutter.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 16/09/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SOURCECUTTER_HPP_
#define SOURCECUTTER_HPP_

#include <stddef.h>

namespace tscbpt
{

template <
	class TSource,
	typename OutType = typename TSource::iterator::value_type>
class SourceCutter
{

public:

	typedef TSource 						SourceType;
	typedef typename SourceType::iterator	InnerIterator;
	typedef SourceCutter<TSource, OutType>	ThisType;
	typedef ThisType						ThisSourceType;

	SourceCutter(SourceType& asource, size_t startRow, size_t startCol, size_t height, size_t width) :
		_xs(startCol), _ys(startRow), _height(height), _width(width), _source(asource){}

	class iterator
	{
	public:
		typedef iterator ThisType;
		typedef iterator& ThisReference;
		typedef OutType value_type;

		OutType operator*() {
			return *_it;
		}

		ThisType operator++(int) {
			ThisType tmp(*this);
			++*this;
			return tmp;
		}

		ThisReference operator++() {
			if (_it != _end) {
				++_it;
				while ((_it.getRow() < _powner->_ys || _it.getRow() >= _powner->_ys + _powner->_height || _it.getCol() < _powner->_xs || _it.getCol() >= _powner->_xs
					+ _powner->_width) && _it != _end) {
					++_it;
				}
			}
			return *this;
		}

		bool operator==(const ThisType& b) const {
			return this->_it == b._it;
		}

		bool operator!=(const ThisType& b) const {
			return this->_it != b._it;
		}

		size_t getRow() const {
			return this->_it.getRow() - _powner->_ys;
		}

		size_t getCol() const {
			return this->_it.getCol() - _powner->_xs;
		}

		friend class SourceCutter;

	protected:
		iterator(ThisSourceType* powner, InnerIterator ait, InnerIterator endit) :
			_it(ait), _end(endit), _powner(powner){
			while ((_it.getRow() < _powner->_ys || _it.getRow() >= _powner->_ys + _powner->_height || _it.getCol() < _powner->_xs || _it.getCol() >= _powner->_xs + _powner->_width)
				&& _it != _end) {
				++_it;
			}
		}
		InnerIterator _it;
		InnerIterator _end;
		ThisSourceType* _powner;
	};


	size_t getRows() const {
		return _height;
	}

	size_t getCols() const {
		return _width;
	}

	iterator begin() {
		return iterator(this, _source.begin(), _source.end());
	}

	iterator end() {
		return iterator(this, _source.end(), _source.end());
	}

private:
	size_t _xs,_ys;
	size_t _height,_width;
	SourceType& _source;

};

}

#endif /* SOURCECUTTER_HPP_ */
