/*
 * ArmadilloWrapper.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 14/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef ARMADILLOWRAPPER_HPP_
#define ARMADILLOWRAPPER_HPP_

#include <armadillo>
#include <tsc/data/matrix/Matrix.hpp>
#include <tsc/bpt/models/NativeMatrixModel.hpp>
#include <tsc/data/matrix/DenseMatrix.hpp>
#include <tsc/data/matrix/SquareMatrix.hpp>
#include <tsc/data/matrix/GetMinDimSize.hpp>
#include <tsc/bpt/models/DynamicMatrixModel.hpp>

namespace tscbpt
{

template<typename ArmaType>
struct to_arma{
	typedef ArmaType					out_type;

	template<typename FromType>
	static out_type from(FromType from);
};

template<>
struct to_arma<arma::cx_mat>{
	typedef arma::cx_mat				out_type;

	template<
		typename MatrixElem,
		size_t MatrixSize,
		size_t PosDims,
		typename PosTElem >
	static out_type from(NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem>& from){
		return arma::cx_mat::fixed<MatrixSize,MatrixSize>(from.getData());
	}

	template <typename MatrixElem, size_t MatrixSize, size_t PosDims, typename PosTElem>
	static const out_type from(const NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem>& from) {
		return arma::cx_mat::fixed<MatrixSize, MatrixSize>(const_cast<MatrixElem*>(from.getData()), false);
	}

	template<typename T>
	static out_type from(DenseMatrix<T>& from){
		return out_type(&(from(0,0)), from.getRows(), from.getCols(), false);
	}

	template <typename T>
	static out_type from(SquareMatrix<T>& from) {
		return out_type(&(from(0, 0)), from.getRows(), from.getCols(), false);
	}

	template<typename T, typename F>
	static out_type from(const Matrix_Base<T, F>& from){
		typedef F	elem_type;

		elem_type data[from.getRows() * from.getCols()];
		for(size_t i = 0; i < from.getCols(); ++i)
			for(size_t j = 0; j < from.getRows(); ++j)
				data[i*from.getRows() + j] = from(j,i);
		return out_type(data, from.getRows(), from.getCols(), true);
	}

	template <typename T, typename U, typename V>
	static out_type from(DynamicMatrixModel<T, U, V>& m) {
		return to_arma<arma::cx_mat>::from(m.getCovariance());
	}
};

template<size_t MatrixSize>
struct to_arma<arma::cx_mat::fixed<MatrixSize,MatrixSize> >{
	typedef arma::cx_mat::fixed<MatrixSize,MatrixSize>		out_type;

	template<
		typename MatrixElem,
		size_t PosDims,
		typename PosTElem >
	static out_type from(NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem>& from){
		return out_type(from.getData(), false);
	}

	template <typename MatrixElem, size_t PosDims, typename PosTElem>
	static out_type from(const NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem>& from) {
		return out_type(from.getData(), false);
	}
};

/**
 * Generic functor to get the minimum size of all the dimensions of an array.
 * IMPLEMENTATION for arma::Mat
 */
template<typename eT>
struct GetMinDimSize<arma::Mat<eT> >: public std::unary_function<arma::Mat<eT>,size_t> {
	inline size_t operator()(const arma::Mat<eT>& m) const {
		return std::min(m.n_rows, m.n_cols);
	}
};

/**
 * Generic functor to get the minimum size of all the dimensions of an array.
 * IMPLEMENTATION for arma::Mat<>::fixed<N,N>
 */
template<size_t N>
struct GetMinDimSize<arma::Mat<std::complex<double> >::fixed<N, N> >: public std::unary_function<arma::Mat<std::complex<double> >::fixed<N, N>, size_t> {
	inline size_t operator()(const arma::Mat<std::complex<double> >::fixed<N, N>& m) const {
		return N;
	}
};

/**
 * Generic functor to get the minimum size of all the dimensions of an array.
 * IMPLEMENTATION for arma::Mat<>::fixed<N,M>
 */
template<size_t N, size_t M>
struct GetMinDimSize<arma::cx_mat::fixed<N, M> >: public std::unary_function<arma::cx_mat::fixed<N, M>, size_t> {
	inline size_t operator()(const arma::cx_mat::fixed<N, M>& m) const {
		return std::min(N,M);
	}
};


/**
 * Definition of the hermitian transpose operator herm_trans().
 * Macro definition to avoid armadillo version-dependent
 * deprecated warnings.
 */
#if ARMA_VERSION_MAJOR >= 2
	#define herm_trans(m) 	(arma::trans((m)))
#else
	#define herm_trans(m) 	(arma::htrans((m)))
#endif

#if ARMA_VERSION_MAJOR >= 2
	#define simple_trans(m) 	(arma::strans((m)))
#else
	#define simple_trans(m) 	(arma::trans((m)))
#endif



}

#endif /* ARMADILLOWRAPPER_HPP_ */
