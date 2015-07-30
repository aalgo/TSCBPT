/*
 * NativeMatrixModel.hpp
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

#ifndef NATIVEMATRIXMODEL_HPP_
#define NATIVEMATRIXMODEL_HPP_

#include <vector>
#include <complex>
#include <armadillo>
#include "RegionModel.hpp"
#include <tsc/image/Pixel.hpp>
#include <tsc/data/matrix/GetMinDimSize.hpp>

namespace tscbpt
{

using namespace std;

template<class Derived, typename MatrixElem>
class VectorialModel_Base{
public:
	MatrixElem const * getData() const {
		return static_cast<const Derived*>(this)->getData();
	}
	size_t getMatrixSize() const {
		return static_cast<const Derived*>(this)->getMatrixSize();
	}
protected:
	static inline void merge(const VectorialModel_Base<Derived, MatrixElem>& a, const VectorialModel_Base<Derived, MatrixElem>& b,
		VectorialModel_Base<Derived, MatrixElem>& out) {
		Derived::merge(a, b, out);
	}
};

// TODO: Move this definition to a better place
template<class Derived, typename MatrixElem>
MatrixElem norm2(const VectorialModel_Base<Derived, MatrixElem>& a){
	MatrixElem tmp();
	MatrixElem const *it = a.getData();
	for(size_t i = 0; i < a.getMatrixSize()*a.getMatrixSize(); ++i){
		tmp += (*it) * (*it);
		++it;
	}
	return tmp;
}

// TODO: Move this definition to a better place
template<class Derived, typename ComplexElem>
ComplexElem norm2(const VectorialModel_Base<Derived, std::complex<ComplexElem> >& a){
	ComplexElem tmp = ComplexElem();
	std::complex<ComplexElem> const *it = a.getData();
	for(size_t i = 0; i < a.getMatrixSize()*a.getMatrixSize(); ++i){
		tmp += norm(*it);
		++it;
	}
	return tmp;
}

// TODO: Move this definition to a better place
template<class Derived, typename ComplexElem>
ComplexElem dist2(const VectorialModel_Base<Derived, std::complex<ComplexElem> >& a, const VectorialModel_Base<Derived, std::complex<ComplexElem> >& b){
	ComplexElem tmp = ComplexElem();
	if(a.getMatrixSize() != b.getMatrixSize())
		__throw_invalid_argument("Called dist2 with 2 different size matrices");
	std::complex<ComplexElem> const *it1 = a.getData();
	std::complex<ComplexElem> const *it2 = b.getData();
	for(size_t i = 0; i < a.getMatrixSize()*a.getMatrixSize(); ++i){
		tmp += norm(*it1 - *it2);
		++it1;
		++it2;
	}
	return tmp;
}

template<
	typename MatrixElem = std::complex<double>,
	size_t MatrixSize = 3,
	size_t PosDims = 2,
	typename PosTElem = float >
class NativeMatrixModel :
	public RegionModelWithSubnodesAndPosition<PosDims, PosTElem>,
	public VectorialModel_Base<NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem>, MatrixElem>
{
public:
	typedef NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem> 	ThisType;

	template <typename DataType>
	NativeMatrixModel(Pixel<DataType, PosDims, PosTElem> p) :
		RegionModelWithSubnodesAndPosition<PosDims, PosTElem> (p) {
		initialize(p.getValue());
	}

	template<typename DataParam, typename ParamT1>
	NativeMatrixModel(DataParam data, ParamT1 x) :
		RegionModelWithSubnodesAndPosition<PosDims, PosTElem> (x) {
		initialize(data);
	}

	template<typename DataParam, typename ParamT1, typename ParamT2>
	NativeMatrixModel(DataParam data, ParamT1 p1, ParamT2 p2) :
		RegionModelWithSubnodesAndPosition<PosDims, PosTElem> (p1, p2) {
		initialize(data);
	}

	template <typename DataParam, typename ParamT1, typename ParamT2, typename ParamT3>
	NativeMatrixModel(DataParam data, ParamT1 p1, ParamT2 p2, ParamT3 p3) :
		RegionModelWithSubnodesAndPosition<PosDims, PosTElem> (p1, p2, p3) {
		initialize(data);
	}

	template <typename DataParam, typename ParamT1, typename ParamT2, typename ParamT3, typename ParamT4>
	NativeMatrixModel(DataParam data, ParamT1 p1, ParamT2 p2, ParamT3 p3, ParamT4 p4) :
		RegionModelWithSubnodesAndPosition<PosDims, PosTElem> (p1, p2, p3, p4) {
		initialize(data);
	}

	NativeMatrixModel(const ThisType& rhs) :
		RegionModelWithSubnodesAndPosition<PosDims, PosTElem> (static_cast<RegionModelWithSubnodesAndPosition<PosDims, PosTElem> >(rhs)) {
		initialize(rhs._data);
	}

//	//TODO: Implement construction with iterator (using it.getRow(), etc...). Maybe through a specialization class
//	template <typename Iterator>
//	NativeMatrixModel(Iterator it);

	MatrixElem * getData() {
		return _data;
	}

	MatrixElem const * getData() const {
		return _data;
	}

	void setData(MatrixElem * data){
		for (size_t i = 0; i < getMatrixSize() * getMatrixSize(); ++i) {
			_data[i] = data[i];
		}
	}

	inline size_t getMatrixSize() const {
		return MatrixSize;
	}

	MatrixElem& getElem(size_t row, size_t col) {
		// AAG 20111007: armadillo stores data column by column
		return _data[col*getMatrixSize() + row];
	}

	const MatrixElem& getElem(size_t row, size_t col) const {
		// AAG 20111007: armadillo stores data column by column
		return _data[col * getMatrixSize() + row];
	}

	void setElem(size_t row, size_t col, const MatrixElem& elem){
		// AAG 20111007: armadillo stores data column by column
		_data[col*getMatrixSize() + row] = elem;
	}

	MatrixElem& operator()(size_t row, size_t col){
		// AAG 20111007: armadillo stores data column by column
		return _data[col*getMatrixSize() + row];
	}

	const MatrixElem& operator()(size_t row, size_t col) const{
		return _data[col*getMatrixSize() + row];
	}

	ThisType merge(const ThisType& b){
		ThisType tmp;
		tmp.subnodes = this->subnodes + b.subnodes;
		for(size_t i = 0; i< getMatrixSize() * getMatrixSize(); i++){
			tmp._data[i] = (_data[i] * MatrixElem(this->subnodes) + b._data[i] * MatrixElem(b.subnodes)) / MatrixElem(tmp.subnodes);
		}
		for (size_t i = 0; i < PosDims; i++) {
			tmp._pos[i] = (this->_pos[i]*this->subnodes + b._pos[i]*b.subnodes) / tmp.subnodes;
		}

		return tmp;
	}

	ThisType unmerge(const ThisType& b){
		ThisType tmp;
		tmp.subnodes = this->subnodes - b.subnodes;
		for(size_t i = 0; i< getMatrixSize() * getMatrixSize(); i++){
			tmp._data[i] = (_data[i] * MatrixElem(this->subnodes) - b._data[i] * MatrixElem(b.subnodes)) / MatrixElem(tmp.subnodes);
		}
		for (size_t i = 0; i < PosDims; i++) {
			tmp._pos[i] = (this->_pos[i]*this->subnodes - b._pos[i]*b.subnodes) / tmp.subnodes;
		}
		return tmp;
	}

	ThisType& operator+=(const ThisType& b){
		for(size_t i = 0; i < getMatrixSize() * getMatrixSize(); ++i){
			_data[i] += b._data[i];
		}
		return *this;
	}

	template <typename T>
	ThisType& operator*=(const T b) {
		for (size_t i = 0; i < getMatrixSize() * getMatrixSize(); ++i) {
			_data[i] *= b;
		}
		return *this;
	}

	template <typename T>
	ThisType& operator/=(const T b) {
		for (size_t i = 0; i < getMatrixSize() * getMatrixSize(); ++i) {
			_data[i] /= b;
		}
		return *this;
	}

private:
	NativeMatrixModel(){}
protected:
	MatrixElem _data[MatrixSize * MatrixSize];

	void initialize(MatrixElem const * data){
		for (size_t i = 0; i < getMatrixSize() * getMatrixSize(); ++i) {
			_data[i] = data[i];
		}
	}

	void initialize(const ThisType& b) {
		for (size_t i = 0; i < getMatrixSize() * getMatrixSize(); ++i) {
			_data[i] = b._data[i];
		}
	}

	// TODO: try to remove dependency with armadillo
	void initialize(const arma::cx_mat& b) {
		for (size_t i = 0; i < getMatrixSize(); ++i) {
			for (size_t j = 0; j < getMatrixSize(); ++j) {
				_data[j * getMatrixSize() + i] = b(i,j);
			}
		}
	}

	template<typename Elem>
	void initialize(const std::vector<Elem>& data) {
		for (size_t i = 0; i < getMatrixSize(); ++i) {
			for (size_t j = 0; j < getMatrixSize(); ++j) {
				// AAG 20111007: armadillo stores data column by column
				_data[j * getMatrixSize() + i] = data[i] * conj(data[j]);
			}
		}
	}
};

template<
	typename MatrixElem = std::complex<double>,
	size_t MatrixSize = 3,
	size_t PosDims = 2,
	typename PosTElem = float >
class NativeMatrixModel_FromIt :
	public NativeMatrixModel<MatrixElem, MatrixSize, PosDims, PosTElem>{
public:
	template<typename Iterator>
	NativeMatrixModel_FromIt(Iterator it);
};

template<
	typename MatrixElem,
	size_t MatrixSize,
	typename PosTElem>
class NativeMatrixModel_FromIt<MatrixElem, MatrixSize, 1, PosTElem> :
	public NativeMatrixModel<MatrixElem, MatrixSize, 1, PosTElem>{
public:
	template<typename Iterator>
	NativeMatrixModel_FromIt(Iterator it):
		NativeMatrixModel<MatrixElem, MatrixSize, 1, PosTElem> (*it, it.getRow()) {
	}
};

template<
	typename MatrixElem,
	size_t MatrixSize,
	typename PosTElem>
class NativeMatrixModel_FromIt<MatrixElem, MatrixSize, 2, PosTElem> :
	public NativeMatrixModel<MatrixElem, MatrixSize, 2, PosTElem>{
public:
	template<typename Iterator>
	NativeMatrixModel_FromIt(Iterator it):
		NativeMatrixModel<MatrixElem, MatrixSize, 2, PosTElem> (*it, it.getRow(), it.getCol()) {
	}
};

template<
	typename MatrixElem,
	size_t MatrixSize,
	typename PosTElem>
class NativeMatrixModel_FromIt<MatrixElem, MatrixSize, 3, PosTElem> :
	public NativeMatrixModel<MatrixElem, MatrixSize, 3, PosTElem>{
public:
	template<typename Iterator>
	NativeMatrixModel_FromIt(Iterator it):
		NativeMatrixModel<MatrixElem, MatrixSize, 3, PosTElem> (*it, it.getRow(), it.getCol(), it.getSlice()) {
	}
};

}


#endif /* NATIVEMATRIXMODEL_HPP_ */
