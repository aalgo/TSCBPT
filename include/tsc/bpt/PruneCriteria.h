/*
 * PruneCriteria.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 21/10/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef PRUNECRITERIA_H_
#define PRUNECRITERIA_H_


#include <cmath>
#include <boost/concept_check.hpp>
#include <tsc/bpt/models/concepts/HasLogDetAverage.h>

namespace tscbpt{

template <typename HomogType = double>
struct RelErrorHomogeneityPruneCriterion{
	typedef HomogType		ParameterValueType;

	RelErrorHomogeneityPruneCriterion(HomogType pruneFactordB) : pruneFactor(std::pow(10.0, pruneFactordB/10.0)) {}
	template<typename NodePointer>
	bool operator()(NodePointer np){
		static const HomogType MIN_NORM_THRESHOLD = 1e-12;
		return np->getModel().getTotalSumOfSquares() / max(norm2(np->getModel()), MIN_NORM_THRESHOLD) / np->getModel().getSubnodes() < pruneFactor;
	}
private:
	HomogType pruneFactor;
};

template <typename HomogType = double>
struct DirectTSSPruneCriterion{
	typedef HomogType		ParameterValueType;

	DirectTSSPruneCriterion(HomogType pruneFactordB) : pruneFactor(std::pow(10.0, pruneFactordB/10.0)) {}
	template<typename NodePointer>
	bool operator()(NodePointer np){
		return np->getModel().getTotalSumOfSquares() < pruneFactor;
	}
private:
	HomogType pruneFactor;
};

template <typename HomogType = double>
struct LogDetPruneCriterion{
	typedef HomogType		ParameterValueType;
	typedef HomogType		homogeneity_value;

	LogDetPruneCriterion(HomogType pruneFactordB) : pruneFactor(std::pow(10.0, pruneFactordB/10.0)) {}
	template<typename NodePointer>
	bool operator()(NodePointer np){
		BOOST_CONCEPT_ASSERT( (concept::HasLogDetAverage< typeof(np->getModel()) > ) );
		return np->getModel().getLogDetAverage() < pruneFactor;
	}
private:
	HomogType pruneFactor;
};

template <typename HomogType = double>
struct RelativeMSEHomogeneityOperator{
	typedef HomogType		ParameterValueType;

	template<typename RegionModel>
	HomogType operator()(RegionModel model){
		static const HomogType MIN_NORM_THRESHOLD = 1e-12;
		return model.getTotalSumOfSquares() / max(norm2(model), MIN_NORM_THRESHOLD) / model.getSubnodes();
	}
};

template <typename HomogType = double>
struct RelErrorModelHomogeneityPruneCriterion{
	typedef HomogType		ParameterValueType;

	RelErrorModelHomogeneityPruneCriterion(HomogType pruneFactordB) : pruneFactor(std::pow(10.0, pruneFactordB/10.0)) {}
	template<typename RegionModel>
	bool operator()(const RegionModel& model){
		static const HomogType MIN_NORM_THRESHOLD = 1e-12;
		return model.getTotalSumOfSquares() / max(norm2(model), MIN_NORM_THRESHOLD) / model.getSubnodes() < pruneFactor;
	}
private:
	HomogType pruneFactor;
};

template <typename HomogType = double>
struct BetterMSEPruneCriterion{
	typedef HomogType		ParameterValueType;
	static const HomogType MIN_NORM_THRESHOLD = static_cast<HomogType>(1e-12);

	template<typename RegionModel>
	BetterMSEPruneCriterion(const RegionModel& refModel) :
		pruneFactor(refModel.getTotalSumOfSquares() / max(norm2(refModel), MIN_NORM_THRESHOLD) /
			refModel.getSubnodes()) {}

	template<typename RegionModel>
	bool operator()(const RegionModel& model){
		return model.getTotalSumOfSquares() / max(norm2(model), MIN_NORM_THRESHOLD) / model.getSubnodes() <= pruneFactor;
	}
private:
	HomogType pruneFactor;
};
template <typename HomogType>
const HomogType BetterMSEPruneCriterion<HomogType>::MIN_NORM_THRESHOLD;


}

#endif /* PRUNECRITERIA_H_ */
