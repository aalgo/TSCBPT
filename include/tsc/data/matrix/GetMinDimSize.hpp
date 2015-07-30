/*
 * GetMinDimSize.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 20/11/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef GETMINDIMSIZE_HPP_
#define GETMINDIMSIZE_HPP_



// *************** Definitions ******************

#include <functional>

namespace tscbpt{

/**
 * Generic functor to get the minimum size of all the dimensions of an array
 */
template<typename T>
// TODO: Evaluate if this indirection level is REALLY needed
struct GetMinDimSize: public std::unary_function<T,size_t> {};

}



// ************* Implementation *****************

#include <tsc/util/types/WrapperOf.hpp>

namespace tscbpt{

/**
 * Generic functor to get the minimum size of all the dimensions of an array.
 * Template specialization for a generic Wrapper or Decorator class.
 */
template<class Base>
struct GetMinDimSize<WrapperOf<Base> >:
public std::unary_function<WrapperOf<Base> ,size_t> {
	inline size_t operator()(const WrapperOf<Base>& m) const {
		return GetMinDimSize<Base>()(static_cast<Base&>(*this));
	}
};

}

#endif /* GETMINDIMSIZE_HPP_ */
