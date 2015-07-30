/*
 * MultiFileWithSizeReader.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 16/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef MULTIFILEWITHSIZEREADER_HPP_
#define MULTIFILEWITHSIZEREADER_HPP_

#include <string>
#include <vector>
#include <iterator>
#include <algorithm>
#include <utility>
#include "FileWithSizeReader.hpp"


namespace tscbpt {

using namespace std;

template<typename DataType>
class MultiFileWithSizeReader {
private:
	vector<FileWithSizeReader<DataType> > _files;

	void checkSizes(){
		size_t rows = _files.front().getRows();
		size_t cols = _files.front().getCols();
		typename vector<FileWithSizeReader<DataType> >::iterator it = _files.begin();
		++it;
		while(it != _files.end()){
			if((*it).getRows() != rows){
				__throw_invalid_argument(__N("#Rows information in the files are different"));
			}
			if((*it).getCols() != cols){
				__throw_invalid_argument(__N("#Columns information in the files are different"));
			}
			++it;
		}
	}

	void checkNFiles(){

	}
public:
	// Use with caution (double pointer)
	MultiFileWithSizeReader(const char** filenames, int nfiles, bool swap = false){
		_files.reserve(nfiles);
		for(int i = 0; i < nfiles; ++i){
			string s(filenames[i]);
			FileWithSizeReader<DataType> file(s, swap);
			_files.push_back(file);
		}
		checkSizes();
	}
	MultiFileWithSizeReader(const string* filenames, int nfiles, bool swap = false){
		_files.reserve(nfiles);
		for(int i = 0; i < nfiles; ++i){
			FileWithSizeReader<DataType> file(filenames[i], swap);
			_files.push_back(file);
		}
		checkSizes();
	}
	// Use with caution (double pointer)
	MultiFileWithSizeReader(const char** filenames, int nfiles, size_t rows, size_t cols, bool swap = false){
		_files.reserve(nfiles);
		for(int i = 0; i < nfiles; ++i){
			string s(filenames[i]);
			FileWithSizeReader<DataType> file(s, rows, cols, swap);
			_files.push_back(file);
		}
	}
	MultiFileWithSizeReader(const string* filenames, int nfiles, size_t rows, size_t cols, bool swap = false){
		_files.reserve(nfiles);
		for(int i = 0; i < nfiles; ++i){
			FileWithSizeReader<DataType> file(filenames[i], rows, cols, swap);
			_files.push_back(file);
		}
	}

	MultiFileWithSizeReader(vector<string> filenames, bool swap = false) {
		_files.reserve(filenames.size());
		for (size_t i = 0; i < filenames.size(); ++i) {
			FileWithSizeReader<DataType> file(filenames[i], swap);
			_files.push_back(file);
		}
		checkSizes();
	}

	MultiFileWithSizeReader(vector<string> filenames, size_t rows, size_t cols, bool swap = false) {
		_files.reserve(filenames.size());
		for (size_t i = 0; i < filenames.size(); ++i) {
			FileWithSizeReader<DataType> file(filenames[i], rows, cols, swap);
			_files.push_back(file);
		}
	}

	size_t getRows() const{
		return _files.front().getRows();
	}

	size_t getCols() const{
		return _files.front().getCols();
	}

	struct iterator : public std::iterator<input_iterator_tag, vector<DataType> >{
		typedef FileWithSizeReader<DataType>						File;
		typedef typename File::iterator								FileIterator;
		typedef vector<DataType>									DataTypeVector;
		typedef vector<File>										FileVector;
		typedef typename vector<File>::iterator						FileVectorIterator;
		typedef vector<FileIterator>								FileIteratorVector;
		typedef typename DataTypeVector::iterator					DataTypeVectorIterator;
		typedef typename FileIteratorVector::iterator				FileIteratorVectorIterator;

		typedef DataType 											data_type;
		typedef vector<DataType>									value_type;
		typedef iterator& 											iterator_ref;
		typedef const iterator& 									iterator_ref_const;
		typedef iterator* 											iterator_pointer;

	private:
		DataTypeVector 				_current;
		FileIteratorVector 			_its;
		size_t						_pos, _cols;

		void increase_its(){
			for(size_t i = 0; i < _its.size(); ++i){
				++(_its[i]);
				_current[i] = *(_its[i]);
			}
			++_pos;
		}

		iterator(FileVector& readers, bool){
			_current.reserve(readers.size());
			_its.reserve(readers.size());
			FileVectorIterator it = readers.begin(), itend = readers.end();
			_cols = (*it).getCols();
			_pos = (*it).getRows() * _cols;
			while(it != itend){
				_its.push_back((*it).end());
				++it;
				//_current.push_back(*(_its.back()));
			}
		}

		friend class MultiFileWithSizeReader<DataType>;

	public:

		iterator(FileVector& readers){
			_current.reserve(readers.size());
			_its.reserve(readers.size());
			FileVectorIterator it = readers.begin();
			_cols = (*it).getCols();
			_pos = 0;
			while(it != readers.end()){
				_its.push_back((*it).begin());
				++it;
				_current.push_back(*(_its.back()));
			}
		}

		iterator_ref operator++(){
			increase_its();
			return *this;
		}

//		iterator operator++(int){
//			iterator tmp(*this);
//			increase_its();
//			return tmp;
//		}

		size_t getRow() const {
			return _pos / _cols;
		}

		size_t getCol() const {
			return _pos  % _cols;
		}

		bool operator==(iterator_ref_const b) const{
			return _its.front() == b._its.front();
		}

		bool operator!=(iterator_ref_const b) const {
			return _its.front() != b._its.front();
		}

		value_type& operator* () {
			return _current;
		}
		const value_type& operator* () const {
			return _current;
		}
	};

	typedef typename iterator::value_type			value_type;

	iterator begin(){
		return iterator(this->_files);
	}

	iterator end(){
		return iterator(this->_files, true);
	}
};

}

#endif /* MULTIFILEWITHSIZEREADER_HPP_ */
