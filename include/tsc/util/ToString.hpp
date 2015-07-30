/*
 * ToString.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 14/02/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef TOSTRING_HPP_
#define TOSTRING_HPP_

#include <iostream>
#include <sstream>
#include <string>

namespace tscbpt {

template <class T>
inline std::string to_string (const T& t)
{
	std::stringstream ss;
	ss << t;
	return ss.str();
}

} /* namespace tscbpt */

#endif /* TOSTRING_HPP_ */
