/*
 * RegionModel.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 30/03/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef REGIONMODEL_HPP_
#define REGIONMODEL_HPP_


//#include <boost/static_assert.hpp>
#include <stddef.h>
#include <tsc/policies/Storage.hpp>
#include <tsc/data/Position.hpp>

namespace tscbpt
{

class RegionModel
{
protected:
	static inline void merge(const RegionModel&, const RegionModel&, RegionModel&) {}
};

template<class Derived>
class ModelWithSubnodesBase : public RegionModel{
public:

	size_t getSubnodes() const {
		return static_cast<Derived*>(this)->getSubnodes();
	}

protected:
	template<class T1, class T2>
	static inline void merge(const ModelWithSubnodesBase<T1>& a, const ModelWithSubnodesBase<T2>& b, ModelWithSubnodesBase<Derived>& out) {
		RegionModel::merge(a, b, out);
		Derived::merge(a, b, out);
	}
};

class RegionModelWithSubnodes : public ModelWithSubnodesBase<RegionModelWithSubnodes>{
public:
	RegionModelWithSubnodes() : subnodes(1){}

	RegionModelWithSubnodes(size_t nsubnodes) : subnodes(nsubnodes){}

	size_t getSubnodes() const {
		return subnodes;
	}
	void setSubnodes(size_t subnodes){
		this->subnodes = subnodes;
	}

protected:
	size_t subnodes;
	static inline void merge(const RegionModelWithSubnodes& a, const RegionModelWithSubnodes& b, RegionModelWithSubnodes& out) {
		out.subnodes = a.subnodes + b.subnodes;
	}
};

template <
	class Derived,
	template<class> class 	StoragePolicy 			= DefaultNativeStoragePolicy
	>
class ModelWithSubnodesDecorator: public ModelWithSubnodesBase<Derived>{
public:
	typedef ModelWithSubnodesDecorator<Derived, StoragePolicy>		ThisType;
	typedef StoragePolicy<RegionModelWithSubnodes>					DefaultStoragePol;
	typedef StoragePolicy<ModelWithSubnodesBase<Derived> >			BaseStoragePol;
	typedef	typename BaseStoragePol::StrongPointerType				DecoratedPointer;

	ModelWithSubnodesDecorator() : decoratedModel(DefaultStoragePol::create()) {}
	ModelWithSubnodesDecorator(ModelWithSubnodesBase<Derived>*	aModel) : decoratedModel(aModel) {}
protected:
	DecoratedPointer	decoratedModel;
	~ModelWithSubnodesDecorator(){
		BaseStoragePol::remove(decoratedModel);
	}
};


template<size_t Dims = 2, typename TElem = float>
class RegionModelWithSubnodesAndPosition :
	public RegionModelWithSubnodes,
	public Position<Dims, TElem>{

public:

	typedef TElem					CoordinateType;
	typedef Position<Dims, TElem>	PositionType;
	static const size_t		dims 			= Dims;

	RegionModelWithSubnodesAndPosition(const PositionType& pos) :
		PositionType(pos) {
	}
	RegionModelWithSubnodesAndPosition(const PositionType& pos, size_t subnodes) :
		RegionModelWithSubnodes(subnodes), PositionType(pos) {
	}
	RegionModelWithSubnodesAndPosition(TElem x):
		PositionType(x){
	}
	RegionModelWithSubnodesAndPosition(TElem x, size_t subnodes):
		RegionModelWithSubnodes(subnodes), PositionType(x) {
	}
	RegionModelWithSubnodesAndPosition(TElem x, TElem y):
		PositionType(x, y) {
	}
	RegionModelWithSubnodesAndPosition(TElem x, TElem y, size_t subnodes):
		RegionModelWithSubnodes(subnodes), PositionType(x, y) {
	}
	RegionModelWithSubnodesAndPosition(TElem x, TElem y, TElem z):
		PositionType(x, y ,z){
	}

	RegionModelWithSubnodesAndPosition(TElem x, TElem y, TElem z, size_t subnodes):
		RegionModelWithSubnodes(subnodes), PositionType(x, y, z) {
	}

	inline size_t getNumberOfDims() const {
		return Dims;
	}

	RegionModelWithSubnodesAndPosition<Dims, TElem> merge(const RegionModelWithSubnodesAndPosition<Dims, TElem>& b) const {
		RegionModelWithSubnodesAndPosition<Dims, TElem> res;
		res.subnodes = this->subnodes + b.subnodes;
		for(size_t i = 0; i < Dims; i++){
			res._pos[i] = (static_cast<TElem>(this->subnodes) * this->_pos[i] +
				static_cast<TElem>(b.subnodes) * b._pos[i]) /
				res.subnodes;
		}
		return res;
	}

	RegionModelWithSubnodesAndPosition<Dims, TElem>& merge(const RegionModelWithSubnodesAndPosition<Dims, TElem>& b,
		RegionModelWithSubnodesAndPosition<Dims, TElem>& res) const {

		res.subnodes = this->subnodes + b.subnodes;
		for(size_t i = 0; i < Dims; i++){
			res._pos[i] = (static_cast<TElem>(this->subnodes) * this->_pos[i] +
				static_cast<TElem>(b.subnodes) * b._pos[i]) /
				res.subnodes;
		}
		return res;
	}

	static inline void merge(const RegionModelWithSubnodesAndPosition<Dims, TElem>& a,
		const RegionModelWithSubnodesAndPosition<Dims, TElem>& b, RegionModelWithSubnodesAndPosition<Dims, TElem>& out) {
		RegionModelWithSubnodes::merge(a, b, out);
		for (size_t i = 0; i < Dims; i++) {
			out._pos[i] = (a._pos[i] * a.subnodes + b._pos[i] * b.subnodes) / out.subnodes;
		}
	}

protected:
	RegionModelWithSubnodesAndPosition(){}
};

}

#endif /* REGIONMODEL_HPP_ */
