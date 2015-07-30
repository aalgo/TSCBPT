/*
 * ArrayAccessor.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 05/05/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef ARRAYACCESSOR_HPP_
#define ARRAYACCESSOR_HPP_

#include <cstddef>
#include <cassert>
#include "types/TypeTraits.hpp"

namespace tscbpt
{

/**
 * Class to access N-dimensional array with the operator(),
 *   e.g. covert expression a[1][2][3] to accessor(1,2,3).
 * Supports up to 8 dimensional array.
 */
template<
	typename ElemType
>
class ArrayAccessor
{};

template<
	typename ArrayType,
	size_t	ArraySize
>
class ArrayAccessor<ArrayType[ArraySize]>
{
public:
	typedef ArrayType			dataArrayType[ArraySize];

	template<size_t N>
	struct result_type{
		typedef typename BasicTraits<typename ArrayTypeAccessor<dataArrayType, N>::type>::const_param_type	type;
		typedef typename BasicTraits<typename ArrayTypeAccessor<dataArrayType, N>::type>::reference_type	reference;
	};

	ArrayAccessor(dataArrayType& data) : _data(data) {}

	typename result_type<1>::type
	operator()(size_t i) const {
		return _data[i];
	}

	typename result_type<1>::reference
	operator()(size_t i) {
		return _data[i];
	}

	typename result_type<2>::type
	operator()(size_t i, size_t j) const {
		return _data[i][j];
	}

	typename result_type<2>::reference
	operator()(size_t i, size_t j) {
		return _data[i][j];
	}

	typename result_type<3>::type
	operator()(size_t i, size_t j, size_t k) const {
		return _data[i][j][k];
	}

	typename result_type<3>::reference
	operator()(size_t i, size_t j, size_t k) {
		return _data[i][j][k];
	}

	typename result_type<4>::type
	operator()(size_t i, size_t j, size_t k, size_t l) const {
		return _data[i][j][k][l];
	}

	typename result_type<4>::reference
	operator()(size_t i, size_t j, size_t k, size_t l) {
		return _data[i][j][k][l];
	}

	typename result_type<5>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m) const {
		return _data[i][j][k][l][m];
	}

	typename result_type<5>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m) {
		return _data[i][j][k][l][m];
	}

	typename result_type<6>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n) const {
		return _data[i][j][k][l][m][n];
	}

	typename result_type<6>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n) {
		return _data[i][j][k][l][m][n];
	}

	typename result_type<7>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o) const {
		return _data[i][j][k][l][m][n][o];
	}

	typename result_type<7>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o) {
		return _data[i][j][k][l][m][n][o];
	}

	typename result_type<8>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n,
		size_t o, size_t p) const {
		return _data[i][j][k][l][m][n][o][p];
	}

	typename result_type<8>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n,
		size_t o, size_t p) {
		return _data[i][j][k][l][m][n][o][p];
	}
private:
	dataArrayType& _data;
};


/**
 * ArrayAccessor for std::vector<> containers
 */
template<
	typename ElemType
>
class ArrayAccessor<vector<ElemType> >
{
public:
	typedef vector<ElemType>		paramType;

	template<size_t N>
	struct result_type{
		typedef typename BasicTraits<typename StdVectorTypeAccessor<paramType ,N>::type>::const_param_type	type;
		typedef typename BasicTraits<typename StdVectorTypeAccessor<paramType ,N>::type>::reference_type	reference;
	};

	ArrayAccessor(vector<ElemType>& data) : _data(data) {}

	typename result_type<1>::type
	operator()(size_t i) const {
		return _data[i];
	}

	typename result_type<1>::reference
	operator()(size_t i) {
		return _data[i];
	}

	typename result_type<2>::type
	operator()(size_t i, size_t j) const {
		return _data[i][j];
	}

	typename result_type<2>::reference
	operator()(size_t i, size_t j) {
		return _data[i][j];
	}

	typename result_type<3>::type
	operator()(size_t i, size_t j, size_t k) const {
		return _data[i][j][k];
	}

	typename result_type<3>::reference
	operator()(size_t i, size_t j, size_t k) {
		return _data[i][j][k];
	}

	typename result_type<4>::type
	operator()(size_t i, size_t j, size_t k, size_t l) const {
		return _data[i][j][k][l];
	}

	typename result_type<4>::reference
	operator()(size_t i, size_t j, size_t k, size_t l) {
		return _data[i][j][k][l];
	}

	typename result_type<5>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m) const {
		return _data[i][j][k][l][m];
	}

	typename result_type<5>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m) {
		return _data[i][j][k][l][m];
	}

	typename result_type<6>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n) const {
		return _data[i][j][k][l][m][n];
	}

	typename result_type<6>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n) {
		return _data[i][j][k][l][m][n];
	}

	typename result_type<7>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o) const {
		return _data[i][j][k][l][m][n][o];
	}

	typename result_type<7>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o) {
		return _data[i][j][k][l][m][n][o];
	}

	typename result_type<8>::type
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n,
		size_t o, size_t p) const {
		return _data[i][j][k][l][m][n][o][p];
	}

	typename result_type<8>::reference
	operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n,
		size_t o, size_t p) {
		return _data[i][j][k][l][m][n][o][p];
	}
private:
	vector<ElemType>& _data;
};


/**
 * ArrayAccessor for T* (pointer) data
 */
template<
	typename ElemType
>
class ArrayAccessor<ElemType* >
{
public:
	typedef ElemType*											pointer_type;
	typedef typename BasicTraits<ElemType>::const_param_type	value_type;
	typedef typename BasicTraits<ElemType>::reference_type		reference_type;
	typedef vector<size_t>										dimensions_type;

	template<size_t N>
	struct result_type{
		typedef value_type		type;
		typedef reference_type	reference;
	};

	ArrayAccessor(pointer_type data) : _data(data) {}

	value_type operator()(size_t i) const {
		return _data[i];
	}

	reference_type operator()(size_t i) {
		return _data[i];
	}

private:
	pointer_type		_data;
};


/**
 * Class to linearize access to a container (e.g. Covert a(2,3) to a(2*k + 3))
 */
template<typename Accessor>
class LinearAccessor{
public:
	typedef Accessor accessor_type;
	typedef typename accessor_type::template result_type<1>::type 		value_type;
	typedef typename accessor_type::template result_type<1>::reference 	reference_type;
	typedef vector<size_t>												dimensions_type;

	LinearAccessor(accessor_type data): _data(data) {}
	LinearAccessor(accessor_type data, size_t d1): _data(data) {
		_dims.push_back(d1);
	}
	LinearAccessor(accessor_type data, size_t d1, size_t d2): _data(data) {
		_dims.push_back(d1);
		_dims.push_back(d2);
		prepare_dims();
	}
	LinearAccessor(accessor_type data, size_t d1, size_t d2, size_t d3): _data(data) {
		_dims.push_back(d1);
		_dims.push_back(d2);
		_dims.push_back(d3);
		prepare_dims();
	}
	LinearAccessor(accessor_type data, size_t d1, size_t d2, size_t d3, size_t d4): _data(data) {
		_dims.push_back(d1);
		_dims.push_back(d2);
		_dims.push_back(d3);
		_dims.push_back(d4);
		prepare_dims();
	}
	LinearAccessor(accessor_type data, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5): _data(data) {
		_dims.push_back(d1);
		_dims.push_back(d2);
		_dims.push_back(d3);
		_dims.push_back(d4);
		_dims.push_back(d5);
		prepare_dims();
	}
	LinearAccessor(accessor_type data, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6): _data(data) {
		_dims.push_back(d1);
		_dims.push_back(d2);
		_dims.push_back(d3);
		_dims.push_back(d4);
		_dims.push_back(d5);
		_dims.push_back(d6);
		prepare_dims();
	}
	LinearAccessor(accessor_type data, size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7): _data(data) {
		_dims.push_back(d1);
		_dims.push_back(d2);
		_dims.push_back(d3);
		_dims.push_back(d4);
		_dims.push_back(d5);
		_dims.push_back(d6);
		_dims.push_back(d7);
		prepare_dims();
	}

	value_type operator()(size_t i) const {
		return _data(i);
	}

	reference_type operator()(size_t i) {
		return _data(i);
	}

	value_type operator()(size_t i, size_t j) const {
		assert(_dims.size() > 0);
		return _data(i*_dims[0] + j);
	}

	reference_type operator()(size_t i, size_t j) {
		assert(_dims.size() > 0);
		return _data(i*_dims[0] + j);
	}

	value_type operator()(size_t i, size_t j, size_t k) const {
		assert(_dims.size() > 1);
		return _data(k * _dims[1] + i * _dims[0] + j);
	}

	reference_type operator()(size_t i, size_t j, size_t k) {
		assert(_dims.size() > 1);
		return _data(k * _dims[1] + i * _dims[0] + j);
	}

	value_type operator()(size_t i, size_t j, size_t k, size_t l) const {
		assert(_dims.size() > 2);
		return _data(l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	reference_type operator()(size_t i, size_t j, size_t k, size_t l) {
		assert(_dims.size() > 2);
		return _data(l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	value_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m) const {
		assert(_dims.size() > 3);
		return _data(m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	reference_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m) {
		assert(_dims.size() > 3);
		return _data(m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	value_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n) const {
		assert(_dims.size() > 4);
		return _data(n * _dims[4] + m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	reference_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n) {
		assert(_dims.size() > 4);
		return _data(n * _dims[4] + m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	value_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o) const {
		assert(_dims.size() > 5);
		return _data(o * _dims[5] + n * _dims[4] + m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	reference_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o) {
		assert(_dims.size() > 5);
		return _data(o * _dims[5] + n * _dims[4] + m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	value_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o, size_t p) const {
		assert(_dims.size() > 6);
		return _data(p * _dims[6] + o * _dims[5] + n * _dims[4] + m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}

	reference_type operator()(size_t i, size_t j, size_t k, size_t l, size_t m, size_t n, size_t o, size_t p) {
		assert(_dims.size() > 6);
		return _data(p * _dims[6] + o * _dims[5] + n * _dims[4] + m * _dims[3] + l * _dims[2] + k * _dims[1] + i * _dims[0] + j);
	}


private:
	accessor_type 		_data;
	dimensions_type		_dims;
	void prepare_dims() {
		for (signed long i = _dims.size() - 2; i >= 0; --i) {
			_dims[i] *= _dims[i + 1];
		}
	}
};

template <typename T>
inline ArrayAccessor<T> make_ArrayAccessor(T& data){
	return ArrayAccessor<T>(data);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1, size_t d2){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1, d2);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1, size_t d2, size_t d3){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1, d2, d3);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1, size_t d2, size_t d3, size_t d4){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1, d2, d3, d4);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1, size_t d2, size_t d3, size_t d4, size_t d5){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1, d2, d3, d4, d5);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1, d2, d3, d4, d5, d6);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6,size_t d7){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1, d2, d3, d4, d5, d6, d7);
}

template <typename T>
inline LinearAccessor<ArrayAccessor<T> > make_LinearAccessor(T& data,
	size_t d1, size_t d2, size_t d3, size_t d4, size_t d5, size_t d6, size_t d7, size_t d8){
	return LinearAccessor<ArrayAccessor<T> > (make_ArrayAccessor(data), d1, d2, d3, d4, d5, d6, d7, d8);
}

}

#endif /* ARRAYACCESSOR_HPP_ */
