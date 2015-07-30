/*
 * HasTotalSumOfSquares.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 21/11/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef HASTOTALSUMOFSQUARES_H_
#define HASTOTALSUMOFSQUARES_H_


#include <boost/concept_check.hpp>


namespace tscbpt { namespace concept {

template <class X>
struct HasTotalSumOfSquares
{
	// Require value_type type definition
	typedef typename X::tss_type		tss_type;

	// Require getTotalSumOfSquares() definition
	BOOST_CONCEPT_USAGE(HasTotalSumOfSquares)
	{
		tss_value = obj.getTotalSumOfSquares();
	}
private:
	X obj;
	tss_type tss_value;
};

}}


#endif /* HASTOTALSUMOFSQUARES_H_ */
