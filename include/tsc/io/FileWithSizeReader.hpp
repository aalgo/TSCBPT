/*
 * FileWithSizeReader.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 09/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef FILEWITHSIZEREADER_HPP_
#define FILEWITHSIZEREADER_HPP_

// ---- Defined in STL ----
// This marks string literals in header files to be extracted for eventual
// translation.  It is primarily used for messages in thrown exceptions; see
// src/functexcept.cc.  We use __N because the more traditional _N is used
// for something else under certain OSes (see BADNAMES).
//#define __N(msgid)     (msgid)

#include <stddef.h>
#include <iostream>
#include <fstream>
#include <string>
#include "BinaryFileIterator.hpp"


namespace tscbpt {

using namespace std;

template<typename DataValue>
class FileWithSizeReader {
private:
	typedef int			file_size_type;
	string _filename;
	size_t _rows,_cols,_offset;
	ifstream _instream;
	bool _swap_endian;

	template<typename Type>
	Type swap_endian(Type value) {
		char * data = reinterpret_cast<char*>(&value);
		const size_t data_size = sizeof(Type);
		for (size_t i = 0; i < sizeof(Type) / 2; ++i) {
			unsigned char tmp = data[i];
			data[i] = data[data_size - 1 - i];
			data[data_size - 1 - i] = tmp;
		}
		return value;
	}

	void readsize(ifstream& file){
		file_size_type n;
		file.read((char*) &n, sizeof(n));
		if(_swap_endian) _cols = swap_endian(n);
		else _cols = n;
		file.read((char*) &n, sizeof(n));
		if(_swap_endian) _rows = swap_endian(n);
		else _rows = n;
		clog<<"Size: "<<_rows<<"x"<<_cols<<endl;

		_offset = 2;

		file.seekg (0, ios::end);
		size_t filesize = file.tellg();

		if(_rows*_cols*sizeof(DataValue) != filesize-2*sizeof(n)){
			__throw_invalid_argument(__N("Rows and Cols information in the file does not correspond with its size"));
		}
	}


public:
	FileWithSizeReader(): _rows(0), _cols(0), _offset(0), _swap_endian(false){}

	FileWithSizeReader(const string& filename, bool swap = false): _filename(filename), _swap_endian(swap){
		ifstream is;
		is.open (filename.c_str(), ios::binary );

		// get length of file:
		readsize(is);
		is.close();
	}

	FileWithSizeReader(const string& filename, size_t rows, size_t cols, bool swap = false): _filename(filename),
			_rows(rows), _cols(cols), _swap_endian(swap){
		ifstream is;
		_offset = 0;
		is.open (filename.c_str(), ios::binary );

		// get length of file:
		is.seekg (0, ios::end);
		size_t filesize = is.tellg();
		is.close();
		if(_rows*_cols*sizeof(DataValue) != filesize){
			cerr << "Theoretical size: " << _rows*_cols*sizeof(DataValue) << ", Real size: " << filesize << endl;
			__throw_invalid_argument(__N("Rows and Cols arguments in the constructor does not correspond with its size"));
		}
	}

	FileWithSizeReader(const FileWithSizeReader<DataValue>& b) {
		this->_filename = b._filename;
		this->_rows = b._rows;
		this->_cols = b._cols;
		this->_offset = b._offset;
		this->_swap_endian = b._swap_endian;
	}

	FileWithSizeReader<DataValue>& operator =(const FileWithSizeReader<DataValue>& b){
		this->_filename = b._filename;
		this->_rows = b._rows;
		this->_cols = b._cols;
		this->_offset = b._offset;
		this->_swap_endian = b._swap_endian;
		return *this;
	}

	typedef BinaryFileIterator<DataValue>	iterator;

	iterator begin() {
		if(_instream.is_open()) _instream.close();
		_instream.open(_filename.c_str(), ios::binary);
		_instream.seekg(_offset*sizeof(file_size_type), ios::beg);
		return iterator(_instream, _swap_endian);
	}

	iterator end() {
		return iterator();
	}

	string getFilename() const
	{
		return _filename;
	}

	size_t getRows() const
	{
		return _rows;
	}

	size_t getCols() const
	{
		return _cols;
	}
};

}

#endif /* FILEWITHSIZEREADER_HPP_ */
