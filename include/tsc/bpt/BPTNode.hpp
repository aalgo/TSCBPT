/* 
 * BPTNode.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 5/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef _BPTNODE_HPP
#define	_BPTNODE_HPP


// *************** Definitions ******************

namespace tscbpt{

template
<
	class 					TRegionModel,
	typename				DissimilaritySetTypeContainer,
	typename 				TId,
	template<class> class 	StoragePolicy
>
class BPTNode;

}



// ************* Implementation *****************

#include <set>
#include <vector>
#include <stdint.h>
#include "BPTDissimilarity.hpp"
#include "BPTFrame.hpp"
#include <tsc/policies/policies.h>
#include <boost/concept_check.hpp>

namespace tscbpt{

using namespace std;

template
<
	class 					TRegionModel,
	// DissimilaritySetTypeContainer refers to a type containing the DissimilaritySet type (typename DissimilaritySetTypeContainer::DissimilaritySet)
	typename				DissimilaritySetTypeContainer,
	typename 				TId						= uint32_t,
	template<class> class 	StoragePolicy 			= DefaultNativeStoragePolicy
>
class BPTNode:
	public StoragePolicy<BPTNode<TRegionModel, DissimilaritySetTypeContainer, TId, StoragePolicy> >
{

public:
	// Typedefs from template parameters
	typedef BPTNode<TRegionModel, DissimilaritySetTypeContainer, TId, StoragePolicy>	ThisType;
	typedef TRegionModel 									RegionModel;
	typedef typename DissimilaritySetTypeContainer::DissimilaritySet
															DissimilaritySet;
	typedef TId												IDType;

	typedef StoragePolicy<ThisType>							StoragePol;
	typedef typename StoragePol::StrongPointerType			StrongNodePointer;
	typedef typename StoragePol::WeakPointerType			WeakNodePointer;

	typedef vector<StrongNodePointer>						NeighborsVector;

private:
    RegionModel _model;
    WeakNodePointer _father;
    StrongNodePointer _leftSoon, _rightSoon;
    DissimilaritySet _dissimilarities;
    IDType			_id;
    static IDType	next_id;

public:
    BOOST_CONCEPT_ASSERT((boost::CopyConstructible<RegionModel>));
    BPTNode(const RegionModel& model) :
		_model(model), _father(NULL), _leftSoon(NULL), _rightSoon(NULL), _id(next_id++) {
	}

    /**
     * Generic BPTNode creation
     */
    template<typename T>
    BPTNode(T modelParam) :
		_model(modelParam), _father(NULL), _leftSoon(NULL), _rightSoon(NULL), _id(next_id++) {
	}

    BPTNode(const RegionModel& model, StrongNodePointer leftSoon, StrongNodePointer rightSoon) :
		_model(model), _father(NULL), _leftSoon(leftSoon), _rightSoon(rightSoon), _id(next_id++) {
	}

    IDType getId() const {
		return _id;
	}

    WeakNodePointer getFather() const
    {
        return _father;
    }

    StrongNodePointer getLeftSoon() const
	{
		return _leftSoon;
	}

    StrongNodePointer getRightSoon() const
	{
		return _rightSoon;
	}

    StrongNodePointer getSibling() const {
		if (!this->getFather()) return StrongNodePointer();
		if (this->getFather()->getLeftSoon() != this) return this->getFather()->getLeftSoon();
		else return this->getFather()->getRightSoon();
	}

    const RegionModel& getModel() const
    {
        return _model;
    }

    RegionModel& getModel() {
		return _model;
	}

    void setFather(WeakNodePointer _father)
    {
        this->_father = _father;
    }

//    void changeSon(StrongNodePointer oldSon, StrongNodePointer newSon){
//    	if(this->_leftSoon == oldSon) this->_leftSoon = newSon;
//    	else if(this->_rightSoon == oldSon) this->_rightSoon = newSon;
//    }

    void setModel(const RegionModel& _model)
    {
    	BOOST_CONCEPT_ASSERT((boost::Assignable<RegionModel>));
        this->_model = _model;
    }

    const DissimilaritySet& getDissimilarities() const
    {
        return _dissimilarities;
    }

    DissimilaritySet& getDissimilarities() {
		return _dissimilarities;
	}

//    const DissimilaritySet& getDissimilarities() const {
//		return _dissimilarities;
//	}

    void setDissimilarities(DissimilaritySet& _dissimilarities)
    {
        this->_dissimilarities = _dissimilarities;
    }

    NeighborsVector getNeighbors() {
    	typedef typename DissimilaritySet::iterator	DissimilarityIterator;
    	NeighborsVector neighbors;
    	neighbors.reserve(_dissimilarities.size());
    	for(DissimilarityIterator it = _dissimilarities.begin(); it != _dissimilarities.end(); ++it){
    		StrongNodePointer neighbor = (*it)->getNeighbor(this);
    		neighbors.push_back(neighbor);
    	}
    	return neighbors;
    }

    bool isLeaf() const {
    	return !_rightSoon && !_leftSoon;
    }

    static void removeBPTNodes(StrongNodePointer p){
    	if(!p->isLeaf()){
    		removeBPTNodes(p->getLeftSoon());
    		removeBPTNodes(p->getRightSoon());
    	}
    	StoragePol::remove(p);
    }

};


// Initialize next_it to 0 (IDs from nodes will start from 0)
// Subsequent generated nodes will increase ID sequentially
template
<
	class 					TRegionModel,
	typename				DissimilaritySetTypeContainer,
	typename 				TId,
	template<class> class 	StoragePolicy
>
TId BPTNode<TRegionModel,DissimilaritySetTypeContainer,TId, StoragePolicy>::next_id = 0;

}

#endif	/* _BPTNODE_HPP */

