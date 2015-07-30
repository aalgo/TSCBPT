/*
 * Storage.hpp
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

#ifndef STORAGE_HPP_
#define STORAGE_HPP_

#include "PtrStorage.hpp"

namespace tscbpt
{

/**
 * Removal policy employed for raw pointers
 */
struct DeleteRemovalPolicy
{
	template <typename T>
	inline static void remove(T& p) {
		delete (p);
	}
};

/**
 * Removal policy that can be employed for Smart pointers
 */
struct NoRemovalPolicy
{
	template <typename T>
	inline static void remove(T& p) {
	}
};

template <
	typename T,
	template <class > class PtrStoragePolicy = DefaultPtrStorage,
	class RemovalPolicy = DeleteRemovalPolicy
	>
struct NativeStorage: public PtrStoragePolicy<T>
{

	typedef T													valueType;
	typedef T													value_type;
	typedef typename PtrStoragePolicy<T>::pointerType 			pointerType;
	typedef typename PtrStoragePolicy<T>::pointerType 			pointer_type;
	typedef typename PtrStoragePolicy<T>::strongPointerType 	StrongPointerType;
	typedef typename PtrStoragePolicy<T>::weakPointerType 		WeakPointerType;

	template <typename Type>
	struct Policy: public NativeStorage<Type, PtrStoragePolicy, DeleteRemovalPolicy>
	{
	protected:
		~Policy() {
		}
	};

	static long balance;

	static StrongPointerType create() {
		balance++;
		return StrongPointerType(new T);
	}

	template <typename T1>
	static StrongPointerType create(T1 t1) {
		balance++;
		return StrongPointerType(new T(t1));
	}

	template <typename T1, typename T2>
	static StrongPointerType create(T1 t1, T2 t2) {
		balance++;
		return StrongPointerType(new T(t1, t2));
	}

	template <typename T1, typename T2, typename T3>
	static StrongPointerType create(T1 t1, T2 t2, T3 t3) {
		balance++;
		return StrongPointerType(new T(t1, t2, t3));
	}

	template <typename T1, typename T2, typename T3, typename T4>
	static StrongPointerType create(T1 t1, T2 t2, T3 t3, T4 t4) {
		balance++;
		return StrongPointerType(new T(t1, t2, t3, t4));
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5>
	static StrongPointerType create(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5) {
		balance++;
		return StrongPointerType(new T(t1, t2, t3, t4, t5));
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
	static StrongPointerType create(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6) {
		balance++;
		return StrongPointerType(new T(t1, t2, t3, t4, t5, t6));
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
	static StrongPointerType create(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7) {
		balance++;
		return StrongPointerType(new T(t1, t2, t3, t4, t5, t6, t7));
	}

	template <typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
	static StrongPointerType create(T1 t1, T2 t2, T3 t3, T4 t4, T5 t5, T6 t6, T7 t7, T8 t8) {
		balance++;
		return StrongPointerType(new T(t1, t2, t3, t4, t5, t6, t7, t8));
	}

	inline static void remove(StrongPointerType& p) {
		balance--;
		RemovalPolicy::remove(p);
	}

protected:
	~NativeStorage() {
	}
};
template <
	typename T,
	template <class > class PtrStoragePolicy,
	class RemovalPolicy
	>
long NativeStorage<T, PtrStoragePolicy, RemovalPolicy>::balance = 0;


template <typename T>
struct DefaultNativeStoragePolicy: public NativeStorage<T>
{
protected:
	~DefaultNativeStoragePolicy() {
	}
};

}

#endif /* STORAGE_HPP_ */
