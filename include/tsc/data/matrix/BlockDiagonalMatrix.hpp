/*
 * BlockDiagonalMatrix.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 19/12/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef BLOCKDIAGONALMATRIX_HPP_
#define BLOCKDIAGONALMATRIX_HPP_

#include <cassert>
#include "Matrix.hpp"
#include "HermitianMatrix.hpp"
#include "GetMinDimSize.hpp"
#include <tsc/util/Algorithms.h>
#include <tsc/util/types/WrapperOf.hpp>


namespace tscbpt
{

using namespace std;

/**
 * Class to hold a block diagonal matrix.
 * The matrix is composed by square submatrices on the diagonal and is
 * zero elsewhere.
 * This class is restricted to equal size blocks of SubMatrixSize.
 */
template<
	typename Field,
	size_t SubMatrixSize,
	typename SubMatrixType = HermitianMatrixFixed<SubMatrixSize, Field>
	>
class BlockDiagonalMatrix:
	public Matrix_Base<BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType>, Field>
{
public:
	typedef Field							FieldType;
	typedef FieldType						elem_type;
	typedef BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType>	this_type;
	typedef SubMatrixType					matrix_type;

	static const size_t subMatrixSize = SubMatrixSize;

	BlockDiagonalMatrix() {}

	BlockDiagonalMatrix(size_t rows, size_t cols) {
		assert(rows == cols);
		assert(rows % subMatrixSize == 0);
		_matrices.resize(rows / subMatrixSize);
	}

	size_t getSize() const {
		return _matrices.size() * subMatrixSize;
	}

	size_t getNumSubMatrices() const {
		return _matrices.size();
	}

	size_t getRows() const {
		return getSize();
	}

	size_t getCols() const {
		return getSize();
	}

	elem_type operator()(size_t row, size_t col) const {
		signed long int si = static_cast<signed long int>(row);
		signed long int sj = static_cast<signed long int>(col);
		return (static_cast<size_t>(abs(si-sj)) > subMatrixSize)? elem_type() : _matrices[row/subMatrixSize](row%subMatrixSize, col%subMatrixSize);
	}

	elem_type& operator()(size_t row, size_t col) {
		signed long int si = static_cast<signed long int>(row);
		signed long int sj = static_cast<signed long int>(col);
		assert(static_cast<size_t>(abs(si-sj)) <= subMatrixSize /* Ensures getting R/W access to an element within the diagonal block */);
		return _matrices[row/subMatrixSize](row%subMatrixSize, col%subMatrixSize);
	}

	const matrix_type& getSubMatrix(size_t index) const {
		assert(index < _matrices.size());
		return _matrices[index];
	}

	matrix_type& getSubMatrix(size_t index) {
		assert(index < _matrices.size());
		return _matrices[index];
	}

	template <typename func>
	void for_each_elem(func _functor) {
		for(size_t i = 0; i < _matrices.size(); ++i){
			_matrices[i].for_each_elem(_functor);
		}
	}

	template <typename func>
	void for_each_elem(func _functor, const this_type& b) {
		for(size_t i = 0; i < _matrices.size(); ++i){
			_matrices[i].for_each_elem(_functor, b._matrices[i]);
		}
	}

private:
	vector<matrix_type>		_matrices;
};


/**
 * Define shape as square for all BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType> classes
 */
template <typename Field, size_t SubMatrixSize, typename SubMatrixType>
struct HasSquareShape<BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType> >
{
	static const bool value = true;
};

/**
 * Define shape as hermitian for all BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType> classes
 * formed by Hermitian blocks
 */
template <typename Field, size_t SubMatrixSize, typename SubMatrixType>
struct HasHermitianShape<BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType> >
{
	static const bool value = HasHermitianShape<SubMatrixType>::value;
};

/**
 * Generic functor to get the minimum size of all the dimensions of an array.
 * IMPLEMENTATION for BlockDiagonalMatrix
 */
template <typename Field, size_t SubMatrixSize, typename SubMatrixType>
struct GetMinDimSize<BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType> >:
public unary_function<BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType> ,size_t> {
	inline size_t operator()(const BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType>& m) const {
		return m.getSize();
	}
};


// log_det() function implementation
template<typename Field,size_t SubMatrixSize,typename SubMatrixType>
Field log_det(const BlockDiagonalMatrix<Field, SubMatrixSize, SubMatrixType>& m){
	Field res = Field();
	for(size_t i = 0; i < m.getNumSubMatrices(); ++i)
		res += log_det(m.getSubMatrix(i));
	return res;
}


}

#endif /* BLOCKDIAGONALMATRIX_HPP_ */
