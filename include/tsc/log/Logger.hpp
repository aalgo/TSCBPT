/*
 * Logger.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 01/04/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <climits>
#include <string>
#include <iostream>
#include <ctime>

namespace tscbpt
{

using std::endl;
using std::clog;

struct LogLevel{
	static const long OFF = 0;
	static const long FATAL = 10;
	static const long ERROR = 100;
	static const long WARN = 1000;
	static const long INFO = 10000;
	static const long DEBUG = 20000;
	static const long TRACE = 100000;
	static const long ALL = LONG_MAX;
};

template<long N>
struct LogName{
	static const char* name;
};
//template<> struct LogName<LogLevel::ERROR>{static const char* name; };

template<long N>
const char* LogName<N>::name = "";

template<>
const char* LogName<LogLevel::FATAL>::name = "FATAL";

template<>
const char* LogName<LogLevel::ERROR>::name = "ERROR";

template<>
const char* LogName<LogLevel::WARN>::name = "WARNING";

template<>
const char* LogName<LogLevel::INFO>::name = "INFO";

template<>
const char* LogName<LogLevel::DEBUG>::name = "DEBUG";

template<>
const char* LogName<LogLevel::TRACE>::name = "TRACE";

struct DefaultMessagingPolicy{
	DefaultMessagingPolicy(): begin(clock()) {}
	template<typename T>
	inline void operator()(long, const T value) const {
		printTime() << value << endl;
	}

	template <typename T, typename LogName>
	inline void operator()( const T value) const {
		printTime() << LogName::name << ": " << value << endl;
	}
private:
	clock_t begin;

	double diffclock(clock_t clock1, clock_t clock2) const
	{
		double diffticks=clock1-clock2;
		double diffms=(diffticks*1000)/CLOCKS_PER_SEC;
		return diffms;
	}

	std::ostream& printTime() const {
		return clog << diffclock(clock(), begin) << ": ";
	}
};

template<
	long LoggingLevel		= LogLevel::WARN,
	class MessagingPolicy	= DefaultMessagingPolicy
>
class Logger
{
public:

	//================================== TESTS ======================================
	/**
	 * Tests are useful to manage checking code inside conditionals. E.g.:
	 * if(logger.warnTest(var.checkState()) { log.warn("...") }
	 * The code will be removed to avoid the conditional overhead depending on level
	 * since condition always false or it will be executed as a normal conditional.
	 *
	 * Different levels are provided to differentiate the severity of the tests.
	 */
	template <typename T>
	inline bool traceLogTest(T arg) const {
		return LoggingLevel >= LogLevel::TRACE && arg;
	}

	template <typename T>
	inline bool debugLogTest(T arg) const {
		return LoggingLevel >= LogLevel::DEBUG && arg;
	}

	template <typename T>
	inline bool infoLogTest(T arg) const {
		return LoggingLevel >= LogLevel::INFO && arg;
	}

	template <typename T>
	inline bool warnLogTest(T arg) const {
		return LoggingLevel >= LogLevel::WARN && arg;
	}

	template <typename T>
	inline bool errorLogTest(T arg) const {
		return LoggingLevel >= LogLevel::ERROR && arg;
	}

	template <typename T>
	inline bool fatalLogTest(T arg) const {
		return LoggingLevel >= LogLevel::FATAL && arg;
	}

	//================================= LOGGING =====================================
	template <typename T>
	inline static void fatalLog(const T mess) {
		if (LoggingLevel >= LogLevel::FATAL) Message(LogLevel::FATAL, mess);
	}

	template <typename T>
	inline static void errorLog(const T mess) {
		if (LoggingLevel >= LogLevel::ERROR) Message(LogLevel::ERROR, mess);
	}

	template <typename T>
	inline static void warnLog(const T mess) {
		if (LoggingLevel >= LogLevel::WARN) Message(LogLevel::WARN, mess);
	}

	template <typename T>
	inline static void infoLog(const T mess) {
		if (LoggingLevel >= LogLevel::INFO) Message(LogLevel::INFO, mess);
	}

	template <typename T>
	inline static void debugLog(const T mess) {
		if (LoggingLevel >= LogLevel::DEBUG) Message(LogLevel::DEBUG, mess);
	}

	template <typename T>
	inline static void traceLog(const T mess) {
		if (LoggingLevel >= LogLevel::TRACE) Message(LogLevel::TRACE, mess);
	}

	template <typename T>
	inline static void log(long loggingLevel, const T mess) {
		if (LoggingLevel >= loggingLevel) Message(loggingLevel, mess);
	}

private:
	static MessagingPolicy		Message;
};

template<
	long LoggingLevel,
	class MessagingPolicy
>
MessagingPolicy Logger<LoggingLevel, MessagingPolicy>::Message = MessagingPolicy();
}

#endif /* LOGGER_HPP_ */
