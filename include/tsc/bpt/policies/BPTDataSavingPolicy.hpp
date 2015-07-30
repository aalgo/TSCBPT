/*
 * BPTDataSavingPolicy.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 13/09/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef BPTDATASAVINGPOLICY_HPP_
#define BPTDATASAVINGPOLICY_HPP_

namespace tscbpt
{

class BPTDataSavingPolicy
{
public:

	template <class NodeSet, class DissimilaritySet>
	void prepareBPTConstruction(NodeSet, DissimilaritySet){}

	void startBPTConstruction(){}

	template<class Dissimilarity>
	void saveSelectedDissimilarity(Dissimilarity){}

	template<class Node>
	void saveFatherNode(Node){}

	void endBPTConstruction(){}

protected:
	~BPTDataSavingPolicy(){}
};

}

#endif /* BPTDATASAVINGPOLICY_HPP_ */
