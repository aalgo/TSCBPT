/*
 * DenseMatrix.hpp
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

#ifndef DENSEMATRIX_HPP_
#define DENSEMATRIX_HPP_

#include <vector>
#include <cassert>


namespace tscbpt
{

using namespace std;

/**
 * Generic class to hold a matrix of any size and not necessarily
 * hermitian.
 */
template<typename Field>
class DenseMatrix
: public Matrix_Base<DenseMatrix<Field>, Field >
{
public:
	typedef Field		FieldType;
	typedef FieldType	elem_type;

	DenseMatrix(size_t rows = 0, size_t cols = 0): _cols(cols) {
		_data.resize(rows*cols);
	}

	size_t getRows() const {
		return _data.size()/_cols;
	}

	size_t getCols() const {
		return _cols;
	}

	elem_type operator()(size_t row, size_t col) const {
		assert(row < getRows());
		assert(col < getCols());
		return _data[col*_cols + row];
	}

	elem_type& operator()(size_t row, size_t col) {
		assert(row < getRows());
		assert(col < getCols());
		return _data[col*_cols + row];
	}

private:
	size_t 					_cols;
	vector<elem_type>		_data;
};

}

#endif /* DENSEMATRIX_HPP_ */
