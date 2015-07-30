/*
 * AddHomogeneity.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 21/11/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef ADDHOMOGENEITY_HPP_
#define ADDHOMOGENEITY_HPP_


// *************** Definitions ******************

namespace tscbpt
{
/**
 * Decorator to hold the total sum of squared error of the wrapped model.
 * It is based on the dist2() generic function.
 */
template<class Base, typename HomogType>
class AddHomogeneity;
}



// ************* Implementation *****************

#include <functional>
#include <tsc/util/types/WrapperOf.hpp>

namespace tscbpt
{

template<
	class Base,
	typename HomogType		= double>
class AddHomogeneity:
		public Base,
		public WrapperOf<Base>		// Mark as a Wrapper of Base class
{
public:
	typedef HomogType							homog_type;
	typedef AddHomogeneity<Base, homog_type>	this_type;
	typedef typename Base::data_type			data_type;

	AddHomogeneity(): Base(), _totSumSquares() {}

	AddHomogeneity(const this_type& b): Base(b), _totSumSquares(b._totSumSquares) {	}

	template<typename Param>
	AddHomogeneity(Param p): Base(p), _totSumSquares() {}

	homog_type getHomogeneity() const __attribute__ ((deprecated)) {
		return _totSumSquares;
	}

	homog_type getTotalSumOfSquares() const {
		return _totSumSquares;
	}

	this_type merge(const this_type& b) const {
		this_type tmp(Base::merge(b));
		// Classical homogeneity
		tmp._totSumSquares = this->_totSumSquares + b._totSumSquares +
			dist2(static_cast<const Base&>(*this), static_cast<const Base&>(b)) *
			this->getSubnodes() * b.getSubnodes() / (this->getSubnodes() + b.getSubnodes());
		return tmp;
	}

	this_type merge(this_type& b) {
		this_type tmp(Base::merge(b));
		// Classical homogeneity
		tmp._totSumSquares = this->_totSumSquares + b._totSumSquares +
			dist2(static_cast<const Base&>(*this), static_cast<const Base&>(b)) *
			this->getSubnodes() * b.getSubnodes() / (this->getSubnodes() + b.getSubnodes());
		return tmp;
	}

	this_type unmerge(const this_type& b) const {
		Base superTmp = Base::unmerge(b);
		this_type tmp(superTmp);
		// Classical homogeneity
		tmp._totSumSquares = this->_totSumSquares - b._totSumSquares + (dist2(static_cast<const Base&>(tmp), static_cast<const Base&>(b))) * tmp.getSubnodes() * b.getSubnodes()
			/ (tmp.getSubnodes() + b.getSubnodes());
		return tmp;
	}

	this_type unmerge(this_type& b) {
		Base superTmp = Base::unmerge(b);
		this_type tmp(superTmp);
		// Classical homogeneity
		tmp._totSumSquares = this->_totSumSquares - b._totSumSquares + (dist2(static_cast<const Base&>(tmp), static_cast<const Base&>(b))) * tmp.getSubnodes() * b.getSubnodes()
			/ (tmp.getSubnodes() + b.getSubnodes());
		return tmp;
	}

	homog_type jointHomogeneity(const this_type& b) const __attribute__ ((deprecated)) {
		return jointSumOfSquares(b);
	}

	homog_type jointSumOfSquares(const this_type& b) const {
		return this->_totSumSquares + b._totSumSquares +
			dist2(static_cast<const Base&>(*this), static_cast<const Base&>(b)) *
			this->getSubnodes() * b.getSubnodes() / (this->getSubnodes() + b.getSubnodes());
	}

	homog_type jointHomogeneity(this_type& b) __attribute__ ((deprecated)) {
		return jointSumOfSquares(b);
	}

	homog_type jointSumOfSquares(this_type& b) {
		return this->_totSumSquares + b._totSumSquares +
			dist2(static_cast<const Base&>(*this), static_cast<const Base&>(b)) *
			this->getSubnodes() * b.getSubnodes() / (this->getSubnodes() + b.getSubnodes());
	}

	void initializeToZero(){
		_totSumSquares = homog_type();
		Base::initializeToZero();
	}

private:
	homog_type	_totSumSquares;
};

}

#endif /* ADDHOMOGENEITY_HPP_ */
