/*
 * plotter.h
 *
 *  Created on: Jun 23, 2016
 *      Author: ubuntu
 */
#include <zmq.hpp>
#include <string>
#include <vector>
#include <complex>
#include <iostream>

#include "json.h"

#ifndef PLOTTER_H_
#define PLOTTER_H_
using namespace std;

class plotter {
public:
	plotter();
	zmq::context_t *context;
	zmq::socket_t *socket;

	Json::FastWriter writer;
    Json::Value top;  // top level object
    Json::Value arg0;

	void nplot(vector<int> obj, string title);
	void nplotfft(int obj, string title);
	void nplotqam(vector<complex<double> >, string title);
	void conv_real_int(vector<int> obj, Json::Value& t1);
	~plotter();
};

#endif /* PLOTTER_H_ */

