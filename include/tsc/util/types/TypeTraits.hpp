/*
 * TypeTraits.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 14/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef TYPETRAITS_HPP_
#define TYPETRAITS_HPP_

#include <vector>

/**
 * This file contains a set of specific traits classes.
 */
namespace tscbpt
{

struct NullType{};

template<typename A, typename B>
struct type_equals{
	static const bool value = false;
};

template<typename T>
struct type_equals<T,T>{
	static const bool value = true;
};

template <typename T>
struct AddPointer{
	typedef T*	type;
};

template <typename T>
struct AddReference{
	typedef T&	type;
};

template <typename T>
struct AddReference<T&>{
	typedef T&	type;
};

template <typename T>
struct AddConst{
	typedef const T	type;
};

template <typename T>
struct AddConst<T&>{
	typedef const T&	type;
};

template <typename T>
struct AddConst<const T>{
	typedef const T	type;
};

namespace {
template <typename T, bool small>
struct BasicTraits_helper{};

template <typename T, bool small>
struct BasicTraits_helper<T&, small>{
	typedef T& 			param_type;
	typedef const T& 	const_param_type;
};

template <typename T>
struct BasicTraits_helper<T*, true>{
	typedef T* 			param_type;
	typedef const T* 	const_param_type;
};

template <typename T>
struct BasicTraits_helper<T,true>{
	typedef T 			param_type;
	typedef T 			const_param_type;
};

template <typename T>
struct BasicTraits_helper<T,false>{
	typedef typename AddReference<T>::type 			param_type;
	typedef typename AddConst<param_type>::type		const_param_type;
};
}

template <typename T>
struct BasicTraits{
	static const bool isSmall = sizeof(T) <= sizeof(long);
	typedef typename BasicTraits_helper<T,isSmall>::param_type			param_type;
	typedef typename BasicTraits_helper<T,isSmall>::const_param_type	const_param_type;
	typedef typename AddReference<T>::type								reference_type;
	typedef typename AddConst<reference_type>::type						const_reference_type;
	typedef T															value_type;
};



template <typename T>
struct PointerTraits
{
	typedef T lastPointeeType;

	static const bool isPointer = false;
	static const size_t pointerCardinality = 0;
};

template <typename T>
struct PointerTraits<T*>
{
	typedef T 											pointeeType;
	typedef typename PointerTraits<T>::lastPointeeType 	lastPointeeType;

	static const bool isPointer = true;
	static const size_t pointerCardinality = PointerTraits<T>::pointerCardinality + 1;
	static T& dereference(T* arg) { return *arg;}
	static T& dereferenceAll(T* arg) {
		if(pointerCardinality > 1) return PointerTraits<T>::dereferenceAll(*arg);
		else return dereference(arg);
	}
};



template <typename T>
struct ArrayTraits{
	typedef T 							last_array_type;
	typedef NullType					array_elem_type;
	static const bool isArrayType 		= false;
	static const size_t arrayDims 	= 0;
};
template <typename T, size_t elems>
struct ArrayTraits<T[elems]>{
	typedef T														array_elem_type;
	typedef typename ArrayTraits<array_elem_type>::last_array_type	last_array_type;
	static const bool isArrayType = true;
	static const size_t num_ArrayElems = elems;
	static const size_t arrayDims = 1 + ArrayTraits<array_elem_type>::arrayDims;
};
template<typename T, size_t pos>
struct ArrayTypeAccessor{
	typedef typename ArrayTypeAccessor<typename ArrayTraits<T>::array_elem_type, pos - 1 >::type	type;
};
template<typename T>
struct ArrayTypeAccessor<T, 0>{
	typedef T	type;
};



template <typename T>
struct StdVectorTraits{
	typedef NullType					vector_elem_type;
	static const bool isStdVector		= false;
};
template <typename T>
struct StdVectorTraits<std::vector<T> >{
	typedef T								vector_elem_type;
	static const bool isStdVector			= true;
};
template<typename T, size_t pos>
struct StdVectorTypeAccessor{
	typedef typename StdVectorTypeAccessor<typename StdVectorTraits<T>::vector_elem_type, pos - 1 >::type	type;
};
template<typename T>
struct StdVectorTypeAccessor<T, 0>{
	typedef T	type;
};


template <typename Type>
struct TypeTraits:
	public PointerTraits<Type>,
	public BasicTraits<Type>,
	public ArrayTraits<Type>
{
};

}

#endif /* TYPETRAITS_HPP_ */
