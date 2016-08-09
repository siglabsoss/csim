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

#include <3rd/json/json.h>
#include <types/fixedcomplex.hpp>

#ifndef PLOTTER_H_
#define PLOTTER_H_

using namespace std;

class plotter
{
public:
    zmq::context_t *context;
    zmq::socket_t *socket;
    Json::FastWriter writer;

    plotter()
    {
        this->context = new zmq::context_t(1);
        this->socket = new zmq::socket_t(*this->context, ZMQ_PUB);
        socket->connect("tcp://localhost:5555"); //Port number
        usleep(1000000.0 / 4.0);
    }

    void send(Json::Value jsn);

    template<typename T> void nplot(vector<T> obj, string title)
    {
        Json::Value jsn;
        jsn["method"] = "nplot";
        conv_real_int(obj, jsn); //Converts vector into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    } //Plots data normally. nplot(rt)

    template<typename T> void nplotfft(vector<T> obj, string title)
    {
        Json::Value jsn;
        jsn["method"] = "nplotfft";
        conv_real_int(obj, jsn); //Converts vector into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    }
    ; //Plots fft of data. plt.plot(abs(fftshift(fft(rf))))

    template<typename T> void nplotqam(vector<T> obj, string title)
    {
        Json::Value jsn;
        jsn["method"] = "nplotqam";
        conv_real_int(obj, jsn); //Converts vector into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    }
    ; //Plots nplotqam of data. plt.plot(r, i, '.b', alpha=0.6)

    template<typename T> void conv_real_int(vector<T> obj, Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++)
            t1["arg0"][i] = obj[i]; //Adds each element in vector to dictionary arg0
    }
    ;

    template<int B> void conv_real_int(vector<sc_int<B> > obj, Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++) {
            t1["arg0"][i] = obj[i].to_int();
        } //Adds each element in vector to dictionary arg0
    }
    ;

    template<int B> void conv_real_int(vector<FixedComplex<B> > obj,
            Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++) {
            //	t1["arg0"][i] = obj[i].real.to_int();
            //cout << obj[i].real.to_int() << endl;
            t1["arg0"]["r"][i] = obj[i].real.to_int();
            t1["arg0"]["i"][i] = obj[i].imag.to_int();
        }    		//Adds each element in vector to dictionary arg0
    }
    ;
};

#endif /* PLOTTER_H_ */
