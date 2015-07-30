/*
 * NativeMatrixModelWithHomogeneity.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 22/07/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef NATIVEMATRIXMODELWITHHOMOGENEITY_HPP_
#define NATIVEMATRIXMODELWITHHOMOGENEITY_HPP_



// *************** Definitions ******************

namespace tscbpt{
/**
 * Decorator over NativeMatrixModel<> adding also a field to hold
 * the total sum of the squared error of the wrapped model.
 * It is based on the dist2() generic function.
 */
template <
	typename 		MatrixElem,
	size_t 			MatrixSize,
	size_t 			PosDims,
	typename 		PosTElem,
	typename 		HomogType>
class NativeMatrixModelWithHomogeneity;
}



// ************* Implementation *****************

#include "NativeMatrixModel.hpp"
#include <tsc/util/types/WrapperOf.hpp>

namespace tscbpt{

template <
	typename MatrixElem = std::complex<double>,
	size_t MatrixSize = 3,
	size_t PosDims = 2,
	typename PosTElem = float,
	typename HomogType = double >
class NativeMatrixModelWithHomogeneity:
	public NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem>,
	public WrapperOf<NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem> >
//	public VectorialModel_Base<
//		NativeMatrixModelWithHomogeneity<MatrixElem, MatrixSize, PosDims, PosTElem, HomogType> , MatrixElem>
{
public:
	typedef NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem> 							SuperType;
	typedef NativeMatrixModelWithHomogeneity<MatrixElem, MatrixSize, PosDims, PosTElem, HomogType> 	ThisType;
	typedef HomogType																				TSSType;

	NativeMatrixModelWithHomogeneity(const SuperType& amodel) :
		SuperType(amodel), _homog(0){ }

	//TODO: Change homogeneity to TSS
//	HomogType getHomogeneity() const __attribute__ ((deprecated)) {
//		return _homog;
//	}

	HomogType getTotalSumOfSquares() const{
		return _homog;
	}

	ThisType merge(const ThisType& b) {

		SuperType superTmp = SuperType::merge(b);
		ThisType tmp(superTmp);

		// Compute homogeneity
		tmp._homog = this->_homog + b._homog + (dist2(*this, b)) * this->getSubnodes() * b.getSubnodes()
			/ (this->getSubnodes() + b.getSubnodes());

		return tmp;
	}

	ThisType unmerge(const ThisType& b) {
		SuperType superTmp = SuperType::unmerge(b);
		ThisType tmp(superTmp);

		// Compute homogeneity
		tmp._homog = this->_homog - b._homog + (dist2(tmp, b)) * tmp.getSubnodes() * b.getSubnodes()
			/ (tmp.getSubnodes() + b.getSubnodes());

		return tmp;
	}

protected:
	HomogType _homog;
};


}

#endif /* NATIVEMATRIXMODELWITHHOMOGENEITY_HPP_ */
