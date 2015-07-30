/*
 * PolSARProFormatMatrixWriter.hpp
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

#ifndef POLSARPROFORMATMATRIXWRITER_HPP_
#define POLSARPROFORMATMATRIXWRITER_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include "../policies/CheckingPolicy.hpp"

namespace tscbpt
{

template<class CheckingPolicy	= FullCheckingPolicy >
class PolSARProFormatMatrixWriter
{
public:
	typedef CheckingPolicy			CheckPol;

	PolSARProFormatMatrixWriter(){}

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
		size_t matrixSize = (*first)->getModel().getMatrixSize();
		size_t nFiles = matrixSize * matrixSize;
		size_t index = 0;
		ofstream fileOut[nFiles];
		std::string file;
		float value;

		index = 0;
		for(size_t i = 1; i <= matrixSize; ++i){
			file = basedir + string("/") + prefix + to_string(i) + to_string(i) + string(".bin");
			fileOut[index++].open(file.data(), ios::out | ios::trunc);
			check.ioStateOK(fileOut[index-1]);
			for(size_t j = i+1; j <= matrixSize; ++j){
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
				for (size_t j = i + 1; j <= matrixSize; ++j) {
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

		index = 0;
		for (size_t i = 1; i <= matrixSize; ++i) {
			fileOut[index++].close();
			for (size_t j = i + 1; j <= matrixSize; ++j) {
				fileOut[index++].close();
				fileOut[index++].close();
			}
		}
	}

protected:
	static const CheckPol		check;
};

template <class CheckingPolicy>
const typename PolSARProFormatMatrixWriter<CheckingPolicy>::CheckPol PolSARProFormatMatrixWriter<CheckingPolicy>::check;

}

#endif /* POLSARPROFORMATMATRIXWRITER_HPP_ */
