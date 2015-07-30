/*
 * Algorithms.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 05/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef ALGORITHMS_H_
#define ALGORITHMS_H_

#include <cstddef>
#include <complex>
#include <functional>
#include <vector>

namespace tscbpt {

template<class InputIterator1, class InputIterator2, class T>
T& average(InputIterator1 first, InputIterator2 last, T& output){
	output=*first;
	size_t elems = 0;
	if(!(first != last)) return output;
	for(++first; first != last; ++first){
		output += *first;
		elems++;
	}
	output /= elems;
	return output;
}

template<class InputIterator1, class InputIterator2, class Measure, typename ValueType>
ValueType& averageUnaryMeasure(InputIterator1 first, InputIterator2 last, Measure m, ValueType& value){
	size_t elems = 0;
	while(first != last){
		value += m(*first);
		++elems;
		++first;
	}
	return value /= elems;
}

template<class InputIterator1, class InputIterator2, class InputIterator3, class InputIterator4, class Measure, typename ValueType>
ValueType& averageBinaryMeasure(InputIterator1 first1, InputIterator2 last1, InputIterator3 first2, InputIterator4 last2, Measure m, ValueType& value){
	size_t elems = 0;
	while(first1 != last1 && first2 != last2){
		value += m(*first1, *first2);
		++elems;
		++first1;
		++first2;
	}
	return value /= elems;
}

template<class InType, class OutType>
struct convertComplex : public std::unary_function<std::complex<InType>, std::complex<OutType> >
{
	inline std::complex<OutType> operator()(const std::complex<InType>& in){
		return std::complex<OutType>(
			static_cast<OutType>(in.real()),
			static_cast<OutType>(in.imag()));
	}
};

template<class InType, class OutType>
struct convertVectorComplex : public std::unary_function<std::vector<complex<InType> >, std::vector<complex<OutType> > >
{
	inline std::vector<std::complex<OutType> > operator()(const std::vector<std::complex<InType> >& in){
		typedef typename std::vector<std::complex<InType> >::const_iterator InIterator;
		std::vector<std::complex<OutType> > out;
		for(InIterator it = in.begin(); it != in.end(); ++it){
			out.push_back(std::complex<OutType>(
				static_cast<OutType>((*it).real()),
				static_cast<OutType>((*it).imag())));
		}
		return out;
	}
};

template<class InType, class OutType, int size>
struct convertVectorComplexFixed : public std::unary_function<std::vector<std::complex<InType> >, std::vector<std::complex<OutType> > >
{
	inline std::vector<std::complex<OutType> > operator()(const std::vector<std::complex<InType> >& in){
		std::vector<std::complex<OutType> > out;
		for(int i = 0; i < size; ++i){
			out.push_back(std::complex<OutType>(
				static_cast<OutType>(in[i].real()),
				static_cast<OutType>(in[i].imag())));
		}
		return out;
	}
};

template<typename InType, typename OutType>
inline std::complex<OutType>& convertCmpx(const std::complex<InType>& in, std::complex<OutType>& out){
	out.real(OutType(in.real()));
	out.imag(OutType(in.imag()));
	return out;
}

template<typename T>
struct NullFunctor: public std::unary_function<T,T>{
	inline T& operator()(T& t) const {
		return t;
	}

	inline const T& operator()(const T& t) const {
		return t;
	}
};


// Assumes that input scattering vector is (HH, HV, VV)
struct HV3DScatteringVector2Pauli{
	template<typename T>
	T operator()(const T& value){
		T tmp = value;
		tmp[0] = (value[0] + value[2]);
		tmp[0] *= (1.0 / sqrt(2.0));
		tmp[1] = (value[0] - value[2]);
		tmp[1] *= (1.0 / sqrt(2.0));
		tmp[2] = (value[1]);
		tmp[2] *= (2.0 / sqrt(2.0));
		return tmp;
	}
};

// Assumes that input scattering vector is (HH, HV, VV)
struct HV3DScatteringVector2PauliMod{
	template<typename T>
	T operator()(const T& value){
		T tmp = value;
		tmp[0] = (value[0] + value[2]);
		tmp[0] *= (1.0 / sqrt(2.0));
		tmp[1] = (value[0] - value[2]);
		tmp[1] *= (1.0 / sqrt(2.0));
		tmp[2] = (value[1]);
		tmp[2] *= (1.0 / sqrt(2.0));
		return tmp;
	}
};

// Assumes that input scattering vector is (HH, HV, VV)
struct HV3DScatteringVector{
	template<typename T>
	T operator()(const T& value){
		T tmp = value;
		tmp[1] *= sqrt(2);
		return tmp;
	}
};

// Does nothing
struct NoOpScatteringVector{
	template<typename T>
	T operator()(const T& value){
		T tmp = value;
		return tmp;
	}
};

// Assumes that input scattering vector is (HH, HV, VH, VV) (Alphabetically sorted)
struct MonostaticScatteringVector{
	static const size_t Channels = 4;
	template<typename T>
	T operator()(const T& value){
		size_t out_size = value.size() - value.size() / Channels;
		T tmp = T(out_size);
		size_t j = 0;
		for(size_t i = 0; i < value.size() ; ++i){
			tmp[j] += value[i];
			if (((i + Channels - 2) % Channels) == 0) {
				tmp[j] *= sqrt(2) / 2;
			}
			if (((i + Channels - 1) % Channels) != 0) {
				++j;
			}
		}
		return tmp;
	}
};

// Assumes that input scattering vector is (HH, HV, VH, VV) (Alphabetically sorted)
struct MonostaticPauliScatteringVector{
	static const size_t Channels = 4;
	template<typename T>
	T operator()(const T& value){
		size_t out_size = value.size() - value.size() / Channels;
		T tmp = T(out_size);
		for(size_t i = 0; i*Channels < value.size(); ++i){
			tmp[i*(Channels-1)]   = (value[i*Channels] + value[i*Channels+3])/sqrt(2);
			tmp[i*(Channels-1)+1] = (value[i*Channels] - value[i*Channels+3])/sqrt(2);
			tmp[i*(Channels-1)+2] = (value[i*Channels+1] + value[i*Channels+2])/sqrt(2);
		}
		return tmp;
	}
};


}
#endif /* ALGORITHMS_H_ */
