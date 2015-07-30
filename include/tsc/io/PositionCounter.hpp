/*
 * PositionCounter.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 31/03/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef POSITIONCOUNTER_HPP_
#define POSITIONCOUNTER_HPP_

#include <boost/static_assert.hpp>

namespace tscbpt
{

template <size_t NDims=2>
struct PositionCounter
{
	BOOST_STATIC_ASSERT(NDims > 0);

	PositionCounter(size_t rows) : _current(0) {
		BOOST_STATIC_ASSERT(NDims == 1);
		_dimensions[0] = rows;
	}

	PositionCounter(size_t rows, size_t cols) : _current(0) {
		BOOST_STATIC_ASSERT(NDims == 2);
		_dimensions[0] = rows;
		_dimensions[1] = cols;
	}

	// bool argument to mark an end iterator constructor
	PositionCounter(size_t rows, size_t cols, bool) : _current(rows*cols) {
		BOOST_STATIC_ASSERT(NDims == 2);
		_dimensions[0] = rows;
		_dimensions[1] = cols;
	}

	PositionCounter(size_t rows, size_t cols, size_t slices) : _current(0) {
		BOOST_STATIC_ASSERT(NDims == 3);
		_dimensions[0] = rows;
		_dimensions[1] = cols;
		_dimensions[2] = slices;
	}

	size_t getNumberOfRows() const {
		return _dimensions[0];
	}

	size_t getNumberOfCols() const {
		if(NDims >= 2)
			return _dimensions[1];
		else
			return 1;
	}

	size_t getNumberOfSlices() const {
		if(NDims >= 3)
			return _dimensions[2];
		else
			return 1;
	}

	size_t getRow() const {
		return _current / _dimensions[0];
	}

	size_t getCol() const {
		return _current % _dimensions[0];
	}

protected:
	void increment(){
		++_current;
	}

private:
	size_t _current;
	size_t _dimensions[NDims];
};

}

#endif /* POSITIONCOUNTER_HPP_ */
