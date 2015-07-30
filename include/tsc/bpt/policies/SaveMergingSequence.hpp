/*
 * SaveMergingSequence.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 13/09/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef SAVEMERGINGSEQUENCE_HPP_
#define SAVEMERGINGSEQUENCE_HPP_

#include "BPTDataSavingPolicy.hpp"
#include <tsc/policies/CheckingPolicy.hpp>
#include <iostream>
#include <fstream>
#include <stdint.h>

namespace tscbpt
{

using namespace std;

template<
	typename 	IDType						= uint32_t,
	class 		CheckingPolicy				= FullCheckingPolicy
>
class SaveMergingSequence: public BPTDataSavingPolicy
{
public:
	typedef CheckingPolicy		CheckingPol;
	typedef IDType				NodeID;

	static const char* const DefaultMSFileName;

	SaveMergingSequence() : msFileName(DefaultMSFileName) {}

	void startBPTConstruction(){
		msOut.open(msFileName, ios::out | ios::trunc);
		Check.errorAssert(!(msOut.fail()));
	}

	template<class Dissimilarity>
	void saveSelectedDissimilarity(Dissimilarity d){
		NodeID id;
		msOut.write(reinterpret_cast<char*> (&(id = d->getA()->getId())), sizeof(id));
		msOut.write(reinterpret_cast<char*> (&(id = d->getB()->getId())), sizeof(id));
	}

	void endBPTConstruction(){
		msOut.close();
	}

	char *getMsFileName() const{
		return msFileName;
	}

	void setMsFileName(const char *msFileName){
		this->msFileName = msFileName;
	}

private:
	ofstream msOut;
	const char* msFileName;
	static const CheckingPol			Check;

protected:
//	~SaveMergingSequence(){}
};

template <typename IDType, class CheckingPolicy>
const char* const SaveMergingSequence<IDType, CheckingPolicy>::DefaultMSFileName 		= "BPT.msq";

template <typename IDType, class CheckingPolicy>
const typename SaveMergingSequence<IDType, CheckingPolicy>::CheckingPol SaveMergingSequence<IDType, CheckingPolicy>::Check;


}



#endif /* SAVEMERGINGSEQUENCE_HPP_ */
