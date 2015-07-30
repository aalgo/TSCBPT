/*
 * HasLogDetAverage.h
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

#ifndef HASLOGDETAVERAGE_H_
#define HASLOGDETAVERAGE_H_


#include <boost/concept_check.hpp>


namespace tscbpt { namespace concept {

template <class X>
struct HasLogDetAverage
{
	// Require value_type type definition
	typedef typename X::logdetavg_type		logdetavg_type;

	BOOST_CONCEPT_USAGE(HasLogDetAverage)
	{
		logdetavg_value = obj.getLogDetAverage();
//		X mergedObj = obj.merge(obj);
//		X unmergedObj = obj.unmerge(obj);
	}
private:
	X obj;
	logdetavg_type logdetavg_value;
};

}}


#endif /* HASLOGDETAVERAGE_H_ */
