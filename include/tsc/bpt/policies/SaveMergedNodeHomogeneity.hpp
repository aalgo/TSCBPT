/*
 * SaveMergedNodeHomogeneity.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 14/09/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SAVEMERGEDNODEHOMOGENEITY_HPP_
#define SAVEMERGEDNODEHOMOGENEITY_HPP_

#include "BPTDataSavingPolicy.hpp"
#include <tsc/policies/CheckingPolicy.hpp>
#include <tsc/bpt/PruneCriteria.h>
#include <iostream>
#include <fstream>

namespace tscbpt
{

using namespace std;

template<
	typename	HomogType					= double,
	class 		CheckingPolicy				= FullCheckingPolicy
>
class SaveMergedNodeHomogeneity
{
public:
	typedef HomogType			homogType;
	typedef CheckingPolicy		CheckingPol;

	static const char* const HomogFileName;

	template <class NodeSet, class DissimilaritySet>
	void prepareBPTConstruction(NodeSet, DissimilaritySet) {}

	void startBPTConstruction() {
		homogOut.open(HomogFileName, ios::out | ios::trunc);
		Check.errorAssert(!(homogOut.fail()));
	}

	template <class Node>
	void saveFatherNode(Node n) {
		homogType h;
		homogOut.write(reinterpret_cast<char*> (&(h = homogeneity(n->getModel()))), sizeof(h));
	}

	template<class Dissimilarity>
	void saveSelectedDissimilarity(Dissimilarity) {}

	void endBPTConstruction() {
		homogOut.close();
	}

private:
	ofstream homogOut;
	static const CheckingPol			Check;
	typedef RelativeMSEHomogeneityOperator<homogType>			HomogOp;
	HomogOp homogeneity;
};

template <typename HomogType, class CheckingPolicy>
const char* const SaveMergedNodeHomogeneity<HomogType, CheckingPolicy>::HomogFileName 		= "BPT/homog.bin";

template <typename HomogType, class CheckingPolicy>
const typename SaveMergedNodeHomogeneity<HomogType, CheckingPolicy>::CheckingPol SaveMergedNodeHomogeneity<HomogType, CheckingPolicy>::Check;

}

#endif /* SAVEMERGEDNODEHOMOGENEITY_HPP_ */
