/*
 * CanBeMerged.h
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

#ifndef CANBEMERGED_H_
#define CANBEMERGED_H_


#include <boost/concept_check.hpp>


namespace tscbpt { namespace concept {

template <class X>
struct CanBeMerged
{
	// Require merge() and unmerge() definition
	BOOST_CONCEPT_USAGE(CanBeMerged)
	{
		X tmp = obj.merge(obj);
		tmp = tmp.unmerge(obj);
	}
private:
	X obj;
};

}}



#endif /* CANBEMERGED_H_ */
