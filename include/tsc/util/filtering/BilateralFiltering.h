/*
 * BilateralFiltering.h
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

#ifndef BILATERALFILTERING_H_
#define BILATERALFILTERING_H_

#include <cstddef>	// for size_t
#include <vector>
#include <deque>
#include <limits>
#include <omp.h>
#include <tsc/util/Algorithms.h>

namespace tscbpt {

/**
 * Apply Distance based cross bilateral filter, as described in [1].
 * NOTE: It only performs one iteration of the weight refinement scheme
 * described in [1].
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<class Array, class DistanceMeasure>
void crossBilateralDBF2Filter(Array& in, Array& ref, Array& out, size_t startRow, size_t startCol, size_t endRow, size_t endCol,
		size_t filterRows, size_t filterCols, DistanceMeasure d, double sigma_s, double sigma_p){
	ProgressDisplay show_progress( (endRow-startRow)*(endCol-startCol) );

	#pragma omp parallel for schedule (dynamic, 1)
	for(size_t i = startRow; i<endRow; i++){
		for(size_t j = startCol; j<endCol; j++){

			double normF = 0.0;
			for(signed long k = static_cast<signed long>(i) - (filterRows-1)/2; k <= static_cast<signed long>(i + filterRows/2); ++k){
				for(signed long l = static_cast<signed long>(j) - (filterCols-1)/2; l <= static_cast<signed long>(j + filterCols/2); ++l){
					size_t m = abs(k) - (k > static_cast<signed long> (endRow - 1)) * (2 * (k - endRow + 1));
					size_t n = abs(l) - (l > static_cast<signed long> (endCol - 1)) * (2 * (l - endCol + 1));
					double distV = 1.0 / (1 + d(ref[i][j], ref[m][n]) / pow(sigma_p, 2)) /
						(1 + (pow(static_cast<double> (k) - i, 2.0) + pow(static_cast<double> (l) - j, 2.0)) / pow(sigma_s, 2));
					out[i][j] += in[m][n] * distV;
					normF += distV;
				}
			}
			out[i][j] /= normF;
			#pragma omp critical
			{++show_progress;}
		}
	}
}

/**
 * Apply Distance based cross bilateral filter, as described in [1].
 * NOTE: It only performs one iteration of the weight refinement scheme
 * described in [1].
 * This method takes also one output parameter (kout) for getting the
 * number of averaged pixels per pixel (the k parameter)[1].
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<class Array, class NormArray, class DistanceMeasure>
void crossBilateralDBF2Filter(Array& in, Array& ref, Array& out, NormArray& kout, size_t startRow, size_t startCol, size_t endRow, size_t endCol,
		size_t filterRows, size_t filterCols, DistanceMeasure d, double sigma_s, double sigma_p){
	ProgressDisplay show_progress( (endRow-startRow)*(endCol-startCol) );

	#pragma omp parallel for schedule (dynamic, 1)
	for(size_t i = startRow; i<endRow; i++){
		for(size_t j = startCol; j<endCol; j++){

			double normF = 0.0;
			for(signed long k = static_cast<signed long>(i) - (filterRows-1)/2; k <= static_cast<signed long>(i + filterRows/2); ++k){
				for(signed long l = static_cast<signed long>(j) - (filterCols-1)/2; l <= static_cast<signed long>(j + filterCols/2); ++l){
					size_t m = abs(k) - (k > static_cast<signed long> (endRow - 1)) * (2 * (k - endRow + 1));
					size_t n = abs(l) - (l > static_cast<signed long> (endCol - 1)) * (2 * (l - endCol + 1));
					double distV = 1.0 / (1 + d(ref[i][j], ref[m][n]) / pow(sigma_p, 2)) /
						(1 + (pow(static_cast<double> (k) - i, 2.0) + pow(static_cast<double> (l) - j, 2.0)) / pow(sigma_s, 2));
					out[i][j] += in[m][n] * distV;
					normF += distV;
				}
			}
			out[i][j] /= normF;
			kout[i][j] = normF;
			#pragma omp critical
			{++show_progress;}
		}
	}
}

/**
 * Equivalent to crossBilateralDBF2Filter() but without progress output
 * (not using the ProgressDisplay class).
 */
template<class Array, class DistanceMeasure>
void crossBilateralDBF2FilterQuiet(Array& in, Array& ref, Array& out, size_t startRow, size_t startCol, size_t endRow, size_t endCol,
		size_t filterRows, size_t filterCols, DistanceMeasure d, double sigma_s, double sigma_p){

	#pragma omp parallel for schedule (dynamic, 1)
	for(size_t i = startRow; i<endRow; i++){
		for(size_t j = startCol; j<endCol; j++){

			double normF = 0.0;
			for(signed long k = static_cast<signed long>(i) - (filterRows-1)/2; k <= static_cast<signed long>(i + filterRows/2); ++k){
				for(signed long l = static_cast<signed long>(j) - (filterCols-1)/2; l <= static_cast<signed long>(j + filterCols/2); ++l){
					size_t m = abs(k) - (k > static_cast<signed long> (endRow - 1)) * (2 * (k - endRow + 1));
					size_t n = abs(l) - (l > static_cast<signed long> (endCol - 1)) * (2 * (l - endCol + 1));
					double distV = 1.0 / (1 + d(ref[i][j], ref[m][n]) / pow(sigma_p, 2)) /
						(1 + (pow(static_cast<double> (k) - i, 2.0) + pow(static_cast<double> (l) - j, 2.0)) / pow(sigma_s, 2));
					out[i][j] += in[m][n] * distV;
					normF += distV;
				}
			}
			out[i][j] /= normF;
		}
	}
}

/**
 * Equivalent to crossBilateralDBF2Filter() but without progress output
 * (not using the ProgressDisplay class).
 * This method takes also one output parameter (kout) for getting the
 * number of averaged pixels per pixel (the k parameter).
 */
template<class Array, class NormArray, class DistanceMeasure>
void crossBilateralDBF2FilterQuiet(Array& in, Array& ref, Array& out, NormArray& kout, size_t startRow, size_t startCol, size_t endRow, size_t endCol,
		size_t filterRows, size_t filterCols, DistanceMeasure d, double sigma_s, double sigma_p){

	#pragma omp parallel for schedule (dynamic, 1)
	for(size_t i = startRow; i<endRow; i++){
		for(size_t j = startCol; j<endCol; j++){

			double normF = 0.0;
			for(signed long k = static_cast<signed long>(i) - (filterRows-1)/2; k <= static_cast<signed long>(i + filterRows/2); ++k){
				for(signed long l = static_cast<signed long>(j) - (filterCols-1)/2; l <= static_cast<signed long>(j + filterCols/2); ++l){
					size_t m = abs(k) - (k > static_cast<signed long> (endRow - 1)) * (2 * (k - endRow + 1));
					size_t n = abs(l) - (l > static_cast<signed long> (endCol - 1)) * (2 * (l - endCol + 1));
					double distV = 1.0 / (1 + d(ref[i][j], ref[m][n]) / pow(sigma_p, 2)) /
						(1 + (pow(static_cast<double> (k) - i, 2.0) + pow(static_cast<double> (l) - j, 2.0)) / pow(sigma_s, 2));
					out[i][j] += in[m][n] * distV;
					normF += distV;
				}
			}
			out[i][j] /= normF;
			kout[i][j] = normF;
		}
	}
}

/**
 * Helper method for iterativeCrossBilateralDBF2Filter()
 */
template<class ArrayIn, class ArrayOut, class Converter>
void copySubArray(ArrayIn in, ArrayOut out, size_t startRow, size_t startCol, size_t endRow, size_t endCol, Converter conv){
	for (size_t i = startRow; i < endRow; i++) {
		for (size_t j = startCol; j < endCol; j++) {
			conv(out(i,j)) = in(i,j);
		}
	}
}

/**
 * Helper method for iterativeCrossBilateralDBF2Filter()
 */
template<class ArrayIn, class ArrayOut, class kOut, class Converter>
void copySubArray_and_kvalue(ArrayIn in, ArrayOut out, kOut& kout, size_t startRow, size_t startCol, size_t endRow, size_t endCol, Converter conv){
	for (size_t i = startRow; i < endRow; i++) {
		for (size_t j = startCol; j < endCol; j++) {
			conv(out(i,j)) = in(i,j);
			kout(i,j) = in.k_value(i,j);
		}
	}
}

/**
 * Perform distance based bilateral filtering [1] employing the
 * BilateralFilterAccessor class.
 * NOTE: This method performs the complete weight refinement scheme defined
 * in [1] for the number of iterations defined in the 'iterations' parameter.
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<class ArrayIn, class ArrayOut, class DistanceMeasure, class Converter>
void iterativeCrossBilateralDBF2Filter(ArrayIn in, ArrayIn ref, ArrayOut out, DistanceMeasure& diss,size_t rows, size_t cols,
		size_t filterRows, size_t filterCols, double sigma_s, double sigma_p, size_t iterations, Converter conv){
	copySubArray(
		make_iterative_crossBLFAccessor(in, ref, diss, rows, cols, filterRows, filterCols, sigma_s, sigma_p, iterations, conv),
		out, 0, 0, rows, cols, conv);
}

/**
 * Perform distance based bilateral filtering [1] employing the
 * BilateralFilterAccessor class.
 * This method takes also one output parameter (kout) for getting the
 * number of averaged pixels per pixel (the k parameter).
 * NOTE: This method performs the complete weight refinement scheme defined
 * in [1] for the number of iterations defined in the 'iterations' parameter.
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<class ArrayIn, class ArrayOut, class kOut, class DistanceMeasure, class Converter>
void iterativeCrossBilateralDBF2Filter(ArrayIn in, ArrayIn ref, ArrayOut out, kOut& kout, DistanceMeasure& diss,size_t rows, size_t cols,
		size_t filterRows, size_t filterCols, double sigma_s, double sigma_p, size_t iterations, Converter conv){
	copySubArray_and_kvalue(
		make_iterative_crossBLFAccessor(in, ref, diss, rows, cols, filterRows, filterCols, sigma_s, sigma_p, iterations, conv),
		out, kout, 0, 0, rows, cols, conv);
}

/**
 * Method to compute automatically the sigma_t parameter, as described
 * in [1].
 * NOTE: May not work properly if the image do not contain any dark
 * area. In this case, specify manually [1].
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<class ArrayIn, class Converter>
double compute_sigma_t(ArrayIn in, size_t rows, size_t cols, Converter conv, size_t block_size = 15){
	typedef typename Converter::result_type		value_type;
	GetMinDimSize<value_type>					getSize;
	double min_power = numeric_limits<double>::max();
	for(size_t i = 0; i+block_size < rows; ){
		for(size_t j = 0; j+block_size < cols; ){
			value_type v = conv(in(i,j));
			for(size_t k = i; k < i+block_size; ++k){
				for(size_t l = j; l < j+block_size; ++l){
					v = v + conv(in(k,l));
				}
			}
			v = (v - conv(in(i,j))) / (block_size*block_size);
			for(size_t k = 0; k < getSize(v); ++k){
				min_power = min(min_power, real(v(k,k)));
			}
			j += block_size;
		}
		i += block_size;
	}
	return (min_power < numeric_limits<double>::max())? min_power : 0.0;
}

} // namespace tscbpt

#endif /* BILATERALFILTERING_H_ */
