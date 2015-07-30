/*
 * Position.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 17/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef POSITION_HPP_
#define POSITION_HPP_

#include <cstddef>
#include <boost/static_assert.hpp>

namespace tscbpt
{

using namespace std;

template<
	size_t		NumDims,
	typename 		CoordinateType = float >
class Position
{
public:

	static const size_t dimensions = NumDims;

	Position(CoordinateType x){
		BOOST_STATIC_ASSERT(dimensions == 1);
		_pos[0] = x;
	}

	Position(CoordinateType x, CoordinateType y) {
		BOOST_STATIC_ASSERT(dimensions == 2);
		_pos[0] = x;
		_pos[1] = y;
	}

	Position(CoordinateType x, CoordinateType y, CoordinateType z) {
		BOOST_STATIC_ASSERT(dimensions == 3);
		_pos[0] = x;
		_pos[1] = y;
		_pos[2] = z;
	}

	Position(CoordinateType const * pos){
		for(size_t i = 0; i< dimensions; ++i){
			_pos[i] = pos[i];
		}
	}


	CoordinateType getX() const {
		return _pos[0];
	}

	CoordinateType getY() const {
		// NOTE: This conditionals will be removed at compile-time with optimization enabled
		if (dimensions >= 2) return _pos[1];
		else return 0.0;
	}

	CoordinateType getZ() const {
		// NOTE: This conditionals will be removed at compile-time with optimization enabled
		if (dimensions >= 3) return _pos[2];
		else return 0.0;
	}

	size_t getNumberOfDimensions() {
		return dimensions;
	}

	CoordinateType const * getPos() const {
		return _pos;
	}

	CoordinateType getDim(size_t d) const {
		if (d >= dimensions) __throw_invalid_argument("Accessed getDim() with an invalid dimension parameter");
		return _pos[d];
	}

	void setX(CoordinateType x) {
		this->_pos[0] = x;
	}

	void setDim(size_t d, CoordinateType x) {
		if (d >= dimensions) __throw_invalid_argument("Accessed setDim() with an invalid dimension parameter");
		this->_pos[d] = x;
	}

	void setY(CoordinateType y) {
		BOOST_STATIC_ASSERT(dimensions >= 2);
		this->_pos[1] = y;
	}

	void setZ(CoordinateType z) {
		BOOST_STATIC_ASSERT(dimensions >= 3);
		this->_pos[2] = z;
	}

protected:
	BOOST_STATIC_ASSERT(NumDims>0);
	CoordinateType	_pos[NumDims];
	Position(){}
};

}

#endif /* POSITION_HPP_ */
