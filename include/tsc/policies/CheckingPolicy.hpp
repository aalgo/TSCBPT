/*
 * CheckingPolicy.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 01/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef CHECKINGPOLICY_HPP_
#define CHECKINGPOLICY_HPP_

#include <algorithm>
#include <stddef.h>
#include <cassert>
#include <iostream>

namespace tscbpt
{

class NullCheckingPolicy
{
public:
	NullCheckingPolicy(){}

	//================================== TESTS ======================================
	/**
	 * Tests are useful to manage checking code inside conditionals. E.g.:
	 * if(CheckingPolicy.warnCheck(var.checkState()) { //Code... }
	 * The code will be removed for a NullCheckingPolicy since condition always false
	 * whereas for a FullCheckingPolicy it will be executed as a normal conditional.
	 *
	 * Similar to logging classes, different levels are provided to differentiate the
	 * severity of the tests.
	 */
	template <typename T>
	inline bool infoCheck(T ) const { return false; }

	template <typename T>
	inline bool warnCheck(T ) const { return false; }

	template <typename T>
	inline bool errorCheck(T ) const { return false; }

	template <typename T>
	inline bool fatalCheck(T ) const { return false; }

	//=================================== ASSERT ====================================
	template <typename T> inline void infoAssert(T) const {}
	template <typename T> inline void warnAssert(T) const {}
	template <typename T> inline void errorAssert(T) const {}
	template <typename T> inline void fatalAssert(T) const {}

	//================================ SIZE CHECKING ================================
	template <typename Size, typename Dimensions>
	inline void sizeMatches(Size , Dimensions& ) const {}

	template <typename Size>
	inline void sizeMatches(Size , Size ) const {}

	template <typename Size>
	inline void sizeMatches(Size , Size , Size ) const {}

	template <typename Size>
	inline void sizeMatches(Size , Size , Size , Size ) const {}

	template <typename Dimensions>
	inline void dimensionsEqual(Dimensions , Dimensions ) const {}

	//============================== POINTER CHECKING ===============================
	template <typename Pointer>
	inline void pointerIsNotNull(Pointer ) const {}

	//================================ FILE CHECKING ================================
	inline void ioStateOK(std::ios&) const {}
};

class FullCheckingPolicy : public NullCheckingPolicy
{
public:
	FullCheckingPolicy(){}

	//================================== TESTS ======================================
	template <typename T>
	inline bool infoCheck(T arg) const {
		return arg;
	}

	template <typename T>
	inline bool warnCheck(T arg) const {
		return arg;
	}

	template <typename T>
	inline bool errorCheck(T arg) const {
		return arg;
	}

	template <typename T>
	inline bool fatalCheck(T arg) const {
		return arg;
	}

	//=================================== ASSERT ====================================
	template <typename T> inline void infoAssert(T arg) const {
		assert(arg);
	}
	template <typename T> inline void warnAssert(T arg) const {
		assert(arg);
	}
	template <typename T> inline void errorAssert(T arg) const {
		assert(arg);
	}
	template <typename T> inline void fatalAssert(T arg) const {
		assert(arg);
	}

	//================================ SIZE CHECKING ================================
	template<typename Size, typename Dimensions>
	inline void sizeMatches(Size totalSize, Dimensions& dims) const {
		Size total = dims[0];
		for(Size i = 1; i<dims.size(); i++)  total *= dims[i];
		if(totalSize != total){
			std::__throw_length_error("ERROR: Size from dimensions differs from data length");
		}
	}

	template <typename Size, typename Dimensions>
	inline void sizeMatches(Size totalSize, Size dim1) const {
		if (totalSize != dim1) {
			std::__throw_length_error("ERROR: Size from dimensions differs from data length");
		}
	}

	template <typename Size, typename Dimensions>
	inline void sizeMatches(Size totalSize, Size dim1, Size dim2) const {
		if (totalSize != dim1 * dim2) {
			std::__throw_length_error("ERROR: Size from dimensions differs from data length");
		}
	}

	template <typename Size, typename Dimensions>
	inline void sizeMatches(Size totalSize, Size dim1, Size dim2, Size dim3) const {
		if (totalSize != dim1 * dim2 * dim3) {
			std::__throw_length_error("ERROR: Size from dimensions differs from data length");
		}
	}

	template <typename Dimension1, typename Dimension2>
	inline void dimensionsEqual(Dimension1 d1, Dimension2 d2) const {
		if(d1 != d2){
			std::__throw_runtime_error("Dimensions do not match");
		}
	}

	//============================== POINTER CHECKING ===============================
	template <typename Pointer>
	inline void pointerIsNotNull(Pointer p) const {
		if (!p) {
			std::__throw_runtime_error("Pointer is null when it should not be");
		}
	}

	//================================ FILE CHECKING ================================
	inline void ioStateOK(std::ios& io) const {
		if(io.fail()){
			std::__throw_ios_failure("IO state failed");
		}
	}
};



}

#endif /* CHECKINGPOLICY_HPP_ */
