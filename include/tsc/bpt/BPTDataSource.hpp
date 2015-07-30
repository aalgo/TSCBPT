/*
 * BPTDataSource.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 07/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef BPTDATASOURCE_HPP_
#define BPTDATASOURCE_HPP_

#include <cstddef>
#include <cassert>
#include <functional>
#include <boost/array.hpp>
#include <boost/multi_array.hpp>
#include <boost/static_assert.hpp>
#include <set>
#include <tsc/util/Algorithms.h>

namespace tscbpt
{

template<typename TNodePointer, size_t Dims, class Functor = NullFunctor<TNodePointer> >
class BPTDataSource
{
public:
	typedef TNodePointer							NodePointer;
	typedef Functor									functor_type;
	typedef typename functor_type::result_type		value_type;
	typedef boost::multi_array<value_type, Dims>	array_type;
	typedef value_type*								iterator;

private:
	template <typename ValueType, size_t NDims>
	struct access
	{
		template <typename T>
		ValueType& operator()(T& _array, NodePointer np) const ;
	};

	template <typename ValueType>
	struct access<ValueType, 1>
	{
		template <typename T>
		ValueType& operator()(T& array, NodePointer np) const {
			return array[np->getModel().getDim(0)];
		}
	};

	template <typename ValueType>
	struct access<ValueType, 2>
	{
		template <typename T>
		ValueType& operator()(T& array, NodePointer np) const {
			return array[np->getModel().getDim(1)][np->getModel().getDim(0)];
		}
	};

	template <typename ValueType>
	struct access<ValueType, 3>
	{
		template <typename T>
		ValueType& operator()(T& array, NodePointer np) const {
			return array[np->getModel().getDim(0)][np->getModel().getDim(2)][np->getModel().getDim(1)];
		}
	};

	template <typename ValueType>
	struct access<ValueType, 4>
	{
		template <typename T>
		ValueType& operator()(T& array, NodePointer np) {
			return array[np->getModel().getDim(3)][np->getModel().getDim(2)][np->getModel().getDim(1)][np->getModel().getDim(
				0)];
		}
	};

public:
	typedef access<value_type, Dims>				accessor_type;

	template<class LeavesItFirst, class LeavesItLast, class PrunedSet, class TDimsIt>
	BPTDataSource(LeavesItFirst leavesFirst, LeavesItLast leavesLast, const PrunedSet& prunedSet, TDimsIt dimsIt, functor_type f = functor_type()) {
		using namespace boost;
		boost::array<typename array_type::index, Dims> extents;
		for (size_t i = 0; i < Dims; ++i) {
			extents[i] = *dimsIt++;
		}
		_leaves.resize(extents);
		populateLeaves(leavesFirst, leavesLast, prunedSet, f);
	}

	template <class PrunedSet, class TDimsIt>
	BPTDataSource(const PrunedSet& prunedSet, TDimsIt dimsIt, functor_type f = functor_type()) {
		using namespace boost;
		boost::array<typename array_type::index, Dims> extents;
		for (size_t i = 0; i < Dims; ++i) {
			extents[i] = *dimsIt++;
		}
		_leaves.resize(extents);
		populateLeaves(prunedSet, f);
	}

	iterator begin(){
		return _leaves.data();
	}

	iterator end() {
		return _leaves.data() + _leaves.num_elements();
	}

	// TODO: Generalize method and probably move away from here
	boost::multi_array<int, Dims-1>	getSliceChanges() const {
		BOOST_STATIC_ASSERT(Dims==3);
		boost::multi_array<int, Dims-1> data;
		boost::array<typename array_type::index, Dims> extents;
		for (size_t i = 1; i < Dims; ++i) {
			extents[i-1] = _leaves.shape()[i];
		}
		data.resize(extents);

		for(size_t i = 0; i < _leaves.shape()[0]-1; ++i){
			for(size_t j = 0; j < _leaves.shape()[1]; ++j){
				for(size_t k = 0; k < _leaves.shape()[2]; ++k){
					if(_leaves[i][j][k] != _leaves[i+1][j][k]) data[j][k]++;
				}
			}
		}
		return data;
	}

	// TODO: Generalize method and probably move away from here
	template<class Set>
	double getRegionsInSlice(size_t slice, Set& regions) const {
		double tmp = 0.0;
		for (size_t j = 0; j < _leaves.shape()[1]; ++j) {
			for (size_t k = 0; k < _leaves.shape()[2]; ++k) {
				if (regions.find(_leaves[slice][j][k]) == regions.end()){
					regions.insert(_leaves[slice][j][k]);
					tmp += _leaves[slice][j][k]->getModel().getSubnodes();
				}
			}
		}
		return tmp / regions.size();
	}

private:
	array_type		_leaves;
	static const accessor_type accessor;

	template<class LeavesItFirst, class LeavesItLast, class PrunedSet>
	void populateLeaves(LeavesItFirst leavesFirst, LeavesItLast leavesLast, const PrunedSet& prunedSet, functor_type func){
		for(; leavesFirst != leavesLast; ++leavesFirst){
			NodePointer np = *leavesFirst;
			while(prunedSet.find(np) == prunedSet.end() && np->getFather()) np = np->getFather();
			accessor(_leaves, *leavesFirst) = func(np);
		}
	}

	template<class NodePointerSet>
	void populateLeaves(const NodePointerSet& prunedSet, functor_type func){
		typedef typename NodePointerSet::const_iterator 		pruned_iterator;
		for(pruned_iterator it = prunedSet.begin(); it != prunedSet.end(); ++it){
			NodePointerSet leaves = NodePointerSet();
			addLeaves(*it, leaves);
			value_type value = func(*it);
			for(pruned_iterator itl = leaves.begin(); itl != leaves.end(); ++itl){
				accessor(_leaves, *itl) = value;
			}
		}
	}

	template<class PrunedSet>
	void addLeaves(NodePointer np, PrunedSet& leaves){
		assert(np);
		if(np->isLeaf()){
			leaves.insert(np);
		} else {
			addLeaves(np->getLeftSoon(), leaves);
			addLeaves(np->getRightSoon(), leaves);
		}
	}
};

template<typename TNodePointer, size_t Dims, class Functor>
const typename BPTDataSource<TNodePointer, Dims, Functor>::accessor_type BPTDataSource<TNodePointer, Dims, Functor>::accessor = typename BPTDataSource<TNodePointer, Dims, Functor>::accessor_type();

}

#endif /* BPTDATASOURCE_HPP_ */
