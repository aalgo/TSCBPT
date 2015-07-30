/*
 * VectorDataWriter.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 05/11/2012
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef VECTORDATAWRITER_HPP_
#define VECTORDATAWRITER_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include "../policies/CheckingPolicy.hpp"

namespace tscbpt {

template<class CheckingPolicy = FullCheckingPolicy>
class VectorDataWriter {
public:
	typedef CheckingPolicy 	CheckPol;
	typedef float			out_type;

	VectorDataWriter(){}

	void writeConfigFile(std::string basedir, size_t rows,
			size_t cols, std::string polarCase = std::string("monostatic"),
			std::string polartype = std::string("full")) {
		std::ofstream fout;
		fout.open((basedir + "/config.txt").data());
		check.ioStateOK(fout);
		fout << "Nrow" << std::endl;
		fout << rows << std::endl;
		fout << "---------" << std::endl;
		fout << "Ncol" << std::endl;
		fout << cols << std::endl;
		fout << "---------" << std::endl;
		fout << "PolarCase" << std::endl;
		fout << polarCase << std::endl;
		fout << "---------" << std::endl;
		fout << "PolarType" << std::endl;
		fout << polartype;
		fout.close();
	}

	template<typename InputIteratorStart, typename InputIteratorEnd>
	void writeDataToDir(std::string basedir, InputIteratorStart first,
			InputIteratorEnd last, std::string prefix = std::string("C")) {
		size_t vectorSize = (*first)->getModel().getNumElems();
		size_t nFiles = vectorSize;
		size_t index = 0;
		std::ofstream fileOut[nFiles];
		std::string file;
		out_type value;

		index = 0;
		for (size_t i = 1; i <= vectorSize; ++i) {
			file = basedir + std::string("/") + prefix + to_string(i) + std::string(".bin");
			fileOut[index++].open(file.data(), ios::out | ios::trunc);
			check.ioStateOK(fileOut[index - 1]);
		}

		for (; first != last; ++first) {
			index = 0;
			for (size_t i = 1; i <= vectorSize; ++i) {
				value = (*first)->getModel()(i - 1);
				fileOut[index++].write(reinterpret_cast<char*>(&value),
						sizeof(value));
				check.ioStateOK(fileOut[index - 1]);
			}
		}

		index = 0;
		for (size_t i = 1; i <= vectorSize; ++i) {
			fileOut[index++].close();
		}
	}

protected:
	static const CheckPol check;
};

template <class CheckingPolicy>
const typename VectorDataWriter<CheckingPolicy>::CheckPol VectorDataWriter<CheckingPolicy>::check;

} /* namespace tscbpt */
#endif /* VECTORDATAWRITER_HPP_ */
