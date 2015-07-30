/*
 * ContainerValueType.hpp
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

#ifndef CONTAINERVALUETYPE_HPP_
#define CONTAINERVALUETYPE_HPP_

namespace tscbpt
{

/**
 * Generic type traits class to get the value type of a container.
 * May be specialized for other types.
 */
template<typename T>
struct ContainerValueType{
	typedef typename T::value_type	type;
};

}

#endif /* CONTAINERVALUETYPE_HPP_ */
