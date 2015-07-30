/*
 * VectorDissimilarityMeasures.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 31/10/2012
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef VECTORDISSIMILARITYMEASURES_HPP_
#define VECTORDISSIMILARITYMEASURES_HPP_

/**
 * This file defines generic dissimilarity measures for Vector models
 */

#include "DissimilarityMeasure.hpp"

namespace tscbpt {

template <class NodePointer, class ValueType>
struct DiagonalRevisedWishartVectorDissimilarityMeasure: public SymmetricDissimilarityMeasure, public std::binary_function<
	NodePointer, NodePointer, ValueType>
{
	inline ValueType operator()(NodePointer a, NodePointer b) const {

		static const ValueType DRW_MIN_THRESHOLD = 1e-12;

		ValueType res = ValueType();
		for(size_t i = 0; i < a->getModel().getNumElems(); ++i){
			ValueType tmp = max(a->getModel()(i), DRW_MIN_THRESHOLD) / max(b->getModel()(i), DRW_MIN_THRESHOLD);
			res += tmp + 1.0 / tmp;
		}
		return res * (a->getModel().getSubnodes() + b->getModel().getSubnodes());
	}
};

} /* namespace tscbpt */
#endif /* VECTORDISSIMILARITYMEASURES_HPP_ */
