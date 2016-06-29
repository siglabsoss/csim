/*
 * plotter.cpp
 *
 *  Created on: Jun 23, 2016
 *      Author: ubuntu
 */

#include "plotter.h"
#include <string>
#include <unistd.h>

using namespace std;
plotter::plotter() {
	this->context = new zmq::context_t(1);
	this->socket = new zmq::socket_t(*this->context, ZMQ_PUB);
	socket->connect ("tcp://localhost:5555"); //Port number
	usleep(1000000.0/4.0);
}

plotter::~plotter() {

}

template<typename T>
void plotter::some(T){
	cout << "other version" << endl;
}


template<>
void plotter::some(char){
	cout << "char version" << endl;
}

template void plotter::some(int);
template void plotter::some(float);
template void plotter::some(char);





void plotter::conv_real_int(vector<int> obj, Json::Value& t1)
{
	for (int i = 0; i < obj.size(); i ++)
	{
		t1["arg0"][i] = obj[i];
	}//Adds each element in vector to dictionary arg0
}

void plotter::nplot(vector<int> obj, string title)
{
//	Json::Value jsn;
//	jsn["method"] = "nplot";
//
//	conv_real_int(obj, jsn);
//
//	jsn["arg1"] = title; //title of graph
//    std::string message = writer.write(jsn);
////    cout << message << endl; //Prints json to be send
//
//    zmq::message_t request (message.size()); //Size of message
//    memcpy (request.data (), (message.c_str()), (message.size())); //Copies data into request
//    socket->send (request); //Sends off data

}

void plotter::nplotfft(int obj, string title)
{
	top["method"] = "nplotfft";
	top["arg1"] = title;
}

void plotter::nplotqam(vector<complex<double> >, string title)
{
	top["method"] = "nplotqam";
	top["arg1"] = title;
}


