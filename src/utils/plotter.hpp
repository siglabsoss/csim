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
#include <cassert>

#include <3rd/json/json.h>
#include <types/fixedcomplex.hpp>
#include <types/circularbuffer.hpp>
#include <core/filter_chain_element.hpp>

#ifndef PLOTTER_H_
#define PLOTTER_H_

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

        // should flush all packets immediatly
        int val;
        val = 0; // set linger to 0
        this->socket->setsockopt(ZMQ_LINGER, &val, sizeof(val));

        socket->connect("tcp://localhost:5556"); //Port number
        std::cout << "Plotter Connecting" << std::endl;
        usleep(1000000.0 / 4.0);
        hello();
    }


    plotter(const plotter &other) = delete;

    void hello();

public:

    static const plotter & get(); //singleton


    void send(const Json::Value &jsn) const;



//    static CircularBuffer<int> cb(const std::vector<int> &ivector)
//    {
//
//        CircularBuffer<int> conv(ivector.size());
//        for(unsigned i = 0; i < ivector.size(); i++)
//        {
//            conv.push_back(ivector[i]);
//        }
//
//        return conv;
//    }


    void nplotber(const std::vector<std::vector<double> > &bers, const std::vector<std::vector<double> > &ebnos, const std::vector<std::string> &titles, const std::string title = std::string("")) const
    {
        Json::Value jsn;
        jsn["method"] = "nplotber";

        assert(bers.size() == ebnos.size());
        assert(bers.size() == titles.size());

        unsigned sz = bers.size();

        for(unsigned i = 0; i < sz; i++)
        {

            assert(bers[i].size() == ebnos[i].size());
            unsigned sz2 = bers[i].size();

            for(unsigned j = 0; j < sz2; j++)
            {
                jsn["arg0"][i][j] = bers[i][j];
                jsn["arg1"][i][j] = ebnos[i][j];
            }
            jsn["arg2"][i] = titles[i];

        }

        jsn["arg3"] = title;

        send(jsn);
    }


    // Normal 2D plot, may plot imag() only if data is complex
    template<typename T> void nplot(const T &obj, const std::string &title) const
    {
        Json::Value jsn;
        jsn["method"] = "nplot";
        conv_real_int(obj, jsn); //Converts CircularBuffer into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    }

    // Plots the FFT of the data.  the FFT is performed by Python's using numpy's floating point FFT
    template<typename T> void nplotfft(const T &obj, const std::string &title) const
    {
        Json::Value jsn;
        jsn["method"] = "nplotfft";
        conv_real_int(obj, jsn); //Converts CircularBuffer into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    }

    // Constellation plot with transparent blobs (good for QAM)
    template<typename T> void nplotqam(const T &obj, std::string title) const
    {
        Json::Value jsn;
        jsn["method"] = "nplotqam";
        conv_real_int(obj, jsn); //Converts CircularBuffer into json
        jsn["arg1"] = title; //title of graph
        send(jsn);
    }

    template<typename T> void conv_real_int(const CircularBuffer<T> &obj, Json::Value& t1) const
    {
        for (unsigned i = 0; i < obj.size(); i++)
        {
            t1["arg0"]["r"][i] = obj[i]; //Adds each element in CircularBuffer to dictionary arg0
        }
    }

    template<typename T> void conv_real_int(const std::vector<T> &obj, Json::Value& t1) const
    {
        for (unsigned i = 0; i < obj.size(); i++)
        {
            t1["arg0"]["r"][i] = obj[i]; //Adds each element in CircularBuffer to dictionary arg0
        }
    }

    void conv_real_int(const CircularBuffer<filter_io_t > &obj, Json::Value& t1) const
    {
        for (unsigned int i = 0; i < obj.size(); i++) {
            ComplexDouble val = obj[i].toComplexDouble();
            //std::cout << "(" << real << "," << imag << ")" << std::endl;
            t1["arg0"]["r"][i] = val.real();
            t1["arg0"]["i"][i] = val.imag();
        }
    }

    void conv_real_int(const CircularBuffer<int> &obj, Json::Value& t1) const
    {
        for (unsigned i = 0; i < obj.size(); i++) {
            t1["arg0"]["r"][i] = obj[i];
        } //Adds each element in CircularBuffer to dictionary arg0
    }

    void conv_real_int(const CircularBuffer<std::complex<double> > &obj, Json::Value& t1) const
    {
        for (unsigned int i = 0; i < obj.size(); i++) {
            t1["arg0"]["r"][i] = std::real(obj[i]);
            t1["arg0"]["i"][i] = std::imag(obj[i]);
        }    		//Adds each element in CircularBuffer to dictionary arg0
    }

    void conv_real_int(const std::vector<std::complex<double> > &obj, Json::Value& t1) const
    {
        for (unsigned int i = 0; i < obj.size(); i++) {
            t1["arg0"]["r"][i] = std::real(obj[i]);
            t1["arg0"]["i"][i] = std::imag(obj[i]);
        }           //Adds each element in CircularBuffer to dictionary arg0
    }

};

#endif /* PLOTTER_H_ */
