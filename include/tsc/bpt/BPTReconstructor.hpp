/*
 * BPTReconstructor.hpp
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

#ifndef BPTRECONSTRUCTOR_HPP_
#define BPTRECONSTRUCTOR_HPP_

#include "../policies/Storage.hpp"
#include "../log/Logger.hpp"
#include "../log/ProgressDisplay.hpp"
#include "models/ModelMerge.hpp"
#include <set>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

namespace tscbpt
{

using namespace std;

/**
 * Reconstruct the BPT structure from a given merging sequence.
 * Thus, there is no need to generate dissimilarities --> faster
 */
template <
	class NodeStoragePolicyType,
	class Logger 					= Logger<>,
	class CheckingPolicy			= FullCheckingPolicy,
	class NodeSetType				= set<typename NodeStoragePolicyType::pointerType>
>
class BPTReconstructor : public Logger, public CheckingPolicy
{

public:
	typedef NodeSetType											NodeSet;
	typedef NodeStoragePolicyType								NodeStoragePolicy;
	typedef CheckingPolicy										CheckingPol;
	typedef Logger												Log;
	typedef typename NodeStoragePolicy::pointerType				NodePointer;

	typedef typename NodeStoragePolicy::valueType::RegionModel	RegionModel;
	typedef typename NodeStoragePolicy::valueType::IDType		IDType;
	typedef typename NodeSet::iterator							NodeIterator;
	typedef typename NodeSet::const_iterator 					NodeConstIterator;


private:
	vector<NodePointer>	nodeId;
	NodeSet				aliveNodes;

public:

	BPTReconstructor(const NodeSet& leaves) {
		// Reserve space in nodeId
		nodeId.resize(2*leaves.size() - 1);
		for (NodeConstIterator it = leaves.begin(); it != leaves.end(); it++) {
			aliveNodes.insert(*it);
			nodeId.at((*it)->getId()) = *it;
		}
	}

	template<typename InputIterator1, typename InputIterator2>
	BPTReconstructor(InputIterator1 first, InputIterator2 last) {
		for (; first != last; ++first) {
			aliveNodes.insert(*first);
			nodeId.push_back(*first);
			this->errorAssert(nodeId.at((*first)->getId()) == *first);
		}
		nodeId.resize(2*nodeId.size() - 1);
	}


	template<template <class,class> class MergeOp >
	NodeSet& getBinaryPartitionForest(istream& mergingSeq, size_t numTrees) {
		typedef MergeOp<NodePointer, RegionModel> 		MergeOp;
		MergeOp	merge;

		ProgressDisplay show_progress( aliveNodes.size() - numTrees );

		while (aliveNodes.size() > numTrees ) {

			if(this->infoLogTest(aliveNodes.size() % 256 == 0)){
				this->infoLog(string("Subnodes: \t") + to_string(aliveNodes.size()));
			}

			// Read the ID of the merged nodes from the merging sequence
			IDType ida,idb;
			mergingSeq.read(reinterpret_cast<char*>(&ida), sizeof(ida));
			if (this->errorCheck(mergingSeq.fail() || mergingSeq.eof())) {
				this->errorLog("ERROR: Unexpected EOF!!!! (Maybe caused by a wrong merging sequence source)");
			}
			if (this->errorCheck(mergingSeq.bad())) {
				this->errorLog("ERROR: Unexpected IO error!!!!");
			}
			mergingSeq.read(reinterpret_cast<char*>(&idb), sizeof(idb));
			if (this->errorCheck(mergingSeq.fail() || mergingSeq.eof())) {
				this->errorLog("ERROR: Unexpected EOF!!!! (Maybe caused by a wrong merging sequence source)");
			}
			if (this->errorCheck(mergingSeq.bad())) {
				this->errorLog("ERROR: Unexpected IO error!!!!");
			}

			NodePointer nodea = nodeId.at(ida); // Get nodes from the merging sequence input
			NodePointer nodeb = nodeId.at(idb);

			if(this->errorCheck(nodea==NULL || nodeb==NULL)){
				this->errorLog("ERROR: Selected nodes for merging do not exist!!!! (Maybe caused by a wrong merging sequence source)");
				this->errorLog(string("\tida: ") + to_string(ida) + "\tidb" + to_string(idb));
			}
			if(this->errorCheck(nodea->getFather() || nodeb->getFather())){
				this->errorLog("ERROR: Selected nodes already merged!!!!");
				this->errorLog(string("nodea merged: \t") + to_string(nodea->getFather()));
				this->errorLog(string("nodeb merged: \t") + to_string(nodeb->getFather()));
			}

			aliveNodes.erase(nodea); // Remove nodes from alive nodes
			aliveNodes.erase(nodeb);

			// Generate father node by fusion of nodea and nodeb
			NodePointer father = NodeStoragePolicy::create(merge(nodea, nodeb), nodea, nodeb);
			nodea->setFather(father);
			nodeb->setFather(father);

			nodeId.at(father->getId()) = father;

			aliveNodes.insert(father);

			++show_progress;
		}

		this->infoLog(string("Construction process finished\n") + "AliveNodes: \t" + to_string(aliveNodes.size()));
		return aliveNodes;
	}

};

}

#endif /* BPTRECONSTRUCTOR_HPP_ */
