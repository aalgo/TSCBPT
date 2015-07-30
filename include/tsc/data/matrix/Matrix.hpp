/*
 * Matrix.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 28/10/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef MATRIX_HPP_
#define MATRIX_HPP_

#include <boost/concept/requires.hpp>
#include <boost/concept/assert.hpp>
#include <boost/concept_check.hpp>
#include <cstddef>


namespace tscbpt
{

template <typename T>
struct MatrixTraits{
	typedef typename T::elem_type	elem_type;
};

template<typename Matrix>
struct HasSquareShape{
	static const bool value 			= false;
};

template<typename Matrix>
struct HasHermitianShape{
	static const bool value 			= false;
};

template<typename Matrix>
struct MatrixShape{
	static const bool isSquare 			= HasSquareShape<Matrix>::value;
	static const bool isHermitian 		= HasHermitianShape<Matrix>::value;
};

namespace concept{

template <typename T>
struct Matrix
	: boost::Assignable<T>
{
private:
	typedef tscbpt::MatrixTraits<T> t;

public:
	typedef typename t::elem_type	elem_type;

	BOOST_CONCEPT_ASSERT((boost::Assignable<elem_type>));

	BOOST_CONCEPT_USAGE(Matrix)
    {
		size = matrix.getRows();	// require getRows() method
		size = matrix.getCols();	// require getCols() method
        elem = matrix(i,j);	// require (i,j) element access
        matrix(i,j) = elem;	// require (i,j) access element modification
    }

private:
	size_t 		size, i, j;
	elem_type	elem;
	T			matrix;

	typedef tscbpt::MatrixShape<T>	shape;
	const static bool 		square 		= shape::isSquare;
	const static bool 		hermitian 	= shape::isHermitian;
};

}// End concept namespace

/**
 * Matrix_Base: generic base class for matrices classes.
 * Encapsulates the most common & basic needed operations for matrices.
 * Contains basic arithmetic operations, (row, col) accessor and
 * getRows() and getCols().
 */
template<typename MatrixImpl, typename Field>
struct Matrix_Base{

	typedef MatrixImpl 						matrix_type;
	typedef tscbpt::MatrixShape<MatrixImpl>	shape;
	typedef Field							elem_type;

	size_t getRows() const {
		return static_cast<const MatrixImpl*>(this)->getRows();
	}

	size_t getCols() const {
		return static_cast<const MatrixImpl*>(this)->getCols();
	}

	elem_type operator()(size_t row, size_t col) const {
		return static_cast<const MatrixImpl*>(this)->operator()(row,col);
	}

	elem_type& operator()(size_t row, size_t col) {
		return static_cast<MatrixImpl*>(this)->operator()(row,col);
	}

	template <typename func>
	void for_each_elem(func _functor) {
		if (shape::isHermitian) {
			for (size_t i = 0; i < getRows(); ++i) {
				for (size_t j = i; j < getCols(); ++j) {
					_functor((static_cast<MatrixImpl*> (this))->operator()(i, j));
				}
			}
		} else {
			for (size_t i = 0; i < getRows(); ++i) {
				for (size_t j = 0; j < getCols(); ++j) {
					_functor((static_cast<MatrixImpl*> (this))->operator()(i, j));
				}
			}
		}
	}

	template <typename func>
	void for_each_elem(func _functor, const matrix_type& b) {
		if (shape::isHermitian) {
			for (size_t i = 0; i < getRows(); ++i) {
				for (size_t j = i; j < getCols(); ++j) {
					_functor((static_cast<MatrixImpl*> (this))->operator()(i, j), (static_cast<MatrixImpl> (b))(i, j));
				}
			}
		} else {
			for (size_t i = 0; i < getRows(); ++i) {
				for (size_t j = 0; j < getCols(); ++j) {
					_functor((static_cast<MatrixImpl*> (this))->operator()(i, j), (static_cast<MatrixImpl> (b))(i, j));
				}
			}
		}
	}

private:
	template <typename T, typename op>
	struct op_constant
	{
		op_constant(T b) : _b(b) {}

		template <typename U>
		void operator()(U& u) const {
			op(u,_b);
		}
	private:
		T _b;
	};

	template <typename op>
	struct op_binary
	{
		template <typename A, typename B>
		void operator()(A& a, const B& b) const {
			op(a, b);
		}
	};

	struct assign{
		template <typename A, typename B>
		assign(A& a, const B& b) {
			a = b;
		}
	};

	struct add_assign{
		template<typename A, typename B>
		add_assign(A& a, const B& b){
			a += b;
		}
	};

	struct minus_assign{
		template<typename A, typename B>
		minus_assign(A& a, const B& b){
			a -= b;
		}
	};

	struct mul_assign{
		template <typename A, typename B>
		mul_assign(A& a, const B& b) {
			a *= b;
		}
	};

	struct div_assign{
		template <typename A, typename B>
		div_assign(A& a, const B& b) {
			a /= b;
		}
	};

public:

	matrix_type& operator=(const elem_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_constant<elem_type, assign >(b));
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator=(const matrix_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_binary<assign>(), b);
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator+=(const elem_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_constant<elem_type, add_assign >(b));
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator+=(const matrix_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_binary<add_assign> (), b);
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator-=(const elem_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_constant<elem_type, minus_assign> (b));
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator-=(const matrix_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_binary<minus_assign> (), b);
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator*=(const elem_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_constant<elem_type, mul_assign >(b));
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator*=(const matrix_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_binary<mul_assign> (), b);
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type& operator/=(const elem_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_constant<elem_type, div_assign >(b));
		return *static_cast<MatrixImpl*>(this);
	}

	matrix_type& operator/=(const matrix_type& b) {
		static_cast<MatrixImpl*>(this)->for_each_elem(op_binary<div_assign> (), b);
		return *static_cast<MatrixImpl*> (this);
	}

	matrix_type operator *(const elem_type& b) const{
		matrix_type tmp(*static_cast<const matrix_type*> (this));
		return (tmp *= b);
	}

	matrix_type operator /(const elem_type& b) const{
		matrix_type tmp(*static_cast<const matrix_type*> (this));
		return (tmp /= b);
	}

	matrix_type operator +(const elem_type& b) const {
		matrix_type tmp(*static_cast<const matrix_type*> (this));
		return (tmp += b);
	}

	matrix_type operator +(const matrix_type& b) const {
		matrix_type tmp(*static_cast<const matrix_type*> (this));
		return (tmp += b);
	}

	matrix_type operator -(const elem_type& b) const {
		matrix_type tmp(*static_cast<const matrix_type*> (this));
		return (tmp -= b);
	}

	matrix_type operator -(const matrix_type& b) const {
		matrix_type tmp(*static_cast<const matrix_type*> (this));
		return (tmp -= b);
	}
};

}// End tscbpt namespace

#endif /* MATRIX_HPP_ */
