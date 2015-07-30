/*
 * Container.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 11/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef CONTAINER_HPP_
#define CONTAINER_HPP_

#include "Contains.hpp"

namespace tscbpt {

template<typename DataType>
class Container : public Contains<DataType, Container<DataType> > {
protected:
	DataType _value;
public:
	typedef DataType	contained_type;
	typedef DataType	elem_type;

	Container(const DataType& value): _value(value) {}

    const DataType& getValue() const
    {
        return _value;
    }

    DataType& getValue()
    {
    	return _value;
    }

    void setValue(const DataType& value)
    {
        this->_value = value;
    }

};

}

#endif /* CONTAINER_HPP_ */
