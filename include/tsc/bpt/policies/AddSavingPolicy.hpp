/*
 * AddSavingPolicy.hpp
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

#ifndef ADDSAVINGPOLICY_HPP_
#define ADDSAVINGPOLICY_HPP_

#include "BPTDataSavingPolicy.hpp"

namespace tscbpt
{

template<
	class PolicyA,
	class PolicyB
>
class AddSavingPolicy: public PolicyA, public PolicyB
{
public:

	template <class NodeSet, class DissimilaritySet>
	void prepareBPTConstruction(NodeSet n, DissimilaritySet d){
		PolicyA::prepareBPTConstruction(n, d);
		PolicyB::prepareBPTConstruction(n, d);
	}

	void startBPTConstruction(){
		PolicyA::startBPTConstruction();
		PolicyB::startBPTConstruction();
	}

	template<class Dissimilarity>
	void saveSelectedDissimilarity(Dissimilarity d){
		PolicyA::saveSelectedDissimilarity(d);
		PolicyB::saveSelectedDissimilarity(d);
	}

	template<class Node>
	void saveFatherNode(Node n){
		PolicyA::saveFatherNode(n);
		PolicyB::saveFatherNode(n);
	}

	void endBPTConstruction(){
		PolicyA::endBPTConstruction();
		PolicyB::endBPTConstruction();
	}

protected:
	~AddSavingPolicy(){}
};


#define BPT_SAVING_POLICY_1(P1)		P1
#define BPT_SAVING_POLICY_2(P1, P2)		AddSavingPolicy<P2, BPT_SAVING_POLICY_1(P1) >
#define BPT_SAVING_POLICY_3(P1, P2, P3)		AddSavingPolicy<P3, BPT_SAVING_POLICY_2(P1, P2) >
#define BPT_SAVING_POLICY_4(P1, P2, P3, P4)		AddSavingPolicy<P4, BPT_SAVING_POLICY_3(P1, P2, P3) >
#define BPT_SAVING_POLICY_5(P1, P2, P3, P4, P5)	\
	AddSavingPolicy<P5, BPT_SAVING_POLICY_4(P1, P2, P3, P4) >
#define BPT_SAVING_POLICY_6(P1, P2, P3, P4, P5, P6)	\
	AddSavingPolicy<P6, BPT_SAVING_POLICY_5(P1, P2, P3, P4, P5) >
#define BPT_SAVING_POLICY_7(P1, P2, P3, P4, P5, P6, P7)	\
	AddSavingPolicy<P7, BPT_SAVING_POLICY_6(P1, P2, P3, P4, P5, P6) >
#define BPT_SAVING_POLICY_8(P1, P2, P3, P4, P5, P6, P7, P8)	\
	AddSavingPolicy<P8, BPT_SAVING_POLICY_7(P1, P2, P3, P4, P5, P6, P7) >
#define BPT_SAVING_POLICY_9(P1, P2, P3, P4, P5, P6, P7, P8, P9)	\
	AddSavingPolicy<P9, BPT_SAVING_POLICY_8(P1, P2, P3, P4, P5, P6, P7, P8) >
#define BPT_SAVING_POLICY_10(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)	\
	AddSavingPolicy<P10, BPT_SAVING_POLICY_9(P1, P2, P3, P4, P5, P6, P7, P8, P9) >

}

#endif /* ADDSAVINGPOLICY_HPP_ */
