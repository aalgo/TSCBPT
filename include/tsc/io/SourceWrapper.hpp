/*
 * SourceWrapper.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 12/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SOURCEWRAPPER_HPP_
#define SOURCEWRAPPER_HPP_

namespace tscbpt
{

template <
	class TSource,
	typename UnaryOperator,
	typename OutType = typename UnaryOperator::result_type>
class SourceWrapper
{
public:
	typedef TSource 						SourceType;
	typedef typename SourceType::iterator	InnerIterator;

	SourceWrapper(SourceType& asource) : source(asource){
		op = UnaryOperator();
	}
	SourceWrapper(SourceType& asource, UnaryOperator aop) : source(asource), op(aop){}

	class iterator{
	public:
		typedef iterator		ThisType;
		typedef iterator&		ThisReference;
		typedef OutType			value_type;


		OutType operator*(){
			return op(*it);
		}

		ThisType operator++(int){
			ThisType tmp(*this);
			++*this;
			return tmp;
		}

		ThisReference operator++(){
			++it;
			return *this;
		}

		bool operator==(const ThisType& b) const {
			return this->it == b.it;
		}

		bool operator!=(const ThisType& b) const {
			return this->it != b.it;
		}

		size_t getRow() const {
			return this->it.getRow();
		}

		size_t getCol() const {
			return this->it.getCol();
		}

		friend class SourceWrapper;

	protected:
		iterator(InnerIterator ait, UnaryOperator aop) : it(ait), op(aop) {}
		InnerIterator it;
		UnaryOperator op;
	};

	size_t getRows() const {
		return source.getRows();
	}

	size_t getCols() const {
		return source.getCols();
	}

	iterator begin(){
		return iterator(source.begin(), op);
	}

	iterator end(){
		return iterator(source.end(), op);
	}

protected:
	SourceType& source;
	UnaryOperator op;
};

}

#endif /* SOURCEWRAPPER_HPP_ */
