/*
 * plotter.h
 *
 *  Created on: Jun 23, 2016
 *      Author: ubuntu
 */
#include <zmq.hpp>
#include <string>
#include<vector>
#include <../inc/jsoncpp.cpp>
#include <complex>
#include <iostream>

#ifndef PLOTTER_H_
#define PLOTTER_H_
using namespace std;

class plotter {
public:
	plotter();

	Json::FastWriter writer;
    Json::Value top;  // top level object
    Json::Value arg0;

	void nplot(vector<int> obj, string title);
	void nplotfft(int obj, string title);
	void nplotqam(vector<complex<double> >, string title);
	~plotter();
};

#endif /* PLOTTER_H_ */

