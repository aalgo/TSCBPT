/*
 * AddLogDetAverage.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 19/11/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef ADDLOGDETAVERAGE_HPP_
#define ADDLOGDETAVERAGE_HPP_


// *************** Definitions ******************
namespace tscbpt
{
/**
 * Decorator to hold the average of the log determinants of the model.
 * Refer to concepts/HasLogDetAverage.h for concept definition.
 */
template<class Base,typename HomogType> class AddLogDetAverage;
}


// ************* Implementation *****************

#include <functional>
#include <tsc/util/types/WrapperOf.hpp>

namespace tscbpt
{

template<
	class Base,
	typename HomogType		= double>
class AddLogDetAverage:
		public Base,
		public WrapperOf<Base>		// Mark as a Wrapper of Base class
{
public:
	typedef HomogType							homog_type;
	typedef HomogType							logdetavg_type;
	typedef AddLogDetAverage<Base, homog_type>	this_type;
	typedef typename Base::data_type			data_type;

	AddLogDetAverage(): Base(), _logDetAvg() {}

	AddLogDetAverage(const this_type& b): Base(b), _logDetAvg(b._logDetAvg) {	}

	template<typename Param>
	AddLogDetAverage(Param p): Base(p) {
		_logDetAvg = abs(log_det(static_cast<const Base&>(*this)));
	}

	homog_type getLogDetAverage() const {
		return _logDetAvg;
	}

	homog_type getHomogeneity() const __attribute__ ((deprecated)) {
		const size_t subMatrixSize = this->getCovariance(0).getCols();
		const size_t subnodes = this->getSubnodes();
		return subnodes * (_logDetAvg - abs(log_det(static_cast<const Base&>(*this))) + subMatrixSize * log(subnodes));
	}

	this_type merge(const this_type& b) const {
		this_type tmp(Base::merge(b));
		tmp._logDetAvg = (this->_logDetAvg * this->getSubnodes() + b._logDetAvg * b.getSubnodes())
			/ (this->getSubnodes() + b.getSubnodes());
		return tmp;
	}

	this_type merge(this_type& b) {
		this_type tmp(Base::merge(b));
		tmp._logDetAvg = (this->_logDetAvg * this->getSubnodes() + b._logDetAvg * b.getSubnodes())
			/ (this->getSubnodes() + b.getSubnodes());
		return tmp;
	}

	this_type unmerge(const this_type& b) const {
		Base superTmp = Base::unmerge(b);
		this_type tmp(superTmp);
		tmp._logDetAvg = (this->_logDetAvg * this->getSubnodes() - b._logDetAvg * b.getSubnodes())
				/ (this->getSubnodes() - b.getSubnodes());
		return tmp;
	}

	this_type unmerge(this_type& b) {
		Base superTmp = Base::unmerge(b);
		this_type tmp(superTmp);
		tmp._logDetAvg = (this->_logDetAvg * this->getSubnodes() - b._logDetAvg * b.getSubnodes())
				/ (this->getSubnodes() - b.getSubnodes());
		return tmp;
	}

	homog_type jointLogDetAverage(const this_type& b) const {
		return (this->_logDetAvg * this->getSubnodes() + b._logDetAvg * b.getSubnodes())
				/ (this->getSubnodes() + b.getSubnodes());
	}

	homog_type jointLogDetAverage(this_type& b){
		return (this->_logDetAvg * this->getSubnodes() + b._logDetAvg * b.getSubnodes())
				/ (this->getSubnodes() + b.getSubnodes());
	}

	void initializeToZero(){
		_logDetAvg = homog_type();
		Base::initializeToZero();
	}

private:
	homog_type	_logDetAvg;
};

}

#endif /* ADDLOGDETAVERAGE_HPP_ */
