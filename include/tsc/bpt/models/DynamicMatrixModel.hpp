/*
 * DynamicMatrixModel.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 14/11/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef DYNAMICMATRIXMODEL_HPP_
#define DYNAMICMATRIXMODEL_HPP_

namespace tscbpt{
template<typename Matrix_ElemType, typename NSubnodesType, typename PositionType> class DynamicMatrixModel;
}



#include <vector>
#include <complex>
#include <tsc/image/Pixel.hpp>
#include <tsc/data/matrix/NormTraits.hpp>
#include <tsc/data/matrix/Matrix.hpp>
#include <tsc/data/matrix/HermitianMatrix.hpp>

namespace tscbpt
{

using namespace std;

template<
	typename Matrix_ElemType	= complex<double>,
	typename NSubnodesType 		= size_t,
	typename PositionType		= float
	>
class DynamicMatrixModel
{
public:

	typedef Matrix_ElemType						matrix_elem_type;
	typedef NSubnodesType						subnodes_type;
	typedef PositionType						position_type;
	typedef DynamicMatrixModel<matrix_elem_type, subnodes_type, position_type>	this_type;
	typedef vector<position_type>				position_vector;
	typedef HermitianMatrix<matrix_elem_type>	matrix_type;

	template<typename PixelType, size_t NDimensions, typename CoordinateType>
	DynamicMatrixModel(Pixel<PixelType, NDimensions, CoordinateType>& p): _subnodes(1){
		for(size_t i = 0; i < NDimensions; ++i){
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

	const matrix_type& getCovariance() const {
		return _covariance;
	}

	matrix_type& getCovariance() {
		return _covariance;
	}

	// TODO: make a clear interface to avoid this functions (Dissimilarities and boxcar filtering)
	// ====================================== COMPATIBILITY ===========================================================
	size_t getMatrixSize() const {
		return _covariance.getSize();
	}

	matrix_elem_type operator()(size_t i, size_t j) const {
		return _covariance(i,j);
	}

	this_type& operator*=(const matrix_elem_type& b) {
		this->_covariance *= b;
		return *this;
	}

	this_type& operator+=(const this_type& b){
		this->_covariance += b._covariance;
		return *this;
	}
	// ===================================== END COMPATIBILITY ========================================================

	this_type merge(const this_type& b) const {
		this_type tmp(*this);
		tmp._subnodes = this->_subnodes + b._subnodes;
		if (MatrixShape<matrix_type>::isHermitian) {
			for (size_t i = 0; i < _covariance.getRows(); ++i) {
				for (size_t j = i; j < _covariance.getCols(); ++j) {
					tmp._covariance(i, j) = (this->_covariance(i, j) * matrix_elem_type(this->_subnodes)
						+ b._covariance(i, j) * matrix_elem_type(b._subnodes)) / matrix_elem_type(tmp._subnodes);
				}
			}
		} else {
			for (size_t i = 0; i < _covariance.getRows(); ++i) {
				for (size_t j = 0; j < _covariance.getCols(); ++j) {
					tmp._covariance(i, j) = (this->_covariance(i, j) * matrix_elem_type(this->_subnodes)
						+ b._covariance(i, j) * matrix_elem_type(b._subnodes)) / matrix_elem_type(tmp._subnodes);
				}
			}
		}
		for (size_t i = 0; i < _position.size(); i++) {
			tmp._position[i] = ((this->_position[i]*this->_subnodes + b._position[i]*b._subnodes) / tmp._subnodes);
		}
		return tmp;
	}

	this_type unmerge(const this_type& b) const {
		this_type tmp(this->_covariance.getSize());
		tmp._subnodes = this->_subnodes - b._subnodes;
		if (MatrixShape<matrix_type>::isHermitian) {
			for (size_t i = 0; i < _covariance.getRows(); i++) {
				for (size_t j = i; j < _covariance.getCols(); ++j) {
					tmp._covariance(i, j) = (this->_covariance(i, j) * this->_subnodes - b._covariance(i, j)
						* b._subnodes) / tmp._subnodes;
				}
			}
		} else {
			for (size_t i = 0; i < _covariance.getRows(); i++) {
				for (size_t j = 0; j < _covariance.getCols(); ++j) {
					tmp._covariance(i, j) = (this->_covariance(i, j) * this->_subnodes - b._covariance(i, j)
						* b._subnodes) / tmp._subnodes;
				}
			}
		}
		for (size_t i = 0; i < _position.size(); i++) {
			tmp._position.push_back((this->_position[i]*this->_subnodes - b._position[i]*b._subnodes) / tmp._subnodes);
		}
		return tmp;
	}


private:
	vector<position_type>		_position;
	subnodes_type				_subnodes;
	matrix_type					_covariance;

	template<typename T, typename F>
	void initialize_covariance(Matrix_Base<T, F>& m){
		_covariance = m;
	}

	template<typename T>
	void initialize_covariance(vector<T>& v){
		_covariance = matrix_type(v.size(), v.size());
		if (MatrixShape<matrix_type>::isHermitian) {
			for (size_t i = 0; i < v.size(); ++i) {
				for (size_t j = i; j < v.size(); ++j) {
					// AAG 20111007: armadillo stores data column by column
					_covariance(i, j) = v[j] * conj(v[i]);
				}
			}
		} else {
			for (size_t i = 0; i < v.size(); ++i) {
				for (size_t j = 0; j < v.size(); ++j) {
					// AAG 20111007: armadillo stores data column by column
					_covariance(j, i) = v[i] * conj(v[j]);
				}
			}
		}
	}
};

template<class T, class U, class V>
typename NormTraits<typename DynamicMatrixModel<T,U,V>::matrix_elem_type>::type  norm2(const DynamicMatrixModel<T,U,V>& a){
	return norm2(a.getCovariance());
}

template<class T, class U, class V>
typename NormTraits<typename DynamicMatrixModel<T,U,V>::matrix_elem_type>::type  dist2(const DynamicMatrixModel<T,U,V>& a, const DynamicMatrixModel<T,U,V>& b){
	return norm2(a.getCovariance()-b.getCovariance());
}

}

#endif /* DYNAMICMATRIXMODEL_HPP_ */
