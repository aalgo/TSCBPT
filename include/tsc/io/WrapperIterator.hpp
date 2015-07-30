/*
 * WrapperIterator.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 12/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef WRAPPERITERATOR_HPP_
#define WRAPPERITERATOR_HPP_

#include <iterator>
#include <typeinfo>


namespace tscbpt {

using namespace std;

template<typename InputIterator, typename OutType, typename UnaryOperator>
class WrapperIterator : public iterator<input_iterator_tag, OutType> {
private:
	InputIterator _it;
	OutType _current;
	UnaryOperator _functor;
public:

	typedef InputIterator input_iterator_type;
	typedef UnaryOperator operator_type;
	typedef OutType out_type;
	typedef OutType value_type;
	typedef WrapperIterator<InputIterator,OutType,UnaryOperator> 	this_value_type;
	typedef WrapperIterator<InputIterator,OutType,UnaryOperator>* 	this_pointer_type;
	typedef WrapperIterator<InputIterator,OutType,UnaryOperator>& 	this_reference_type;
	typedef typename iterator_traits<this_value_type>::difference_type
																	difference_type;
	typedef OutType&												reference;
	typedef OutType*												pointer;

	WrapperIterator(InputIterator it, UnaryOperator functor):
		_it(it), _functor(functor) {
		_current = _functor(*_it);
	}

	this_value_type operator ++(int) {
		this_value_type tmp(*this);
		++_it;
		_current = _functor(*_it);
		return tmp;
	}

	this_reference_type operator ++() {
		++_it;
		_current = _functor(*_it);
		return *this;
	}

	bool operator<(const this_reference_type b) const{
		return this->_it < b._it;
	}
	bool operator<(const InputIterator& b) const{
		return this->_it < b;
	}

	bool operator==(const this_reference_type b) const{
		return this->_it == b._it;
	}
	bool operator==(const InputIterator& b) const{
		return this->_it == b;
	}

	bool operator!=(const this_reference_type b) const{
		return this->_it != b._it;
	}
	bool operator!=(const InputIterator& b) const{
		return this->_it != b;
	}

	reference operator *(){
		return _current;
	}

//	const reference operator *() const{
//		return _current;
//	}

};

template<typename InnerInputIterator, typename OutType, typename InnerOutType, typename UnaryOperator, typename InnerUnaryOperator>
class WrapperIterator<WrapperIterator<InnerInputIterator, InnerOutType, InnerUnaryOperator>, OutType, UnaryOperator > : public iterator<input_iterator_tag, OutType> {

public:
	typedef WrapperIterator<InnerInputIterator, InnerOutType, InnerUnaryOperator> input_iterator_type;
	typedef InnerInputIterator	inner_input_iterator_type;
	typedef UnaryOperator operator_type;
	typedef OutType out_type;
	typedef WrapperIterator<input_iterator_type, OutType, UnaryOperator> this_value_type;
	typedef WrapperIterator<input_iterator_type, OutType, UnaryOperator>* this_pointer_type;
	typedef WrapperIterator<input_iterator_type, OutType, UnaryOperator>& this_reference_type;
	typedef typename iterator_traits<this_value_type>::difference_type difference_type;
	typedef typename iterator_traits<this_value_type>::reference reference;
	typedef typename iterator_traits<this_value_type>::pointer pointer;

private:
	input_iterator_type _it;
	OutType _current;
	UnaryOperator _functor;
public:

	WrapperIterator(input_iterator_type it, UnaryOperator functor):
		_it(it), _current(functor(*_it)), _functor(functor) {
	}

	this_value_type operator ++(int) {
		this_value_type tmp(*this);
		_it++;
		_current = _functor(*_it);
		return tmp;
	}

	this_reference_type operator ++() {
		++_it;
		_current = _functor(*_it);
		return *this;
	}

	bool operator<(const this_reference_type b) const{
		return this->_it < b._it;
	}
	bool operator<(const input_iterator_type& b) const{
		return this->_it < b;
	}
	bool operator<(const inner_input_iterator_type& b) const {
		return this->_it < b;
	}

	bool operator==(const this_reference_type b) const{
		return this->_it == b._it;
	}
	bool operator==(const input_iterator_type& b) const{
		return this->_it == b;
	}
	bool operator==(const inner_input_iterator_type& b) const {
		return this->_it == b;
	}

	template<typename Iterator>
	bool operator!=(const Iterator b) const{
		return this->_it != b;
	}

	reference operator *(){
		return _current;
	}

//	const reference operator *() const{
//		return _current;
//	}

};

}

#endif /* WRAPPERITERATOR_HPP_ */
