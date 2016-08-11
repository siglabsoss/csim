/*
 * plotter.h
 *
 *  Created on: Jun 23, 2016
 *      Author: ubuntu
 */
#include <3rd/zmq.hpp>
#include <string>
#include <complex>
#include <iostream>

#include <3rd/json/json.h>
#include <types/fixedcomplex.hpp>
#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>

#ifndef PLOTTER_H_
#define PLOTTER_H_

using namespace std;

class plotter
{
public:
    zmq::context_t *context;
    zmq::socket_t *socket;

    plotter()
    {
        this->context = new zmq::context_t(1);
        this->socket = new zmq::socket_t(*this->context, ZMQ_PUB);
        socket->connect("tcp://localhost:5556"); //Port number
        usleep(1000000.0 / 4.0);
    }

    void send(const Json::Value &jsn) const;

    template<typename T> void nplot(const CircularBuffer<T> &obj, const string &title) const
    {
        Json::Value jsn;
        jsn["method"] = "nplot";
        conv_real_int(obj, jsn); //Converts CircularBuffer into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    } //Plots data normally. nplot(rt)

    template<typename T> void nplot(const CircularBuffer<complex<T> > obj, const string &title) const
    {
    	Json::Value jsn;
    	jsn["method"] = "nplot";
    	conv_real_int(obj, jsn); //Converts CircularBuffer into json
    	jsn["arg1"] = title; //title of graph
    	send(jsn);
    }

    template<typename T> void nplotfft(const CircularBuffer<T> &obj, const string &title) const
    {
        Json::Value jsn;
        jsn["method"] = "nplotfft";
        conv_real_int(obj, jsn); //Converts CircularBuffer into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    }
    ; //Plots fft of data. plt.plot(abs(fftshift(fft(rf))))

    template<typename T> void nplotqam(const CircularBuffer<T> &obj, string title) const
    {
        Json::Value jsn;
        jsn["method"] = "nplotqam";
        conv_real_int(obj, jsn); //Converts CircularBuffer into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    }
    ; //Plots nplotqam of data. plt.plot(r, i, '.b', alpha=0.6)

    template<typename T> void conv_real_int(const CircularBuffer<T> &obj, Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++)
            t1["arg0"]["r"][i] = obj[i]; //Adds each element in CircularBuffer to dictionary arg0
    }
    ;

    void conv_real_int(const CircularBuffer<filter_io_t > &obj, Json::Value& t1) const
    {
        for (int i = 0; i < obj.size(); i++) {
            double real = 0;
            double imag = 0;
            switch(obj[i].type) {
                case IO_TYPE_NULL:
                    break;
                case IO_TYPE_COMPLEX_DOUBLE:
                    real = obj[i].rf.real();
                    imag = obj[i].rf.imag();
                    break;
                case IO_TYPE_FIXED_COMPLEX_16:
                    real = obj[i].fc.real / 32768.0;
                    imag = obj[i].fc.imag / 32768.0;
                    break;
                case IO_TYPE_FIXED_COMPLEX_32:
                    real = obj[i].fc32.real / 32768.0;
                    imag = obj[i].fc32.imag / 32768.0;
                    break;
                case IO_TYPE_FIXED_COMPLEX_16_NEW:
                    real = obj[i].fcn.real().to_double();
                    imag = obj[i].fcn.imag().to_double();
                    break;
                case IO_TYPE_FIXED_COMPLEX_32_NEW:
                    real = obj[i].fcn32.real().to_double();
                    imag = obj[i].fcn32.imag().to_double();
                    break;
                case IO_TYPE_BYTE:
                    break;
            }
            //std::cout << "(" << real << "," << imag << ")" << std::endl;
            t1["arg0"]["r"][i] = real;
            t1["arg0"]["i"][i] = imag;
        }
    }

    template<int B> void conv_real_int(const CircularBuffer<sc_int<B> > &obj, Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++) {
            t1["arg0"]["r"][i] = obj[i].to_int();
        } //Adds each element in CircularBuffer to dictionary arg0
    }
    ;

    template<int B> void conv_real_int(const CircularBuffer<FixedComplex<B> > &obj,
            Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++) {
            //	t1["arg0"][i] = obj[i].real.to_int();
            //cout << obj[i].real.to_int() << endl;
            t1["arg0"]["r"][i] = obj[i].real.to_int();
            t1["arg0"]["i"][i] = obj[i].imag.to_int();
        }    		//Adds each element in CircularBuffer to dictionary arg0
    }
    ;

    void conv_real_int(const CircularBuffer<complex<double> > &obj, Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++) {
            t1["arg0"]["r"][i] = std::real(obj[i]);
            t1["arg0"]["i"][i] = std::imag(obj[i]);//obj[i].imag.to_int();
        }    		//Adds each element in CircularBuffer to dictionary arg0
    }
    ;

};

#endif /* PLOTTER_H_ */
