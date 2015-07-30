/*
 * SubMatrixSize.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 15/02/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SUBMATRIXSIZE_HPP_
#define SUBMATRIXSIZE_HPP_

namespace tscbpt {

template<typename Type>
struct SubMatrixSize;
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
struct SubMatrixSize {
	typedef Type 		main_type;
};

/**
 * Specialization for Matrix_Base<>
 */
template<typename MatrixImpl, typename Field>
struct SubMatrixSize<Matrix_Base<MatrixImpl, Field> > {
	typedef Matrix_Base<MatrixImpl, Field> 		main_type;

	inline static size_t getValue(const main_type value){
		return min(value.getRows(), value.getCols());
	}
};

/**
 * Specialization for double
 */
template<>
struct SubMatrixSize<double> {
	typedef double 		main_type;

	inline static size_t getValue(const main_type){
		return 1;
	}
};

/**
 * Specialization for VectorMatrixModel<>
 */
template<typename Matrix_ElemType, typename NSubnodesType, typename PositionType, 	size_t SubMatrixSz>
struct SubMatrixSize<VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSz> > {
	typedef VectorMatrixModel<Matrix_ElemType, NSubnodesType, PositionType, SubMatrixSz> 		main_type;
	static const size_t value = SubMatrixSz;
	inline static size_t getValue(const main_type){
		return SubMatrixSz;
	}
};


/**
 * Specialization for NativeMatrixModel<>
 */
template <typename MatrixElem, size_t MatrixSize, size_t PosDims, 	typename PosTElem>
struct SubMatrixSize<NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem> > {
	typedef NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem> 		main_type;

	inline static size_t getValue(const main_type value){
		return value.getMatrixSize();
	}
};

/**
 * Specialization for NativeMatrixModelWithHomogeneity<>
 */
template <typename MatrixElem, size_t MatrixSize, size_t PosDims, 	typename PosTElem, typename HomogType>
struct SubMatrixSize<NativeMatrixModelWithHomogeneity<MatrixElem, MatrixSize, PosDims, PosTElem, HomogType> > {
	typedef NativeMatrixModelWithHomogeneity<MatrixElem, MatrixSize, PosDims, PosTElem, HomogType> 		main_type;

	inline static size_t getValue(const main_type value){
		return value.getMatrixSize();
	}
};

/**
 * Specialization for AddHomogeneity<>
 */
template<class Base, typename HomogType>
struct SubMatrixSize<AddHomogeneity<Base, HomogType> > {
	typedef AddHomogeneity<Base, HomogType> 		main_type;

	inline static size_t getValue(const main_type value){
		return SubMatrixSize<Base>::getValue(static_cast<Base>(value));
	}
};

/**
 * Specialization for AddLogDetAverage<>
 */
template<class Base, typename HomogType>
struct SubMatrixSize<AddLogDetAverage<Base, HomogType> > {
	typedef AddLogDetAverage<Base, HomogType> 		main_type;

	inline static size_t getValue(const main_type value){
		return SubMatrixSize<Base>::getValue(static_cast<Base>(value));
	}
};

/**
 * Specialization for VectorModel<>
 */
template<typename ElemType, typename NSubnodesType, typename PositionType>
struct SubMatrixSize<VectorModel<ElemType, NSubnodesType, PositionType> > {
	typedef VectorModel<ElemType, NSubnodesType, PositionType> 		main_type;

	inline static size_t getValue(const main_type value){
		return SubMatrixSize<ElemType>::getValue(value(0));
	}
};


} /* namespace tscbpt */
#endif /* SUBMATRIXSIZE_HPP_ */
