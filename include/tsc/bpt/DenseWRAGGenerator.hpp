/*
 * DenseWRAGGenerator.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 01/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef DENSEWRAGGENERATOR_HPP_
#define DENSEWRAGGENERATOR_HPP_

#include <vector>
#include "../policies/Storage.hpp"
#include "../policies/CheckingPolicy.hpp"
#include <tsc/log/log.h>

namespace tscbpt
{

/**
 * This class takes a chunk of dense data (vector, array, cube, hypercube...),
 * converts is to a set of BPTNodes (the BPT leaves) and generates the
 * corresponding Weighted Region Adjacency Graph (WRAG) by generating the
 * BPTDissimilarities according to the adjacency relations.
 */
template<
	typename StoredType,
	template<class> class 	PtrStoragePolicy 			= DefaultNativeStoragePolicy,
	class CheckingPolicy								= FullCheckingPolicy
>
class DenseWRAGGenerator :
	public PtrStoragePolicy<void* >
{

public:
	typedef StoredType 										Node;
	typedef PtrStoragePolicy<Node>							NodeStoragePol;
	typedef typename NodeStoragePol::StrongPointerType		StrongNodePointer;
	typedef std::vector<StrongNodePointer>					NodePointerVector;
	typedef typename NodePointerVector::iterator			NodePointerVectorIterator;
	typedef NodePointerVectorIterator						iterator;
	typedef size_t											Size;
	typedef signed long										RelPos;
	typedef std::vector<Size>								SizeVector;
	typedef std::vector<RelPos>								RelPosVector;
	typedef CheckingPolicy									CheckPol;

public:

	template<typename InputIteratorFirst, typename InputIteratorLast>
	DenseWRAGGenerator(InputIteratorFirst first, InputIteratorLast last, SizeVector dimensions) :
		_dimensions(dimensions) {
		setTotal();
		_data.reserve(_totalSize);
		for(; first != last; ++first)
			_data.push_back(NodeStoragePol::create(*first));
		Check.sizeMatches(_data.size(), _dimensions);
	}

	template<typename InputIteratorFirst, typename InputIteratorLast>
	DenseWRAGGenerator(InputIteratorFirst first, InputIteratorLast last, Size rows) {
		_dimensions.push_back(rows);
		setTotal();
		_data.reserve(_totalSize);
		for (; first != last; ++first)
			_data.push_back(NodeStoragePol::create(*first));
		Check.sizeMatches(_data.size(), _dimensions);
	}

	template<typename InputIteratorFirst, typename InputIteratorLast>
	DenseWRAGGenerator(InputIteratorFirst first, InputIteratorLast last, Size rows, Size cols) {
		_dimensions.push_back(rows);
		_dimensions.push_back(cols);
		setTotal();
		_data.reserve(_totalSize);
		for (; first != last; ++first)
			_data.push_back(NodeStoragePol::create(*first));
		Check.sizeMatches(_data.size(), _dimensions);
	}

	template<typename InputIteratorFirst, typename InputIteratorLast>
	DenseWRAGGenerator(InputIteratorFirst first, InputIteratorLast last, Size rows, Size cols, Size slices) {
		_dimensions.push_back(slices);
		_dimensions.push_back(rows);
		_dimensions.push_back(cols);
		setTotal();
		_data.reserve(_totalSize);
		for (; first != last; ++first)
			_data.push_back(NodeStoragePol::create(*first));
		Check.sizeMatches(_data.size(), _dimensions);
	}

	inline bool validPosition(Size pos){
		return /*pos >= 0 && */ pos < _totalSize;
	}

	bool validPosition(SizeVector& v){
		bool ret = true;
		for(Size i = 0; i < _dimensions.size() && (ret = ret && v[i] < _dimensions[i]); ++i) {}
		return ret;
	}

	bool validNeighbor(SizeVector& pos, RelPosVector& neighbor) {
		bool ret = true;
		for (Size i = 0; i < _dimensions.size() && ret; ++i) {
			ret = ret && static_cast<RelPos>(pos[i]) + neighbor[i] < static_cast<RelPos>(_dimensions[i]) && static_cast<RelPos>(pos[i]) + neighbor[i] >= 0;
		}
		return ret;
	}

	SizeVector getPosVector(Size pos){
		SizeVector ret(_dimensions.size());
		Size tmpIdx = _totalSize;
		for(Size i = 0; i < _dimensions.size(); ++i){
			tmpIdx /= _dimensions[i];
			ret[i] = (pos / (tmpIdx));
			pos = pos % tmpIdx;
		}
		return ret;
	}

	template<typename Pos>
	Pos getLinearIndex(vector<Pos >& pos){
		Check.dimensionsEqual(pos.size(), _dimensions.size());
		Pos index = pos.back();
		Pos multiplier = 1;
		for(Size i = _dimensions.size() - 1; i > 0; --i){
			multiplier *= _dimensions[i];
			index += pos[i-1] * multiplier;
		}
		return index;
	}

	//FIXME: Implement GENERIC border detection
	template<class DissimilarityMeasure, typename Pos, class BPTDissimilarity>
	void generateWRAG(DissimilarityMeasure d, vector<vector<Pos> >& neighborhood){
		typedef PtrStoragePolicy<BPTDissimilarity>		BPTDissStorage;
		typedef typename BPTDissStorage::strongPointerType	BPTDissPointer;
		_checkNeighborhood(d,neighborhood);
		ProgressDisplay show_progress( _totalSize );
		for(Size i = 0; i < _totalSize; ++i){
			for (typename vector<vector<Pos> >::iterator itn = neighborhood.begin(); itn != neighborhood.end(); ++itn) {
				SizeVector posV = getPosVector(i);
				if(validNeighbor(posV, *itn)){
					Pos pos = i + getLinearIndex(*itn);
					BPTDissPointer diss = BPTDissStorage::create(_data[i], _data[pos], d(_data[i], _data[pos]));
					_data[i]->getDissimilarities().insert(diss);
					if(DissimilarityMeasure::isSymmetric){
						_data[pos]->getDissimilarities().insert(diss);
					}
				}
			}
			++show_progress;
		}
	}

	template <class DissimilarityMeasure, class BPTDissimilarity>
	void generateWRAG_2D_Connectivity8(DissimilarityMeasure d) {
		vector<vector<RelPos> > neighborhood;
		vector<RelPos> pos; pos.reserve(2);
		// Define relative neighborhood 2D
		pos.push_back(-1); pos.push_back(-1); neighborhood.push_back(pos); pos.clear();
		pos.push_back(-1); pos.push_back(0); neighborhood.push_back(pos); pos.clear();
		pos.push_back(-1); pos.push_back(1); neighborhood.push_back(pos); pos.clear();
		pos.push_back(0); pos.push_back(-1); neighborhood.push_back(pos); pos.clear();
		pos.push_back(0); pos.push_back(1); neighborhood.push_back(pos); pos.clear();
		pos.push_back(1); pos.push_back(-1); neighborhood.push_back(pos); pos.clear();
		pos.push_back(1); pos.push_back(0); neighborhood.push_back(pos); pos.clear();
		pos.push_back(1); pos.push_back(1); neighborhood.push_back(pos); pos.clear();
		generateWRAG<DissimilarityMeasure, RelPos, BPTDissimilarity>(d, neighborhood);
	}


	template <class DissimilarityMeasure, class BPTDissimilarity>
	void generateWRAG_3D_Connectivity10(DissimilarityMeasure d) {
		typedef PtrStoragePolicy<BPTDissimilarity> BPTDissStorage;
		typedef typename BPTDissStorage::strongPointerType BPTDissPointer;

		//TODO: Remove common code and reduce function size (relative pos)
		for (Size k = 0; k < _dimensions[0] - 1; ++k) {
			if (DissimilarityMeasure::isSymmetric) {
				// Main loop
				for (Size i = 0; i < _dimensions[1] - 1; ++i) {
					for (Size j = 1; j < _dimensions[2] - 1; ++j) {
						Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + j;
						Size neighborIndex;
						BPTDissPointer diss;
						// Add E neighbor
						neighborIndex = index + 1;
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add SW neighbor
						neighborIndex = index + _dimensions[2] - 1;
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add S neighbor
						neighborIndex = index + _dimensions[2];
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add SE neighbor
						neighborIndex = index + _dimensions[2] + 1;
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add Future neighbor
						neighborIndex = index + _dimensions[1] * _dimensions[2];
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						_data[neighborIndex]->getDissimilarities().insert(diss);
					}
				}

				// Left line
				for (Size i = 0; i < _dimensions[1] - 1; ++i) {
					Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2];
					Size neighborIndex;
					BPTDissPointer diss;
					// Add E neighbor
					neighborIndex = index + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add S neighbor
					neighborIndex = index + _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add SE neighbor
					neighborIndex = index + _dimensions[2] + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add Future neighbor
					neighborIndex = index + _dimensions[1] * _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);
				}

				// Right line
				for (Size i = 0; i < _dimensions[1] - 1; ++i) {
					Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + _dimensions[2] - 1;
					Size neighborIndex;
					BPTDissPointer diss;

					// Add SW neighbor
					neighborIndex = index + _dimensions[2] - 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add S neighbor
					neighborIndex = index + _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add Future neighbor
					neighborIndex = index + _dimensions[1] * _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);
				}

				// Bottom line
				for (Size j = 1; j < _dimensions[2] - 1; ++j) {
					Size index = k * _dimensions[1] * _dimensions[2] + (_dimensions[1] - 1) * _dimensions[2] + j;
					Size neighborIndex;
					BPTDissPointer diss;

					// Add E neighbor
					neighborIndex = index + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add Future neighbor
					neighborIndex = index + _dimensions[1] * _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);
				}

			} else {

				// Main loop
				for (Size i = 0; i < _dimensions[1] - 1; ++i) {
					for (Size j = 1; j < _dimensions[2] - 1; ++j) {
						Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + j;
						Size neighborIndex;
						BPTDissPointer diss;
						// Add E neighbor
						neighborIndex = index + 1;
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
							d(_data[neighborIndex], _data[index]));
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add SW neighbor
						neighborIndex = index + _dimensions[2] - 1;
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
							d(_data[neighborIndex], _data[index]));
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add S neighbor
						neighborIndex = index + _dimensions[2];
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
							d(_data[neighborIndex], _data[index]));
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add SE neighbor
						neighborIndex = index + _dimensions[2] + 1;
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
							d(_data[neighborIndex], _data[index]));
						_data[neighborIndex]->getDissimilarities().insert(diss);

						// Add Future neighbor
						neighborIndex = index + _dimensions[1] * _dimensions[2];
						diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
							d(_data[index], _data[neighborIndex]));
						_data[index]->getDissimilarities().insert(diss);
						_data[neighborIndex]->getDissimilarities().insert(diss);
					}
				}

				// Left line
				for (Size i = 0; i < _dimensions[1] - 1; ++i) {
					Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2];
					Size neighborIndex;
					BPTDissPointer diss;
					// Add E neighbor
					neighborIndex = index + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add S neighbor
					neighborIndex = index + _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add SE neighbor
					neighborIndex = index + _dimensions[2] + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add Future neighbor
					neighborIndex = index + _dimensions[1] * _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);
				}

				// Right line
				for (Size i = 0; i < _dimensions[1] - 1; ++i) {
					Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + _dimensions[2] - 1;
					Size neighborIndex;
					BPTDissPointer diss;

					// Add SW neighbor
					neighborIndex = index + _dimensions[2] - 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add S neighbor
					neighborIndex = index + _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add Future neighbor
					neighborIndex = index + _dimensions[1] * _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

				}

				// Bottom line
				for (Size j = 1; j < _dimensions[2] - 1; ++j) {
					Size index = k * _dimensions[1] * _dimensions[2] + (_dimensions[1] - 1) * _dimensions[2] + j;
					Size neighborIndex;
					BPTDissPointer diss;

					// Add E neighbor
					neighborIndex = index + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add Future neighbor
					neighborIndex = index + _dimensions[1] * _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);
				}
			}
		}
		Size k = _dimensions[0] - 1;
		if (DissimilarityMeasure::isSymmetric) {
			// Main loop
			for (Size i = 0; i < _dimensions[1] - 1; ++i) {
				for (Size j = 1; j < _dimensions[2] - 1; ++j) {
					Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + j;
					Size neighborIndex;
					BPTDissPointer diss;
					// Add E neighbor
					neighborIndex = index + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add SW neighbor
					neighborIndex = index + _dimensions[2] - 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add S neighbor
					neighborIndex = index + _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add SE neighbor
					neighborIndex = index + _dimensions[2] + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					_data[neighborIndex]->getDissimilarities().insert(diss);
				}
			}

			// Left line
			for (Size i = 0; i < _dimensions[1] - 1; ++i) {
				Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2];
				Size neighborIndex;
				BPTDissPointer diss;
				// Add E neighbor
				neighborIndex = index + 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				_data[neighborIndex]->getDissimilarities().insert(diss);

				// Add S neighbor
				neighborIndex = index + _dimensions[2];
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				_data[neighborIndex]->getDissimilarities().insert(diss);

				// Add SE neighbor
				neighborIndex = index + _dimensions[2] + 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				_data[neighborIndex]->getDissimilarities().insert(diss);
			}

			// Right line
			for (Size i = 0; i < _dimensions[1] - 1; ++i) {
				Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + _dimensions[2] - 1;
				Size neighborIndex;
				BPTDissPointer diss;

				// Add SW neighbor
				neighborIndex = index + _dimensions[2] - 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				_data[neighborIndex]->getDissimilarities().insert(diss);

				// Add S neighbor
				neighborIndex = index + _dimensions[2];
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				_data[neighborIndex]->getDissimilarities().insert(diss);
			}

			// Bottom line
			for (Size j = 1; j < _dimensions[2] - 1; ++j) {
				Size index = k * _dimensions[1] * _dimensions[2] + (_dimensions[1] - 1) * _dimensions[2] + j;
				Size neighborIndex;
				BPTDissPointer diss;

				// Add E neighbor
				neighborIndex = index + 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				_data[neighborIndex]->getDissimilarities().insert(diss);
			}

		} else {

			// Main loop
			for (Size i = 0; i < _dimensions[1] - 1; ++i) {
				for (Size j = 1; j < _dimensions[2] - 1; ++j) {
					Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + j;
					Size neighborIndex;
					BPTDissPointer diss;
					// Add E neighbor
					neighborIndex = index + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add SW neighbor
					neighborIndex = index + _dimensions[2] - 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add S neighbor
					neighborIndex = index + _dimensions[2];
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);

					// Add SE neighbor
					neighborIndex = index + _dimensions[2] + 1;
					diss = BPTDissStorage::create(_data[index], _data[neighborIndex],
						d(_data[index], _data[neighborIndex]));
					_data[index]->getDissimilarities().insert(diss);
					diss = BPTDissStorage::create(_data[neighborIndex], _data[index],
						d(_data[neighborIndex], _data[index]));
					_data[neighborIndex]->getDissimilarities().insert(diss);
				}
			}

			// Left line
			for (Size i = 0; i < _dimensions[1] - 1; ++i) {
				Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2];
				Size neighborIndex;
				BPTDissPointer diss;
				// Add E neighbor
				neighborIndex = index + 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				diss
					= BPTDissStorage::create(_data[neighborIndex], _data[index], d(_data[neighborIndex], _data[index]));
				_data[neighborIndex]->getDissimilarities().insert(diss);

				// Add S neighbor
				neighborIndex = index + _dimensions[2];
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				diss
					= BPTDissStorage::create(_data[neighborIndex], _data[index], d(_data[neighborIndex], _data[index]));
				_data[neighborIndex]->getDissimilarities().insert(diss);

				// Add SE neighbor
				neighborIndex = index + _dimensions[2] + 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				diss
					= BPTDissStorage::create(_data[neighborIndex], _data[index], d(_data[neighborIndex], _data[index]));
				_data[neighborIndex]->getDissimilarities().insert(diss);
			}

			// Right line
			for (Size i = 0; i < _dimensions[1] - 1; ++i) {
				Size index = k * _dimensions[1] * _dimensions[2] + i * _dimensions[2] + _dimensions[2] - 1;
				Size neighborIndex;
				BPTDissPointer diss;

				// Add SW neighbor
				neighborIndex = index + _dimensions[2] - 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				diss
					= BPTDissStorage::create(_data[neighborIndex], _data[index], d(_data[neighborIndex], _data[index]));
				_data[neighborIndex]->getDissimilarities().insert(diss);

				// Add S neighbor
				neighborIndex = index + _dimensions[2];
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				diss
					= BPTDissStorage::create(_data[neighborIndex], _data[index], d(_data[neighborIndex], _data[index]));
				_data[neighborIndex]->getDissimilarities().insert(diss);
			}

			// Bottom line
			for (Size j = 1; j < _dimensions[2] - 1; ++j) {
				Size index = k * _dimensions[1] * _dimensions[2] + (_dimensions[1] - 1) * _dimensions[2] + j;
				Size neighborIndex;
				BPTDissPointer diss;

				// Add E neighbor
				neighborIndex = index + 1;
				diss
					= BPTDissStorage::create(_data[index], _data[neighborIndex], d(_data[index], _data[neighborIndex]));
				_data[index]->getDissimilarities().insert(diss);
				diss
					= BPTDissStorage::create(_data[neighborIndex], _data[index], d(_data[neighborIndex], _data[index]));
				_data[neighborIndex]->getDissimilarities().insert(diss);
			}
		}
	}

	inline NodePointerVectorIterator begin(){
		return _data.begin();
	}

	inline NodePointerVectorIterator end() {
		return _data.end();
	}

	const SizeVector& getDimensions() const{
		return _dimensions;
	}

	NodePointerVector& getData(){
		return _data;
	}

	const NodePointerVector& getData() const {
		return _data;
	}


protected:
	SizeVector				_dimensions;
	NodePointerVector		_data;
	Size					_totalSize;
	static CheckPol			Check;

	void setTotal(){
		_totalSize = _dimensions.at(0);
		for (Size i = 1; i < _dimensions.size(); i++)
			_totalSize *= _dimensions[i];
	}

	template<class DissimilarityMeasure, typename Pos>
	void _checkNeighborhood(DissimilarityMeasure, vector<vector<Pos> >& neighborhood) {
		typedef vector<vector<Pos> > 		TNeighborhood;
		if(neighborhood.size() > 0){
			Check.dimensionsEqual(neighborhood[0].size(), _dimensions.size());
			/**
			 * If dissimilarity is symmetric keep only forward dissimilarities
			 */
			if(DissimilarityMeasure::isSymmetric){
				typename TNeighborhood::iterator it = neighborhood.begin();
				while(it != neighborhood.end()){
					if(getLinearIndex(*it) <= 0){
						neighborhood.erase(it);
					}else{
						++it;
					}
				}
			}
		}
	}

	inline Size _getLinearIndex_NoCheck(vector<Size>& pos) {
		Size index = pos[0];
		Size multiplier = 1;
		for (int i = 1; i < _dimensions.size(); ++i) {
			multiplier *= _dimensions[i - 1];
			index += pos[i] * multiplier;
		}
		return index;
	}
};

template<
	typename StoredType,
	template<class> class PtrStoragePolicy,
	class CheckingPolicy
>
CheckingPolicy DenseWRAGGenerator<StoredType, PtrStoragePolicy, CheckingPolicy>::Check = CheckingPolicy();

}

#endif /* DENSEWRAGGENERATOR_HPP_ */
