/*
 * Contains.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 12/05/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef CONTAINS_HPP_
#define CONTAINS_HPP_

namespace tscbpt
{

template<typename Type, class Derived>
class Contains
{
public:
	Type& getValue(){
		return static_cast<Derived*>(this)->getValue();
	}

	const Type& getValue() const {
		return static_cast<Derived*> (this)->getValue();
	}
};

}

#endif /* CONTAINS_HPP_ */
