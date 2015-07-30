/*
 * Pixel.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 09/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef PIXEL_HPP_
#define PIXEL_HPP_

#include <cstddef>
#include <boost/static_assert.hpp>
#include <tsc/data/Container.hpp>
#include <tsc/data/Position.hpp>


namespace tscbpt {

using namespace std;

template<
	typename 		PixelType,
	size_t 			Dimension = 2,
	typename 		CoordinateType = float>
class Pixel :
	public Container<PixelType>,
	public Position<Dimension, CoordinateType>{

public:
	typedef Position<Dimension, CoordinateType>		PositionType;

	Pixel(const PixelType& value, const PositionType& pos) :
		Container<PixelType> (value), Position<Dimension, CoordinateType> (pos) {
	}
	Pixel(const PixelType& value, CoordinateType const * pos):
		Container<PixelType>(value),
		Position<Dimension, CoordinateType> (pos) {
	}
};

template <
	typename PixelType,
	typename CoordinateType>
class Pixel<PixelType, 1, CoordinateType>:
	public Container<PixelType>,
	public Position<1, CoordinateType>
{
public:
	typedef Position<1, CoordinateType>		PositionType;
	Pixel(const PixelType& value, const PositionType& pos) :
		Container<PixelType> (value), Position<1, CoordinateType> (pos) {
	}
	Pixel(const PixelType& value, CoordinateType const * pos) :
		Container<PixelType> (value), Position<1, CoordinateType> (pos) {
	}
	Pixel(const PixelType& value, CoordinateType x) :
		Container<PixelType> (value), Position<1, CoordinateType> (x) {
	}
};

template <
	typename PixelType,
	typename CoordinateType>
class Pixel<PixelType, 2, CoordinateType>:
	public Container<PixelType>,
	public Position<2, CoordinateType>
{
public:
	typedef Position<2, CoordinateType>		PositionType;
	Pixel(const PixelType& value, const PositionType& pos) :
		Container<PixelType> (value), Position<2, CoordinateType> (pos) {
	}
	Pixel(const PixelType& value, CoordinateType const * pos) :
		Container<PixelType> (value), Position<2, CoordinateType> (pos) {
	}
	Pixel(const PixelType& value, CoordinateType x, CoordinateType y) :
		Container<PixelType> (value), Position<2, CoordinateType> (x, y) {
	}
};

template <
	typename PixelType,
	typename CoordinateType>
class Pixel<PixelType, 3, CoordinateType>:
	public Container<PixelType>,
	public Position<3, CoordinateType>
{
public:
	typedef Position<3, CoordinateType>		PositionType;
	Pixel(const PixelType& value, const PositionType& pos) :
		Container<PixelType> (value), Position<3, CoordinateType> (pos) {
	}
	Pixel(const PixelType& value, CoordinateType const * pos) :
		Container<PixelType> (value), Position<3, CoordinateType> (pos) {
	}
	Pixel(const PixelType& value, CoordinateType x, CoordinateType y, CoordinateType z) :
		Container<PixelType> (value), Position<3, CoordinateType> (x, y, z) {
	}
};

template<class PixelType, size_t Dimension, typename CoordinateType, class charT, class traits>
  std::basic_ostream<charT,traits>&
    operator<< (std::basic_ostream<charT,traits>& ostr, const Pixel<PixelType, Dimension, CoordinateType>& rhs){
	ostr<<"{(";
	size_t i = 0;
	for(; i<Dimension-1; i++){
		ostr<<rhs.getDim(i)<<",";
	}
	ostr<<rhs.getDim(Dimension-1)<<"):"<<rhs.getValue()<<"}";
	return ostr;
}

}

#endif /* PIXEL_HPP_ */
