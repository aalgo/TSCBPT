/*
 * StaticTests.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 21/03/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef STATICTESTS_H_
#define STATICTESTS_H_

#include <tscbpt.h>
#include <boost/static_assert.hpp>
#include <boost/concept/assert.hpp>


namespace
{

using namespace std;
using namespace tscbpt;

/**
 * Class to compile-time testing the TSCBPT library.
 *
 * This file does not add any code to the executable, it contains just some
 * static tests to ensure the correct definition of some template classes.
 * It may serve as an example of some library features or as a container
 * to include further static tests of newer classes or features, by
 * instantiating the corresponding template classes and functions.
 */
class StaticTests
{
public:
	StaticTests(){}

	void __PtrPoliciesTests() {
		// Check PtrStoragePolicies
		typedef DefaultPtrStorage<int> intPtrSt;
		typedef DefaultPtrStorage<double> doublePtrSt;
		typedef DefaultPtrStorage<set<double> > setPtrSt;

		int i = 3;
		double d = 0.0;
		set<double> s;

		intPtrSt::pointerType pint;
		doublePtrSt::pointerType pdouble;
		setPtrSt::pointerType pset;

		pint = &i;
		i = *pint;
		pdouble = &d;
		d = *pdouble;
		pset = &s;
		s = *pset;
	}
	void __StPoliciesTests() {
		// Check NativeStorage
		typedef NativeStorage<int> intPtrSt;
		typedef NativeStorage<double> doublePtrSt;
		typedef NativeStorage<set<double> > setPtrSt;

		intPtrSt::pointerType pint;
		doublePtrSt::pointerType pdouble;
		setPtrSt::pointerType pset;

		pint = intPtrSt::create();
		intPtrSt::remove(pint);

		pdouble = doublePtrSt::create();
		doublePtrSt::remove(pdouble);

		pset = setPtrSt::create();
		setPtrSt::remove(pset);
	}
	template<typename BPT>
		void __GenericTestBPTFrame() {
		typename BPT::WeakNodePointer p;
		typename BPT::StrongNodePointer pl;
		typename BPT::StrongNodePointer pr;
		typename BPT::Dissimilarity diss(pl, pr, 1.0);
		typename BPT::DissimilarityPointer pd = &diss;
		typename BPT::DissimilaritySet dissSet;
		dissSet = pl->getDissimilarities();
		typename BPT::NodeSet nodeSet;
		dissSet.insert(pd);
		nodeSet.insert(p);
		typename BPT::Constructor constructor(nodeSet);
		typename BPT::Reconstructor reconstructor(nodeSet);
	}
	template<typename BPT, typename ModelParam>
	void __GenericTestBPTFrame(ModelParam param) {
		__GenericTestBPTFrame<BPT>();
		typename BPT::Node n(param);
		typename BPT::WeakNodePointer p = n.getFather();
		typename BPT::StrongNodePointer pl = n.getLeftSoon();
		typename BPT::StrongNodePointer pr = n.getRightSoon();
		typename BPT::Dissimilarity diss(pl, pr, 1.0);
		typename BPT::DissimilarityPointer pd = &diss;
		typename BPT::DissimilaritySet dissSet;
		dissSet = n.getDissimilarities();
		typename BPT::NodeSet nodeSet;
		dissSet.insert(pd);
		nodeSet.insert(p);
	}
	void __TestBPTFrame() {
		typedef BPTFrame<double> BPTd;
		__GenericTestBPTFrame<BPTd>(2.0);

		typedef BPTFrame<int, double, unsigned int, DefaultNativeStoragePolicy, FullCheckingPolicy> BPTi;
		__GenericTestBPTFrame<BPTi>(2);
	}

	void __TestModels() {
		typedef NativeMatrixModel<> 		RegionModel;
		typedef BPTFrame<RegionModel> BPT;
		__GenericTestBPTFrame<BPT>();
	}

	void __TestArrayAccessor(){
		int a2int[4][3];
		vector<vector<int> > v2int;
		int value = make_ArrayAccessor(v2int)(1,2); value++;
		make_ArrayAccessor(v2int)(1,2) = value;
		value = make_ArrayAccessor(a2int)(1,2); value++;
		make_ArrayAccessor(a2int)(1,2) = value;
		int *p = &(a2int[0][0]);
		value = make_ArrayAccessor(p)(1); value++;
		make_ArrayAccessor(p)(1) = value;
		value = make_LinearAccessor(p,3)(1,2); value++;
		make_LinearAccessor(p,3)(1,2) = value;
	}

	void __Test_BasicTraits(){
		// Test type_equals
		BOOST_STATIC_ASSERT((type_equals<int,int>::value));
		BOOST_STATIC_ASSERT((!(type_equals<int,double>::value)));
		BOOST_STATIC_ASSERT((!(type_equals<signed int,unsigned int>::value)));
		BOOST_STATIC_ASSERT((!(type_equals<int,int*>::value)));
		BOOST_STATIC_ASSERT((!(type_equals<int,int&>::value)));
		BOOST_STATIC_ASSERT((!(type_equals<int*,double&>::value)));
		BOOST_STATIC_ASSERT((!(type_equals<int*,double*>::value)));
		BOOST_STATIC_ASSERT((!(type_equals<int&,double&>::value)));
		BOOST_STATIC_ASSERT((!(type_equals<const int&,int&>::value)));

		//Test AddConst
		BOOST_STATIC_ASSERT((type_equals<AddConst<int>::type, const int>::value));
		BOOST_STATIC_ASSERT((type_equals<AddConst<int&>::type, const int&>::value));
		BOOST_STATIC_ASSERT((type_equals<AddConst<int*>::type, int* const>::value));
		BOOST_STATIC_ASSERT((type_equals<AddConst<int*&>::type, int* const &>::value));
		BOOST_STATIC_ASSERT((type_equals<AddConst<const int>::type, const int>::value));
		BOOST_STATIC_ASSERT((type_equals<AddConst<const int&>::type, const int&>::value));
		BOOST_STATIC_ASSERT((type_equals<AddConst<const int*>::type, const int* const>::value));

		// Test BasicTraits
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int>::value_type, int>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int>::reference_type, int&>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int>::param_type, int>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int>::const_param_type, int>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int>::const_reference_type, const int&>::value));

		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int*>::value_type, int*>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int*>::reference_type, int*&>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int*>::param_type, int*>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int*>::const_param_type, int const*>::value));
		BOOST_STATIC_ASSERT((type_equals<BasicTraits<int*>::const_reference_type, int* const &>::value));
	}

	void __TestMatrix(){
		BOOST_CONCEPT_ASSERT((concept::Matrix<DenseMatrix<int> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<DenseMatrix<double> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<DenseMatrix<float> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<DenseMatrix<std::complex<double> > >));
		BOOST_STATIC_ASSERT((!(MatrixShape<DenseMatrix<int> >::isSquare)));
		BOOST_STATIC_ASSERT((!(MatrixShape<DenseMatrix<int> >::isHermitian)));

		BOOST_CONCEPT_ASSERT((concept::Matrix<SquareMatrix<int> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<SquareMatrix<double> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<SquareMatrix<float> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<SquareMatrix<std::complex<double> > >));
		BOOST_STATIC_ASSERT((MatrixShape<SquareMatrix<int> >::isSquare));
		BOOST_STATIC_ASSERT((!(MatrixShape<SquareMatrix<int> >::isHermitian)));

		BOOST_CONCEPT_ASSERT((concept::Matrix<HermitianMatrix<int> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<HermitianMatrix<double> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<HermitianMatrixFixed<5, double> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<HermitianMatrix<float> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<HermitianMatrix<std::complex<double> > >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<HermitianMatrix<std::complex<double> >::fixed<3> >));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrix<int> >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrix<int> >::isHermitian));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrix<std::complex<double> > >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrix<std::complex<double> > >::isHermitian));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrixFixed<2, int> >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrixFixed<2, int> >::isHermitian));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrixFixed<3, std::complex<double> > >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<HermitianMatrixFixed<3, std::complex<double> > >::isHermitian));

		BOOST_CONCEPT_ASSERT((concept::Matrix<BlockDiagonalMatrix<int, 3> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<BlockDiagonalMatrix<double, 9> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<BlockDiagonalMatrix<std::complex<double>, 3> >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<BlockDiagonalMatrix<float, 3, DenseMatrix<float> > >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<BlockDiagonalMatrix<std::complex<double>, 3, DenseMatrix<std::complex<double> > > >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<BlockDiagonalMatrix<std::complex<double>, 3, SquareMatrix<std::complex<double> > > >));
		BOOST_CONCEPT_ASSERT((concept::Matrix<BlockDiagonalMatrix<std::complex<double>, 3, HermitianMatrix<std::complex<double> > > >));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<long int, 3> >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<long int, 3> >::isHermitian));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<float, 3> >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<float, 3> >::isHermitian));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3> >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3> >::isHermitian));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3, DenseMatrix<std::complex<double> > > >::isSquare));
		BOOST_STATIC_ASSERT((!(MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3, DenseMatrix<std::complex<double> > > >::isHermitian)));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3, SquareMatrix<std::complex<double> > > >::isSquare));
		BOOST_STATIC_ASSERT((!(MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3, SquareMatrix<std::complex<double> > > >::isHermitian)));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3, HermitianMatrix<std::complex<double> > > >::isSquare));
		BOOST_STATIC_ASSERT((MatrixShape<BlockDiagonalMatrix<std::complex<double>, 3, HermitianMatrix<std::complex<double> > > >::isHermitian));


		HermitianMatrix<std::complex<double> > mc(3, 3);
		HermitianMatrix<double> md(9, 9);
		size_t s = mc.getRows();
		s = md.getCols();
		double v = md(1,3);
		md(3,3) = v;
		md *= 5;
		md /= 2;
		md += 3;
		md -= 1;
		mc *= v;
		mc *= s;
		mc += mc(0,0);

		BlockDiagonalMatrix<std::complex<double>, 3> mb(9, 9);
		s = mb.getRows();
		s = mb.getCols();
		std::complex<double> c = mb(0,0);
		mb(3,3) = v;
		mb *= std::complex<double>(5);
		mb /= std::complex<double>(2);
		mb += std::complex<double>(3);
		mb -= c;
	}
};

}

#endif /* STATICTESTS_H_ */
