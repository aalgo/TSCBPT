/* 
 * BPTDissimilarity.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 5/11/2010
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef _BPTDISSIMILARITY_HPP
#define	_BPTDISSIMILARITY_HPP


// *************** Definitions ******************

namespace tscbpt{

template
<class NodePointer, typename ValueType>
class BPTDissimilarity;
}


// ************* Implementation *****************

#include<boost/concept_check.hpp>

namespace tscbpt{

template
<
	class NodePointer,
	typename ValueType 		= double
>
class BPTDissimilarity {

	// Concept checking.
	// Require that template arguments are copy constructible and assignable
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<NodePointer>));
	BOOST_CONCEPT_ASSERT((boost::CopyConstructible<ValueType>));


private:
	NodePointer _a, _b;
	ValueType _dissimilarityValue;

public:
	typedef ValueType 								value_type;
	typedef NodePointer 							node_pointer;
	typedef BPTDissimilarity<NodePointer,ValueType> thisType;


    BPTDissimilarity(NodePointer a, NodePointer b, ValueType dissimilarityValue):
			_a(a), _b(b), _dissimilarityValue(dissimilarityValue){}

    NodePointer getA() const
    {
        return _a;
    }

    NodePointer getB() const
    {
        return _b;
    }

    ValueType getDissimilarityValue() const
    {
        return _dissimilarityValue;
    }

    void setA(NodePointer a)
    {
    	BOOST_CONCEPT_ASSERT((boost::Assignable<NodePointer>));
        this->_a = a;
    }

    void setB(NodePointer b)
    {
    	BOOST_CONCEPT_ASSERT((boost::Assignable<NodePointer>));
        this->_b = b;
    }

    NodePointer getNeighbor(NodePointer n){
    	if(this->_a==n) return this->_b;
    	else if(this->_b==n) return this->_a;
    	else return NodePointer();
    }

    const NodePointer getNeighbor(const NodePointer n) const {
    	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<NodePointer>));
    	if (this->_a == n) return this->_b;
		else if (this->_b == n) return this->_a;
		else return NodePointer();
	}

    void setDissimilarityValue(ValueType dissimilarityValue)
    {
    	BOOST_CONCEPT_ASSERT((boost::Assignable<ValueType>));
        this->_dissimilarityValue = dissimilarityValue;
	}

    bool operator <(const thisType& b) const {
    	BOOST_CONCEPT_ASSERT((boost::LessThanComparable<ValueType>));
    	BOOST_CONCEPT_ASSERT((boost::EqualityComparable<ValueType>));
    	return this->_dissimilarityValue < b._dissimilarityValue ||
    			(this->_dissimilarityValue == b._dissimilarityValue &&
    					this < &b);
    }
};

}
#endif	/* _BPTDISSIMILARITY_HPP */

