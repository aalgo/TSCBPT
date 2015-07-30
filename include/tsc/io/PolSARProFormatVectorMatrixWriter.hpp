/*
 * PolSARProFormatVectorMatrixWriter.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 08/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef POLSARPROFORMATVECTORMATRIXWRITER_HPP_
#define POLSARPROFORMATVECTORMATRIXWRITER_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include "../policies/CheckingPolicy.hpp"
#include <tsc/util/ToString.hpp>

namespace tscbpt
{

template<
	size_t SubMatrixSize 	= 3,
	class CheckingPolicy	= FullCheckingPolicy
	>
class PolSARProFormatVectorMatrixWriter
{
public:
	typedef CheckingPolicy			CheckPol;

	PolSARProFormatVectorMatrixWriter(){}

	void writeConfigFile(std::string basedir, size_t rows, size_t cols,
		std::string polarCase = string("monostatic"), std::string polartype = string("full")){
		ofstream fout;
		fout.open((basedir + "/config.txt").data());
		check.ioStateOK(fout);
		fout << "Nrow" << endl;
		fout << rows << endl;
		fout << "---------" << endl;
		fout << "Ncol" << endl;
		fout << cols << endl;
		fout << "---------" << endl;
		fout << "PolarCase" << endl;
		fout << polarCase << endl;
		fout << "---------" << endl;
		fout << "PolarType" << endl;
		fout << polartype;
		fout.close();
	}

	template<typename InputIteratorStart, typename InputIteratorEnd>
	void writeDataToDir(std::string basedir, InputIteratorStart first, InputIteratorEnd last, std::string prefix = string("T")){
		const size_t matrixSize = (*first)->getModel().getMatrixSize();
		const size_t nFiles = SubMatrixSize * SubMatrixSize * (matrixSize/SubMatrixSize);
		size_t index = 0;
		ofstream fileOut[nFiles];
		std::string file;
		float value;

		index = 0;
		for(size_t i = 1; i <= matrixSize; ++i){
			file = basedir + string("/") + prefix + to_string(i) + to_string(i) + string(".bin");
			fileOut[index++].open(file.data(), ios::out | ios::trunc);
			check.ioStateOK(fileOut[index-1]);
			size_t matrix_index = (i-1) / SubMatrixSize;
			for(size_t j = i+1; j <= (matrix_index + 1)*SubMatrixSize; ++j){
				fileOut[index++].open((basedir + string("/") + prefix + to_string(i) + to_string(j) + string("_real.bin")).data());
				check.ioStateOK(fileOut[index-1]);
				fileOut[index++].open((basedir + string("/") + prefix + to_string(i) + to_string(j) + string("_imag.bin")).data());
				check.ioStateOK(fileOut[index-1]);
			}
		}

		for(; first != last; ++first){
			index = 0;
			for (size_t i = 1; i <= matrixSize; ++i) {
				value = (*first)->getModel()(i-1,i-1).real();
				fileOut[index++].write(reinterpret_cast<char*>(&value), sizeof(value));
				check.ioStateOK(fileOut[index - 1]);
				size_t matrix_index = (i-1) / SubMatrixSize;
				for (size_t j = i + 1; j <= (matrix_index + 1)*SubMatrixSize; ++j) {
					complex<double> elem = (*first)->getModel()(i-1,j-1);
					value = elem.real();
					fileOut[index++].write(reinterpret_cast<char*>(&value), sizeof(value));
					check.ioStateOK(fileOut[index - 1]);
					value = elem.imag();
					fileOut[index++].write(reinterpret_cast<char*>(&value), sizeof(value));
					check.ioStateOK(fileOut[index - 1]);
				}
			}
		}

		for (size_t i = 0; i < nFiles; ++i) {
			fileOut[i].close();
		}
	}

protected:
	static const CheckPol		check;
};

template <size_t SubMatrixSize, class CheckingPolicy>
const typename PolSARProFormatVectorMatrixWriter<SubMatrixSize, CheckingPolicy>::CheckPol PolSARProFormatVectorMatrixWriter<SubMatrixSize, CheckingPolicy>::check;

}

#endif /* POLSARPROFORMATVECTORMATRIXWRITER_HPP_ */
