/*
 * NormTraits.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 21/11/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef NORMTRAITS_HPP_
#define NORMTRAITS_HPP_

namespace tscbpt {
template<typename Type> struct NormTraits;
}

#include <complex>
#include "Matrix.hpp"

namespace tscbpt
{

/**
 * Traits class to get the return type of the norm operator
 */
template<typename Type>
struct NormTraits
{
	typedef Type	type;
};

/**
 * Traits class to get the return type of the norm operator.
 * In a complex element, the result of the norm is the template type.
 */
template<typename T>
struct NormTraits<complex<T> >
{
	typedef T		type;
};

template<class Derived, class Field>
typename NormTraits<Field>::type dist2(const Matrix_Base<Derived, Field>& a, const Matrix_Base<Derived, Field>& b){
	typedef Matrix_Base<Derived, Field>			matrix_type;
	typedef typename NormTraits<Field>::type	value_type;
	value_type tmp = value_type();
	assert(a.getRows() == b.getRows() && a.getCols() == b.getCols());
	if (matrix_type::shape::isHermitian) {
		for (size_t i = 0; i < a.getRows(); ++i) {
			// NOTE: std::norm is equivalent to norm2 (squared norm)!!!
			tmp += std::norm(a(i, i) - b(i, i));
			for (size_t j = i + 1; j < a.getCols(); ++j) {
				tmp += 2*std::norm(a(i, j) - b(i, j));
			}
		}
	} else {
		for (size_t i = 0; i < a.getRows(); ++i) {
			for (size_t j = 0; j < a.getCols(); ++j) {
				tmp += std::norm(a(i, j) - b(i, j));
			}
		}
	}
	return tmp;
}

template<class Derived, class Field>
typename NormTraits<Field>::type norm2(const Matrix_Base<Derived, Field>& a){
	typedef Matrix_Base<Derived, Field>			matrix_type;
	typedef typename NormTraits<Field>::type	value_type;
	value_type tmp = value_type();
	if (matrix_type::shape::isHermitian) {
		for (size_t i = 0; i < a.getRows(); ++i) {
			// NOTE: std::norm is equivalent to norm2 (squared norm)!!!
			tmp += std::norm(a(i, i));
			for (size_t j = i + 1; j < a.getCols(); ++j) {
				tmp += 2*std::norm(a(i, j));
			}
		}
	} else {
		for (size_t i = 0; i < a.getRows(); ++i) {
			for (size_t j = 0; j < a.getCols(); ++j) {
				tmp += std::norm(a(i, j));
			}
		}
	}
	return tmp;
}

inline NormTraits<double>::type norm2(double a){
	return a*a;
}

}

#endif /* NORMTRAITS_HPP_ */
