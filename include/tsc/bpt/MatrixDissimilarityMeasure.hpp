/*
 * MatrixDissimilarityMeasure.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 23/07/2015
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef INCLUDE_TSC_BPT_MATRIXDISSIMILARITYMEASURE_HPP_
#define INCLUDE_TSC_BPT_MATRIXDISSIMILARITYMEASURE_HPP_

#include "DissimilarityMeasure.hpp"

namespace tscbpt
{

template<class NodePointer, class ValueType>
struct RevisedWishartDissimilarityMeasure:
	public SymmetricDissimilarityMeasure,
	public std::binary_function<NodePointer, NodePointer, ValueType>{

	ValueType operator()(NodePointer a, NodePointer b) const {
		arma::cx_mat ma = to_arma<arma::cx_mat>::from(a->getModel());
		arma::cx_mat mb = to_arma<arma::cx_mat>::from(b->getModel());
		// Same as below but more efficient with solve()
//		return (real(trace(inv(ma) * mb)) +
//			real(trace(inv(mb) * ma))) *
		return (real(trace(solve(ma,mb))) +
			real(trace(solve(mb,ma)))) *
			(a->getModel().getSubnodes() + b->getModel().getSubnodes());
	}
};


template <class NodePointer, class ValueType>
struct DiagonalRevisedWishartDissimilarityMeasure:
	public SymmetricDissimilarityMeasure,
	public std::binary_function<NodePointer, NodePointer, ValueType>
{
	inline ValueType operator()(NodePointer a, NodePointer b) const {
		static const ValueType DRW_MIN_THRESHOLD = 1e-12;
		ValueType res = ValueType();
		for(size_t i = 0; i < a->getModel().getMatrixSize(); ++i){
			ValueType tmp = max(a->getModel()(i, i).real(), DRW_MIN_THRESHOLD) / max(b->getModel()(i, i).real(), DRW_MIN_THRESHOLD);
			res += tmp + 1.0 / tmp;
		}
		return res * (a->getModel().getSubnodes() + b->getModel().getSubnodes());
	}
};

template<class NodePointer, class ValueType>
struct DiagonalGeodesicDissimilarityMeasure:
	public SymmetricDissimilarityMeasure,
	public std::binary_function<NodePointer, NodePointer, ValueType>{

	ValueType operator()(NodePointer a, NodePointer b) const {
		static const ValueType DG_MIN_THRESHOLD = 1e-12;

		ValueType res = ValueType();
		for(size_t i = 0; i < a->getModel().getMatrixSize(); ++i){
			res += pow(log(max(a->getModel()(i, i).real(), DG_MIN_THRESHOLD) / max(b->getModel()(i, i).real(), DG_MIN_THRESHOLD)), 2);
		}
		return sqrt(res) + log(2*a->getModel().getSubnodes()*b->getModel().getSubnodes() /
			(a->getModel().getSubnodes() + b->getModel().getSubnodes()));
	}
};

template <class NodePointer, class ValueType>
struct GeodesicDissimilarityMeasure:
	public SymmetricDissimilarityMeasure,
	public std::binary_function<NodePointer, NodePointer, ValueType>
{

	ValueType operator()(NodePointer a, NodePointer b) const {
		arma::cx_mat ma = to_arma<arma::cx_mat>::from(a->getModel());
		arma::cx_mat mb = to_arma<arma::cx_mat>::from(b->getModel());

		arma::cx_vec eigval;
		arma::cx_mat eigvec;

		arma::eig_gen(eigval, eigvec, solve(ma,mb));

		return norm(log(eigval),2) + log(2*a->getModel().getSubnodes()*b->getModel().getSubnodes() /
			(a->getModel().getSubnodes() + b->getModel().getSubnodes()));
	}
};

template <class NodePointer, class ValueType>
struct HomogDissimilarityMeasure: public SymmetricDissimilarityMeasure, public std::binary_function<
	NodePointer, NodePointer, ValueType>
{

	ValueType operator()(NodePointer a, NodePointer b) const {
		arma::cx_mat ma = to_arma<arma::cx_mat>::from(a->getModel());
		arma::cx_mat mb = to_arma<arma::cx_mat>::from(b->getModel());
		arma::cx_mat mf = (ma*a->getModel().getSubnodes() + mb*b->getModel().getSubnodes()) / (a->getModel().getSubnodes() + b->getModel().getSubnodes());

		double ha = a->getModel().getTotalSumOfSquares();
		double hb = b->getModel().getTotalSumOfSquares();

		return 1.0 / ((a->getModel().getSubnodes() + b->getModel().getSubnodes()) * pow(norm(mf, "fro"), 2)) *
			(ha + hb + pow(norm(ma-mb, "fro"), 2) * a->getModel().getSubnodes() * b->getModel().getSubnodes() / (a->getModel().getSubnodes() + b->getModel().getSubnodes()));
	}
};

template <class NodePointer, class ValueType>
struct GeodesicVectorMatrixDissimilarityMeasure: public SymmetricDissimilarityMeasure, public std::binary_function<
	NodePointer, NodePointer, ValueType>
{
	template <typename Matrix>
	arma::cx_mat generateLogMatrix(const Matrix& ma, const Matrix& mb) const {
		arma::vec eigval;
		arma::cx_mat eigvec;
		arma::eig_sym(eigval, eigvec, ma);
		const arma::cx_mat mai = eigvec * arma::diagmat(pow(eigval, -0.5)) * (eigvec.t()); 	// Compute A^(-0.5)
		arma::eig_sym(eigval, eigvec, mai * mb * mai);		// eigs { A^(-0.5) * B * A^(-0.5) }
		return eigvec * arma::diagmat(log(eigval)) * (eigvec.t());	// logm { A^(-0.5) * B * A^(-0.5) }
	}
	template<typename T>
	ValueType measure(T a, T b) const {
		ValueType res = ValueType();
		const size_t covariances = a.getNumCovariances();

		#pragma omp parallel for num_threads(min(static_cast<int>(covariances), omp_get_max_threads())) schedule (dynamic, 1) shared(a,b) reduction (+:res) default(none)
		for(size_t i = 0; i < covariances; ++i){
			arma::cx_mat ma = to_arma<arma::cx_mat>::from(a.getCovariance(i));
			arma::cx_mat mb = to_arma<arma::cx_mat>::from(b.getCovariance(i));

			// Both computations seem equivalent
			// Option A: simpler but need for eig_gen() which is slow
//			arma::cx_vec eigval;
//			arma::cx_mat eigvec;
//			arma::eig_gen(eigval, eigvec, solve(ma, mb));
//			res += accu(pow(log(abs(eigval)),2));

			// Option B: More complex: 2 calls to eig_sym() (faster than eig_gen)
			// and several matrix multiplications
			res += pow(arma::norm(generateLogMatrix(ma,mb),"fro"), 2);
		}
		return sqrt(res);
	}

	ValueType operator()(NodePointer a, NodePointer b) const {

		return measure(a->getModel(), b->getModel())
				+ log(2*a->getModel().getSubnodes()*b->getModel().getSubnodes() / (a->getModel().getSubnodes() + b->getModel().getSubnodes()))
				;
	}
};


template <class NodePointer, class ValueType>
struct WishartVectorMatrixDissimilarityMeasure: public SymmetricDissimilarityMeasure, public std::binary_function<
	NodePointer, NodePointer, ValueType>
{

	template<typename T>
	ValueType measure(T a, T b) const {
		ValueType res = ValueType();
		const size_t covariances = a.getNumCovariances();

		#pragma omp parallel for num_threads(min(static_cast<int>(covariances), omp_get_max_threads())) schedule (dynamic, 1) shared(a,b) reduction (+:res) default(none)
		for(size_t i = 0; i < covariances; ++i){
			arma::cx_mat ma = to_arma<arma::cx_mat>::from(a.getCovariance(i));
			arma::cx_mat mb = to_arma<arma::cx_mat>::from(b.getCovariance(i));
			res += real(trace(solve(ma,mb))) + real(trace(solve(mb,ma)));
		}
		return res;
	}

	ValueType operator()(NodePointer a, NodePointer b) const {
		return measure(a->getModel(), b->getModel()) * (a->getModel().getSubnodes() + b->getModel().getSubnodes());
	}
};

} /* namespace tscbpt */

#endif /* INCLUDE_TSC_BPT_MATRIXDISSIMILARITYMEASURE_HPP_ */
