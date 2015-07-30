/*
 * TotalMatrixSize.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 18/02/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef TOTALMATRIXSIZE_HPP_
#define TOTALMATRIXSIZE_HPP_

namespace tscbpt {

template<typename Type>
struct TotalMatrixSize;
}

#include <cstddef>
#include <cmath>
#include <tsc/data/matrix/Matrix.hpp>
#include <tsc/bpt/models/VectorMatrixModel.hpp>
#include <tsc/bpt/models/NativeMatrixModel.hpp>
#include <tsc/bpt/models/NativeMatrixModelWithHomogeneity.hpp>
#include <tsc/bpt/models/AddHomogeneity.hpp>
#include <tsc/bpt/models/AddLogDetAverage.hpp>
#include <tsc/bpt/models/VectorModel.hpp>


namespace tscbpt {


template<typename Type>
struct TotalMatrixSize {
	typedef Type 		main_type;
};

/**
 * Specialization for Matrix_Base<>
 */
template<typename MatrixImpl, typename Field>
struct TotalMatrixSize<Matrix_Base<MatrixImpl, Field> > {
	typedef Matrix_Base<MatrixImpl, Field> 		main_type;

	inline static size_t getValue(const main_type value){
		return min(value.getRows(), value.getCols());
	}
};

/**
 * Specialization for double
 */
template<>
struct TotalMatrixSize<double> {
	typedef double 		main_type;

	inline static size_t getValue(const main_type){
		return 1;
	}
};

/**
 * Specialization for VectorMatrixModel<>
 */
template<typename Matrix_ElemType, typename NSubnodesType, typename PositionType, 	size_t SubMatrixSz>
struct TotalMatrixSize<VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSz> > {
	typedef VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSz> 		main_type;
	static const size_t value = SubMatrixSz;
	inline static size_t getValue(const main_type value){
		return value.getMatrixSize();
	}
};


/**
 * Specialization for NativeMatrixModel<>
 */
template <typename MatrixElem, size_t MatrixSize, size_t PosDims, 	typename PosTElem>
struct TotalMatrixSize<NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem> > {
	typedef NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem> 		main_type;

	inline static size_t getValue(const main_type value){
		return value.getMatrixSize();
	}
};

/**
 * Specialization for NativeMatrixModelWithHomogeneity<>
 */
template <typename MatrixElem, size_t MatrixSize, size_t PosDims, 	typename PosTElem, typename HomogType>
struct TotalMatrixSize<NativeMatrixModelWithHomogeneity<MatrixElem, MatrixSize, PosDims, PosTElem, HomogType> > {
	typedef NativeMatrixModelWithHomogeneity<MatrixElem, MatrixSize, PosDims, PosTElem, HomogType> 		main_type;

	inline static size_t getValue(const main_type value){
		return value.getMatrixSize();
	}
};

/**
 * Specialization for AddHomogeneity<>
 */
template<class Base, typename HomogType>
struct TotalMatrixSize<AddHomogeneity<Base, HomogType> > {
	typedef AddHomogeneity<Base, HomogType> 		main_type;

	inline static size_t getValue(const main_type value){
		return TotalMatrixSize<Base>::getValue(static_cast<Base>(value));
	}
};

/**
 * Specialization for AddLogDetAverage<>
 */
template<class Base, typename HomogType>
struct TotalMatrixSize<AddLogDetAverage<Base, HomogType> > {
	typedef AddLogDetAverage<Base, HomogType> 		main_type;

	inline static size_t getValue(const main_type value){
		return TotalMatrixSize<Base>::getValue(static_cast<Base>(value));
	}
};

/**
 * Specialization for VectorModel<>
 */
template<typename ElemType, typename NSubnodesType, typename PositionType>
struct TotalMatrixSize<VectorModel<ElemType, NSubnodesType, PositionType> > {
	typedef VectorModel<ElemType, NSubnodesType, PositionType> 		main_type;

	inline static size_t getValue(const main_type value){
		return value.getNumElems() * TotalMatrixSize<ElemType>::getValue(value(0));
	}
};


} /* namespace tscbpt */
#endif /* TOTALMATRIXSIZE_HPP_ */
