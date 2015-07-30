/*
 * TEBPT_config.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 14/03/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef TEBPT_CONFIG_H_
#define TEBPT_CONFIG_H_


#include <tscbpt.h>


/**
 * Default size of the submatrix size for the VectorMatrix models.
 * (may be passed as a compiler argument with -D flag)
 * For time series, the covariance matrices will be grouped into submatrices
 * of the size specified in SUBMATRIX_SIZE.
 */
#ifndef SUBMATRIX_SIZE
#define SUBMATRIX_SIZE 		3
#endif

using namespace tscbpt;

/**
 * Define the scattering vector operator (basis) and the corresponding output
 * folder.
 */
#if defined(NO_S_VECTOR_MOD)
	typedef tscbpt::NoOpScatteringVector				SOperator;
	#define OUTPUT_FOLDER						"/C3"
#elif defined(PAULI_S_VECTOR)
	typedef tscbpt::MonostaticPauliScatteringVector		SOperator;
	#define	 OUTPUT_FOLDER						"/T3"
#else
	typedef tscbpt::MonostaticScatteringVector			SOperator;
	#define OUTPUT_FOLDER						"/C3"
#endif


/**
 *  Define the region model employed for BPT processing
 */
// This is the main definition of the BPT Frame.
// In general, use BPTFrame<Model>
typedef BPTFrame<AddHomogeneity<VectorMatrixModel<complex<double>, size_t, float, SUBMATRIX_SIZE> > >		BPT;
//  typedef BPTFrame<AddLogDetAverage<VectorMatrixModel<complex<double>, size_t, float, SUBMATRIX_SIZE> > >		BPT;
//  typedef BPTFrame<AddHomogeneity<AddLogDetAverage<VectorMatrixModel<complex<double>, size_t, float, SUBMATRIX_SIZE> > > >		BPT;
// Other examples:
//typedef BPTFrame<AddHomogeneity<DynamicMatrixModel<> > >		BPT;
//typedef BPTFrame<AddHomogeneity<VectorMatrixModel<> > >		BPT;


/**
 *  Define the Dissimilarity measure employed for BPT construction
 */
//typedef DiagonalRevisedWishartDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue> Dissimilarity;
//typedef RevisedWishartDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue> Dissimilarity;
//typedef DiagonalGeodesicDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue> Dissimilarity;
//typedef GeodesicDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue> Dissimilarity;
//typedef HomogDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue>					Dissimilarity;
typedef GeodesicVectorMatrixDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue> Dissimilarity;
//Mixed dissimilarity
//typedef DiagonalGeodesicDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue> Diss1Measure;
//typedef GeodesicVectorDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue> Diss2Measure;
//typedef MixedDissimilarityMeasure<BPT::NodePointer, BPT::DissimilarityValue,Diss1Measure,Diss2Measure> Dissimilarity;


/**
 * Define pruning criterion
 */
// BPT Pruning
typedef RelErrorHomogeneityPruneCriterion<> 	PruneCriterion;
//typedef LogDetPruneCriterion<> 				PruneCriterion;


#endif /* TEBPT_CONFIG_H_ */
