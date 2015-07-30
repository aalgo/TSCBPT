/*
 * TemporalStability.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 17/07/2015
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef TEMPORALSTABILITY_HPP_
#define TEMPORALSTABILITY_HPP_

#include <armadillo>
#include <functional>


namespace tscbpt
{

template <class NodePointer, class ValueType = double>
struct GeodesicVectorChangeMeasureFull: public std::unary_function<NodePointer, ValueType>
{
	ValueType operator()(const NodePointer a) const {
		ValueType res = ValueType();
		const size_t covariances = a->getModel().getNumCovariances();
		const size_t comparations = covariances - 1;
		arma::cx_mat vma[covariances];
		for(size_t i = 0; i < covariances; ++i){
			vma[i] = to_arma<arma::cx_mat>::from(a->getModel().getCovariance(i));
		}

		#pragma omp parallel for num_threads(min(static_cast<int>(comparations), omp_get_max_threads())) schedule(dynamic, 1) shared(vma) reduction(+:res) default(none)
		for(size_t i = 0; i < comparations; ++i){
			for(size_t j = i+1; j < covariances; ++j){
			arma::cx_vec eigval;
			arma::cx_mat eigvec;

			arma::eig_gen(eigval, eigvec, solve(vma[i], vma[j]));

			res += sqrt(accu(pow(log(abs(eigval)),2)));
			}
		}
		return res / (comparations*covariances/2.0);
	}
};

template <class NodePointer, class ValueType = double>
struct DiagonalGeodesicVectorChangeMeasureFull: public std::unary_function<NodePointer, ValueType>
{
	ValueType operator()(NodePointer a) const {
		static const ValueType DG_MIN_THRESHOLD = 1e-12;

		ValueType res = ValueType();
		for(size_t i = 0; i < a->getModel().getNumCovariances() - 1; ++i){
			for(size_t j = i+1; j < a->getModel().getNumCovariances(); ++j){
				double tmp = 0;
				for(size_t k = 0; k < a->getModel().subMatrix_size; ++k){
					tmp += pow(log(max(a->getModel().getCovariance(i)(k, k).real(), DG_MIN_THRESHOLD) / max(a->getModel().getCovariance(j)(k, k).real(), DG_MIN_THRESHOLD)), 2);
				}
				res += sqrt(tmp);
			}
		}

		return res / ((a->getModel().getNumCovariances() - 1) * (a->getModel().getNumCovariances()) / 2);
	}
};

template <class NodePointer, class SimilarityMeasure, class ValueType = double>
struct SimilarityToRegion: public std::unary_function<NodePointer, ValueType>
{
	SimilarityToRegion(NodePointer pRefNode, SimilarityMeasure pSim = SimilarityMeasure())
	: refNode(pRefNode), measure(pSim){	}

	ValueType operator()(NodePointer a) const {
		return measure(refNode, a);
	}
private:
	NodePointer refNode;
	SimilarityMeasure measure;
};

template <class NodePointer, class ValueType = double>
struct GeodesicVectorPairChangeMeasure: public std::unary_function<NodePointer, ValueType>
{
	GeodesicVectorPairChangeMeasure(size_t a, size_t b): _a(a), _b(b){}

	ValueType operator()(const NodePointer a) const {
		arma::cx_vec eigval;
		arma::cx_mat eigvec;
		arma::eig_gen(eigval, eigvec, solve(
			to_arma<arma::cx_mat>::from(a->getModel().getCovariance(_a)),
			to_arma<arma::cx_mat>::from(a->getModel().getCovariance(_b))));
		return ValueType(sqrt(accu(pow(log(abs(eigval)),2))));
	}
private:
	size_t _a, _b;
};


} /* namespace tscbpt */

#endif /* TEMPORALSTABILITY_HPP_ */
