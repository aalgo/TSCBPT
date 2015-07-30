/*
 * VectorModel.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 31/10/2012
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef VECTORMODEL_HPP_
#define VECTORMODEL_HPP_

#include <vector>
#include <tsc/image/Pixel.hpp>

namespace tscbpt {

using namespace std;

/**
 * Generic model to hold a vector of ElemType as a region model.
 * Do not confuse with VectorMatrixModel, which holds a vector of matrices
 * in order to represent a BlockDiagonal Matrix.
 */
template<
	typename ElemType = double,
	typename NSubnodesType = size_t,
	typename PositionType = float>
class VectorModel {
public:
	typedef ElemType 											elem_type;
	typedef NSubnodesType 										subnodes_type;
	typedef PositionType 										position_type;
	typedef vector<position_type>								position_vector;
	typedef vector<elem_type>									data_vector;
	typedef VectorModel<ElemType, NSubnodesType, PositionType> 	this_type;
	typedef this_type											data_type;

	VectorModel() {
	}

	VectorModel(const this_type& b) :
			_vector(b._vector), _position(b._position), _subnodes(b._subnodes) {
	}

	template<typename PixelType, size_t NDimensions, typename CoordinateType>
	VectorModel(const Pixel<PixelType, NDimensions, CoordinateType>& p) :
			_subnodes(1) {
		for (size_t i = 0; i < NDimensions; ++i) {
			_position.push_back(p.getDim(i));
		}
		initialize_vector(p.getValue());
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

	const data_vector& getVector() const {
		return _vector;
	}

	data_vector& getVector() {
		return _vector;
	}

	size_t getNumElems() const {
		return _vector.size();
	}

	// TODO: make a clear interface to avoid this functions (Dissimilarities and boxcar filtering)
	// ====================================== COMPATIBILITY ===========================================================
	elem_type operator()(size_t i) const {
		return _vector.at(i);
	}

	this_type& operator*=(const elem_type& b) {
		for (size_t i = 0; i < _vector.size(); ++i) {
			_vector[i] *= b;
		}
		return *this;
	}

	this_type& operator+=(const this_type& b) {
		for (size_t i = 0; i < _vector.size(); ++i) {
			_vector[i] += b._vector[i];
		}
		return *this;
	}
	// ===================================== END COMPATIBILITY ========================================================

	this_type merge(const this_type& b) const {
		this_type tmp(*this);
		tmp._subnodes = this->_subnodes + b._subnodes;
		for (size_t i = 0; i < _position.size(); i++) {
			tmp._position[i] = ((this->_position[i] * this->_subnodes + b._position[i] * b._subnodes) / tmp._subnodes);
		}
		for (size_t i = 0; i < _vector.size(); ++i) {
			tmp._vector[i] = ((this->_vector[i] * elem_type(this->_subnodes) + b._vector[i] * elem_type(b._subnodes))
					/ elem_type(tmp._subnodes));
		}
		return tmp;
	}

	this_type unmerge(const this_type& b) const {
		this_type tmp(*this);
		tmp._subnodes = this->_subnodes - b._subnodes;
		for (size_t i = 0; i < _position.size(); i++) {
			tmp._position.push_back(
				(this->_position[i] * this->_subnodes - b._position[i] * b._subnodes) / tmp._subnodes);
		}
		for (size_t i = 0; i < _vector.size(); ++i) {
			tmp._vector[i] = ((this->_vector[i] * elem_type(this->_subnodes) - b._vector[i] * elem_type(b._subnodes))
					/ elem_type(tmp._subnodes));
		}
		return tmp;
	}

private:
	data_vector _vector;
	position_vector _position;
	subnodes_type _subnodes;

	template<typename T>
	void initialize_vector(const vector<T>& v) {
		_vector = vector<elem_type>(v.size());
		for (size_t i = 0; i < v.size(); ++i) {
			_vector[i] = v[i];
		}
	}
};



template<
	typename ElemType,
	typename NSubnodesType,
	typename PositionType>
typename NormTraits<typename VectorModel<ElemType, NSubnodesType, PositionType>::elem_type>::type norm2(
	const VectorModel<ElemType, NSubnodesType, PositionType>& a){

	typedef typename NormTraits<typename VectorModel<ElemType, NSubnodesType, PositionType>::elem_type>::type	ret_type;
	ret_type ret = ret_type();
	for (size_t i = 0; i < a.getNumElems(); ++i) {
		ret += norm2(a(i));
	}
	return ret;
}

template<
	typename ElemType,
	typename NSubnodesType,
	typename PositionType>
typename NormTraits<typename VectorModel<ElemType, NSubnodesType, PositionType>::elem_type>::type dist2(
		const VectorModel<ElemType, NSubnodesType, PositionType>& a,
		const VectorModel<ElemType, NSubnodesType, PositionType>& b){

	typedef typename NormTraits<typename VectorModel<ElemType, NSubnodesType, PositionType>::elem_type>::type	ret_type;
	ret_type ret = ret_type();
	for (size_t i = 0; i < a.getNumElems(); ++i) {
		ret += norm2(a(i) - b(i));
	}
	return ret;
}




} /* namespace tscbpt */
#endif /* VECTORMODEL_HPP_ */
