/*
 * DissimilarityMeasure.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 23/03/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef DISSIMILARITYMEASURE_HPP_
#define DISSIMILARITYMEASURE_HPP_

#include <functional>
#include "../util/ArmadilloWrapper.hpp"
#include <tsc/bpt/models/concepts/HasLogDetAverage.h>
#include <boost/concept_check.hpp>
#include <omp.h>
#include <cmath>

namespace tscbpt
{

struct DissimilarityMeasure {};

struct SymmetricDissimilarityMeasure: public DissimilarityMeasure
{
	static const bool isSymmetric = true;
};

struct NonSymmetricDissimilarityMeasure: public DissimilarityMeasure
{
	static const bool isSymmetric = false;
};

template <class NodePointer, class ValueType, class Diss1, class Diss2>
struct MixedDissimilarityMeasure: public SymmetricDissimilarityMeasure, public std::binary_function<
	NodePointer, NodePointer, ValueType>
{
	MixedDissimilarityMeasure(Diss1 diss1 = Diss1(), Diss2 diss2 = Diss2(), double sigma_f = 10):
		d1(diss1), d2(diss2), sf(sigma_f) {
		assert(sigma_f>=0);
	}

	ValueType operator()(NodePointer a, NodePointer b) const {
		ValueType res1 = d1(a,b);
		ValueType res2 = d2(a,b);

		double n = sqrt((a->getModel().getSubnodes() * b->getModel().getSubnodes()));
		double beta = exp(-n/sf);
		return beta*res1 + (1-beta)*res2;
	}
private:
	Diss1 d1;
	Diss2 d2;
	double sf;
};

}

#endif /* DISSIMILARITYMEASURE_HPP_ */
