/*
 * SquareMatrix.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 07/11/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SQUAREMATRIX_HPP_
#define SQUAREMATRIX_HPP_

#include <vector>
#include <cmath>
#include <cassert>

namespace tscbpt
{

using namespace std;

template<typename Field>
class SquareMatrix
: public Matrix_Base<SquareMatrix<Field>, Field >
{
public:
	typedef Field		FieldType;
	typedef FieldType	elem_type;

	SquareMatrix(size_t rows = 0, size_t cols = 0){
		assert(rows == cols);
		_data.resize(rows*cols);
	}

	size_t getSize() const {
		return static_cast<size_t>(sqrt(_data.size()));
	}

	size_t getRows() const {
		return getSize();
	}

	size_t getCols() const {
		return getSize();
	}

	elem_type operator()(size_t row, size_t col) const {
		assert(row < getRows());
		assert(col < getCols());
		return _data[col * getSize() + row];
	}

	elem_type& operator()(size_t row, size_t col) {
		assert(row < getRows());
		assert(col < getCols());
		return _data[col * getSize() + row];
	}

private:
	vector<elem_type>		_data;
};


/**
 * Define shape as square for all SquareMatrix<Field> classes
 */
template<typename Field>
struct HasSquareShape<SquareMatrix<Field> >{
	static const bool value 			= true;
};

}

#endif /* SQUAREMATRIX_HPP_ */
