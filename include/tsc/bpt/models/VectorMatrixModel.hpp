/*
 * VectorMatrixModel.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 25/11/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef VECTORMATRIXMODEL_HPP_
#define VECTORMATRIXMODEL_HPP_

#include <vector>
#include <string>
#include <tsc/data/matrix/HermitianMatrix.hpp>
#include <tsc/data/matrix/NormTraits.hpp>
#include <tsc/data/matrix/BlockDiagonalMatrix.hpp>
#include <tsc/data/matrix/GetMinDimSize.hpp>
#include <tsc/image/Pixel.hpp>

namespace tscbpt
{

using namespace std;

/**
 * Model to hold a vector of matrices, representing a Block Diagonal
 * hermitian matrix.
 *
 * NOTE: A generic VectorModel of Hermitian matrices could be used but this
 * class provides the same interface for accessing the Block Diagonal matrix
 * as a big matrix --> Some dissimilarities (like Diagonal ones) do not need
 * to be modified to be used with this model. Other full matrix measures
 * are redefined to be more efficient (ending with
 * -VectorMatrixDissimilarityMeasure).
 */
template<
	typename Matrix_ElemType	= complex<double>,
	typename NSubnodesType 		= size_t,
	typename PositionType		= float,
	size_t SubMatrixSize 		= 3
	>
class VectorMatrixModel
{
public:
	static const size_t subMatrix_size = SubMatrixSize;

	typedef Matrix_ElemType												matrix_elem_type;
	typedef NSubnodesType												subnodes_type;
	typedef PositionType												position_type;
	typedef vector<position_type>										position_vector;
	typedef HermitianMatrixFixed<SubMatrixSize, matrix_elem_type>		matrix_type;
	typedef BlockDiagonalMatrix<matrix_elem_type, subMatrix_size, matrix_type>	covariance_type;
	typedef VectorMatrixModel<matrix_elem_type, subnodes_type, position_type, SubMatrixSize>		this_type;
	typedef this_type													data_type;

	VectorMatrixModel(){}

	VectorMatrixModel(const this_type& b) : _matrix(b._matrix), _position(b._position), _subnodes(b._subnodes) {	}

	template<typename PixelType, size_t NDimensions, typename CoordinateType>
	VectorMatrixModel(Pixel<PixelType, NDimensions, CoordinateType>& p): _subnodes(1) {
		for(size_t i = 0; i < NDimensions; ++i) {
			_position.push_back(p.getDim(i));
		}
		initialize_covariance(p.getValue());
	}

	const position_vector& getPosition() const {
		return _position;
	}

	position_vector& getPosition() {
		return _position;
	}

	position_type getDim(size_t n) const {
		return _position.at(n);
	}

	subnodes_type getSubnodes() const {
		return _subnodes;
	}

	const matrix_type& getCovariance(size_t index) const {
		return _matrix.getSubMatrix(index);
	}

	matrix_type& getCovariance(size_t index) {
		return _matrix.getSubMatrix(index);
	}

	const covariance_type& getFullCovariance() const {
		return _matrix;
	}

	covariance_type& getFullCovariance() {
		return _matrix;
	}

	size_t getNumCovariances() const {
		return _matrix.getRows() / subMatrix_size;
	}

	// TODO: make a clear interface to avoid this functions (Dissimilarities and boxcar filtering)
	// ====================================== COMPATIBILITY ===========================================================
	size_t getMatrixSize() const {
		return _matrix.getRows();
	}

	matrix_elem_type operator()(size_t i, size_t j) const {
		return _matrix(i,j);
	}

	this_type& operator*=(const matrix_elem_type& b) {
		_matrix *= b;
		return *this;
	}

	this_type& operator+=(const this_type& b) {
		_matrix += b._matrix;
		return *this;
	}
	// ===================================== END COMPATIBILITY ========================================================

	this_type merge(const this_type& b) const {
		this_type tmp(*this);
		tmp._subnodes = this->_subnodes + b._subnodes;
		for (size_t i = 0; i < _position.size(); i++) {
			tmp._position[i] = ((this->_position[i] * this->_subnodes + b._position[i] * b._subnodes) / tmp._subnodes);
		}
		tmp._matrix = ((this->_matrix * matrix_elem_type(this->_subnodes) + b._matrix * matrix_elem_type(b._subnodes))
				/ matrix_elem_type(tmp._subnodes));
		return tmp;
	}

	this_type unmerge(const this_type& b) const {
		this_type tmp(*this);
		tmp._subnodes = this->_subnodes - b._subnodes;
		for (size_t i = 0; i < _position.size(); i++) {
			tmp._position.push_back(
				(this->_position[i] * this->_subnodes - b._position[i] * b._subnodes) / tmp._subnodes);
		}
		tmp._matrix = ((this->_matrix * matrix_elem_type(this->_subnodes) - b._matrix * matrix_elem_type(b._subnodes))
				/ matrix_elem_type(tmp._subnodes));
		return tmp;
	}

	void initializeToZero(){
		_matrix = covariance_type();
		for(typename vector<position_type>::iterator it = _position.begin(); it != _position.end(); ++it) *it = position_type();
		_subnodes = subnodes_type();
	}

private:
	covariance_type				_matrix;
	vector<position_type>		_position;
	subnodes_type				_subnodes;


	template <typename T>
	void initialize_covariance(vector<T>& v) {
		assert(v.size() % subMatrix_size == 0);
		_matrix = covariance_type(v.size(), v.size());

		if (MatrixShape<matrix_type>::isHermitian) {
			for (size_t i = 0; i < v.size(); ++i) {
				for (size_t j = i; j < (i/subMatrix_size + 1) * subMatrix_size; ++j) {
					// armadillo stores data column by column
					_matrix(i, j) = v[j] * conj(v[i]);
				}
			}
		} else {
			for (size_t i = 0; i < v.size(); ++i) {
				for (size_t j = (i/subMatrix_size) * subMatrix_size; j < (i/subMatrix_size + 1) * subMatrix_size; ++j) {
					// armadillo stores data column by column
					_matrix(i, j) = v[j] * conj(v[i]);
				}
			}
		}
	}

};

template<
	typename Matrix_ElemType,
	typename NSubnodesType,
	typename PositionType,
	size_t SubMatrixSize
	>
typename NormTraits<typename VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSize>::matrix_elem_type>::type norm2(
	const VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSize>& a){

	typedef typename NormTraits<typename VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSize>::matrix_elem_type>::type	ret_type;
	ret_type ret = ret_type();
	for (size_t i = 0; i < a.getNumCovariances(); ++i) {
		ret += norm2(a.getCovariance(i));
	}
	return ret;
}

template<
	typename Matrix_ElemType,
	typename NSubnodesType,
	typename PositionType,
	size_t SubMatrixSize
	>
typename NormTraits<typename VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSize>::matrix_elem_type>::type dist2(
	const VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSize>& a,
	const VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSize>& b){

	typedef typename NormTraits<typename VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSize>::matrix_elem_type>::type	ret_type;
	ret_type ret = ret_type();
	for (size_t i = 0; i < a.getNumCovariances(); ++i) {
		ret += norm2(a.getCovariance(i) - b.getCovariance(i));
	}
	return ret;
}

// Static member definition needed to avoid compilation errors when optimization disabled
template<
	typename Matrix_ElemType, typename NSubnodesType, typename PositionType,size_t SubMatrixSize>
const size_t VectorMatrixModel<Matrix_ElemType,NSubnodesType,PositionType,SubMatrixSize>::subMatrix_size;

/**
 * Generic functor to get the minimum size of all the dimensions of an array.
 * IMPLEMENTATION for VectorMatrixModel
 */
template<typename Matrix_ElemType, typename NSubnodesType, typename PositionType,size_t SubMatrixSize>
struct GetMinDimSize<VectorMatrixModel<Matrix_ElemType,NSubnodesType,PositionType,SubMatrixSize> >:
public unary_function<VectorMatrixModel<Matrix_ElemType,NSubnodesType,PositionType,SubMatrixSize> ,size_t> {
	inline size_t operator()(const VectorMatrixModel<Matrix_ElemType,NSubnodesType,PositionType,SubMatrixSize>& m) const {
		return m.getFullCovariance().getSize();
	}
};


// log_det() function implementation
template<typename Matrix_ElemType,typename NSubnodesType,typename PositionType,size_t SubMatrixSize>
Matrix_ElemType log_det(const VectorMatrixModel<Matrix_ElemType,NSubnodesType,PositionType,SubMatrixSize>& m){
	return log_det(m.getFullCovariance());
}

}

#endif /* VECTORMATRIXMODEL_HPP_ */
