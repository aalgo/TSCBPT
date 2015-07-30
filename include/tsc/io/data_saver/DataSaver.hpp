/*
 * DataSaver.hpp
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

#ifndef DATASAVER_HPP_
#define DATASAVER_HPP_

#include <cstddef>

namespace tscbpt {

class DataSaver {
public:
	void initialize(){}

	template<typename Elem>
	size_t save_elem(Elem){return size_t();}

	void finalize(){}
};

} /* namespace tscbpt */
#endif /* DATASAVER_HPP_ */
