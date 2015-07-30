/*
 * PNodeComparators.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 26/06/2013
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef PNODECOMPARATORS_H_
#define PNODECOMPARATORS_H_


#include <functional>


namespace tscbpt
{

using std::binary_function;
using std::vector;

template<typename NodePointer>
struct pnode_bigger:
	public binary_function<NodePointer, NodePointer, bool> {
	bool operator()(NodePointer const __x, NodePointer const __y) const {
		return __x->getModel().getSubnodes() > __y->getModel().getSubnodes() ||
				(__x->getModel().getSubnodes() == __y->getModel().getSubnodes() &&
						__x < __y);
	}
};

template<typename NodePointer>
struct pnode_smaller:
	public binary_function<NodePointer, NodePointer, bool> {
	bool operator()(NodePointer const __x, NodePointer const __y) const {
		return __x->getModel().getSubnodes() < __y->getModel().getSubnodes() ||
				(__x->getModel().getSubnodes() == __y->getModel().getSubnodes() &&
						__x < __y);
	}
};

template<typename NodePointer>
struct pnode_later:
	public binary_function<NodePointer, NodePointer, bool> {
	bool operator()(NodePointer const __x, NodePointer const __y) const {
		return __x->getId() > __y->getId();
	}
};

/**
 * Comparator to sort Nodes within a contained by ascending homogeneity
 * in terms of TotalSumOfSquares.
 */
template <class T>
struct pnode_homog_less: public binary_function<T, T, bool>
{
	inline bool operator()(T const __x, T const __y) const {
		return __x->getModel().getTotalSumOfSquares() / norm2(__x->getModel()) / __x->getModel().getSubnodes()
			< __y->getModel().getTotalSumOfSquares() / norm2(__y->getModel()) / __y->getModel().getSubnodes()
			|| (__x->getModel().getTotalSumOfSquares() / norm2(__x->getModel()) / __x->getModel().getSubnodes()
				== __y->getModel().getTotalSumOfSquares() / norm2(__y->getModel()) / __y->getModel().getSubnodes() && __x
				< __y);
	}
};

}

#endif /* PNODECOMPARATORS_H_ */
