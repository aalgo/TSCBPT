/*
 * FileReader.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 05/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef FILEREADER_HPP_
#define FILEREADER_HPP_

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

namespace tscbpt {

using namespace std;

template<typename DataValue>
class FileReader {
private:
	string _filename;
	size_t _filesize;
public:
	FileReader(string filename): _filename(filename){
		ifstream is;
		is.open (filename.c_str(), ios::binary );

		// get length of file:
		is.seekg (0, ios::end);
		_filesize = is.tellg();
		is.close();
	}

	struct iterator{
	private:
		ifstream _ifile;
		size_t _readbytes;
		DataValue _current;

		iterator(string filename): _ifile(filename.c_str(), ios::in | ios::binary),
				_readbytes(0){}
		iterator(string filename, size_t pos): _ifile(filename.c_str(), ios::in | ios::binary),
						_readbytes(pos){
			_ifile.seekg (pos, ios::beg);
		}

	public:
		DataValue operator *(){
			return _current;
		}

		iterator& operator ++(int n) {
			if(_ifile.eof()){
				__throw_invalid_argument(__N("EOF reached and operator++ called!"));
			}else{
				_ifile.read(&(_current), sizeof(DataValue));
			}
			return *this;
		}

		iterator& operator ++() {
			if(_ifile.eof()){
				__throw_invalid_argument(__N("EOF reached and operator++ called!"));
			}else{
				_ifile.read(&(_current), sizeof(DataValue));
			}
			return *this;
		}

		bool operator<(const iterator& b) {
			return this->_readbytes < b._readbytes;
		}
		bool operator==(const iterator& b) {
			return this->_readbytes == b._readbytes;
		}
	};

	iterator begin_iterator() {
		return iterator(_filename);
	}

	iterator end_iterator() {
		return iterator(_filename, _filesize);
	}

	string getFilename() const
	{
		return _filename;
	}

	size_t getFilesize() const
	{
		return _filesize;
	}
};

}

#endif /* FILEREADER_HPP_ */
