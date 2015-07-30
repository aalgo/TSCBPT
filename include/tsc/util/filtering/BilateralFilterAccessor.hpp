/*
 * BilateralFilterAccessor.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 07/05/2012
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef BILATERALFILTERACCESSOR_HPP_
#define BILATERALFILTERACCESSOR_HPP_

#include <cstddef>	// for size_t
#include <cassert>
#include <deque>
#include <vector>
#include <tsc/util/types/types.h>

namespace tscbpt
{

/**
 * Class to perform the iterative bilateral filter at once, as described
 * in [1] by computing all the weight refinement iterations at once.
 * It employs a multilevel cache queue in order to be able to store the
 * temporary previous results.
 * Additionally, it has accessor interface: the elements are accessed through
 * the accessor(row,col) operator, and the new values are computed by rows
 * on demand.
 * It allows an unary operator (functor) to be applied to each element
 * (the Converter template parameter)
 * NOTE: Only some previous rows results in the cache are accessible. The
 * class is not intended for random access, but for (almost) consecutive
 * element access.
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<
	class ArrayDataIn,
	class ArrayDataRef,
	class SimilarityMeasure,
	class Converter 			= NoOpConverter<typename ContainerValueType<ArrayDataRef>::type >
>
class BilateralFilterAccessor
{
public:
	typedef ArrayDataIn						array_data_in_type;
	typedef ArrayDataRef					array_data_ref_type;
	typedef SimilarityMeasure				similarity_type;
	typedef typename Converter::result_type	value_type;
	typedef Converter						converter_type;

	typedef vector<value_type>				cache_line_type;
	typedef deque<cache_line_type>			cache_type;
	typedef vector<cache_type>				cache_vector_type;

	typedef vector<double>					k_cache_line_type;
	typedef deque<k_cache_line_type>		k_cache_type;
	typedef vector<k_cache_type>			k_cache_vector_type;

private:
	cache_vector_type	cache;
	k_cache_vector_type kcache;
	similarity_type& d;
	converter_type conv;

	array_data_in_type &in;
	array_data_ref_type &ref;
	unsigned long cols, rows, filterHeight, filterWidth;
	vector<unsigned long> last_cached_line;
	unsigned long iterations;
	double sigma_s, sigma_p;

	void initialize_cache(){
		long c = iterations-1;
		unsigned long initial_cache_size = min(1 + static_cast<unsigned long>(filterHeight / 2) * c, rows);
		for(unsigned long f = 0; f < initial_cache_size; ++f){
			cache[c].push_back(cache_line_type(cols));
			kcache[c].push_back(k_cache_line_type(cols));
			#pragma omp parallel for
			for(unsigned long i = 0; i < cols; ++i) {
				double normF = 1.0;
				cache[c].back()[i] = conv(in(f,i));
				for(signed long k = static_cast<signed long>(f) - (filterHeight-1)/2; k <= static_cast<signed long>(f +filterHeight/2); ++k){
					for(signed long l = static_cast<signed long>(i) - (filterWidth-1)/2; l <= static_cast<signed long>(i + filterWidth/2); ++l){
						if(!(k == static_cast<signed long>(f) && l == static_cast<signed long>(i))){
							unsigned long m = abs(k) - (k > static_cast<signed long> (rows - 1)) * (2 * (k - rows + 1));
							unsigned long n = abs(l) - (l > static_cast<signed long> (cols - 1)) * (2 * (l - cols + 1));
							double distV = 1.0 / (1 + d(conv(ref(f,i)), conv(ref(m,n))) / pow(sigma_p, 2)) /
								(1 + (pow(static_cast<double> (k) - f, 2.0) + pow(static_cast<double> (l) - i, 2.0)) / pow(sigma_s, 2));
							cache[c].back()[i] += conv(in(m,n)) * distV;
							normF += distV;
						}
					}
				}
				cache[c].back()[i] /= normF;
				kcache[c].back()[i] = normF;
			}
			last_cached_line[c] = f;
		}
		for(c = iterations-2; c >=0; --c){
			initial_cache_size = min(1 + static_cast<unsigned long>(filterHeight / 2) * c, rows);
			for(unsigned long f = 0; f < initial_cache_size; ++f){
				cache[c].push_back(cache_line_type(cols));
				kcache[c].push_back(k_cache_line_type(cols));
				#pragma omp parallel for
				for(unsigned long i = 0; i < cols; ++i) {
					double normF = 1.0;
					cache[c].back()[i] = conv(in(f,i));
					for(signed long k = static_cast<signed long>(f) - (filterHeight-1)/2; k <= static_cast<signed long>(f +filterHeight/2); ++k){
						for(signed long l = static_cast<signed long>(i) - (filterWidth-1)/2; l <= static_cast<signed long>(i + filterWidth/2); ++l){
							if(!(k == static_cast<signed long>(f) && l == static_cast<signed long>(i))){
								unsigned long m = abs(k) - (k > static_cast<signed long> (rows - 1)) * (2 * (k - rows + 1));
								unsigned long n = abs(l) - (l > static_cast<signed long> (cols - 1)) * (2 * (l - cols + 1));
								double distV = 1.0 / (1 + d(cache[c+1][f][i], cache[c+1][m][n]) / pow(sigma_p, 2)) /
									(1 + (pow(static_cast<double> (k) - f, 2.0) + pow(static_cast<double> (l) - i, 2.0)) / pow(sigma_s, 2));
								cache[c].back()[i] += conv(in(m,n)) * distV;
								normF += distV;
							}
						}
					}
					cache[c].back()[i] /= normF;
					kcache[c].back()[i] = normF;
				}
				last_cached_line[c] = f;
			}
		}

	}

	void loadCacheLine(){
		// This method must be called to cache consecutive lines
		long c = iterations-1;
		unsigned long line = last_cached_line[c] + 1;
		if(line < rows){
			cache[c].push_back(cache_line_type(cols));
			kcache[c].push_back(k_cache_line_type(cols));
			if(cache[c].size() > min(1 + static_cast<unsigned long>(filterHeight / 2) * 2 * c, rows))  cache[c].pop_front();
			if(kcache[c].size() > min(1 + static_cast<unsigned long>(filterHeight / 2) * 2 * c, rows)) kcache[c].pop_front();
			#pragma omp parallel for
			for(unsigned long i = 0; i < cols; ++i) {
				double normF = 1.0;
				cache[c].back()[i] = conv(in(line,i));
				for(signed long k = static_cast<signed long>(line) - (filterHeight-1)/2; k <= static_cast<signed long>(line +filterHeight/2); ++k){
					for(signed long l = static_cast<signed long>(i) - (filterWidth-1)/2; l <= static_cast<signed long>(i + filterWidth/2); ++l){
						if(!(k == static_cast<signed long>(line) && l == static_cast<signed long>(i))){
							unsigned long m = abs(k) - (k > static_cast<signed long> (rows - 1)) * (2 * (k - rows + 1));
							unsigned long n = abs(l) - (l > static_cast<signed long> (cols - 1)) * (2 * (l - cols + 1));
							double distV = 1.0 / (1 + d(conv(ref(line,i)), conv(ref(m,n))) / pow(sigma_p, 2)) /
								(1 + (pow(static_cast<double> (k) - line, 2.0) + pow(static_cast<double> (l) - i, 2.0)) / pow(sigma_s, 2));
							cache[c].back()[i] += conv(in(m,n)) * distV;
							normF += distV;
						}
					}
				}
				cache[c].back()[i] /= normF;
				kcache[c].back()[i] = normF;
			}
			last_cached_line[c]++;
		}
		for(c = iterations-2; c >=0; --c){
			line = last_cached_line[c] + 1;
			if(line < rows){
				cache[c].push_back(cache_line_type(cols));
				kcache[c].push_back(k_cache_line_type(cols));
				if(cache[c].size() > min(1 + static_cast<unsigned long>(filterHeight / 2) * 2 * c, rows))  cache[c].pop_front();
				if(kcache[c].size() > min(1 + static_cast<unsigned long>(filterHeight / 2) * 2 * c, rows)) kcache[c].pop_front();
				#pragma omp parallel for
				for(unsigned long i = 0; i < cols; ++i) {
					double normF = 1.0;
					cache[c].back()[i] = conv(in(line,i));
					for(signed long k = static_cast<signed long>(line) - (filterHeight-1)/2; k <= static_cast<signed long>(line +filterHeight/2); ++k){
						for(signed long l = static_cast<signed long>(i) - (filterWidth-1)/2; l <= static_cast<signed long>(i + filterWidth/2); ++l){
							if(!(k == static_cast<signed long>(line) && l == static_cast<signed long>(i))){
								unsigned long m = abs(k) - (k > static_cast<signed long> (rows - 1)) * (2 * (k - rows + 1));
								unsigned long n = abs(l) - (l > static_cast<signed long> (cols - 1)) * (2 * (l - cols + 1));
								unsigned long cache_line = line - last_cached_line[c+1] + cache[c+1].size() - 1;
								unsigned long cache_m = m - last_cached_line[c+1] + cache[c+1].size() - 1;
								double distV = 1.0 / (1 + d(cache[c+1][cache_line][i], cache[c+1][cache_m][n]) / pow(sigma_p, 2)) /
									(1 + (pow(static_cast<double> (k) - line, 2.0) + pow(static_cast<double> (l) - i, 2.0)) / pow(sigma_s, 2));
								cache[c].back()[i] += conv(in(m,n)) * distV;
								normF += distV;
							}
						}
					}
					cache[c].back()[i] /= normF;
					kcache[c].back()[i] = normF;
				}
				last_cached_line[c]++;
			}
		}
	}

public:
	BilateralFilterAccessor(array_data_in_type& in_array, array_data_ref_type& ref_array, similarity_type& d,
		unsigned long nRows, unsigned long nCols, unsigned long filterRows, unsigned long filterCols, double sigma_s, double sigma_p,
		unsigned long iterations, converter_type conv = converter_type()): d(d), in(in_array), ref(ref_array) {

		assert(iterations > 0);

		cols = nCols;
		rows = nRows;
		filterHeight = filterRows;
		filterWidth = filterCols;

		this->sigma_s = sigma_s;
		this->sigma_p = sigma_p;
		this->iterations = iterations;
		this->conv = conv;

		// Initialize cache
		cache.resize(iterations);
		kcache.resize(iterations);
		last_cached_line.resize(iterations);
		initialize_cache();
	}

	value_type operator()(unsigned long i, unsigned long j) {
		assert(i >= last_cached_line.front() + 1 - cache.front().size());
		assert(i < rows);
		assert(j < cols);
		// Update cache if needed
		if(i > last_cached_line.front()){
			loadCacheLine();
		}
		return cache.front()[i - (last_cached_line.front() - cache.front().size()) - 1][j];
	}

	// The k value for the last iteration is obtained properly
	double k_value(unsigned long i, unsigned long j) {
		assert(i >= last_cached_line.front() + 1 - cache.front().size());
		assert(i < rows);
		assert(j < cols);
		// Update cache if needed
		while (i > last_cached_line.front()) {
			loadCacheLine();
		}
		return kcache.front()[i - (last_cached_line.front() - cache.front().size()) - 1][j];
	}

	// FIXME BUG: May generate problems related with border effects
	// Only affects to k values of inner iterations, last iteration is OK.
	double k_value(unsigned long it, unsigned long i, unsigned long j) {
		assert(i >= last_cached_line.front() + 1 - cache.front().size());
		assert(i < rows);
		assert(j < cols);
		assert(it > 0);
		assert(it <= iterations);
		// Update cache if needed
		while (i > last_cached_line.front()) {
			loadCacheLine();
		}
		return kcache[iterations-it][i - (last_cached_line.front() - cache.front().size()) - 1][j];
	}

};

// make() function definition for convenience.
template<
	class ArrayDataIn,
	class ArrayDataRef,
	class SimilarityMeasure,
	class Converter
>
BilateralFilterAccessor<ArrayDataIn, ArrayDataRef, SimilarityMeasure, Converter> make_iterative_crossBLFAccessor(
	ArrayDataIn& in, ArrayDataRef& ref, SimilarityMeasure& diss,
	unsigned long nRows, unsigned long nCols, unsigned long filterRows, unsigned long filterCols, double sigma_s, double sigma_p,
	unsigned long iterations, Converter conv){
	return BilateralFilterAccessor<ArrayDataIn, ArrayDataRef, SimilarityMeasure, Converter>
		(in, ref, diss, nRows, nCols, filterRows, filterCols, sigma_s, sigma_p, iterations, conv);
}

}

#endif /* BILATERALFILTERACCESSOR_HPP_ */
