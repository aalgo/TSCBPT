/*
 * ProgressDisplay.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 30/08/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef PROGRESSDISPLAY_HPP_
#define PROGRESSDISPLAY_HPP_


#include <string>
#include <iostream>
#include <cstdio>
#include <boost/timer.hpp>
#include <tsc/util/types/TypeTraits.hpp>
#include "Timer.hpp"


namespace tscbpt {

using std::string;
using tscbpt::BasicTraits;

template <
	typename T 			= 	size_t,
	std::ostream& out		= 	std::cout,
	class Timer			=	boost::timer
>
class GenericProgressDisplay{
public:
	typedef typename BasicTraits<T>::param_type 			T_param;
	typedef typename BasicTraits<T>::const_param_type 	const_T_param;

private:
	T _expected_count, _count;
	Timer _timer;
	string _prev, _line, _end;
	double _last_ET;
	static const double update_time;
	void display(){
		if (_timer.elapsed() - _last_ET > update_time) {
			double percent = static_cast<double> (_count) * 100.0 / static_cast<double> (_expected_count);
			char message[256];
			sprintf(message, "%s  [%4.1f%%]  ETA: ", _line.c_str(), percent);
			out << "\r" << message;
			if(percent > 0){
				display_time(_timer.elapsed() * (100. /percent - 1.));
				out << "  ";
			}else{
				out << "---";
			}
			_last_ET = _timer.elapsed();
			out.flush();
		}
	}

	void display_time(double seconds){
		if (seconds > 60) {
			int hours = static_cast<int> (seconds / 3600.0);
			char message[64];
			sprintf(message, "%02d:", hours);
			out << message;
			seconds -= hours * 3600.0;

			int mins = static_cast<int> (seconds / 60.0);
			sprintf(message, "%02d:", mins);
			out << message;
			seconds -= mins * 60.0;

			sprintf(message, "%02.0f", seconds);
			out << message;
		} else {
			char message[64];
			sprintf(message, "%4.1f s", seconds);
			out << message;
		}
	}

public:
	GenericProgressDisplay(const_T_param expected_count,
		const string& prev = "\n",
		const string& line = "",
		const string& end = "Done ")
	:_expected_count(expected_count), _count(0), _timer(), _prev(prev), _line(line), _end(end), _last_ET(0.0){
		out << _prev;
		out.flush();
	}

	GenericProgressDisplay(const_T_param expected_count, const_T_param initial_count, const string& prev = "\n",
		const string& line = "", const string& end = "") :
		_expected_count(expected_count), _count(initial_count), _timer(), _prev(prev), _line(line), _end(end), _last_ET(0.0) {
		out << _prev;
		out.flush();
	}

	~GenericProgressDisplay(){
		char message[256];
		sprintf(message, "%8.3f s", _timer.elapsed());
		out << "\r                                                                      \r" << _end << message << endl;
	}

	const_T_param getExpected_count() const {
		return _expected_count;
	}

	void setExpected_count(const_T_param expected_count) {
		this->_expected_count = expected_count;
	}

	const_T_param getCurrent_count() const {
		return _count;
	}

	void setCurrent_count(const_T_param count) {
		this->_count = count;
		display();
	}

	const_T_param operator+=(const_T_param increment) {
		//  Effects: Display appropriate progress tic if needed.
		//  Postconditions: count()== original count() + increment
		//  Returns: count().
		_count += increment;
		display();
		return _count;
	}

	const_T_param operator++(){
		return operator+=( 1 );
	}
};

template <typename T,	std::ostream& out, class Timer>
const double GenericProgressDisplay<T, out, Timer>::update_time = 0.53;

typedef GenericProgressDisplay<>			PortableProgressDisplay;
typedef GenericProgressDisplay<size_t, std::cout, tscbpt::Timer>			PreciseProgressDisplay;

typedef PreciseProgressDisplay				ProgressDisplay;

}
#endif /* PROGRESSDISPLAY_HPP_ */
