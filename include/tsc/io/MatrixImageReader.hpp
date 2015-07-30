/*
 * MatrixImageReader.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 15/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef MATRIXIMAGEREADER_HPP_
#define MATRIXIMAGEREADER_HPP_

#include "../bpt/models/NativeMatrixModel.hpp"
#include "../image/Pixel.hpp"
//#include "WrapperIterator.hpp"
#include <string>
//#include <boost/static_assert.hpp>

namespace tscbpt
{

template<
	typename OutDataType 	= NativeMatrixModel<>,
	typename TConverter		= HV3DScatteringVector,
	typename TReader 		= MultiFileWithSizeReader<complex<float> >
	>
class MatrixImageReader
{
public:
	typedef TReader									ReaderType;
	typedef typename ReaderType::value_type			inner_value_type;
	typedef OutDataType								value_type;
	typedef typename ReaderType::iterator			ReaderIterator;
	typedef TConverter								Converter;

	MatrixImageReader(const std::string* filenames, int nfiles) : reader(filenames, nfiles){
	}
	MatrixImageReader(const std::string* filenames, int nfiles, size_t rows, size_t cols) :
		reader(filenames, nfiles, rows, cols) {
	}

	struct iterator{
		typedef	OutDataType					value_type;

		iterator(const ReaderIterator& ait): it(ait){
		}

		value_type operator*(){
			Converter toSVector;
			inner_value_type data = toSVector(*it);
			return value_type(Pixel<inner_value_type>(
					data, static_cast<float>(it.getCol()), static_cast<float>(it.getRow())));
		}

		iterator operator++(int) {
			iterator tmp(*this);
			++*this;
			return tmp;
		}

		iterator& operator++(){
			++it;
			return *this;
		}

		size_t getRow() const {
			return this->it.getRow();
		}

		size_t getCol() const {
			return this->it.getCol();
		}

		template<typename RowType>
		RowType getRow() const {
			return this->it.getRow();
		}

		template<typename ColType>
		ColType getCol() const {
			return this->it.getCol();
		}

		template<typename SliceType>
		SliceType getSlice() const {
			return this->it.getSlice();
		}

		bool operator==(const iterator& rhs) const {
			return this->it == rhs.it;
		}

		bool operator!=(const iterator& rhs) const {
			return this->it != rhs.it;
		}


	private:
		ReaderIterator	it;

		template<class T, size_t NumDims>
		struct ConstructorCaller{
			static T call(ReaderIterator& it);
		};
		template <class T>
		struct ConstructorCaller<T, 1>
		{
			static T call(ReaderIterator& it) {
				return T(*it, static_cast<float>(it.getRow()));
			}
		};
		template <class T>
		struct ConstructorCaller<T, 2>
		{
			static T call(ReaderIterator& it){
				return T(*it, static_cast<float>(it.getCol()), static_cast<float>(it.getRow()));
			}
		};
		template <class T>
		struct ConstructorCaller<T, 3>
		{
			static T call(ReaderIterator& it) {
				return T(*it, static_cast<float>(it.getSlice()),
					static_cast<float>(it.getCol()), static_cast<float>(it.getRow()));
			}
		};
	};

	size_t getRows() const {
		return reader.getRows();
	}

	size_t getCols() const {
		return reader.getCols();
	}

	iterator begin(){
		return iterator(reader.begin());
	}

	iterator end() {
		return iterator(reader.end());
	}

protected:
	ReaderType							reader;

};

}

#endif /* MATRIXIMAGEREADER_HPP_ */
