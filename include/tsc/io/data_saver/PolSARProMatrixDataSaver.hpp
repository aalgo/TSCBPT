/*
 * PolSARProMatrixDataSaver.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 09/01/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef POLSARPROMATRIXDATASAVER_HPP_
#define POLSARPROMATRIXDATASAVER_HPP_

#include "DataSaver.hpp"
#include "../../policies/CheckingPolicy.hpp"
#include <tsc/util/ToString.hpp>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <complex>
#include <cassert>
#include <sys/types.h>
#include <sys/stat.h>

namespace tscbpt {

using namespace std;

template<
	class CheckingPolicy	= FullCheckingPolicy
	>
class PolSARProMatrixDataSaver : public DataSaver{
public:
	typedef CheckingPolicy			CheckPol;
	typedef PolSARProMatrixDataSaver<CheckingPolicy>	this_type;

	PolSARProMatrixDataSaver(size_t matrix_size, size_t sub_matrix_size, string basedir, string prefix = string("C")): msize(matrix_size), subMatrix_size(sub_matrix_size){
		const size_t nFiles = subMatrix_size * subMatrix_size * (msize / subMatrix_size);
		fileOut = new ofstream[nFiles];
		string file;
		size_t index = 0;
		{	// Creating the directory path basedir
			struct stat fstat;
			if (stat(basedir.c_str(), &fstat) != 0){
				cout << "\n  Creating dir " << basedir.c_str() << endl;
				assert(mkdir(basedir.c_str(), S_IRWXU)==0);
			}
		}
		for (size_t i = 1; i <= msize; ++i) {
			file = basedir + string("/") + prefix + to_string(i) + to_string(i)	+ string(".bin");
			fileOut[index++].open(file.data(), ios::out | ios::trunc);
			check.ioStateOK(fileOut[index-1]);
			size_t matrix_index = (i - 1) / subMatrix_size;
			for (size_t j = i + 1; j <= (matrix_index + 1) * subMatrix_size;
					++j) {
				fileOut[index++].open((basedir + string("/") + prefix + to_string(i) + to_string(j) + string("_real.bin")).data(), ios::out | ios::trunc);
				check.ioStateOK(fileOut[index-1]);
				fileOut[index++].open((basedir + string("/") + prefix + to_string(i) + to_string(j) + string("_imag.bin")).data(), ios::out | ios::trunc);
				check.ioStateOK(fileOut[index-1]);
			}
		}
	}

	template<typename Elem>
	size_t save_elem(Elem el){
		size_t index = 0;
		float value;
		for (size_t i = 1; i <= msize; ++i) {
			value = el->getModel()(i - 1, i - 1).real();
			fileOut[index++].write(reinterpret_cast<char*>(&value), sizeof(value));
			check.ioStateOK(fileOut[index - 1]);
			size_t matrix_index = (i - 1) / subMatrix_size;
			for (size_t j = i + 1; j <= (matrix_index + 1) * subMatrix_size; ++j) {
				complex<double> elem = el->getModel()(i - 1, j - 1);
				value = elem.real();
				fileOut[index++].write(reinterpret_cast<char*>(&value), sizeof(value));
				check.ioStateOK(fileOut[index - 1]);
				value = elem.imag();
				fileOut[index++].write(reinterpret_cast<char*>(&value), sizeof(value));
				check.ioStateOK(fileOut[index - 1]);
			}
		}
		return index * sizeof(value);
	}

	void finalize(){
		const size_t nFiles = subMatrix_size * subMatrix_size * (msize/subMatrix_size);
		for (size_t i = 0; i < nFiles; ++i) {
			fileOut[i].close();
		}
	}

	~PolSARProMatrixDataSaver(){
		if(fileOut) delete[] fileOut;
	}

private:
	size_t msize;
	ofstream* fileOut;
	size_t subMatrix_size;

protected:
	static const CheckPol		check;
};

template <class CheckingPolicy>
const typename PolSARProMatrixDataSaver<CheckingPolicy>::CheckPol PolSARProMatrixDataSaver<CheckingPolicy>::check;

} /* namespace tscbpt */
#endif /* POLSARPROMATRIXDATASAVER_HPP_ */
