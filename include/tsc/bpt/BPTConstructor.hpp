/*
 * BPTConstructor.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 18/02/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef BPTCONSTRUCTOR_HPP_
#define BPTCONSTRUCTOR_HPP_

#include "../policies/Storage.hpp"
#include "../policies/CheckingPolicy.hpp"
#include "policies/BPTDataSavingPolicy.hpp"
#include "../log/Logger.hpp"
#include "../log/ProgressDisplay.hpp"
#include "models/ModelMerge.hpp"
#include "../util/ToString.hpp"
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <string>

namespace tscbpt
{

/**
 * Template to construct the BPT structure from the original
 * Weighted Region Adjacency Graph (WRAG)
 */
template <
	class NodeStoragePol,
	class DissimilarityStoragePol,
	class Logger 						= Logger<>,
	class CheckingPol 					= FullCheckingPolicy,
	class SavingPol						= BPTDataSavingPolicy,
	class DissimilaritySetType			= set<typename DissimilarityStoragePol::pointerType, pdiss_value_less<typename DissimilarityStoragePol::pointerType> >,
	class NodeSetType					= set<typename NodeStoragePol::pointerType>
>
class BPTConstructor : public CheckingPol, public Logger, public SavingPol
{
public:

	typedef Logger												Log;
	typedef DissimilarityStoragePol							 	DissimilarityStoragePolicy;
	typedef NodeStoragePol							 			NodeStoragePolicy;
	typedef CheckingPol											CheckingPolicy;
	typedef DissimilaritySetType				 				DissimilaritySet;
	typedef NodeSetType				 							NodeSet;
	typedef SavingPol											SavingPolicy;

	typedef typename NodeStoragePolicy::pointerType				NodePointer;
	typedef typename DissimilarityStoragePolicy::pointerType 	DissimilarityPointer;

	typedef typename NodeStoragePolicy::valueType::RegionModel	RegionModel;
	typedef typename NodeSet::iterator							NodeIterator;
	typedef typename NodeSet::const_iterator 					NodeConstIterator;
	typedef typename DissimilaritySet::iterator 				DissimilarityIterator;
	typedef typename DissimilaritySet::const_iterator 			DissimilarityConstIterator;


private:
	NodeSet aliveNodes;
	DissimilaritySet aliveDissimilarities;


public:
	BPTConstructor(const NodeSet& leaves) {
		for (NodeConstIterator it = leaves.begin(); it != leaves.end(); it++) {
			aliveNodes.insert(*it);
			const DissimilaritySet& nodeDiss = (*it)->getDissimilarities();
			aliveDissimilarities.insert(*(nodeDiss.begin()));
		}
	}

	template<typename InputIterator1, typename InputIterator2>
	BPTConstructor(InputIterator1 first, InputIterator2 last) {
		for (; first != last; ++first) {
			aliveNodes.insert(*first);
			const DissimilaritySet& nodeDiss = (*first)->getDissimilarities();
			aliveDissimilarities.insert(*(nodeDiss.begin()));
		}
	}

	template<class TDissimilarityMeasure, template <class,class> class MergeOp >
	NodeSet& getBinaryPartitionForest(size_t numTrees, TDissimilarityMeasure dissimilarityMeasure) {

		typedef TDissimilarityMeasure					DissimilarityMeasureType;
		typedef MergeOp<NodePointer, RegionModel> 		MergeOp;

		MergeOp	merge;

		SavingPolicy::prepareBPTConstruction(aliveNodes, aliveDissimilarities);
		SavingPolicy::startBPTConstruction();

		ProgressDisplay show_progress( aliveNodes.size() - numTrees );

		while (aliveNodes.size() > numTrees && aliveDissimilarities.size() > 0) {

			if(this->infoLogTest(aliveNodes.size() % 256 == 0)){
				this->infoLog(string("Subnodes: \t") + to_string(aliveNodes.size()) + string(" \tDissimilarities: \t") + to_string(aliveDissimilarities.size()));
			}

			DissimilarityPointer first = *(aliveDissimilarities.begin()); // Get first (smallest) alive dissimilarity
			aliveDissimilarities.erase(aliveDissimilarities.begin()); //   and remove it from alive dissimilarities

			NodePointer nodea = first->getA(); // Get nodes from the dissimilarity
			NodePointer nodeb = first->getB();
			if(this->errorCheck(nodea == NULL || nodeb == NULL)){
				this->errorLog("ERROR: Dissimilarity contains NULL nodes!!!!");
			}

			SavingPolicy::saveSelectedDissimilarity(first);

			if(this->errorCheck(nodea->getFather() || nodeb->getFather())){
				this->errorLog("ERROR: Selected nodes already merged!!!!");
				this->errorLog(string("nodea father: \t") + to_string(nodea->getFather()));
				this->errorLog(string("nodeb father: \t") + to_string(nodeb->getFather()));
			}

			size_t removed = aliveNodes.erase(nodea); 		// Remove nodes from alive nodes
			if(this->errorCheck(removed == 0)) this->errorLog("ERROR: nodea is not in aliveNodes!!!!");
			removed = aliveNodes.erase(nodeb);			// Remove nodes from alive nodes
			if(this->errorCheck(removed == 0)) this->errorLog("ERROR: nodeb is not in aliveNodes!!!!");

			// Generate father node by fusion of nodea and nodeb
			NodePointer father = NodeStoragePolicy::create(merge(nodea, nodeb), nodea, nodeb);
			nodea->setFather(father);
			nodeb->setFather(father);

			NodeSet fatherNeigbors; // Collect father neighborhood
			typedef std::map<NodePointer, DissimilarityPointer> FirstDissMap;
			typedef typename FirstDissMap::iterator FirstDissIterator;
			FirstDissMap oldNeighbors; // Save old first dissimilarities for neighbors

			for (DissimilarityConstIterator dit = nodea->getDissimilarities().begin(); dit
				!= nodea->getDissimilarities().end(); ++dit) { // Collect from nodea
				DissimilarityPointer diss = *dit;
				NodePointer neighbor = diss->getNeighbor(nodea);
				if (neighbor != nodeb) { // Do not process nodeb (it's also a nodea neighbor)
					DissimilarityPointer oldFirstDiss = *(neighbor->getDissimilarities().begin());
					removed = neighbor->getDissimilarities().erase(diss); // Remove nodea in neighbor's neighborhood
					if(this->errorCheck(removed == 0)) this->errorLog("ERROR: nodea is not in its neighbor's neighborhood!!!!");
					fatherNeigbors.insert(neighbor);

					// Create and insert dissimilarity into father's dissimilarities
					DissimilarityPointer fdiss = DissimilarityStoragePolicy::create(father, neighbor,
						dissimilarityMeasure(father, neighbor));
					father->getDissimilarities().insert(fdiss);
					if (DissimilarityMeasureType::isSymmetric == true) {
						neighbor->getDissimilarities().insert(fdiss);
					} else {
						DissimilarityPointer dissRev = DissimilarityStoragePolicy::create(neighbor, father,
							dissimilarityMeasure(neighbor, father));
						neighbor->getDissimilarities().insert(dissRev);
					}

					// Update aliveDissimilarities when necessary
					DissimilarityPointer newFirstDiss = *(neighbor->getDissimilarities().begin());
					if(newFirstDiss != oldFirstDiss){
						oldNeighbors.insert(make_pair(neighbor, oldFirstDiss));
					}

//					DissimilarityStoragePolicy::remove(diss);
				}
			}
			for (DissimilarityConstIterator dit = nodeb->getDissimilarities().begin(); dit
				!= nodeb->getDissimilarities().end(); ++dit) { // Collect from nodeb
				DissimilarityPointer diss = *dit;
				NodePointer neighbor = diss->getNeighbor(nodeb);
				if (neighbor != nodea) { // Do not process nodea (it's also a nodeb neighbor)
					DissimilarityPointer oldFirstDiss = *(neighbor->getDissimilarities().begin());
					removed = neighbor->getDissimilarities().erase(diss); // Remove nodeb in neighbor's neighborhood
					if(this->errorCheck(removed == 0)) this->errorLog("ERROR: nodeb is not in its neighbor's neighborhood!!!!");
					if (fatherNeigbors.insert(neighbor).second) { // i.e. If it is a new father neighbor

						// Create and insert dissimilarity into father's dissimilarities
						DissimilarityPointer diss = DissimilarityStoragePolicy::create(father, neighbor,
							dissimilarityMeasure(father, neighbor));
						father->getDissimilarities().insert(diss);
						if (DissimilarityMeasureType::isSymmetric == true) {
							neighbor->getDissimilarities().insert(diss);
						} else {
							DissimilarityPointer dissRev = DissimilarityStoragePolicy::create(neighbor, father,
								dissimilarityMeasure(neighbor, father));
							neighbor->getDissimilarities().insert(dissRev);
						}
					}
					// Update aliveDissimilarities when necessary
					DissimilarityPointer newFirstDiss = *(neighbor->getDissimilarities().begin());
					if (newFirstDiss != oldFirstDiss) {
						oldNeighbors.insert(make_pair(neighbor, oldFirstDiss));
					}

//					DissimilarityStoragePolicy::remove(diss);
				}
			}

			if (this->errorCheck(fatherNeigbors.find(nodea)!=fatherNeigbors.end())) {
				this->errorLog("ERROR: nodea as father neighbor!!!!");
			}
			if (this->errorCheck(fatherNeigbors.find(nodeb) != fatherNeigbors.end())) {
				this->errorLog("ERROR: nodeb as father neighbor!!!!");
			}

			SavingPolicy::saveFatherNode(father);

			// Add father node to aliveNodes
			aliveNodes.insert(father);

			// Update aliveDissimilarities with father's first dissimilarity
			DissimilarityPointer fathersFirst = *(father->getDissimilarities().begin());
			if(father->getDissimilarities().size() > 0)
				aliveDissimilarities.insert(fathersFirst);

			// Update aliveDissimilarities from father neighbors when needed
			for(FirstDissIterator it = oldNeighbors.begin(); it != oldNeighbors.end(); ++it){
				NodePointer node = (*it).first;
				DissimilarityPointer oldDiss = (*it).second;
				DissimilarityPointer newDiss = *(node->getDissimilarities().begin());
				aliveDissimilarities.erase(oldDiss);
				if (DissimilarityMeasureType::isSymmetric) { // Since the first one has been added, any diss involving father can be skipped
					if(newDiss->getA() != father) aliveDissimilarities.insert(newDiss);
				} else {
					aliveDissimilarities.insert(newDiss);
				}
			}

			// TODO: Insert some kind of policy to manage dissimilarity preservation
			// Remove dissimilarities from nodea
			for (DissimilarityConstIterator dit = nodea->getDissimilarities().begin(); dit
				!= nodea->getDissimilarities().end(); ++dit) {
				DissimilarityPointer diss = *dit;
				NodePointer neighbor = diss->getNeighbor(nodea);
				if (neighbor != nodeb) { // Do not remove nodeb dissimilarity (it's also a nodea neighbor)
					DissimilarityStoragePolicy::remove(diss);
				}
			}
			nodea->getDissimilarities().clear();
			// Remove dissimilarities from nodeb
			for (DissimilarityConstIterator dit = nodeb->getDissimilarities().begin(); dit
				!= nodeb->getDissimilarities().end(); ++dit) {
				DissimilarityPointer diss = *dit;
				DissimilarityStoragePolicy::remove(diss);
			}
			nodeb->getDissimilarities().clear();

			++show_progress;
		}

		SavingPolicy::endBPTConstruction();

		this->infoLog(
			string("Construction process finished\n") + "AliveNodes: \t" + to_string(aliveNodes.size())
				+ "AliveDissimilarities: \t" + to_string(aliveDissimilarities.size()));
		return aliveNodes;
	}

};

}

#endif /* BPTCONSTRUCTOR_HPP_ */
