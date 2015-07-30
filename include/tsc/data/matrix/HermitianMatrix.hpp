/*
 * HermitianMatrix.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 28/10/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef HERMITIANMATRIX_HPP_
#define HERMITIANMATRIX_HPP_

namespace tscbpt {
/**
 * Variable size hermitian matrix container.
 * It is storage-efficient since it only stores the diagonal and the
 *   upper triangular matrix. However, the diagonal and the off-diagonal
 *   elements are stored having the same type in the same container.
 */
template<typename Field> class HermitianMatrix;
}

#include <vector>
#include <cassert>
#include <complex>
#include "Matrix.hpp"
#include <tsc/util/ArmadilloWrapper.hpp>

namespace tscbpt
{

using namespace std;

/**
 * Variable size hermitian matrix container.
 * It is storage-efficient since it only stores the diagonal and the
 *   upper triangular matrix. However, the diagonal and the off-diagonal
 *   elements are stored having the same type in the same container.
 */
template<typename Field>
class HermitianMatrix
	: public Matrix_Base<HermitianMatrix<Field>, Field >
{
public:
	typedef Field						FieldType;
	typedef FieldType					elem_type;
	typedef HermitianMatrix<elem_type>	this_type;

	HermitianMatrix() {}

	HermitianMatrix(size_t rows, size_t cols){
		assert(rows == cols);
		_data.resize((rows * cols - rows)/2 + rows);
	}

	size_t getSize() const {
		return static_cast<size_t>(-0.5 + sqrt(0.25+2*_data.size()));
	}

	size_t getRows() const {
		return getSize();
	}

	size_t getCols() const {
		return getSize();
	}

	elem_type operator()(size_t row, size_t col) const {
		if(row > col) return (operator()(col, row));
		assert(col < getSize());
		return _data[row * getSize() - (row*row - row)/2 + (col-row)];
	}

	elem_type& operator()(size_t row, size_t col) {
		if (row > col) return (operator()(col, row));
		assert(col < getSize());
		return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
	}

private:
	vector<elem_type>		_data;
};


template <size_t Size, typename FieldType>
class HermitianMatrixFixed: public Matrix_Base<HermitianMatrixFixed<Size, FieldType >, FieldType>
{
public:
	static const size_t rows = Size;
	static const size_t cols = Size;

	typedef FieldType elem_type;
	typedef HermitianMatrixFixed<Size, elem_type>	this_type;

	HermitianMatrixFixed() {}

	HermitianMatrixFixed(size_t rows, size_t cols) {
		assert(rows == Size && cols == Size);
	}

	size_t getSize() const {
		return rows;
	}

	size_t getRows() const {
		return getSize();
	}

	size_t getCols() const {
		return getSize();
	}

	elem_type operator()(size_t row, size_t col) const {
		if (row > col) return conj(operator()(col, row));
		assert(col < getSize());
		return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
	}

	elem_type& operator()(size_t row, size_t col) {
		assert(row <= col);
		assert(col < getSize());
		return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
	}

private:
	elem_type _data[rows + (rows * cols - rows) / 2];
};



/**
 * Variable size hermitian matrix container specialization for complex numbers.
 * It is storage-efficient since it only stores the diagonal and the
 *   upper triangular matrix. However, the diagonal and the off-diagonal
 *   elements are stored having the same type in the same container.
 */
template <typename T>
class HermitianMatrix<complex<T> >
	: public Matrix_Base<HermitianMatrix<complex<T> >, complex<T> >
{
public:
	typedef complex<T> 					FieldType;
	typedef FieldType					elem_type;
	typedef HermitianMatrix<elem_type>	this_type;

	HermitianMatrix() {}

	HermitianMatrix(size_t rows, size_t cols) {
		assert(rows == cols);
		_data.resize((rows * rows - rows) / 2 + rows);
	}

	size_t getSize() const {
		return static_cast<size_t> (-0.5 + sqrt(0.25 + 2 * _data.size()));
	}

	size_t getRows() const {
		return getSize();
	}

	size_t getCols() const {
		return getSize();
	}

	elem_type operator()(size_t row, size_t col) const {
		if (row > col) return conj(operator()(col, row));
		assert(col < getSize());
		return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
	}

	elem_type& operator()(size_t row, size_t col) {
		assert(row <= col);
		assert(col < getSize());
		return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
	}

	template<size_t Size, typename FieldType = elem_type>
	class fixed:
		public Matrix_Base<fixed<Size,FieldType>, FieldType> {
	public:
		static const size_t rows = Size;
		static const size_t cols = Size;

		typedef FieldType		elem_type;

		fixed(size_t rows = Size, size_t cols = Size) {
			assert(rows == Size && cols == Size);
		}

		size_t getSize() const {
			return rows;
		}

		size_t getRows() const {
			return getSize();
		}

		size_t getCols() const {
			return getSize();
		}

		elem_type operator()(size_t row, size_t col) const {
			if (row > col) return conj(operator()(col, row));
			assert(col < getSize());
			return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
		}

		elem_type& operator()(size_t row, size_t col) {
			assert(row <= col);
			assert(col < getSize());
			return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
		}

	private:
		elem_type _data[rows + (rows*cols - rows) / 2];
	};

private:
	vector<elem_type> _data;
};

template <size_t Size, typename T>
class HermitianMatrixFixed<Size, complex<T> >: public Matrix_Base<HermitianMatrixFixed<Size, complex<T> >, complex<T> >
{
public:
	static const size_t rows 	= Size;
	static const size_t cols 	= Size;
	static const size_t n_elems = rows + (rows * cols - rows) / 2;

	typedef complex<T> 								elem_type;
	typedef HermitianMatrixFixed<Size, elem_type>	this_type;
	typedef Matrix_Base<this_type, elem_type>		matrix_base;

	HermitianMatrixFixed() {}

	HermitianMatrixFixed(size_t rows, size_t cols) {
		assert(rows == Size && cols == Size);
	}

	size_t getSize() const {
		return rows;
	}

	size_t getRows() const {
		return getSize();
	}

	size_t getCols() const {
		return getSize();
	}

	elem_type operator()(size_t row, size_t col) const {
		if (row > col) return conj(operator()(col, row));
		assert(col < getSize());
		return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
	}

	elem_type& operator()(size_t row, size_t col) {
		assert(row <= col);
		assert(col < getSize());
		return _data[row * getSize() - (row * row - row) / 2 + (col - row)];
	}


//	using matrix_base::for_each_elem;

//	template <typename func>
//	void for_each_elem(func _functor) {
//		for (size_t i = 0; i < n_elems; ++i) {
//			_functor(_data[i]);
//		}
//	}

private:
	elem_type _data[n_elems];
};

/**
 * Define shape as square for all HermitianMatrix<Field> classes
 */
template<typename Field>
struct HasSquareShape<HermitianMatrix<Field> >{
	static const bool value 			= true;
};

/**
 * Define shape as square for all HermitianMatrixFixed<Size, Field> classes
 */
template<typename Field, size_t Size>
struct HasSquareShape<HermitianMatrixFixed<Size, Field> >{
	static const bool value 			= true;
};

/**
 * Define shape as hermitian for all HermitianMatrix<Field> classes
 */
template<typename Field>
struct HasHermitianShape<HermitianMatrix<Field> >{
	static const bool value 			= true;
};

/**
 * Define shape as hermitian for all HermitianMatrixFixed<Size, Field> classes
 */
template<typename Field, size_t Size>
struct HasHermitianShape<HermitianMatrixFixed<Size, Field> >{
	static const bool value 			= true;
};

/**
 * log_det() implementation
 */
template <typename FieldType>
FieldType log_det(const HermitianMatrix<FieldType> &m){
	arma::cx_mat ma = to_arma<arma::cx_mat>::from(m);
	FieldType val;
	double sign;
	arma::log_det(val, sign, ma);
	assert(sign > 0);
	return val;
}

/**
 * log_det() implementation
 */
template <size_t Size, typename FieldType>
FieldType log_det(const HermitianMatrixFixed<Size,FieldType> &m){
	arma::cx_mat ma = to_arma<arma::cx_mat>::from(m);
	FieldType val;
	double sign;
	arma::log_det(val, sign, ma);
	assert(sign > 0);
	return val;
//	return log(abs(det(ma)));
}

}

#endif /* HERMITIANMATRIX_HPP_ */
