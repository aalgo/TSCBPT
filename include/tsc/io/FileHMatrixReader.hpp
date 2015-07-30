/*
 * FileHMatrixReader.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 22/12/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef FILEHMATRIXREADER_HPP_
#define FILEHMATRIXREADER_HPP_

#include <vector>
#include <string>
#include <armadillo>
#include "FileWithSizeReader.hpp"
#include "PositionCounter.hpp"


namespace tscbpt {

using namespace std;
using namespace arma;

template<typename InType, typename OutType, size_t matrix_size=3>
class FileHMatrixReader {
private:
	FileWithSizeReader<InType> _ins[matrix_size*matrix_size];
public:
	FileHMatrixReader(string& prefix, size_t nrows, size_t ncols, size_t offset = 0){
		int k=0;
		for(size_t i = 1; i<= matrix_size; i++){
			_ins[k++] = FileWithSizeReader<InType>(prefix + to_string(i+offset) + to_string(i+offset) + ".bin", nrows, ncols);
			for(size_t j = i+1; j<= matrix_size; j++){
				_ins[k++] = FileWithSizeReader<InType>(prefix + to_string(i+offset) + to_string(j+offset) + "_real.bin", nrows, ncols);
				_ins[k++] = FileWithSizeReader<InType>(prefix + to_string(i+offset) + to_string(j+offset) + "_imag.bin", nrows, ncols);
			}
		}
	}

	struct iterator : public std::iterator<
			std::input_iterator_tag,
			typename Mat<complex<OutType> >::template fixed<matrix_size, matrix_size> >,
			PositionCounter<2> {
	private:
		typename FileWithSizeReader<InType>::iterator  _its[matrix_size*matrix_size];
		typename Mat<complex<OutType> >::template fixed<matrix_size, matrix_size> _data;

	protected:
		iterator(FileWithSizeReader<InType> ins[matrix_size*matrix_size]): PositionCounter<2>::PositionCounter(ins[0].getRows(), ins[0].getCols()){
			for(size_t i = 0; i< matrix_size; i++){
				for(size_t j = 0; j< matrix_size; j++){
					_its[i*matrix_size + j] = ins[i*matrix_size + j].begin();
				}
			}
			load_dataValue();
		}

		// bool argument to mark an end iterator constructor
		iterator(FileWithSizeReader<InType>  ins[matrix_size*matrix_size], bool): PositionCounter<2>::PositionCounter(ins[0].getRows(), ins[0].getCols(), true){
			for(size_t i = 0; i< matrix_size; i++){
				for(size_t j = 0; j< matrix_size; j++){
					_its[i*matrix_size + j] = ins[i*matrix_size + j].end();
				}
			}
		}

		void inc_iterators(){
			for(size_t i = 0; i< matrix_size; i++){
				for(size_t j = 0; j< matrix_size; j++){
					++_its[i*matrix_size + j];
				}
			}
			increment();
		}

		void load_dataValue(){
			int k=0;
			for(size_t i = 1; i<= matrix_size; i++){
				_data(i-1,i-1) = complex<OutType>(OutType(*_its[k++]), OutType());
				for(size_t j = i+1; j<= matrix_size; j++){
					OutType real = OutType(*_its[k++]);
					OutType imag = OutType(*_its[k++]);
					_data(i-1,j-1) = complex<OutType>(real, imag);
					_data(j-1,i-1) = conj(_data(i-1,j-1));
				}
			}
		}

	public:
		typedef typename FileHMatrixReader<InType, OutType, matrix_size>::iterator 	this_value_type;
		typedef typename FileHMatrixReader<InType, OutType, matrix_size>::iterator* this_pointer_type;
		typedef typename FileHMatrixReader<InType, OutType, matrix_size>::iterator& this_reference_type;
		typedef typename iterator_traits<this_value_type>::difference_type 	difference_type;
		typedef typename iterator_traits<this_value_type>::reference   		reference;
		typedef typename iterator_traits<this_value_type>::pointer     		pointer;

		this_value_type operator ++(int n) {
			this_value_type tmp(*this);
			inc_iterators();
			load_dataValue();
			return tmp;
		}

		this_reference_type operator ++() {
			inc_iterators();
			load_dataValue();
			return *this;
		}

		bool operator<(const this_reference_type b) const{
			return this->_its[0] < b._its[0];
		}

		bool operator==(const this_reference_type b) const{
			return this->_its[0] == b._its[0];
		}

		bool operator!=(const this_reference_type b) const{
			return _its[0] != b._its[0];
		}

		reference operator *(){
			return _data;
		}
		const reference operator *() const{
			return _data;
		}

		pointer operator->() const
		{ return &(operator*()); }

		friend class FileHMatrixReader<InType, OutType, matrix_size>;
	};

	iterator begin() {
		return iterator(this->_ins);
	}

	iterator end() {
		return iterator(this->_ins, true);
	}
};

}


#endif /* FILEHMATRIXREADER_HPP_ */
