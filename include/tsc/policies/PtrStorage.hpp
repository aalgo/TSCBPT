/*
 * PtrStorage.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 03/02/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef PTRSTORAGE_HPP_
#define PTRSTORAGE_HPP_

namespace tscbpt{

/**
 * Simple class to wrap a simple native pointer
 */
template<class T>
struct DefaultPtrStorage{

	typedef T* pointerType;
	typedef T* strongPointerType;
	typedef T* weakPointerType;

protected:
	~DefaultPtrStorage(){}
};

}


#endif /* PTRSTORAGE_HPP_ */
