/*
 * NoOpConverter.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 06/06/2012
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef NOOPCONVERTER_HPP_
#define NOOPCONVERTER_HPP_

#include <functional>

namespace tscbpt
{

/**
 * Converter that does nothing.
 * Used as a default template parameter definition.
 */
template<typename T>
struct NoOpConverter: public std::unary_function<T, T>{
	T& operator()(T& t) const {
		return t;
	}
};

}

#endif /* NOOPCONVERTER_HPP_ */
