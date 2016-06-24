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

}

plotter::~plotter() {

}

void plotter::nplot(vector<int> obj, string title)
{
	top["method"] = "nplot";

	for (int i = 0; i < obj.size(); i ++)
	{
		top["arg0"][i] = obj[i];
	}//Adds each element in vector to dictionary arg0

	top["arg1"] = title; //title of graph
    std::string message = writer.write(top);
    cout << message << endl; //Prints json to be send

    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_PUB);
    socket.connect ("tcp://localhost:5555"); //Port number
    usleep(1000000.0/4.0);
    zmq::message_t request (message.size()); //Size of message
    memcpy (request.data (), (message.c_str()), (message.size())); //Copies data into request
    socket.send (request); //Sends off data
//    socket.send(message.c_str());

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


