/*
 * BoxCarFiltering.h
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

#ifndef BOXCARFILTERING_H_
#define BOXCARFILTERING_H_

#include <tsc/util/types/TypeTraits.hpp>
#include <cstddef> // for size_t

namespace tscbpt {

/**
 * BoxCar filter of the complete array (full) preserving image size
 * (applying mirroring to solve border effect).
 * Applied to filter an array of NodePointer.
 * It assumes the separability of the filter and applies it independently for
 * each dimension (more efficient).
 */
//TODO: Generalize this function (Not only for NodePointer array)
template<class Array>
void boxCarFilter2DFullInterp(Array array, size_t rows, size_t cols, size_t filterRows, size_t filterCols){
	// Assuming that Array is an array of node pointers, obtain the
	// region model type for the buffer definition
	typedef typename PointerTraits<typename Array::value_type>::pointeeType::RegionModel		ModelType;
	std::vector<ModelType> buffer;
	buffer.reserve(max(rows,cols));
	for(size_t i = 0; i < rows; i++){
		for(size_t j = 0; j < cols; j++){
			buffer.push_back(array[i*cols + j]->getModel());
		}
		for(size_t j = 0; j < cols; j++){
			for(signed long k = -(static_cast<signed long>(filterCols)-1)/2; k <= static_cast<signed long>(filterCols)/2; k++){
				size_t ck = abs(static_cast<signed long>(j)+k);
				if(ck >= cols) ck = 2*cols - ck - 2;
				if(ck!=j) array[i*cols + j]->getModel() += buffer[ck];
			}
		}
		buffer.clear();
	}
	const double factor = 1.0 / (filterRows*filterCols);
	for (size_t j = 0; j < cols; j++) {
		for(size_t i = 0; i < rows; i++){
			buffer.push_back(array[i*cols + j]->getModel());
		}
		for(size_t i = 0; i < rows; i++){
			for(signed long k = -(static_cast<signed long>(filterRows)-1)/2; k <= static_cast<signed long>(filterRows)/2; k++){
				size_t ck = abs(static_cast<signed long>(i)+k);
				if(ck >= rows) ck = 2*rows - ck - 2;
				if(ck!=i) array[i*cols + j]->getModel() += buffer[ck];
			}
			array[i*cols + j]->getModel() *= factor;
		}
		buffer.clear();
	}
}

/**
 * BoxCar filter of the complete array (full) in 3D preserving image size
 * (applying mirroring to solve border effect).
 * Applied to filtering an array of NodePointer.
 * It assumes the separability of the filter and applies it independently for
 * each dimension (more efficient).
 */
//TODO: Generalize this function (Not only for NodePointer array)
template<class Array>
void boxCarFilter3DFullInterp(Array array, size_t rows, size_t cols, size_t slices, size_t filterRows, size_t filterCols, size_t filterSlices){
	// Assuming that Array is an array of node pointers, obtain the
	// region model type for the buffer definition
	typedef typename PointerTraits<typename Array::value_type>::pointeeType::RegionModel		ModelType;
	std::vector<ModelType> buffer;
	buffer.reserve(max(max(rows,cols),slices));
	for(size_t h = 0; h< slices; h++){
		for(size_t i = 0; i < rows; i++){
			for(size_t j = 0; j < cols; j++){
				buffer.push_back(array[h*rows*cols + i*cols + j]->getModel());
			}
			for(size_t j = 0; j < cols; j++){
				for(signed long k = -(static_cast<signed long>(filterCols)-1)/2; k <= static_cast<signed long>(filterCols)/2; k++){
					size_t ck = abs(static_cast<signed long>(j)+k);
					if(ck >= cols) ck = 2*cols - ck - 2;
					if(ck!=j) array[h*rows*cols + i*cols + j]->getModel() += buffer[ck];
				}
			}
			buffer.clear();
		}
	}
	for(size_t h = 0; h< slices; h++){
		for(size_t j = 0; j < cols; j++){
			for(size_t i = 0; i < rows; i++){
				buffer.push_back(array[h*rows*cols + i*cols + j]->getModel());
			}
			for(size_t i = 0; i < rows; i++){
				for(signed long k = -(static_cast<signed long>(filterRows)-1)/2; k <= static_cast<signed long>(filterRows)/2; k++){
					size_t ck = abs(static_cast<signed long>(j)+k);
					if(ck >= rows) ck = 2*rows - ck - 2;
					if(ck!=i) array[h*rows*cols + i*cols + j]->getModel() += buffer[ck];
				}
			}
			buffer.clear();
		}
	}
	const double factor = 1.0 / (filterRows*filterCols*filterSlices);
	for(size_t i = 0; i < rows; i++){
		for(size_t j = 0; j < cols; j++){
			for(size_t h = 0; h< slices; h++){
				buffer.push_back(array[h*rows*cols + i*cols + j]->getModel());
			}
			for(size_t h = 0; h< slices; h++){
				for(signed long k = -(static_cast<signed long>(filterSlices)-1)/2; k <= static_cast<signed long>(filterSlices)/2; k++){
					size_t ck = abs(static_cast<signed long>(j)+k);
					if(ck >= slices) ck = 2*slices - ck - 2;
					if(ck!=h) array[h*rows*cols + i*cols + j]->getModel() += buffer[ck];
				}
				array[h*rows*cols + i*cols + j]->getModel() *= factor;
			}
			buffer.clear();
		}
	}
}

}

#endif /* BOXCARFILTERING_H_ */
