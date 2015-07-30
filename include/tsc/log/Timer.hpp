/*
 * Timer.hpp
 *
 * Copyright (c) 2015 Alberto Alonso Gonzalez
 *
 * This Source Code Form is subject to the terms of the
 * Mozilla Public License, v. 2.0.
 * If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *  Created on: 22/09/2011
 *      Author: Alberto Alonso-Gonzalez
 */

#ifndef TIMER_HPP_
#define TIMER_HPP_

#include <cstdio>
#include <cstdlib>
#include <sys/time.h>


namespace tscbpt
{

using namespace std;

class Timer {

public:

	Timer(){
		start();
	}

    void start(){
        gettimeofday(&startTime, NULL);
    }

    double elapsed(){
        timeval endTime;
        long seconds, nseconds;
        double duration;

        gettimeofday(&endTime, NULL);

        seconds  = endTime.tv_sec  - startTime.tv_sec;
        nseconds = endTime.tv_usec - startTime.tv_usec;

        duration = seconds + nseconds/1000000.0;

        return duration;
    }

    static void printTime(double duration){
        printf("%5.6f seconds\n", duration);
    }

private:
    timeval startTime;

};

}

#endif /* TIMER_HPP_ */
