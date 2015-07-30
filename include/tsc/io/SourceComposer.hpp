/*
 * SourceComposer.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 13/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SOURCECOMPOSER_HPP_
#define SOURCECOMPOSER_HPP_

#include <vector>
#include <string>

namespace tscbpt
{

template<class TSource>
class SourceComposer
{
public:
	typedef TSource													SourceType;
	typedef typename SourceType::iterator							SourceIterator;
	typedef typename SourceIterator::value_type						value_type;
	typedef value_type&												reference;
	typedef std::vector<SourceType*>								SourceVector;
	typedef typename std::vector<SourceType>::iterator				SourceVectorIterator;
	typedef std::vector<std::string>								StringVector;
	typedef typename std::vector<std::string>::const_iterator		StringVectorIterator;

	SourceComposer(const SourceVector& somesources) : sources(somesources) {}


	class iterator
	{
	public:
		typedef iterator ThisType;
		typedef iterator& ThisReference;
		typedef typename SourceIterator::value_type						value_type;

		value_type operator*() {
			return *it;
		}

		ThisType operator++(int) {
			ThisType tmp(*this);
			++*this;
			return tmp;
		}

		ThisReference operator++() {
			++it;
			if(it == sources[index]->end() && index < sources.size()-1){
				++index;
				it = sources[index]->begin();
			}
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

		size_t getSlice() const {
			return this->index;
		}

	protected:
		iterator(SourceVector& somesources) :
			sources(somesources), it(somesources.front()->begin()), index(0) {
		}

		// bool argument to mark an end iterator constructor
		iterator(SourceVector& somesources, bool) :
			sources(somesources), it(somesources.back()->end()), index(somesources.size()-1) {
		}
		SourceVector& sources;
		SourceIterator it;
		size_t index;

		friend class SourceComposer;

	};

	size_t getRows() const {
		return sources.front()->getRows();
	}

	size_t getCols() const {
		return sources.front()->getCols();
	}

	size_t getSlices() const {
		return sources.size();
	}

	iterator begin() {
		return iterator(sources);
	}

	iterator end() {
		return iterator(sources, true);
	}

private:
	SourceVector		sources;
};

}

#endif /* SOURCECOMPOSER_HPP_ */
