/*
 * ModelMerge.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 12/09/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef MODELMERGE_HPP_
#define MODELMERGE_HPP_

#include<functional>
#include<boost/concept_check.hpp>
#include<tsc/bpt/models/concepts/CanBeMerged.h>

namespace tscbpt{

template <class NodePointer, class RegionModel>
struct ModelMerge: public std::binary_function<NodePointer, NodePointer, RegionModel>
{
	inline RegionModel operator()(NodePointer a, NodePointer b) const {
		BOOST_CONCEPT_ASSERT((concept::CanBeMerged< typeof(a->getModel()) >));
		return a->getModel().merge(b->getModel());
	}
};

template <class NodePointer, class RegionModel>
struct ModelUnmerge: public std::binary_function<NodePointer, NodePointer, RegionModel>
{
	inline RegionModel operator()(NodePointer a, NodePointer b) const {
		BOOST_CONCEPT_ASSERT((concept::CanBeMerged< typeof(a->getModel()) >));
		return a->getModel().unmerge(b->getModel());
	}
};

}

#endif /* MODELMERGE_HPP_ */
