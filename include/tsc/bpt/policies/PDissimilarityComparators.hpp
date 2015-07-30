/*
 * PDissimilarityComparators.hpp
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

#ifndef PDISSIMILARITYCOMPARATORS_HPP_
#define PDISSIMILARITYCOMPARATORS_HPP_


#include <functional>

namespace tscbpt{

/**
 * This file contains different functors to manage the sorting of
 * dissimilarity sets
 */

template<class T>
struct pdiss_value_less:
	public std::binary_function<T, T, bool> {
	bool operator()(T const __x, T const __y) const {
		return __x->getDissimilarityValue() < __y->getDissimilarityValue() ||
				(__x->getDissimilarityValue() == __y->getDissimilarityValue() &&
						__x < __y);
	}
};

}

#endif /* PDISSIMILARITYCOMPARATORS_HPP_ */
