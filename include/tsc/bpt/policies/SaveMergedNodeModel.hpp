/*
 * SaveMergedNodeModel.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 09/01/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SAVEMERGEDNODEMODEL_HPP_
#define SAVEMERGEDNODEMODEL_HPP_

#include "BPTDataSavingPolicy.hpp"
#include <tsc/policies/CheckingPolicy.hpp>
#include <tsc/bpt/models/SubMatrixSize.hpp>
#include <tsc/bpt/models/TotalMatrixSize.hpp>
#include <iostream>
#include <fstream>
#include <string>


namespace tscbpt {

using namespace std;

template<
	typename	RegionModel,
	class 		DataSaver,
	class 		CheckingPolicy				= FullCheckingPolicy
>
class SaveMergedNodeModel : public BPTDataSavingPolicy{
public:

	typedef RegionModel			region_model;
	typedef CheckingPolicy		CheckingPol;
	typedef CheckingPolicy		checkig_policy;
	typedef DataSaver			data_saver_type;
	typedef SaveMergedNodeModel<RegionModel, DataSaver, CheckingPolicy>	this_type;

	typedef data_saver_type		data_saver;

	typedef SubMatrixSize<region_model> 	sub_matrix_size;
	typedef TotalMatrixSize<region_model> 	total_matrix_size;

	static const char* const DEFAULT_OUTPUT_SUBFOLDER;

	SaveMergedNodeModel(string aBasedir = DEFAULT_OUTPUT_SUBFOLDER, string aPrefix = string("C")) : saver(NULL), basedir(aBasedir), prefix(aPrefix) {}

	template <class NodeSet, class DissimilaritySet>
	void prepareBPTConstruction(NodeSet nodeSet, DissimilaritySet){
		size_t matrix_size = total_matrix_size::getValue((*(nodeSet.begin()))->getModel());
		// TODO: More general interface
		saver = new data_saver(matrix_size, sub_matrix_size::getValue((*(nodeSet.begin()))->getModel()), basedir, prefix);
	}

	void startBPTConstruction() {}

	template<class Node>
	void saveFatherNode(Node node){
		saver->save_elem(node);
	}

	void endBPTConstruction(){
		saver->finalize();
		delete(saver);
	}


private:
	data_saver* saver;
	string basedir, prefix;
	static const CheckingPol			Check;
};

template <typename RegionModel, class DataSaver, class CheckingPolicy>
const char* const SaveMergedNodeModel<RegionModel, DataSaver, CheckingPolicy>::DEFAULT_OUTPUT_SUBFOLDER 		= "BPT/";

template <typename RegionModel, class DataSaver, class CheckingPolicy>
const typename SaveMergedNodeModel<RegionModel, DataSaver, CheckingPolicy>::CheckingPol SaveMergedNodeModel<RegionModel, DataSaver, CheckingPolicy>::Check;

} /* namespace tscbpt */
#endif /* SAVEMERGEDNODEMODEL_HPP_ */
