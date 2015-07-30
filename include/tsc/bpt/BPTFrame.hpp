/*
 * BPTFrame.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 14/03/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef BPTFRAME_HPP_
#define BPTFRAME_HPP_

#include "../policies/policies.h"
#include "BPTDissimilarity.hpp"
#include "BPTNode.hpp"
#include "policies/PDissimilarityComparators.hpp"
#include "BPTConstructor.hpp"
#include "BPTReconstructor.hpp"
#include "../log/Logger.hpp"
#include "PNodeComparators.h"
#include "policies/BPTDataSavingPolicy.hpp"
#include "policies/AddSavingPolicy.hpp"
#include "policies/SaveMergingSequence.hpp"
#include "policies/SaveMergedNodeModel.hpp"
#include "policies/SaveMergedNodeHomogeneity.hpp"
#include <tsc/io/data_saver/PolSARProMatrixDataSaver.hpp>
#include <set>
#include <deque>
#include <stdint.h>

namespace tscbpt
{

using std::set;

/**
 * Class to encapsulate the most common template arguments
 * and dependencies among the different elements of the BPT
 * structure, for convenience.
 * All the needed template options may be defined in this class
 * and all the BPT classes may be found within it as typedefs.
 *
 * You can still define different arguments for each particular class
 * apart of the BPTFrame, in case of different needs.
 */
template<
	typename				TRegionModel,
	typename 				TDissimilarityValue 		= double,
	typename 				IDType						= uint32_t,
	template<class> class 	PtrStoragePolicy 			= DefaultNativeStoragePolicy,
	class 					CheckingPolicy				= FullCheckingPolicy,
	class 					Logger						= Logger<>
>
class BPTFrame:
	public PtrStoragePolicy<BPTFrame<TRegionModel, TDissimilarityValue, IDType, PtrStoragePolicy, CheckingPolicy> >
{
	public:
		typedef TRegionModel 								RegionModel;
		typedef TDissimilarityValue 						DissimilarityValue;
		typedef BPTFrame<TRegionModel, TDissimilarityValue, IDType, PtrStoragePolicy, CheckingPolicy, Logger> ThisType;
		typedef CheckingPolicy								CheckingPol;
		typedef IDType										NodeID;

	private:

		struct DissimilaritySetTypeContainer;



	public:
		typedef BPTNode<RegionModel, DissimilaritySetTypeContainer, NodeID, PtrStoragePolicy>			Node;
		typedef PtrStoragePolicy<Node>												NodeStoragePolicy;
		typedef typename NodeStoragePolicy::pointerType								NodePointer;
		typedef typename NodeStoragePolicy::strongPointerType						StrongNodePointer;
		typedef typename NodeStoragePolicy::weakPointerType							WeakNodePointer;
		typedef	BPTDissimilarity<NodePointer, DissimilarityValue>			 		Dissimilarity;
		typedef PtrStoragePolicy<Dissimilarity>										DissimilarityStoragePolicy;
		typedef typename DissimilarityStoragePolicy::pointerType					DissimilarityPointer;
		typedef set<NodePointer>													NodeSet;
		typedef set<DissimilarityPointer, pdiss_value_less<DissimilarityPointer> >	DissimilaritySet;

		typedef SaveMergingSequence<IDType,CheckingPolicy>							SaveMSPol;
		typedef SaveMergedNodeModel<RegionModel, PolSARProMatrixDataSaver<>, CheckingPol>			SaveMNPol;
		typedef SaveMergedNodeHomogeneity<>															SaveHomogPol;

#if defined(SAVE_BPT_MODELS_AND_HOMOGENEITY)
		typedef BPT_SAVING_POLICY_3(SaveMSPol, SaveMNPol, SaveHomogPol)				DefaultDataSavingPolicy;
#elif defined(SAVE_BPT_MODELS)
		typedef BPT_SAVING_POLICY_2(SaveMSPol, SaveMNPol)							DefaultDataSavingPolicy;
#else
		typedef BPT_SAVING_POLICY_1(SaveMSPol)										DefaultDataSavingPolicy;
#endif

		typedef BPTConstructor<NodeStoragePolicy,DissimilarityStoragePolicy,
			Logger,	CheckingPol, DefaultDataSavingPolicy, DissimilaritySet,
			NodeSet>																Constructor;

		typedef BPTReconstructor<NodeStoragePolicy, Logger, CheckingPol, NodeSet>	Reconstructor;


	private:

		struct DissimilaritySetTypeContainer{
			typedef set<DissimilarityPointer, pdiss_value_less<DissimilarityPointer> >	DissimilaritySet;
		};

	public:


	template <class PruneCriteria>
	static NodeSet& prune(NodePointer root, NodeSet& out, PruneCriteria criterion) {
		std::deque<NodePointer> remaining;
		remaining.push_back(root);
		while (remaining.size() > 0) {
			NodePointer np = remaining.front();
			remaining.pop_front();
			if (criterion(np) || np->isLeaf()) {
				out.insert(np);
			} else {
				remaining.push_back(np->getLeftSoon());
				remaining.push_back(np->getRightSoon());
			}
		}
		return out;
	}

	static NodeSet& NRegs_prune(NodePointer root, NodeSet& out, size_t regs) {
		assert(regs > 0);
		std::set<NodePointer, pnode_later<NodePointer> > remaining;
		remaining.insert(root);
		while (remaining.size() && remaining.size() < regs) {
			NodePointer np = *(remaining.begin());
			remaining.erase(remaining.begin());
			if (np->isLeaf()) {
				out.insert(np);
			}else{
				remaining.insert(np->getLeftSoon());
				remaining.insert(np->getRightSoon());
			}
		}
		out.insert(remaining.begin(), remaining.end());
		return out;
	}

};

}

#endif /* BPTFRAME_HPP_ */
