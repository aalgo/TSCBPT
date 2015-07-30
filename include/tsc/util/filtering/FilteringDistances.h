/*
 * FilteringDistances.h
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 24/10/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef FILTERINGDISTANCES_H_
#define FILTERINGDISTANCES_H_

#include <armadillo>
#include <functional>
#include <complex>
#include <cmath>

/**
 * This file contains the distances employed for the Distance Based
 * Bilateral filtering, as described in [1].
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
namespace tscbpt {

/**
 * Diagonal geodesic exponential dissimilarity, as described in [1].
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<typename T>
struct BLFGeodesicDissExp:
	public std::binary_function<T, T, double >
	{

	typedef T ModelType;

	BLFGeodesicDissExp(double sigma_t = 0.1): _sigma_t(sigma_t){}

	double operator()(const ModelType a, const ModelType b) const {
		double res = 0;
		for (size_t i = 0; i < a.getRows(); ++i) {
			res += pow( log( (std::abs(a(i,i))+_sigma_t) / (std::abs(b(i,i))+_sigma_t) ), 2);
		}
		return exp(sqrt(res)) - 1;
	}
	private:
	double _sigma_t;
};

/**
 * Specialization needed for arma::Mat since it does not have the getRows()
 * function. Instead the attribute n_rows is employed.
 */
template<typename T>
struct BLFGeodesicDissExp<arma::Mat<T> >:
	public std::binary_function<arma::Mat<T>, arma::Mat<T>, double >
	{

	typedef arma::Mat<T> ModelType;

	BLFGeodesicDissExp(double sigma_t = 0.1): _sigma_t(sigma_t){}

	double operator()(const ModelType a, const ModelType b) const {
		double res = 0;
		for (size_t i = 0; i < a.n_rows; ++i) {
			res += pow(log( (std::abs(a(i,i))+_sigma_t) / (std::abs(b(i,i))+_sigma_t) ), 2);
		}
		return exp(sqrt(res)) - 1;
	}
	private:
	double _sigma_t;
};

/**
 * Diagonal Wishart dissimilarity, as described in [1].
 * [1] Alonso-González, A.; López-Martínez, C.; Salembier, P.; Deng, X.
 * Bilateral Distance Based Filtering for Polarimetric SAR Data.
 * Remote Sens. 2013, 5, 5620-5641.
 */
template<typename T>
struct BLFDiagonalWishartDiss:
	public std::binary_function<T, T, double >
	{
	typedef T		ModelType;

	BLFDiagonalWishartDiss(double sigma_t = 0.0): _offset(sigma_t) {}

	double operator()(const ModelType a, const ModelType b) const {
		double res = 0;
		for (size_t i = 0; i < a.getRows(); ++i) {
			const double min = DRW_MIN_THRESHOLD;
			double tmp = (max(std::abs(a(i, i)), min) + _offset) / (max(std::abs(b(i, i)), min) + _offset);
			res += tmp + 1.0 / tmp;
		}
		return (res - 2.0 * a.getRows()) / (a.getRows());
	}

private:
	double _offset;
	static const double DRW_MIN_THRESHOLD = 1e-12;
};

/**
 * Specialization needed for arma::Mat since it does not have the getRows()
 * function. Instead the attribute n_rows is employed.
 */
template<typename T>
struct BLFDiagonalWishartDiss<arma::Mat<T> >:
	public std::binary_function<arma::Mat<T>, arma::Mat<T>, double>
{
	typedef arma::Mat<T> ModelType;

	BLFDiagonalWishartDiss(double sigma_t = 0.0): _sigma_t(sigma_t) {}

	double operator()(const ModelType a, const ModelType b) const {
		double res = 0;
		for (size_t i = 0; i < a.n_rows; ++i) {
			const double min = DRW_MIN_THRESHOLD;
			double tmp = (max(std::abs(a(i, i)), min) + _sigma_t) / (max(std::abs(b(i, i)), min) + _sigma_t);
			res += tmp + 1.0 / tmp;
		}
		return (res - 2.0 * a.n_rows) / a.n_rows;
	}

private:
	double _sigma_t;
	static const double DRW_MIN_THRESHOLD = 1e-12;
};

}


#endif /* FILTERINGDISTANCES_H_ */
