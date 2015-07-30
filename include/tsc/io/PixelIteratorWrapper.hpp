/*
 * PixelIteratorWrapper.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 18/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef PIXELITERATORWRAPPER_HPP_
#define PIXELITERATORWRAPPER_HPP_

#include <cstddef>
#include "../image/Pixel.hpp"

namespace tscbpt
{

template<class InputIterator, size_t NDims>
struct PixelIteratorWrapper
{

	typedef InputIterator						iterator;
	typedef typename iterator::value_type 		value_type;

	typedef PixelIteratorWrapper<iterator, NDims>	ThisType;

	static const size_t	dimensions		= NDims;

	PixelIteratorWrapper(iterator ait) :
		it(ait) {
	}

	Pixel<value_type, NDims> operator*() {
		return ConstructorCaller< Pixel<value_type, NDims>, NDims >::call(it);
	}

	ThisType operator++(int) {
		iterator tmp(*this);
		++*this;
		return tmp;
	}

	ThisType& operator++() {
		++it;
		return *this;
	}

	template <typename RowType>
	RowType getRow() const {
		return this->it.getRow();
	}

	template <typename ColType>
	ColType getCol() const {
		return this->it.getCol();
	}

	template <typename SliceType>
	SliceType getSlice() const {
		return this->it.getSlice();
	}

	bool operator==(const ThisType& rhs) const {
		return this->it == rhs.it;
	}

	bool operator!=(const ThisType& rhs) const {
		return this->it != rhs.it;
	}
private:
	iterator it;
	template <class T, size_t NumDims>
	struct ConstructorCaller
	{
		template<typename Iterator>
		static T call(Iterator& it);
	};
	template <class T>
	struct ConstructorCaller<T, 1>
	{
		template<typename Iterator>
		static T call(Iterator& it) {
			return T(*it, static_cast<float> (it.getRow()));
		}
	};
	template <class T>
	struct ConstructorCaller<T, 2>
	{
		template<typename Iterator>
		static T call(Iterator& it) {
			return T(*it, static_cast<float> (it.getCol()), static_cast<float> (it.getRow()));
		}
	};
	template <class T>
	struct ConstructorCaller<T, 3>
	{
		template<typename Iterator>
		static T call(Iterator& it) {
			return T(*it, static_cast<float> (it.getSlice()), static_cast<float> (it.getCol()),
				static_cast<float> (it.getRow()));
		}
	};
};

template<class InputIterator>
PixelIteratorWrapper<InputIterator, 1> make_pixel_iterator1D(InputIterator ait){
	return PixelIteratorWrapper<InputIterator, 1>(ait);
}

template<class InputIterator>
PixelIteratorWrapper<InputIterator, 2> make_pixel_iterator2D(InputIterator ait){
	return PixelIteratorWrapper<InputIterator, 2>(ait);
}

template<class InputIterator>
PixelIteratorWrapper<InputIterator, 3> make_pixel_iterator3D(InputIterator ait){
	return PixelIteratorWrapper<InputIterator, 3>(ait);
}

}

#endif /* PIXELITERATORWRAPPER_HPP_ */
