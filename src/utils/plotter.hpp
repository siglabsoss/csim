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
#include <mutex>

#include <3rd/json/json.h>
#include <types/fixedcomplex.hpp>
#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>

#ifndef PLOTTER_H_
#define PLOTTER_H_

using namespace std;

class plotter
{
private:
    zmq::context_t *context;
    zmq::socket_t *socket;
    static plotter *m_instance;

    //static std::mutex  m_mutex;

    plotter()
    {
        this->context = new zmq::context_t(1);
        this->socket = new zmq::socket_t(*this->context, ZMQ_PUB);
        socket->connect("tcp://localhost:5556"); //Port number
        usleep(1000000.0 / 4.0);
    }

    plotter(const plotter &other) = delete;
public:

    static const plotter & get(); //singleton


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
        for (unsigned int i = 0; i < obj.size(); i++) {
            ComplexDouble val = obj[i].toComplexDouble();
            //std::cout << "(" << real << "," << imag << ")" << std::endl;
            t1["arg0"]["r"][i] = val.real();
            t1["arg0"]["i"][i] = val.imag();
        }
    }

    template<int B> void conv_real_int(const CircularBuffer<sc_int<B> > &obj, Json::Value& t1)
    {
        for (int i = 0; i < obj.size(); i++) {
            t1["arg0"]["r"][i] = obj[i].to_int();
        } //Adds each element in CircularBuffer to dictionary arg0
    }
    ;


    void conv_real_int(const CircularBuffer<complex<double> > &obj, Json::Value& t1)
    {
        for (unsigned int i = 0; i < obj.size(); i++) {
            t1["arg0"]["r"][i] = std::real(obj[i]);
            t1["arg0"]["i"][i] = std::imag(obj[i]);//obj[i].imag.to_int();
        }    		//Adds each element in CircularBuffer to dictionary arg0
    }
    ;

};

#endif /* PLOTTER_H_ */
